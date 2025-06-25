// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifdef USE_SYSTEM_OPEN62541
#include <open62541/client_config_default.h>
#include <open62541/plugin/securitypolicy_default.h>
#include <open62541/plugin/pki_default.h>
#endif

#include "qopen62541backend.h"
#include "qopen62541node.h"
#include "qopen62541utils.h"
#include "qopen62541valueconverter.h"
#include <private/qopcuaclient_p.h>
#include <private/qopcuasecuritypolicyuris_p.h>

#include "qopcuaauthenticationinformation.h"
#include <qopcuaerrorstate.h>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/qloggingcategory.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qurl.h>
#include <QtCore/private/qnumeric_p.h> // for qt_saturate

#include <algorithm>
#include <limits>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_OPEN62541)

Open62541AsyncBackend::Open62541AsyncBackend(QOpen62541Client *parent)
    : QOpcUaBackend()
    , m_uaclient(nullptr)
    , m_clientImpl(parent)
    , m_clientIterateInterval(50)
    , m_asyncRequestTimeout(15000)
    , m_clientIterateTimer(this)
    , m_clientIterateOnDemandTimer(this)
    , m_minPublishingInterval(0)
{
    QObject::connect(&m_clientIterateTimer, &QTimer::timeout,
                     this, &Open62541AsyncBackend::iterateClient);

    m_clientIterateOnDemandTimer.setSingleShot(true);
    QObject::connect(&m_clientIterateOnDemandTimer, &QTimer::timeout,
                     this, &Open62541AsyncBackend::iterateClient);

    QObject::connect(parent, &QOpcUaClientImpl::connectionSettingsChanged,
                     this, &Open62541AsyncBackend::handleConnectionSettingsChanged);
}

Open62541AsyncBackend::~Open62541AsyncBackend()
{
    cleanupSubscriptions();
    if (m_uaclient)
        UA_Client_delete(m_uaclient);
}

void Open62541AsyncBackend::readAttributes(quint64 handle, UA_NodeId id, QOpcUa::NodeAttributes attr, QString indexRange)
{
    UaDeleter<UA_NodeId> nodeIdDeleter(&id, UA_NodeId_clear);

    if (!m_uaclient) {
        QList<QOpcUaReadResult> resultMetadata;

        qt_forEachAttribute(attr, [&](QOpcUa::NodeAttribute attribute){
            QOpcUaReadResult temp;
            temp.setAttribute(attribute);
            resultMetadata.push_back(temp);
        });
        emit attributesRead(handle, resultMetadata, QOpcUa::UaStatusCode::BadDisconnect);
        return;
    }

    UA_ReadRequest req;
    UA_ReadRequest_init(&req);
    req.requestHeader.timeoutHint = m_asyncRequestTimeout;
    UaDeleter<UA_ReadRequest> requestDeleter(&req, UA_ReadRequest_clear);
    req.timestampsToReturn = UA_TIMESTAMPSTORETURN_BOTH;

    qt_forEachAttribute(attr, [&req](QOpcUa::NodeAttribute attr) {
       Q_UNUSED(attr);
        ++req.nodesToReadSize;
    });

    if (req.nodesToReadSize)
        req.nodesToRead = static_cast<UA_ReadValueId *>(UA_Array_new(req.nodesToReadSize, &UA_TYPES[UA_TYPES_READVALUEID]));

    QList<QOpcUaReadResult> resultMetadata;
    size_t index = 0;
    qt_forEachAttribute(attr, [&](QOpcUa::NodeAttribute attribute){
        auto &current = req.nodesToRead[index++];

        current.attributeId = QOpen62541ValueConverter::toUaAttributeId(attribute);
        UA_NodeId_copy(&id, &current.nodeId);
        if (indexRange.size())
            QOpen62541ValueConverter::scalarFromQt<UA_String, QString>(indexRange, &current.indexRange);

        QOpcUaReadResult temp;
        temp.setAttribute(attribute);
        resultMetadata.push_back(temp);
    });

    quint32 requestId = 0;
    UA_StatusCode result = __UA_Client_AsyncService(m_uaclient, &req, &UA_TYPES[UA_TYPES_READREQUEST],
                                                    &asyncReadCallback, &UA_TYPES[UA_TYPES_READRESPONSE], this,
                                                    &requestId);

    if (result != UA_STATUSCODE_GOOD) {
        const auto statusCode = static_cast<QOpcUa::UaStatusCode>(result);
        for (auto &entry : resultMetadata) {
            entry.setStatusCode(statusCode);
        }
        emit attributesRead(handle, resultMetadata, statusCode);
        return;
    }

    m_asyncReadContext[requestId] = { handle, resultMetadata };

    triggerIterateClient();
}

void Open62541AsyncBackend::writeAttribute(quint64 handle, UA_NodeId id, QOpcUa::NodeAttribute attrId, QVariant value, QOpcUa::Types type, QString indexRange)
{
    if (!m_uaclient) {
        UA_NodeId_clear(&id);
        emit attributeWritten(handle, attrId, value, QOpcUa::UaStatusCode::BadDisconnect);
        return;
    }

    if (type == QOpcUa::Types::Undefined && attrId != QOpcUa::NodeAttribute::Value)
        type = attributeIdToTypeId(attrId);

    UA_WriteRequest req;
    UA_WriteRequest_init(&req);
    req.requestHeader.timeoutHint = m_asyncRequestTimeout;
    UaDeleter<UA_WriteRequest> requestDeleter(&req, UA_WriteRequest_clear);
    req.nodesToWriteSize = 1;
    req.nodesToWrite = UA_WriteValue_new();

    UA_WriteValue_init(req.nodesToWrite);
    req.nodesToWrite->attributeId = QOpen62541ValueConverter::toUaAttributeId(attrId);
    req.nodesToWrite->nodeId = id;
    req.nodesToWrite->value.value = QOpen62541ValueConverter::toOpen62541Variant(value, type);
    req.nodesToWrite->value.hasValue = true;
    if (indexRange.size())
        QOpen62541ValueConverter::scalarFromQt<UA_String, QString>(indexRange, &req.nodesToWrite->indexRange);

    quint32 requestId = 0;
    UA_StatusCode result = __UA_Client_AsyncService(m_uaclient, &req, &UA_TYPES[UA_TYPES_WRITEREQUEST],
                                                    &asyncWriteAttributesCallback, &UA_TYPES[UA_TYPES_WRITERESPONSE], this,
                                                    &requestId);

    if (result != UA_STATUSCODE_GOOD) {
        emit attributeWritten(handle, attrId, value, static_cast<QOpcUa::UaStatusCode>(result));
        return;
    }

    m_asyncWriteAttributesContext[requestId] = { handle, {{attrId, value}} };
    triggerIterateClient();
}

void Open62541AsyncBackend::writeAttributes(quint64 handle, UA_NodeId id, QOpcUaNode::AttributeMap toWrite, QOpcUa::Types valueAttributeType)
{
    UaDeleter<UA_NodeId> nodeIdDeleter(&id, UA_NodeId_clear);

    if (!m_uaclient) {
        emit attributeWritten(handle, QOpcUa::NodeAttribute::None, QVariant(), QOpcUa::UaStatusCode::BadDisconnect);
        return;
    }

    if (toWrite.size() == 0) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "No values to be written";
        emit attributeWritten(handle, QOpcUa::NodeAttribute::None, QVariant(), QOpcUa::UaStatusCode::BadNothingToDo);
        return;
    }

    UA_WriteRequest req;
    UA_WriteRequest_init(&req);
    req.requestHeader.timeoutHint = m_asyncRequestTimeout;
    UaDeleter<UA_WriteRequest> requestDeleter(&req, UA_WriteRequest_clear);
    req.nodesToWriteSize = toWrite.size();
    req.nodesToWrite = static_cast<UA_WriteValue *>(UA_Array_new(req.nodesToWriteSize, &UA_TYPES[UA_TYPES_WRITEVALUE]));
    size_t index = 0;
    for (auto it = toWrite.begin(); it != toWrite.end(); ++it, ++index) {
        UA_WriteValue_init(&(req.nodesToWrite[index]));
        req.nodesToWrite[index].attributeId = QOpen62541ValueConverter::toUaAttributeId(it.key());
        UA_NodeId_copy(&id, &(req.nodesToWrite[index].nodeId));
        QOpcUa::Types type = it.key() == QOpcUa::NodeAttribute::Value ? valueAttributeType : attributeIdToTypeId(it.key());
        req.nodesToWrite[index].value.value = QOpen62541ValueConverter::toOpen62541Variant(it.value(), type);
    }

    quint32 requestId = 0;
    UA_StatusCode result = __UA_Client_AsyncService(m_uaclient, &req, &UA_TYPES[UA_TYPES_WRITEREQUEST],
                                                      &asyncWriteAttributesCallback, &UA_TYPES[UA_TYPES_WRITERESPONSE], this,
                                                      &requestId);

    if (result != UA_STATUSCODE_GOOD) {
        for (auto it = toWrite.begin(); it != toWrite.end(); ++it) {
            emit attributeWritten(handle, it.key(), it.value(), static_cast<QOpcUa::UaStatusCode>(result));
        }
        return;
    }

    m_asyncWriteAttributesContext[requestId] = { handle, toWrite };
    triggerIterateClient();
}

void Open62541AsyncBackend::enableMonitoring(quint64 handle, UA_NodeId id, QOpcUa::NodeAttributes attr, const QOpcUaMonitoringParameters &settings)
{
    UaDeleter<UA_NodeId> nodeIdDeleter(&id, UA_NodeId_clear);

    if (!m_uaclient) {
        qt_forEachAttribute(attr, [&](QOpcUa::NodeAttribute attribute){
            QOpcUaMonitoringParameters s;
            s.setStatusCode(QOpcUa::UaStatusCode::BadDisconnect);
            emit monitoringEnableDisable(handle, attribute, true, s);
        });
        return;
    }

    QOpen62541Subscription *usedSubscription = nullptr;

    // Create a new subscription if necessary
    if (settings.subscriptionId()) {
        auto sub = m_subscriptions.find(settings.subscriptionId());
        if (sub == m_subscriptions.end()) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "There is no subscription with id" << settings.subscriptionId();

            qt_forEachAttribute(attr, [&](QOpcUa::NodeAttribute attribute){
                QOpcUaMonitoringParameters s;
                s.setStatusCode(QOpcUa::UaStatusCode::BadSubscriptionIdInvalid);
                emit monitoringEnableDisable(handle, attribute, true, s);
            });
            return;
        }
        usedSubscription = sub.value(); // Ignore interval != subscription.interval
    } else {
        usedSubscription = getSubscription(settings);
    }

    if (!usedSubscription) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not create subscription with interval" << settings.publishingInterval();
        qt_forEachAttribute(attr, [&](QOpcUa::NodeAttribute attribute){
            QOpcUaMonitoringParameters s;
            s.setStatusCode(QOpcUa::UaStatusCode::BadSubscriptionIdInvalid);
            emit monitoringEnableDisable(handle, attribute, true, s);
        });
        return;
    }

    qt_forEachAttribute(attr, [&](QOpcUa::NodeAttribute attribute){
        if (getSubscriptionForItem(handle, attribute)) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Monitored item for" << attribute << "has already been created";
            QOpcUaMonitoringParameters s;
            s.setStatusCode(QOpcUa::UaStatusCode::BadEntryExists);
            emit monitoringEnableDisable(handle, attribute, true, s);
        } else {
            bool success = usedSubscription->addAttributeMonitoredItem(handle, attribute, id, settings);
            if (success)
                m_attributeMapping[handle][attribute] = usedSubscription;
        }
    });

    if (usedSubscription->monitoredItemsCount() == 0)
        removeSubscription(usedSubscription->subscriptionId()); // No items were added
}

void Open62541AsyncBackend::disableMonitoring(quint64 handle, QOpcUa::NodeAttributes attr)
{
    if (!m_uaclient) {
        qt_forEachAttribute(attr, [&](QOpcUa::NodeAttribute attribute){
            QOpcUaMonitoringParameters s;
            s.setStatusCode(QOpcUa::UaStatusCode::BadDisconnect);
            emit monitoringEnableDisable(handle, attribute, false, s);
        });
        return;
    }

    qt_forEachAttribute(attr, [&](QOpcUa::NodeAttribute attribute){
        QOpen62541Subscription *sub = getSubscriptionForItem(handle, attribute);
        if (sub) {
            sub->removeAttributeMonitoredItem(handle, attribute);
            m_attributeMapping[handle].remove(attribute);
            if (sub->monitoredItemsCount() == 0)
                removeSubscription(sub->subscriptionId());
        } else {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "There is no monitored item for this attribute";
            QOpcUaMonitoringParameters s;
            s.setStatusCode(QOpcUa::UaStatusCode::BadMonitoredItemIdInvalid);
            emit monitoringEnableDisable(handle, attribute, false, s);
        }
    });
}

void Open62541AsyncBackend::modifyMonitoring(quint64 handle, QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, QVariant value)
{
    if (!m_uaclient) {
        QOpcUaMonitoringParameters p;
        p.setStatusCode(QOpcUa::UaStatusCode::BadDisconnect);
        emit monitoringStatusChanged(handle, attr, item, p);
        return;
    }

    QOpen62541Subscription *subscription = getSubscriptionForItem(handle, attr);
    if (!subscription) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify" << item << ", the monitored item does not exist";
        QOpcUaMonitoringParameters p;
        p.setStatusCode(QOpcUa::UaStatusCode::BadMonitoredItemIdInvalid);
        emit monitoringStatusChanged(handle, attr, item, p);
        return;
    }

    subscription->modifyMonitoring(handle, attr, item, value);
}

QOpen62541Subscription *Open62541AsyncBackend::getSubscription(const QOpcUaMonitoringParameters &settings)
{
    if (settings.subscriptionType() == QOpcUaMonitoringParameters::SubscriptionType::Shared) {
        // Requesting multiple subscriptions with publishing interval < minimum publishing interval breaks subscription sharing
        double interval = revisePublishingInterval(settings.publishingInterval(), m_minPublishingInterval);
        for (auto entry : std::as_const(m_subscriptions)) {
            if (qFuzzyCompare(entry->interval(), interval) && entry->shared() == QOpcUaMonitoringParameters::SubscriptionType::Shared)
                return entry;
        }
    }

    QOpen62541Subscription *sub = new QOpen62541Subscription(this, settings);
    UA_UInt32 id = sub->createOnServer();
    if (!id) {
        delete sub;
        return nullptr;
    }
    m_subscriptions[id] = sub;
    if (sub->interval() > settings.publishingInterval()) // The publishing interval has been revised by the server.
        m_minPublishingInterval = sub->interval();
    // This must be a queued connection to prevent the slot from being called while the client is inside UA_Client_run_iterate().
    QObject::connect(sub, &QOpen62541Subscription::timeout, this, &Open62541AsyncBackend::handleSubscriptionTimeout, Qt::QueuedConnection);
    return sub;
}

bool Open62541AsyncBackend::removeSubscription(UA_UInt32 subscriptionId)
{
    auto sub = m_subscriptions.find(subscriptionId);
    if (sub != m_subscriptions.end()) {
        sub.value()->removeOnServer();
        delete sub.value();
        m_subscriptions.remove(subscriptionId);
        return true;
    }
    return false;
}

void Open62541AsyncBackend::callMethod(quint64 handle, UA_NodeId objectId, UA_NodeId methodId, QList<QOpcUa::TypedVariant> args)
{
    if (!m_uaclient) {
        emit methodCallFinished(handle, Open62541Utils::nodeIdToQString(methodId), QVariant(), QOpcUa::UaStatusCode::BadDisconnect);
        UA_NodeId_clear(&objectId);
        UA_NodeId_clear(&methodId);
        return;
    }

    UA_Variant *inputArgs = nullptr;

    if (args.size()) {
        inputArgs = static_cast<UA_Variant *>(UA_Array_new(args.size(), &UA_TYPES[UA_TYPES_VARIANT]));
        for (qsizetype i = 0; i < args.size(); ++i)
            inputArgs[i] = QOpen62541ValueConverter::toOpen62541Variant(args[i].first, args[i].second);
    }

    quint32 requestId = 0;

    UA_CallRequest request;
    UA_CallRequest_init(&request);
    request.requestHeader.timeoutHint = m_asyncRequestTimeout;
    UaDeleter<UA_CallRequest> requestDeleter(&request, UA_CallRequest_clear);

    request.methodsToCallSize = 1;
    request.methodsToCall = UA_CallMethodRequest_new();
    request.methodsToCall->objectId = objectId;
    request.methodsToCall->methodId = methodId;
    request.methodsToCall->inputArguments = inputArgs;
    request.methodsToCall->inputArgumentsSize = args.size();

    UA_StatusCode result = __UA_Client_AsyncService(m_uaclient, &request, &UA_TYPES[UA_TYPES_CALLREQUEST],
                                                      &asyncMethodCallback,
                                                      &UA_TYPES[UA_TYPES_CALLRESPONSE],
                                                      this, &requestId);
    if (result != UA_STATUSCODE_GOOD)
        emit methodCallFinished(handle, Open62541Utils::nodeIdToQString(methodId), QVariant(),
                                static_cast<QOpcUa::UaStatusCode>(result));

    m_asyncCallContext[requestId] = { handle, Open62541Utils::nodeIdToQString(methodId) };
    triggerIterateClient();
}

void Open62541AsyncBackend::resolveBrowsePath(quint64 handle, UA_NodeId startNode, const QList<QOpcUaRelativePathElement> &path)
{
    if (!m_uaclient) {
        UA_NodeId_clear(&startNode);
        emit resolveBrowsePathFinished(handle, {}, path, QOpcUa::UaStatusCode::BadDisconnect);
        return;
    }

    UA_TranslateBrowsePathsToNodeIdsRequest req;
    UA_TranslateBrowsePathsToNodeIdsRequest_init(&req);
    req.requestHeader.timeoutHint = m_asyncRequestTimeout;
    UaDeleter<UA_TranslateBrowsePathsToNodeIdsRequest> requestDeleter(
                &req,UA_TranslateBrowsePathsToNodeIdsRequest_clear);

    req.browsePathsSize = 1;
    req.browsePaths = UA_BrowsePath_new();
    UA_BrowsePath_init(req.browsePaths);
    req.browsePaths->startingNode = startNode;
    req.browsePaths->relativePath.elementsSize = path.size();
    req.browsePaths->relativePath.elements = static_cast<UA_RelativePathElement *>(UA_Array_new(path.size(), &UA_TYPES[UA_TYPES_RELATIVEPATHELEMENT]));

    for (qsizetype i = 0 ; i < path.size(); ++i) {
        req.browsePaths->relativePath.elements[i].includeSubtypes = path[i].includeSubtypes();
        req.browsePaths->relativePath.elements[i].isInverse = path[i].isInverse();
        req.browsePaths->relativePath.elements[i].referenceTypeId = Open62541Utils::nodeIdFromQString(path[i].referenceTypeId());
        req.browsePaths->relativePath.elements[i].targetName = UA_QUALIFIEDNAME_ALLOC(path[i].targetName().namespaceIndex(),
                                                                                      path[i].targetName().name().toUtf8().constData());
    }

    quint32 requestId = 0;
    UA_StatusCode result = __UA_Client_AsyncService(m_uaclient, &req, &UA_TYPES[UA_TYPES_TRANSLATEBROWSEPATHSTONODEIDSREQUEST],
                                                      &asyncTranslateBrowsePathCallback,
                                                      &UA_TYPES[UA_TYPES_TRANSLATEBROWSEPATHSTONODEIDSRESPONSE],
                                                      this, &requestId);

    if (result != UA_STATUSCODE_GOOD) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Translate browse path failed:" << UA_StatusCode_name(result);
        emit resolveBrowsePathFinished(handle, QList<QOpcUaBrowsePathTarget>(), path,
                                         static_cast<QOpcUa::UaStatusCode>(result));
        return;
    }

    m_asyncTranslateContext[requestId] = { handle, path };
    triggerIterateClient();
}

void Open62541AsyncBackend::open62541LogHandler (void *logContext, UA_LogLevel level, UA_LogCategory category,
                                                 const char *msg, va_list args) {

    Q_UNUSED(logContext)

    Q_STATIC_ASSERT(UA_LOGCATEGORY_NETWORK == 0);
    Q_STATIC_ASSERT(UA_LOGCATEGORY_SECURECHANNEL == 1);
    Q_STATIC_ASSERT(UA_LOGCATEGORY_SESSION == 2);
    Q_STATIC_ASSERT(UA_LOGCATEGORY_SERVER == 3);
    Q_STATIC_ASSERT(UA_LOGCATEGORY_CLIENT == 4);
    Q_STATIC_ASSERT(UA_LOGCATEGORY_USERLAND == 5);
    Q_STATIC_ASSERT(UA_LOGCATEGORY_SECURITYPOLICY == 6);
    Q_STATIC_ASSERT(UA_LOGCATEGORY_EVENTLOOP == 7);
    Q_STATIC_ASSERT(UA_LOGCATEGORY_PUBSUB == 8);
    Q_STATIC_ASSERT(UA_LOGCATEGORY_DISCOVERY == 9);

    Q_ASSERT(category <= UA_LOGCATEGORY_DISCOVERY);

    const auto logMessage = QString::vasprintf(msg, args);

    static const QLoggingCategory loggingCategories[] {
        QLoggingCategory("qt.opcua.plugins.open62541.sdk.network"),
        QLoggingCategory("qt.opcua.plugins.open62541.sdk.securechannel"),
        QLoggingCategory("qt.opcua.plugins.open62541.sdk.session"),
        QLoggingCategory("qt.opcua.plugins.open62541.sdk.server"),
        QLoggingCategory("qt.opcua.plugins.open62541.sdk.client"),
        QLoggingCategory("qt.opcua.plugins.open62541.sdk.userland"),
        QLoggingCategory("qt.opcua.plugins.open62541.sdk.securitypolicy"),
        QLoggingCategory("qt.opcua.plugins.open62541.sdk.eventloop"),
        QLoggingCategory("qt.opcua.plugins.open62541.sdk.pubsub"),
        QLoggingCategory("qt.opcua.plugins.open62541.sdk.discovery")
    };

    switch (level) {
    case UA_LOGLEVEL_TRACE:
    case UA_LOGLEVEL_DEBUG:
        qCDebug(loggingCategories[category]).noquote() << logMessage;
        break;
    case UA_LOGLEVEL_INFO:
        qCInfo(loggingCategories[category]).noquote() << logMessage;
        break;
    case UA_LOGLEVEL_WARNING:
        qCWarning(loggingCategories[category]).noquote() << logMessage;
        break;
    case UA_LOGLEVEL_ERROR:
    case UA_LOGLEVEL_FATAL:
        qCCritical(loggingCategories[category]).noquote() << logMessage;
        break;
    default:
        qCCritical(loggingCategories[category]).noquote() << "Unknown UA_LOGLEVEL" << logMessage;
        break;
    }
}

void Open62541AsyncBackend::findServers(const QUrl &url, const QStringList &localeIds, const QStringList &serverUris)
{
    UA_ClientConfig initialConfig {};
    initialConfig.logging = &m_open62541Logger;
    UA_ClientConfig_setDefault(&initialConfig);

    UA_Client *tmpClient = UA_Client_newWithConfig(&initialConfig);

    UaDeleter<UA_Client> clientDeleter(tmpClient, UA_Client_delete);

    UA_String *uaServerUris = nullptr;
    if (!serverUris.isEmpty()) {
        uaServerUris = static_cast<UA_String *>(UA_Array_new(serverUris.size(), &UA_TYPES[UA_TYPES_STRING]));
        for (qsizetype i = 0; i < serverUris.size(); ++i)
            QOpen62541ValueConverter::scalarFromQt(serverUris.at(i), &uaServerUris[i]);
    }
    UaArrayDeleter<UA_TYPES_STRING> serverUrisDeleter(uaServerUris, serverUris.size());

    UA_String *uaLocaleIds = nullptr;
    if (!localeIds.isEmpty()) {
        uaLocaleIds = static_cast<UA_String *>(UA_Array_new(localeIds.size(), &UA_TYPES[UA_TYPES_STRING]));
        for (qsizetype i = 0; i < localeIds.size(); ++i)
            QOpen62541ValueConverter::scalarFromQt(localeIds.at(i), &uaLocaleIds[i]);
    }
    UaArrayDeleter<UA_TYPES_STRING> localeIdsDeleter(uaLocaleIds, localeIds.size());

    size_t serversSize = 0;
    UA_ApplicationDescription *servers = nullptr;

    UA_StatusCode result = UA_Client_findServers(tmpClient, url.toString(QUrl::RemoveUserInfo).toUtf8().constData(),
                                                 serverUris.size(), uaServerUris, localeIds.size(), uaLocaleIds,
                                                 &serversSize, &servers);

    UaArrayDeleter<UA_TYPES_APPLICATIONDESCRIPTION> serversDeleter(servers, serversSize);

    QList<QOpcUaApplicationDescription> ret;

    for (size_t i = 0; i < serversSize; ++i)
        ret.append(convertApplicationDescription(servers[i]));

    if (result != UA_STATUSCODE_GOOD) {
        qCDebug(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to get servers:" << static_cast<QOpcUa::UaStatusCode>(result);
    }

    emit findServersFinished(ret, static_cast<QOpcUa::UaStatusCode>(result), url);
}

void Open62541AsyncBackend::readNodeAttributes(const QList<QOpcUaReadItem> &nodesToRead)
{
    if (!m_uaclient) {
        emit readNodeAttributesFinished({}, QOpcUa::UaStatusCode::BadDisconnect);
        return;
    }

    if (nodesToRead.size() == 0) {
        emit readNodeAttributesFinished(QList<QOpcUaReadResult>(), QOpcUa::UaStatusCode::BadNothingToDo);
        return;
    }

    UA_ReadRequest req;
    UA_ReadRequest_init(&req);
    req.requestHeader.timeoutHint = m_asyncRequestTimeout;
    UaDeleter<UA_ReadRequest> requestDeleter(&req, UA_ReadRequest_clear);

    req.nodesToReadSize = nodesToRead.size();
    req.nodesToRead = static_cast<UA_ReadValueId *>(UA_Array_new(nodesToRead.size(), &UA_TYPES[UA_TYPES_READVALUEID]));
    req.timestampsToReturn = UA_TIMESTAMPSTORETURN_BOTH;

    for (qsizetype i = 0; i < nodesToRead.size(); ++i) {
        UA_ReadValueId_init(&req.nodesToRead[i]);
        req.nodesToRead[i].attributeId = QOpen62541ValueConverter::toUaAttributeId(nodesToRead.at(i).attribute());
        req.nodesToRead[i].nodeId = Open62541Utils::nodeIdFromQString(nodesToRead.at(i).nodeId());
        if (!nodesToRead[i].indexRange().isEmpty())
            QOpen62541ValueConverter::scalarFromQt<UA_String, QString>(nodesToRead.at(i).indexRange(),
                                                                       &req.nodesToRead[i].indexRange);
    }

    quint32 requestId = 0;
    UA_StatusCode result = __UA_Client_AsyncService(m_uaclient, &req, &UA_TYPES[UA_TYPES_READREQUEST], &asyncBatchReadCallback,
                                                      &UA_TYPES[UA_TYPES_READRESPONSE], this, &requestId);

    if (result != UA_STATUSCODE_GOOD) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Batch read failed:" << result;
        emit readNodeAttributesFinished(QList<QOpcUaReadResult>(), static_cast<QOpcUa::UaStatusCode>(result));
        return;
    }

    m_asyncBatchReadContext[requestId] = { nodesToRead };
    triggerIterateClient();
}

void Open62541AsyncBackend::writeNodeAttributes(const QList<QOpcUaWriteItem> &nodesToWrite)
{
    if (!m_uaclient) {
        emit writeNodeAttributesFinished({}, QOpcUa::UaStatusCode::BadDisconnect);
        return;
    }

    if (nodesToWrite.isEmpty()) {
        emit writeNodeAttributesFinished(QList<QOpcUaWriteResult>(), QOpcUa::UaStatusCode::BadNothingToDo);
        return;
    }

    UA_WriteRequest req;
    UA_WriteRequest_init(&req);
    req.requestHeader.timeoutHint = m_asyncRequestTimeout;
    UaDeleter<UA_WriteRequest> requestDeleter(&req, UA_WriteRequest_clear);

    req.nodesToWriteSize = nodesToWrite.size();
    req.nodesToWrite = static_cast<UA_WriteValue *>(UA_Array_new(nodesToWrite.size(), &UA_TYPES[UA_TYPES_WRITEVALUE]));

    for (qsizetype i = 0; i < nodesToWrite.size(); ++i) {
        const auto &currentItem = nodesToWrite.at(i);
        auto &currentUaItem = req.nodesToWrite[i];
        currentUaItem.attributeId = QOpen62541ValueConverter::toUaAttributeId(currentItem.attribute());
        currentUaItem.nodeId = Open62541Utils::nodeIdFromQString(currentItem.nodeId());
        if (currentItem.hasStatusCode()) {
            currentUaItem.value.status = currentItem.statusCode();
            currentUaItem.value.hasStatus = UA_TRUE;
        }
        if (!currentItem.indexRange().isEmpty())
            QOpen62541ValueConverter::scalarFromQt<UA_String, QString>(currentItem.indexRange(), &currentUaItem.indexRange);
        if (!currentItem.value().isNull()) {
            currentUaItem.value.hasValue = true;
            currentUaItem.value.value = QOpen62541ValueConverter::toOpen62541Variant(currentItem.value(), currentItem.type());
        }
        if (currentItem.sourceTimestamp().isValid()) {
            QOpen62541ValueConverter::scalarFromQt<UA_DateTime, QDateTime>(currentItem.sourceTimestamp(),
                                                                           &currentUaItem.value.sourceTimestamp);
            currentUaItem.value.hasSourceTimestamp = UA_TRUE;
        }
        if (currentItem.serverTimestamp().isValid()) {
            QOpen62541ValueConverter::scalarFromQt<UA_DateTime, QDateTime>(currentItem.serverTimestamp(),
                                                                           &currentUaItem.value.serverTimestamp);
            currentUaItem.value.hasServerTimestamp = UA_TRUE;
        }
    }

    quint32 requestId = 0;
    UA_StatusCode result = __UA_Client_AsyncService(m_uaclient, &req, &UA_TYPES[UA_TYPES_WRITEREQUEST], &asyncBatchWriteCallback,
                                                      &UA_TYPES[UA_TYPES_WRITERESPONSE], this, &requestId);

    if (result != UA_STATUSCODE_GOOD) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Batch write failed:" << result;
        emit writeNodeAttributesFinished(QList<QOpcUaWriteResult>(), static_cast<QOpcUa::UaStatusCode>(result));
        return;
    }

    m_asyncBatchWriteContext[requestId] = { nodesToWrite };
    triggerIterateClient();
}

void Open62541AsyncBackend::readHistoryRaw(QOpcUaHistoryReadRawRequest request, QList<QByteArray> continuationPoints, bool releaseContinuationPoints, quint64 handle)
{
    if (!m_uaclient) {
        emit historyDataAvailable({}, {}, QOpcUa::UaStatusCode::BadDisconnect, handle);
        return;
    }

    if (!continuationPoints.empty() && continuationPoints.size() != request.nodesToRead().size()) {
        emit historyDataAvailable({}, {}, QOpcUa::UaStatusCode::BadInternalError, handle);
        return;
    }

    UA_HistoryReadRequest uarequest;
    UA_HistoryReadRequest_init(&uarequest);
    uarequest.requestHeader.timeoutHint = m_asyncRequestTimeout;
    uarequest.nodesToReadSize = request.nodesToRead().size();
    uarequest.nodesToRead = static_cast<UA_HistoryReadValueId*>(UA_Array_new(uarequest.nodesToReadSize, &UA_TYPES[UA_TYPES_HISTORYREADVALUEID]));
    for (size_t i = 0; i < uarequest.nodesToReadSize; ++i) {
        uarequest.nodesToRead[i].nodeId = Open62541Utils::nodeIdFromQString(request.nodesToRead().at(i).nodeId());
        QOpen62541ValueConverter::scalarFromQt<UA_String, QString>(request.nodesToRead().at(i).indexRange(), &uarequest.nodesToRead[i].indexRange);
        uarequest.nodesToRead[i].dataEncoding = UA_QUALIFIEDNAME_ALLOC(0, "Default Binary");
        if (!continuationPoints.isEmpty())
            QOpen62541ValueConverter::scalarFromQt<UA_ByteString, QByteArray>(continuationPoints.at(i), &uarequest.nodesToRead[i].continuationPoint);
    }
    uarequest.timestampsToReturn = static_cast<UA_TimestampsToReturn>(request.timestampsToReturn());

    if (releaseContinuationPoints)
        uarequest.releaseContinuationPoints = releaseContinuationPoints;

    uarequest.historyReadDetails.encoding = UA_EXTENSIONOBJECT_DECODED;
    uarequest.historyReadDetails.content.decoded.type = &UA_TYPES[UA_TYPES_READRAWMODIFIEDDETAILS];
    UA_ReadRawModifiedDetails *details = UA_ReadRawModifiedDetails_new();
    uarequest.historyReadDetails.content.decoded.data = details;
    QOpen62541ValueConverter::scalarFromQt<UA_DateTime, QDateTime>(request.startTimestamp(), &details->startTime);
    QOpen62541ValueConverter::scalarFromQt<UA_DateTime, QDateTime>(request.endTimestamp(), &details->endTime);
    details->isReadModified = UA_FALSE;
    details->returnBounds = request.returnBounds();
    details->numValuesPerNode = request.numValuesPerNode();

    quint32 requestId = 0;
    UA_StatusCode resultCode = __UA_Client_AsyncService(m_uaclient, &uarequest, &UA_TYPES[UA_TYPES_HISTORYREADREQUEST], &asyncReadHistoryDataCallBack,
                                                      &UA_TYPES[UA_TYPES_HISTORYREADRESPONSE], this, &requestId);

    UA_HistoryReadRequest_clear(&uarequest);

    if (resultCode != UA_STATUSCODE_GOOD) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Read history data failed:" << resultCode;
        emit historyDataAvailable({}, {}, QOpcUa::UaStatusCode(resultCode), handle);
        return;
    }

    m_asyncReadHistoryDataContext[requestId] = {handle, request};
    triggerIterateClient();
}

void Open62541AsyncBackend::readHistoryEvents(const QOpcUaHistoryReadEventRequest &request, const QList<QByteArray> &continuationPoints,
                                              bool releaseContinuationPoints, quint64 handle)
{
    if (!m_uaclient) {
        emit historyDataAvailable({}, {}, QOpcUa::UaStatusCode::BadDisconnect, handle);
        return;
    }

    if (!continuationPoints.empty() && continuationPoints.size() != request.nodesToRead().size()) {
        emit historyDataAvailable({}, {}, QOpcUa::UaStatusCode::BadInternalError, handle);
        return;
    }

    UA_HistoryReadRequest uarequest;
    UA_HistoryReadRequest_init(&uarequest);
    uarequest.requestHeader.timeoutHint = m_asyncRequestTimeout;
    uarequest.nodesToReadSize = request.nodesToRead().size();
    uarequest.nodesToRead = static_cast<UA_HistoryReadValueId*>(UA_Array_new(uarequest.nodesToReadSize, &UA_TYPES[UA_TYPES_HISTORYREADVALUEID]));

    for (size_t i = 0; i < uarequest.nodesToReadSize; ++i) {
        uarequest.nodesToRead[i].nodeId = Open62541Utils::nodeIdFromQString(request.nodesToRead().at(i).nodeId());
        QOpen62541ValueConverter::scalarFromQt<UA_String, QString>(request.nodesToRead().at(i).indexRange(), &uarequest.nodesToRead[i].indexRange);
        uarequest.nodesToRead[i].dataEncoding = UA_QUALIFIEDNAME_ALLOC(0, "Default Binary");
        if (!continuationPoints.isEmpty())
            QOpen62541ValueConverter::scalarFromQt<UA_ByteString, QByteArray>(continuationPoints.at(i), &uarequest.nodesToRead[i].continuationPoint);
    }

    uarequest.timestampsToReturn = UA_TIMESTAMPSTORETURN_BOTH;

    if (releaseContinuationPoints)
        uarequest.releaseContinuationPoints = releaseContinuationPoints;

    uarequest.historyReadDetails.encoding = UA_EXTENSIONOBJECT_DECODED;
    uarequest.historyReadDetails.content.decoded.type = &UA_TYPES[UA_TYPES_READEVENTDETAILS];
    UA_ReadEventDetails *details = UA_ReadEventDetails_new();
    uarequest.historyReadDetails.content.decoded.data = details;
    QOpen62541ValueConverter::scalarFromQt<UA_DateTime, QDateTime>(request.startTimestamp(), &details->startTime);
    QOpen62541ValueConverter::scalarFromQt<UA_DateTime, QDateTime>(request.endTimestamp(), &details->endTime);
    details->numValuesPerNode = request.numValuesPerNode();

    QOpen62541ValueConverter::scalarFromQt<UA_EventFilter, QOpcUaMonitoringParameters::EventFilter>(request.filter(), &details->filter);

    quint32 requestId = 0;
    UA_StatusCode resultCode = __UA_Client_AsyncService(m_uaclient, &uarequest, &UA_TYPES[UA_TYPES_HISTORYREADREQUEST], &asyncReadHistoryEventsCallback,
                                                        &UA_TYPES[UA_TYPES_HISTORYREADRESPONSE], this, &requestId);

    UA_HistoryReadRequest_clear(&uarequest);

    if (resultCode != UA_STATUSCODE_GOOD) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Read history data failed:" << resultCode;
        emit historyDataAvailable({}, {}, QOpcUa::UaStatusCode(resultCode), handle);
        return;
    }

    m_asyncReadHistoryEventsContext[requestId] = {handle, request};
    triggerIterateClient();
}

void Open62541AsyncBackend::addNode(const QOpcUaAddNodeItem &nodeToAdd)
{
    if (!m_uaclient) {
        emit addNodeFinished(nodeToAdd.requestedNewNodeId(), {}, QOpcUa::UaStatusCode::BadDisconnect);
        return;
    }

    UA_AddNodesRequest req;
    UA_AddNodesRequest_init(&req);
    req.requestHeader.timeoutHint = m_asyncRequestTimeout;
    UaDeleter<UA_AddNodesRequest> requestDeleter(&req, UA_AddNodesRequest_clear);
    req.nodesToAddSize = 1;
    req.nodesToAdd = UA_AddNodesItem_new();
    UA_AddNodesItem_init(req.nodesToAdd);

    QOpen62541ValueConverter::scalarFromQt<UA_ExpandedNodeId, QOpcUaExpandedNodeId>(
                nodeToAdd.parentNodeId(), &req.nodesToAdd->parentNodeId);

    req.nodesToAdd->referenceTypeId = Open62541Utils::nodeIdFromQString(nodeToAdd.referenceTypeId());

    QOpen62541ValueConverter::scalarFromQt<UA_ExpandedNodeId, QOpcUaExpandedNodeId>(
                nodeToAdd.requestedNewNodeId(), &req.nodesToAdd->requestedNewNodeId);

    QOpen62541ValueConverter::scalarFromQt<UA_QualifiedName, QOpcUaQualifiedName>(
                nodeToAdd.browseName(), &req.nodesToAdd->browseName);

    req.nodesToAdd->nodeClass = static_cast<UA_NodeClass>(nodeToAdd.nodeClass());

    req.nodesToAdd->nodeAttributes = assembleNodeAttributes(nodeToAdd.nodeAttributes(),
                                                            nodeToAdd.nodeClass());

    if (!nodeToAdd.typeDefinition().nodeId().isEmpty())
        QOpen62541ValueConverter::scalarFromQt<UA_ExpandedNodeId, QOpcUaExpandedNodeId>(
                    nodeToAdd.typeDefinition(), &req.nodesToAdd->typeDefinition);

    quint32 requestId = 0;
    UA_StatusCode result = __UA_Client_AsyncService(m_uaclient, &req, &UA_TYPES[UA_TYPES_ADDNODESREQUEST],
                                                      &asyncAddNodeCallback,
                                                      &UA_TYPES[UA_TYPES_ADDNODESRESPONSE],
                                                      this, &requestId);

    if (result != UA_STATUSCODE_GOOD) {
        qCDebug(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to add node:" << result;
        emit addNodeFinished(nodeToAdd.requestedNewNodeId(), QString(), static_cast<QOpcUa::UaStatusCode>(result));
        return;
    }

    m_asyncAddNodeContext[requestId] = { nodeToAdd.requestedNewNodeId() };
    triggerIterateClient();
}

void Open62541AsyncBackend::deleteNode(const QString &nodeId, bool deleteTargetReferences)
{
    if (!m_uaclient) {
        emit deleteNodeFinished(nodeId, QOpcUa::UaStatusCode::BadDisconnect);
        return;
    }

    UA_DeleteNodesRequest request;
    UA_DeleteNodesRequest_init(&request);
    request.requestHeader.timeoutHint = m_asyncRequestTimeout;
    UaDeleter<UA_DeleteNodesRequest> requestDeleter(&request, UA_DeleteNodesRequest_clear);

    request.nodesToDeleteSize = 1;
    request.nodesToDelete = UA_DeleteNodesItem_new();

    request.nodesToDelete->nodeId = Open62541Utils::nodeIdFromQString(nodeId);
    request.nodesToDelete->deleteTargetReferences = deleteTargetReferences;

    quint32 requestId = 0;
    UA_StatusCode result = __UA_Client_AsyncService(m_uaclient, &request, &UA_TYPES[UA_TYPES_DELETENODESREQUEST],
                                                      &asyncDeleteNodeCallback,
                                                      &UA_TYPES[UA_TYPES_DELETENODESRESPONSE],
                                                      this, &requestId);

    QOpcUa::UaStatusCode resultStatus = static_cast<QOpcUa::UaStatusCode>(result);

    if (result != QOpcUa::UaStatusCode::Good) {
        qCDebug(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to delete node" << nodeId << "with status code" << resultStatus;
        emit deleteNodeFinished(nodeId, resultStatus);
        return;
    }

    m_asyncDeleteNodeContext[requestId] = { nodeId };
    triggerIterateClient();
}

void Open62541AsyncBackend::addReference(const QOpcUaAddReferenceItem &referenceToAdd)
{
    if (!m_uaclient) {
        emit addReferenceFinished(referenceToAdd.sourceNodeId(), referenceToAdd.referenceTypeId(),
                                  referenceToAdd.targetNodeId(), referenceToAdd.isForwardReference(),
                                  QOpcUa::UaStatusCode::BadDisconnect);
        return;
    }

    UA_AddReferencesRequest request;
    UA_AddReferencesRequest_init(&request);
    request.requestHeader.timeoutHint = m_asyncRequestTimeout;
    UaDeleter<UA_AddReferencesRequest> requestDeleter(&request, UA_AddReferencesRequest_clear);

    request.referencesToAddSize = 1;
    request.referencesToAdd = UA_AddReferencesItem_new();

    request.referencesToAdd->isForward = referenceToAdd.isForwardReference();
    QOpen62541ValueConverter::scalarFromQt<UA_NodeId, QString>(referenceToAdd.sourceNodeId(),
                                                               &request.referencesToAdd->sourceNodeId);
    QOpen62541ValueConverter::scalarFromQt<UA_ExpandedNodeId, QOpcUaExpandedNodeId>(referenceToAdd.targetNodeId(),
                                                                                  &request.referencesToAdd->targetNodeId);
    QOpen62541ValueConverter::scalarFromQt<UA_NodeId, QString>(referenceToAdd.referenceTypeId(),
                                                               &request.referencesToAdd->referenceTypeId);
    request.referencesToAdd->targetNodeClass = static_cast<UA_NodeClass>(referenceToAdd.targetNodeClass());
    QOpen62541ValueConverter::scalarFromQt<UA_String, QString>(referenceToAdd.targetServerUri(),
                                                               &request.referencesToAdd->targetServerUri);

    quint32 requestId = 0;
    UA_StatusCode result = __UA_Client_AsyncService(m_uaclient, &request, &UA_TYPES[UA_TYPES_ADDREFERENCESREQUEST],
                                                      &asyncAddReferenceCallback,
                                                      &UA_TYPES[UA_TYPES_ADDREFERENCESRESPONSE],
                                                      this, &requestId);

    QOpcUa::UaStatusCode statusCode = static_cast<QOpcUa::UaStatusCode>(result);
    if (result != UA_STATUSCODE_GOOD) {
        qCDebug(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to add reference from" << referenceToAdd.sourceNodeId() << "to"
                                            << referenceToAdd.targetNodeId().nodeId() << ":" << statusCode;
        emit addReferenceFinished(referenceToAdd.sourceNodeId(), referenceToAdd.referenceTypeId(),
                                  referenceToAdd.targetNodeId(), referenceToAdd.isForwardReference(), statusCode);
        return;
    }

    m_asyncAddReferenceContext[requestId] = { referenceToAdd.sourceNodeId(), referenceToAdd.referenceTypeId(),
                                              referenceToAdd.targetNodeId(), referenceToAdd.isForwardReference() };
    triggerIterateClient();
}

void Open62541AsyncBackend::deleteReference(const QOpcUaDeleteReferenceItem &referenceToDelete)
{
    if (!m_uaclient) {
        emit deleteReferenceFinished(referenceToDelete.sourceNodeId(), referenceToDelete.referenceTypeId(),
                                     referenceToDelete.targetNodeId(), referenceToDelete.isForwardReference(),
                                     QOpcUa::UaStatusCode::BadDisconnect);
        return;
    }

    UA_DeleteReferencesRequest request;
    UA_DeleteReferencesRequest_init(&request);
    request.requestHeader.timeoutHint = m_asyncRequestTimeout;
    UaDeleter<UA_DeleteReferencesRequest> requestDeleter(&request, UA_DeleteReferencesRequest_clear);

    request.referencesToDeleteSize = 1;
    request.referencesToDelete = UA_DeleteReferencesItem_new();
    request.referencesToDelete->isForward = referenceToDelete.isForwardReference();
    QOpen62541ValueConverter::scalarFromQt<UA_NodeId, QString>(referenceToDelete.sourceNodeId(),
                                                               &request.referencesToDelete->sourceNodeId);
    QOpen62541ValueConverter::scalarFromQt<UA_ExpandedNodeId, QOpcUaExpandedNodeId>(referenceToDelete.targetNodeId(),
                                                                                    &request.referencesToDelete->targetNodeId);
    QOpen62541ValueConverter::scalarFromQt<UA_NodeId, QString>(referenceToDelete.referenceTypeId(),
                                                               &request.referencesToDelete->referenceTypeId);
    request.referencesToDelete->deleteBidirectional = referenceToDelete.deleteBidirectional();

    quint32 requestId = 0;
    UA_StatusCode result = __UA_Client_AsyncService(m_uaclient, &request, &UA_TYPES[UA_TYPES_DELETEREFERENCESREQUEST],
                                                      &asyncDeleteReferenceCallback,
                                                      &UA_TYPES[UA_TYPES_DELETEREFERENCESRESPONSE],
                                                      this, &requestId);

    QOpcUa::UaStatusCode statusCode = static_cast<QOpcUa::UaStatusCode>(result);
    if (result != UA_STATUSCODE_GOOD) {
        qCDebug(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to delete reference from" << referenceToDelete.sourceNodeId() << "to"
                                            << referenceToDelete.targetNodeId().nodeId() << ":" << statusCode;

        emit deleteReferenceFinished(referenceToDelete.sourceNodeId(), referenceToDelete.referenceTypeId(),
                                     referenceToDelete.targetNodeId(),
                                     referenceToDelete.isForwardReference(), statusCode);
        return;
    }

    m_asyncDeleteReferenceContext[requestId] = { referenceToDelete.sourceNodeId(), referenceToDelete.referenceTypeId(),
                                               referenceToDelete.targetNodeId(), referenceToDelete.isForwardReference()};
    triggerIterateClient();
}

static void convertBrowseResult(UA_BrowseResult *src, size_t referencesSize, QList<QOpcUaReferenceDescription> &dst)
{
    if (!src)
        return;

    for (size_t i = 0; i < referencesSize; ++i) {
        QOpcUaReferenceDescription temp;
        temp.setTargetNodeId(QOpen62541ValueConverter::scalarToQt<QOpcUaExpandedNodeId>(&src->references[i].nodeId));
        temp.setTypeDefinition(QOpen62541ValueConverter::scalarToQt<QOpcUaExpandedNodeId>(&src->references[i].typeDefinition));
        temp.setRefTypeId(Open62541Utils::nodeIdToQString(src->references[i].referenceTypeId));
        temp.setNodeClass(static_cast<QOpcUa::NodeClass>(src->references[i].nodeClass));
        temp.setBrowseName(QOpen62541ValueConverter::scalarToQt<QOpcUaQualifiedName, UA_QualifiedName>(&src->references[i].browseName));
        temp.setDisplayName(QOpen62541ValueConverter::scalarToQt<QOpcUaLocalizedText, UA_LocalizedText>(&src->references[i].displayName));
        temp.setIsForwardReference(src->references[i].isForward);
        dst.push_back(temp);
    }
}

void Open62541AsyncBackend::browse(quint64 handle, UA_NodeId id, const QOpcUaBrowseRequest &request)
{
    if (!m_uaclient) {
        emit browseFinished(handle, {}, QOpcUa::UaStatusCode::BadDisconnect);
        return;
    }

    UA_BrowseRequest uaRequest;
    UA_BrowseRequest_init(&uaRequest);
    uaRequest.requestHeader.timeoutHint = m_asyncRequestTimeout;
    UaDeleter<UA_BrowseRequest> requestDeleter(&uaRequest, UA_BrowseRequest_clear);

    uaRequest.nodesToBrowse = UA_BrowseDescription_new();
    uaRequest.nodesToBrowseSize = 1;
    uaRequest.nodesToBrowse->browseDirection = static_cast<UA_BrowseDirection>(request.browseDirection());
    uaRequest.nodesToBrowse->includeSubtypes = request.includeSubtypes();
    uaRequest.nodesToBrowse->nodeClassMask = static_cast<quint32>(request.nodeClassMask());
    uaRequest.nodesToBrowse->nodeId = id;
    uaRequest.nodesToBrowse->resultMask = UA_BROWSERESULTMASK_ALL;
    uaRequest.nodesToBrowse->referenceTypeId = Open62541Utils::nodeIdFromQString(request.referenceTypeId());
    uaRequest.requestedMaxReferencesPerNode = 0; // Let the server choose a maximum value

    quint32 requestId = 0;
    UA_StatusCode result = __UA_Client_AsyncService(m_uaclient, &uaRequest, &UA_TYPES[UA_TYPES_BROWSEREQUEST], &asyncBrowseCallback,
                                                      &UA_TYPES[UA_TYPES_BROWSERESPONSE], this, &requestId);

    if (result != UA_STATUSCODE_GOOD) {
        emit browseFinished(handle, QList<QOpcUaReferenceDescription>(), static_cast<QOpcUa::UaStatusCode>(result));
        return;
    }

    m_asyncBrowseContext[requestId] = { handle, false, QList<QOpcUaReferenceDescription>() };
    triggerIterateClient();
}

void Open62541AsyncBackend::clientStateCallback(UA_Client *client,
                                                UA_SecureChannelState channelState,
                                                UA_SessionState sessionState,
                                                UA_StatusCode connectStatus)
{
    Q_UNUSED(channelState)
    Q_UNUSED(sessionState)
    Q_UNUSED(connectStatus)

    Open62541AsyncBackend *backend = static_cast<Open62541AsyncBackend *>(UA_Client_getContext(client));
    if (!backend)
        return;

    // The connection is gone, no need to keep iterating
    backend->m_clientIterateTimer.stop();
    backend->m_clientIterateOnDemandTimer.stop();

    // UA_Client_disconnect() must be called from outside this callback
    QMetaObject::invokeMethod(backend, [backend]() {
        backend->disconnectInternal(QOpcUaClient::ConnectionError);
    }, Qt::QueuedConnection);
}

void Open62541AsyncBackend::inactivityCallback(UA_Client *client)
{
    // The client state callback is not called if the background check encounters a timeout.
    // This call triggers the disconnect and the appropriate state change
    clientStateCallback(client, UA_SECURECHANNELSTATE_CLOSED, UA_SESSIONSTATE_CLOSED, UA_STATUSCODE_BADTIMEOUT);
}

void Open62541AsyncBackend::connectToEndpoint(const QOpcUaEndpointDescription &endpoint)
{
    disconnectInternal();

    emit stateAndOrErrorChanged(QOpcUaClient::Connecting, QOpcUaClient::NoError);

    QString errorMessage;
    const auto endpointError = verifyEndpointDescription(endpoint, &errorMessage);
    if (endpointError != QOpcUaClient::ClientError::NoError) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << errorMessage;
        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, endpointError);
        return;
    }

#ifdef UA_ENABLE_ENCRYPTION
    const auto pkiConfig = m_clientImpl->m_client->pkiConfiguration();
#endif

    if (!m_clientImpl->supportedSecurityPolicies().contains(endpoint.securityPolicy())
#ifdef UA_ENABLE_ENCRYPTION
        || (!pkiConfig.isKeyAndCertificateFileSet() && endpoint.securityPolicy() != QOpcUa::NonePolicy)
#endif
        ) {
#ifndef UA_ENABLE_ENCRYPTION
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "The open62541 plugin has been built without encryption support, only the"
                                              << QOpcUa::NonePolicy << "security policy is supported";
#endif
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Unsupported security policy:" << endpoint.securityPolicy();

#ifdef UA_ENABLE_ENCRYPTION
        if (!pkiConfig.isKeyAndCertificateFileSet()) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541)
                << "No certificate and private key is set, only the" << QOpcUa::NonePolicy << "security policy is supported";
        }
#endif
        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::ClientError::UnsupportedSecurityPolicy);
        return;
    }

    const auto identity = m_clientImpl->m_client->applicationIdentity();
    const auto authInfo = m_clientImpl->m_client->authenticationInformation();

    if (authInfo.authenticationType() == QOpcUaUserTokenPolicy::TokenType::Certificate) {
#ifndef UA_ENABLE_ENCRYPTION
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "The open62541 plugin has been built without encryption support, "
                                                 "certificate auth is not supported";
        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected,
                                    QOpcUaClient::ClientError::UnsupportedAuthenticationInformation);
        return;
#else
        if (!authInfo.authenticationData().isValid() && !pkiConfig.isKeyAndCertificateFileSet()) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Unable to do certificate auth when no certificate is set";
            emit stateAndOrErrorChanged(QOpcUaClient::Disconnected,
                                        QOpcUaClient::ClientError::InvalidAuthenticationInformation);
            return;
        }
#endif
    }

    // Possible early out if no token matching the configuration and the client's capabilities is available
    // If the token's security policy URI is not None, at least a certificate and a private key must be set
    bool suitableTokenFound = false;
    const auto userIdentityTokens = endpoint.userIdentityTokens();
    for (const auto &token : userIdentityTokens) {
        // An empty security policy URI means that the endpoint's policy is to be used
        const auto tokenPolicy = token.securityPolicy().isEmpty() ? endpoint.securityPolicy() : token.securityPolicy();

        if (token.tokenType() == authInfo.authenticationType() &&
            m_clientImpl->supportedSecurityPolicies().contains(tokenPolicy) &&
#ifdef UA_ENABLE_ENCRYPTION
            (tokenPolicy == QOpcUa::NonePolicy || pkiConfig.isKeyAndCertificateFileSet())
#else
            tokenPolicy == QOpcUa::NonePolicy
#endif
            ) {
            suitableTokenFound = true;
            break;
        }
    }

    if (!suitableTokenFound) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "No suitable identity token found in the endpoint";
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Availabe tokens in the endpoint:";
        for (const auto &token : userIdentityTokens) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "    Type:" << token.tokenType()
                                                  << "SecurityPolicyUri:" << token.securityPolicy();
        }

        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected,
                                    QOpcUaClient::ClientError::NoMatchingUserIdentityTokenFound);
        return;
    }

    UA_ClientConfig initialConfig {};
    initialConfig.logging = &m_open62541Logger;
    m_uaclient = UA_Client_newWithConfig(&initialConfig);

    auto conf = UA_Client_getConfig(m_uaclient);
    m_currentConnectionSettings = m_clientImpl->m_client->connectionSettings();

#ifdef UA_ENABLE_ENCRYPTION
    if (!setupClientConfigSecurity(authInfo, pkiConfig, endpoint))
        return;
#else
    const auto setDefaultResult = UA_ClientConfig_setDefault(conf);
    if (setDefaultResult != UA_STATUSCODE_GOOD) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to set client config defaults:" << static_cast<QOpcUa::UaStatusCode>(setDefaultResult);
        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::UnknownError);
        UA_Client_delete(m_uaclient);
        m_uaclient = nullptr;
        return;
    }
#endif

    using Timeout_t = decltype(conf->timeout);
    conf->timeout = qt_saturate<Timeout_t>(m_currentConnectionSettings.connectTimeout().count());
    conf->secureChannelLifeTime = qt_saturate<Timeout_t>(m_currentConnectionSettings.secureChannelLifeTime().count());
    conf->requestedSessionTimeout = qt_saturate<Timeout_t>(m_currentConnectionSettings.sessionTimeout().count());

    const auto sessionLocaleIds = m_currentConnectionSettings.sessionLocaleIds();
    if (!sessionLocaleIds.isEmpty()) {
        conf->sessionLocaleIds = static_cast<UA_String *>(UA_Array_new(sessionLocaleIds.size(), &UA_TYPES[UA_TYPES_STRING]));
        for (qsizetype i = 0; i < sessionLocaleIds.size(); ++i)
            conf->sessionLocaleIds[i] = UA_STRING_ALLOC(sessionLocaleIds[i].toUtf8().constData());
        conf->sessionLocaleIdsSize = sessionLocaleIds.size();
    }

    UA_LocalizedText_clear(&conf->clientDescription.applicationName);
    UA_String_clear(&conf->clientDescription.applicationUri);
    UA_String_clear(&conf->clientDescription.productUri);

    conf->clientContext = this;

    // Send periodic read requests as keepalive
    conf->connectivityCheckInterval = 60000;
    conf->inactivityCallback = inactivityCallback;

    conf->clientDescription.applicationName = UA_LOCALIZEDTEXT_ALLOC("", identity.applicationName().toUtf8().constData());
    conf->clientDescription.applicationUri  = UA_STRING_ALLOC(identity.applicationUri().toUtf8().constData());
    conf->clientDescription.productUri      = UA_STRING_ALLOC(identity.productUri().toUtf8().constData());
    conf->clientDescription.applicationType = UA_APPLICATIONTYPE_CLIENT;

    conf->securityPolicyUri = UA_STRING_ALLOC(endpoint.securityPolicy().toUtf8().constData());
    conf->securityMode = static_cast<UA_MessageSecurityMode>(endpoint.securityMode());

    establishConnectionInternal(authInfo,
#ifdef UA_ENABLE_ENCRYPTION
                                pkiConfig,
#endif
                                endpoint);
}

void Open62541AsyncBackend::disconnectFromEndpoint()
{
    disconnectInternal();
}

void Open62541AsyncBackend::requestEndpoints(const QUrl &url)
{
    UA_ClientConfig initialConfig {};
    initialConfig.logging = &m_open62541Logger;
    UA_ClientConfig_setDefault(&initialConfig);

    UA_Client *tmpClient = UA_Client_newWithConfig(&initialConfig);

    size_t numEndpoints = 0;
    UA_EndpointDescription *endpoints = nullptr;
    UA_StatusCode res = UA_Client_getEndpoints(tmpClient, url.toString(QUrl::RemoveUserInfo).toUtf8().constData(), &numEndpoints, &endpoints);
    UaArrayDeleter<UA_TYPES_ENDPOINTDESCRIPTION> endpointDescriptionDeleter(endpoints, numEndpoints);
    QList<QOpcUaEndpointDescription> ret;

    namespace vc = QOpen62541ValueConverter;
    using namespace QOpcUa;
    if (res == UA_STATUSCODE_GOOD && numEndpoints) {
        for (size_t i = 0; i < numEndpoints ; ++i) {
            QOpcUaEndpointDescription epd;
            QOpcUaApplicationDescription &apd = epd.serverRef();

            apd.setApplicationUri(vc::scalarToQt<QString, UA_String>(&endpoints[i].server.applicationUri));
            apd.setProductUri(vc::scalarToQt<QString, UA_String>(&endpoints[i].server.productUri));
            apd.setApplicationName(vc::scalarToQt<QOpcUaLocalizedText, UA_LocalizedText>(&endpoints[i].server.applicationName));
            apd.setApplicationType(static_cast<QOpcUaApplicationDescription::ApplicationType>(endpoints[i].server.applicationType));
            apd.setGatewayServerUri(vc::scalarToQt<QString, UA_String>(&endpoints[i].server.gatewayServerUri));
            apd.setDiscoveryProfileUri(vc::scalarToQt<QString, UA_String>(&endpoints[i].server.discoveryProfileUri));
            for (size_t j = 0; j < endpoints[i].server.discoveryUrlsSize; ++j)
                apd.discoveryUrlsRef().append(vc::scalarToQt<QString, UA_String>(&endpoints[i].server.discoveryUrls[j]));

            epd.setEndpointUrl(vc::scalarToQt<QString, UA_String>(&endpoints[i].endpointUrl));
            epd.setServerCertificate(vc::scalarToQt<QByteArray, UA_ByteString>(&endpoints[i].serverCertificate));
            epd.setSecurityMode(static_cast<QOpcUaEndpointDescription::MessageSecurityMode>(endpoints[i].securityMode));
            epd.setSecurityPolicy(vc::scalarToQt<QString, UA_String>(&endpoints[i].securityPolicyUri));
            for (size_t j = 0; j < endpoints[i].userIdentityTokensSize; ++j) {
                QOpcUaUserTokenPolicy policy;
                UA_UserTokenPolicy *policySrc = &endpoints[i].userIdentityTokens[j];
                policy.setPolicyId(vc::scalarToQt<QString, UA_String>(&policySrc->policyId));
                policy.setTokenType(static_cast<QOpcUaUserTokenPolicy::TokenType>(endpoints[i].userIdentityTokens[j].tokenType));
                policy.setIssuedTokenType(vc::scalarToQt<QString, UA_String>(&endpoints[i].userIdentityTokens[j].issuedTokenType));
                policy.setIssuerEndpointUrl(vc::scalarToQt<QString, UA_String>(&endpoints[i].userIdentityTokens[j].issuerEndpointUrl));
                policy.setSecurityPolicy(vc::scalarToQt<QString, UA_String>(&endpoints[i].userIdentityTokens[j].securityPolicyUri));
                epd.userIdentityTokensRef().append(policy);
            }

            epd.setTransportProfileUri(vc::scalarToQt<QString, UA_String>(&endpoints[i].transportProfileUri));
            epd.setSecurityLevel(endpoints[i].securityLevel);
            ret.append(epd);
        }
    } else {
        if (res == UA_STATUSCODE_GOOD)
            qWarning() << "Server returned an empty endpoint list";
        else
            qWarning() << "Failed to retrieve endpoints from " << url.toString(QUrl::RemoveUserInfo).toUtf8().constData()
                       << "with status" << UA_StatusCode_name(res);
    }

    emit endpointsRequestFinished(ret, static_cast<QOpcUa::UaStatusCode>(res), url);

    UA_Client_delete(tmpClient);
}

void Open62541AsyncBackend::handleConnectionSettingsChanged(const QOpcUaConnectionSettings &settings)
{
    // If there is no client, the settings will be applied during the next connectToEndpoint() call
    if (m_uaclient) {
        if (m_currentConnectionSettings.requestTimeout() != settings.requestTimeout()) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Changing the request timeout for an established connection is not supported."
                                                  << "The change will be active after the next connectToEndpoint() call.";
        }

        if (m_currentConnectionSettings.secureChannelLifeTime() != settings.secureChannelLifeTime()) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Changing the secure channel lifetime for an established connection is not supported."
                                                  << "The change will be active after the next connectToEndpoint() call.";
        }

        if (m_currentConnectionSettings.sessionTimeout() != settings.sessionTimeout()) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Changing the session timeout for an established connection is not supported."
                                                  << "The change will be active after the next connectToEndpoint() call.";
        }

        if (m_currentConnectionSettings.connectTimeout() != settings.connectTimeout()) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Changing the connect timeout for an established connection is not supported."
                                                  << "The change will be active after the next connectToEndpoint() call.";
        }

        if (m_currentConnectionSettings.sessionLocaleIds() != settings.sessionLocaleIds()) {
            const auto conf = UA_Client_getConfig(m_uaclient);

            if (conf->sessionLocaleIdsSize) {
                UA_Array_delete(conf->sessionLocaleIds, conf->sessionLocaleIdsSize, &UA_TYPES[UA_TYPES_LOCALEID]);
                conf->sessionLocaleIdsSize = 0;
            }

            if (!settings.sessionLocaleIds().isEmpty()) {
                const auto sessionLocaleIds = settings.sessionLocaleIds();
                conf->sessionLocaleIds = static_cast<UA_String *>(UA_Array_new(sessionLocaleIds.size(), &UA_TYPES[UA_TYPES_STRING]));
                for (qsizetype i = 0; i < sessionLocaleIds.size(); ++i)
                    conf->sessionLocaleIds[i] = UA_STRING_ALLOC(sessionLocaleIds[i].toUtf8().constData());
                conf->sessionLocaleIdsSize = sessionLocaleIds.size();
            }

            const auto result = UA_Client_activateCurrentSession(m_uaclient);
            if (result != UA_STATUSCODE_GOOD) {
                qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Changing the session locale Ids failed with" << UA_StatusCode_name(result);
            } else {
                qCInfo(QT_OPCUA_PLUGINS_OPEN62541) << "The session locale ids were updated to" << settings.sessionLocaleIds().join(QChar::fromLatin1(' '));
            }
        }

        m_currentConnectionSettings = settings;
    }
}

void Open62541AsyncBackend::iterateClient()
{
    if (!m_uaclient)
        return;

    // If BADSERVERNOTCONNECTED is returned, the subscriptions are gone and local information can be deleted.
    if (UA_Client_run_iterate(m_uaclient,
                              std::max<quint32>(1, m_clientIterateInterval / 2)) == UA_STATUSCODE_BADSERVERNOTCONNECTED) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Unable to send publish request";
        cleanupSubscriptions();
    }
}

void Open62541AsyncBackend::triggerIterateClient()
{
    // This is 5 seconds faster over all client tests compared to just
    // calling QTimer::singleShot() every time
    if (!m_clientIterateOnDemandTimer.isActive()) {
        // Restart the normal iterate timer, no need to have more invocations
        if (m_clientIterateTimer.isActive())
            m_clientIterateTimer.start(m_clientIterateInterval);
        m_clientIterateOnDemandTimer.start(0);
    }
}

void Open62541AsyncBackend::handleSubscriptionTimeout(QOpen62541Subscription *sub, QList<QPair<quint64, QOpcUa::NodeAttribute>> items)
{
    for (auto it : std::as_const(items)) {
        auto item = m_attributeMapping.find(it.first);
        if (item == m_attributeMapping.end())
            continue;
        item->remove(it.second);
    }
    m_subscriptions.remove(sub->subscriptionId());
    delete sub;
}

QOpen62541Subscription *Open62541AsyncBackend::getSubscriptionForItem(quint64 handle, QOpcUa::NodeAttribute attr)
{
    auto nodeEntry = m_attributeMapping.find(handle);
    if (nodeEntry == m_attributeMapping.end())
        return nullptr;

    auto subscription = nodeEntry->find(attr);
    if (subscription == nodeEntry->end()) {
        return nullptr;
    }

    return subscription.value();
}

QOpcUaApplicationDescription Open62541AsyncBackend::convertApplicationDescription(UA_ApplicationDescription &desc)
{
    QOpcUaApplicationDescription temp;

    temp.setApplicationUri(QOpen62541ValueConverter::scalarToQt<QString, UA_String>(&desc.applicationUri));
    temp.setProductUri(QOpen62541ValueConverter::scalarToQt<QString, UA_String>(&desc.productUri));
    temp.setApplicationName(QOpen62541ValueConverter::scalarToQt<QOpcUaLocalizedText, UA_LocalizedText>(&desc.applicationName));
    temp.setApplicationType(static_cast<QOpcUaApplicationDescription::ApplicationType>(desc.applicationType));
    temp.setGatewayServerUri(QOpen62541ValueConverter::scalarToQt<QString, UA_String>(&desc.gatewayServerUri));
    temp.setDiscoveryProfileUri(QOpen62541ValueConverter::scalarToQt<QString, UA_String>(&desc.discoveryProfileUri));


    for (size_t i = 0; i < desc.discoveryUrlsSize; ++i)
        temp.discoveryUrlsRef().append(QOpen62541ValueConverter::scalarToQt<QString, UA_String>(&desc.discoveryUrls[i]));

    return temp;
}

void Open62541AsyncBackend::cleanupSubscriptions()
{
    qDeleteAll(m_subscriptions);
    m_subscriptions.clear();
    m_attributeMapping.clear();
    m_minPublishingInterval = 0;
}

void Open62541AsyncBackend::registerNodes(const QStringList &nodesToRegister)
{
    if (!m_uaclient) {
        emit registerNodesFinished(nodesToRegister, {}, QOpcUa::UaStatusCode::BadDisconnect);
        return;
    }

    UA_RegisterNodesRequest req;
    UA_RegisterNodesRequest_init(&req);
    req.requestHeader.timeoutHint = m_asyncRequestTimeout;

    req.nodesToRegisterSize = nodesToRegister.size();
    req.nodesToRegister = static_cast<UA_NodeId *>(UA_Array_new(nodesToRegister.size(), &UA_TYPES[UA_TYPES_NODEID]));

    for (int i = 0; i < nodesToRegister.size(); ++i)
        QOpen62541ValueConverter::scalarFromQt<UA_NodeId, QString>(nodesToRegister.at(i), &req.nodesToRegister[i]);

    quint32 requestId = 0;
    UA_StatusCode result = __UA_Client_AsyncService(m_uaclient, &req, &UA_TYPES[UA_TYPES_REGISTERNODESREQUEST],
                                                      &asyncRegisterNodesCallback,
                                                      &UA_TYPES[UA_TYPES_REGISTERNODESRESPONSE],
                                                      this, &requestId);

    UA_RegisterNodesRequest_clear(&req);

    if (result != UA_STATUSCODE_GOOD)  {
        emit registerNodesFinished(nodesToRegister, {}, QOpcUa::UaStatusCode(result));
    } else {
        m_asyncRegisterUnregisterNodesContext[requestId] = { nodesToRegister };
        triggerIterateClient();
    }
}

void Open62541AsyncBackend::unregisterNodes(const QStringList &nodesToUnregister)
{
    if (!m_uaclient) {
        emit unregisterNodesFinished(nodesToUnregister, QOpcUa::UaStatusCode::BadDisconnect);
        return;
    }

    UA_UnregisterNodesRequest req;
    UA_UnregisterNodesRequest_init(&req);
    req.requestHeader.timeoutHint = m_asyncRequestTimeout;

    req.nodesToUnregisterSize = nodesToUnregister.size();
    req.nodesToUnregister = static_cast<UA_NodeId *>(UA_Array_new(nodesToUnregister.size(), &UA_TYPES[UA_TYPES_NODEID]));

    for (int i = 0; i < nodesToUnregister.size(); ++i)
        QOpen62541ValueConverter::scalarFromQt<UA_NodeId, QString>(nodesToUnregister.at(i), &req.nodesToUnregister[i]);

    quint32 requestId = 0;
    UA_StatusCode result = __UA_Client_AsyncService(m_uaclient, &req, &UA_TYPES[UA_TYPES_UNREGISTERNODESREQUEST],
                                                    &asyncUnregisterNodesCallback,
                                                    &UA_TYPES[UA_TYPES_UNREGISTERNODESRESPONSE],
                                                    this, &requestId);

    UA_UnregisterNodesRequest_clear(&req);

    if (result != UA_STATUSCODE_GOOD) {
        emit unregisterNodesFinished(nodesToUnregister, QOpcUa::UaStatusCode(result));
    } else {
        m_asyncRegisterUnregisterNodesContext[requestId] = { nodesToUnregister };
        triggerIterateClient();
    }
}

void Open62541AsyncBackend::asyncMethodCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response)
{
    Q_UNUSED(client)

    Open62541AsyncBackend *backend = static_cast<Open62541AsyncBackend *>(userdata);
    const auto context = backend->m_asyncCallContext.take(requestId);

    QVariant result;

    const auto cr = static_cast<UA_CallResponse *>(response);

    if (cr->resultsSize && cr->results->outputArgumentsSize > 1 && cr->results->statusCode == UA_STATUSCODE_GOOD) {
        QVariantList temp;
        for (size_t i = 0; i < cr->results->outputArgumentsSize; ++i)
            temp.append(QOpen62541ValueConverter::toQVariant(cr->results->outputArguments[i]));

        result = temp;
    } else if (cr->resultsSize && cr->results->outputArgumentsSize == 1 && cr->results->statusCode == UA_STATUSCODE_GOOD) {
        result = QOpen62541ValueConverter::toQVariant(cr->results->outputArguments[0]);
    }

    emit backend->methodCallFinished(context.handle, context.methodNodeId, result,
                                     static_cast<QOpcUa::UaStatusCode>(cr->resultsSize ?
                                                                           cr->results->statusCode :
                                                                           cr->responseHeader.serviceResult));
}

void Open62541AsyncBackend::asyncTranslateBrowsePathCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response)
{
    Q_UNUSED(client)

    Open62541AsyncBackend *backend = static_cast<Open62541AsyncBackend *>(userdata);
    const auto context = backend->m_asyncTranslateContext.take(requestId);

    const auto res = static_cast<UA_TranslateBrowsePathsToNodeIdsResponse *>(response);

    if (res->responseHeader.serviceResult != UA_STATUSCODE_GOOD || res->resultsSize != 1) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Translate browse path failed:" << UA_StatusCode_name(res->responseHeader.serviceResult);
        emit backend->resolveBrowsePathFinished(context.handle, QList<QOpcUaBrowsePathTarget>(), context.path,
                                                static_cast<QOpcUa::UaStatusCode>(res->responseHeader.serviceResult));
        return;
    }

    QList<QOpcUaBrowsePathTarget> ret;
    for (size_t i = 0; i < res->results->targetsSize ; ++i) {
        QOpcUaBrowsePathTarget temp;
        temp.setRemainingPathIndex(res->results->targets[i].remainingPathIndex);
        temp.targetIdRef().setNamespaceUri(QString::fromUtf8(reinterpret_cast<char *>(res->results->targets[i].targetId.namespaceUri.data)));
        temp.targetIdRef().setServerIndex(res->results->targets[i].targetId.serverIndex);
        temp.targetIdRef().setNodeId(Open62541Utils::nodeIdToQString(res->results->targets[i].targetId.nodeId));
        ret.append(temp);
    }

    emit backend->resolveBrowsePathFinished(context.handle, ret, context.path, static_cast<QOpcUa::UaStatusCode>(res->results->statusCode));
}

void Open62541AsyncBackend::asyncAddNodeCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response)
{
    Q_UNUSED(client)

    Open62541AsyncBackend *backend = static_cast<Open62541AsyncBackend *>(userdata);
    const auto context = backend->m_asyncAddNodeContext.take(requestId);

    const auto res = static_cast<UA_AddNodesResponse *>(response);

    QOpcUa::UaStatusCode status = QOpcUa::UaStatusCode::Good;
    QString resultId;
    if (res->responseHeader.serviceResult == UA_STATUSCODE_GOOD) {
        if (res->results->statusCode == UA_STATUSCODE_GOOD)
            resultId = Open62541Utils::nodeIdToQString(res->results->addedNodeId);
        else {
            status = static_cast<QOpcUa::UaStatusCode>(res->results->statusCode);
            qCDebug(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to add node:" << status;
        }
    } else {
        status = static_cast<QOpcUa::UaStatusCode>(res->responseHeader.serviceResult);
        qCDebug(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to add node:" << status;
    }

    emit backend->addNodeFinished(context.requestedNodeId, resultId, status);
}

void Open62541AsyncBackend::asyncDeleteNodeCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response)
{
    Q_UNUSED(client)

    Open62541AsyncBackend *backend = static_cast<Open62541AsyncBackend *>(userdata);
    const auto context = backend->m_asyncDeleteNodeContext.take(requestId);

    const auto res = static_cast<UA_DeleteNodesResponse *>(response);

    emit backend->deleteNodeFinished(context.nodeId,
                                     static_cast<QOpcUa::UaStatusCode>(res->resultsSize ?
                                                                           res->results[0] : res->responseHeader.serviceResult));
}

void Open62541AsyncBackend::asyncAddReferenceCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response)
{
    Q_UNUSED(client)

    Open62541AsyncBackend *backend = static_cast<Open62541AsyncBackend *>(userdata);
    const auto context = backend->m_asyncAddReferenceContext.take(requestId);

    const auto res = static_cast<UA_AddReferencesResponse *>(response);

    emit backend->addReferenceFinished(context.sourceNodeId, context.referenceTypeId, context.targetNodeId,
                                       context.isForwardReference,
                                       static_cast<QOpcUa::UaStatusCode>(res->resultsSize ?
                                                                             res->results[0] : res->responseHeader.serviceResult));
}

void Open62541AsyncBackend::asyncDeleteReferenceCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response)
{
    Q_UNUSED(client)

    Open62541AsyncBackend *backend = static_cast<Open62541AsyncBackend *>(userdata);
    const auto context = backend->m_asyncDeleteReferenceContext.take(requestId);

    const auto res = static_cast<UA_DeleteReferencesResponse *>(response);

    emit backend->deleteReferenceFinished(context.sourceNodeId, context.referenceTypeId,
                                          context.targetNodeId, context.isForwardReference,
                                          static_cast<QOpcUa::UaStatusCode>(res->resultsSize ?
                                                                                res->results[0] :
                                                                            res->responseHeader.serviceResult));
}

void Open62541AsyncBackend::asyncReadCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response)
{
    Q_UNUSED(client)

    Open62541AsyncBackend *backend = static_cast<Open62541AsyncBackend *>(userdata);
    auto context = backend->m_asyncReadContext.take(requestId);

    const auto res = static_cast<UA_ReadResponse *>(response);

    for (qsizetype i = 0; i < context.results.size(); ++i) {
        // Use the service result as status code if there is no specific result for the current value.
        // This ensures a result for each attribute when UA_Client_Service_read is called for a disconnected client.
        if (static_cast<size_t>(i) >= res->resultsSize) {
            context.results[i].setStatusCode(static_cast<QOpcUa::UaStatusCode>(res->responseHeader.serviceResult));
            continue;
        }
        if (res->results[i].hasStatus)
            context.results[i].setStatusCode(static_cast<QOpcUa::UaStatusCode>(res->results[i].status));
        else
            context.results[i].setStatusCode(QOpcUa::UaStatusCode::Good);
        if (res->results[i].hasValue && res->results[i].value.data)
                context.results[i].setValue(QOpen62541ValueConverter::toQVariant(res->results[i].value));
        if (res->results[i].hasSourceTimestamp)
            context.results[i].setSourceTimestamp(QOpen62541ValueConverter::scalarToQt<QDateTime, UA_DateTime>(&res->results[i].sourceTimestamp));
        if (res->results[i].hasServerTimestamp)
            context.results[i].setServerTimestamp(QOpen62541ValueConverter::scalarToQt<QDateTime, UA_DateTime>(&res->results[i].serverTimestamp));
    }

    emit backend->attributesRead(context.handle, context.results,
                                 static_cast<QOpcUa::UaStatusCode>(res->responseHeader.serviceResult));
}

void Open62541AsyncBackend::asyncWriteAttributesCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response)
{
    Q_UNUSED(client)

    Open62541AsyncBackend *backend = static_cast<Open62541AsyncBackend *>(userdata);
    auto context = backend->m_asyncWriteAttributesContext.take(requestId);

    const auto res = static_cast<UA_WriteResponse *>(response);

    size_t index = 0;
    for (auto it = context.toWrite.begin(); it != context.toWrite.end(); ++it, ++index) {
        QOpcUa::UaStatusCode status = index < res->resultsSize ?
                    static_cast<QOpcUa::UaStatusCode>(res->results[index]) :
                    static_cast<QOpcUa::UaStatusCode>(res->responseHeader.serviceResult);
        emit backend->attributeWritten(context.handle, it.key(), it.value(), status);
    }
}

void Open62541AsyncBackend::asyncBrowseCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response)
{
    Open62541AsyncBackend *backend = static_cast<Open62541AsyncBackend *>(userdata);
    auto context = backend->m_asyncBrowseContext.take(requestId);

    UA_StatusCode statusCode = UA_STATUSCODE_GOOD;
    size_t referencesSize = 0;
    UA_BrowseResult *references = nullptr;
    UA_ByteString *continuationPoint = nullptr;

    if (context.isBrowseNext) {
        const auto res = static_cast<UA_BrowseNextResponse *>(response);
        referencesSize = res->resultsSize ? res->results->referencesSize : 0;
        references = res->results;
        statusCode = referencesSize ? references->statusCode : res->responseHeader.serviceResult;
        continuationPoint = res->resultsSize ? &res->results->continuationPoint : nullptr;
    } else {
        const auto res = static_cast<UA_BrowseResponse *>(response);
        referencesSize = res->resultsSize ? res->results->referencesSize : 0;
        references = res->results;
        statusCode = referencesSize ? references->statusCode : res->responseHeader.serviceResult;
        continuationPoint = res->resultsSize ? &res->results->continuationPoint : nullptr;
    }

    convertBrowseResult(references, referencesSize, context.results);

    if (statusCode == UA_STATUSCODE_GOOD && continuationPoint && continuationPoint->length) {
        UA_BrowseNextRequest request;
        UA_BrowseNextRequest_init(&request);
        request.requestHeader.timeoutHint = backend->m_asyncRequestTimeout;
        UaDeleter<UA_BrowseNextRequest> requestDeleter(&request, UA_BrowseNextRequest_clear);

        request.continuationPointsSize = 1;
        request.continuationPoints = UA_ByteString_new();
        UA_ByteString_copy(continuationPoint, request.continuationPoints);

        quint32 requestId = 0;
        statusCode =__UA_Client_AsyncService(client, &request, &UA_TYPES[UA_TYPES_BROWSENEXTREQUEST], &asyncBrowseCallback,
                                   &UA_TYPES[UA_TYPES_BROWSENEXTRESPONSE], backend, &requestId);

        if (statusCode == UA_STATUSCODE_GOOD) {
            context.isBrowseNext = true;
            backend->m_asyncBrowseContext[requestId] = context;
            backend->triggerIterateClient();
            return;
        }
    } else if (statusCode != UA_STATUSCODE_GOOD) {
        emit backend->browseFinished(context.handle, QList<QOpcUaReferenceDescription>(),
                                     static_cast<QOpcUa::UaStatusCode>(statusCode));
        return;
    }

    emit backend->browseFinished(context.handle, context.results, static_cast<QOpcUa::UaStatusCode>(statusCode));
}

void Open62541AsyncBackend::asyncBatchReadCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response)
{
    Q_UNUSED(client)

    Open62541AsyncBackend *backend = static_cast<Open62541AsyncBackend *>(userdata);
    auto context = backend->m_asyncBatchReadContext.take(requestId);

    const auto res = static_cast<UA_ReadResponse *>(response);

    QOpcUa::UaStatusCode serviceResult = static_cast<QOpcUa::UaStatusCode>(res->responseHeader.serviceResult);

    if (serviceResult != QOpcUa::UaStatusCode::Good) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Batch read failed:" << serviceResult;
        emit backend->readNodeAttributesFinished(QList<QOpcUaReadResult>(), serviceResult);
    } else {
        QList<QOpcUaReadResult> ret;

        for (qsizetype i = 0; i < context.nodesToRead.size(); ++i) {
            QOpcUaReadResult item;
            item.setAttribute(context.nodesToRead.at(i).attribute());
            item.setNodeId(context.nodesToRead.at(i).nodeId());
            item.setIndexRange(context.nodesToRead.at(i).indexRange());
            if (static_cast<size_t>(i) < res->resultsSize) {
                if (res->results[i].hasServerTimestamp)
                    item.setServerTimestamp(QOpen62541ValueConverter::scalarToQt<QDateTime>(&res->results[i].serverTimestamp));
                if (res->results[i].hasSourceTimestamp)
                    item.setSourceTimestamp(QOpen62541ValueConverter::scalarToQt<QDateTime>(&res->results[i].sourceTimestamp));
                if (res->results[i].hasValue)
                    item.setValue(QOpen62541ValueConverter::toQVariant(res->results[i].value));
                if (res->results[i].hasStatus)
                    item.setStatusCode(static_cast<QOpcUa::UaStatusCode>(res->results[i].status));
                else
                    item.setStatusCode(serviceResult);
            } else {
                item.setStatusCode(serviceResult);
            }
            ret.push_back(item);
        }
        emit backend->readNodeAttributesFinished(ret, serviceResult);
    }
}

void Open62541AsyncBackend::asyncBatchWriteCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response)
{
    Q_UNUSED(client)

    Open62541AsyncBackend *backend = static_cast<Open62541AsyncBackend *>(userdata);
    const auto context = backend->m_asyncBatchWriteContext.take(requestId);

    const auto res = static_cast<UA_WriteResponse *>(response);

    QOpcUa::UaStatusCode serviceResult = QOpcUa::UaStatusCode(res->responseHeader.serviceResult);

    if (serviceResult != QOpcUa::UaStatusCode::Good) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Batch write failed:" << serviceResult;
        emit backend->writeNodeAttributesFinished(QList<QOpcUaWriteResult>(), serviceResult);
    } else {
        QList<QOpcUaWriteResult> ret;

        for (qsizetype i = 0; i < context.nodesToWrite.size(); ++i) {
            QOpcUaWriteResult item;
            item.setAttribute(context.nodesToWrite.at(i).attribute());
            item.setNodeId(context.nodesToWrite.at(i).nodeId());
            item.setIndexRange(context.nodesToWrite.at(i).indexRange());
            if (static_cast<size_t>(i) < res->resultsSize)
                item.setStatusCode(QOpcUa::UaStatusCode(res->results[i]));
            else
                item.setStatusCode(serviceResult);
            ret.push_back(item);
        }
        emit backend->writeNodeAttributesFinished(ret, serviceResult);
    }
}

void Open62541AsyncBackend::asyncReadHistoryDataCallBack(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response)
{
    Q_UNUSED(client);

    Open62541AsyncBackend *backend = static_cast<Open62541AsyncBackend *>(userdata);
    AsyncReadHistoryDataContext context = backend->m_asyncReadHistoryDataContext.take(requestId);

    UA_HistoryReadResponse* res = static_cast<UA_HistoryReadResponse*>(response);

    QList<QByteArray> continuationPoints;

    QList<QOpcUaHistoryData> historyData;

    for (size_t i = 0; i < res->resultsSize; ++i) {
        if (res->results[i].historyData.encoding != UA_EXTENSIONOBJECT_DECODED) {
            emit backend->historyDataAvailable({}, {}, QOpcUa::UaStatusCode(res->responseHeader.serviceResult), context.handle);
            return;
        }

        historyData.push_back(QOpcUaHistoryData(context.historyReadRawRequest.nodesToRead().at(i).nodeId()));

        historyData[i].setStatusCode(QOpcUa::UaStatusCode(res->results[i].statusCode));

        if (res->results[i].statusCode != UA_STATUSCODE_GOOD)
            continue;

        if (res->results[i].historyData.content.decoded.type != &UA_TYPES[UA_TYPES_HISTORYDATA]) {
            historyData[i].setStatusCode(QOpcUa::UaStatusCode::BadInternalError);
            continue;
        }

        UA_HistoryData *data = static_cast<UA_HistoryData *>(res->results[i].historyData.content.decoded.data);
        for (size_t j = 0; j < data->dataValuesSize; ++j) {
            const QOpcUaDataValue value = QOpen62541ValueConverter::scalarToQt<QOpcUaDataValue, UA_DataValue>(&data->dataValues[j]);
            historyData[i].addValue(value);
        }

        continuationPoints.push_back(QOpen62541ValueConverter::scalarToQt<QByteArray, UA_ByteString>(&res->results[i].continuationPoint));
    }

    emit backend->historyDataAvailable(historyData, continuationPoints, QOpcUa::UaStatusCode(res->responseHeader.serviceResult), context.handle);
}

void Open62541AsyncBackend::asyncRegisterNodesCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response)
{
    Q_UNUSED(client)

    Open62541AsyncBackend *backend = static_cast<Open62541AsyncBackend *>(userdata);
    const auto context = backend->m_asyncRegisterUnregisterNodesContext.take(requestId);

    const auto res = static_cast<UA_RegisterNodesResponse *>(response);

    const auto serviceResult = QOpcUa::UaStatusCode(res->responseHeader.serviceResult);

    if (serviceResult != QOpcUa::UaStatusCode::Good) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Register nodes failed:" << serviceResult;
        emit backend->registerNodesFinished(context.nodeIds, {}, serviceResult);
    } else {
        QStringList resultIds;
        for (size_t i = 0; i < res->registeredNodeIdsSize; ++i)
            resultIds.push_back(QOpen62541ValueConverter::scalarToQt<QString, UA_NodeId>(&res->registeredNodeIds[i]));

        emit backend->registerNodesFinished(context.nodeIds, resultIds, serviceResult);
    }
}

void Open62541AsyncBackend::asyncUnregisterNodesCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response)
{
    Q_UNUSED(client)

    Open62541AsyncBackend *backend = static_cast<Open62541AsyncBackend *>(userdata);
    const auto context = backend->m_asyncRegisterUnregisterNodesContext.take(requestId);

    const auto res = static_cast<UA_UnregisterNodesResponse *>(response);

    const auto serviceResult = QOpcUa::UaStatusCode(res->responseHeader.serviceResult);

    if (serviceResult != QOpcUa::UaStatusCode::Good)
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Unregister nodes failed:" << serviceResult;

    emit backend->unregisterNodesFinished(context.nodeIds, serviceResult);
}

void Open62541AsyncBackend::asyncReadHistoryEventsCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response)
{
    Q_UNUSED(client);

    Open62541AsyncBackend *backend = static_cast<Open62541AsyncBackend *>(userdata);
    auto context = backend->m_asyncReadHistoryEventsContext.take(requestId);

    auto res = static_cast<UA_HistoryReadResponse*>(response);

    QList<QByteArray> continuationPoints;

    QList<QOpcUaHistoryEvent> historyEvents;

    for (size_t i = 0; i < res->resultsSize; ++i) {
        if (res->results[i].historyData.encoding != UA_EXTENSIONOBJECT_DECODED) {
            emit backend->historyEventsAvailable({}, {}, QOpcUa::UaStatusCode(res->responseHeader.serviceResult), context.handle);
            return;
        }

        historyEvents.push_back(QOpcUaHistoryEvent(context.historyReadEventRequest.nodesToRead().at(i).nodeId()));

        historyEvents[i].setStatusCode(QOpcUa::UaStatusCode(res->results[i].statusCode));

        if (res->results[i].statusCode != UA_STATUSCODE_GOOD)
            continue;

        if (res->results[i].historyData.content.decoded.type != &UA_TYPES[UA_TYPES_HISTORYEVENT]) {
            historyEvents[i].setStatusCode(QOpcUa::UaStatusCode::BadInternalError);
            continue;
        }

        auto events = static_cast<UA_HistoryEvent *>(res->results[i].historyData.content.decoded.data);
        for (size_t j = 0; j < events->eventsSize; ++j) {
            QVariantList eventFields;
            for (size_t k = 0; k < events->events[j].eventFieldsSize; ++k)
                eventFields.push_back(QOpen62541ValueConverter::toQVariant(events->events[j].eventFields[k]));
            historyEvents.back().addEvent(eventFields);
        }

        continuationPoints.push_back(QOpen62541ValueConverter::scalarToQt<QByteArray, UA_ByteString>(&res->results[i].continuationPoint));
    }

    emit backend->historyEventsAvailable(historyEvents, continuationPoints, QOpcUa::UaStatusCode(res->responseHeader.serviceResult), context.handle);
}

bool Open62541AsyncBackend::loadFileToByteString(const QString &location, UA_ByteString *target) const
{
    if (location.isEmpty()) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Unable to read from empty file path";
        return false;
    }

    if (!target) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "No target ByteString given";
        return false;
    }

    UA_ByteString_init(target);

    QFile file(location);

    if (!file.open(QFile::ReadOnly)) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to open file" << location << file.errorString();
        return false;
    }

    QByteArray data = file.readAll();

    UA_ByteString temp;
    temp.length = data.size();
    if (data.isEmpty())
        temp.data = nullptr;
    else {
        if (data.startsWith('-')) { // PEM file
            // mbedTLS expects PEM encoded data to be null terminated
            data = data.append('\0');
            temp.length = data.size();
        }
        temp.data = reinterpret_cast<unsigned char *>(data.data());
    }

    return UA_ByteString_copy(&temp, target) == UA_STATUSCODE_GOOD;
}

bool Open62541AsyncBackend::loadAllFilesInDirectory(const QString &location, UA_ByteString **target, qsizetype *size) const
{
    if (location.isEmpty()) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Unable to read from empty file path";
        return false;
    }

    if (!target) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "No target ByteString given";
        return false;
    }

    *target = nullptr;
    *size = 0;

    QDir dir(location);

    auto entries = dir.entryList(QDir::Files);

    if (entries.isEmpty()) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Directory is empty";
        return true;
    }

    const qsizetype tempSize = entries.size();
    UA_ByteString *list = static_cast<UA_ByteString *>(UA_Array_new(tempSize, &UA_TYPES[UA_TYPES_BYTESTRING]));

    if (!list) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to allocate memory for loading files in" << location;
        return false;
    }

    for (qsizetype i = 0; i < entries.size(); ++i) {
        if (!loadFileToByteString(dir.filePath(entries.at(i)), &list[i])) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to open file" << entries.at(i);
            UA_Array_delete(list, tempSize, &UA_TYPES[UA_TYPES_BYTESTRING]);
            size = 0;
            *target = nullptr;
            return false;
        }
    }

    *target = list;
    *size = tempSize;

    return true;
}

void Open62541AsyncBackend::disconnectInternal(QOpcUaClient::ClientError error)
{
    m_clientIterateTimer.stop();
    m_clientIterateOnDemandTimer.stop();

    if (m_uaclient) {
        // Disable the state callback, we will emit stateAndOrErrorChanged() here
        UA_Client_getConfig(m_uaclient)->stateCallback = nullptr;
        UA_Client_getConfig(m_uaclient)->inactivityCallback = nullptr;
    }

    cleanupSubscriptions();

    if (m_uaclient) {
        UA_Client_disconnect(m_uaclient);
        UA_Client_delete(m_uaclient);
        m_uaclient = nullptr;
        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, error);
    }
}

#ifdef UA_ENABLE_ENCRYPTION
bool Open62541AsyncBackend::setupClientConfigSecurity(const QOpcUaAuthenticationInformation &authInfo,
                                                      const QOpcUaPkiConfiguration &pkiConfig,
                                                      const QOpcUaEndpointDescription &endpoint)
{
    const auto conf = UA_Client_getConfig(m_uaclient);
    if (!conf)
        return false;

    UA_ByteString privateKey = UA_BYTESTRING_NULL;
    UaDeleter<UA_ByteString> privateKeyDeleter(&privateKey, &UA_ByteString_clear);

    if (pkiConfig.isKeyAndCertificateFileSet()) {
        UA_ByteString localCertificate;
        UA_ByteString *trustList = nullptr;
        qsizetype trustListSize = 0;
        UA_ByteString *revocationList = nullptr;
        qsizetype revocationListSize = 0;

        bool success = loadFileToByteString(pkiConfig.clientCertificateFile(), &localCertificate);

        if (!success) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to load client certificate";
            emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::InvalidPki);
            UA_Client_delete(m_uaclient);
            m_uaclient = nullptr;
            return false;
        }

        UaDeleter<UA_ByteString> clientCertDeleter(&localCertificate, &UA_ByteString_clear);

        success = loadPrivateKeyWithPotentialPassword(pkiConfig.privateKeyFile(), privateKey);

        if (!success) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to load private key";
            emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::InvalidPki);
            UA_Client_delete(m_uaclient);
            m_uaclient = nullptr;
            return false;
        }

        if (!pkiConfig.trustListDirectory().isEmpty()) {
            success = loadAllFilesInDirectory(pkiConfig.trustListDirectory(), &trustList, &trustListSize);

            if (!success) {
                qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to load trust list";
                emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::InvalidPki);
                UA_Client_delete(m_uaclient);
                m_uaclient = nullptr;
                return false;
            }
        }

        UaArrayDeleter<UA_TYPES_BYTESTRING> trustListDeleter(trustList, trustListSize);

        if (!pkiConfig.revocationListDirectory().isEmpty()) {
            success = loadAllFilesInDirectory(pkiConfig.revocationListDirectory(), &revocationList, &revocationListSize);

            if (!success) {
                qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to load revocation list";
                emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::InvalidPki);
                UA_Client_delete(m_uaclient);
                m_uaclient = nullptr;
                return false;
            }
        }

        UaArrayDeleter<UA_TYPES_BYTESTRING> revocationListDeleter(revocationList, revocationListSize);

        // UA_ClientConfig_setDefaultEncryption() no longer adds Basic128Rsa15 and Basic256 to the security policies in v1.4.7
        // This must be done manually (for now).
        UA_StatusCode result = UA_ClientConfig_setDefault(conf);

        if (result != UA_STATUSCODE_GOOD) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to set client config defaults:" << static_cast<QOpcUa::UaStatusCode>(result);
            emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::UnknownError);
            UA_Client_delete(m_uaclient);
            m_uaclient = nullptr;
            return false;
        }

        if (conf->certificateVerification.clear)
            conf->certificateVerification.clear(&conf->certificateVerification);
        result = UA_CertificateVerification_Trustlist(&conf->certificateVerification,
                                                      trustList, trustListSize,
                                                      nullptr, 0,
                                                      revocationList, revocationListSize);

        if (result != UA_STATUSCODE_GOOD) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to set up certificate verification:" << static_cast<QOpcUa::UaStatusCode>(result);
            emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::InvalidPki);
            UA_Client_delete(m_uaclient);
            m_uaclient = nullptr;
            return false;
        }

        result = setAuthSecurityPolicyInClientConfig(conf, localCertificate, privateKey, endpoint, authInfo.authenticationType());

        if (result != UA_STATUSCODE_GOOD) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to set up auth security policies:" << static_cast<QOpcUa::UaStatusCode>(result);
            emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::InvalidPki);
            UA_Client_delete(m_uaclient);
            m_uaclient = nullptr;
            return false;
        }

        result = setSecurityPolicyInClientConfig(conf, localCertificate, privateKey, endpoint);

        if (result != UA_STATUSCODE_GOOD) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to set up security policies:" << static_cast<QOpcUa::UaStatusCode>(result);
            emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::InvalidPki);
            UA_Client_delete(m_uaclient);
            m_uaclient = nullptr;
            return false;
        }
    } else {
        return UA_ClientConfig_setDefault(conf) == UA_STATUSCODE_GOOD;
    }

    return true;
}
#endif

void Open62541AsyncBackend::establishConnectionInternal(const QOpcUaAuthenticationInformation &authInfo,
#ifdef UA_ENABLE_ENCRYPTION
                                            const QOpcUaPkiConfiguration &pkiConfig,
#endif
                                            const QOpcUaEndpointDescription &endpoint)
{
    const auto conf = UA_Client_getConfig(m_uaclient);
    if (!conf)
        return;

    UA_StatusCode ret = UA_STATUSCODE_BADINTERNALERROR;
    bool retry = false;

    do {
        retry = false;

        if (authInfo.authenticationType() == QOpcUaUserTokenPolicy::TokenType::Anonymous) {
            // Nothing to do, Anonymous is default
        } else if (authInfo.authenticationType() == QOpcUaUserTokenPolicy::TokenType::Username) {
            const auto credentials = authInfo.authenticationData().value<QPair<QString, QString>>();
            UA_ClientConfig_setAuthenticationUsername(conf, credentials.first.toUtf8().constData(),
                                                      credentials.second.toUtf8().constData());
        } else if (authInfo.authenticationType() == QOpcUaUserTokenPolicy::TokenType::Certificate) {
#ifdef UA_ENABLE_ENCRYPTION
            QString certPath;
            QString keyPath;

            if (authInfo.authenticationData().canConvert<QPair<QString, QString>>()) {
                const auto authPaths = authInfo.authenticationData().value<QPair<QString, QString>>();

                if (authPaths.first.isEmpty() || authPaths.second.isEmpty()) {
                    qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Certificate and private key path must be set for certificate auth";
                    emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::ClientError::InvalidAuthenticationInformation);
                    UA_Client_delete(m_uaclient);
                    m_uaclient = nullptr;
                    return;
                }

                certPath = authPaths.first;
                keyPath = authPaths.second;
            } else {
                certPath = pkiConfig.clientCertificateFile();
                keyPath = pkiConfig.privateKeyFile();
            }

            UA_ByteString cert = UA_BYTESTRING_NULL;
            UA_ByteString key = UA_BYTESTRING_NULL;

            if (!loadFileToByteString(certPath, &cert)) {
                qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to load certificate for certificate auth";
                emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::ClientError::InvalidAuthenticationInformation);
                UA_Client_delete(m_uaclient);
                m_uaclient = nullptr;
                return;
            }

            UaDeleter<UA_ByteString> certDeleter(&cert, &UA_ByteString_clear);

            if (!loadPrivateKeyWithPotentialPassword(keyPath, key)) {
                qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to load private key for certificate auth";
                emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::ClientError::InvalidAuthenticationInformation);
                UA_Client_delete(m_uaclient);
                m_uaclient = nullptr;
                return;
            }

            UaDeleter<UA_ByteString> keyDeleter(&key, &UA_ByteString_clear);

            // UA_ClientConfig_setAuthenticationCert() overwrites the auth security policies,
            // so we must call our custom implementation

            UA_X509IdentityToken *token = UA_X509IdentityToken_new();
            UA_StatusCode result = UA_ByteString_copy(&cert, &token->certificateData);
            UA_ExtensionObject_clear(&conf->userIdentityToken);
            conf->userIdentityToken.encoding = UA_EXTENSIONOBJECT_DECODED;
            conf->userIdentityToken.content.decoded.type = &UA_TYPES[UA_TYPES_X509IDENTITYTOKEN];
            conf->userIdentityToken.content.decoded.data = token;

            if (result != UA_STATUSCODE_GOOD) {
                qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to initialize certificate auth:" << UA_StatusCode_name(result);
                emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::ClientError::UnsupportedAuthenticationInformation);
                UA_Client_delete(m_uaclient);
                m_uaclient = nullptr;
                return;
            }
#endif
        } else {
            emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::UnsupportedAuthenticationInformation);
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to connect: Selected authentication type"
                                                  << authInfo.authenticationType() << "is not supported.";
            UA_Client_delete(m_uaclient);
            m_uaclient = nullptr;
            return;
        }

        ret = UA_Client_connect(m_uaclient, endpoint.endpointUrl().toUtf8().constData());

#ifdef UA_ENABLE_ENCRYPTION
        if (ret == UA_STATUSCODE_BADCERTIFICATEUNTRUSTED) {
            QOpcUaErrorState errorState;
            errorState.setClientSideError(true);
            errorState.setConnectionStep(QOpcUaErrorState::ConnectionStep::CertificateValidation);
            errorState.setErrorCode(QOpcUa::UaStatusCode::BadCertificateUntrusted);

            emit QOpcUaBackend::connectError(&errorState);

            if (errorState.ignoreError()) {
                // Use the AcceptAll certificate verification
                UA_CertificateVerification_AcceptAll(&conf->certificateVerification);
                retry = true;
            }
        }
#endif
    } while (retry);

    if (ret != UA_STATUSCODE_GOOD) {
        UA_Client_delete(m_uaclient);
        m_uaclient = nullptr;
        QOpcUaClient::ClientError error = QOpcUaClient::UnknownError;
        switch (ret) {
        case UA_STATUSCODE_BADUSERACCESSDENIED:
        case UA_STATUSCODE_BADIDENTITYTOKENINVALID: // Intentional fall through
            error = QOpcUaClient::AccessDenied;
            break;
        case UA_STATUSCODE_BADCERTIFICATEUNTRUSTED:
            error = QOpcUaClient::CertificateUntrusted;
            break;
        default:
            break;
        }

        QOpcUaErrorState errorState;
        errorState.setConnectionStep(QOpcUaErrorState::ConnectionStep::Unknown);
        errorState.setErrorCode(static_cast<QOpcUa::UaStatusCode>(ret));
        errorState.setClientSideError(false);
        errorState.setIgnoreError(false);

        // This signal is connected using Qt::BlockingQueuedConnection. It will place a metacall to a different thread and waits
        // until this metacall is fully handled before returning.
        emit QOpcUaBackend::connectError(&errorState);

        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, error);
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Open62541: Failed to connect";
        return;
    }

    using Timeout_t = decltype(conf->timeout);
    conf->timeout = qt_saturate<Timeout_t>(m_currentConnectionSettings.requestTimeout().count());

    // Attach the client state callback after the successful connect
    conf->stateCallback = clientStateCallback;
    conf->noReconnect = true;

    m_clientIterateTimer.start(m_clientIterateInterval);
    emit stateAndOrErrorChanged(QOpcUaClient::Connected, QOpcUaClient::NoError);
}

#ifdef UA_ENABLE_ENCRYPTION
bool Open62541AsyncBackend::loadPrivateKeyWithPotentialPassword(const QString &privateKeyPath, UA_ByteString &privateKey)
{
    UA_ByteString privateKeyData = UA_BYTESTRING_NULL;
    const auto guard = qScopeGuard([&privateKeyData]() {
        UA_ByteString_clear(&privateKeyData);
    });

    const auto success = loadFileToByteString(privateKeyPath, &privateKeyData);

    if (!success)
        return false;

    UA_StatusCode decryptResult = UA_STATUSCODE_BADINTERNALERROR;
    QString password;
    bool previousTryWasInvalid = false;
    do {
        auto uaPassword = UA_STRING_ALLOC(password.toUtf8().constData());
        decryptResult = UA_PKI_decryptPrivateKey(privateKeyData, uaPassword, &privateKey);
        UA_String_clear(&uaPassword);

        // The key was already DER or had no password
        if (decryptResult == UA_STATUSCODE_GOOD)
            return true;

        // Decoding failed, a password may be required or the given password was invalid
        if (decryptResult == UA_STATUSCODE_BADSECURITYCHECKSFAILED) {
            emit passwordForPrivateKeyRequired(privateKeyPath, &password, previousTryWasInvalid);

            // No further attempt requested
            if (password.isEmpty())
                return false;

            previousTryWasInvalid = true;
        }
    } while (decryptResult == UA_STATUSCODE_BADSECURITYCHECKSFAILED);

    return false;
}

// Only add the security policy the requested endpoint requires
UA_StatusCode Open62541AsyncBackend::setSecurityPolicyInClientConfig(UA_ClientConfig *conf, const UA_ByteString &cert, const UA_ByteString &key,
                                                                     const QOpcUaEndpointDescription &desc)
{
    if (desc.securityPolicy() == QOpcUa::NonePolicy)
        return UA_STATUSCODE_GOOD;

    const size_t numPolicies = conf->securityPoliciesSize + 1;
    conf->securityPolicies = static_cast<UA_SecurityPolicy *>(UA_realloc(conf->securityPolicies, sizeof(UA_SecurityPolicy) * numPolicies));

    UA_StatusCode result = UA_STATUSCODE_GOOD;

    if (desc.securityPolicy() == QOpcUa::Basic128Rsa15Policy)
        result = UA_SecurityPolicy_Basic128Rsa15(&conf->securityPolicies[1],
                                                 cert, key, conf->logging);
    else if (desc.securityPolicy() == QOpcUa::Basic256Policy)
        result = UA_SecurityPolicy_Basic256(&conf->securityPolicies[1],
                                            cert, key, conf->logging);
    else if (desc.securityPolicy() == QOpcUa::Aes256Sha256RsaPssPolicy)
        result = UA_SecurityPolicy_Aes256Sha256RsaPss(&conf->securityPolicies[1],
                                                      cert, key, conf->logging);
    else if (desc.securityPolicy() == QOpcUa::Basic256Sha256Policy)
        result = UA_SecurityPolicy_Basic256Sha256(&conf->securityPolicies[1],
                                                  cert, key, conf->logging);
    else if (desc.securityPolicy() == QOpcUa::Aes128Sha256RsaOaepPolicy)
        result = UA_SecurityPolicy_Aes128Sha256RsaOaep(&conf->securityPolicies[1],
                                                       cert, key, conf->logging);
    else
        result = UA_STATUSCODE_BADINVALIDARGUMENT;

    if (result != UA_STATUSCODE_GOOD) {
        // UA_ClientConfig_clear() doesn't check for a valid clear() pointer on the policy
        // Don't increment securityPoliciesSize in case of an error
        return result;
    } else {
        conf->securityPoliciesSize = numPolicies;
    }

    return result;
}

// Find the most secure security policy a token of the required type supports
UA_StatusCode Open62541AsyncBackend::setAuthSecurityPolicyInClientConfig(UA_ClientConfig *conf, const UA_ByteString &cert,
                                                                         const UA_ByteString &key,
                                                                         const QOpcUaEndpointDescription &desc,
                                                                         QOpcUaUserTokenPolicy::TokenType tokenType)
{
    // No None policy for auth, but all encrypting policies
    const size_t numPolicies = 1;

    for (size_t i = 0; i < conf->authSecurityPoliciesSize; i++) {
        conf->authSecurityPolicies[i].clear(&conf->authSecurityPolicies[i]);
    }
    UA_free(conf->authSecurityPolicies);
    conf->authSecurityPolicies = nullptr;
    conf->authSecurityPoliciesSize = 0;

    UA_StatusCode result = UA_STATUSCODE_BADINVALIDARGUMENT;

    QString selectedPolicy;
    int lastIndex = -1;
    const auto tempTokens = desc.userIdentityTokens();
    for (const auto &token : tempTokens) {
        if (token.tokenType() != tokenType)
            continue;

        auto currentPolicy = token.securityPolicy();
        if (currentPolicy.isEmpty())
            currentPolicy = desc.securityPolicy();

        // Assumes that supportedSecurityPolicies() is sorted by strength
        // The supportedSecurityPolicies() has been modified accordingly.
        const auto index = m_clientImpl->supportedSecurityPolicies().indexOf(currentPolicy);
        if (index > lastIndex) {
            lastIndex = index;
            selectedPolicy = currentPolicy;
        }
    }

    if (!selectedPolicy.isEmpty()) {
        if (selectedPolicy == QOpcUa::NonePolicy)
            return UA_STATUSCODE_GOOD;

        conf->authSecurityPolicies = static_cast<UA_SecurityPolicy*>(UA_realloc(conf->authSecurityPolicies,
                                                                                sizeof(UA_SecurityPolicy) * numPolicies));
        conf->authSecurityPoliciesSize = numPolicies;

        if (selectedPolicy == QOpcUa::Basic128Rsa15Policy)
            result = UA_SecurityPolicy_Basic128Rsa15(&conf->authSecurityPolicies[0],
                                                     cert, key, conf->logging);
        else if (selectedPolicy == QOpcUa::Basic256Policy)
            result = UA_SecurityPolicy_Basic256(&conf->authSecurityPolicies[0],
                                                cert, key, conf->logging);
        else if (selectedPolicy == QOpcUa::Aes256Sha256RsaPssPolicy)
            result = UA_SecurityPolicy_Aes256Sha256RsaPss(&conf->authSecurityPolicies[0],
                                                          cert, key, conf->logging);
        else if (selectedPolicy == QOpcUa::Basic256Sha256Policy)
            result = UA_SecurityPolicy_Basic256Sha256(&conf->authSecurityPolicies[0],
                                                      cert, key, conf->logging);
        else if (selectedPolicy == QOpcUa::Aes128Sha256RsaOaepPolicy)
            result = UA_SecurityPolicy_Aes128Sha256RsaOaep(&conf->authSecurityPolicies[0],
                                                           cert, key, conf->logging);

        if (result != UA_STATUSCODE_GOOD) {
            // UA_ClientConfig_clear() doesn't check for a valid clear() pointer on the policy
            conf->authSecurityPoliciesSize = 0;
            UA_free(conf->authSecurityPolicies);
            conf->authSecurityPolicies = nullptr;
        }
    }

    return result;
}
#endif

UA_ExtensionObject Open62541AsyncBackend::assembleNodeAttributes(const QOpcUaNodeCreationAttributes &nodeAttributes,
                                                                 QOpcUa::NodeClass nodeClass)
{
    UA_ExtensionObject obj;
    UA_ExtensionObject_init(&obj);
    obj.encoding = UA_EXTENSIONOBJECT_DECODED;

    switch (nodeClass) {
    case QOpcUa::NodeClass::Object: {
        UA_ObjectAttributes *attr = UA_ObjectAttributes_new();
        *attr = UA_ObjectAttributes_default;
        obj.content.decoded.data = attr;
        obj.content.decoded.type = &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES];

        if (nodeAttributes.hasEventNotifier()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_EVENTNOTIFIER;
            attr->eventNotifier = nodeAttributes.eventNotifier();
        }
        break;
    }
    case QOpcUa::NodeClass::Variable: {
        UA_VariableAttributes *attr = UA_VariableAttributes_new();
        *attr = UA_VariableAttributes_default;
        obj.content.decoded.data = attr;
        obj.content.decoded.type = &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES];

        if (nodeAttributes.hasValue()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_VALUE;
            attr->value = QOpen62541ValueConverter::toOpen62541Variant(nodeAttributes.value(),
                                                                       nodeAttributes.valueType());
        }
        if (nodeAttributes.hasDataTypeId()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_DATATYPE;
            attr->dataType = Open62541Utils::nodeIdFromQString(nodeAttributes.dataTypeId());
        }
        if (nodeAttributes.hasValueRank()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_VALUERANK;
            attr->valueRank = nodeAttributes.valueRank();
        }
        if (nodeAttributes.hasArrayDimensions()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_ARRAYDIMENSIONS;
            attr->arrayDimensions = copyArrayDimensions(nodeAttributes.arrayDimensions(), &attr->arrayDimensionsSize);
        }
        if (nodeAttributes.hasAccessLevel()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_ACCESSLEVEL;
            attr->accessLevel = nodeAttributes.accessLevel();
        }
        if (nodeAttributes.hasUserAccessLevel()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_USERACCESSLEVEL;
            attr->userAccessLevel = nodeAttributes.userAccessLevel();
        }
        if (nodeAttributes.hasMinimumSamplingInterval()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_MINIMUMSAMPLINGINTERVAL;
            attr->minimumSamplingInterval = nodeAttributes.minimumSamplingInterval();
        }
        if (nodeAttributes.hasHistorizing()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_HISTORIZING;
            attr->historizing = nodeAttributes.historizing();
        }
        break;
    }
    case QOpcUa::NodeClass::Method: {
        UA_MethodAttributes *attr = UA_MethodAttributes_new();
        *attr = UA_MethodAttributes_default;
        obj.content.decoded.data = attr;
        obj.content.decoded.type = &UA_TYPES[UA_TYPES_METHODATTRIBUTES];

        if (nodeAttributes.hasExecutable()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_EXECUTABLE;
            attr->executable = nodeAttributes.executable();
        }
        if (nodeAttributes.hasUserExecutable()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_USEREXECUTABLE;
            attr->userExecutable = nodeAttributes.userExecutable();
        }
        break;
    }
    case QOpcUa::NodeClass::ObjectType: {
        UA_ObjectTypeAttributes *attr = UA_ObjectTypeAttributes_new();
        *attr = UA_ObjectTypeAttributes_default;
        obj.content.decoded.data = attr;
        obj.content.decoded.type = &UA_TYPES[UA_TYPES_OBJECTTYPEATTRIBUTES];

        if (nodeAttributes.hasIsAbstract()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_ISABSTRACT;
            attr->isAbstract = nodeAttributes.isAbstract();
        }
        break;
    }
    case QOpcUa::NodeClass::VariableType: {
        UA_VariableTypeAttributes *attr = UA_VariableTypeAttributes_new();
        *attr = UA_VariableTypeAttributes_default;
        obj.content.decoded.data = attr;
        obj.content.decoded.type = &UA_TYPES[UA_TYPES_VARIABLETYPEATTRIBUTES];

        if (nodeAttributes.hasValue()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_VALUE;
            attr->value = QOpen62541ValueConverter::toOpen62541Variant(nodeAttributes.value(),
                                                                       nodeAttributes.valueType());
        }
        if (nodeAttributes.hasDataTypeId()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_DATATYPE;
            attr->dataType = Open62541Utils::nodeIdFromQString(nodeAttributes.dataTypeId());
        }
        if (nodeAttributes.hasValueRank()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_VALUERANK;
            attr->valueRank = nodeAttributes.valueRank();
        }
        if (nodeAttributes.hasArrayDimensions()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_ARRAYDIMENSIONS;
            attr->arrayDimensions = copyArrayDimensions(nodeAttributes.arrayDimensions(), &attr->arrayDimensionsSize);
        }
        if (nodeAttributes.hasIsAbstract()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_ISABSTRACT;
            attr->isAbstract = nodeAttributes.isAbstract();
        }
        break;
    }
    case QOpcUa::NodeClass::ReferenceType: {
        UA_ReferenceTypeAttributes *attr = UA_ReferenceTypeAttributes_new();
        *attr = UA_ReferenceTypeAttributes_default;
        obj.content.decoded.data = attr;
        obj.content.decoded.type = &UA_TYPES[UA_TYPES_REFERENCETYPEATTRIBUTES];

        if (nodeAttributes.hasIsAbstract()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_ISABSTRACT;
            attr->isAbstract = nodeAttributes.isAbstract();
        }
        if (nodeAttributes.hasSymmetric()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_SYMMETRIC;
            attr->symmetric = nodeAttributes.symmetric();
        }
        if (nodeAttributes.hasInverseName()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_INVERSENAME;
            QOpen62541ValueConverter::scalarFromQt<UA_LocalizedText, QOpcUaLocalizedText>(
                        nodeAttributes.inverseName(), &attr->inverseName);
        }
        break;
    }
    case QOpcUa::NodeClass::DataType: {
        UA_DataTypeAttributes *attr = UA_DataTypeAttributes_new();
        *attr = UA_DataTypeAttributes_default;
        obj.content.decoded.data = attr;
        obj.content.decoded.type = &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES];

        if (nodeAttributes.hasIsAbstract()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_ISABSTRACT;
            attr->isAbstract = nodeAttributes.isAbstract();
        }
        break;
    }
    case QOpcUa::NodeClass::View: {
        UA_ViewAttributes *attr = UA_ViewAttributes_new();
        *attr = UA_ViewAttributes_default;
        obj.content.decoded.data = attr;
        obj.content.decoded.type = &UA_TYPES[UA_TYPES_VIEWATTRIBUTES];

        if (nodeAttributes.hasContainsNoLoops()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_CONTAINSNOLOOPS;
            attr->containsNoLoops = nodeAttributes.containsNoLoops();
        }
        if (nodeAttributes.hasEventNotifier()) {
            attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_EVENTNOTIFIER;
            attr->eventNotifier = nodeAttributes.eventNotifier();
        }
        break;
    }
    default:
        qCDebug(QT_OPCUA_PLUGINS_OPEN62541) << "Could not convert node attributes, unknown node class";
        UA_ExtensionObject_init(&obj);
        return obj;
    }

    UA_ObjectAttributes *attr = reinterpret_cast<UA_ObjectAttributes *>(obj.content.decoded.data);
    if (nodeAttributes.hasDisplayName()) {
        attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_DISPLAYNAME;
        QOpen62541ValueConverter::scalarFromQt<UA_LocalizedText, QOpcUaLocalizedText>(
                    nodeAttributes.displayName(), &attr->displayName);
    }
    if (nodeAttributes.hasDescription()) {
        attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_DESCRIPTION;
        QOpen62541ValueConverter::scalarFromQt<UA_LocalizedText, QOpcUaLocalizedText>(
                    nodeAttributes.description(), &attr->description);
    }
    if (nodeAttributes.hasWriteMask()) {
        attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_WRITEMASK;
        attr->writeMask = nodeAttributes.writeMask();
    }
    if (nodeAttributes.hasUserWriteMask()) {
        attr->specifiedAttributes |= UA_NODEATTRIBUTESMASK_USERWRITEMASK;
        attr->userWriteMask = nodeAttributes.userWriteMask();
    }

    return obj;
}

UA_UInt32 *Open62541AsyncBackend::copyArrayDimensions(const QList<quint32> &arrayDimensions, size_t *outputSize)
{
    if (outputSize)
        *outputSize = arrayDimensions.size();

    if (!outputSize)
        return nullptr;

    UA_UInt32 *data = nullptr;
    UA_StatusCode res = UA_Array_copy(arrayDimensions.constData(), arrayDimensions.size(),
                                      reinterpret_cast<void **>(&data), &UA_TYPES[UA_TYPES_UINT32]);
    return res == UA_STATUSCODE_GOOD ? data : nullptr;
}

QT_END_NAMESPACE

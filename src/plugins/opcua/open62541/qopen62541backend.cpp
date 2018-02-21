/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qopen62541backend.h"
#include "qopen62541node.h"
#include "qopen62541utils.h"
#include "qopen62541valueconverter.h"
#include <private/qopcuaclient_p.h>

#include <QtCore/qloggingcategory.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qurl.h>
#include <QtCore/quuid.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_OPEN62541)

struct UaVariantMemberDeleter
{
    static void cleanup(UA_Variant *p) { UA_Variant_deleteMembers(p); }
};

struct UaLocalizedTextMemberDeleter
{
    static void cleanup(UA_LocalizedText *p) { UA_LocalizedText_deleteMembers(p); }
};

Open62541AsyncBackend::Open62541AsyncBackend(QOpen62541Client *parent)
    : QOpcUaBackend()
    , m_uaclient(nullptr)
    , m_clientImpl(parent)
    , m_useStateCallback(false)
    , m_subscriptionTimer(this)
    , m_sendPublishRequests(false)
{
    m_subscriptionTimer.setSingleShot(true);
    QObject::connect(&m_subscriptionTimer, &QTimer::timeout,
                     this, &Open62541AsyncBackend::sendPublishRequest);
}

Open62541AsyncBackend::~Open62541AsyncBackend()
{
    qDeleteAll(m_subscriptions);
}

void Open62541AsyncBackend::readAttributes(uintptr_t handle, UA_NodeId id, QOpcUa::NodeAttributes attr, QString indexRange)
{
    UA_ReadRequest req;
    UA_ReadRequest_init(&req);
    QVector<UA_ReadValueId> valueIds;

    UA_ReadValueId readId;
    UA_ReadValueId_init(&readId);
    readId.nodeId = id;

    QVector<QOpcUaReadResult> vec;

    qt_forEachAttribute(attr, [&](QOpcUa::NodeAttribute attribute){
        readId.attributeId = QOpen62541ValueConverter::toUaAttributeId(attribute);
        if (indexRange.length())
            readId.indexRange = UA_STRING_ALLOC(indexRange.toUtf8().data());
        valueIds.push_back(readId);
        QOpcUaReadResult temp;
        temp.attributeId = attribute;
        vec.push_back(temp);
    });

    UA_ReadResponse res;
    UA_ReadResponse_init(&res);
    req.nodesToRead = valueIds.data();
    req.nodesToReadSize = valueIds.size();
    req.timestampsToReturn = UA_TIMESTAMPSTORETURN_BOTH;

    res = UA_Client_Service_read(m_uaclient, req);

    for (int i = 0; i < vec.size(); ++i) {
        // Use the service result as status code if there is no specific result for the current value.
        // This ensures a result for each attribute when UA_Client_Service_read is called for a disconnected client.
        if (static_cast<size_t>(i) >= res.resultsSize) {
            vec[i].statusCode = static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult);
            continue;
        }
        if (res.results[i].hasStatus)
            vec[i].statusCode = static_cast<QOpcUa::UaStatusCode>(res.results[i].status);
        else
            vec[i].statusCode = QOpcUa::UaStatusCode::Good;
        if (res.results[i].hasValue && res.results[i].value.data)
                vec[i].value = QOpen62541ValueConverter::toQVariant(res.results[i].value);
        if (res.results[i].hasServerTimestamp)
            vec[i].sourceTimestamp = QOpen62541ValueConverter::uaDateTimeToQDateTime(res.results[i].sourceTimestamp);
        if (res.results[i].hasSourceTimestamp)
            vec[i].serverTimestamp = QOpen62541ValueConverter::uaDateTimeToQDateTime(res.results[i].serverTimestamp);
    }
    emit attributesRead(handle, vec, static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult));
    UA_ReadValueId_deleteMembers(&readId);
    UA_ReadResponse_deleteMembers(&res);
}

void Open62541AsyncBackend::writeAttribute(uintptr_t handle, UA_NodeId id, QOpcUa::NodeAttribute attrId, QVariant value, QOpcUa::Types type, QString indexRange)
{
    if (type == QOpcUa::Types::Undefined && attrId != QOpcUa::NodeAttribute::Value)
        type = attributeIdToTypeId(attrId);

    UA_WriteRequest req;
    UA_WriteRequest_init(&req);
    req.nodesToWriteSize = 1;
    req.nodesToWrite = UA_WriteValue_new();

    UA_WriteValue_init(req.nodesToWrite);
    req.nodesToWrite->attributeId = QOpen62541ValueConverter::toUaAttributeId(attrId);
    req.nodesToWrite->nodeId = id;
    req.nodesToWrite->value.value = QOpen62541ValueConverter::toOpen62541Variant(value, type);
    req.nodesToWrite->value.hasValue = true;
    if (indexRange.length())
        req.nodesToWrite->indexRange = UA_STRING_ALLOC(indexRange.toUtf8().data());

    UA_WriteResponse res = UA_Client_Service_write(m_uaclient, req);

    QOpcUa::UaStatusCode status = res.resultsSize ?
                static_cast<QOpcUa::UaStatusCode>(res.results[0]) : static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult);

    emit attributeWritten(handle, attrId, value, status);

    UA_WriteRequest_deleteMembers(&req);
    UA_WriteResponse_deleteMembers(&res);
}

void Open62541AsyncBackend::writeAttributes(uintptr_t handle, UA_NodeId id, QOpcUaNode::AttributeMap toWrite, QOpcUa::Types valueAttributeType)
{
    if (toWrite.size() == 0) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "No values to be written";
        emit attributeWritten(handle, QOpcUa::NodeAttribute::None, QVariant(), QOpcUa::UaStatusCode::BadNothingToDo);
        return;
    }

    UA_WriteRequest req;
    UA_WriteRequest_init(&req);
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
    UA_WriteResponse res = UA_Client_Service_write(m_uaclient, req);

    index = 0;
    for (auto it = toWrite.begin(); it != toWrite.end(); ++it, ++index) {
        QOpcUa::UaStatusCode status = index < res.resultsSize ?
                    static_cast<QOpcUa::UaStatusCode>(res.results[index]) : static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult);
        emit attributeWritten(handle, it.key(), it.value(), status);
    }

    UA_WriteRequest_deleteMembers(&req);
    UA_WriteResponse_deleteMembers(&res);
    UA_NodeId_deleteMembers(&id);
}

void Open62541AsyncBackend::enableMonitoring(uintptr_t handle, UA_NodeId id, QOpcUa::NodeAttributes attr, const QOpcUaMonitoringParameters &settings)
{
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

    modifyPublishRequests();
}

void Open62541AsyncBackend::disableMonitoring(uintptr_t handle, QOpcUa::NodeAttributes attr)
{
    qt_forEachAttribute(attr, [&](QOpcUa::NodeAttribute attribute){
        QOpen62541Subscription *sub = getSubscriptionForItem(handle, attribute);
        if (sub) {
            sub->removeAttributeMonitoredItem(handle, attribute);
            m_attributeMapping[handle].remove(attribute);
            if (sub->monitoredItemsCount() == 0)
                removeSubscription(sub->subscriptionId());
        }
    });
    modifyPublishRequests();
}

void Open62541AsyncBackend::modifyMonitoring(uintptr_t handle, QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, QVariant value)
{
    QOpen62541Subscription *subscription = getSubscriptionForItem(handle, attr);
    if (!subscription) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify parameter for" << handle << ", the monitored item does not exist";
        QOpcUaMonitoringParameters p;
        p.setStatusCode(QOpcUa::UaStatusCode::BadMonitoredItemIdInvalid);
        emit monitoringStatusChanged(handle, attr, item, p);
        return;
    }

    subscription->modifyMonitoring(handle, attr, item, value);
    modifyPublishRequests();
}

QOpen62541Subscription *Open62541AsyncBackend::getSubscription(const QOpcUaMonitoringParameters &settings)
{
    if (settings.shared() == QOpcUaMonitoringParameters::SubscriptionType::Shared) {
        for (auto entry : qAsConst(m_subscriptions)) {
            if (qFuzzyCompare(entry->interval(), settings.publishingInterval()) && entry->shared() == QOpcUaMonitoringParameters::SubscriptionType::Shared)
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

void Open62541AsyncBackend::callMethod(uintptr_t handle, UA_NodeId objectId, UA_NodeId methodId, QVector<QOpcUa::TypedVariant> args)
{
    UA_Variant *inputArgs = nullptr;

    if (args.size()) {
        inputArgs = static_cast<UA_Variant *>(UA_Array_new(args.size(), &UA_TYPES[UA_TYPES_VARIANT]));
        for (int i = 0; i < args.size(); ++i)
            inputArgs[i] = QOpen62541ValueConverter::toOpen62541Variant(args[i].first, args[i].second);
    }

    size_t outputSize = 0;
    UA_Variant *outputArguments;
    UA_StatusCode res = UA_Client_call(m_uaclient, objectId, methodId, args.size(), inputArgs, &outputSize, &outputArguments);

    if (res != UA_STATUSCODE_GOOD)
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not call method:" << UA_StatusCode_name(res);

    QVariant result;

    if (outputSize > 1 && res == UA_STATUSCODE_GOOD) {
        QVariantList temp;
        for (size_t i = 0; i < outputSize; ++i)
            temp.append(QOpen62541ValueConverter::toQVariant(outputArguments[i]));

        result = temp;
    } else if (outputSize == 1 && res == UA_STATUSCODE_GOOD) {
        result = QOpen62541ValueConverter::toQVariant(outputArguments[0]);
    }

    UA_Array_delete(inputArgs, args.size(), &UA_TYPES[UA_TYPES_VARIANT]);
    if (outputSize > 0)
        UA_Array_delete(outputArguments, outputSize, &UA_TYPES[UA_TYPES_VARIANT]);

    emit methodCallFinished(handle, Open62541Utils::nodeIdToQString(methodId), result, static_cast<QOpcUa::UaStatusCode>(res));

    UA_NodeId_deleteMembers(&objectId);
    UA_NodeId_deleteMembers(&methodId);
}

static void convertBrowseResult(UA_BrowseResult *src, quint32 referencesSize, QVector<QOpcUaReferenceDescription> &dst)
{
    if (!src)
        return;

    for (size_t i = 0; i < referencesSize; ++i) {
        QOpcUaReferenceDescription temp;
        temp.setNodeId(Open62541Utils::nodeIdToQString(src->references[i].nodeId.nodeId));
        temp.setRefType(static_cast<QOpcUa::ReferenceTypeId>(src->references[i].referenceTypeId.identifier.numeric));
        temp.setNodeClass(static_cast<QOpcUa::NodeClass>(src->references[i].nodeClass));
        temp.setBrowseName(QOpen62541ValueConverter::scalarToQVariant<QOpcUa::QQualifiedName, UA_QualifiedName>(
                    &(src->references[i].browseName), QMetaType::Type::UnknownType).value<QOpcUa::QQualifiedName>());
        temp.setDisplayName(QOpen62541ValueConverter::scalarToQVariant<QOpcUa::QLocalizedText, UA_LocalizedText>(
                    &(src->references[i].displayName), QMetaType::Type::UnknownType).value<QOpcUa::QLocalizedText>());
        dst.push_back(temp);
    }
}

void Open62541AsyncBackend::browseChildren(uintptr_t handle, UA_NodeId id, QOpcUa::ReferenceTypeId referenceType, QOpcUa::NodeClasses nodeClassMask)
{
    UA_BrowseRequest request;
    UA_BrowseRequest_init(&request);
    request.nodesToBrowse = UA_BrowseDescription_new();
    request.nodesToBrowseSize = 1;
    request.nodesToBrowse->browseDirection = UA_BROWSEDIRECTION_FORWARD;
    request.nodesToBrowse->includeSubtypes = true;
    request.nodesToBrowse->nodeClassMask = static_cast<quint32>(nodeClassMask);
    request.nodesToBrowse->nodeId = id;
    request.nodesToBrowse->resultMask = UA_BROWSERESULTMASK_BROWSENAME | UA_BROWSERESULTMASK_DISPLAYNAME |
            UA_BROWSERESULTMASK_REFERENCETYPEID | UA_BROWSERESULTMASK_NODECLASS;
    request.nodesToBrowse->referenceTypeId = UA_NODEID_NUMERIC(0, static_cast<quint32>(referenceType));
    request.requestedMaxReferencesPerNode = 0; // Let the server choose a maximum value

    UA_BrowseResponse *response = UA_BrowseResponse_new();
    *response = UA_Client_Service_browse(m_uaclient, request);
    UA_BrowseRequest_deleteMembers(&request);

    QVector<QOpcUaReferenceDescription> ret;

    QOpcUa::UaStatusCode statusCode = QOpcUa::UaStatusCode::Good;

    while (response->resultsSize && statusCode == QOpcUa::UaStatusCode::Good) {
        UA_BrowseResponse *res = static_cast<UA_BrowseResponse *>(response);

        if (res->responseHeader.serviceResult != UA_STATUSCODE_GOOD || res->results->statusCode != UA_STATUSCODE_GOOD) {
            statusCode = static_cast<QOpcUa::UaStatusCode>(res->results->statusCode);
            break;
        }

        convertBrowseResult(res->results, res->results->referencesSize, ret);

        if (res->results->continuationPoint.length) {
            UA_BrowseNextRequest nextReq;
            UA_BrowseNextRequest_init(&nextReq);
            nextReq.continuationPoints = UA_ByteString_new();
            UA_ByteString_copy(&(res->results->continuationPoint), nextReq.continuationPoints);
            nextReq.continuationPointsSize = 1;
            UA_BrowseResponse_deleteMembers(res);
            *reinterpret_cast<UA_BrowseNextResponse *>(response) = UA_Client_Service_browseNext(m_uaclient, nextReq);
            UA_BrowseNextRequest_deleteMembers(&nextReq);
        } else {
            break;
        }
    }

    emit browseFinished(handle, ret, statusCode);

    UA_BrowseResponse_delete(static_cast<UA_BrowseResponse *>(response));
}

static void clientStateCallback(UA_Client *client, UA_ClientState state)
{
    Open62541AsyncBackend *backend = static_cast<Open62541AsyncBackend *>(UA_Client_getContext(client));
    if (!backend || !backend->m_useStateCallback)
        return;

    if (state == UA_CLIENTSTATE_DISCONNECTED) {
        emit backend->m_clientImpl->stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::ConnectionError);
        backend->m_useStateCallback = false;
    }
}

void Open62541AsyncBackend::connectToEndpoint(const QUrl &url)
{
    UA_ClientConfig conf = UA_ClientConfig_default;
    conf.clientContext = this;
    conf.stateCallback = &clientStateCallback;
    m_uaclient = UA_Client_new(conf);
    UA_StatusCode ret;

    if (url.userName().length())
        ret = UA_Client_connect_username(m_uaclient, url.toString(QUrl::RemoveUserInfo).toUtf8().constData(),
                                         url.userName().toUtf8().constData(), url.password().toUtf8().constData());
    else
        ret = UA_Client_connect(m_uaclient, url.toString().toUtf8().constData());

    if (ret != UA_STATUSCODE_GOOD) {
        UA_Client_delete(m_uaclient);
        m_uaclient = nullptr;
        QOpcUaClient::ClientError error = ret == UA_STATUSCODE_BADUSERACCESSDENIED ? QOpcUaClient::AccessDenied : QOpcUaClient::UnknownError;
        emit m_clientImpl->stateAndOrErrorChanged(QOpcUaClient::Disconnected, error);
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Open62541: Failed to connect";
        return;
    }

    m_useStateCallback = true;
    emit stateAndOrErrorChanged(QOpcUaClient::Connected, QOpcUaClient::NoError);
}

void Open62541AsyncBackend::disconnectFromEndpoint()
{
    m_subscriptionTimer.stop();
    qDeleteAll(m_subscriptions);
    m_subscriptions.clear();
    m_attributeMapping.clear();

    UA_StatusCode ret = UA_Client_disconnect(m_uaclient);
    if (ret != UA_STATUSCODE_GOOD) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Open62541: Failed to disconnect";
        // Fall through intentionally
    }

    UA_Client_delete(m_uaclient);
    m_uaclient = nullptr;
    emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::NoError);
}

void Open62541AsyncBackend::sendPublishRequest()
{
    if (!m_uaclient)
        return;

    if (!m_sendPublishRequests) {
        return;
    }

    // If BADSERVERNOTCONNECTED is returned, the subscriptions are gone and local information can be deleted.
    if (UA_Client_runAsync(m_uaclient, 1) == UA_STATUSCODE_BADSERVERNOTCONNECTED) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Unable to send publish request";
        m_sendPublishRequests = false;
        qDeleteAll(m_subscriptions);
        m_subscriptions.clear();
        m_attributeMapping.clear();
        return;
    }

    m_subscriptionTimer.start(0);
}

void Open62541AsyncBackend::modifyPublishRequests()
{
    if (m_subscriptions.count() == 0) {
        m_subscriptionTimer.stop();
        m_sendPublishRequests = false;
        return;
    }

    m_subscriptionTimer.stop();
    m_sendPublishRequests = true;
    sendPublishRequest();
}

QOpen62541Subscription *Open62541AsyncBackend::getSubscriptionForItem(uintptr_t handle, QOpcUa::NodeAttribute attr)
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

QT_END_NAMESPACE

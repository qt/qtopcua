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

#include "qopcuaauthenticationinformation.h"
#include <qopcuaerrorstate.h>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/qloggingcategory.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qurl.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_OPEN62541)

Open62541AsyncBackend::Open62541AsyncBackend(QOpen62541Client *parent)
    : QOpcUaBackend()
    , m_uaclient(nullptr)
    , m_clientImpl(parent)
    , m_useStateCallback(false)
    , m_subscriptionTimer(this)
    , m_sendPublishRequests(false)
    , m_minPublishingInterval(0)
{
    m_subscriptionTimer.setSingleShot(true);
    QObject::connect(&m_subscriptionTimer, &QTimer::timeout,
                     this, &Open62541AsyncBackend::sendPublishRequest);
}

Open62541AsyncBackend::~Open62541AsyncBackend()
{
    cleanupSubscriptions();
    if (m_uaclient)
        UA_Client_delete(m_uaclient);
}

void Open62541AsyncBackend::readAttributes(quint64 handle, UA_NodeId id, QOpcUa::NodeAttributes attr, QString indexRange)
{
    UA_ReadRequest req;
    UA_ReadRequest_init(&req);
    QVector<UA_ReadValueId> valueIds;

    UA_ReadValueId readId;
    UA_ReadValueId_init(&readId);
    UaDeleter<UA_ReadValueId> readIdDeleter(&readId, UA_ReadValueId_deleteMembers);
    readId.nodeId = id;

    QVector<QOpcUaReadResult> vec;

    qt_forEachAttribute(attr, [&](QOpcUa::NodeAttribute attribute){
        readId.attributeId = QOpen62541ValueConverter::toUaAttributeId(attribute);
        if (indexRange.length())
            QOpen62541ValueConverter::scalarFromQt<UA_String, QString>(indexRange, &readId.indexRange);
        valueIds.push_back(readId);
        QOpcUaReadResult temp;
        temp.setAttribute(attribute);
        vec.push_back(temp);
    });

    UA_ReadResponse res;
    UA_ReadResponse_init(&res);
    req.nodesToRead = valueIds.data();
    req.nodesToReadSize = valueIds.size();
    req.timestampsToReturn = UA_TIMESTAMPSTORETURN_BOTH;

    res = UA_Client_Service_read(m_uaclient, req);

    UaDeleter<UA_ReadResponse> responseDeleter(&res, UA_ReadResponse_deleteMembers);

    for (int i = 0; i < vec.size(); ++i) {
        // Use the service result as status code if there is no specific result for the current value.
        // This ensures a result for each attribute when UA_Client_Service_read is called for a disconnected client.
        if (static_cast<size_t>(i) >= res.resultsSize) {
            vec[i].setStatusCode(static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult));
            continue;
        }
        if (res.results[i].hasStatus)
            vec[i].setStatusCode(static_cast<QOpcUa::UaStatusCode>(res.results[i].status));
        else
            vec[i].setStatusCode(QOpcUa::UaStatusCode::Good);
        if (res.results[i].hasValue && res.results[i].value.data)
                vec[i].setValue(QOpen62541ValueConverter::toQVariant(res.results[i].value));
        if (res.results[i].hasServerTimestamp)
            vec[i].setSourceTimestamp(QOpen62541ValueConverter::scalarToQt<QDateTime, UA_DateTime>(&res.results[i].sourceTimestamp));
        if (res.results[i].hasSourceTimestamp)
            vec[i].setServerTimestamp(QOpen62541ValueConverter::scalarToQt<QDateTime, UA_DateTime>(&res.results[i].serverTimestamp));
    }
    emit attributesRead(handle, vec, static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult));
}

void Open62541AsyncBackend::writeAttribute(quint64 handle, UA_NodeId id, QOpcUa::NodeAttribute attrId, QVariant value, QOpcUa::Types type, QString indexRange)
{
    if (type == QOpcUa::Types::Undefined && attrId != QOpcUa::NodeAttribute::Value)
        type = attributeIdToTypeId(attrId);

    UA_WriteRequest req;
    UA_WriteRequest_init(&req);
    UaDeleter<UA_WriteRequest> requestDeleter(&req, UA_WriteRequest_deleteMembers);
    req.nodesToWriteSize = 1;
    req.nodesToWrite = UA_WriteValue_new();

    UA_WriteValue_init(req.nodesToWrite);
    req.nodesToWrite->attributeId = QOpen62541ValueConverter::toUaAttributeId(attrId);
    req.nodesToWrite->nodeId = id;
    req.nodesToWrite->value.value = QOpen62541ValueConverter::toOpen62541Variant(value, type);
    req.nodesToWrite->value.hasValue = true;
    if (indexRange.length())
        QOpen62541ValueConverter::scalarFromQt<UA_String, QString>(indexRange, &req.nodesToWrite->indexRange);

    UA_WriteResponse res = UA_Client_Service_write(m_uaclient, req);
    UaDeleter<UA_WriteResponse> responseDeleter(&res, UA_WriteResponse_deleteMembers);

    QOpcUa::UaStatusCode status = res.resultsSize ?
                static_cast<QOpcUa::UaStatusCode>(res.results[0]) : static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult);

    emit attributeWritten(handle, attrId, value, status);
}

void Open62541AsyncBackend::writeAttributes(quint64 handle, UA_NodeId id, QOpcUaNode::AttributeMap toWrite, QOpcUa::Types valueAttributeType)
{
    UaDeleter<UA_NodeId> nodeIdDeleter(&id, UA_NodeId_deleteMembers);

    if (toWrite.size() == 0) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "No values to be written";
        emit attributeWritten(handle, QOpcUa::NodeAttribute::None, QVariant(), QOpcUa::UaStatusCode::BadNothingToDo);
        return;
    }

    UA_WriteRequest req;
    UA_WriteRequest_init(&req);
    UaDeleter<UA_WriteRequest> requestDeleter(&req, UA_WriteRequest_deleteMembers);
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
    UaDeleter<UA_WriteResponse> responseDeleter(&res, UA_WriteResponse_deleteMembers);

    index = 0;
    for (auto it = toWrite.begin(); it != toWrite.end(); ++it, ++index) {
        QOpcUa::UaStatusCode status = index < res.resultsSize ?
                    static_cast<QOpcUa::UaStatusCode>(res.results[index]) : static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult);
        emit attributeWritten(handle, it.key(), it.value(), status);
    }
}

void Open62541AsyncBackend::enableMonitoring(quint64 handle, UA_NodeId id, QOpcUa::NodeAttributes attr, const QOpcUaMonitoringParameters &settings)
{
    UaDeleter<UA_NodeId> nodeIdDeleter(&id, UA_NodeId_deleteMembers);

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

void Open62541AsyncBackend::disableMonitoring(quint64 handle, QOpcUa::NodeAttributes attr)
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

void Open62541AsyncBackend::modifyMonitoring(quint64 handle, QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, QVariant value)
{
    QOpen62541Subscription *subscription = getSubscriptionForItem(handle, attr);
    if (!subscription) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify" << item << ", the monitored item does not exist";
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
    if (settings.subscriptionType() == QOpcUaMonitoringParameters::SubscriptionType::Shared) {
        // Requesting multiple subscriptions with publishing interval < minimum publishing interval breaks subscription sharing
        double interval = revisePublishingInterval(settings.publishingInterval(), m_minPublishingInterval);
        for (auto entry : qAsConst(m_subscriptions)) {
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
    if (sub->interval() > settings.samplingInterval()) // The publishing interval has been revised by the server.
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
        modifyPublishRequests();
        return true;
    }
    return false;
}

void Open62541AsyncBackend::callMethod(quint64 handle, UA_NodeId objectId, UA_NodeId methodId, QVector<QOpcUa::TypedVariant> args)
{
    UaDeleter<UA_NodeId> objectIdDeleter(&objectId, UA_NodeId_deleteMembers);
    UaDeleter<UA_NodeId> methodIdDeleter(&methodId, UA_NodeId_deleteMembers);

    UA_Variant *inputArgs = nullptr;

    if (args.size()) {
        inputArgs = static_cast<UA_Variant *>(UA_Array_new(args.size(), &UA_TYPES[UA_TYPES_VARIANT]));
        for (int i = 0; i < args.size(); ++i)
            inputArgs[i] = QOpen62541ValueConverter::toOpen62541Variant(args[i].first, args[i].second);
    }
    UaArrayDeleter<UA_TYPES_VARIANT> inputArgsDeleter(inputArgs, args.size());

    size_t outputSize = 0;
    UA_Variant *outputArguments = nullptr;
    UA_StatusCode res = UA_Client_call(m_uaclient, objectId, methodId, args.size(), inputArgs, &outputSize, &outputArguments);
    UaArrayDeleter<UA_TYPES_VARIANT> outputArgsDeleter(outputArguments, outputSize);

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

    emit methodCallFinished(handle, Open62541Utils::nodeIdToQString(methodId), result, static_cast<QOpcUa::UaStatusCode>(res));
}

void Open62541AsyncBackend::resolveBrowsePath(quint64 handle, UA_NodeId startNode, const QVector<QOpcUaRelativePathElement> &path)
{
    UA_TranslateBrowsePathsToNodeIdsRequest req;
    UA_TranslateBrowsePathsToNodeIdsRequest_init(&req);
    UaDeleter<UA_TranslateBrowsePathsToNodeIdsRequest> requestDeleter(
                &req,UA_TranslateBrowsePathsToNodeIdsRequest_deleteMembers);

    req.browsePathsSize = 1;
    req.browsePaths = UA_BrowsePath_new();
    UA_BrowsePath_init(req.browsePaths);
    req.browsePaths->startingNode = startNode;
    req.browsePaths->relativePath.elementsSize = path.size();
    req.browsePaths->relativePath.elements = static_cast<UA_RelativePathElement *>(UA_Array_new(path.size(), &UA_TYPES[UA_TYPES_RELATIVEPATHELEMENT]));

    for (int i = 0 ; i < path.size(); ++i) {
        req.browsePaths->relativePath.elements[i].includeSubtypes = path[i].includeSubtypes();
        req.browsePaths->relativePath.elements[i].isInverse = path[i].isInverse();
        req.browsePaths->relativePath.elements[i].referenceTypeId = Open62541Utils::nodeIdFromQString(path[i].referenceTypeId());
        req.browsePaths->relativePath.elements[i].targetName = UA_QUALIFIEDNAME_ALLOC(path[i].targetName().namespaceIndex(),
                                                                                      path[i].targetName().name().toUtf8().constData());
    }

    UA_TranslateBrowsePathsToNodeIdsResponse res = UA_Client_Service_translateBrowsePathsToNodeIds(m_uaclient, req);
    UaDeleter<UA_TranslateBrowsePathsToNodeIdsResponse> responseDeleter(
                &res, UA_TranslateBrowsePathsToNodeIdsResponse_deleteMembers);

    if (res.responseHeader.serviceResult != UA_STATUSCODE_GOOD || res.resultsSize != 1) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Translate browse path failed:" << UA_StatusCode_name(res.responseHeader.serviceResult);
        emit resolveBrowsePathFinished(handle, QVector<QOpcUaBrowsePathTarget>(), path,
                                         static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult));
        return;
    }

    QVector<QOpcUaBrowsePathTarget> ret;
    for (size_t i = 0; i < res.results[0].targetsSize ; ++i) {
        QOpcUaBrowsePathTarget temp;
        temp.setRemainingPathIndex(res.results[0].targets[i].remainingPathIndex);
        temp.targetIdRef().setNamespaceUri(QString::fromUtf8(reinterpret_cast<char *>(res.results[0].targets[i].targetId.namespaceUri.data)));
        temp.targetIdRef().setServerIndex(res.results[0].targets[i].targetId.serverIndex);
        temp.targetIdRef().setNodeId(Open62541Utils::nodeIdToQString(res.results[0].targets[i].targetId.nodeId));
        ret.append(temp);
    }

    emit resolveBrowsePathFinished(handle, ret, path, static_cast<QOpcUa::UaStatusCode>(res.results[0].statusCode));
}

void Open62541AsyncBackend::findServers(const QUrl &url, const QStringList &localeIds, const QStringList &serverUris)
{
    UA_Client *tmpClient = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(tmpClient));
    UaDeleter<UA_Client> clientDeleter(tmpClient, UA_Client_delete);

    UA_String *uaServerUris = nullptr;
    if (!serverUris.isEmpty()) {
        uaServerUris = static_cast<UA_String *>(UA_Array_new(serverUris.size(), &UA_TYPES[UA_TYPES_STRING]));
        for (int i = 0; i < serverUris.size(); ++i)
            QOpen62541ValueConverter::scalarFromQt(serverUris.at(i), &uaServerUris[i]);
    }
    UaArrayDeleter<UA_TYPES_STRING> serverUrisDeleter(uaServerUris, serverUris.size());

    UA_String *uaLocaleIds = nullptr;
    if (!localeIds.isEmpty()) {
        uaLocaleIds = static_cast<UA_String *>(UA_Array_new(localeIds.size(), &UA_TYPES[UA_TYPES_STRING]));
        for (int i = 0; i < localeIds.size(); ++i)
            QOpen62541ValueConverter::scalarFromQt(localeIds.at(i), &uaLocaleIds[i]);
    }
    UaArrayDeleter<UA_TYPES_STRING> localeIdsDeleter(uaLocaleIds, localeIds.size());

    size_t serversSize = 0;
    UA_ApplicationDescription *servers = nullptr;

    UA_StatusCode result = UA_Client_findServers(tmpClient, url.toString(QUrl::RemoveUserInfo).toUtf8().constData(),
                                                 serverUris.size(), uaServerUris, localeIds.size(), uaLocaleIds,
                                                 &serversSize, &servers);

    UaArrayDeleter<UA_TYPES_APPLICATIONDESCRIPTION> serversDeleter(servers, serversSize);

    QVector<QOpcUaApplicationDescription> ret;

    for (size_t i = 0; i < serversSize; ++i)
        ret.append(convertApplicationDescription(servers[i]));

    if (result != UA_STATUSCODE_GOOD) {
        qCDebug(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to get servers:" << static_cast<QOpcUa::UaStatusCode>(result);
    }

    emit findServersFinished(ret, static_cast<QOpcUa::UaStatusCode>(result), url);
}

void Open62541AsyncBackend::readNodeAttributes(const QVector<QOpcUaReadItem> &nodesToRead)
{
    if (nodesToRead.size() == 0) {
        emit readNodeAttributesFinished(QVector<QOpcUaReadResult>(), QOpcUa::UaStatusCode::BadNothingToDo);
        return;
    }

    UA_ReadRequest req;
    UA_ReadRequest_init(&req);
    UaDeleter<UA_ReadRequest> requestDeleter(&req, UA_ReadRequest_deleteMembers);

    req.nodesToReadSize = nodesToRead.size();
    req.nodesToRead = static_cast<UA_ReadValueId *>(UA_Array_new(nodesToRead.size(), &UA_TYPES[UA_TYPES_READVALUEID]));
    req.timestampsToReturn = UA_TIMESTAMPSTORETURN_BOTH;

    for (int i = 0; i < nodesToRead.size(); ++i) {
        UA_ReadValueId_init(&req.nodesToRead[i]);
        req.nodesToRead[i].attributeId = QOpen62541ValueConverter::toUaAttributeId(nodesToRead.at(i).attribute());
        req.nodesToRead[i].nodeId = Open62541Utils::nodeIdFromQString(nodesToRead.at(i).nodeId());
        if (!nodesToRead[i].indexRange().isEmpty())
            QOpen62541ValueConverter::scalarFromQt<UA_String, QString>(nodesToRead.at(i).indexRange(),
                                                                       &req.nodesToRead[i].indexRange);
    }

    UA_ReadResponse res = UA_Client_Service_read(m_uaclient, req);
    UaDeleter<UA_ReadResponse> responseDeleter(&res, UA_ReadResponse_deleteMembers);

    QOpcUa::UaStatusCode serviceResult = static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult);

    if (serviceResult != QOpcUa::UaStatusCode::Good) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Batch read failed:" << serviceResult;
        emit readNodeAttributesFinished(QVector<QOpcUaReadResult>(), serviceResult);
    } else {
        QVector<QOpcUaReadResult> ret;

        for (int i = 0; i < nodesToRead.size(); ++i) {
            QOpcUaReadResult item;
            item.setAttribute(nodesToRead.at(i).attribute());
            item.setNodeId(nodesToRead.at(i).nodeId());
            item.setIndexRange(nodesToRead.at(i).indexRange());
            if (static_cast<size_t>(i) < res.resultsSize) {
                if (res.results[i].hasServerTimestamp)
                    item.setServerTimestamp(QOpen62541ValueConverter::scalarToQt<QDateTime>(&res.results[i].serverTimestamp));
                if (res.results[i].hasSourceTimestamp)
                    item.setSourceTimestamp(QOpen62541ValueConverter::scalarToQt<QDateTime>(&res.results[i].sourceTimestamp));
                if (res.results[i].hasValue)
                    item.setValue(QOpen62541ValueConverter::toQVariant(res.results[i].value));
                if (res.results[i].hasStatus)
                    item.setStatusCode(static_cast<QOpcUa::UaStatusCode>(res.results[i].status));
                else
                    item.setStatusCode(serviceResult);
            } else {
                item.setStatusCode(serviceResult);
            }
            ret.push_back(item);
        }
        emit readNodeAttributesFinished(ret, serviceResult);
    }
}

void Open62541AsyncBackend::writeNodeAttributes(const QVector<QOpcUaWriteItem> &nodesToWrite)
{
    if (nodesToWrite.isEmpty()) {
        emit writeNodeAttributesFinished(QVector<QOpcUaWriteResult>(), QOpcUa::UaStatusCode::BadNothingToDo);
        return;
    }

    UA_WriteRequest req;
    UA_WriteRequest_init(&req);
    UaDeleter<UA_WriteRequest> requestDeleter(&req, UA_WriteRequest_deleteMembers);

    req.nodesToWriteSize = nodesToWrite.size();
    req.nodesToWrite = static_cast<UA_WriteValue *>(UA_Array_new(nodesToWrite.size(), &UA_TYPES[UA_TYPES_WRITEVALUE]));

    for (int i = 0; i < nodesToWrite.size(); ++i) {
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

    UA_WriteResponse res = UA_Client_Service_write(m_uaclient, req);
    UaDeleter<UA_WriteResponse> responseDeleter(&res, UA_WriteResponse_deleteMembers);

    QOpcUa::UaStatusCode serviceResult = QOpcUa::UaStatusCode(res.responseHeader.serviceResult);

    if (serviceResult != QOpcUa::UaStatusCode::Good) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Batch write failed:" << serviceResult;
        emit writeNodeAttributesFinished(QVector<QOpcUaWriteResult>(), serviceResult);
    } else {
        QVector<QOpcUaWriteResult> ret;

        for (int i = 0; i < nodesToWrite.size(); ++i) {
            QOpcUaWriteResult item;
            item.setAttribute(nodesToWrite.at(i).attribute());
            item.setNodeId(nodesToWrite.at(i).nodeId());
            item.setIndexRange(nodesToWrite.at(i).indexRange());
            if (static_cast<size_t>(i) < res.resultsSize)
                item.setStatusCode(QOpcUa::UaStatusCode(res.results[i]));
            else
                item.setStatusCode(serviceResult);
            ret.push_back(item);
        }
        emit writeNodeAttributesFinished(ret, serviceResult);
    }
}

void Open62541AsyncBackend::addNode(const QOpcUaAddNodeItem &nodeToAdd)
{
    UA_AddNodesRequest req;
    UA_AddNodesRequest_init(&req);
    UaDeleter<UA_AddNodesRequest> requestDeleter(&req, UA_AddNodesRequest_deleteMembers);
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

    UA_AddNodesResponse res = UA_Client_Service_addNodes(m_uaclient, req);
    UaDeleter<UA_AddNodesResponse> responseDeleter(&res, UA_AddNodesResponse_deleteMembers);

    QOpcUa::UaStatusCode status = QOpcUa::UaStatusCode::Good;
    QString resultId;
    if (res.responseHeader.serviceResult == UA_STATUSCODE_GOOD) {
        if (res.results[0].statusCode == UA_STATUSCODE_GOOD)
            resultId = Open62541Utils::nodeIdToQString(res.results[0].addedNodeId);
        else {
            status = static_cast<QOpcUa::UaStatusCode>(res.results[0].statusCode);
            qCDebug(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to add node:" << status;
        }
    } else {
        status = static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult);
        qCDebug(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to add node:" << status;
    }

    emit addNodeFinished(nodeToAdd.requestedNewNodeId(), resultId, status);
}

void Open62541AsyncBackend::deleteNode(const QString &nodeId, bool deleteTargetReferences)
{
    UA_NodeId id = Open62541Utils::nodeIdFromQString(nodeId);
    UaDeleter<UA_NodeId> nodeIdDeleter(&id, UA_NodeId_deleteMembers);

    UA_StatusCode res = UA_Client_deleteNode(m_uaclient, id, deleteTargetReferences);

    QOpcUa::UaStatusCode resultStatus = static_cast<QOpcUa::UaStatusCode>(res);

    if (resultStatus != QOpcUa::UaStatusCode::Good) {
        qCDebug(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to delete node" << nodeId << "with status code" << resultStatus;
    }

    emit deleteNodeFinished(nodeId, resultStatus);
}

void Open62541AsyncBackend::addReference(const QOpcUaAddReferenceItem &referenceToAdd)
{
    UA_ExpandedNodeId target;
    UA_ExpandedNodeId_init(&target);
    UaDeleter<UA_ExpandedNodeId> nodeIdDeleter(&target, UA_ExpandedNodeId_deleteMembers);

    QOpen62541ValueConverter::scalarFromQt<UA_ExpandedNodeId, QOpcUaExpandedNodeId>(
                referenceToAdd.targetNodeId(), &target);

    UA_String serverUri;
    UaDeleter<UA_String> serverUriDeleter(&serverUri, UA_String_deleteMembers);
    QOpen62541ValueConverter::scalarFromQt<UA_String, QString>(
                referenceToAdd.targetServerUri(), &serverUri);

    UA_NodeClass nodeClass = static_cast<UA_NodeClass>(referenceToAdd.targetNodeClass());

    UA_StatusCode res = UA_Client_addReference(m_uaclient,
                                               Open62541Utils::nodeIdFromQString(referenceToAdd.sourceNodeId()),
                                               Open62541Utils::nodeIdFromQString(referenceToAdd.referenceTypeId()),
                                               referenceToAdd.isForwardReference(), serverUri, target, nodeClass);

    QOpcUa::UaStatusCode statusCode = static_cast<QOpcUa::UaStatusCode>(res);
    if (res != UA_STATUSCODE_GOOD)
        qCDebug(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to add reference from" << referenceToAdd.sourceNodeId() << "to"
                                            << referenceToAdd.targetNodeId().nodeId() << ":" << statusCode;

    emit addReferenceFinished(referenceToAdd.sourceNodeId(), referenceToAdd.referenceTypeId(),
                              referenceToAdd.targetNodeId(),
                              referenceToAdd.isForwardReference(), statusCode);
}

void Open62541AsyncBackend::deleteReference(const QOpcUaDeleteReferenceItem &referenceToDelete)
{
    UA_ExpandedNodeId target;
    UA_ExpandedNodeId_init(&target);
    UaDeleter<UA_ExpandedNodeId> targetDeleter(&target, UA_ExpandedNodeId_deleteMembers);
    QOpen62541ValueConverter::scalarFromQt<UA_ExpandedNodeId, QOpcUaExpandedNodeId>(
                referenceToDelete.targetNodeId(), &target);

    UA_StatusCode res = UA_Client_deleteReference(m_uaclient,
                                                  Open62541Utils::nodeIdFromQString(referenceToDelete.sourceNodeId()),
                                                  Open62541Utils::nodeIdFromQString(referenceToDelete.referenceTypeId()),
                                                  referenceToDelete.isForwardReference(),
                                                  target, referenceToDelete.deleteBidirectional());

    QOpcUa::UaStatusCode statusCode = static_cast<QOpcUa::UaStatusCode>(res);
    if (res != UA_STATUSCODE_GOOD)
        qCDebug(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to delete reference from" << referenceToDelete.sourceNodeId() << "to"
                                            << referenceToDelete.targetNodeId().nodeId() << ":" << statusCode;

    emit deleteReferenceFinished(referenceToDelete.sourceNodeId(), referenceToDelete.referenceTypeId(),
                                 referenceToDelete.targetNodeId(),
                                 referenceToDelete.isForwardReference(), statusCode);
}

static void convertBrowseResult(UA_BrowseResult *src, quint32 referencesSize, QVector<QOpcUaReferenceDescription> &dst)
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
    UA_BrowseRequest uaRequest;
    UA_BrowseRequest_init(&uaRequest);
    UaDeleter<UA_BrowseRequest> requestDeleter(&uaRequest, UA_BrowseRequest_deleteMembers);

    uaRequest.nodesToBrowse = UA_BrowseDescription_new();
    uaRequest.nodesToBrowseSize = 1;
    uaRequest.nodesToBrowse->browseDirection = static_cast<UA_BrowseDirection>(request.browseDirection());
    uaRequest.nodesToBrowse->includeSubtypes = request.includeSubtypes();
    uaRequest.nodesToBrowse->nodeClassMask = static_cast<quint32>(request.nodeClassMask());
    uaRequest.nodesToBrowse->nodeId = id;
    uaRequest.nodesToBrowse->resultMask = UA_BROWSERESULTMASK_ALL;
    uaRequest.nodesToBrowse->referenceTypeId = Open62541Utils::nodeIdFromQString(request.referenceTypeId());
    uaRequest.requestedMaxReferencesPerNode = 0; // Let the server choose a maximum value

    UA_BrowseResponse *response = UA_BrowseResponse_new();
    UaDeleter<UA_BrowseResponse> responseDeleter(response, UA_BrowseResponse_delete);
    *response = UA_Client_Service_browse(m_uaclient, uaRequest);

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
            UaDeleter<UA_BrowseNextRequest> nextReqDeleter(&nextReq, UA_BrowseNextRequest_deleteMembers);
            nextReq.continuationPoints = UA_ByteString_new();
            UA_ByteString_copy(&(res->results->continuationPoint), nextReq.continuationPoints);
            nextReq.continuationPointsSize = 1;
            UA_BrowseResponse_deleteMembers(res); // Deallocate the pointer members before overwriting the response
            *reinterpret_cast<UA_BrowseNextResponse *>(response) = UA_Client_Service_browseNext(m_uaclient, nextReq);
        } else {
            break;
        }
    }

    emit browseFinished(handle, ret, statusCode);
}

static void clientStateCallback(UA_Client *client, UA_ClientState state)
{
    Open62541AsyncBackend *backend = static_cast<Open62541AsyncBackend *>(UA_Client_getContext(client));
    if (!backend || !backend->m_useStateCallback)
        return;

    if (state == UA_CLIENTSTATE_DISCONNECTED) {
        emit backend->stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::ConnectionError);
        backend->m_useStateCallback = false;
        // Use a queued connection to make sure the subscription is not deleted if the callback was triggered
        // inside of one of its methods.
        QMetaObject::invokeMethod(backend, "cleanupSubscriptions", Qt::QueuedConnection);
    }
}

void Open62541AsyncBackend::connectToEndpoint(const QOpcUaEndpointDescription &endpoint)
{
    cleanupSubscriptions();

    if (m_uaclient)
        UA_Client_delete(m_uaclient);

    QString errorMessage;
    if (!verifyEndpointDescription(endpoint, &errorMessage)) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << errorMessage;
        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::ClientError::InvalidUrl);
        return;
    }

    if (!m_clientImpl->supportedSecurityPolicies().contains(endpoint.securityPolicy())) {
#ifndef UA_ENABLE_ENCRYPTION
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "The open62541 plugin has been built without encryption support";
#endif
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Unsupported security policy:" << endpoint.securityPolicy();
        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::ClientError::InvalidUrl);
        return;
    }

    emit stateAndOrErrorChanged(QOpcUaClient::Connecting, QOpcUaClient::NoError);

    m_useStateCallback = false;

    m_uaclient = UA_Client_new();
    auto conf = UA_Client_getConfig(m_uaclient);

    const auto identity = m_clientImpl->m_client->applicationIdentity();
    const auto authInfo = m_clientImpl->m_client->authenticationInformation();
#ifdef UA_ENABLE_ENCRYPTION
    const auto pkiConfig = m_clientImpl->m_client->pkiConfiguration();
#endif

#ifdef UA_ENABLE_ENCRYPTION
    if (pkiConfig.isPkiValid()) {
        UA_ByteString localCertificate;
        UA_ByteString privateKey;
        UA_ByteString *trustList = nullptr;
        int trustListSize = 0;
        UA_ByteString *revocationList = nullptr;
        int revocationListSize = 0;

        bool success = loadFileToByteString(pkiConfig.clientCertificateFile(), &localCertificate);

        if (!success) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to load client certificate";
            emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::AccessDenied);
            return;
        }

        UaDeleter<UA_ByteString> clientCertDeleter(&localCertificate, &UA_ByteString_deleteMembers);

        success = loadFileToByteString(pkiConfig.privateKeyFile(), &privateKey);

        if (!success) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to load private key";
            emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::AccessDenied);
            return;
        }

        UaDeleter<UA_ByteString> privateKeyDeleter(&privateKey, &UA_ByteString_deleteMembers);

        success = loadAllFilesInDirectory(pkiConfig.trustListDirectory(), &trustList, &trustListSize);

        if (!success) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to load trust list";
            emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::AccessDenied);
            return;
        }

        UaArrayDeleter<UA_TYPES_BYTESTRING> trustListDeleter(trustList, trustListSize);

        success = loadAllFilesInDirectory(pkiConfig.revocationListDirectory(), &revocationList, &revocationListSize);

        if (!success) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to load revocation list";
            emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::AccessDenied);
            return;
        }

        UaArrayDeleter<UA_TYPES_BYTESTRING> revocationListDeleter(revocationList, revocationListSize);

        UA_StatusCode result = UA_ClientConfig_setDefaultEncryption(conf, localCertificate, privateKey, trustList,
                                                                    trustListSize, revocationList, revocationListSize);

        if (result != UA_STATUSCODE_GOOD) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to initialize PKI:" << static_cast<QOpcUa::UaStatusCode>(result);
            emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::AccessDenied);
            return;
        }
    } else {
#else
    {
#endif
        UA_ClientConfig_setDefault(conf);
    }

    conf->clientContext = this;
    conf->stateCallback = &clientStateCallback;
    conf->clientDescription.applicationName = UA_LOCALIZEDTEXT_ALLOC("", identity.applicationName().toUtf8().constData());
    conf->clientDescription.applicationUri  = UA_STRING_ALLOC(identity.applicationUri().toUtf8().constData());
    conf->clientDescription.productUri      = UA_STRING_ALLOC(identity.productUri().toUtf8().constData());
    conf->clientDescription.applicationType = UA_APPLICATIONTYPE_CLIENT;

    conf->securityPolicyUri = UA_STRING_ALLOC(endpoint.securityPolicy().toUtf8().constData());
    conf->securityMode = static_cast<UA_MessageSecurityMode>(endpoint.securityMode());

    UA_StatusCode ret;

    if (authInfo.authenticationType() == QOpcUaUserTokenPolicy::TokenType::Anonymous) {
        ret = UA_Client_connect(m_uaclient, endpoint.endpointUrl().toUtf8().constData());
    } else if (authInfo.authenticationType() == QOpcUaUserTokenPolicy::TokenType::Username) {

        bool suitableTokenFound = false;
        for (const auto token : endpoint.userIdentityTokens()) {
            if (token.tokenType() == QOpcUaUserTokenPolicy::Username &&
                    m_clientImpl->supportedSecurityPolicies().contains(token.securityPolicy())) {
                suitableTokenFound = true;
                break;
            }
        }

        if (!suitableTokenFound) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "No suitable user token policy found";
            emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::ClientError::NoError);
            return;
        }

        const auto credentials = authInfo.authenticationData().value<QPair<QString, QString>>();
        ret = UA_Client_connect_username(m_uaclient, endpoint.endpointUrl().toUtf8().constData(),
                                         credentials.first.toUtf8().constData(), credentials.second.toUtf8().constData());
    } else {
        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::UnsupportedAuthenticationInformation);
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to connect: Selected authentication type"
                                          << authInfo.authenticationType() << "is not supported.";
        return;
    }

    if (ret != UA_STATUSCODE_GOOD) {
        UA_Client_delete(m_uaclient);
        m_uaclient = nullptr;
        QOpcUaClient::ClientError error = ret == UA_STATUSCODE_BADUSERACCESSDENIED ? QOpcUaClient::AccessDenied : QOpcUaClient::UnknownError;

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

    m_useStateCallback = true;
    emit stateAndOrErrorChanged(QOpcUaClient::Connected, QOpcUaClient::NoError);
}

void Open62541AsyncBackend::disconnectFromEndpoint()
{
    m_subscriptionTimer.stop();
    cleanupSubscriptions();

    m_useStateCallback = false;

    if (m_uaclient) {
        UA_StatusCode ret = UA_Client_disconnect(m_uaclient);
        if (ret != UA_STATUSCODE_GOOD) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Open62541: Failed to disconnect";
            // Fall through intentionally
        }
        UA_Client_delete(m_uaclient);
        m_uaclient = nullptr;
    }

    emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::NoError);
}

void Open62541AsyncBackend::requestEndpoints(const QUrl &url)
{
    UA_Client *tmpClient = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(tmpClient));
    size_t numEndpoints = 0;
    UA_EndpointDescription *endpoints = nullptr;
    UA_StatusCode res = UA_Client_getEndpoints(tmpClient, url.toString(QUrl::RemoveUserInfo).toUtf8().constData(), &numEndpoints, &endpoints);
    UaArrayDeleter<UA_TYPES_ENDPOINTDESCRIPTION> endpointDescriptionDeleter(endpoints, numEndpoints);
    QVector<QOpcUaEndpointDescription> ret;

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
            qWarning() << "Failed to retrive endpoints from " << url.toString(QUrl::RemoveUserInfo).toUtf8().constData()
                       << "with status" << UA_StatusCode_name(res);
    }

    emit endpointsRequestFinished(ret, static_cast<QOpcUa::UaStatusCode>(res), url);

    UA_Client_delete(tmpClient);
}

void Open62541AsyncBackend::sendPublishRequest()
{
    if (!m_uaclient)
        return;

    if (!m_sendPublishRequests) {
        return;
    }

    // If BADSERVERNOTCONNECTED is returned, the subscriptions are gone and local information can be deleted.
    if (UA_Client_run_iterate(m_uaclient, 1) == UA_STATUSCODE_BADSERVERNOTCONNECTED) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Unable to send publish request";
        m_sendPublishRequests = false;
        cleanupSubscriptions();
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

void Open62541AsyncBackend::handleSubscriptionTimeout(QOpen62541Subscription *sub, QVector<QPair<quint64, QOpcUa::NodeAttribute>> items)
{
    for (auto it : qAsConst(items)) {
        auto item = m_attributeMapping.find(it.first);
        if (item == m_attributeMapping.end())
            continue;
        item->remove(it.second);
    }
    m_subscriptions.remove(sub->subscriptionId());
    delete sub;
    modifyPublishRequests();
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
    temp.length = data.length();
    if (data.isEmpty())
        temp.data = nullptr;
    else {
        if (data.startsWith('-')) { // PEM file
            // Remove trailing newline, mbedTLS doesn't tolerate this when loading a certificate
            data = QString::fromLatin1(data).trimmed().toLatin1();
        }
        temp.data = reinterpret_cast<unsigned char *>(data.data());
    }

    return UA_ByteString_copy(&temp, target) == UA_STATUSCODE_GOOD;
}

bool Open62541AsyncBackend::loadAllFilesInDirectory(const QString &location, UA_ByteString **target, int *size) const
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

    const int tempSize = entries.size();
    UA_ByteString *list = static_cast<UA_ByteString *>(UA_Array_new(tempSize, &UA_TYPES[UA_TYPES_BYTESTRING]));

    if (!list) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to allocate memory for loading files in" << location;
        return false;
    }

    for (int i = 0; i < entries.size(); ++i) {
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

UA_UInt32 *Open62541AsyncBackend::copyArrayDimensions(const QVector<quint32> &arrayDimensions, size_t *outputSize)
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

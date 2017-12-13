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
    , m_subscriptionTimer(this)
    , m_sendPublishRequests(false)
    , m_shortestInterval(std::numeric_limits<qint32>::max())
{
    m_subscriptionTimer.setSingleShot(true);
    QObject::connect(&m_subscriptionTimer, &QTimer::timeout,
                     this, &Open62541AsyncBackend::sendPublishRequest);
}

Open62541AsyncBackend::~Open62541AsyncBackend()
{
    qDeleteAll(m_subscriptions);
}

void Open62541AsyncBackend::readAttributes(uintptr_t handle, UA_NodeId id, QOpcUaNode::NodeAttributes attr, QString indexRange)
{
    UA_ReadRequest req;
    UA_ReadRequest_init(&req);
    QVector<UA_ReadValueId> valueIds;

    UA_ReadValueId readId;
    UA_ReadValueId_init(&readId);
    readId.nodeId = id;

    QVector<QOpcUaReadResult> vec;

    qt_forEachAttribute(attr, [&](QOpcUaNode::NodeAttribute attribute){
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

    res = UA_Client_Service_read(m_uaclient, req);

    for (size_t i = 0; i < res.resultsSize; ++i) {
        if (res.results[i].hasStatus)
            vec[i].statusCode = static_cast<QOpcUa::UaStatusCode>(res.results[i].status);
        else
            vec[i].statusCode = QOpcUa::UaStatusCode::Good;
        if (res.results[i].hasValue && res.results[i].value.data)
                vec[i].value = QOpen62541ValueConverter::toQVariant(res.results[i].value);
    }
    emit attributesRead(handle, vec, static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult));
    UA_ReadValueId_deleteMembers(&readId);
    UA_ReadResponse_deleteMembers(&res);
}

void Open62541AsyncBackend::writeAttribute(uintptr_t handle, UA_NodeId id, QOpcUaNode::NodeAttribute attrId, QVariant value, QOpcUa::Types type, QString indexRange)
{
    if (type == QOpcUa::Types::Undefined && attrId != QOpcUaNode::NodeAttribute::Value)
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
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "No values to be written");
        emit attributeWritten(handle, QOpcUaNode::NodeAttribute::None, QVariant(), QOpcUa::UaStatusCode::BadNothingToDo);
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
        QOpcUa::Types type = it.key() == QOpcUaNode::NodeAttribute::Value ? valueAttributeType : attributeIdToTypeId(it.key());
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

void Open62541AsyncBackend::enableMonitoring(uintptr_t handle, UA_NodeId id, QOpcUaNode::NodeAttributes attr, const QOpcUaMonitoringParameters &settings)
{
    QOpen62541Subscription *usedSubscription = nullptr;

    // Create a new subscription if necessary
    if (settings.subscriptionId()) {
        auto sub = m_subscriptions.find(settings.subscriptionId());
        if (sub == m_subscriptions.end()) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "There is no subscription with id %u", settings.subscriptionId());

            qt_forEachAttribute(attr, [&](QOpcUaNode::NodeAttribute attribute){
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
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Could not create subscription with interval %f", settings.publishingInterval());
        qt_forEachAttribute(attr, [&](QOpcUaNode::NodeAttribute attribute){
            QOpcUaMonitoringParameters s;
            s.setStatusCode(QOpcUa::UaStatusCode::BadSubscriptionIdInvalid);
            emit monitoringEnableDisable(handle, attribute, true, s);
        });
        return;
    }

    qt_forEachAttribute(attr, [&](QOpcUaNode::NodeAttribute attribute){
        bool success = usedSubscription->addAttributeMonitoredItem(handle, attribute, id, settings);
        if (success)
            m_attributeMapping[handle][attribute] = usedSubscription;
    });

    if (usedSubscription->monitoredItemsCount() == 0)
        removeSubscription(usedSubscription->subscriptionId()); // No items were added

    modifyPublishRequests();
}

void Open62541AsyncBackend::disableMonitoring(uintptr_t handle, QOpcUaNode::NodeAttributes attr)
{
    qt_forEachAttribute(attr, [&](QOpcUaNode::NodeAttribute attribute){
        QOpen62541Subscription *sub = getSubscriptionForItem(handle, attribute);
        if (sub) {
            sub->removeAttributeMonitoredItem(handle, attribute);
            if (sub->monitoredItemsCount() == 0)
                removeSubscription(sub->subscriptionId());
        }
    });
    modifyPublishRequests();
}

void Open62541AsyncBackend::modifyMonitoring(uintptr_t handle, QOpcUaNode::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, QVariant value)
{
    QOpen62541Subscription *subscription = getSubscriptionForItem(handle, attr);
    if (!subscription) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Could not modify parameter for %lu, the monitored item does not exist", handle);
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
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Could not call method: 0x%X", res);

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

static UA_StatusCode nodeIter(UA_NodeId childId, UA_Boolean isInverse, UA_NodeId referenceTypeId, void *pass)
{
    Q_UNUSED(referenceTypeId);
    if (isInverse)
        return UA_STATUSCODE_GOOD;

    // ### TODO: Question: Is it actually correct to skip these
    UA_NodeId temp = UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE);
    if (UA_NodeId_equal(&childId, &temp))
        return UA_STATUSCODE_GOOD;

    temp = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE);
    if (UA_NodeId_equal(&childId, &temp))
        return UA_STATUSCODE_GOOD;

    auto back = static_cast<QStringList *>(pass);

    QString childName;
    if (childId.identifierType == UA_NODEIDTYPE_NUMERIC) {
        childName = QStringLiteral("ns=%1;i=%2").arg(childId.namespaceIndex).arg(childId.identifier.numeric);
    } else if (childId.identifierType == UA_NODEIDTYPE_STRING) {
        childName = QStringLiteral("ns=%1;s=%2").arg(childId.namespaceIndex).arg(
                    QString::fromUtf8(reinterpret_cast<char *>(childId.identifier.string.data), childId.identifier.string.length));
    } else if (childId.identifierType == UA_NODEIDTYPE_GUID) {
        UA_Guid tempId = childId.identifier.guid;
        const QUuid uuid(tempId.data1, tempId.data2, tempId.data3, tempId.data4[0], tempId.data4[1], tempId.data4[2],
                tempId.data4[3], tempId.data4[4], tempId.data4[5], tempId.data4[6], tempId.data4[7]);
        childName = QStringLiteral("ns=%1;g=").arg(childId.namespaceIndex).append(uuid.toString().midRef(1, 36)); // Remove enclosing {...}
    } else if (childId.identifierType == UA_NODEIDTYPE_BYTESTRING) {
        const QString base64String = QByteArray(reinterpret_cast<char *>(childId.identifier.byteString.data),
                                          childId.identifier.byteString.length).toBase64();
        childName = QStringLiteral("ns=%1;b=").arg(childId.namespaceIndex).append(base64String);
    }
    else {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Skipping child with unsupported nodeid type";
    }

    if (!childName.isEmpty())
        back->append(childName);

    return UA_STATUSCODE_GOOD;
}

QStringList Open62541AsyncBackend::childrenIds(const UA_NodeId *parentNode)
{
    QStringList result;
    UA_StatusCode sc = UA_Client_forEachChildNodeCall(m_uaclient, *parentNode, nodeIter, &result);
    if (sc != UA_STATUSCODE_GOOD)
        return QStringList();

    return result;
}

void Open62541AsyncBackend::connectToEndpoint(const QUrl &url)
{
    m_uaclient = UA_Client_new(UA_ClientConfig_default);
    UA_StatusCode ret;

    if (url.userName().length()) {
        QUrl temp = url;
        const QString userName = temp.userName();
        const QString password = temp.password();
        temp.setPassword(QString());
        temp.setUserName(QString());
        ret = UA_Client_connect_username(m_uaclient, temp.toString().toUtf8().constData(), userName.toUtf8().constData(), password.toUtf8().constData());
    } else {
        ret = UA_Client_connect(m_uaclient, url.toString().toUtf8().constData());
    }

    if (ret != UA_STATUSCODE_GOOD) {
        UA_Client_delete(m_uaclient);
        m_uaclient = nullptr;
        QOpcUaClient::ClientError error = ret == UA_STATUSCODE_BADUSERACCESSDENIED ? QOpcUaClient::AccessDenied : QOpcUaClient::UnknownError;
        emit m_clientImpl->stateAndOrErrorChanged(QOpcUaClient::Disconnected, error);
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Open62541: Failed to connect.");
        return;
    }

    emit stateAndOrErrorChanged(QOpcUaClient::Connected, QOpcUaClient::NoError);
}

void Open62541AsyncBackend::disconnectFromEndpoint()
{
    UA_StatusCode ret = UA_Client_disconnect(m_uaclient);
    if (ret != UA_STATUSCODE_GOOD) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Open62541: Failed to disconnect.");
        // Fall through intentionally
    }

    UA_Client_delete(m_uaclient);
    m_uaclient = nullptr;
    m_subscriptionTimer.stop();
    emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::NoError);
}

void Open62541AsyncBackend::sendPublishRequest()
{
    if (!m_uaclient)
        return;

    if (!m_sendPublishRequests) {
        return;
    }

    if (UA_Client_Subscriptions_manuallySendPublishRequest(m_uaclient) == UA_STATUSCODE_BADSERVERNOTCONNECTED) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Unable to send publish request");
        m_sendPublishRequests = false;
        return;
    }

    m_subscriptionTimer.start(m_shortestInterval);
}

void Open62541AsyncBackend::modifyPublishRequests()
{
    if (m_subscriptions.count() == 0) {
        m_subscriptionTimer.stop();
        m_sendPublishRequests = false;
        m_shortestInterval = std::numeric_limits<qint32>::max();
        return;
    }

    for (auto it : qAsConst(m_subscriptions))
        if (it->interval() < m_shortestInterval)
            m_shortestInterval = it->interval();

    m_subscriptionTimer.stop();
    m_sendPublishRequests = true;
    sendPublishRequest();
}

QOpen62541Subscription *Open62541AsyncBackend::getSubscriptionForItem(uintptr_t handle, QOpcUaNode::NodeAttribute attr)
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

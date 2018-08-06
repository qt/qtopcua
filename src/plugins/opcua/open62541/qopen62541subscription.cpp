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
#include "qopen62541client.h"
#include "qopen62541node.h"
#include "qopen62541subscription.h"
#include "qopen62541valueconverter.h"
#include <private/qopcuanode_p.h>

#include <QtCore/qloggingcategory.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_OPEN62541)

static void monitoredValueHandler(UA_Client *client, UA_UInt32 subId, void *subContext, UA_UInt32 monId, void *monContext, UA_DataValue *value)
{
    Q_UNUSED(client)
    Q_UNUSED(subId)
    Q_UNUSED(subContext)
    QOpen62541Subscription *subscription = static_cast<QOpen62541Subscription *>(monContext);
    subscription->monitoredValueUpdated(monId, value);
}

static void stateChangeHandler(UA_Client *client, UA_UInt32 subId, void *subContext, UA_StatusChangeNotification *notification)
{
    Q_UNUSED(client);
    Q_UNUSED(subId);

    if (notification->status != UA_STATUSCODE_BADTIMEOUT)
        return;

    QOpen62541Subscription *sub = static_cast<QOpen62541Subscription *>(subContext);
    sub->sendTimeoutNotification();
}

QOpen62541Subscription::QOpen62541Subscription(Open62541AsyncBackend *backend, const QOpcUaMonitoringParameters &settings)
    : m_backend(backend)
    , m_interval(settings.publishingInterval())
    , m_subscriptionId(0)
    , m_lifetimeCount(settings.lifetimeCount() ? settings.lifetimeCount() : UA_CreateSubscriptionRequest_default().requestedLifetimeCount)
    , m_maxKeepaliveCount(settings.maxKeepAliveCount() ? settings.maxKeepAliveCount() : UA_CreateSubscriptionRequest_default().requestedMaxKeepAliveCount)
    , m_shared(settings.shared())
    , m_priority(settings.priority())
    , m_maxNotificationsPerPublish(settings.maxNotificationsPerPublish())
    , m_clientHandle(0)
    , m_timeout(false)
{
}

QOpen62541Subscription::~QOpen62541Subscription()
{
    removeOnServer();
}

UA_UInt32 QOpen62541Subscription::createOnServer()
{
    UA_CreateSubscriptionRequest req = UA_CreateSubscriptionRequest_default();
    req.requestedPublishingInterval = m_interval;
    req.requestedLifetimeCount = m_lifetimeCount;
    req.requestedMaxKeepAliveCount = m_maxKeepaliveCount;
    req.priority = m_priority;
    req.maxNotificationsPerPublish = m_maxNotificationsPerPublish;
    UA_CreateSubscriptionResponse res = UA_Client_Subscriptions_create(m_backend->m_uaclient, req, this, stateChangeHandler, NULL);

    if (res.responseHeader.serviceResult != UA_STATUSCODE_GOOD) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not create subscription with interval" << m_interval << UA_StatusCode_name(res.responseHeader.serviceResult);
        return 0;
    }

    m_subscriptionId = res.subscriptionId;
    m_maxKeepaliveCount = res.revisedMaxKeepAliveCount;
    m_lifetimeCount = res.revisedLifetimeCount;
    m_interval = res.revisedPublishingInterval;
    return m_subscriptionId;
}

bool QOpen62541Subscription::removeOnServer()
{
    UA_StatusCode res = UA_STATUSCODE_GOOD;
    if (m_subscriptionId) {
        res = UA_Client_Subscriptions_deleteSingle(m_backend->m_uaclient, m_subscriptionId);
        m_subscriptionId = 0;
    }

    for (auto it : qAsConst(m_itemIdToItemMapping)) {
        QOpcUaMonitoringParameters s;
        s.setStatusCode(m_timeout ? QOpcUa::UaStatusCode::BadTimeout : QOpcUa::UaStatusCode::BadDisconnect);
        emit m_backend->monitoringEnableDisable(it->handle, it->attr, false, s);
    }

    qDeleteAll(m_itemIdToItemMapping);

    m_itemIdToItemMapping.clear();
    m_handleToItemMapping.clear();

    return (res == UA_STATUSCODE_GOOD) ? true : false;
}

void QOpen62541Subscription::modifyMonitoring(uintptr_t handle, QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, QVariant value)
{
    QOpcUaMonitoringParameters p;
    p.setStatusCode(QOpcUa::UaStatusCode::BadNotImplemented);

    MonitoredItem *monItem = getItemForAttribute(handle, attr);
    if (!monItem) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify parameter for" << handle << ", there are no monitored items";
        p.setStatusCode(QOpcUa::UaStatusCode::BadAttributeIdInvalid);
        emit m_backend->monitoringStatusChanged(handle, attr, item, p);
        return;
    }

    p = monItem->parameters;

    // SetPublishingMode service
    if (item == QOpcUaMonitoringParameters::Parameter::PublishingEnabled) {
        if (value.type() != QVariant::Bool) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "New value for PublishingEnabled is not a boolean";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return;
        }

        UA_SetPublishingModeRequest req;
        UA_SetPublishingModeRequest_init(&req);
        req.publishingEnabled = value.toBool();
        req.subscriptionIdsSize = 1;
        req.subscriptionIds = UA_UInt32_new();
        *req.subscriptionIds = m_subscriptionId;
        UA_SetPublishingModeResponse res = UA_Client_Subscriptions_setPublishingMode(m_backend->m_uaclient, req);

        if (res.responseHeader.serviceResult != UA_STATUSCODE_GOOD) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to set publishing mode:" << res.responseHeader.serviceResult;
            p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult));
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return;
        }

        if (res.resultsSize && res.results[0] == UA_STATUSCODE_GOOD)
            p.setPublishingEnabled(value.toBool());

        p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(res.results[0]));
        emit m_backend->monitoringStatusChanged(handle, attr, item, p);

        UA_SetPublishingModeRequest_deleteMembers(&req);
        UA_SetPublishingModeResponse_deleteMembers(&res);
        return;
    }

    // SetMonitoringMode service
    if (item == QOpcUaMonitoringParameters::Parameter::MonitoringMode) {
        if (value.type() != QVariant::UserType || value.userType() != QMetaType::type("QOpcUaMonitoringParameters::MonitoringMode")) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "New value for MonitoringMode is not a monitoring mode";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return;
        }

        UA_SetMonitoringModeRequest req;
        UA_SetMonitoringModeRequest_init(&req);
        req.monitoringMode = static_cast<UA_MonitoringMode>(value.value<QOpcUaMonitoringParameters::MonitoringMode>());
        req.monitoredItemIdsSize = 1;
        req.monitoredItemIds = UA_UInt32_new();
        *req.monitoredItemIds = monItem->monitoredItemId;
        req.subscriptionId = m_subscriptionId;
        UA_SetMonitoringModeResponse res = UA_Client_MonitoredItems_setMonitoringMode(m_backend->m_uaclient, req);

        if (res.responseHeader.serviceResult != UA_STATUSCODE_GOOD) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to set monitoring mode:" << res.responseHeader.serviceResult;
            p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult));
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return;
        }

        if (res.resultsSize && res.results[0] == UA_STATUSCODE_GOOD)
            p.setMonitoringMode(value.value<QOpcUaMonitoringParameters::MonitoringMode>());

        p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(res.results[0]));
        emit m_backend->monitoringStatusChanged(handle, attr, item, p);

        UA_SetMonitoringModeRequest_deleteMembers(&req);
        UA_SetMonitoringModeResponse_deleteMembers(&res);
        return;
    }

    if (modifySubscriptionParameters(handle, attr, item, value))
        return;
    if (modifyMonitoredItemParameters(handle, attr, item, value))
        return;

    qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Modifying" << item << "is not implemented";
    p.setStatusCode(QOpcUa::UaStatusCode::BadNotImplemented);
    emit m_backend->monitoringStatusChanged(handle, attr, item, p);
}

bool QOpen62541Subscription::addAttributeMonitoredItem(uintptr_t handle, QOpcUa::NodeAttribute attr, const UA_NodeId &id, QOpcUaMonitoringParameters settings)
{
    UA_MonitoredItemCreateRequest req;
    UA_MonitoredItemCreateRequest_init(&req);
    req.itemToMonitor.attributeId = QOpen62541ValueConverter::toUaAttributeId(attr);
    UA_NodeId_copy(&id, &(req.itemToMonitor.nodeId));
    if (settings.indexRange().size())
        req.itemToMonitor.indexRange = UA_STRING_ALLOC(settings.indexRange().toUtf8().data());
    req.monitoringMode = static_cast<UA_MonitoringMode>(settings.monitoringMode());
    req.requestedParameters.samplingInterval = qFuzzyCompare(settings.samplingInterval(), 0.0) ? m_interval : settings.samplingInterval();
    req.requestedParameters.queueSize = settings.queueSize() == 0 ? 1 : settings.queueSize();
    req.requestedParameters.discardOldest = settings.discardOldest();
    req.requestedParameters.clientHandle = ++m_clientHandle;
    if (settings.filter().type() == QVariant::UserType && settings.filter().userType() == QMetaType::type("QOpcUaMonitoringParameters::DataChangeFilter"))
        req.requestedParameters.filter = createFilter(settings.filter());

    UA_MonitoredItemCreateResult res = UA_Client_MonitoredItems_createDataChange(m_backend->m_uaclient, m_subscriptionId, UA_TIMESTAMPSTORETURN_BOTH, req, this, monitoredValueHandler, nullptr);

    UA_MonitoredItemCreateRequest_deleteMembers(&req);

    if (res.statusCode != UA_STATUSCODE_GOOD) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not add monitored item to subscription" << m_subscriptionId << ":" << UA_StatusCode_name(res.statusCode);
        QOpcUaMonitoringParameters s;
        s.setStatusCode(static_cast<QOpcUa::UaStatusCode>(res.statusCode));
        emit m_backend->monitoringEnableDisable(handle, attr, true, s);
        return false;
    }

    MonitoredItem *temp = new MonitoredItem(handle, attr, res.monitoredItemId);
    m_handleToItemMapping[handle][attr] = temp;
    m_itemIdToItemMapping[res.monitoredItemId] = temp;

    QOpcUaMonitoringParameters s = settings;
    s.setSubscriptionId(m_subscriptionId);
    s.setPublishingInterval(m_interval);
    s.setMaxKeepAliveCount(m_maxKeepaliveCount);
    s.setLifetimeCount(m_lifetimeCount);
    s.setStatusCode(QOpcUa::UaStatusCode::Good);
    s.setSamplingInterval(res.revisedSamplingInterval);
    s.setQueueSize(res.revisedQueueSize);
    temp->parameters = s;
    temp->clientHandle = m_clientHandle;

    s.setFilter(QVariant());
    emit m_backend->monitoringEnableDisable(handle, attr, true, s);

    return true;
}

bool QOpen62541Subscription::removeAttributeMonitoredItem(uintptr_t handle, QOpcUa::NodeAttribute attr)
{
    MonitoredItem *item = getItemForAttribute(handle, attr);
    if (!item) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "There is no monitored item for this attribute";
        QOpcUaMonitoringParameters s;
        s.setStatusCode(QOpcUa::UaStatusCode::BadMonitoredItemIdInvalid);
        emit m_backend->monitoringEnableDisable(handle, attr, false, s);
        return false;
    }

    UA_StatusCode res = UA_Client_MonitoredItems_deleteSingle(m_backend->m_uaclient, m_subscriptionId, item->monitoredItemId);
    if (res != UA_STATUSCODE_GOOD)
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not remove monitored item" << item->monitoredItemId << "from subscription" << m_subscriptionId << ":" << UA_StatusCode_name(res);

    m_itemIdToItemMapping.remove(item->monitoredItemId);
    auto it = m_handleToItemMapping.find(handle);
    it->remove(attr);
    if (it->empty())
        m_handleToItemMapping.erase(it);

    delete item;

    QOpcUaMonitoringParameters s;
    s.setStatusCode(static_cast<QOpcUa::UaStatusCode>(res));
    emit m_backend->monitoringEnableDisable(handle, attr, false, s);

    return true;
}

void QOpen62541Subscription::monitoredValueUpdated(UA_UInt32 monId, UA_DataValue *value)
{
    auto item = m_itemIdToItemMapping.constFind(monId);
    if (item == m_itemIdToItemMapping.constEnd())
        return;
    QOpcUaReadResult res;

    if (!value || value == UA_EMPTY_ARRAY_SENTINEL) {
        res.statusCode = QOpcUa::UaStatusCode::Good;
        emit m_backend->attributeUpdated(item.value()->handle, res);
        return;
    }

    res.value = QOpen62541ValueConverter::toQVariant(value->value);
    res.attributeId = item.value()->attr;
    if (value->hasServerTimestamp)
        res.serverTimestamp = QOpen62541ValueConverter::uaDateTimeToQDateTime(value->serverTimestamp);
    if (value->hasSourceTimestamp)
        res.sourceTimestamp = QOpen62541ValueConverter::uaDateTimeToQDateTime(value->sourceTimestamp);
    res.statusCode = QOpcUa::UaStatusCode::Good;
    emit m_backend->attributeUpdated(item.value()->handle, res);
}

void QOpen62541Subscription::sendTimeoutNotification()
{
    QVector<QPair<uintptr_t, QOpcUa::NodeAttribute>> items;
    for (auto it : qAsConst(m_handleToItemMapping)) {
        for (auto item : it) {
            items.push_back({item->handle, item->attr});
        }
    }
    emit timeout(this, items);
    m_timeout = true;
}

double QOpen62541Subscription::interval() const
{
    return m_interval;
}

UA_UInt32 QOpen62541Subscription::subscriptionId() const
{
    return m_subscriptionId;
}

int QOpen62541Subscription::monitoredItemsCount() const
{
    return m_itemIdToItemMapping.size();
}

QOpcUaMonitoringParameters::SubscriptionType QOpen62541Subscription::shared() const
{
    return m_shared;
}

QOpen62541Subscription::MonitoredItem *QOpen62541Subscription::getItemForAttribute(uintptr_t handle, QOpcUa::NodeAttribute attr)
{
    auto nodeEntry = m_handleToItemMapping.constFind(handle);

    if (nodeEntry == m_handleToItemMapping.constEnd())
        return nullptr;

    auto item = nodeEntry->constFind(attr);
    if (item == nodeEntry->constEnd())
        return nullptr;

    return item.value();
}

UA_ExtensionObject QOpen62541Subscription::createFilter(const QVariant &filterData)
{
    UA_ExtensionObject obj;
    UA_ExtensionObject_init(&obj);

    if (filterData.type() == QVariant::UserType && filterData.userType() == QMetaType::type("QOpcUaMonitoringParameters::DataChangeFilter")) {
        QOpcUaMonitoringParameters::DataChangeFilter temp = filterData.value<QOpcUaMonitoringParameters::DataChangeFilter>();
        UA_DataChangeFilter *filter = UA_DataChangeFilter_new();
        filter->deadbandType = static_cast<UA_UInt32>(temp.deadbandType);
        filter->deadbandValue = temp.deadbandValue;
        filter->trigger = static_cast<UA_DataChangeTrigger>(temp.trigger);
        obj.encoding = UA_EXTENSIONOBJECT_DECODED;
        obj.content.decoded.type = &UA_TYPES[UA_TYPES_DATACHANGEFILTER];
        obj.content.decoded.data = filter;
        return obj;
    }

    if (filterData.isValid())
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not create filter, invalid input.";

    return obj;
}

bool QOpen62541Subscription::modifySubscriptionParameters(uintptr_t handle, QOpcUa::NodeAttribute attr, const QOpcUaMonitoringParameters::Parameter &item, const QVariant &value)
{
    QOpcUaMonitoringParameters p;

    UA_ModifySubscriptionRequest req;
    UA_ModifySubscriptionRequest_init(&req);
    req.subscriptionId = m_subscriptionId;
    req.requestedPublishingInterval = m_interval;
    req.requestedLifetimeCount = m_lifetimeCount;
    req.requestedMaxKeepAliveCount = m_maxKeepaliveCount;
    req.maxNotificationsPerPublish = m_maxNotificationsPerPublish;

    bool match = true;

    switch (item) {
    case QOpcUaMonitoringParameters::Parameter::PublishingInterval: {
        bool ok;
        req.requestedPublishingInterval = value.toDouble(&ok);
        if (!ok) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify PublishingInterval for" << handle << ", value is not a double";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return true;
        }
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::LifetimeCount: {
        bool ok;
        req.requestedLifetimeCount = value.toUInt(&ok);
        if (!ok) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify LifetimeCount for" << handle << ", value is not an integer";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return true;
        }
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::MaxKeepAliveCount: {
        bool ok;
        req.requestedMaxKeepAliveCount = value.toUInt(&ok);
        if (!ok) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify MaxKeepAliveCount for" << handle << ", value is not an integer";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return true;
        }
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::Priority: {
        bool ok;
        req.priority = value.toUInt(&ok);
        if (!ok) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify Priority for" << handle << ", value is not an integer";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return true;
        }
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::MaxNotificationsPerPublish: {
        bool ok;
        req.maxNotificationsPerPublish = value.toUInt(&ok);
        if (!ok) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify MaxNotificationsPerPublish for" << handle << ", value is not an integer";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return true;
        }
        break;
    }
    default:
        match = false;
        break;
    }

    if (match) {
        UA_ModifySubscriptionResponse res = UA_Client_Subscriptions_modify(m_backend->m_uaclient, req);

        if (res.responseHeader.serviceResult != UA_STATUSCODE_GOOD) {
            p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult));
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
        } else {
            QOpcUaMonitoringParameters::Parameters changed = item;
            if (!qFuzzyCompare(p.publishingInterval(), m_interval))
                changed |= QOpcUaMonitoringParameters::Parameter::PublishingInterval;
            if (p.lifetimeCount() != m_lifetimeCount)
                changed |= QOpcUaMonitoringParameters::Parameter::LifetimeCount;
            if (p.maxKeepAliveCount() != m_maxKeepaliveCount)
                changed |= QOpcUaMonitoringParameters::Parameter::MaxKeepAliveCount;

            m_lifetimeCount = res.revisedLifetimeCount;
            m_maxKeepaliveCount = res.revisedMaxKeepAliveCount;
            m_interval = res.revisedPublishingInterval;
            if (item == QOpcUaMonitoringParameters::Parameter::Priority)
                m_priority = value.toUInt();
            if (item == QOpcUaMonitoringParameters::Parameter::MaxNotificationsPerPublish)
                m_maxNotificationsPerPublish = value.toUInt();

            p.setStatusCode(QOpcUa::UaStatusCode::Good);
            p.setPublishingInterval(m_interval);
            p.setLifetimeCount(m_lifetimeCount);
            p.setMaxKeepAliveCount(m_maxKeepaliveCount);
            p.setPriority(m_priority);
            p.setMaxNotificationsPerPublish(m_maxNotificationsPerPublish);

            for (auto it : qAsConst(m_itemIdToItemMapping))
                emit m_backend->monitoringStatusChanged(it->handle, it->attr, changed, p);
        }
        return true;
    }
    return false;
}

bool QOpen62541Subscription::modifyMonitoredItemParameters(uintptr_t handle, QOpcUa::NodeAttribute attr, const QOpcUaMonitoringParameters::Parameter &item, const QVariant &value)
{
    MonitoredItem *monItem = getItemForAttribute(handle, attr);
    QOpcUaMonitoringParameters p = monItem->parameters;

    UA_ModifyMonitoredItemsRequest req;
    UA_ModifyMonitoredItemsRequest_init(&req);
    req.subscriptionId = m_subscriptionId;
    req.itemsToModifySize = 1;
    req.itemsToModify = UA_MonitoredItemModifyRequest_new();
    UA_MonitoredItemModifyRequest_init(req.itemsToModify);
    req.itemsToModify->monitoredItemId = monItem->monitoredItemId;
    req.itemsToModify->requestedParameters.discardOldest = monItem->parameters.discardOldest();
    req.itemsToModify->requestedParameters.queueSize = monItem->parameters.queueSize();
    req.itemsToModify->requestedParameters.samplingInterval = monItem->parameters.samplingInterval();
    req.itemsToModify->monitoredItemId = monItem->monitoredItemId;
    req.itemsToModify->requestedParameters.clientHandle = monItem->clientHandle;
    if (item != QOpcUaMonitoringParameters::Parameter::Filter)
        req.itemsToModify->requestedParameters.filter = createFilter(monItem->parameters.filter());

    bool match = true;

    switch (item) {
    case QOpcUaMonitoringParameters::Parameter::DiscardOldest: {
        if (value.type() != QVariant::Bool) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify DiscardOldest for" << handle << ", value is not a bool";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            UA_ModifyMonitoredItemsRequest_deleteMembers(&req);
            return true;
        }
        req.itemsToModify->requestedParameters.discardOldest = value.toBool();
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::QueueSize: {
        if (value.type() != QVariant::UInt) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify QueueSize for" << handle << ", value is not an integer";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            UA_ModifyMonitoredItemsRequest_deleteMembers(&req);
            return true;
        }
        req.itemsToModify->requestedParameters.queueSize = value.toUInt();
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::SamplingInterval: {
        if (value.type() != QVariant::Double) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify SamplingInterval for" << handle << ", value is not a double";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            UA_ModifyMonitoredItemsRequest_deleteMembers(&req);
            return true;
        }
        req.itemsToModify->requestedParameters.samplingInterval = value.toDouble();
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::Filter: {
        req.itemsToModify->requestedParameters.filter = createFilter(value);
        break;
    }
    default:
        match = false;
        break;
    }

    if (match) {
        UA_ModifyMonitoredItemsResponse res = UA_Client_MonitoredItems_modify(m_backend->m_uaclient, req);

        if (res.responseHeader.serviceResult != UA_STATUSCODE_GOOD || res.results[0].statusCode != UA_STATUSCODE_GOOD) {
            p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult == UA_STATUSCODE_GOOD ? res.results[0].statusCode : res.responseHeader.serviceResult));
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            UA_ModifyMonitoredItemsRequest_deleteMembers(&req);
            UA_ModifyMonitoredItemsResponse_deleteMembers(&res);
            return true;
        } else {
            p.setStatusCode(QOpcUa::UaStatusCode::Good);
            QOpcUaMonitoringParameters::Parameters changed = item;
            if (!qFuzzyCompare(p.samplingInterval(), res.results[0].revisedSamplingInterval)) {
                p.setSamplingInterval(res.results[0].revisedSamplingInterval);
                changed |= QOpcUaMonitoringParameters::Parameter::SamplingInterval;
            }
            if (p.queueSize() != res.results[0].revisedQueueSize) {
                p.setQueueSize(res.results[0].revisedQueueSize);
                changed |= QOpcUaMonitoringParameters::Parameter::QueueSize;
            }

            if (item == QOpcUaMonitoringParameters::Parameter::DiscardOldest) {
                p.setDiscardOldest(value.toBool());
                changed |= QOpcUaMonitoringParameters::Parameter::DiscardOldest;
            }

            emit m_backend->monitoringStatusChanged(handle, attr, changed, p);
            monItem->parameters = p;
            UA_ModifyMonitoredItemsRequest_deleteMembers(&req);
            UA_ModifyMonitoredItemsResponse_deleteMembers(&res);
        }
        return true;
    }

    UA_ModifyMonitoredItemsRequest_deleteMembers(&req);

    return false;
}

QT_END_NAMESPACE

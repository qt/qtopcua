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

static void monitoredValueHandler(UA_UInt32 monId, UA_DataValue *value, void *context)
{
    QOpen62541Subscription *subscription = static_cast<QOpen62541Subscription *>(context);
    subscription->monitoredValueUpdated(monId, value);
}

QOpen62541Subscription::QOpen62541Subscription(Open62541AsyncBackend *backend, const QOpcUaMonitoringParameters &settings)
    : m_backend(backend)
    , m_interval(settings.publishingInterval())
    , m_subscriptionId(0)
    , m_lifetimeCount(settings.lifetimeCount() ? settings.lifetimeCount() : UA_SubscriptionSettings_default.requestedLifetimeCount)
    , m_maxKeepaliveCount(settings.maxKeepAliveCount() ? settings.maxKeepAliveCount() : UA_SubscriptionSettings_default.requestedMaxKeepAliveCount)
    , m_shared(settings.shared())
    , m_priority(settings.priority())
{
}

QOpen62541Subscription::~QOpen62541Subscription()
{
    removeOnServer();
    qDeleteAll(m_itemIdToItemMapping);
}

UA_UInt32 QOpen62541Subscription::createOnServer()
{
    UA_UInt32 subscriptionId = 0;
    UA_SubscriptionSettings settings = UA_SubscriptionSettings_default;
    settings.requestedPublishingInterval = m_interval;
    settings.requestedLifetimeCount = m_lifetimeCount;
    settings.requestedMaxKeepAliveCount = m_maxKeepaliveCount;
    settings.priority = m_priority;
    UA_StatusCode res = UA_Client_Subscriptions_new(m_backend->m_uaclient, settings, &subscriptionId);

    if (res != UA_STATUSCODE_GOOD) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Could not create subscription with interval %f: 0x%X", m_interval, res);
        return 0;
    }

    m_subscriptionId = subscriptionId;
    return subscriptionId;
}

bool QOpen62541Subscription::removeOnServer()
{
    if (m_subscriptionId == 0)
        return false;

    UA_StatusCode res = UA_Client_Subscriptions_remove(m_backend->m_uaclient, m_subscriptionId);
    m_subscriptionId = 0;

    return (res == UA_STATUSCODE_GOOD) ? true : false;
}

void QOpen62541Subscription::modifyMonitoring(uintptr_t handle, QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, QVariant value)
{
    QOpcUaMonitoringParameters p;
    p.setStatusCode(QOpcUa::UaStatusCode::BadNotImplemented);

    if (!getItemForAttribute(handle, attr)) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Could not modify parameter for %lu, there are no monitored items", handle);
        p.setStatusCode(QOpcUa::UaStatusCode::BadAttributeIdInvalid);
        emit m_backend->monitoringStatusChanged(handle, attr, item, p);
        return;
    }

    // SetPublishingMode service
    if (item == QOpcUaMonitoringParameters::Parameter::PublishingEnabled) {
        emit m_backend->monitoringStatusChanged(handle, attr, item, p);
        return;
    }

    // SetMonitoringMode service
    if (item == QOpcUaMonitoringParameters::Parameter::MonitoringMode) {
        emit m_backend->monitoringStatusChanged(handle, attr, item, p);
        return;
    }

    // ModifySubscription service
    {
        UA_ModifySubscriptionRequest req;
        UA_ModifySubscriptionRequest_init(&req);
        req.subscriptionId = m_subscriptionId;
        req.requestedPublishingInterval = m_interval;
        req.requestedLifetimeCount = m_lifetimeCount;
        req.requestedMaxKeepAliveCount = m_maxKeepaliveCount;

        bool match = false;

        switch (item) {
        case QOpcUaMonitoringParameters::Parameter::PublishingInterval: {
            bool ok;
            req.requestedPublishingInterval = value.toDouble(&ok);
            if (!ok) {
                qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Could not modify PublishingInterval for %lu, value is not a double", handle);
                p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
                emit m_backend->monitoringStatusChanged(handle, attr, item, p);
                return;
            }
            match = true;
            break;
        }
        case QOpcUaMonitoringParameters::Parameter::LifetimeCount: {
            bool ok;
            req.requestedLifetimeCount = value.toUInt(&ok);
            if (!ok) {
                qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Could not modify LifetimeCount for %lu, value is not an integer", handle);
                p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
                emit m_backend->monitoringStatusChanged(handle, attr, item, p);
                return;
            }
            match = true;
            break;
        }
        case QOpcUaMonitoringParameters::Parameter::MaxKeepAliveCount: {
            bool ok;
            req.requestedMaxKeepAliveCount = value.toUInt(&ok);
            if (!ok) {
                qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Could not modify MaxKeepAliveCount for %lu, value is not an integer", handle);
                p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
                emit m_backend->monitoringStatusChanged(handle, attr, item, p);
                return;
            }
            match = true;
            break;
        }
        default:
            break;
        }

        if (match) {
            UA_ModifySubscriptionResponse res = UA_Client_Service_modifySubscription(m_backend->m_uaclient, req);

            if (res.responseHeader.serviceResult != UA_STATUSCODE_GOOD) {
                p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult));
                emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            } else {
                p.setStatusCode(QOpcUa::UaStatusCode::Good);
                p.setPublishingInterval(res.revisedPublishingInterval);
                p.setLifetimeCount(res.revisedLifetimeCount);
                p.setMaxKeepAliveCount(res.revisedMaxKeepAliveCount);

                QOpcUaMonitoringParameters::Parameters changed = item;
                if (!qFuzzyCompare(p.publishingInterval(), m_interval))
                    changed |= QOpcUaMonitoringParameters::Parameter::PublishingInterval;
                if (p.lifetimeCount() != m_lifetimeCount)
                    changed |= QOpcUaMonitoringParameters::Parameter::LifetimeCount;
                if (p.maxKeepAliveCount() != m_maxKeepaliveCount)
                    changed |= QOpcUaMonitoringParameters::Parameter::MaxKeepAliveCount;

                for (auto it : qAsConst(m_itemIdToItemMapping))
                    emit m_backend->monitoringStatusChanged(it->handle, it->attr, changed, p);

                m_lifetimeCount = res.revisedLifetimeCount;
                m_maxKeepaliveCount = res.revisedMaxKeepAliveCount;
                m_interval = res.revisedPublishingInterval;
            }
            return;
        }
    }

    // ModifyMonitoredItems service
    {
        // TODO: Add support as soon as Open62541 supports this.
    }

    qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Revising attribute is not implemented:" << item;
    p.setStatusCode(QOpcUa::UaStatusCode::BadNotImplemented);
    emit m_backend->monitoringStatusChanged(handle, attr, item, p);
}

bool QOpen62541Subscription::addAttributeMonitoredItem(uintptr_t handle, QOpcUa::NodeAttribute attr, const UA_NodeId &id, QOpcUaMonitoringParameters settings)
{
    Q_UNUSED(settings); // This is for later applications like including parameters for the monitored item into settings
    UA_UInt32 monitoredItemId = 0;
    UA_StatusCode ret = UA_Client_Subscriptions_addMonitoredItem(m_backend->m_uaclient, m_subscriptionId, id,
                                                                 QOpen62541ValueConverter::toUaAttributeId(attr),
                                                                 monitoredValueHandler, this, &monitoredItemId);

    if (ret != UA_STATUSCODE_GOOD) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Could not add monitored item to subscription %u: 0x%X", m_subscriptionId, ret);
        QOpcUaMonitoringParameters s;
        s.setStatusCode(static_cast<QOpcUa::UaStatusCode>(ret));
        emit m_backend->monitoringEnableDisable(handle, attr, true, s);
        return false;
    }

    MonitoredItem *temp = new MonitoredItem(handle, attr, monitoredItemId);
    m_handleToItemMapping[handle][attr] = temp;
    m_itemIdToItemMapping[monitoredItemId] = temp;

    QOpcUaMonitoringParameters s;
    s.setSubscriptionId(m_subscriptionId);
    s.setPublishingInterval(m_interval);
    s.setMaxKeepAliveCount(m_maxKeepaliveCount);
    s.setLifetimeCount(m_lifetimeCount);
    s.setStatusCode(static_cast<QOpcUa::UaStatusCode>(ret));
    s.setSamplingInterval(m_interval);
    emit m_backend->monitoringEnableDisable(handle, attr, true, s);

    return true;
}

bool QOpen62541Subscription::removeAttributeMonitoredItem(uintptr_t handle, QOpcUa::NodeAttribute attr)
{
    MonitoredItem *item = getItemForAttribute(handle, attr);
    if (!item) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "There is no monitored item for this attribute");
        QOpcUaMonitoringParameters s;
        s.setStatusCode(QOpcUa::UaStatusCode::BadMonitoredItemIdInvalid);
        emit m_backend->monitoringEnableDisable(handle, attr, false, s);
        return false;
    }

    UA_StatusCode res = UA_Client_Subscriptions_removeMonitoredItem(m_backend->m_uaclient, m_subscriptionId, item->monitoredItemId);
    if (res != UA_STATUSCODE_GOOD)
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Could not remove monitored item %u from subscription %u: 0x%X",
                  item->monitoredItemId, m_subscriptionId, res);

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
    emit m_backend->attributeUpdated(item.value()->handle, item.value()->attr, QOpen62541ValueConverter::toQVariant(value->value));
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

QT_END_NAMESPACE

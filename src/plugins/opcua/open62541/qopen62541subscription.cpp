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
#include <private/qopcuamonitoredvalue_p.h>
#include <private/qopcuanode_p.h>

#include <QtCore/qloggingcategory.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_OPEN62541)

static void monitoredValueHandler(UA_UInt32 monId, UA_DataValue *value, void *context)
{
    QOpen62541Subscription *subscription = static_cast<QOpen62541Subscription *>(context);
    subscription->monitoredValueUpdated(monId, value);
}

QOpen62541Subscription::QOpen62541Subscription(Open62541AsyncBackend *backend, quint32 interval)
    : m_backend(backend)
    , m_interval(interval)
    , m_subscriptionId(0)
{
}

QOpen62541Subscription::~QOpen62541Subscription()
{
    removeNativeSubscription();
}

QOpcUaMonitoredEvent *QOpen62541Subscription::addEvent(QOpcUaNode *node)
{
    Q_UNUSED(node);
    Q_UNIMPLEMENTED();
    return nullptr;
}

void QOpen62541Subscription::removeEvent(QOpcUaMonitoredEvent *event)
{
    Q_UNUSED(event);
    Q_UNIMPLEMENTED();
}

QOpcUaMonitoredValue *QOpen62541Subscription::addValue(QOpcUaNode *node)
{
    if (!ensureNativeSubscription())
        return nullptr;

    QOpen62541Node *open62541node = static_cast<QOpen62541Node *>(node->d_func()->m_impl.data());
    UA_UInt32 monitoredItemId = 0;
    UA_StatusCode ret = UA_Client_Subscriptions_addMonitoredItem(m_backend->m_uaclient, m_subscriptionId,
                                                                 open62541node->nativeNodeId(), UA_ATTRIBUTEID_VALUE,
                                                                 monitoredValueHandler, this, &monitoredItemId);
    if (ret != UA_STATUSCODE_GOOD) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not add monitored item:" << ret;
        return nullptr;
    }

    QOpcUaMonitoredValue *monitoredValue = new QOpcUaMonitoredValue(node, m_qsubscription);
    if (m_dataChangeHandles.contains(monitoredItemId)) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "monitoredItemId already handled:" << monitoredItemId;
    } else {
        m_dataChangeHandles[monitoredItemId] = monitoredValue;
    }

    // Open62541 does not support automated subscriptions, hence we need to poll for the initial
    // value.
    UA_Client_Subscriptions_manuallySendPublishRequest(m_backend->m_uaclient);
    QMetaObject::invokeMethod(m_backend, "activateSubscriptionTimer", Qt::QueuedConnection, Q_ARG(int, m_interval));

    return monitoredValue;
}

void QOpen62541Subscription::removeValue(QOpcUaMonitoredValue *monitoredValue)
{
    auto it = m_dataChangeHandles.begin();
    while (it != m_dataChangeHandles.end()) {
        if (it.value() == monitoredValue) {
            UA_StatusCode ret = UA_Client_Subscriptions_removeMonitoredItem(m_backend->m_uaclient, m_subscriptionId, it.key());
            if (ret != UA_STATUSCODE_GOOD) {
                qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not remove monitored value from subscription:" << it.key();
            }
            m_dataChangeHandles.erase(it);
            QMetaObject::invokeMethod(m_backend, "removeSubscriptionTimer", Qt::QueuedConnection, Q_ARG(int, m_interval));
            break;
        }
        ++it;
    }
}

void QOpen62541Subscription::monitoredValueUpdated(UA_UInt32 monId, UA_DataValue *value)
{
    auto monitoredValue = m_dataChangeHandles.find(monId);
    if (monitoredValue == m_dataChangeHandles.end()) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not find object for monitoredItemId:" << monId;
        return;
    }

    if (!value || !value->hasValue)
        return;

    QVariant var = QOpen62541ValueConverter::toQVariant(value->value);
    if (var.isValid())
        (*monitoredValue)->d_func()->triggerValueChanged(var);
    else
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not convert value for node:" << (*monitoredValue)->node().displayName();
}

bool QOpen62541Subscription::ensureNativeSubscription()
{
    if (m_subscriptionId == 0) {
        QMetaObject::invokeMethod(m_backend, "createSubscription",
                                  Qt::BlockingQueuedConnection,
                                  Q_RETURN_ARG(UA_UInt32, m_subscriptionId),
                                  Q_ARG(int, m_interval));
    }
    return m_subscriptionId != 0;
}

void QOpen62541Subscription::removeNativeSubscription()
{
    if (m_subscriptionId != 0) {
        QMetaObject::invokeMethod(m_backend, "deleteSubscription",
                                  Qt::BlockingQueuedConnection,
                                  Q_ARG(UA_UInt32, m_subscriptionId));
        m_subscriptionId = 0;
    }
}

QT_END_NAMESPACE

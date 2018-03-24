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

#ifndef QOPEN62541SUBSCRIPTION_H
#define QOPEN62541SUBSCRIPTION_H

#include "qopen62541.h"
#include <QtOpcUa/qopcuanode.h>

QT_BEGIN_NAMESPACE

class Open62541AsyncBackend;

class QOpen62541Subscription : public QObject
{
    Q_OBJECT

public:
    QOpen62541Subscription(Open62541AsyncBackend *backend, const QOpcUaMonitoringParameters &settings);
    ~QOpen62541Subscription();

    UA_UInt32 createOnServer();
    bool removeOnServer();

    void modifyMonitoring(uintptr_t handle, QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, QVariant value);

    bool addAttributeMonitoredItem(uintptr_t handle, QOpcUa::NodeAttribute attr, const UA_NodeId &id, QOpcUaMonitoringParameters settings);
    bool removeAttributeMonitoredItem(uintptr_t handle, QOpcUa::NodeAttribute attr);

    void monitoredValueUpdated(UA_UInt32 monId, UA_DataValue *value);
    void sendTimeoutNotification();

    struct MonitoredItem {
        uintptr_t handle;
        QOpcUa::NodeAttribute attr;
        UA_UInt32 monitoredItemId;
        UA_UInt32 clientHandle;
        QOpcUaMonitoringParameters parameters;
        MonitoredItem(uintptr_t h, QOpcUa::NodeAttribute a, UA_UInt32 id)
            : handle(h)
            , attr(a)
            , monitoredItemId(id)
        {}
        MonitoredItem()
            : handle(0)
            , monitoredItemId(0)
        {}
    };

    double interval() const;
    UA_UInt32 subscriptionId() const;
    int monitoredItemsCount() const;

    QOpcUaMonitoringParameters::SubscriptionType shared() const;

signals:
    void timeout(QOpen62541Subscription *sub, QVector<QPair<uintptr_t, QOpcUa::NodeAttribute>> items);

private:
    MonitoredItem *getItemForAttribute(uintptr_t handle, QOpcUa::NodeAttribute attr);
    UA_ExtensionObject createFilter(const QVariant &filterData);

    bool modifySubscriptionParameters(uintptr_t handle, QOpcUa::NodeAttribute attr, const QOpcUaMonitoringParameters::Parameter &item, const QVariant &value);
    bool modifyMonitoredItemParameters(uintptr_t handle, QOpcUa::NodeAttribute attr, const QOpcUaMonitoringParameters::Parameter &item, const QVariant &value);

    Open62541AsyncBackend *m_backend;
    double m_interval;
    UA_UInt32 m_subscriptionId;
    UA_UInt32 m_lifetimeCount;
    UA_UInt32 m_maxKeepaliveCount;
    QOpcUaMonitoringParameters::SubscriptionType m_shared;
    quint8 m_priority;
    quint32 m_maxNotificationsPerPublish;

    QHash<uintptr_t, QHash<QOpcUa::NodeAttribute, MonitoredItem *>> m_handleToItemMapping; // Handle -> Attribute -> MonitoredItem
    QHash<UA_UInt32, MonitoredItem *> m_itemIdToItemMapping; // ItemId -> Item for fast lookup on data change

    quint32 m_clientHandle;
    bool m_timeout;
};

QT_END_NAMESPACE

#endif // QOPEN62541SUBSCRIPTION_H

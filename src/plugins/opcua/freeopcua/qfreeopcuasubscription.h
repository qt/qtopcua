/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
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

#ifndef QFREEOPCUASUBSCRIPTION_H
#define QFREEOPCUASUBSCRIPTION_H

#include "qfreeopcuanode.h"

#include <QtCore/qhash.h>

#include <opc/ua/subscription.h>

QT_BEGIN_NAMESPACE

class QFreeOpcUaWorker;

class QFreeOpcUaSubscription : public OpcUa::SubscriptionHandler
{
public:
    QFreeOpcUaSubscription(QFreeOpcUaWorker *backend, const QOpcUaMonitoringParameters &settings);
    ~QFreeOpcUaSubscription() override;

    // FreeOPC-UA callbacks
    void DataChange(uint32_t handle, const OpcUa::Node &node, const OpcUa::Variant &val,
                    OpcUa::AttributeId attr) override;

    quint32 createOnServer();
    bool removeOnServer();

    void modifyMonitoring(uintptr_t handle, QOpcUaNode::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, QVariant value);

    bool addAttributeMonitoredItem(uintptr_t handle, QOpcUaNode::NodeAttribute attr, const OpcUa::Node &node, QOpcUaMonitoringParameters settings);
    bool removeAttributeMonitoredItem(uintptr_t handle, QOpcUaNode::NodeAttribute attr);

    double interval() const;
    QOpcUaMonitoringParameters::SubscriptionType shared() const;
    quint32 subscriptionId() const;
    int monitoredItemsCount() const;

    struct MonitoredItem {
        uintptr_t handle;
        QOpcUaNode::NodeAttribute attr;
        quint32 monitoredItemId;
        MonitoredItem(uintptr_t h, QOpcUaNode::NodeAttribute a, quint32 id)
            : handle(h)
            , attr(a)
            , monitoredItemId(id)
        {}
        MonitoredItem()
            : handle(0)
            , monitoredItemId(0)
        {}
    };

private:
    MonitoredItem *getItemForAttribute(uintptr_t handle, QOpcUaNode::NodeAttribute attr);

    double m_interval;
    QOpcUaMonitoringParameters::SubscriptionType m_shared;

    OpcUa::Subscription::SharedPtr m_subscription;
    QFreeOpcUaWorker *m_backend;

    QHash<quint32, MonitoredItem *> m_itemIdToItemMapping;
    QHash<uintptr_t, QHash<QOpcUaNode::NodeAttribute, MonitoredItem *>> m_handleToItemMapping;
};

QT_END_NAMESPACE

#endif // QFREEOPCUASUBSCRIPTION_H

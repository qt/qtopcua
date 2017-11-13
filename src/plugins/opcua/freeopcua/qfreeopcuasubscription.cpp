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

#include "qfreeopcuasubscription.h"
#include "qfreeopcuanode.h"
#include "qfreeopcuaclient.h"
#include "qopcuamonitoredvalue.h"
#include "qfreeopcuavalueconverter.h"

#include <private/qopcuamonitoredevent_p.h>
#include <private/qopcuamonitoredvalue_p.h>
#include <private/qopcuanode_p.h>

#include <qopcuasubscription.h>

#include <QtCore/QDebug>
#include <QtCore/QLoggingCategory>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_FREEOPCUA)

QFreeOpcUaSubscription::QFreeOpcUaSubscription(OpcUa::UaClient *client, quint32 interval)
    : m_client(client)
{
    Q_ASSERT(m_client);

    try {
        m_subscription = m_client->CreateSubscription(interval, *this);
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Caught: %s", ex.what());
    }
}

QFreeOpcUaSubscription::~QFreeOpcUaSubscription()
{
    try {
        if (m_subscription) {
            for (int32_t handle : m_dataChangeHandles.keys())
                m_subscription->UnSubscribe(handle);

            for (int32_t handle : m_eventHandles.keys())
                m_subscription->UnSubscribe(handle);

            m_subscription->Delete();
        }
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Caught: %s", ex.what());
    }
}

void QFreeOpcUaSubscription::DataChange(quint32 handle, const OpcUa::Node &node,
                                        const OpcUa::Variant &val,
                                        OpcUa::AttributeId attr)
{
    OPCUA_UNUSED(node);
    OPCUA_UNUSED(attr);

    auto it = m_dataChangeHandles.find(handle);
    if (it == m_dataChangeHandles.end()) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Received event for unknown handle: %ul", handle);
        return;
    }

    try {
        (*it)->d_func()->triggerValueChanged(QFreeOpcUaValueConverter::toQVariant(val));
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Caught: %s", ex.what());
    }
}

QOpcUaMonitoredValue *QFreeOpcUaSubscription::addValue(QOpcUaNode *node)
{
    if (!m_subscription)
        return nullptr;

    try {
        // Only add a monitored item if the node has a value attribute
        QFreeOpcUaNode *nnode = static_cast<QFreeOpcUaNode *>(node->d_func()->m_impl.data());
        if (nnode->m_node.GetAttribute(OpcUa::AttributeId::Value).Status != OpcUa::StatusCode::Good)
            return nullptr;

        if (m_subscription) {
            uint32_t handle = m_subscription->SubscribeDataChange(m_client->GetNode(node->nodeId().toStdString()));
            QOpcUaMonitoredValue *monitoredValue = new QOpcUaMonitoredValue(node, m_qsubscription);
            m_dataChangeHandles[handle] = monitoredValue;
            return monitoredValue;
        }
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Caught: %s", ex.what());
    }
    return nullptr;
}

void QFreeOpcUaSubscription::Event(quint32 handle, const OpcUa::Event &event)
{
    auto it = m_eventHandles.find(handle);
    if (it == m_eventHandles.end()) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "Received event for unknown handle:" << handle;
        return;
    }

    try {
        QVector<QVariant> val;
        val.reserve(3);

        val.push_back(QVariant(QString::fromStdString(event.Message.Text)));
        val.push_back(QVariant(QString::fromStdString(event.SourceName)));
        val.push_back(QVariant(event.Severity));

        QOpcUaMonitoredEvent *me = *it;
        me->d_func()->triggerNewEvent(val);
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Caught: %s", ex.what());
    }
}

QOpcUaMonitoredEvent *QFreeOpcUaSubscription::addEvent(QOpcUaNode *node)
{
    // Note: Callback is not called due to some error in the event implementation in freeopcua
    if (!m_subscription)
        return nullptr;

    try {
        QFreeOpcUaNode *nnode = static_cast<QFreeOpcUaNode *>(node->d_func()->m_impl.data());
        if (nnode->m_node.GetAttribute(OpcUa::AttributeId::EventNotifier).Status != OpcUa::StatusCode::Good)
            return nullptr;

        OpcUa::Node serverNode = m_client->GetNode(OpcUa::ObjectId::Server);
        OpcUa::Node typeNode = m_client->GetNode(node->nodeId().toStdString());

        uint32_t handle = m_subscription->SubscribeEvents(serverNode, typeNode);
        QOpcUaMonitoredEvent *monitoredEvent = new QOpcUaMonitoredEvent(node, m_qsubscription);
        m_eventHandles[handle] = monitoredEvent;
        return monitoredEvent;
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Caught: %s", ex.what());
    }
    return nullptr;
}

void QFreeOpcUaSubscription::removeEvent(QOpcUaMonitoredEvent *event)
{
    try {
        auto it = m_eventHandles.begin();
        while (it != m_eventHandles.end()) {
            if (it.value() == event) {
                m_subscription->UnSubscribe(it.key());
                m_eventHandles.erase(it);
                break;
            }
            ++it;
        }
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Caught: %s", ex.what());
    }
}

void QFreeOpcUaSubscription::removeValue(QOpcUaMonitoredValue *value)
{
    try {
        auto it = m_dataChangeHandles.begin();
        while (it != m_dataChangeHandles.end()) {
            if (it.value() == value) {
                m_subscription->UnSubscribe(it.key());
                m_dataChangeHandles.erase(it);
                break;
            }
            ++it;
        }
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Caught: %s", ex.what());
    }
}

QT_END_NAMESPACE

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

#include "qfreeopcuaclient.h"
#include "qfreeopcuamonitoreditem.h"
#include "qfreeopcuavalueconverter.h"

#include <qopcuasubscription.h>

#include <QtCore/QDebug>
#include <QtCore/QDateTime>

QT_BEGIN_NAMESPACE

QFreeOpcUaSubscription::~QFreeOpcUaSubscription()
{
    unsubscribe();
}

QFreeOpcUaSubscription::QFreeOpcUaSubscription(QFreeOpcUaClient *myclient, int interval /* = 0 */)
    : QOpcUaSubscription(myclient)
    , m_pClient(myclient)
    , m_interval(interval)
    , m_eventHandle(0)
    , m_status(false)
{
    if (m_pClient) {
        try {
            m_dataChangeSubscription =  m_pClient->CreateSubscription(m_interval, *this);
            m_status = true;
        } catch (...) {
            m_status = false;
        }
    }
}

void QFreeOpcUaSubscription::DataChange(quint32 handle, const OpcUa::Node &node,
                                        const OpcUa::Variant &val,
                                        OpcUa::AttributeId attr)
{
    OPCUA_UNUSED(node);
    OPCUA_UNUSED(attr);

    if (m_dataChangeHandles.contains(handle)) {
        QOpcUaMonitoredItem* item = m_dataChangeHandles.value(handle);
        item->triggerValueChanged(QFreeOpcUaValueConverter::toQVariant(val));
    } else {
        qWarning() << "Received data change for unknown handle:" << handle;
    }
}

void QFreeOpcUaSubscription::Event(quint32 handle, const OpcUa::Event &event)
{
    if (m_dataChangeHandles.contains(handle)) {
        QList<QVariant> val;

        val.push_back(QVariant(QString::fromStdString(event.Message.Text)));
        val.push_back(QVariant(QString::fromStdString(event.SourceName)));
        val.push_back(QVariant((double) event.Severity));

        QOpcUaMonitoredItem* item = m_dataChangeHandles.value(handle);
        item->triggerNewEvent(val);
    } else {
        qWarning() << "Received event for unknown handle:" << handle;
    }
}

QOpcUaMonitoredItem *QFreeOpcUaSubscription::addItem(const QString &xmlNodeId)
{
    QFreeOpcUaMonitoredItem* item = 0;

    try {
        if (m_dataChangeSubscription) {
            OpcUa::Node mynode = m_pClient->GetNode(xmlNodeId.toStdString());
            uint32_t handle = m_dataChangeSubscription->SubscribeDataChange(mynode);

            item = new QFreeOpcUaMonitoredItem(this);
            m_dataChangeHandles[handle] = item;
        }
    } catch (...) {
        m_status = false;
    }

    return item;
}

QOpcUaMonitoredItem *QFreeOpcUaSubscription::addEventItem(const QString &eventId)
{
    QFreeOpcUaMonitoredItem* item = 0;

    try {
        if (m_dataChangeSubscription) {
            OpcUa::Node serverNode = m_pClient->GetNode(OpcUa::ObjectId::Server);
            OpcUa::Node typeNode = m_pClient->GetNode(eventId.toStdString());

            uint32_t handle = m_dataChangeSubscription->SubscribeEvents(serverNode, typeNode);

            item = new QFreeOpcUaMonitoredItem(this);
            m_dataChangeHandles[handle] = item;
        }
    } catch (...) {
        m_status = false;
    }

    return item;
}

bool QFreeOpcUaSubscription::unsubscribe(QOpcUaMonitoredItem *item)
{
    if (m_dataChangeSubscription && m_status) {

        for (int32_t handle : m_dataChangeHandles.keys()) {
            if (m_dataChangeHandles.value(handle) == item) {
                m_dataChangeSubscription->UnSubscribe(handle);
                m_dataChangeHandles.remove(handle);

                return true;
            }
        }
    }

    return false;
}

bool QFreeOpcUaSubscription::success() const
{
    return m_status;
}

void QFreeOpcUaSubscription::unsubscribe()
{
    try {
        if (m_dataChangeSubscription && m_status) {
            for (int32_t handle : m_dataChangeHandles.keys()) {
                m_dataChangeSubscription->UnSubscribe(handle);
                m_dataChangeHandles.remove(handle);
            }
        }
    } catch (...) {
        m_status = false;
    }
}

QT_END_NAMESPACE

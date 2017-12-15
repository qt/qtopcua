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

#include "qfreeopcuaclient.h"
#include "qfreeopcuasubscription.h"
#include "qfreeopcuavalueconverter.h"
#include "qfreeopcuaworker.h"
#include <private/qopcuanode_p.h>

#include <QtCore/qloggingcategory.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_FREEOPCUA)

QFreeOpcUaSubscription::QFreeOpcUaSubscription(QFreeOpcUaWorker *backend, const QOpcUaMonitoringParameters &settings)
    : m_interval(settings.publishingInterval())
    , m_shared(settings.shared())
    , m_backend(backend)
{
    Q_ASSERT(m_backend);
}

QFreeOpcUaSubscription::~QFreeOpcUaSubscription()
{
    removeOnServer();
}

quint32 QFreeOpcUaSubscription::createOnServer()
{
    if (m_subscription)
        return 0;

    try {
        m_subscription = m_backend->CreateSubscription(m_interval, *this);
        m_interval = m_subscription->GetData().RevisedPublishingInterval;
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "CreateOnServer caught:" << ex.what();
        return 0;
    }
    return m_subscription->GetId();
}

bool QFreeOpcUaSubscription::removeOnServer()
{
    bool success = false;
    try {
        if (m_subscription) {
            m_subscription->Delete();
            success = true;
        }
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "RemoveOnServer caught:" << ex.what();
    }

    m_subscription.reset();

    for (auto it : qAsConst(m_itemIdToItemMapping)) {
        QOpcUaMonitoringParameters s;
        s.setStatusCode(QOpcUa::UaStatusCode::BadDisconnect);
        emit m_backend->monitoringEnableDisable(it->handle, it->attr, false, s);
    }

    qDeleteAll(m_itemIdToItemMapping);

    m_itemIdToItemMapping.clear();
    m_handleToItemMapping.clear();

    return success;
}

void QFreeOpcUaSubscription::StatusChange(OpcUa::StatusCode status)
{
    if (status == OpcUa::StatusCode::BadDisconnect)
        removeOnServer();
}

void QFreeOpcUaSubscription::modifyMonitoring(uintptr_t handle, QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, QVariant value)
{
    Q_UNUSED(item);
    Q_UNUSED(value);

    if (!getItemForAttribute(handle, attr)) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "Could not modify parameter for" << handle << ", there are no monitored items";
        QOpcUaMonitoringParameters p;
        p.setStatusCode(QOpcUa::UaStatusCode::BadAttributeIdInvalid);
        emit m_backend->monitoringStatusChanged(handle, attr, item, p);
        return;
    }

    QOpcUaMonitoringParameters s;
    s.setStatusCode(QOpcUa::UaStatusCode::BadNotImplemented);
    emit m_backend->monitoringEnableDisable(handle, attr, true, s);
}

bool QFreeOpcUaSubscription::addAttributeMonitoredItem(uintptr_t handle, QOpcUa::NodeAttribute attr, const OpcUa::Node &node, QOpcUaMonitoringParameters settings)
{
    Q_UNUSED(settings); // Setting these options is not yet supported

    quint32 monitoredItemId;

    try {
        if (m_subscription) {
            monitoredItemId = m_subscription->SubscribeDataChange(node, QFreeOpcUaValueConverter::toUaAttributeId(attr));
        }
        else {
            QOpcUaMonitoringParameters s;
            s.setStatusCode(QOpcUa::UaStatusCode::BadSubscriptionIdInvalid);
            emit m_backend->monitoringEnableDisable(handle, attr, true, s);
            return false;
        }
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "addAttributeMonitoredItem caught:" << ex.what();
        QOpcUaMonitoringParameters s;
        s.setStatusCode(QFreeOpcUaValueConverter::exceptionToStatusCode(ex));
        emit m_backend->monitoringEnableDisable(handle, attr, true, s);
        return false;
    }

    MonitoredItem *temp = new MonitoredItem(handle, attr, monitoredItemId);
    m_handleToItemMapping[handle][attr] = temp;
    m_itemIdToItemMapping[monitoredItemId] = temp;

    QOpcUaMonitoringParameters s;
    s.setSubscriptionId(m_subscription->GetId());
    s.setPublishingInterval(m_interval);
    s.setMaxKeepAliveCount(m_subscription->GetData().RevisedMaxKeepAliveCount);
    s.setLifetimeCount(m_subscription->GetData().RevisedLifetimeCount);
    s.setStatusCode(QOpcUa::UaStatusCode::Good);
    s.setSamplingInterval(m_interval);
    emit m_backend->monitoringEnableDisable(handle, attr, true, s);

    return true;
}

bool QFreeOpcUaSubscription::removeAttributeMonitoredItem(uintptr_t handle, QOpcUa::NodeAttribute attr)
{
    QScopedPointer<MonitoredItem> item(getItemForAttribute(handle, attr));

    if (!item) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "There is no monitored item for this attribute";
        QOpcUaMonitoringParameters s;
        s.setStatusCode(QOpcUa::UaStatusCode::BadMonitoredItemIdInvalid);
        emit m_backend->monitoringEnableDisable(handle, attr, false, s);
        return false;
    }

    QOpcUaMonitoringParameters s;

    try {
        m_subscription->UnSubscribe(item->monitoredItemId);
        s.setStatusCode(QOpcUa::UaStatusCode::Good);
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "removeAttributeMonitoredItem caught:" << ex.what();
        s.setStatusCode(QFreeOpcUaValueConverter::exceptionToStatusCode(ex));
    }

    m_itemIdToItemMapping.remove(item->monitoredItemId);
    auto it = m_handleToItemMapping.find(handle);
    it->remove(attr);
    if (it->empty())
        m_handleToItemMapping.erase(it);

    emit m_backend->monitoringEnableDisable(handle, attr, false, s);

    return true;
}

double QFreeOpcUaSubscription::interval() const
{
    return m_interval;
}

QOpcUaMonitoringParameters::SubscriptionType QFreeOpcUaSubscription::shared() const
{
    return m_shared;
}

quint32 QFreeOpcUaSubscription::subscriptionId() const
{
    if (m_subscription)
        return m_subscription->GetId();
    else
        return 0;
}

int QFreeOpcUaSubscription::monitoredItemsCount() const
{
    return m_itemIdToItemMapping.size();
}

QFreeOpcUaSubscription::MonitoredItem *QFreeOpcUaSubscription::getItemForAttribute(uintptr_t handle, QOpcUa::NodeAttribute attr)
{
    auto nodeEntry = m_handleToItemMapping.find(handle);

    if (nodeEntry == m_handleToItemMapping.end())
        return nullptr;

    auto item = nodeEntry->find(attr);
    if (item == nodeEntry->end())
        return nullptr;

    return item.value();
}

void QFreeOpcUaSubscription::DataChange(quint32 handle, const OpcUa::Node &node,
                                        const OpcUa::Variant &val,
                                        OpcUa::AttributeId attr)
{
    OPCUA_UNUSED(node);
    OPCUA_UNUSED(attr);

    auto item = m_itemIdToItemMapping.find(handle);
    if (item == m_itemIdToItemMapping.end())
        return;
    emit m_backend->attributeUpdated(item.value()->handle, item.value()->attr, QFreeOpcUaValueConverter::toQVariant(val));
}

QT_END_NAMESPACE

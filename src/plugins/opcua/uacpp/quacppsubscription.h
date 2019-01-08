/******************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#ifndef QUACPPSUBSCRIPTION_H
#define QUACPPSUBSCRIPTION_H

#include <quacppbackend.h>

#include <uanodeid.h>
#include <uasubscription.h>

QT_BEGIN_NAMESPACE

class QUACppSubscription : public UaClientSdk::UaSubscriptionCallback
{
public:
    QUACppSubscription(UACppAsyncBackend *backend, const QOpcUaMonitoringParameters &settings);
    ~QUACppSubscription();

    quint32 createOnServer();
    bool removeOnServer();


    bool addAttributeMonitoredItem(quint64 handle, QOpcUa::NodeAttribute attr, const UaNodeId &id, QOpcUaMonitoringParameters parameters);
    void modifyMonitoring(quint64 handle, QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, QVariant value);
    bool removeAttributeMonitoredItem(quint64 handle, QOpcUa::NodeAttribute attr);

    double interval() const;
    quint32 subscriptionId() const;
    int monitoredItemsCount() const;

    QOpcUaMonitoringParameters::SubscriptionType shared() const;

    // UaSubscription overrides
    void subscriptionStatusChanged(OpcUa_UInt32 clientSubscriptionHandle, const UaStatus &status) override;
    void dataChange(OpcUa_UInt32 clientSubscriptionHandle, const UaMonitoredItemNotifications &dataNotifications, const UaDiagnosticInfos &diagnosticInfos) override;
    void newEvents(OpcUa_UInt32 clientSubscriptionHandle, UaEventFieldLists &eventFieldList) override;
private:
    OpcUa_ExtensionObject createFilter(const QVariant &filterData);
    void createDataChangeFilter(const QOpcUaMonitoringParameters::DataChangeFilter &filter, OpcUa_ExtensionObject *out);
    void createEventFilter(const QOpcUaMonitoringParameters::EventFilter &filter, OpcUa_ExtensionObject *out);
    QOpcUaEventFilterResult convertEventFilterResult(const OpcUa_ExtensionObject &obj);

    bool modifySubscriptionParameters(quint64 handle, QOpcUa::NodeAttribute attr, const QOpcUaMonitoringParameters::Parameter &item, const QVariant &value);
    bool modifyMonitoredItemParameters(quint64 handle, QOpcUa::NodeAttribute attr, const QOpcUaMonitoringParameters::Parameter &item, const QVariant &value);

    UACppAsyncBackend *m_backend;
    QOpcUaMonitoringParameters m_subscriptionParameters;
    UaClientSdk::UaSubscription *m_nativeSubscription;
    QHash<QPair<quint64, QOpcUa::NodeAttribute>,
        QPair<OpcUa_MonitoredItemCreateResult, QOpcUaMonitoringParameters>> m_monitoredItems;
    QHash<quint32, QPair<quint64, QOpcUa::NodeAttribute>> m_monitoredIds;
};

QT_END_NAMESPACE

#endif // QUACPPSUBSCRIPTION_H

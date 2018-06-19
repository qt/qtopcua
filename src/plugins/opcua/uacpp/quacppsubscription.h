/******************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#ifndef QUACPPSUBSCRIPTION_H
#define QUACPPSUBSCRIPTION_H

#include <quacppbackend.h>

#include <uabase/uanodeid.h>
#include <uaclient/uasubscription.h>

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

// Copyright (C) 2017 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAMONITORINGPARAMETERS_P_H
#define QOPCUAMONITORINGPARAMETERS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtOpcUa/qopcuamonitoringparameters.h>

#include <QtCore/qshareddata.h>
#include <QtCore/private/qglobal_p.h>

QT_BEGIN_NAMESPACE

class Q_OPCUA_EXPORT QOpcUaMonitoringParametersPrivate : public QSharedData
{
public:

    QOpcUaMonitoringParametersPrivate()
        : samplingInterval(0)
        , queueSize(0)
        , discardOldest(true)
        , monitoringMode(QOpcUaMonitoringParameters::MonitoringMode::Reporting)
        , subscriptionId(0)
        , monitoredItemId(0)
        , publishingInterval(0)
        , lifetimeCount(150) // Must be at least three times the maxKeepAliveCount (OPC UA 1.05 part 4, page 5.13.2.2).
        , maxKeepAliveCount(50) // Allow 50 publishing intervals before a keepalive is expected to reduce the network traffic.
        , maxNotificationsPerPublish(0)
        , priority(0)
        , publishingEnabled(true)
        , statusCode(QOpcUa::UaStatusCode::BadNoEntryExists)
        , shared(QOpcUaMonitoringParameters::SubscriptionType::Shared)
    {}

    // MonitoredItem
    double samplingInterval;
    QVariant filter;
    QVariant filterResult;
    quint32 queueSize;
    bool discardOldest;
    QOpcUaMonitoringParameters::MonitoringMode monitoringMode;
    QString indexRange;
    QSet<quint32> triggeredItemIds;
    QHash<quint32, QOpcUa::UaStatusCode> addTriggeredItemStatus;

    // Subscription
    quint32 subscriptionId;
    quint32 monitoredItemId;
    double publishingInterval;
    quint32 lifetimeCount;
    quint32 maxKeepAliveCount;
    quint32 maxNotificationsPerPublish;
    quint8 priority;
    bool publishingEnabled;

    // Qt OPC UA specific
    QOpcUa::UaStatusCode statusCode;
    QOpcUaMonitoringParameters::SubscriptionType shared;
};

QT_END_NAMESPACE

#endif // QOPCUAMONITORINGPARAMETERS_P_H

/****************************************************************************
**
** Copyright (C) 2017 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
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
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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
        , lifetimeCount(150) // Must be at least three times the maxKeepAliveCount (OPC-UA part 4, page 76).
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

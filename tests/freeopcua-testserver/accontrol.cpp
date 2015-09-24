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

#include "accontrol.h"

#include <opc/ua/server/server.h>


ACControl::ACControl(QObject *parent)
    : QObject(parent)
    , m_currentTemp(16.0)
    , m_temperatureSetpoint(22.0)
{
    m_timer.setInterval(200);
    m_timer.setSingleShot(false);

    connect(&m_timer, &QTimer::timeout, this, &ACControl::adjustTemperature);

    m_timer.start();
}

void ACControl::initNodes(OpcUa::UaServer &server)
{
    OpcUa::Node root = server.GetObjectsNode();

    OpcUa::Node acControl = root.AddFolder("ns=3;s=ACControl", "ACControl");

    m_setPointNode = acControl.AddVariable("ns=3;s=ACControl.SetPoint", "ACSetpoint", OpcUa::Variant(22.0));
    m_currentTempNode = acControl.AddVariable("ns=3;s=ACControl.CurrentTemp", "ACCurrentTemp", OpcUa::Variant(16.0));

    m_startNode = acControl.AddVariable("ns=3;s=ACControl.Start", "ACStart", OpcUa::Variant(true));
    m_stopNode = acControl.AddVariable("ns=3;s=ACControl.Stop", "ACStop", OpcUa::Variant(false));
    m_stateNode = acControl.AddVariable("ns=3;s=ACControl.IsRunning", "ACIsRunning", OpcUa::Variant(true));

    // get set point updates
    m_subscription = server.CreateSubscription(100, *this);
    m_subscription->SubscribeDataChange(m_setPointNode);

    // this is a workaround for method calls (not yet implemented in FreeOpcUa)
    m_subscription->SubscribeDataChange(m_startNode);
    m_subscription->SubscribeDataChange(m_stopNode);
}

void ACControl::DataChange(uint32_t handle, const OpcUa::Node &node, const OpcUa::Variant &val, OpcUa::AttributeId attr)
{
    Q_UNUSED(handle);
    Q_UNUSED(attr);

    if (node == m_setPointNode)
        m_temperatureSetpoint = val.As<double>();

    if (node == m_startNode && val.As<bool>()) {
        m_timer.metaObject()->invokeMethod(&m_timer, "start");
        m_stateNode.SetValue(OpcUa::Variant(true));
    }
    if (node == m_stopNode && val.As<bool>()) {
        m_timer.metaObject()->invokeMethod(&m_timer, "stop");
        m_stateNode.SetValue(OpcUa::Variant(false));
    }
}

void ACControl::adjustTemperature()
{
    const double diff = m_temperatureSetpoint - m_currentTemp;

    if (diff > 0.1) {
        m_currentTemp += 0.1;
        m_currentTempNode.SetValue(OpcUa::Variant(m_currentTemp));
    } else if (diff < 0.1) {
        m_currentTemp -= 0.1;
        m_currentTempNode.SetValue(OpcUa::Variant(m_currentTemp));
    }
}


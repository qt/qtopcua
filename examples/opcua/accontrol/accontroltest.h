/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the QtOpcUa module.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef ACCONTROLTEST_H
#define ACCONTROLTEST_H

#include <QtWidgets/qmainwindow.h>
#include <QtOpcUa/qopcuaclient.h>

#include "ui_accontroltest.h"

QT_BEGIN_NAMESPACE
class QOpcUaClient;
class QOpcUaNode;
class QOpcUaProvider;
class QOpcUaMonitoredValue;
class QOpcUaSubscription;
QT_END_NAMESPACE

class QOpcUaACControlTest : public QMainWindow
{
    Q_OBJECT

public:
    explicit QOpcUaACControlTest(QWidget *parent = 0);
    virtual ~QOpcUaACControlTest();

public slots:
    void writeValue(int val);
    void updateSetpoint(QVariant val);
    void updateTemperature(QVariant val);
    void updateText(const QVariant &txt);
    void stop(void);
    void start(void);
    void stateChange(QVariant state);

    void clientConnected();
    void clientDisconnected();
    void statusChanged(QOpcUaClient::ClientState state);

private:
    Ui::QOpcUaACControlTest ui;

    QOpcUaNode *m_pSetPointNode;

    QOpcUaSubscription   *m_oneSecondSubscription;
    QOpcUaNode           *m_timeNode;
    QOpcUaMonitoredValue *m_pTimeMonitor;

    QOpcUaSubscription   *m_hundredMsSubscription;
    QOpcUaMonitoredValue *m_pSetPointMonitor;
    QOpcUaMonitoredValue *m_pTemperatureMonitor;
    QOpcUaNode           *m_temperatureNode;
    QOpcUaMonitoredValue *m_pStateMonitor;
    QOpcUaNode           *m_stateNode;

    QOpcUaClient        *m_pClient;
    QOpcUaProvider      *m_pProvider;
};

#endif // ACCONTROLTEST_H

/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the QtOpcUa module.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
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

#include "accontroltest.h"
#include "ui_accontroltest.h"

#include <QtOpcUa/qopcuaprovider.h>
#include <QtOpcUa/qopcuaclient.h>

#include <QtCore/qcommandlineparser.h>
#include <QtCore/qdebug.h>

const QString SETPOINT_NODE = QStringLiteral("ns=3;s=ACControl.SetPoint");

QOpcUaACControlTest::QOpcUaACControlTest(QWidget *parent)
    : QMainWindow(parent)
    , m_pTimeSubscription(0)
    , m_pSetPointSubscription(0)
    , m_pTemperatureSubscription(0)
    , m_pStateSubscription(0)
{
    // parse command line here
    QCommandLineParser parser;
    parser.setApplicationDescription("A graphical OPC UA client for a simulated air conditioner using QtOpcUa");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption endpointOption(QStringList() << "e" << "endpoint",
                                      "OPC UA endpoint to connect to.",
                                      "endpoint", "opc.tcp://localhost:43344");
    parser.addOption(endpointOption);

    QCommandLineOption pluginOption(QStringList() << "p" << "plugin",
                                      "override the OPC UA plugin used",
                                      "plugin", QString());
    parser.addOption(pluginOption);

    parser.process(qApp->arguments());

    // setup UI, the OPC UA connection and some subscriptions
    ui.setupUi(this);

    m_pProvider = new QOpcUaProvider(parser.value(pluginOption), this);
    m_pClient = m_pProvider->createClient();

    if (!m_pClient) {
        qFatal("Could not initialize QtOpcUa plugin: %s\n",
               qPrintable(parser.value(pluginOption)));
    }

    // show currently selected plugin in the title bar
    setWindowTitle(windowTitle() + " (backend: " + m_pProvider->backend() + ")");

    bool res = m_pClient->connectToEndpoint(parser.value(endpointOption));
    if (!res)
        qFatal("Failed to connect to endpoint: %s\n", qPrintable(parser.value(endpointOption)));

    // get current time from server every 1 second and display it
    m_pTimeSubscription = m_pClient->dataMonitor(1000, QStringLiteral("ns=0;i=2258"));
    if (m_pTimeSubscription) {
        connect(m_pTimeSubscription, &QOpcUaMonitoredItem::valueChanged, this,
                &QOpcUaACControlTest::updateText);
    }

    // subscribe to current set point and temperature
    m_pSetPointSubscription = m_pClient->dataMonitor(100, SETPOINT_NODE);
    if (m_pSetPointSubscription) {
        connect(m_pSetPointSubscription, &QOpcUaMonitoredItem::valueChanged, this,
                &QOpcUaACControlTest::updateSetpoint);
    }
    ui.setpointBar->setFormat("%p °C");

    m_pTemperatureSubscription = m_pClient->dataMonitor(100, QStringLiteral("ns=3;s=ACControl.CurrentTemp"));
    if (m_pTemperatureSubscription) {
        connect(m_pTemperatureSubscription, &QOpcUaMonitoredItem::valueChanged,
                this, &QOpcUaACControlTest::updateTemperature);
    }
    ui.temperatureBar->setFormat("%p °C");

    ui.horizontalSlider->setEnabled(false); // wait for real value from server
    connect(ui.horizontalSlider, &QAbstractSlider::valueChanged, this, &QOpcUaACControlTest::writeValue);

    // connect start/stop logic
    connect(ui.stopButton, &QAbstractButton::clicked, this, &QOpcUaACControlTest::stop);
    connect(ui.startButton, &QAbstractButton::clicked, this, &QOpcUaACControlTest::start);

    m_pStateSubscription = m_pClient->dataMonitor(100, QStringLiteral("ns=3;s=ACControl.IsRunning"));
    if (m_pStateSubscription) {
        connect(m_pStateSubscription, &QOpcUaMonitoredItem::valueChanged, this,
                &QOpcUaACControlTest::stateChange);
    }
}

void QOpcUaACControlTest::writeValue(int val)
{
    if (!m_pClient->write(SETPOINT_NODE, QVariant(static_cast<double>(val))))
        qWarning() << "Failed to write new set point value!";
}

void QOpcUaACControlTest::updateText(const QVariant &txt)
{
    ui.myLabel->setText(txt.toString());
}

void QOpcUaACControlTest::updateSetpoint(QVariant val)
{
    ui.setpointBar->setValue(val.toInt());

    if (!ui.horizontalSlider->isEnabled()) {
        ui.horizontalSlider->setEnabled(true);
        ui.horizontalSlider->setValue(val.toInt());
    }
}

void QOpcUaACControlTest::updateTemperature(QVariant val)
{
    ui.temperatureBar->setValue(val.toInt());
}

void QOpcUaACControlTest::start(void)
{
    bool res = m_pClient->write(QStringLiteral("ns=3;s=ACControl.Start"), true);
    if (!res)
        qDebug("Could not call start method");
}

void QOpcUaACControlTest::stop(void)
{
    bool res = m_pClient->write(QStringLiteral("ns=3;s=ACControl.Stop"), true);
    if (!res)
        qDebug("Could not call stop method");
}

void QOpcUaACControlTest::stateChange(QVariant state)
{
    if (state.toInt() == 0) {
        ui.stopButton->setDisabled(true);
        ui.startButton->setEnabled(true);
        ui.startButton->setFocus();
    } else {
        ui.stopButton->setEnabled(true);
        ui.startButton->setDisabled(true);
        ui.stopButton->setFocus();
    }
}


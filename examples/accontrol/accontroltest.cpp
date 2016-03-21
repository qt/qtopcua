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

#include <QtOpcUa/QOpcUaClient>
#include <QtOpcUa/QOpcUaMonitoredValue>
#include <QtOpcUa/QOpcUaSubscription>
#include <QtOpcUa/QOpcUaNode>
#include <QtOpcUa/QOpcUaProvider>

#include <QtCore/qcommandlineparser.h>
#include <QtCore/qdebug.h>

const QString SETPOINT_NODE = QStringLiteral("ns=3;s=ACControl.SetPoint");

QOpcUaACControlTest::QOpcUaACControlTest(QWidget *parent)
    : QMainWindow(parent)
    , m_oneSecondSubscription(0)
    , m_pTimeMonitor(0)
    , m_hundredMsSubscription(0)
    , m_pSetPointMonitor(0)
    , m_pTemperatureMonitor(0)
    , m_pStateMonitor(0)
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

    if (!parser.isSet(pluginOption)) {
        qWarning() << "You must select a plugin via -p or --plugin, e.g. freeopcua";
        exit(EXIT_FAILURE);
    }

    m_pProvider = new QOpcUaProvider(this);
    m_pClient = m_pProvider->createClient(parser.value(pluginOption));

    if (!m_pClient) {
        qWarning() << "Could not initialize QtOpcUa plugin: " <<
               qPrintable(parser.value(pluginOption));
        exit(EXIT_FAILURE);
    }

    // show currently selected plugin in the title bar
    setWindowTitle(windowTitle() + " (backend: " + m_pClient->backend() + ")");

    bool res = m_pClient->connectToEndpoint(parser.value(endpointOption));
    if (!res) {
        qWarning() << "Failed to connect to endpoint: " << qPrintable(parser.value(endpointOption));
        exit(EXIT_FAILURE);
    }

    m_oneSecondSubscription = m_pClient->createSubscription(1000);
    // get current time from server every 1 second and display it

    m_timeNode = m_pClient->node(QStringLiteral("ns=0;i=2258"));
    m_pTimeMonitor = m_oneSecondSubscription->addValue(m_timeNode);
    if (m_pTimeMonitor) {
        connect(m_pTimeMonitor, &QOpcUaMonitoredValue::valueChanged, this,
                &QOpcUaACControlTest::updateText);
    }

    m_hundredMsSubscription = m_pClient->createSubscription(100);

    // subscribe to current set point and temperature
    m_pSetPointNode = m_pClient->node(SETPOINT_NODE);
    m_pSetPointMonitor = m_hundredMsSubscription->addValue(m_pSetPointNode);
    if (m_pSetPointMonitor) {
        connect(m_pSetPointMonitor, &QOpcUaMonitoredValue::valueChanged, this,
                &QOpcUaACControlTest::updateSetpoint);
    }

    m_temperatureNode = m_pClient->node(QStringLiteral("ns=3;s=ACControl.CurrentTemp"));
    m_pTemperatureMonitor = m_hundredMsSubscription->addValue(m_temperatureNode);
    if (m_pTemperatureMonitor) {
        connect(m_pTemperatureMonitor, &QOpcUaMonitoredValue::valueChanged,
                this, &QOpcUaACControlTest::updateTemperature);
    }

    ui.horizontalSlider->setEnabled(false); // wait for real value from server
    connect(ui.horizontalSlider, &QAbstractSlider::valueChanged, this, &QOpcUaACControlTest::writeValue);

    // connect start/stop logic
    connect(ui.stopButton, &QAbstractButton::clicked, this, &QOpcUaACControlTest::stop);
    connect(ui.startButton, &QAbstractButton::clicked, this, &QOpcUaACControlTest::start);

    m_stateNode = m_pClient->node(QStringLiteral("ns=3;s=ACControl.IsRunning"));
    m_pStateMonitor = m_oneSecondSubscription->addValue(m_stateNode);
    if (m_pStateMonitor) {
        connect(m_pStateMonitor, &QOpcUaMonitoredValue::valueChanged, this,
                &QOpcUaACControlTest::stateChange);
    }
}

QOpcUaACControlTest::~QOpcUaACControlTest()
{
    delete m_pClient;
    delete m_pProvider;
    delete m_pSetPointMonitor;
    delete m_pTemperatureMonitor;
    delete m_pTimeMonitor;
    delete m_oneSecondSubscription;
    delete m_hundredMsSubscription;
}

void QOpcUaACControlTest::writeValue(int val)
{
    if (!m_pSetPointNode->setValue(QVariant(static_cast<double>(val))))
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
    ui.temperatureBar->setValue(val.toDouble()*10);
}

void QOpcUaACControlTest::start(void)
{
    QScopedPointer<QOpcUaNode> node(m_pClient->node(QStringLiteral("ns=3;s=ACControl.Start")));
    bool res = node->setValue(true);
    if (!res)
        qDebug("Could not call start method");
}

void QOpcUaACControlTest::stop(void)
{
    QScopedPointer<QOpcUaNode> node(m_pClient->node(QStringLiteral("ns=3;s=ACControl.Stop")));
    bool res = node->setValue(true);
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


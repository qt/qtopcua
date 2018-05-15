/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt OPC UA module.
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

#include "mainwindow.h"
#include "opcuamodel.h"

#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextCharFormat>
#include <QPlainTextEdit>
#include <QTextBlock>
#include <QTreeView>
#include <QHeaderView>
#include <QOpcUaProvider>

QT_BEGIN_NAMESPACE

static MainWindow *mainWindowGlobal = nullptr;
static QtMessageHandler oldMessageHandler = nullptr;

static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (!mainWindowGlobal)
        return;

   QString message = "%1: %2";
   QString contextStr = " (%1:%2, %3)";
   QString typeString;

   if (type == QtDebugMsg)
       typeString = QObject::tr("Debug");
   else if (type == QtInfoMsg)
        typeString = QObject::tr("Info");
   else if (type == QtWarningMsg)
        typeString = QObject::tr("Warning");
   else if (type == QtCriticalMsg)
        typeString = QObject::tr("Critical");
   else if (type == QtFatalMsg)
        typeString = QObject::tr("Fatal");

   message = message.arg(typeString).arg(msg);
   contextStr = contextStr.arg(context.file).arg(context.line).arg(context.function);

   QColor color = Qt::black;
   if (type == QtFatalMsg || type == QtCriticalMsg)
       color = Qt::darkRed;
   else if (type == QtWarningMsg)
       color = Qt::darkYellow;

   // Logging messages from backends are sent from different threads and need to be
   // synchronized with the GUI thread.
   QMetaObject::invokeMethod(mainWindowGlobal, "log", Qt::QueuedConnection,
                             Q_ARG(QString, message),
                             Q_ARG(QString, contextStr),
                             Q_ARG(QColor, color));

   if (oldMessageHandler)
       oldMessageHandler(type, context, msg);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
  , mServerUrl(new QLineEdit(this))
  , mOpcUaPlugin(new QComboBox(this))
  , mConnectButton(new QPushButton(tr("Connect"), this))
  , mLog(new QPlainTextEdit(this))
  , mTreeView(new QTreeView(this))
  , mOpcUaModel(new OpcUaModel(this))
  , mOpcUaProvider(new QOpcUaProvider(this))
  , mClientConnected(false)
{
    mainWindowGlobal = this;

    auto hbox = new QHBoxLayout;
    hbox->addWidget(mOpcUaPlugin);
    hbox->addWidget(mServerUrl);
    hbox->addWidget(mConnectButton);

    auto vbox = new QVBoxLayout;
    vbox->addLayout(hbox);
    vbox->addWidget(mTreeView);
    vbox->addWidget(new QLabel(tr("Log:")));
    vbox->addWidget(mLog);

    auto widget = new QWidget;
    widget->setLayout(vbox);
    setCentralWidget(widget);

    updateUiState();

    mServerUrl->setText("opc.tcp://127.0.0.1:43344");
    mOpcUaPlugin->addItems(mOpcUaProvider->availableBackends());
    mLog->setReadOnly(true);
    mLog->setLineWrapMode(QPlainTextEdit::NoWrap);
    setMinimumWidth(500);
    mTreeView->setModel(mOpcUaModel);
    mTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    mTreeView->setTextElideMode(Qt::ElideRight);
    mTreeView->setAlternatingRowColors(true);

    if (mOpcUaPlugin->count() == 0) {
        mOpcUaPlugin->setDisabled(true);
        mConnectButton->setDisabled(true);
        QMessageBox::critical(this, tr("No OPCUA plugins available"), tr("The list of available OPCUA plugins is empty. No connection possible."));
    }

    connect(mConnectButton, &QPushButton::clicked, this, &MainWindow::connectToServer);
    oldMessageHandler = qInstallMessageHandler(&messageHandler);
}

void MainWindow::connectToServer()
{
    if (mClientConnected) {
        mOpcUaClient->disconnectFromEndpoint();
        return;
    }

    mOpcUaClient = mOpcUaProvider->createClient(mOpcUaPlugin->currentText());
    if (!mOpcUaClient) {
        const QString message(tr("Connecting to the given sever failed. See the log for details."));
        log(message, QString(), Qt::red);
        QMessageBox::critical(this, tr("Failed to connect to server"), message);
        return;
    }

    connect(mOpcUaClient, &QOpcUaClient::connected, this, &MainWindow::clientConnected);
    connect(mOpcUaClient, &QOpcUaClient::disconnected, this, &MainWindow::clientDisconnected);
    connect(mOpcUaClient, &QOpcUaClient::errorChanged, this, &MainWindow::clientError);
    connect(mOpcUaClient, &QOpcUaClient::stateChanged, this, &MainWindow::clientState);

    mOpcUaClient->connectToEndpoint(mServerUrl->text());
}

void MainWindow::clientConnected()
{
    mClientConnected = true;
    updateUiState();
    mOpcUaModel->setOpcUaClient(mOpcUaClient);
    mTreeView->header()->setSectionResizeMode(1 /* Value column*/, QHeaderView::Interactive);
}

void MainWindow::clientDisconnected()
{
    mClientConnected = false;
    mOpcUaClient->deleteLater();
    mOpcUaModel->setOpcUaClient(nullptr);
    updateUiState();
}

void MainWindow::clientError(QOpcUaClient::ClientError error)
{
    qDebug() << "Client error changed" << error;
}

void MainWindow::clientState(QOpcUaClient::ClientState state)
{
    qDebug() << "Client state changed" << state;
}

void MainWindow::updateUiState()
{
    mConnectButton->setText(mClientConnected?tr("Disconnect"):tr("Connect"));
    mOpcUaPlugin->setDisabled(mClientConnected);
    mServerUrl->setDisabled(mClientConnected);
}

void MainWindow::log(const QString &text, const QString &context, QColor color)
{
    auto cf = mLog->currentCharFormat();
    cf.setForeground(color);
    mLog->setCurrentCharFormat(cf);
    mLog->appendPlainText(text);
    if (!context.isEmpty()) {
        cf.setForeground(Qt::gray);
        mLog->setCurrentCharFormat(cf);
        mLog->insertPlainText(context);
    }
}

void MainWindow::log(const QString &text, QColor color)
{
    log(text, QString(), color);
}

QT_END_NAMESPACE

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
#include "certificatedialog.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QTextCharFormat>
#include <QTextBlock>
#include <QOpcUaProvider>
#include <QOpcUaAuthenticationInformation>
#include <QOpcUaErrorState>

QT_BEGIN_NAMESPACE

static MainWindow *mainWindowGlobal = nullptr;
static QtMessageHandler oldMessageHandler = nullptr;

static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (!mainWindowGlobal)
        return;

   QString message;
   QColor color = Qt::black;

   switch (type) {
   case QtWarningMsg:
       message = QObject::tr("Warning");
       color = Qt::darkYellow;
       break;
   case QtCriticalMsg:
       message = QObject::tr("Critical");
       color = Qt::darkRed;
       break;
   case QtFatalMsg:
       message = QObject::tr("Fatal");
        color = Qt::darkRed;
       break;
   case QtInfoMsg:
       message = QObject::tr("Info");
       break;
   case QtDebugMsg:
       message = QObject::tr("Debug");
       break;
   }
   message += QLatin1String(": ");
   message += msg;

   const QString contextStr =
       QStringLiteral(" (%1:%2, %3)").arg(context.file).arg(context.line).arg(context.function);

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
  , ui(new Ui::MainWindow)
  , mOpcUaModel(new OpcUaModel(this))
  , mOpcUaProvider(new QOpcUaProvider(this))
{
    ui->setupUi(this);
    mainWindowGlobal = this;

    connect(ui->quitAction, &QAction::triggered, this, &QWidget::close);
    ui->quitAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));

    connect(ui->aboutAction, &QAction::triggered, this, &QApplication::aboutQt);
    ui->aboutAction->setShortcut(QKeySequence(QKeySequence::HelpContents));

    updateUiState();

    ui->opcUaPlugin->addItems(QOpcUaProvider::availableBackends());
    ui->treeView->setModel(mOpcUaModel);
    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    if (ui->opcUaPlugin->count() == 0) {
        ui->opcUaPlugin->setDisabled(true);
        ui->connectButton->setDisabled(true);
        QMessageBox::critical(this, tr("No OPCUA plugins available"), tr("The list of available OPCUA plugins is empty. No connection possible."));
    }

    connect(ui->findServersButton, &QPushButton::clicked, this, &MainWindow::findServers);
    connect(ui->host, &QLineEdit::returnPressed, this->ui->findServersButton,
            [this]() { this->ui->findServersButton->animateClick(); });
    connect(ui->getEndpointsButton, &QPushButton::clicked, this, &MainWindow::getEndpoints);
    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::connectToServer);
    oldMessageHandler = qInstallMessageHandler(&messageHandler);

    setupPkiConfiguration();

    //! [Application Identity]
    m_identity = m_pkiConfig.applicationIdentity();
    //! [Application Identity]
}

MainWindow::~MainWindow()
{
    delete ui;
}

//! [PKI Configuration]
void MainWindow::setupPkiConfiguration()
{
    QString pkidir = QCoreApplication::applicationDirPath();
#ifdef Q_OS_WIN
    pkidir += "../";
#endif
    pkidir += "/pki";
    m_pkiConfig.setClientCertificateFile(pkidir + "/own/certs/opcuaviewer.der");
    m_pkiConfig.setPrivateKeyFile(pkidir + "/own/private/opcuaviewer.pem");
    m_pkiConfig.setTrustListDirectory(pkidir + "/trusted/certs");
    m_pkiConfig.setRevocationListDirectory(pkidir + "/trusted/crl");
    m_pkiConfig.setIssuerListDirectory(pkidir + "/issuers/certs");
    m_pkiConfig.setIssuerRevocationListDirectory(pkidir + "/issuers/crl");

    // create the folders if they don't exist yet
    createPkiFolders();
}
//! [PKI Configuration]

void MainWindow::createClient()
{
    if (mOpcUaClient == nullptr) {
        mOpcUaClient = mOpcUaProvider->createClient(ui->opcUaPlugin->currentText());
        if (!mOpcUaClient) {
            const QString message(tr("Connecting to the given sever failed. See the log for details."));
            log(message, QString(), Qt::red);
            QMessageBox::critical(this, tr("Failed to connect to server"), message);
            return;
        }

        connect(mOpcUaClient, &QOpcUaClient::connectError, this, &MainWindow::showErrorDialog);
        mOpcUaClient->setApplicationIdentity(m_identity);
        mOpcUaClient->setPkiConfiguration(m_pkiConfig);

        if (mOpcUaClient->supportedUserTokenTypes().contains(QOpcUaUserTokenPolicy::TokenType::Certificate)) {
            QOpcUaAuthenticationInformation authInfo;
            authInfo.setCertificateAuthentication();
            mOpcUaClient->setAuthenticationInformation(authInfo);
        }

        connect(mOpcUaClient, &QOpcUaClient::connected, this, &MainWindow::clientConnected);
        connect(mOpcUaClient, &QOpcUaClient::disconnected, this, &MainWindow::clientDisconnected);
        connect(mOpcUaClient, &QOpcUaClient::errorChanged, this, &MainWindow::clientError);
        connect(mOpcUaClient, &QOpcUaClient::stateChanged, this, &MainWindow::clientState);
        connect(mOpcUaClient, &QOpcUaClient::endpointsRequestFinished, this, &MainWindow::getEndpointsComplete);
        connect(mOpcUaClient, &QOpcUaClient::findServersFinished, this, &MainWindow::findServersComplete);
    }
}

void MainWindow::findServers()
{
    QStringList localeIds;
    QStringList serverUris;
    QUrl url(ui->host->text());

    updateUiState();

    createClient();
    // set default port if missing
    if (url.port() == -1) url.setPort(4840);

    if (mOpcUaClient) {
        mOpcUaClient->findServers(url, localeIds, serverUris);
        qDebug() << "Discovering servers on " << url.toString();
    }
}

void MainWindow::findServersComplete(const QVector<QOpcUaApplicationDescription> &servers, QOpcUa::UaStatusCode statusCode)
{
    QOpcUaApplicationDescription server;

    if (isSuccessStatus(statusCode)) {
        ui->servers->clear();
        for (const auto &server : servers) {
            QVector<QString> urls = server.discoveryUrls();
            for (const auto &url : qAsConst(urls))
                ui->servers->addItem(url);
        }
    }

    updateUiState();
}

void MainWindow::getEndpoints()
{
    ui->endpoints->clear();
    updateUiState();

    if (ui->servers->currentIndex() >= 0) {
        const QString serverUrl = ui->servers->currentText();
        createClient();
        mOpcUaClient->requestEndpoints(serverUrl);
    }
}

void MainWindow::getEndpointsComplete(const QVector<QOpcUaEndpointDescription> &endpoints, QOpcUa::UaStatusCode statusCode)
{
    int index = 0;
    const char *modes[] = {
        "Invalid",
        "None",
        "Sign",
        "SignAndEncrypt"
    };

    if (isSuccessStatus(statusCode)) {
        mEndpointList = endpoints;
        for (const auto &endpoint : endpoints) {
            if (endpoint.securityMode() > sizeof(modes)) {
                qWarning() << "Invalid security mode";
                continue;
            }

            const QString EndpointName = QString("%1 (%2)")
                    .arg(endpoint.securityPolicy(), modes[endpoint.securityMode()]);
            ui->endpoints->addItem(EndpointName, index++);
        }
    }

    updateUiState();
}

void MainWindow::connectToServer()
{
    if (mClientConnected) {
        mOpcUaClient->disconnectFromEndpoint();
        return;
    }

    if (ui->endpoints->currentIndex() >= 0) {
        m_endpoint = mEndpointList[ui->endpoints->currentIndex()];
        createClient();
        mOpcUaClient->connectToEndpoint(m_endpoint);
    }
}

void MainWindow::clientConnected()
{
    mClientConnected = true;
    updateUiState();

    connect(mOpcUaClient, &QOpcUaClient::namespaceArrayUpdated, this, &MainWindow::namespacesArrayUpdated);
    mOpcUaClient->updateNamespaceArray();
}

void MainWindow::clientDisconnected()
{
    mClientConnected = false;
    mOpcUaClient->deleteLater();
    mOpcUaClient = nullptr;
    mOpcUaModel->setOpcUaClient(nullptr);
    updateUiState();
}

void MainWindow::namespacesArrayUpdated(const QStringList &namespaceArray)
{
    if (namespaceArray.isEmpty()) {
        qWarning() << "Failed to retrieve the namespaces array";
        return;
    }

    disconnect(mOpcUaClient, &QOpcUaClient::namespaceArrayUpdated, this, &MainWindow::namespacesArrayUpdated);
    mOpcUaModel->setOpcUaClient(mOpcUaClient);
    ui->treeView->header()->setSectionResizeMode(1 /* Value column*/, QHeaderView::Interactive);
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
    // allow changing the backend only if it was not already created
    ui->opcUaPlugin->setEnabled(mOpcUaClient == nullptr);
    ui->connectButton->setText(mClientConnected ? tr("Disconnect") : tr("Connect"));

    if (mClientConnected) {
        ui->host->setEnabled(false);
        ui->servers->setEnabled(false);
        ui->endpoints->setEnabled(false);
        ui->findServersButton->setEnabled(false);
        ui->getEndpointsButton->setEnabled(false);
        ui->connectButton->setEnabled(true);
    } else {
        ui->host->setEnabled(true);
        ui->servers->setEnabled(ui->servers->count() > 0);
        ui->endpoints->setEnabled(ui->endpoints->count() > 0);

        ui->findServersButton->setDisabled(ui->host->text().isEmpty());
        ui->getEndpointsButton->setEnabled(ui->servers->currentIndex() != -1);
        ui->connectButton->setEnabled(ui->endpoints->currentIndex() != -1);
    }

    if (!mOpcUaClient) {
        ui->servers->setEnabled(false);
        ui->endpoints->setEnabled(false);
        ui->getEndpointsButton->setEnabled(false);
        ui->connectButton->setEnabled(false);
    }
}

void MainWindow::log(const QString &text, const QString &context, const QColor &color)
{
    auto cf = ui->log->currentCharFormat();
    cf.setForeground(color);
    ui->log->setCurrentCharFormat(cf);
    ui->log->appendPlainText(text);
    if (!context.isEmpty()) {
        cf.setForeground(Qt::gray);
        ui->log->setCurrentCharFormat(cf);
        ui->log->insertPlainText(context);
    }
}

void MainWindow::log(const QString &text, const QColor &color)
{
    log(text, QString(), color);
}

bool MainWindow::createPkiPath(const QString &path)
{
    const QString msg = tr("Creating PKI path '%1': %2");

    QDir dir;
    const bool ret = dir.mkpath(path);
    if (ret)
        qDebug() << msg.arg(path, "SUCCESS.");
    else
        qCritical("%s", qPrintable(msg.arg(path, "FAILED.")));

    return ret;
}

bool MainWindow::createPkiFolders()
{
    bool result = createPkiPath(m_pkiConfig.trustListDirectory());
    if (!result)
        return result;

    result = createPkiPath(m_pkiConfig.revocationListDirectory());
    if (!result)
        return result;

    result = createPkiPath(m_pkiConfig.issuerListDirectory());
    if (!result)
        return result;

    result = createPkiPath(m_pkiConfig.issuerRevocationListDirectory());
    if (!result)
        return result;

    return result;
}

void MainWindow::showErrorDialog(QOpcUaErrorState *errorState)
{
    int result = 0;

    const QString statuscode = QOpcUa::statusToString(errorState->errorCode());

    QString msg = errorState->isClientSideError() ? tr("The client reported: ") : tr("The server reported: ");

    switch (errorState->connectionStep()) {
    case QOpcUaErrorState::ConnectionStep::Unknown:
        break;
    case QOpcUaErrorState::ConnectionStep::CertificateValidation: {
        CertificateDialog dlg(this);
        msg += tr("Server certificate validation failed with error 0x%1 (%2).\nClick 'Abort' to abort the connect, or 'Ignore' to continue connecting.")
                  .arg(static_cast<ulong>(errorState->errorCode()), 8, 16, QLatin1Char('0')).arg(statuscode);
        result = dlg.showCertificate(msg, m_endpoint.serverCertificate(), m_pkiConfig.trustListDirectory());
        errorState->setIgnoreError(result == 1);
    }
        break;
    case QOpcUaErrorState::ConnectionStep::OpenSecureChannel:
        msg += tr("OpenSecureChannel failed with error 0x%1 (%2).").arg(errorState->errorCode(), 8, 16, QLatin1Char('0')).arg(statuscode);
        QMessageBox::warning(this, tr("Connection Error"), msg);
        break;
    case QOpcUaErrorState::ConnectionStep::CreateSession:
        msg += tr("CreateSession failed with error 0x%1 (%2).").arg(errorState->errorCode(), 8, 16, QLatin1Char('0')).arg(statuscode);
        QMessageBox::warning(this, tr("Connection Error"), msg);
        break;
    case QOpcUaErrorState::ConnectionStep::ActivateSession:
        msg += tr("ActivateSession failed with error 0x%1 (%2).").arg(errorState->errorCode(), 8, 16, QLatin1Char('0')).arg(statuscode);
        QMessageBox::warning(this, tr("Connection Error"), msg);
        break;
    }
}

QT_END_NAMESPACE

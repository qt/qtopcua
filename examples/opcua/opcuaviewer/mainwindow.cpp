// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "mainwindow.h"
#include "opcuamodel.h"
#include "certificatedialog.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTextCharFormat>
#include <QTextBlock>
#include <QOpcUaProvider>
#include <QOpcUaAuthenticationInformation>
#include <QOpcUaErrorState>
#include <QOpcUaHistoryReadResponse>

#include <QOpcUaHistoryReadRawRequest>

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

MainWindow::MainWindow(const QString &initialUrl, QWidget *parent) : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , mOpcUaModel(new OpcUaModel(this))
  , mOpcUaProvider(new QOpcUaProvider(this))
{
    ui->setupUi(this);
    ui->host->setText(initialUrl);
    mainWindowGlobal = this;

    connect(ui->quitAction, &QAction::triggered, this, &QWidget::close);
    ui->quitAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));

    connect(ui->aboutAction, &QAction::triggered, this, &QApplication::aboutQt);
    ui->aboutAction->setShortcut(QKeySequence(QKeySequence::HelpContents));

    updateUiState();

    ui->opcUaPlugin->addItems(QOpcUaProvider::availableBackends());
    ui->treeView->setModel(mOpcUaModel);
    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    if (ui->opcUaPlugin->count() == 0) {
        QMessageBox::critical(this, tr("No OPCUA plugins available"), tr("The list of available OPCUA plugins is empty. No connection possible."));
    }

    mContextMenu = new QMenu(ui->treeView);
    mContextMenuMonitoringAction = mContextMenu->addAction(tr("Enable Monitoring"), this, &MainWindow::toggleMonitoring);
    mContextMenuHistorizingAction = mContextMenu->addAction(tr("Request historic data"), this, &MainWindow::showHistorizing);

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::openCustomContextMenu);

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

static bool copyDirRecursively(const QString &from, const QString &to)
{
    const QDir srcDir(from);
    const QDir targetDir(to);
    if (!QDir().mkpath(to))
        return false;

    const QFileInfoList infos =
            srcDir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    for (const QFileInfo &info : infos) {
        const QString srcItemPath = info.absoluteFilePath();
        const QString dstItemPath = targetDir.absoluteFilePath(info.fileName());
        if (info.isDir()) {
            if (!copyDirRecursively(srcItemPath, dstItemPath))
                return false;
        } else if (info.isFile()) {
            if (!QFile::copy(srcItemPath, dstItemPath))
                return false;
        }
    }
    return true;
}

//! [PKI Configuration]
void MainWindow::setupPkiConfiguration()
{
    const QDir pkidir =
            QDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/pki");

    if (!pkidir.exists() && !copyDirRecursively(":/pki", pkidir.path()))
        qFatal("Could not set up directory %s!", qUtf8Printable(pkidir.path()));

    m_pkiConfig.setClientCertificateFile(pkidir.absoluteFilePath("own/certs/opcuaviewer.der"));
    m_pkiConfig.setPrivateKeyFile(pkidir.absoluteFilePath("own/private/opcuaviewer.pem"));
    m_pkiConfig.setTrustListDirectory(pkidir.absoluteFilePath("trusted/certs"));
    m_pkiConfig.setRevocationListDirectory(pkidir.absoluteFilePath("trusted/crl"));
    m_pkiConfig.setIssuerListDirectory(pkidir.absoluteFilePath("issuers/certs"));
    m_pkiConfig.setIssuerRevocationListDirectory(pkidir.absoluteFilePath("issuers/crl"));

    const QStringList toCreate = { m_pkiConfig.issuerListDirectory(),
                                   m_pkiConfig.issuerRevocationListDirectory() };
    for (const QString &dir : toCreate) {
        if (!QDir().mkpath(dir))
            qFatal("Could not create directory %s!", qUtf8Printable(dir));
    }
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

void MainWindow::findServersComplete(const QList<QOpcUaApplicationDescription> &servers, QOpcUa::UaStatusCode statusCode)
{
    QOpcUaApplicationDescription server;

    if (isSuccessStatus(statusCode)) {
        ui->servers->clear();
        for (const auto &server : servers) {
            const auto urls = server.discoveryUrls();
            for (const auto &url : std::as_const(urls))
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

void MainWindow::getEndpointsComplete(const QList<QOpcUaEndpointDescription> &endpoints, QOpcUa::UaStatusCode statusCode)
{
    int index = 0;
    const std::array<const char *, 4> modes = {
        "Invalid",
        "None",
        "Sign",
        "SignAndEncrypt"
    };

    if (isSuccessStatus(statusCode)) {
        mEndpointList = endpoints;
        for (const auto &endpoint : endpoints) {
            if (endpoint.securityMode() >= modes.size()) {
                qWarning() << "Invalid security mode";
                continue;
            }

            const QString EndpointName = QStringLiteral("%1 (%2)")
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

void MainWindow::openCustomContextMenu(const QPoint &point)
{
    QModelIndex index = ui->treeView->indexAt(point);
    // show the context menu only for the value column
    if (index.isValid() && index.column() == 1) {
        TreeItem* item = static_cast<TreeItem *>(index.internalPointer());
        if (item) {
            mContextMenuMonitoringAction->setData(index);
            mContextMenuMonitoringAction->setEnabled(item->supportsMonitoring());
            mContextMenuMonitoringAction->setText(item->monitoringEnabled() ? tr("Disable Monitoring") : tr("Enable Monitoring"));

            mContextMenuHistorizingAction->setData(index);
            QModelIndex isHistoricIndex = mOpcUaModel->index(index.row(), 7, index.parent());
            mContextMenuHistorizingAction->setEnabled(mOpcUaModel->data(isHistoricIndex, Qt::DisplayRole).toString() == "true");
            mContextMenu->exec(ui->treeView->viewport()->mapToGlobal(point));
        }
    }
}

void MainWindow::toggleMonitoring()
{
    QModelIndex index = mContextMenuMonitoringAction->data().toModelIndex();
    if (index.isValid()) {
        TreeItem* item = static_cast<TreeItem *>(index.internalPointer());
        if (item) {
            item->setMonitoringEnabled(!item->monitoringEnabled());
        }
    }
}

void MainWindow::showHistorizing()
{
    QModelIndex modelIndex = mContextMenuHistorizingAction->data().toModelIndex();
    QModelIndex nodeIdIndex = mOpcUaModel->index(modelIndex.row(), 4, modelIndex.parent());
    QString nodeId = mOpcUaModel->data(nodeIdIndex, Qt::DisplayRole).toString();
    auto request = QOpcUaHistoryReadRawRequest(
                {QOpcUaReadItem(nodeId)},
                QDateTime::currentDateTime(),
                QDateTime::currentDateTime().addDays(-2),
                5,
                false
                );
    mHistoryReadResponse.reset(mOpcUaClient->readHistoryData(request));

    if (mHistoryReadResponse) {
        QObject::connect(mHistoryReadResponse.get(), &QOpcUaHistoryReadResponse::readHistoryDataFinished,
                         this, &MainWindow::handleReadHistoryDataFinished);
        QObject::connect(mHistoryReadResponse.get(), &QOpcUaHistoryReadResponse::stateChanged, this, [](QOpcUaHistoryReadResponse::State state) {
            qDebug() << "History read state changed to" << state;
        });
    } else {
        qWarning() << "Failed to request history data";
    }
}

void MainWindow::handleReadHistoryDataFinished(QList<QOpcUaHistoryData> results, QOpcUa::UaStatusCode serviceResult)
{
    if (serviceResult != QOpcUa::UaStatusCode::Good) {
        qWarning() << "readHistoryData request finished with bad status code: " << serviceResult;
        return;
    }

    for (int i = 0; i < results.count(); ++i) {
        qInfo() << "NodeId:" << results.at(i).nodeId() << "; statusCode:" << results.at(i).statusCode() << "; returned values:" << results.at(i).count();
        for (int j = 0; j < results.at(i).count(); ++j) {
            qInfo() << j
                       << "source timestamp:" << results.at(i).result()[j].sourceTimestamp()
                       << "server timestamp:" <<  results.at(i).result()[j].serverTimestamp()
                       << "value:" << results.at(i).result()[j].value();

        }
    }
}

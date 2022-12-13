// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QItemSelection>
#include <QMainWindow>
#include <QOpcUaClient>
#include <QOpcUaHistoryData>
#include <QOpcUaProvider>

class OpcUaModel;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(const QString &initialUrl, QWidget *parent = nullptr);
    ~MainWindow();
    Q_INVOKABLE void log(const QString &text, const QString &context, const QColor &color);
    void log(const QString &text, const QColor &color = Qt::black);

private slots:
    void connectToServer();
    void findServers();
    void findServersComplete(const QList<QOpcUaApplicationDescription> &servers, QOpcUa::UaStatusCode statusCode);
    void getEndpoints();
    void getEndpointsComplete(const QList<QOpcUaEndpointDescription> &endpoints, QOpcUa::UaStatusCode statusCode);
    void clientConnected();
    void clientDisconnected();
    void namespacesArrayUpdated(const QStringList &namespaceArray);
    void clientError(QOpcUaClient::ClientError);
    void clientState(QOpcUaClient::ClientState);
    void showErrorDialog(QOpcUaErrorState *errorState);
    void openCustomContextMenu(const QPoint &point);
    void toggleMonitoring();
    void showHistorizing();
    void handleReadHistoryDataFinished(QList<QOpcUaHistoryData> results, QOpcUa::UaStatusCode serviceResult);

private:
    void createClient();
    void updateUiState();
    void setupPkiConfiguration();

private:
    Ui::MainWindow *ui;
    OpcUaModel *mOpcUaModel;
    QOpcUaProvider *mOpcUaProvider;
    QOpcUaClient *mOpcUaClient = nullptr;
    QList<QOpcUaEndpointDescription> mEndpointList;
    bool mClientConnected = false;
    QOpcUaApplicationIdentity m_identity;
    QOpcUaPkiConfiguration m_pkiConfig;
    QOpcUaEndpointDescription m_endpoint; // current endpoint used to connect
    QMenu *mContextMenu;
    QAction *mContextMenuMonitoringAction;
    QAction *mContextMenuHistorizingAction;
    QScopedPointer<QOpcUaHistoryReadResponse> mHistoryReadResponse;
};

#endif // MAINWINDOW_H

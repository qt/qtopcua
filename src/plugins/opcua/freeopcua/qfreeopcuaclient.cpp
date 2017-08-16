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

#include "qfreeopcuaclient.h"
#include <private/qopcuaclient_p.h>
#include <private/qopcuasubscription_p.h>

#include "qfreeopcuanode.h"
#include "qfreeopcuavalueconverter.h"
#include "qfreeopcuasubscription.h"

#include <qopcuasubscription.h>

// freeopcua
#include <opc/ua/node.h>
#include <opc/ua/client/client.h>
#include <opc/ua/protocol/variant.h>
#include <opc/ua/services/method.h>

#include <QtCore/qdebug.h>
#include <QtNetwork/qhostinfo.h>
#include <QtConcurrent/QtConcurrent>

QT_BEGIN_NAMESPACE

class FreeOpcuaWorker : public QObject, public OpcUa::UaClient
{
    Q_OBJECT
public:
    FreeOpcuaWorker() : QObject() {}

    QOpcUaNode *node(const QString &nodeId, QFreeOpcUaClientImpl *client);
    QOpcUaSubscription *createSubscription(quint32 interval);

signals:
    void connectFinished(bool succeeded);
    void disconnectFinished(bool succeeded);

public slots:
    void stopWorker();
    void asyncConnectToEndpoint(const QUrl &url);
    void asyncDisconnectFromEndpoint();
};

void FreeOpcuaWorker::stopWorker()
{
    QThread::currentThread()->quit();
}

void FreeOpcuaWorker::asyncConnectToEndpoint(const QUrl &url)
{
    try {
        QString sNodeName = QHostInfo::localHostName();
        SetApplicationURI(QString("urn:%1:%2:%3").arg(
                              sNodeName).arg("qt-project").arg("QOpcUaClient").toStdString());
        SetProductURI("urn:qt-project:QOpcUaClient");
        SetSessionName(GetApplicationURI());

        Connect(url.toString().toStdString());

        // Check connection status by getting the root node
        GetRootNode();
    } catch (const std::exception &e) {
        Disconnect();
        qWarning() << "Client could not connect to endpoint" << url;
        qWarning() << e.what();
        emit connectFinished(false);
        return;
    }
    emit connectFinished(true);
}

void FreeOpcuaWorker::asyncDisconnectFromEndpoint()
{
    try {
        Disconnect();
        emit disconnectFinished(true);
        return;
    } catch (const std::exception &ex) {
        qWarning() << "Could not disconnect from endpoint: " << ex.what();
    }

    emit disconnectFinished(false);
}

class WorkerThread : public QThread
{
    Q_OBJECT
public:
    WorkerThread() : QThread(), opcuaWorker(0) {}

    void setupWorker(QFreeOpcUaClientImpl* client);
    void stopWorker();
    void connectToServer(const QUrl &url);
    void disconnectServer();

    // for now easy pass through
    // TODO make thread safe
    QPointer<FreeOpcuaWorker> worker() { return opcuaWorker; }

private:
    QPointer<FreeOpcuaWorker> opcuaWorker;
};

void WorkerThread::setupWorker(QFreeOpcUaClientImpl* client)
{
    opcuaWorker = new FreeOpcuaWorker();
    opcuaWorker->moveToThread(this);
    connect(this, &QThread::finished, opcuaWorker, &QObject::deleteLater);
    connect(this, &QThread::finished, this, &QObject::deleteLater);
    connect(opcuaWorker, &FreeOpcuaWorker::connectFinished,
            client, &QFreeOpcUaClientImpl::connectToEndpointFinished);
    connect(opcuaWorker, &FreeOpcuaWorker::disconnectFinished,
            client, &QFreeOpcUaClientImpl::disconnectFromEndpointFinished);
}

void WorkerThread::connectToServer(const QUrl &url)
{
    QMetaObject::invokeMethod(opcuaWorker, "asyncConnectToEndpoint", Qt::QueuedConnection,
                              Q_ARG(QUrl, url));
}

void WorkerThread::disconnectServer()
{
    QMetaObject::invokeMethod(opcuaWorker, "asyncDisconnectFromEndpoint", Qt::QueuedConnection);
}

void WorkerThread::stopWorker()
{
    QMetaObject::invokeMethod(opcuaWorker, "stopWorker", Qt::QueuedConnection);
}

QFreeOpcUaClientImpl::QFreeOpcUaClientImpl()
    : QOpcUaClientImpl()
{
    workerThread = new WorkerThread();
    workerThread->setupWorker(this);
    workerThread->start();
}

QFreeOpcUaClientImpl::~QFreeOpcUaClientImpl()
{
    workerThread->stopWorker();
    workerThread = 0; // worker thread cleans itself up after stopping
}

void QFreeOpcUaClientImpl::connectToEndpoint(const QUrl &url)
{
    workerThread->connectToServer(url);
}

void QFreeOpcUaClientImpl::connectToEndpointFinished(bool succeeded)
{
    if (succeeded)
        emit stateAndOrErrorChanged(QOpcUaClient::Connected, QOpcUaClient::NoError);
    else
        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::NoError);
}

void QFreeOpcUaClientImpl::secureConnectToEndpoint(const QUrl &)
{
    // No need to do something, never reached
}

void QFreeOpcUaClientImpl::disconnectFromEndpoint()
{
    workerThread->disconnectServer();
}

void QFreeOpcUaClientImpl::disconnectFromEndpointFinished(bool succeeded)
{
    if (succeeded)
        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::NoError);
    else
        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::UnknownError);
}

QOpcUaNode *QFreeOpcUaClientImpl::node(const QString &nodeId)
{
    QPointer<FreeOpcuaWorker> opcuaWorker = workerThread->worker();
    if (opcuaWorker.isNull())
        return nullptr;

    return opcuaWorker->node(nodeId, this);
}

QOpcUaNode *FreeOpcuaWorker::node(const QString &nodeId, QFreeOpcUaClientImpl *clientImpl)
{
    try {
        OpcUa::Node node = GetNode(nodeId.toStdString());
        node.GetBrowseName(); // make the client fetch the node data from the server
        QFreeOpcUaNode* n = new QFreeOpcUaNode(node, this);
        return new QOpcUaNode(n, clientImpl->m_client);
    } catch (const std::exception &ex) {
        qWarning() << "Could not get node: " << nodeId << " " << ex.what();
        return nullptr;
    }
    return nullptr;
}

QOpcUaSubscription *QFreeOpcUaClientImpl::createSubscription(quint32 interval)
{
    QPointer<FreeOpcuaWorker> opcuaWorker = workerThread->worker();
    if (opcuaWorker.isNull())
        return nullptr;

    return opcuaWorker->createSubscription(interval);
}

QOpcUaSubscription *FreeOpcuaWorker::createSubscription(quint32 interval)
{
    QFreeOpcUaSubscription *backendSubscription = new QFreeOpcUaSubscription(this, interval);
    QOpcUaSubscription *subscription = new QOpcUaSubscription(backendSubscription, interval);
    backendSubscription->m_qsubscription = subscription;
    return subscription;
}

QT_END_NAMESPACE

#include <qfreeopcuaclient.moc>

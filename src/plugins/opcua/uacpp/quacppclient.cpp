/******************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
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
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#include "quacppbackend.h"
#include "quacppclient.h"
#include "quacppnode.h"
#include "quacpputils.h"

#include <private/qopcuaclient_p.h>

#include <QtCore/QLoggingCategory>
#include <QtCore/QStringList>
#include <QtCore/QThread>
#include <QtCore/QUrl>
#include <QtCore/QUuid>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_UACPP)

QUACppClient::QUACppClient()
    : QOpcUaClientImpl()
    , m_backend(new UACppAsyncBackend(this))
{
    m_thread = new QThread();
    connectBackendWithClient(m_backend);
    m_backend->moveToThread(m_thread);
    connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);
    connect(m_thread, &QThread::finished, m_backend, &QObject::deleteLater);
    m_thread->start();
}

QUACppClient::~QUACppClient()
{
    if (m_thread->isRunning())
        m_thread->quit();
}

void QUACppClient::connectToEndpoint(const QUrl &url)
{
    QMetaObject::invokeMethod(m_backend, "connectToEndpoint", Qt::QueuedConnection, Q_ARG(QUrl, url));
}

void QUACppClient::disconnectFromEndpoint()
{
    QMetaObject::invokeMethod(m_backend, "disconnectFromEndpoint", Qt::QueuedConnection);
}

QOpcUaNode *QUACppClient::node(const QString &nodeId)
{
    UaNodeId nativeId = UACppUtils::nodeIdFromQString(nodeId);
    if (nativeId.isNull())
        return nullptr;

    auto tempNode = new QUACppNode(nativeId, this, nodeId);
    if (!tempNode->registered()) {
        qCDebug(QT_OPCUA_PLUGINS_UACPP) << "Failed to register node with backend, maximum number of nodes reached.";
        delete tempNode;
        return nullptr;
    }
    return new QOpcUaNode(tempNode, m_client);
}

QString QUACppClient::backend() const
{
    return QStringLiteral("uacpp");
}

bool QUACppClient::requestEndpoints(const QUrl &url)
{
    return QMetaObject::invokeMethod(m_backend, "requestEndpoints", Qt::QueuedConnection, Q_ARG(QUrl, url));
}

bool QUACppClient::findServers(const QUrl &url, const QStringList &localeIds, const QStringList &serverUris)
{
    Q_UNUSED(url);
    Q_UNUSED(localeIds);
    Q_UNUSED(serverUris);

    qInfo("The uacpp backend does not yet support the FindServers service.");

    return false;
}

bool QUACppClient::batchRead(const QVector<QOpcUaReadItem> &nodesToRead)
{
    Q_UNUSED(nodesToRead);
    qInfo("Batch read is currently not implemented in the uacpp backend");
    return false;
}

bool QUACppClient::batchWrite(const QVector<QOpcUaWriteItem> &nodesToWrite)
{
    Q_UNUSED(nodesToWrite);

    qInfo("Batch write is currently not implemented in the uacpp backend");
    return false;
}

QT_END_NAMESPACE

/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
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

#include "qopen62541client.h"
#include "qopen62541node.h"
#include "qopen62541subscription.h"
#include "qopen62541valueconverter.h"
#include "qopen62541backend.h"
#include "qopen62541utils.h"

#include <QtCore/QStringList>
#include <QtCore/QThread>
#include <QtCore/QUrl>

#include <QtOpcUa/private/qopcuaclient_p.h>

QOpen62541Client::QOpen62541Client()
    : QOpcUaClientImpl()
    , m_backend(new Open62541AsyncBackend(this))
{
    m_thread = new QThread();
    m_backend->moveToThread(m_thread);
    connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);
    connect(m_thread, &QThread::finished, m_backend, &QObject::deleteLater);
    m_thread->start();
}

QOpen62541Client::~QOpen62541Client()
{
    if (m_thread->isRunning())
        m_thread->quit();
}

void QOpen62541Client::connectToEndpoint(const QUrl &url)
{
    QMetaObject::invokeMethod(m_backend, "connectToEndpoint", Qt::QueuedConnection, Q_ARG(QUrl, url));
}

void QOpen62541Client::secureConnectToEndpoint(const QUrl &url)
{
    Q_UNIMPLEMENTED();
    Q_UNUSED(url)
    emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::SecureConnectionError);
    return;
}

void QOpen62541Client::disconnectFromEndpoint()
{
    QMetaObject::invokeMethod(m_backend, "disconnectFromEndpoint", Qt::QueuedConnection);
}

QOpcUaNode *QOpen62541Client::node(const QString &nodeId)
{
    UA_NodeId uaNodeId = Open62541Utils::nodeIdFromQString(nodeId);

    return new QOpcUaNode(new QOpen62541Node(uaNodeId, this, nodeId), m_client);
}

QOpcUaSubscription *QOpen62541Client::createSubscription(quint32 interval)
{
    QOpen62541Subscription *backendSubscription = new QOpen62541Subscription(m_backend, interval);
    QOpcUaSubscription *subscription = new QOpcUaSubscription(backendSubscription, interval);
    backendSubscription->m_qsubscription = subscription;
    return subscription;
}

QString QOpen62541Client::backend() const
{
    return QStringLiteral("open62541");
}

UA_Client *QOpen62541Client::nativeClient() const
{
    return m_backend->m_uaclient;
}


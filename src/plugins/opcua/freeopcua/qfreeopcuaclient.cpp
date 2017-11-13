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
#include "qfreeopcuanode.h"
#include "qfreeopcuaworker.h"
#include <private/qopcuaclient_p.h>

#include <QtCore/qloggingcategory.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_FREEOPCUA)

QFreeOpcUaClientImpl::QFreeOpcUaClientImpl()
    : QOpcUaClientImpl()
{
    m_thread = new QThread();
    m_opcuaWorker = new QFreeOpcUaWorker(this);
    connectBackendWithClient(m_opcuaWorker);
    m_opcuaWorker->moveToThread(m_thread);
    connect(m_thread, &QThread::finished, m_opcuaWorker, &QObject::deleteLater);
    connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);
    m_thread->start();
}

QFreeOpcUaClientImpl::~QFreeOpcUaClientImpl()
{
    if (m_thread->isRunning())
        m_thread->quit();
}

void QFreeOpcUaClientImpl::connectToEndpoint(const QUrl &url)
{
    QMetaObject::invokeMethod(m_opcuaWorker, "asyncConnectToEndpoint", Qt::QueuedConnection,
                              Q_ARG(QUrl, url));
}

void QFreeOpcUaClientImpl::secureConnectToEndpoint(const QUrl &)
{
    // No need to do something, never reached
}

void QFreeOpcUaClientImpl::disconnectFromEndpoint()
{
    QMetaObject::invokeMethod(m_opcuaWorker, "asyncDisconnectFromEndpoint", Qt::QueuedConnection);
}

QOpcUaNode *QFreeOpcUaClientImpl::node(const QString &nodeId)
{
    if (!m_opcuaWorker)
        return new QOpcUaNode(new QFreeOpcUaNode(OpcUa::Node(), nullptr), m_client);

    try {
        OpcUa::Node node = m_opcuaWorker->GetNode(nodeId.toStdString());
        QFreeOpcUaNode *n = new QFreeOpcUaNode(node, this);
        return new QOpcUaNode(n, m_client);
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Could not get node: %s %s", qUtf8Printable(nodeId), ex.what());
        return new QOpcUaNode(new QFreeOpcUaNode(OpcUa::Node(), this), m_client);
    }
}

QT_END_NAMESPACE

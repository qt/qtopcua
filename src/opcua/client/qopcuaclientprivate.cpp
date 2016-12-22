/****************************************************************************
**
** Copyright (C) 2016 basysKom GmbH, opensource@basyskom.com
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

#include <private/qopcuaclient_p.h>
#include <QDebug>

QOpcUaClientPrivate::QOpcUaClientPrivate(QOpcUaClientImpl *impl, QOpcUaClient *parent)
    : QObject(parent)
    , m_impl(impl)
    , m_state(QOpcUaClient::UnconnectedState)
    , q_ptr(parent)
{
    connect(m_impl.data(), &QOpcUaClientImpl::connected,
            this, &QOpcUaClientPrivate::clientConnected);
    connect(m_impl.data(), &QOpcUaClientImpl::disconnected,
            this, &QOpcUaClientPrivate::clientDisconnected);
}

QOpcUaClientPrivate::~QOpcUaClientPrivate()
{
}

void QOpcUaClientPrivate::connectToEndpoint(const QUrl &url)
{
    Q_Q(QOpcUaClient);

    bool result = processUrl(url);
    if (result) {
        m_impl->connectToEndpoint(url);
        m_state = QOpcUaClient::ConnectingState;
        emit q->stateChanged(m_state);
    } else {
        emit q->disconnected();
        emit q->error(QOpcUaClient::InvalidUrl);
    }
}

void QOpcUaClientPrivate::secureConnectToEndpoint(const QUrl &url)
{
    Q_Q(QOpcUaClient);

    if (!m_impl->isSecureConnectionSupported()) {
        qWarning("Backend does not support secure connections. Cancelling connection.");
        return;
    }

    bool result = processUrl(url);
    if (result) {
        m_impl->secureConnectToEndpoint(url);
        m_state = QOpcUaClient::ClosingState;
        emit q->stateChanged(m_state);
    } else {
        emit q->disconnected();
        emit q->error(QOpcUaClient::InvalidUrl);
    }
}

void QOpcUaClientPrivate::disconnectFromEndpoint()
{
    if (m_state != QOpcUaClient::ConnectedState)
        return;

    m_impl->disconnectFromEndpoint();
}

bool QOpcUaClientPrivate::processUrl(const QUrl &url)
{
    if (url.scheme() != QStringLiteral("opc.tcp")) {
        qWarning() << "Wrong url scheme, could not connect";
        return false;
    }

    m_url = url;
    return true;
}

void QOpcUaClientPrivate::clientConnected()
{
    Q_Q(QOpcUaClient);

    m_state = QOpcUaClient::ConnectedState;
    emit q->stateChanged(m_state);
    emit q->connected();
}

void QOpcUaClientPrivate::clientDisconnected()
{
    Q_Q(QOpcUaClient);

    m_state = QOpcUaClient::UnconnectedState;
    emit q->stateChanged(m_state);
    emit q->disconnected();
}

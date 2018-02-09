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

#include <QtCore/qloggingcategory.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA)

QOpcUaClientPrivate::QOpcUaClientPrivate(QOpcUaClientImpl *impl)
    : QObjectPrivate()
    , m_impl(impl)
    , m_state(QOpcUaClient::Disconnected)
    , m_error(QOpcUaClient::NoError)
{
    // callback from client implementation
    QObject::connect(m_impl.data(), &QOpcUaClientImpl::stateAndOrErrorChanged,
                    [this](QOpcUaClient::ClientState state, QOpcUaClient::ClientError error) {
        setStateAndError(state, error);
    });
}

QOpcUaClientPrivate::~QOpcUaClientPrivate()
{
}

void QOpcUaClientPrivate::connectToEndpoint(const QUrl &url)
{
    bool result = checkAndSetUrl(url);
    if (result) {
        setStateAndError(QOpcUaClient::Connecting);
        m_impl->connectToEndpoint(url);
    } else {
        setStateAndError(QOpcUaClient::Disconnected, QOpcUaClient::InvalidUrl);
    }
}

void QOpcUaClientPrivate::disconnectFromEndpoint()
{
    if (m_state != QOpcUaClient::Connected) {
        qCWarning(QT_OPCUA) << "Closing a connection without being connected";
        return;
    }

    setStateAndError(QOpcUaClient::Closing);
    m_impl->disconnectFromEndpoint();
}

bool QOpcUaClientPrivate::checkAndSetUrl(const QUrl &url)
{
    if (url.scheme() != QStringLiteral("opc.tcp")) {
        qCWarning(QT_OPCUA) << "Wrong url scheme, could not connect";
        return false;
    }

    m_url = url;
    return true;
}

void QOpcUaClientPrivate::setStateAndError(QOpcUaClient::ClientState state,
                                           QOpcUaClient::ClientError error)
{
     Q_Q(QOpcUaClient);

    // ensure that state and error transition are atomic before signal emission
    bool stateChanged = false;
    bool errorOccurred = false;

    if (m_state != state) {
        m_state = state;
        stateChanged = true;
    }
    if (error != QOpcUaClient::NoError && m_error != error) {
        errorOccurred = true;
    }
    m_error = error;

    if (errorOccurred)
        emit q->errorChanged(m_error);
    if (stateChanged) {
        emit q->stateChanged(m_state);

        if (m_state == QOpcUaClient::Connected)
            emit q->connected();
        else if (m_state == QOpcUaClient::Disconnected)
            emit q->disconnected();
    }

    // According to UPC-UA part 5, page 23, the server is allowed to change entries of the namespace
    // array if there is no active session. This could invalidate the cached namespaces table.
    if (state == QOpcUaClient::Disconnected) {
        m_namespaceArray.clear();
    }
}

bool QOpcUaClientPrivate::updateNamespaceArray()
{
    if (m_state != QOpcUaClient::ClientState::Connected)
        return false;

    if (!m_namespaceArrayNode) {
        m_namespaceArrayNode.reset(m_impl->node(QStringLiteral("ns=0;i=2255")));
        if (!m_namespaceArrayNode)
            return false;
        QObjectPrivate::connect(m_namespaceArrayNode.data(), &QOpcUaNode::attributeRead, this, &QOpcUaClientPrivate::namespaceArrayUpdated);
    }

    return m_namespaceArrayNode->readAttributes(QOpcUa::NodeAttribute::Value);
}

QStringList QOpcUaClientPrivate::namespaceArray() const
{
    return m_namespaceArray;
}

void QOpcUaClientPrivate::namespaceArrayUpdated(QOpcUa::NodeAttributes attr)
{
    Q_Q(QOpcUaClient);

    const QVariant value = m_namespaceArrayNode->attribute(QOpcUa::NodeAttribute::Value);

    if (!(attr & QOpcUa::NodeAttribute::Value) || value.type() != QVariant::Type::List) {
        m_namespaceArray.clear();
        emit q->namespaceArrayUpdated(QStringList());
        return;
    }

    m_namespaceArray.clear();
    for (auto it : value.toList())
        m_namespaceArray.append(it.toString());

    emit q->namespaceArrayUpdated(m_namespaceArray);
}

QT_END_NAMESPACE

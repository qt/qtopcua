/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#include "opcuaconnection.h"
#include <QOpcUaProvider>
#include <QLoggingCategory>

QT_BEGIN_NAMESPACE

/*!
    \qmltype Connection
    \inqmlmodule QtOpcUa
    \brief Connects to a server.
    \since QtOpcUa 5.12

    The main API uses backends to make connections. You have to set the backend before
    any connection attempt.

    \code
    import QtOpcUa 5.13 as QtOpcUa

    QtOpcUa.Connection {
        backend: "open62541"
    }

    Component.onCompleted: {
        connection.connectToEndpoint("opc.tcp://127.0.0.1:43344");
    }
    \endcode
*/

/*!
    \qmlproperty stringlist Connection::availableBackends
    \readonly

    Returns the names of all available backends as a list.
    These are used to select a backend when connecting.

    \sa Connection::backend
*/

/*!
    \qmlproperty bool Connection::connected
    \readonly

    Status of the connection.
    \c true when there is a connection, otherwise \c false.
*/

/*!
    \qmlproperty string Connection::backend

    Set the backend to use for a connection to the server.
    Has to be set before any connection attempt.

    \sa Connection::availableBackends
*/

/*!
    \qmlproperty bool Connection::defaultConnection

    Makes this the default connection.
    Usually each node needs to be given a connection to use. If this property
    is set to \c true, this connection will be used in all cases where a node has no
    connection set. Already established connections are not affected.
    If \c defaultConnection is set to \c true on multiple connection the last one is used.

    \code
    QtOpcUa.Connection {
        ...
        defaultConnection: true
        ...
    }
    \endcode

    \sa Node
*/

/*!
    \qmlproperty stringlist Connection::namespaces
    \readonly

    List of strings of all namespace URIs registered on the connected server.
*/

/*!
    \qmlproperty AuthenticationInformation Connection::authenticationInformation

    Set the authentication information to this connection. The authentication information has
    to be set before calling \l connectToEndpoint. If no authentication information is set,
    the anonymous mode will be used.
    It has no effect on the current connection. If the client is disconnected and then reconnected,
    the new credentials are used.
    Reading and writing this property before a \l backend is set, writes are ignored and reads return
    and invalid \l AuthenticationInformation.
*/

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_QML)

OpcUaConnection* OpcUaConnection::m_defaultConnection = nullptr;

OpcUaConnection::OpcUaConnection(QObject *parent):
    QObject(parent)
{
}

OpcUaConnection::~OpcUaConnection()
{
    setDefaultConnection(false);
    if (m_client) {
        m_client->deleteLater();
        m_client = nullptr;
    }
}

QStringList OpcUaConnection::availableBackends() const
{
    return QOpcUaProvider::availableBackends();
}

bool OpcUaConnection::connected() const
{
    return m_connected && m_client;
}

void OpcUaConnection::setBackend(const QString &name)
{
    if (name.isEmpty())
        return;

    if (!availableBackends().contains(name)) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << tr("Backend '%1' is not available").arg(name);
        qCDebug(QT_OPCUA_PLUGINS_QML) << tr("Available backends:") << availableBackends().join(QLatin1Char(','));
        return;
    }

    if (m_client) {
        if (m_client->backend() == name)
            return;

        m_client->disconnectFromEndpoint();
        m_client->disconnect(this);
        m_client->deleteLater();
    }

    QOpcUaProvider provider;
    m_client = provider.createClient(name);
    if (m_client) {
        qCDebug(QT_OPCUA_PLUGINS_QML) << "Created plugin" << m_client->backend();
        connect(m_client, &QOpcUaClient::stateChanged, this, &OpcUaConnection::clientStateHandler);
        connect(m_client, &QOpcUaClient::namespaceArrayUpdated, this, &OpcUaConnection::namespacesChanged);
        connect(m_client, &QOpcUaClient::namespaceArrayUpdated, this, [&]() {
            if (!m_connected) {
                m_connected = true;
                emit connectedChanged();
            }
        });
        m_client->setNamespaceAutoupdate(true);
    } else {
        qCWarning(QT_OPCUA_PLUGINS_QML) << tr("Backend '%1' could not be created.").arg(name);
    }
    emit backendChanged();
}

QString OpcUaConnection::backend() const
{
    if (m_client)
        return m_client->backend();
    else
        return QString();
}

OpcUaConnection *OpcUaConnection::defaultConnection()
{
    return m_defaultConnection;
}

bool OpcUaConnection::isDefaultConnection() const
{
    return m_defaultConnection == this;
}

/*!
    \qmlmethod Connection::connectToEndpoint(url)

    Connects to the given endpoint.
*/

void OpcUaConnection::connectToEndpoint(const QUrl &url)
{
    if (!m_client)
        return;

    connect(m_client, &QOpcUaClient::endpointsRequestFinished, this,
            &OpcUaConnection::requestEndpointsFinishedHandler, Qt::UniqueConnection);
    m_client->requestEndpoints(url.toString());
}

/*!
    \qmlmethod Connection::disconnectFromEndpoint(url)

    Disconnects an established connection.
*/

void OpcUaConnection::disconnectFromEndpoint()
{
    if (!m_client)
        return;

    m_client->disconnectFromEndpoint();
}

void OpcUaConnection::setDefaultConnection(bool defaultConnection)
{
    if (!defaultConnection && m_defaultConnection == this)
        m_defaultConnection = nullptr;

    if (defaultConnection)
        m_defaultConnection = this;

    emit defaultConnectionChanged();
}

void OpcUaConnection::clientStateHandler(QOpcUaClient::ClientState state)
{
    if (m_connected) {
        // don't immediately send the state; we have to wait for the namespace
        // array to be updated
        m_connected = (state == QOpcUaClient::ClientState::Connected);
        emit connectedChanged();
    }
}

void OpcUaConnection::requestEndpointsFinishedHandler(const QVector<QOpcUaEndpointDescription> &endpoints)
{
    disconnect(m_client, &QOpcUaClient::endpointsRequestFinished, this, &OpcUaConnection::requestEndpointsFinishedHandler);

    bool found = false;
    QOpcUaEndpointDescription chosenEndpoint;

    for (const auto &endpoint : qAsConst(endpoints)) {
        if (QUrl(endpoint.endpointUrl()).scheme() != QLatin1String("opc.tcp"))
            continue;
        if (endpoint.securityPolicyUri() == QLatin1String("http://opcfoundation.org/UA/SecurityPolicy#None")) {
            found = true;
            chosenEndpoint = endpoint;
            break;
        }
    }

    if (!found) {
        m_connected = false;
        emit connectedChanged();
    } else {
        m_client->connectToEndpoint(chosenEndpoint);
    }
}

QStringList OpcUaConnection::namespaces() const
{
    if (!m_client)
        return QStringList();

    return m_client->namespaceArray();
}

void OpcUaConnection::setAuthenticationInformation(const QOpcUaAuthenticationInformation &authenticationInformation)
{
    if (!m_client)
        return;
    m_client->setAuthenticationInformation(authenticationInformation);
}

QOpcUaAuthenticationInformation OpcUaConnection::authenticationInformation() const
{
    if (!m_client)
        return QOpcUaAuthenticationInformation();

    return m_client->authenticationInformation();
}

QT_END_NAMESPACE

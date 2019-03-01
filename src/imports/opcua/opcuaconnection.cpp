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
#include "opcuareadresult.h"
#include "universalnode.h"
#include <QJSEngine>
#include <QLoggingCategory>
#include <QOpcUaProvider>
#include <QOpcUaReadItem>
#include <QOpcUaReadResult>

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

/*!
    \qmlproperty stringlist Connection::supportedSecurityPolicies
    \since 5.13

    A list of strings containing the supported security policies

    This property is currently available as a Technology Preview, and therefore the API
    and functionality provided may be subject to change at any time without
    prior notice.
*/

/*!
    \qmlproperty array[tokenTypes] Connection::supportedUserTokenTypes
    \since 5.13

    An array of user token policy types of all supported user token types.

    This property is currently available as a Technology Preview, and therefore the API
    and functionality provided may be subject to change at any time without
    prior notice.
*/

/*!
    \qmlsignal Connection::readNodeAttributesFinished(readResults)

    Emitted when the read request, started using \l readNodeAttributes(), is finished.
    The parameter of this signal is an array of \l ReadResult, which contains the
    values requested from the server.

    \code
    connection.onReadNodeAttributesFinished(results) {
        for (var i = 0; results.length; i++) {
            if (results[i].status.isGood) {
                console.log(results[i].value);
            } else {
                // handle error
            }
        }
    }
    \endcode

    \sa readNodeAttributes(), ReadResult
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
        connect(m_client, &QOpcUaClient::readNodeAttributesFinished, this, &OpcUaConnection::handleReadNodeAttributesFinished);
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
    \qmlmethod Connection::connectToEndpoint(endpointDescription)

    Connects to the given endpoint.

    \sa EndpointDescription endpoints
*/

void OpcUaConnection::connectToEndpoint(const QOpcUaEndpointDescription &endpointDescription)
{
    if (!m_client)
        return;

    m_client->connectToEndpoint(endpointDescription);
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

/*!
    \qmlmethod Connection::readNodeAttributes(valuesToBeRead)

    This function is used to read multiple values from a server in one go.
    Returns \c true if the read request was dispatched successfully.

    The values to be read have to be passed as JavaScript array of \l ReadItem.

    \code
    // List of items to read
    var readItemList = [];
    // Item to be added to the list of items to be read
    var readItem;

    // Prepare an item to be read

    // Create a new read item and fill properties
    readItem = QtOpcUa.ReadItem.create();
    readItem.ns = "http://qt-project.org";
    readItem.nodeId = "s=Demo.Static.Scalar.Double";
    readItem.attribute = QtOpcUa.Constants.NodeAttribute.DisplayName;

    // Add the prepared item to the list of items to be read
    readItemList.push(readItem);

    // Add further items
    [...]

    if (!connection.readNodeAttributes(readItemList)) {
        // handle error
    }
    \endcode

    The result of the read request are provided by the signal
    \l readNodeAttributesFinished().

    \sa readNodeAttributesFinished(), ReadItem
*/
bool OpcUaConnection::readNodeAttributes(const QJSValue &value)
{
    if (!m_client || !m_connected) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << tr("Not connected to server.");
        return false;
    }
    if (!value.isArray()) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << tr("List of ReadItems it not an array.");
        return false;
    }

    QVector<QOpcUaReadItem> readItemList;

    for (int i = 0; i < value.property("length").toInt(); ++i){
        const auto &readItem = qjsvalue_cast<OpcUaReadItem>(value.property(i));
        if (readItem.nodeId().isEmpty()) {
            qCWarning(QT_OPCUA_PLUGINS_QML) << tr("Invalid ReadItem in list of items at index %1").arg(i);
            return false;
        }

        QString finalNode;
        bool ok;
        int index = readItem.namespaceIdentifier().toInt(&ok);
        if (ok) {
            QString identifier;
            UniversalNode::splitNodeIdAndNamespace(readItem.nodeId(), nullptr, &identifier);
            finalNode = UniversalNode::createNodeString(index, identifier);
        } else {
            finalNode = UniversalNode::resolveNamespaceToNode(readItem.nodeId(), readItem.namespaceIdentifier().toString(), m_client);
        }

        if (finalNode.isEmpty()) {
            qCWarning(QT_OPCUA_PLUGINS_QML) << tr("Failed to resolve node.");
            return false;
        }
        readItemList.push_back(QOpcUaReadItem(finalNode,
                                              readItem.attribute(),
                                              readItem.indexRange())
                               );
    }

    return m_client->readNodeAttributes(readItemList);
}

QStringList OpcUaConnection::supportedSecurityPolicies() const
{
    if (!m_client)
        return QStringList();
    return m_client->supportedSecurityPolicies();
}

QJSValue OpcUaConnection::supportedUserTokenTypes() const
{
    if (!m_client)
        return QJSValue();

    auto engine = qjsEngine(this);
    if (!engine)
        return QJSValue();

    const auto tokenTypes = m_client->supportedUserTokenTypes();
    auto returnValue = engine->newArray(tokenTypes.size());
    for (int i = 0; i < tokenTypes.size(); ++i)
        returnValue.setProperty(i, tokenTypes[i]);

    return returnValue;
}

void OpcUaConnection::handleReadNodeAttributesFinished(const QVector<QOpcUaReadResult> &results)
{
    QVariantList returnValue;

    for (const auto &result : results)
        returnValue.append(QVariant::fromValue(OpcUaReadResult(result, m_client)));

    emit readNodeAttributesFinished(QVariant::fromValue(returnValue));
}

QT_END_NAMESPACE

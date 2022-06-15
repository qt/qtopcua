// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/opcuaconnection_p.h>
#include <private/opcuareadresult_p.h>
#include <private/opcuawriteitem_p.h>
#include <private/opcuawriteresult_p.h>
#include <private/universalnode_p.h>

#include <QJSEngine>
#include <QLoggingCategory>
#include <QOpcUaProvider>
#include <QOpcUaReadItem>
#include <QOpcUaReadResult>
#include <QOpcUaWriteItem>

QT_BEGIN_NAMESPACE

/*!
    \qmltype Connection
    \inqmlmodule QtOpcUa
    \brief Connects to a server.
    \since QtOpcUa 5.12

    The main API uses backends to make connections. You have to set the backend before
    any connection attempt.

    \code
    import QtOpcUa as QtOpcUa

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
    \qmlproperty QOpcUaEndpointDescription Connection::currentEndpoint
    \since 5.13

    An endpoint description of the server to which the connection is connected to.
    When the connection is not established, an empty endpoint description is returned.
*/

/*!
    \qmlsignal Connection::readNodeAttributesFinished(readResults)
    \since 5.13

    Emitted when the read request, started using \l readNodeAttributes(), is finished.
    The \a readResults parameter is an array of \l ReadResult entries, containing the
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

/*!
    \qmlsignal Connection::writeNodeAttributesFinished(writeResults)
    \since 5.13

    Emitted when the write request started using \l writeNodeAttributes() is
    finished. The \a writeResults parameter is an array of \l WriteResult entries,
    containing the values requested from the server.

    \code
    for (var i = 0; i < writeResults.length; i++) {
        console.log(writeResults[i].nodeId);
        console.log(writeResults[i].namespaceName);
        console.log(writeResults[i].attribute);

        if (writeResults[i].status.isBad) {
            // value was not written
        }
    }
    \endcode

    \sa writeNodeAttributes(), WriteResult

*/

/*!
    \qmlproperty QOpcUaClient Connection::connection
    \since 5.13

    This property is used only to inject a connection from C++. In case of complex setup of
    a connection you can use C++ to handle all the details. After the connection is established
    it can be handed to QML using this property. Ownership of the client is transferred to QML.

    \code
    class MyClass : public QObject {
        Q_OBJECT
        Q_PROPERTY(QOpcUaClient* connection READ connection NOTIFY connectionChanged)

    public:
        MyClass (QObject* parent = nullptr);
        QOpcUaClient *connection() const;

    signals:
        void connectionChanged(QOpcUaClient *);
    \endcode

    Emitting the signal \c connectionChanged when the client setup is completed, the QML code below will
    use the connection.

    \code
    import QtOpcUa as QtOpcUa

    MyClass {
        id: myclass
    }

    QtOpcUa.Connection {
        connection: myclass.connection
     }
    \endcode

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
    removeConnection();
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

        removeConnection();
    }

    QOpcUaProvider provider;
    m_client = provider.createClient(name);
    if (m_client) {
        qCDebug(QT_OPCUA_PLUGINS_QML) << "Created plugin" << m_client->backend();
        setupConnection();
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

    Connects to the endpoint specified with \a endpointDescription.

    \sa EndpointDescription
*/

void OpcUaConnection::connectToEndpoint(const QOpcUaEndpointDescription &endpointDescription)
{
    if (!m_client)
        return;

    m_client->connectToEndpoint(endpointDescription);
}

/*!
    \qmlmethod Connection::disconnectFromEndpoint()

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

QOpcUaEndpointDescription OpcUaConnection::currentEndpoint() const
{
    if (!m_client || !m_connected)
        return QOpcUaEndpointDescription();

    return m_client->endpoint();
}

void OpcUaConnection::setAuthenticationInformation(const QOpcUaAuthenticationInformation &authenticationInformation)
{
    if (!m_client)
        return;
    m_client->setAuthenticationInformation(authenticationInformation);
}

void OpcUaConnection::setConnection(QOpcUaClient *client)
{
    if (!client)
        return;
    removeConnection();
    m_client = client;
    setupConnection();
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

    The \a valuesToBeRead parameter must be a JavaScript array of \l ReadItem
    entries.

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

    QList<QOpcUaReadItem> readItemList;

    for (int i = 0, end = value.property(QStringLiteral("length")).toInt(); i < end; ++i){
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

/*!
    \qmlmethod Connection::writeNodeAttributes(valuesToBeWritten)

    This function is used to write multiple values to a server in one go.
    Returns \c true if the write request was dispatched successfully.

    The \a valuesToBeWritten parameter must be a JavaScript array of
    \l WriteItem entries.

    \code
    // List of items to write
    var writeItemList = [];
    // Item to be added to the list of items to be written
    var writeItem;

    // Prepare an item to be written

    // Create a new write item and fill properties
    writeItem = QtOpcUa.WriteItem.create();
    writeItem.ns = "http://qt-project.org";
    writeItem.nodeId = "s=Demo.Static.Scalar.Double";
    writeItem.attribute = QtOpcUa.Constants.NodeAttribute.Value;
    writeItem.value = 32.1;
    writeItem.valueType = QtOpcUa.Constants.Double;

    // Add the prepared item to the list of items to be written
    writeItemList.push(writeItem);

    // Add further items
    [...]

    if (!connection.writeNodeAttributes(writeItemList)) {
        // handle error
    }
    \endcode

    The result of the write request are provided by the signal
    \l Connection::writeNodeAttributesFinished().

    \sa Connection::writeNodeAttributesFinished() WriteItem
*/
bool OpcUaConnection::writeNodeAttributes(const QJSValue &value)
{
    if (!m_client || !m_connected) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << tr("Not connected to server.");
        return false;
    }
    if (!value.isArray()) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << tr("List of WriteItems it not an array.");
        return false;
    }

    QList<QOpcUaWriteItem> writeItemList;

    for (int i = 0, end = value.property(QStringLiteral("length")).toInt(); i < end; ++i) {
        const auto &writeItem = qjsvalue_cast<OpcUaWriteItem>(value.property(i));
        if (writeItem.nodeId().isEmpty()) {
            qCWarning(QT_OPCUA_PLUGINS_QML) << tr("Invalid WriteItem in list of items at index %1").arg(i);
            return false;
        }

        QString finalNode;
        bool ok;
        int index = writeItem.namespaceIdentifier().toInt(&ok);
        if (ok) {
            QString identifier;
            UniversalNode::splitNodeIdAndNamespace(writeItem.nodeId(), nullptr, &identifier);
            finalNode = UniversalNode::createNodeString(index, identifier);
        } else {
            finalNode = UniversalNode::resolveNamespaceToNode(writeItem.nodeId(), writeItem.namespaceIdentifier().toString(), m_client);
        }

        if (finalNode.isEmpty()) {
            qCWarning(QT_OPCUA_PLUGINS_QML) << tr("Failed to resolve node.");
            return false;
        }

        auto tmp = QOpcUaWriteItem(finalNode,
                                   writeItem.attribute(),
                                   writeItem.value(),
                                   writeItem.valueType(),
                                   writeItem.indexRange());

        tmp.setSourceTimestamp(writeItem.sourceTimestamp());
        tmp.setServerTimestamp(writeItem.serverTimestamp());
        if (writeItem.hasStatusCode())
            tmp.setStatusCode(static_cast<QOpcUa::UaStatusCode>(writeItem.statusCode()));
        writeItemList.push_back(tmp);
    }

    return m_client->writeNodeAttributes(writeItemList);
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

QOpcUaClient *OpcUaConnection::connection() const
{
    return m_client;
}

void OpcUaConnection::handleReadNodeAttributesFinished(const QList<QOpcUaReadResult> &results)
{
    QVariantList returnValue;

    for (const auto &result : results)
        returnValue.append(QVariant::fromValue(OpcUaReadResult(result, m_client)));

    emit readNodeAttributesFinished(QVariant::fromValue(returnValue));
}

void OpcUaConnection::handleWriteNodeAttributesFinished(const QList<QOpcUaWriteResult> &results)
{
    QVariantList returnValue;

    for (const auto &result : results)
        returnValue.append(QVariant::fromValue(OpcUaWriteResult(result, m_client)));

    emit writeNodeAttributesFinished(QVariant::fromValue(returnValue));
}

void OpcUaConnection::removeConnection()
{
    if (m_client) {
        m_client->disconnect(this);
        m_client->disconnectFromEndpoint();
        if (!m_client->parent()) {
            m_client->deleteLater();
        }
        m_client = nullptr;
    }
}

void OpcUaConnection::setupConnection()
{
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
    connect(m_client, &QOpcUaClient::writeNodeAttributesFinished, this, &OpcUaConnection::handleWriteNodeAttributesFinished);
    m_connected = (!m_client->namespaceArray().isEmpty() && m_client->state() == QOpcUaClient::Connected);
    if (m_connected)
        emit connectedChanged();
}

QT_END_NAMESPACE

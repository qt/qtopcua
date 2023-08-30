// Copyright (C) 2019 The Qt Company Ltd.
// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuaclient.h"
#include "qopcuaconnectionsettings.h"
#include "qopcuaexpandednodeid.h"
#include "qopcuaqualifiedname.h"

#include <private/qopcuaclient_p.h>

#include <QtCore/qloggingcategory.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA)

/*!
    \class QOpcUaClient
    \inmodule QtOpcUa

    \brief QOpcUaClient allows interaction with an OPC UA server.

    \section1 QOpcUaClient

    QOpcUaClient implements basic client capabilities to communicate with
    OPC UA enabled devices and applications. This includes querying a discovery server
    for known servers, requesting a list of endpoints from a server, connecting and
    disconnecting.

    After successfully connecting to a server, QOpcUaClient allows getting \l QOpcUaNode
    objects which enable further interaction with nodes on the OPC UA server.
    For operations that concern multiple nodes, QOpcUaClient offers an API which supports
    reading multiple attributes of multiple nodes in a single request to the server.

    QOpcUaClient also keeps a local copy of the server's namespace array which is created after
    a successful connect. This information can be queried or updated while the connection lasts.
    The copy of the namespace array is also used for the resolution of expanded node ids and the
    creation of qualified names from a namespace URI.

    \section1 Addressing Nodes

    For an introduction to nodes and node ids, see \l QOpcUaNode.

    \section1 Usage
    Create a \l QOpcUaClient using \l QOpcUaProvider, request a list of endpoints from the server
    using \l requestEndpoints and call \l connectToEndpoint() to connect to one of the available endpoints.
    After the connection is established, a \l QOpcUaNode object for the root node is requested.
    \code
    QOpcUaProvider provider;
    if (provider.availableBackends().isEmpty())
        return;
    QOpcUaClient *client = provider.createClient(provider.availableBackends()[0]);
    if (!client)
        return;
    // Connect to the stateChanged signal. Compatible slots of QObjects can be used instead of a lambda.
    QObject::connect(client, &QOpcUaClient::stateChanged, [client](QOpcUaClient::ClientState state) {
        qDebug() << "Client state changed:" << state;
        if (state == QOpcUaClient::ClientState::Connected) {
            QOpcUaNode *node = client->node("ns=0;i=84");
            if (node)
                qDebug() << "A node object has been created";
        }
    });

    QObject::connect(client, &QOpcUaClient::endpointsRequestFinished,
                     [client](QList<QOpcUaEndpointDescription> endpoints) {
        qDebug() << "Endpoints returned:" << endpoints.count();
        if (endpoints.size())
            client->connectToEndpoint(endpoints.first()); // Connect to the first endpoint in the list
    });

    client->requestEndpoints(QUrl("opc.tcp://127.0.0.1:4840")); // Request a list of endpoints from the server
    \endcode
*/

/*!
    \enum QOpcUaClient::ClientState

    This enum type specifies the connection state of the client.

    \value Disconnected
           The client is not connected to a server.
    \value Connecting
           The client is currently connecting to a server.
    \value Connected
           The client is connected to a server.
    \value Closing
           The client has been connected and requests a disconnect from the server.
*/

/*!
    \enum QOpcUaClient::ClientError

    This enum type specifies the current error state of the client.

    \value NoError
           No error occurred.
    \value InvalidUrl
           The url to connect to has been wrongly specified or a connection to this url failed.
    \value AccessDenied
           An attempt to connect to a server using username/password failed due to wrong credentials.
    \value ConnectionError
           An error occurred with the connection.
    \value UnknownError
           An unknown error occurred.
    \value UnsupportedAuthenticationInformation
           The given type or data of authentication information is not supported.
*/

/*!
    \property QOpcUaClient::error
    \brief Specifies the current error state of the client.
*/

/*!
    \property QOpcUaClient::state
    \brief Specifies the current connection state of the client.
*/

/*!
    \fn QOpcUaClient::connected()

    This signal is emitted when a connection has been established.
*/

/*!
    \fn QOpcUaClient::disconnected()

    This signal is emitted when a connection has been closed following to a close request.
*/

/*!
    \fn QOpcUaClient::connectError(QOpcUaErrorState *errorState)
    \since QtOpcUa 5.13

    This signal is emitted when an error happened during connection establishment.
    The parameter \a errorState contains information about the error.

    In case of client side errors, these can be ignored by calling
    \l QOpcUaErrorState::setIgnoreError on the object.

    During execution of a slot connected to this signal the backend is stopped and
    waits for all slots to return. This allows to pop up a user dialog to ask the
    enduser for example if to trust an unknown certificate before the backend continues.
 */

/*!
    \fn QOpcUaClient::passwordForPrivateKeyRequired(QString keyFilePath, QString *password, bool previousTryWasInvalid)
    \since QtOpcUa 5.13

    This signal is emitted when a password for an encrypted private key is required.
    The parameter \a keyFilePath contains the file path to key which is used.
    The parameter \a previousTryWasInvalid is true if a previous try to decrypt the key failed (aka invalid password).
    The parameter \a password points to a QString that has to be filled with the actual password for the key.
    In case the previous try failed it contains the previously used password.

    During execution of a slot connected to this signal the backend is stopped and
    waits for all slots to return. This allows to pop up a user dialog to ask the
    enduser for the password.
 */

/*!
    \fn void QOpcUaClient::namespaceArrayUpdated(QStringList namespaces)

    This signal is emitted after an updateNamespaceArray operation has finished.
    \a namespaces contains the content of the server's namespace table. The index
    of an entry in \a namespaces corresponds to the namespace index used in the node id.

    If the namespace array content stays the same after the update this signal is emitted nevertheless.

    \sa namespaceArrayChanged() updateNamespaceArray()
*/

/*!
    \fn void QOpcUaClient::namespaceArrayChanged(QStringList namespaces)

    This signal is emitted after the namespace array has changed.
    \a namespaces contains the content of the server's namespace table. The index
    of an entry in \a namespaces corresponds to the namespace index used in the node id.

    \sa namespaceArrayUpdated() updateNamespaceArray()
*/

/*!
    \fn void QOpcUaClient::endpointsRequestFinished(QList<QOpcUaEndpointDescription> endpoints, QOpcUa::UaStatusCode statusCode, QUrl requestUrl)

    This signal is emitted after a \l requestEndpoints() operation has finished.
    \a statusCode contains the result of the operation. If the result is \l {QOpcUa::UaStatusCode} {Good},
    \a endpoints contains the descriptions of all endpoints that are available on the server.
    \a requestUrl contains the URL that was used in the \l requestEndpoints() call.
*/

/*!
    \fn void QOpcUaClient::findServersFinished(QList<QOpcUaApplicationDescription> servers, QOpcUa::UaStatusCode statusCode, QUrl requestUrl);

    This signal is emitted after a \l findServers() operation has finished.
    \a statusCode contains the result of the operation. If the result is \l {QOpcUa::UaStatusCode} {Good},
    \a servers contains the application descriptions of all servers known to the queried server that matched the filter criteria.
    \a requestUrl contains the URL that was used in the \l findServers() call.
*/

/*!
    \fn void QOpcUaClient::readNodeAttributesFinished(QList<QOpcUaReadResult> results, QOpcUa::UaStatusCode serviceResult)

    This signal is emitted after a \l readNodeAttributes() operation has finished.

    The elements in \a results have the same order as the elements in the request. For each requested element,
    there is a value together with timestamps and the status code in \a results.
    \a serviceResult contains the status code from the OPC UA Read service.

    \sa readNodeAttributes() QOpcUaReadResult QOpcUaReadItem
*/

/*!
    \fn void QOpcUaClient::writeNodeAttributesFinished(QList<QOpcUaWriteResult> results, QOpcUa::UaStatusCode serviceResult)

    This signal is emitted after a \l writeNodeAttributes() operation has finished.

    The elements in \a results have the same order as the elements in the write request.
    They contain the value, timestamps and status code received from the server as well as the node id,
    attribute and index range from the write item. This facilitates matching the result with the request.

    \a serviceResult is the status code from the the OPC UA Write service. If \a serviceResult is not
    \l {QOpcUa::UaStatusCode} {Good}, the entries in \a results also have an invalid status code and must
    not be used.

    \sa writeNodeAttributes() QOpcUaWriteResult
*/

/*!
    \fn void QOpcUaClient::addNodeFinished(QOpcUaExpandedNodeId requestedNodeId, QString assignedNodeId, QOpcUa::UaStatusCode statusCode)

    This signal is emitted after an \l addNode() operation has finished.
    \a requestedNodeId is the requested node id from the \l addNode() call, \a assignedNodeId is the node id the server has assigned to the new node.
    \a statusCode contains the result of the operation. If the result is \l {QOpcUa::UaStatusCode} {Bad}, \a assignedNodeId is empty and no node
    has been added to the server's address space.
*/

/*!
    \fn void QOpcUaClient::deleteNodeFinished(QString nodeId, QOpcUa::UaStatusCode statusCode)

    This signal is emitted after a \l deleteNode() operation has finished.
    \a nodeId is the node id from the \l deleteNode() call.
    \a statusCode contains the result of the operation.
*/

/*!
    \fn void QOpcUaClient::addReferenceFinished(QString sourceNodeId, QString referenceTypeId, QOpcUaExpandedNodeId targetNodeId, bool isForwardReference, QOpcUa::UaStatusCode statusCode)

    This signal is emitted after an \l addReference() operation has finished.
    \a sourceNodeId, \a referenceTypeId, \a targetNodeId and \a isForwardReference are the values from the \l addReference() call.
    \a statusCode contains the result of the operation.
*/

/*!
    \fn void QOpcUaClient::deleteReferenceFinished(QString sourceNodeId, QString referenceTypeId, QOpcUaExpandedNodeId targetNodeId, bool isForwardReference, QOpcUa::UaStatusCode statusCode)

    This signal is emitted after a \l deleteReference() operation has finished.
    \a sourceNodeId, \a referenceTypeId, \a targetNodeId and \a isForwardReference are the values from the \l deleteReference() call.
    \a statusCode contains the result of the operation.
*/

/*!
    \internal QOpcUaClientImpl is an opaque type (as seen from the public API).
    This prevents users of the public API to use this constructor (even though
    it is public).
*/
QOpcUaClient::QOpcUaClient(QOpcUaClientImpl *impl, QObject *parent)
    : QObject(*(new QOpcUaClientPrivate(impl)), parent)
{
    impl->m_client = this;
}

/*!
    Destroys the \l QOpcUaClient instance.
*/
QOpcUaClient::~QOpcUaClient()
{
}

/*!
    Sets the application identity for this \l QOpcUaClient instance to \a identity.
    \since QtOpcUa 5.13
*/
void QOpcUaClient::setApplicationIdentity(const QOpcUaApplicationIdentity &identity)
{
    Q_D(QOpcUaClient);
    d->setApplicationIdentity(identity);
}

/*!
    Returns the application identity of this \l QOpcUaClient instance.
    \since QtOpcUa 5.13
*/
QOpcUaApplicationIdentity QOpcUaClient::applicationIdentity() const
{
    Q_D(const QOpcUaClient);
    return d->applicationIdentity();
}

/*!
    Sets the application PKI configuration for this \l QOpcUaClient instance to \a config.
    \since QtOpcUa 5.13
*/
void QOpcUaClient::setPkiConfiguration(const QOpcUaPkiConfiguration &config)
{
    Q_D(QOpcUaClient);
    d->setPkiConfiguration(config);
}

/*!
    Returns the application's PKI configuration of this \l QOpcUaClient instance.
    \since QtOpcUa 5.13
*/
QOpcUaPkiConfiguration QOpcUaClient::pkiConfiguration() const
{
    Q_D(const QOpcUaClient);
    return d->pkiConfiguration();
}

/*!
    Connects to the OPC UA endpoint given in \a endpoint.
    \since QtOpcUa 5.13

    \code
    QEndpointDescription endpointDescription;
    ...
    client->connectToEndpoint(endpointDescription);
    \endcode

    A list of available endpoints is usually obtained by calling \l QOpcUaClient::requestEndpoints().

    If the endpoint requires username authentication, at least a user name must be set in \l QOpcUaAuthenticationInformation.
    Calling this function before setting an authentication information will use the anonymous authentication.

    \code
    QOpcUaAuthenticationInformation authInfo;
    authInfo.setUsernameAuthentication("user", "password");

    client->setAuthenticationInformation(authInfo);
    \endcode

    \sa connected(), stateChanged(), setAuthenticationInformation(), QOpcUaEndpointDescription
*/
void QOpcUaClient::connectToEndpoint(const QOpcUaEndpointDescription &endpoint)
{
    Q_D(QOpcUaClient);
    d->connectToEndpoint(endpoint);
}

/*!
    Disconnects from the server.
    \sa disconnected(), connectToEndpoint()
*/
void QOpcUaClient::disconnectFromEndpoint()
{
    Q_D(QOpcUaClient);
    d->disconnectFromEndpoint();
}

/*!
    Returns the description of the endpoint the client is currently connected to
    or was last connected to.
*/
QOpcUaEndpointDescription QOpcUaClient::endpoint() const
{
    Q_D(const QOpcUaClient);
    return d->m_endpoint;
}

QOpcUaClient::ClientState QOpcUaClient::state() const
{
    Q_D(const QOpcUaClient);
    return d->m_state;
}

/*!
    Returns the current error state of the client.
*/
QOpcUaClient::ClientError QOpcUaClient::error() const
{
    Q_D(const QOpcUaClient);
    return d->m_error;
}

/*!
    Returns a \l QOpcUaNode object associated with the OPC UA node identified
    by \a nodeId. The caller becomes owner of the node object.

    If the client is not connected, \c nullptr is returned. The backends may also
    return \c nullptr for other error cases (for example for a malformed node id).
*/
QOpcUaNode *QOpcUaClient::node(const QString &nodeId)
{
    if (state() != QOpcUaClient::Connected)
       return nullptr;

    Q_D(QOpcUaClient);
    return d->m_impl->node(nodeId);
}

/*!
    Returns a \l QOpcUaNode object associated with the OPC UA node identified
    by \a expandedNodeId. The caller becomes owner of the node object.

    If the node is not on the currently connected server, the namespace can't be resolved,
    the node id is malformed or the client is not connected, \c nullptr is returned.

    \sa updateNamespaceArray()
*/
QOpcUaNode *QOpcUaClient::node(const QOpcUaExpandedNodeId &expandedNodeId)
{
    if (expandedNodeId.serverIndex()) {
        qCWarning(QT_OPCUA) << "Can't create a QOpcuaNode for a node on a different server.";
        return nullptr;
    }

    const QString nodeId = resolveExpandedNodeId(expandedNodeId);

    if (!nodeId.isEmpty())
        return node(nodeId);
    else
        return nullptr;
}

/*!
    Requests an update of the namespace array from the server.
    Returns \c true if the operation has been successfully dispatched.

    The \l namespaceArrayUpdated() signal is emitted after the operation is finished.

    \sa namespaceArray() namespaceArrayUpdated()
*/
bool QOpcUaClient::updateNamespaceArray()
{
    if (state() != QOpcUaClient::Connected)
       return false;

    Q_D(QOpcUaClient);
    return d->updateNamespaceArray();
}

/*!
    Returns the cached value of the namespace array.

    The value is only valid after the \l namespaceArrayUpdated() signal has been emitted.

    \sa updateNamespaceArray() namespaceArrayUpdated()
*/
QStringList QOpcUaClient::namespaceArray() const
{
    Q_D(const QOpcUaClient);
    return d->namespaceArray();
}

/*!
    Attempts to resolve \a expandedNodeId to a node id string with numeric namespace index.
    Returns the node id string if the conversion was successful.

    An empty string is returned if the namespace index can't be resolved or if the identifier part
    of the expanded node id is malformed. \a ok will be set to \c true if the conversion has been successful.
    If the expanded node id could not be resolved, \a ok will be set to \c false.
*/
QString QOpcUaClient::resolveExpandedNodeId(const QOpcUaExpandedNodeId &expandedNodeId, bool *ok) const
{
    if (expandedNodeId.serverIndex() && !expandedNodeId.namespaceUri().isEmpty()) {
        qCWarning(QT_OPCUA) << "Can't resolve a namespace index on a different server.";
        if (ok)
            *ok = false;
        return QString();
    }

    if (expandedNodeId.namespaceUri().isEmpty()) {
        if (ok)
            *ok = true;
        return expandedNodeId.nodeId();
    } else {
        if (!namespaceArray().size()) {
            qCWarning(QT_OPCUA) << "Namespaces table missing, unable to resolve namespace URI.";
            if (ok)
                *ok = false;
            return QString();
        }

        int index = namespaceArray().indexOf(expandedNodeId.namespaceUri());

        if (index < 0) {
            qCWarning(QT_OPCUA) << "Failed to resolve namespace" << expandedNodeId.namespaceUri();
            if (ok)
                *ok = false;
            return QString();
        }

        QStringList splitId = expandedNodeId.nodeId().split(QLatin1String(";"));
        if (splitId.size() != 2) {
            qCWarning(QT_OPCUA) << "Failed to split node id" << expandedNodeId.nodeId();
            if (ok)
                *ok = false;
            return QString();
        }

        if (ok)
            *ok = true;
        return QStringLiteral("ns=%1;").arg(index).append(splitId.at(1));
    }
}

/*!
    Attempts to create a qualified name from \a namespaceUri and the name string \a name.
    Returns the resulting qualified name. An empty qualified name is returned if
    \a namespaceUri can't be resolved.

    \a ok will be set to \c true if the namespace URI resolution has been successful.
    If the namespace URI could not be resolved, \a ok will be set to \c false.
*/
QOpcUaQualifiedName QOpcUaClient::qualifiedNameFromNamespaceUri(const QString &namespaceUri, const QString &name, bool *ok) const
{
    if (namespaceArray().isEmpty()) {
        qCWarning(QT_OPCUA) << "Namespaces table missing, unable to resolve namespace URI.";
        if (ok)
            *ok = false;
        return QOpcUaQualifiedName();
    }

    int index = namespaceArray().indexOf(namespaceUri);

    if (index < 0) {
        qCWarning(QT_OPCUA) << "Failed to resolve namespace" << namespaceUri;
        if (ok)
            *ok = false;
        return QOpcUaQualifiedName();
    }

    if (ok)
        *ok = true;

    return QOpcUaQualifiedName(index, name);
};

/*!
    Adds the node described by \a nodeToAdd on the server.

    Returns \c true if the asynchronous call has been successfully dispatched.

    The success of the operation is returned in the \l addNodeFinished() signal.

    The following example code adds new a Variable node on the server:

    \code
    QOpcUaNodeCreationAttributes attributes;
    attributes.setDisplayName(QOpcUaLocalizedText("en", "My new Variable node"));
    attributes.setDescription(QOpcUaLocalizedText("en", "A node which has been added at runtime"));
    attributes.setValue(23.0, QOpcUa::Types::Double);
    attributes.setDataTypeId(QOpcUa::ns0ID(QOpcUa::NodeIds::Namespace0::Double));
    attributes.setValueRank(-2); // Scalar or array
    attributes.setAccessLevel(QOpcUa::AccessLevelBit::CurrentRead);
    attributes.setUserAccessLevel(QOpcUa::AccessLevelBit::CurrentRead);

    QOpcUaAddNodeItem item;
    item.setParentNodeId(QOpcUaExpandedNodeId("ns=3;s=TestFolder"));
    item.setReferenceTypeId(QOpcUa::nodeIdFromReferenceType(QOpcUa::ReferenceTypeId::Organizes));
    item.setRequestedNewNodeId(QOpcUaExpandedNodeId("ns=3;s=MyNewVariableNode"));
    item.setBrowseName(QOpcUaQualifiedName(3, "MyNewVariableNode"));
    item.setNodeClass(QOpcUa::NodeClass::Variable);
    item.setNodeAttributes(attributes);

    m_client->addNode(item);
    \endcode

    \sa deleteNode() addNodeFinished() QOpcUaAddNodeItem
*/
bool QOpcUaClient::addNode(const QOpcUaAddNodeItem &nodeToAdd)
{
    if (state() != QOpcUaClient::Connected)
       return false;

    Q_D(QOpcUaClient);
    return d->m_impl->addNode(nodeToAdd);
}

/*!
    Deletes the node with node id \a nodeId from the server.
    If \a deleteTargetReferences is \c false, only the references with source node \a nodeId are deleted.
    If \a deleteTargetReferences is \c true, references with \a nodeId as target are deleted too.

    Returns \c true if the asynchronous call has been successfully dispatched.

    The success of the operation is returned in the \l deleteNodeFinished() signal.

    The following example code deletes a node and all references to it from the server:

    \code
    m_client->deleteNode(QOpcUaExpandedNodeId("ns=3;s=MyNewVariableNode"), true);
    \endcode

    \sa addNode() deleteNodeFinished()
*/
bool QOpcUaClient::deleteNode(const QString &nodeId, bool deleteTargetReferences)
{
    if (state() != QOpcUaClient::Connected)
       return false;

    Q_D(QOpcUaClient);
    return d->m_impl->deleteNode(nodeId, deleteTargetReferences);
}

/*!
    Adds the reference described by \a referenceToAdd to the server.

    Returns \c true if the asynchronous call has been successfully dispatched.

    The success of the operation is returned in the \l addReferenceFinished() signal.

    The following example code adds a reference to a node to the "Objects" folder:

    \code
    QOpcUaAddReferenceItem item;
    item.setSourceNodeId(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::ObjectsFolder));
    item.setReferenceTypeId(QOpcUa::nodeIdFromInteger(0, static_cast<quint32>(QOpcUa::ReferenceTypeId::Organizes)));
    item.setIsForwardReference(true);
    item.setTargetNodeId(QOpcUaExpandedNodeId("ns=3;s=MyNewVariableNode"));
    item.setTargetNodeClass(QOpcUa::NodeClass::Variable);

    m_client->addReference(item);
    \endcode

    \sa deleteReference() addReferenceFinished() QOpcUaAddReferenceItem
*/
bool QOpcUaClient::addReference(const QOpcUaAddReferenceItem &referenceToAdd)
{
    if (state() != QOpcUaClient::Connected)
       return false;

    Q_D(QOpcUaClient);
    return d->m_impl->addReference(referenceToAdd);
}

/*!
    Deletes the reference described by \a referenceToDelete from the server.

    Returns \c true if the asynchronous call has been successfully dispatched.

    The success of the operation is returned in the \l deleteReferenceFinished() signal.

    The following example code deletes a reference to a node from the "Objects" folder:

    \code
    QOpcUaDeleteReferenceItem item;
    item.setSourceNodeId(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::ObjectsFolder));
    item.setReferenceTypeId(QOpcUa::nodeIdFromInteger(0, static_cast<quint32>(QOpcUa::ReferenceTypeId::Organizes)));
    item.setIsForwardReference(true);
    item.setTargetNodeId(QOpcUaExpandedNodeId("ns=3;s=MyNewVariableNode"));
    item.setDeleteBidirectional(true);

    m_client->deleteReference(item);
    \endcode

    \sa addReference() deleteReferenceFinished() QOpcUaDeleteReferenceItem
*/
bool QOpcUaClient::deleteReference(const QOpcUaDeleteReferenceItem &referenceToDelete)
{
    if (state() != QOpcUaClient::Connected)
       return false;

    Q_D(QOpcUaClient);
    return d->m_impl->deleteReference(referenceToDelete);
}

/*!
    Starts an asynchronous \c GetEndpoints request to read a list of available endpoints
    from the server at \a url.
    Returns \c true if the asynchronous call has been successfully dispatched.

    The endpoint information is returned in the \l endpointsRequestFinished() signal.
*/
bool QOpcUaClient::requestEndpoints(const QUrl &url)
{
    Q_D(QOpcUaClient);
    return d->m_impl->requestEndpoints(url);
}

/*!
    Starts an asynchronous FindServers request to read a list of known servers from a server or
    discovery server at \a url.
    Returns \c true if the asynchronous call has been successfully dispatched.

    \a localeIds can be used to select the language of the application names returned by the request.
    The format is specified in OPC-UA part 3, 8.4, for example "en" for English, or "de-DE" for
    German (Germany). If more than one locale ID is specified, the server uses the first match. If there
    is no match or \a localeIds is empty, a default locale is chosen by the server.

    \a serverUris may be used to restrict the results to servers with a matching applicationUri in their
    application description. For example, finding the current URL of the server with the applicationUri
    "MyPLC", the following call can be used:

    \code
        client->findServers(discoveryServerUrl, QStringList(), QStringList({"MyPLC"}));
    \endcode

    The results are returned in the \l findServersFinished() signal.
*/
bool QOpcUaClient::findServers(const QUrl &url, const QStringList &localeIds, const QStringList &serverUris)
{
    Q_D(QOpcUaClient);
    return d->m_impl->findServers(url, localeIds, serverUris);
}

/*!
    Starts a read of multiple attributes on different nodes.
    The node id, the attribute and an index range can be specified for every entry in \a nodesToRead.

    Returns true if the asynchronous request has been successfully dispatched.
    The results are returned in the \l readNodeAttributesFinished() signal.

    This read function offers an alternative way to read attributes of nodes which can be used
    for scenarios where the values of a large number of node attributes on different nodes must be read
    without requiring the other features of the \l QOpcUaNode based API like monitoring for value changes.
    All read items in the request are sent to the server in a single request and are answered in a single
    response which generates a single \l readNodeAttributesFinished() signal. This reduces the network overhead and
    the number of signal slot connections if many different nodes are involved.

    In the following example, the display name attribute and the two index ranges "0:2" and "5:7" of the value
    attribute of the same node and the entire value attribute of a second node are read using a single service call:
    \code
    QList<QOpcUaReadItem> request;
    request.push_back(QOpcUaReadItem("ns=1;s=MyArrayNode",
                                     QOpcUa::NodeAttribute::DisplayName));
    request.push_back(QOpcUaReadItem("ns=1;s=MyArrayNode",
                                     QOpcUa::NodeAttribute::Value, "0:2"));
    request.push_back(QOpcUaReadItem("ns=1;s=MyArrayNode",
                                     QOpcUa::NodeAttribute::Value, "5:7"));
    request.push_back(QOpcUaReadItem("ns=1;s=MyScalarNode));
    m_client->readNodeAttributes(request);
    \endcode

    \sa QOpcUaReadItem readNodeAttributesFinished()
*/
bool QOpcUaClient::readNodeAttributes(const QList<QOpcUaReadItem> &nodesToRead)
{
    if (state() != QOpcUaClient::Connected)
       return false;

    Q_D(QOpcUaClient);
    return d->m_impl->readNodeAttributes(nodesToRead);
}

/*!
    Starts a write for multiple attributes on different nodes.
    The node id, the attribute, the value, the value type and an index range can be specified
    for every entry in \a nodesToWrite.

    Returns \c true if the asynchronous request has been successfully dispatched.
    The results are returned in the \l writeNodeAttributesFinished() signal.

    This write function offers an alternative way to write attributes of nodes which can be used
    for scenarios where the values of a large number of node attributes on different nodes must be written
    without requiring the other features of the \l QOpcUaNode based API like monitoring for value changes.
    All write items in the request are sent to the server in a single request and are answered in a single
    response which generates a single \l writeNodeAttributesFinished() signal. This reduces the network overhead and
    the number of signal slot connections if many different nodes are involved.

    In the following example, the Values attributes of two different nodes are written in one call.
    The second node has an array value of which only the first two elements are overwritten:

    \code
    QList<QOpcUaWriteItem> request;

    request.append(QOpcUaWriteItem("ns=2;s=Demo.Static.Scalar.Double", QOpcUa::NodeAttribute::Value,
                                      23.0, QOpcUa::Types::Double));
    request.append(QOpcUaWriteItem("ns=2;s=Demo.Static.Arrays.UInt32", QOpcUa::NodeAttribute::Value,
                                      QVariantList({0, 1, 2}), QOpcUa::Types::UInt32, "0:2"));

    m_client->writeNodeAttributes(request);
    \endcode

    \sa QOpcUaWriteItem writeNodeAttributesFinished()
*/
bool QOpcUaClient::writeNodeAttributes(const QList<QOpcUaWriteItem> &nodesToWrite)
{
    if (state() != QOpcUaClient::Connected)
       return false;

    Q_D(QOpcUaClient);
    return d->m_impl->writeNodeAttributes(nodesToWrite);
}

/*!
    Returns the name of the backend used by this instance of QOpcUaClient,
    e.g. "open62541".
*/
QString QOpcUaClient::backend() const
{
    Q_D(const QOpcUaClient);
    return d->m_impl->backend();
}

/*!
    Enables automatic update of the namespace table.

    Enabling this will keep the local copy of the namespace table updated automatically.
    \l namespaceArrayUpdated will be emitted when the array changed.
    \a isEnabled determines if autoupdate is being enabled or disabled.

    A subscription will be made on the node on the server to keep track of changes.
    In case a server does not support subscriptions this will not work and
    \l isNamespaceAutoupdateEnabled returns \c false.

    \sa namespaceArray() namespaceArrayUpdated()
*/
void QOpcUaClient::setNamespaceAutoupdate(bool isEnabled)
{
    Q_D(QOpcUaClient);
    d->m_enableNamespaceArrayAutoupdate = isEnabled;
    d->setupNamespaceArrayMonitoring();
}

/*!
    Returns whether autoupdate of the namespace array is enabled.
*/
bool QOpcUaClient::isNamespaceAutoupdateEnabled() const
{
    Q_D(const QOpcUaClient);
    return d->m_enableNamespaceArrayAutoupdate;
}

/*!
    Sets the interval for the namespace table subscription.

    The subscription may be revised by the server.

    \a interval determines the interval to check for changes in milliseconds. The default is once per second.

    \sa QOpcUaClient::setNamespaceAutoupdate(bool isEnabled)
*/
void QOpcUaClient::setNamespaceAutoupdateInterval(int interval)
{
    Q_D(QOpcUaClient);
    d->m_namespaceArrayUpdateInterval = interval;
    d->setupNamespaceArrayMonitoring();
}

/*!
    Returns the current revised update interval of the namespace array.

    \sa setNamespaceAutoupdateInterval(int interval)
*/
int QOpcUaClient::namespaceAutoupdateInterval() const
{
    Q_D(const QOpcUaClient);
    return d->m_namespaceArrayUpdateInterval;
}

/*!
    Sets the authentication information of this client to \a authenticationInformation.

    \sa connectToEndpoint()
*/
void QOpcUaClient::setAuthenticationInformation(const QOpcUaAuthenticationInformation &authenticationInformation)
{
    Q_D(QOpcUaClient);
    d->m_authenticationInformation = authenticationInformation;
}

/*!
    Returns the current authentication information.
*/
const QOpcUaAuthenticationInformation &QOpcUaClient::authenticationInformation() const
{
    Q_D(const QOpcUaClient);
    return d->m_authenticationInformation;
}

/*!
    \since 6.6

    Sets the connection settings for this client to \a connectionSettings.

    Example:
    \code
    QOpcUaConnectionSettings settings;
    // Ask the server to give localized texts in german with french as fallback
    settings.setSessionLocaleIds({ "de", "fr" });
    // We need to call some long running methods, increase the request timeout
    settings.setRequestTimeout(std::chrono::minutes(2));
    opcuaClient->setConnectionSettings(settings);
    \endcode

    The values from \a connectionSettings are applied to any new connections after this point.

    \sa connectionSettings()
 */
void QOpcUaClient::setConnectionSettings(const QOpcUaConnectionSettings &connectionSettings)
{
    Q_D(QOpcUaClient);
    d->m_connectionSettings = connectionSettings;
}

/*!
    \since 6.6

    Returns the connection settings for this client.

    \sa setConnectionSettings()
 */
QOpcUaConnectionSettings QOpcUaClient::connectionSettings() const
{
    Q_D(const QOpcUaClient);
    return d->m_connectionSettings;
}

/*!
    \since QtOpcUa 5.14

    Returns the security policies supported by the used backend.

    This function is currently available as a Technology Preview, and therefore the API
    and functionality provided by the function may be subject to change at any time without
    prior notice.
*/
QStringList QOpcUaClient::supportedSecurityPolicies() const
{
    Q_D(const QOpcUaClient);
    return d->m_impl->supportedSecurityPolicies();
}

/*!
    \since QtOpcUa 5.14

    Returns the user token types supported by the used backend.

    This function is currently available as a Technology Preview, and therefore the API
    and functionality provided by the function may be subject to change at any time without
    prior notice.

    \sa QOpcUaUserTokenPolicy::TokenType
*/
QList<QOpcUaUserTokenPolicy::TokenType> QOpcUaClient::supportedUserTokenTypes() const
{
    Q_D(const QOpcUaClient);
    return d->m_impl->supportedUserTokenTypes();
}

/*!
    \since 6.3

    Starts a read raw history \a request for one or multiple nodes. This is the Qt OPC UA representation for the OPC UA
    ReadHistory service for reading raw historical data defined in
    \l {https://reference.opcfoundation.org/v104/Core/docs/Part4/5.10.3/} {OPC-UA part 4, 5.10.3}.

    The start timestamp, end timestamp, number of values per node, returnBounds and nodes to read
    can be specified in a \l QOpcUaHistoryReadRawRequest.

    Returns a \l QOpcUaHistoryReadResponse which contains the state of the request if the asynchronous
    request has been successfully dispatched. The results are returned in the
    \l QOpcUaHistoryReadResponse::readHistoryDataFinished(const QList<QOpcUaHistoryData> &results, QOpcUa::UaStatusCode serviceResult)
    signal.

    In the following example, the historic data from the last two days of two nodes are requested and printed.
    The result is limited to ten values per node.

    \code
    QOpcUaHistoryReadRawRequest request(
                { QOpcUaReadItem("ns=1;s=myValue1"), QOpcUaReadItem("ns=1;s=myValue2") },
                QDateTime::currentDateTime(),
                QDateTime::currentDateTime().addDays(-2),
                10,
                true);

    QOpcUaHistoryReadResponse *response = m_client->readHistoryData(request);
    if (response) {
       QObject::connect(response, &QOpcUaHistoryReadResponse::readHistoryDataFinished,
                        [] (QList<QOpcUaHistoryData> results, QOpcUa::UaStatusCode serviceResult) {
                            if (serviceResult != QOpcUa::UaStatusCode::Good) {
                                qWarning() << "Fetching historical data failed with:" << serviceResult;
                            } else {
                                for (const auto& result : results) {
                                    qInfo() << "NodeId:" << result.nodeId();
                                    for (const auto &dataValue : result.result())
                                        qInfo() << "Value:" << dataValue.value();
                                }
                            }
                        });
    }
    \endcode
*/
QOpcUaHistoryReadResponse *QOpcUaClient::readHistoryData(const QOpcUaHistoryReadRawRequest &request)
{
    Q_D(const QOpcUaClient);
    return d->m_impl->readHistoryData(request);
}

QT_END_NAMESPACE

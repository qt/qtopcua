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

#include "qopcuaclient.h"
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
    For operations that concern multiple nodes, QOpcUaClient offers a batch API which supports
    reading multiple attributes of multiple nodes in a single request to the server.

    QOpcUaClient also keeps a local copy of the server's namespace array which is created after
    a successful connect. This information can be queried or updated while the connection lasts.
    The copy of the namespace array is also used for the resolution of expanded node ids and the
    creation of qualified names from a namespace URI.

    \section1 Addressing Nodes

    For an introduction to nodes and node ids, see \l QOpcUaNode.

    \section1 Usage
    Create a \l QOpcUaClient using \l QOpcUaProvider and call \l connectToEndpoint() to connect to a server.
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
    client->connectToEndpoint(QUrl("opc.tcp://127.0.0.1:4840")); // Connect the client to the server
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
    \fn void QOpcUaClient::endpointsRequestFinished(QVector<QOpcUa::QEndpointDescription> endpoints, QOpcUa::UaStatusCode statusCode)

    This signal is emitted after a \l requestEndpoints() operation has finished.
    \a statusCode contains the result of the operation. If the result is \l {QOpcUa::UaStatusCode} {Good},
    \a endpoints contains the descriptions of all endpoints that are available on the server.
*/

/*!
    \fn void QOpcUaClient::findServersFinished(QVector<QOpcUa::QApplicationDescription> servers, QOpcUa::UaStatusCode statusCode);

    This signal is emitted after a \l findServers() operation has finished.
    \a statusCode contains the result of the operation. If the result is \l {QOpcUa::UaStatusCode} {Good},
    \a servers contains the application descriptions of all servers known to the queried server that matched the filter criteria.
*/

/*!
    \fn void QOpcUaClient::batchReadFinished(QVector<QOpcUaReadResult> results, QOpcUa::UaStatusCode serviceResult)

    This signal is emitted after a \l batchRead() operation has finished.

    The elements in \a results have the same order as the elements in the request. For each requested element,
    there is a value together with timestamps and the status code in \a results.
    \a serviceResult contains the status code from the OPC UA Read service.

    \sa batchRead() QOpcUaReadResult QOpcUaReadItem
*/

/*!
    \internal QOpcUaClientImpl is an opaque type (as seen from the public API).
    This prevents users of the public API to use this constructor (eventhough
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
    Connects to the OPC UA endpoint given in \a url.
    \sa disconnectFromEndpoint()
*/
void QOpcUaClient::connectToEndpoint(const QUrl &url)
{
    Q_D(QOpcUaClient);
    d->connectToEndpoint(url);
}

/*!
    Disconnects from the server.
    \sa connectToEndpoint()
*/
void QOpcUaClient::disconnectFromEndpoint()
{
    Q_D(QOpcUaClient);
    d->disconnectFromEndpoint();
}

/*!
    Returns the URL of the OPC UA server the client is currently connected to
    or was last connected to.
*/
QUrl QOpcUaClient::url() const
{
    Q_D(const QOpcUaClient);
    return d->m_url;
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
QOpcUaNode *QOpcUaClient::node(const QOpcUa::QExpandedNodeId &expandedNodeId)
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
QString QOpcUaClient::resolveExpandedNodeId(const QOpcUa::QExpandedNodeId &expandedNodeId, bool *ok) const
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
QOpcUa::QQualifiedName QOpcUaClient::qualifiedNameFromNamespaceUri(const QString &namespaceUri, const QString &name, bool *ok) const
{
    if (namespaceArray().isEmpty()) {
        qCWarning(QT_OPCUA) << "Namespaces table missing, unable to resolve namespace URI.";
        if (ok)
            *ok = false;
        return QOpcUa::QQualifiedName();
    }

    int index = namespaceArray().indexOf(namespaceUri);

    if (index < 0) {
        qCWarning(QT_OPCUA) << "Failed to resolve namespace" << namespaceUri;
        if (ok)
            *ok = false;
        return QOpcUa::QQualifiedName();
    }

    if (ok)
        *ok = true;

    return QOpcUa::QQualifiedName(index, name);
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
    Starts a batch read of multiple attributes on different nodes.
    The node id, the attribute and an index range can be specified for every entry in \a nodesToRead.

    Returns true if the asynchronous request has been successfully dispatched.
    The results are returned in the \l batchReadFinished() signal.

    The batch read API offers an alternative way to read attributes of nodes which can be used
    for scenarios where the values of a large number of node attributes on different nodes must be read
    without requiring the other features of the \l QOpcUaNode based API like monitoring for value changes.
    All read items in the request are sent to the server in a single request and are answered in a single
    response which generates a single \l batchReadFinished() signal. This reduces the network overhead and
    the number of signal slot connections if many different nodes are involved.

    In the following example, the display name attribute and the two index ranges "0:2" and "5:7" of the value
    attribute of the same node and the entire value attribute of a second node are read using a single service call:
    \code
    QVector<QOpcUaReadItem> request;
    request.push_back(QOpcUaReadItem("ns=1;s=MyArrayNode",
                                     QOpcUa::NodeAttribute::DisplayName));
    request.push_back(QOpcUaReadItem("ns=1;s=MyArrayNode",
                                     QOpcUa::NodeAttribute::Value, "0:2"));
    request.push_back(QOpcUaReadItem("ns=1;s=MyArrayNode",
                                     QOpcUa::NodeAttribute::Value, "5:7"));
    request.push_back(QOpcUaReadItem("ns=1;s=MyScalarNode));
    m_client->batchRead(request);
    \endcode

    \sa QOpcUaReadItem batchReadFinished()
*/
bool QOpcUaClient::batchRead(const QVector<QOpcUaReadItem> &nodesToRead)
{
    if (state() != QOpcUaClient::Connected)
       return false;

    Q_D(QOpcUaClient);
    return d->m_impl->batchRead(nodesToRead);
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

QT_END_NAMESPACE

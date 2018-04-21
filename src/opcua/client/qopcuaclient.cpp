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
    OPC UA enabled devices and applications. This includes connecting,
    disconnecting and getting \l QOpcUaNode objects for a node on the server.

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
    Returns the name of the backend used by this instance of QOpcUaClient,
    e.g. "open62541".
*/
QString QOpcUaClient::backend() const
{
    Q_D(const QOpcUaClient);
    return d->m_impl->backend();
}

QT_END_NAMESPACE

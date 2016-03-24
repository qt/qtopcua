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

#include <QtOpcUa/qopcuaclient.h>
#include <private/qopcuaclient_p.h>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaClient
    \inmodule QtOpcUa

    \brief QOpcUaClient allows interaction with an OPC UA server.

    \chapter QOpcUaClient

    QOpcUaClient implements basic client capabilities to communicate with
    OPC UA enabled devices and applications.

    \section1 Addressing Nodes

    For an introduction to nodes, see QOpcUaNode.

    Addressing nodes on the OPC UA server using QOpcUaClient is done via a
    text format.
    A node is identified by its namespace ID and an identifier which can e. g.
    be numeric or a string.
    The identifier of a node residing in namespace 0 and having the numeric
    identifier 42, the string is \c ns=0;i=42, a node with a string
    identifier can be addressed via \c ns=0;s=myStringIdentifier.

    \section1 Secure connection

    \section2 FreeOPCUA
    Currently, the only security mechanism implemented in FreeOPCUA is
    user/password authentication which is done over an unencrypted channel.
*/

/*!
    \internal QOpcUaClientImpl is an opaque type (as seen from the public API).
    This prevents users of the public API to use this constructor (eventhough
    it is public).
*/
QOpcUaClient::QOpcUaClient(QOpcUaClientImpl *impl, QObject *parent)
    : QObject(*new QOpcUaClientPrivate(impl), parent)
{
    impl->m_clientPrivate = d_func();
}

QOpcUaClient::~QOpcUaClient()
{
}

/*!
    \fn  bool QOpcUaClient::connectToEndpoint(const QUrl &url)

    Connects to the OPC UA endpoint given in \a url.
    true is returned in case of success, false is returned when the connection
    fails.
*/
bool QOpcUaClient::connectToEndpoint(const QUrl &url)
{
    Q_D(QOpcUaClient);
    bool result = d->processUrl(url);
    if (result)
     return d->m_impl->connectToEndpoint(url);
    else
        return false;
}

/*!
    \fn bool QOpcUaClient::secureConnectToEndpoint(const QUrl &url)

    Connects to an endpoint given by \a url using the highest security level
    supported by client and server.
    true is returned in case of success, false is returned when the connection
    fails.
    User name and password must be set on \a url before connectToEndpoint
    is called.

    \warning Currently not supported by the FreeOPCUA backend.
*/
bool QOpcUaClient::secureConnectToEndpoint(const QUrl &url)
{
    Q_D(QOpcUaClient);
    bool result = d->processUrl(url);
    if (result)
        return d->m_impl->secureConnectToEndpoint(url);
    else
        return false;
}

/*!
    \fn bool QOpcUaClient::disconnectFromEndpoint()

    Disconnects from the server.
    true is returned in case of success, false is returned when the disconnect
    fails.
*/
bool QOpcUaClient::disconnectFromEndpoint()
{
    if (!isConnected())
       return false;

    return d_func()->m_impl->disconnectFromEndpoint();
}

/*!
    Returns the URL of the OPC UA server.
*/
QUrl QOpcUaClient::url() const
{
    return d_func()->m_url;
}

/*!
    \property QOpcUaClient::connected
    \brief the connection status of QOpcUaClient.

    The connection can be either unsecure or secure.

    \sa setConnected()
*/
bool QOpcUaClient::isConnected() const
{
    return d_func()->m_connected;
}

/*!
    \fn QOpcUaNode *QOpcUaClient::node(const QString &nodeId)

    Returns a pointer to a QOpcUaNode object containing the information about
    the OPC UA node identified by \a nodeId.
*/
QOpcUaNode *QOpcUaClient::node(const QString &nodeId)
{
    if (!isConnected())
       return Q_NULLPTR;

    return d_func()->m_impl->node(nodeId);
}

QString QOpcUaClient::backend() const
{
    return d_func()->m_impl->backend();
}

/*!
 * \fn QString QOpcUaClient::createSubscription()
 * \brief QOpcUaClient::createSubscription
 * \return a new subscription. The caller takes ownership.
 */
QOpcUaSubscription *QOpcUaClient::createSubscription(quint32 interval)
{
    return d_func()->m_impl->createSubscription(interval);
}

QT_END_NAMESPACE

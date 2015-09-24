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

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaClient
    \inmodule QtOpcUa

    \brief QOpcUaClient allows interaction with an OPC UA server.

    \chapter QOpcUaClient

    QOpcUaClient implements basic client capabilities to communicate with
    OPC UA enabled devices and applications.
    Three classes are exposed to the user: QOpcUaClient, QOpcUaNode and
    QOpcUaSubscription.
    The functionality is implemented in plugins. Currently only one utilizing the
    LGPL v3 implementation FreeOPCUA is provided. Other SDKs can be wrapped in
    additional plugins.

    \section1 Addressing Nodes

    For an introduction to nodes, see QOpcUaNode.

    Addressing nodes on the OPC UA server using QOpcUaClient is done via an
    XML format.
    A node is identified by its namespace ID and an identifier which can e. g.
    be numeric or a string.
    The identifier of a node residing in namespace 0 and having the numeric
    identifier 42, the XML string is \c ns=0;i=42, a node with a string
    identifier can be addressed via \c ns=0;s=myStringIdentifier.

    \section1 Security

    \section2 FreeOPCUA
    Currently, the only security mechanism implemented in FreeOPCUA is
    user/password authentication which is done over an unencrypted channel.
*/

/*!
    Constructs a QOpcUaClient.
    Can only be used in derived classes.
    The \a parent is passed on to the QObject constructor.
*/
QOpcUaClient::QOpcUaClient(QObject *parent)
    : QObject(parent)
    , m_connected(false)
{
}

/*!
    Destroys the QOpcUaClient object and all associated objects like
    subscriptions and nodes and unloads the plugin if it's the last object
    using it.
*/
QOpcUaClient::~QOpcUaClient()
{
    cleanupChildren();
}


/*!
    Derived classes can call this to set the connection state to \a connected.
    A signal \c connectedChanged() is emitted with the new value.
    Also the new value is returned.
*/
bool QOpcUaClient::setConnected(bool connected)
{
    m_connected = connected;
    emit connectedChanged(m_connected);
    return m_connected;
}

/*!
    \fn  bool QOpcUaClient::connectToEndpoint(const QString &url)

    Connects to the OPC UA endpoint given in \a url.
    true is returned in case of success, false is returned when the connection
    fails.
*/

/*!
    Connects to the endpoint given via the property \c url.
    true is returned in case of success, false is returned when the connection
    fails.
*/
bool QOpcUaClient::connectToEndpoint()
{
    if (m_url.size() > 0)
        return connectToEndpoint(m_url);
    return false;
}

/*!
    \fn bool QOpcUaClient::secureConnectToEndpoint(const QString &url)

    Connects to an endpoint given by \a url using the highest security level
    supported by client and server.
    true is returned in case of success, false is returned when the connection
    fails.
*/


/*!
    Connects to an endpoint given via the property \c url using the highest
    security level supported by client and server.
    true is returned in case of success, false is returned when the connection
    fails.
*/
bool QOpcUaClient::secureConnectToEndpoint()
{
    if (m_url.size() > 0)
        return secureConnectToEndpoint(m_url);
    return false;
}

/*!
    \fn bool QOpcUaClient::disconnectFromEndpoint()

    Disconnects from the server.
    true is returned in case of success, false is returned when the disconnect
    fails.

    Must be reimplemented in the concrete plugin implementation.
*/


/*!
    \fn QVariant QOpcUaClient::read(const QString &xmlNodeId)

    Reads the value of the OPC UA node given in \a xmlNodeId.

    Must be reimplemented in the concrete plugin implementation.
*/

/*!
    \fn bool QOpcUaClient::write(const QString &xmlNodeId, const QVariant &value, QOpcUa::Types type = QOpcUa::Undefined)

    Writes the value given in \a value to the OPC UA node given in
    \a xmlNodeId. The data type must be supplied in \a type.
    true is returned in case of success, false is returned when the
    write call fails.

    Must be reimplemented in the concrete plugin implementation.
  */

/*!
    \fn bool QOpcUaClient::call(const QString &xmlObjectNodeId, const QString &xmlMethodNodeId, QVector<QOpcUaTypedVariant> *args = 0, QVector<QVariant>  *ret = 0)

    Calls the OPC UA method \a xmlMethodNodeId of the object
    \a xmlObjectNodeId with the parameters given via \a args. The result is
    returned in \a ret. The success of the service call is returned by the
    method.

    Must be reimplemented in the concrete plugin implementation.
*/

/*!
    \fn QPair<QString, QString> QOpcUaClient::readEui(const QString &xmlNodeId)

    Reads Engineering unit information from the OPC UA node identified by
    \a xmlNodeId and returned as a pair of strings containing the unit and
    its description.

    Must be reimplemented in the concrete plugin implementation.
*/

/*!
    \fn QPair<double, double> QOpcUaClient::readEuRange(const QString &xmlNodeId)

    Reads value range from the OPC UA node identified by \a xmlNodeId and
    returned as a pair of doubles containing the lower and upper limit.

    Must be reimplemented in the concrete plugin implementation.
*/

/*!
    \fn QOpcUaMonitoredItem *QOpcUaClient::dataMonitor(double interval, const QString &xmlNodeId)

    Returns a \l QOpcUaMonitoredItem for the node identified by the \a xmlNodeId and
    the requested \a interval.
*/

/*!
    \fn QOpcUaMonitoredItem *QOpcUaClient::eventMonitor(const QString &xmlEventNodeId)

    Returns a \l QOpcUaMonitoredItem pointer for the event identified by \a xmlEventNodeId.
*/

/*!
    \fn QOpcUaNode *QOpcUaClient::node(const QString &xmlNodeId)

    Returns a pointer to a QOpcUaNode object containing the information about
    the OPC UA node identified by \a xmlNodeId.

    Must be reimplemented in the concrete plugin implementation.
*/

/*!
    \fn QList<QPair<QVariant, QDateTime> > QOpcUaClient::readHistorical(const QString &node, int maxCount, const QDateTime &begin, const QDateTime &end)

    Reads up to \a maxCount points of historical data from the OPC UA server
    for the node \a node between the two timestamps \a begin and \a end.

    Must be reimplemented in the concrete plugin implementation.
*/


/*!
    Sets \a url as the URL of the OPC UA server to connect to.
 */
void QOpcUaClient::setUrl(const QString &url)
{
    if (m_url != url) {
        m_url = url;
        emit urlChanged();
    }
}

/*!
    \property QOpcUaClient::url
    \brief the URL of the OPC UA server.
*/
QString QOpcUaClient::url() const
{
    return m_url;
}

/*!
    \property QOpcUaClient::connected
    \brief the connection status of QOpcUaClient.
*/
bool QOpcUaClient::isConnected() const
{
    return m_connected;
}


void QOpcUaClient::setUsername(const QString &username)
{
    m_username = username;
}

/*!
    \property QOpcUaClient::username
    \brief the user name for user/pass authentication with an OPC UA server.

    This value must be set before calling any connect method.
*/

QString QOpcUaClient::username() const
{
    return m_username;
}

void QOpcUaClient::setPassword(const QString &password)
{
    m_password = password;
}

/*!
    \property QOpcUaClient::password
    \brief the password for user/pass authentication with an OPC UA server.

    This value must be set before calling any connect method.
*/
QString QOpcUaClient::password() const
{
    return m_password;
}

/*!
    \internal
    Destroys all node objects.
*/
void QOpcUaClient::cleanupChildren()
{
    QList<QOpcUaNode *> nodes = findChildren<QOpcUaNode *>();
    qDeleteAll(nodes);
}

/*!
  \fn QOpcUaClient::createSubscription(double interval)

  This method must be overridden in derived classes to provide new subscriptions.
  QOpcUaClient keeps track of existing subscriptions and only asks for a new one
  if there is not already one with the requested \a interval.

  Returns a new subscription, 0 if the creation failed.

  \sa getSubscription()
*/

/*!
    Returns a pointer to a \l QOpcUaSubscription with the specified \a interval.
    If such a subscription does not exist yet, it is created.

    \sa createSubscription()
 */
QOpcUaSubscription *QOpcUaClient::getSubscription(double interval)
{
    if (m_subscriptions.contains(interval))
        return m_subscriptions.value(interval);

    QOpcUaSubscription *newSubscription = createSubscription(interval);

    if (newSubscription)
        m_subscriptions.insert(interval, newSubscription);

    return newSubscription;
}

/*!
    Removes the subscription with the specified \a interval.
    Returns false if there is no matching subscription, true if it was found
    and deleted.
 */
bool QOpcUaClient::removeSubscription(double interval)
{
    if (m_subscriptions.contains(interval)) {
        delete m_subscriptions.value(interval);
        m_subscriptions.remove(interval);
        return true;
    }

    return false;
}

QT_END_NAMESPACE

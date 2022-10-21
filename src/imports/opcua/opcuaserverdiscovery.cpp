/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
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

#include "opcuaserverdiscovery.h"

QT_BEGIN_NAMESPACE

/*!
    \qmltype ServerDiscovery
    \inqmlmodule QtOpcUa
    \brief Provides information about available servers.
    \since QtOpcUa 5.13

    Allows to fetch and access information about servers known to a server or discovery server.

    \snippet ../../src/imports/doc/snippets/basic/basic.qml Basic discovery
*/

/*!
    \qmlproperty string ServerDiscovery::discoveryUrl

    URL of the server to retrieve the list of servers from.
    Every time the URL is changed, a request to the given server is started.

    When starting the request, the list of available servers is cleared
    and the status is set to \l {Status::Status}{Status.GoodCompletesAsynchronously}.
    Once the request is finished, \l status changes.
    Make sure to check the \l status before acessing the list of servers.

    \code
    onServersChanged: {
            if (status.isGood) {
                if (status.status == QtOpcUa.Status.GoodCompletesAsynchronusly)
                    return; // wait until finished
                if (count > 0) {
                    var serverUrl = at(0).serverUrl();
                    console.log(serverUrl);
                }
            } else {
                // handle error
            }
    }
    \endcode

    \sa ApplicationDescription status at count Status
*/

/*!
    \qmlproperty int ServerDiscovery::count

    Current number of servers in this element.
    Before using any data from this server discovery, you should check \l status if retrieval of the
    information was successful.

    \sa status Status
*/

/*!
    \qmlproperty Status ServerDiscovery::status

    The current status of this element.
    In case the last retrieval of servers was successful, the status
    should be \c Status.Good.

    \code
    if (status.isGood) {
        // Choose endpoint to connect to
    } else {
        // handle error
    }
    \endcode

    \sa Status
*/

/*!
    \qmlsignal SeverDiscovery::serversChanged()

    Emitted when a retrieval request started, finished or failed.
    In a called function, you should first the the \l status of the object.
    In case the status is \l Status.GoodCompletesAsynchronously, the request is still running.
    In case the status is \l Status.Good, the request has finished and the application descriptions
    can be read. In case the status is not good, an error happended and \l status contains the
    returned error code.

    \code
    onServersChanged: {
            if (status.isGood) {
                if (status.status == QtOpcUa.Status.GoodCompletesAsynchronusly)
                    return; // wait until finished
                if (count > 0) {
                    var serverUrl = at(0).endpointUrl();
                    console.log(serverUrl);
                }
            } else {
                // handle error
            }
    }
    \endcode

    \sa status count at QtOpcUa.Status ApplicationDescription
*/

OpcUaServerDiscovery::OpcUaServerDiscovery(QObject *parent)
    : QStandardItemModel(parent)
{
    insertColumn(0);
}

OpcUaServerDiscovery::~OpcUaServerDiscovery() = default;

const QString &OpcUaServerDiscovery::discoveryUrl() const
{
    return m_discoveryUrl;
}

void OpcUaServerDiscovery::setDiscoveryUrl(const QString &discoveryUrl)
{
    if (discoveryUrl == m_discoveryUrl)
        return;
    m_discoveryUrl = discoveryUrl;

    startFindServers();
    emit discoveryUrlChanged();
}

int OpcUaServerDiscovery::count() const
{
    return rowCount();
}

/*!
    \qmlmethod ApplicationDescription ServerDiscovery::at(index)

    Returns the application description at given \a index.
    In case there are no servers available or the index is invalid, an invalid
    application description is returned.
    Before using any returned data, you should check \l status if retrieval of the
    information was successful.

    \code
    if (servers.status.isGood) {
        if (servers.count > 0)
            var serverUrl = at(0).serverUrl();
            console.log(serverUrl);
            // Choose endpoint to connect to
    } else {
        // handle error
    }
    \endcode

    \sa count status ApplicationDescription
*/

/*!
    \qmlproperty Connection ServerDiscovery::connection

    The connection to be used for requesting information.

    If this property is not set, the default connection will be used, if any.

    \sa Connection, Connection::defaultConnection
*/

QOpcUaApplicationDescription OpcUaServerDiscovery::at(int row) const
{
    return index(row, 0).data(Qt::UserRole).value<QOpcUaApplicationDescription>();
}

const OpcUaStatus &OpcUaServerDiscovery::status() const
{
    return m_status;
}

void OpcUaServerDiscovery::connectSignals()
{
    auto conn = connection();
    if (!conn || !conn->m_client)
        return;
    connect(conn->m_client, &QOpcUaClient::findServersFinished, this, &OpcUaServerDiscovery::handleServers, Qt::UniqueConnection);
    startFindServers();
}

void OpcUaServerDiscovery::handleServers(const QList<QOpcUaApplicationDescription> &servers, QOpcUa::UaStatusCode statusCode, const QUrl &requestUrl)
{
    if (requestUrl != m_discoveryUrl)
        return; // response is not for last request

    m_status = OpcUaStatus(statusCode);

    if (m_status.isBad()) {
        emit statusChanged();
        return;
    }

    clearData();
    for (const auto &i : qAsConst(servers)) {
        const int newRow = QStandardItemModel::rowCount();
        QStandardItemModel::insertRow(newRow);
        QStandardItemModel::setData(index(newRow, 0), i.applicationUri() + QLatin1String("\n") + i.productUri(), Qt::DisplayRole);
        QStandardItemModel::setData(index(newRow, 0), QVariant::fromValue(i), Qt::UserRole);
    }
    emit countChanged();
    emit serversChanged();
    emit statusChanged();
}

void OpcUaServerDiscovery::startFindServers()
{
    if (m_discoveryUrl.isEmpty())
        return;

    if (!m_connection) {
        // In case no connection is set the default connection will be
        // used, which fill trigger this function afterwards.
        connection();
        return;
    }

    clearData();

    auto conn = connection();
    if (!conn || !conn->m_client) {
        m_status = OpcUaStatus(QOpcUa::BadNotConnected);
    } else if (m_discoveryUrl.isEmpty()) {
        m_status = OpcUaStatus(QOpcUa::BadInvalidArgument);
    } else {
        m_status = OpcUaStatus(QOpcUa::GoodCompletesAsynchronously);
        conn->m_client->findServers(m_discoveryUrl);
    }

    emit serversChanged();
    emit statusChanged();
}

void OpcUaServerDiscovery::clearData()
{
    QStandardItemModel::removeRows(0, QStandardItemModel::rowCount());
}

void OpcUaServerDiscovery::setConnection(OpcUaConnection *connection)
{
    if (connection == m_connection || !connection)
        return;

    if (m_connection)
        disconnect(m_connection, &OpcUaConnection::backendChanged, this, &OpcUaServerDiscovery::connectSignals);

    m_connection = connection;

    connect(m_connection, &OpcUaConnection::backendChanged, this, &OpcUaServerDiscovery::connectSignals);
    connectSignals();
    emit connectionChanged(connection);
}

OpcUaConnection *OpcUaServerDiscovery::connection()
{
    if (!m_connection)
        setConnection(OpcUaConnection::defaultConnection());

    return m_connection;
}

QT_END_NAMESPACE

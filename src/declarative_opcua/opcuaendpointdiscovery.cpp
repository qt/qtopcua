// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/opcuaendpointdiscovery_p.h>
#include <private/opcuaconnection_p.h>

QT_BEGIN_NAMESPACE

/*!
    \qmltype EndpointDiscovery
    \inqmlmodule QtOpcUa
    \brief Provides information about available endpoints on a server.
    \since QtOpcUa 5.13

    Allows to fetch and access information about available endpoints on a server.

    \snippet ../../src/declarative_opcua/doc/snippets/basic/basic.qml Basic discovery
*/

/*!
    \qmlproperty string EndpointDiscovery::serverUrl

    Discovery URL of the server to fetch the endpoints from.
    Every time the URL is changed, a request to the given server is started.

    When starting the request, the list of available endpoints is cleared
    and the status is set to \l {Status::Status}{Status.GoodCompletesAsynchronously}. Once the request is finished,
    \l status changes.

    Make sure to check \l status before accessing the list of endpoints.

    \sa endpointsChanged
*/

/*!
    \qmlproperty int EndpointDiscovery::count

    Current number of endpoints in this element.
    Before using any data from an endpoint discovery, you should check \l status if the information
    was successfully retrieved.

    \sa status Status
*/

/*!
    \qmlproperty Status EndpointDiscovery::status

    The current status of this element.
    In case the last retrieval of endpoints was successful, the status
    is \l {Status::Status}{Status.Good}.

    \code
    if (endpoints.status.isGood) {
        // do something
    } else {
        // handle error
    }
    \endcode

    \sa Status
*/

/*!
    \qmlsignal EndpointDiscovery::endpointsChanged()

    Emitted when a retrieval request started, finished or failed.
    In a called function, you should first check the \l status of the object.
    If the status is \l {Status::Status}{Status.GoodCompletesAsynchronously},
    the request is still running.
    If the status is \l {Status::Status}{Status.Good}, the request has finished
    and the endpoint descriptions can be read. If the status is not good, an
    error happened and \l status contains the returned error code.

    \code
    onEndpointsChanged: {
            if (endpoints.status.isGood) {
                if (endpoints.status.status == QtOpcua.Status.GoodCompletesAsynchronusly)
                    return; // wait until finished
                if (endpoints.count > 0) {
                    var endpointUrl = endpoints.at(0).endpointUrl();
                    console.log(endpointUrl);
                }
            } else {
                // handle error
            }
    }
    \endcode

    \sa status count at Status EndpointDescription
*/

/*!
    \qmlproperty Connection EndpointDiscovery::connection

    The connection to be used for requesting information.

    If this property is not set, the default connection will be used, if any.

    \sa Connection, Connection::defaultConnection
*/

OpcUaEndpointDiscovery::OpcUaEndpointDiscovery(QObject *parent)
    : QObject(parent)
{
    connect(this, &OpcUaEndpointDiscovery::serverUrlChanged, this, &OpcUaEndpointDiscovery::startRequestEndpoints);
    connect(this, &OpcUaEndpointDiscovery::connectionChanged, this, &OpcUaEndpointDiscovery::startRequestEndpoints);
}

OpcUaEndpointDiscovery::~OpcUaEndpointDiscovery() = default;

const QString &OpcUaEndpointDiscovery::serverUrl() const
{
    return m_serverUrl;
}

void OpcUaEndpointDiscovery::setServerUrl(const QString &serverUrl)
{
    if (serverUrl == m_serverUrl)
        return;

    m_serverUrl = serverUrl;
    emit serverUrlChanged(m_serverUrl);
}

int OpcUaEndpointDiscovery::count() const
{
    return m_endpoints.size();
}

/*!
    \qmlmethod EndpointDescription EndpointDiscovery::at(index)

    Returns the endpoint description at given \a index.
    In case there are no endpoints available or the index is invalid, an invalid
    endpoint description is returned.
    Before using any data from this, you should check \l status if retrieval of the
    information was successful.

    \code
    if (endpoints.status.isGood) {
        if (endpoints.count > 0)
            var endpointUrl = endpoints.at(0).endpointUrl();
            console.log(endpointUrl);
    } else {
        // handle error
    }
    \endcode

    \sa count status EndpointDescription
*/

QOpcUaEndpointDescription OpcUaEndpointDiscovery::at(int row) const
{
    if (row >= m_endpoints.size())
        return QOpcUaEndpointDescription();
    return m_endpoints.at(row);
}

const OpcUaStatus &OpcUaEndpointDiscovery::status() const
{
    return m_status;
}

void OpcUaEndpointDiscovery::connectSignals()
{
    auto conn = connection();

    if (!conn || !conn->m_client)
        return;
    connect(conn->m_client, &QOpcUaClient::endpointsRequestFinished, this, &OpcUaEndpointDiscovery::handleEndpoints, Qt::UniqueConnection);
}

void OpcUaEndpointDiscovery::handleEndpoints(const QList<QOpcUaEndpointDescription> &endpoints, QOpcUa::UaStatusCode statusCode, const QUrl &requestUrl)
{
    if (requestUrl != m_serverUrl)
        return; // response is not for last request

    m_status = OpcUaStatus(statusCode);

    if (m_status.isBad()) {
        emit statusChanged();
        return;
    }

    m_endpoints = endpoints;
    emit endpointsChanged();
    emit countChanged();
    emit statusChanged();
}

void OpcUaEndpointDiscovery::startRequestEndpoints()
{
    if (!m_componentCompleted)
        return;

    if (m_serverUrl.isEmpty())
        return;

    m_endpoints.clear();

    if (!m_connection) {
        // If there is not connection set, try the default connection
        // Any connection change will restart this function
        connection();
        return;
    }

    auto conn = connection();

    if (!conn || !conn->m_client) {
        m_status = OpcUaStatus(QOpcUa::BadNotConnected);
    } else if (m_serverUrl.isEmpty()) {
        m_status = OpcUaStatus(QOpcUa::BadInvalidArgument);
    } else {
        m_status = OpcUaStatus(QOpcUa::GoodCompletesAsynchronously);
        conn->m_client->requestEndpoints(m_serverUrl);
    }

    emit endpointsChanged();
    emit statusChanged();
}

void OpcUaEndpointDiscovery::setConnection(OpcUaConnection *connection)
{
    if (connection == m_connection || !connection)
        return;

    if (m_connection)
        disconnect(m_connection, &OpcUaConnection::backendChanged, this, &OpcUaEndpointDiscovery::connectSignals);

    m_connection = connection;

    connect(m_connection, &OpcUaConnection::backendChanged, this, &OpcUaEndpointDiscovery::connectSignals, Qt::UniqueConnection);
    connectSignals();
    emit connectionChanged(connection);
}

OpcUaConnection *OpcUaEndpointDiscovery::connection()
{
    if (!m_connection)
        setConnection(OpcUaConnection::defaultConnection());

    return m_connection;
}

void OpcUaEndpointDiscovery::classBegin()
{
}

void OpcUaEndpointDiscovery::componentComplete()
{
    m_componentCompleted = true;
    startRequestEndpoints();
}

QT_END_NAMESPACE

// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuahistoryreadeventrequest.h"

#include <QtOpcUa/qopcuareaditem.h>

#include <QtCore/qdatetime.h>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaHistoryReadEventRequest
    \inmodule QtOpcUa
    \brief This class stores the necessary information to request historic data from a server.
    \since 6.7

    This is the Qt OPC UA representation for the OPC UA ReadEventDetails for reading historical data
    defined in \l {https://reference.opcfoundation.org/Core/docs/Part11/6.4.2/} {OPC-UA part 11, 6.4.2}.

    When requesting historic data from a server, several values need to be provided to the server
    to know which data to collect. The QOpcUaHistoryReadEventRequest class provides the required values.
    \a startTimestamp and \a endTimestamp define the timerange where historic events should be collected from.
    \a nodesToRead defines from which nodes historic data should be collected.
    \a numValuesPerNode defines the maximum number of events that should be returned per node.
    \a filter is the event filter used to determine which events and which of their fields to return.
*/
class QOpcUaHistoryReadEventRequestData : public QSharedData
{
public:
    QDateTime startTimestamp;
    QDateTime endTimestamp;
    quint32 numValuesPerNode = 0;
    QList<QOpcUaReadItem> nodesToRead;
    QOpcUaMonitoringParameters::EventFilter filter;
};

QT_DEFINE_QESDP_SPECIALIZATION_DTOR(QOpcUaHistoryReadEventRequestData)

/*!
    Constructs an invalid QOpcUaHistoryReadEventRequest.
 */
QOpcUaHistoryReadEventRequest::QOpcUaHistoryReadEventRequest()
    : data(new QOpcUaHistoryReadEventRequestData)
{
}

/*!
    Constructs a QOpcUaHistoryReadEventRequest item with the given values.
*/
QOpcUaHistoryReadEventRequest::QOpcUaHistoryReadEventRequest(const QList<QOpcUaReadItem> &nodesToRead,
                                                             const QDateTime &startTimestamp,
                                                             const QDateTime &endTimestamp,
                                                             const QOpcUaMonitoringParameters::EventFilter &filter)
    : data(new QOpcUaHistoryReadEventRequestData)
{
    data->startTimestamp = startTimestamp;
    data->endTimestamp = endTimestamp;
    data->nodesToRead = nodesToRead;
    data->filter = filter;
}

/*!
    Constructs a QOpcUaHistoryReadEventRequest item from \a other.
*/
QOpcUaHistoryReadEventRequest::QOpcUaHistoryReadEventRequest(const QOpcUaHistoryReadEventRequest &other)
    : data(other.data)
{
}

/*!
    Destroys the request object.
 */
QOpcUaHistoryReadEventRequest::~QOpcUaHistoryReadEventRequest()
{
}

/*!
    \fn QOpcUaHistoryReadEventRequest::QOpcUaHistoryReadEventRequest(QOpcUaHistoryReadEventRequest &&other)

    Move-constructs a new read event request object from \a other.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn QOpcUaHistoryReadEventRequest &QOpcUaHistoryReadEventRequest::operator=(QOpcUaHistoryReadEventRequest &&other)

    Move-assigns \a other to this QOpcUaHistoryReadEventRequest instance.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn void QOpcUaHistoryReadEventRequest::swap(QOpcUaHistoryReadEventRequest &other)

    Swaps read event request object \a other with this read event request
    object. This operation is very fast and never fails.
*/

/*!
    Returns the start time stamp.
*/
QDateTime QOpcUaHistoryReadEventRequest::startTimestamp() const
{
    return data->startTimestamp;
}

/*!
    Sets \a startTimestamp for the historical data to be fetched.
*/
void QOpcUaHistoryReadEventRequest::setStartTimestamp(const QDateTime &startTimestamp)
{
    if (!(data->startTimestamp == startTimestamp)) {
        data.detach();
        data->startTimestamp = startTimestamp;
    }
}

/*!
    Returns the end time stamp.
*/
QDateTime QOpcUaHistoryReadEventRequest::endTimestamp() const
{
    return data->endTimestamp;
}

/*!
    Sets \a endTimestamp for the historical data to be fetched.
*/
void QOpcUaHistoryReadEventRequest::setEndTimestamp(const QDateTime &endTimestamp)
{
    if (!(data->endTimestamp == endTimestamp)) {
        data.detach();
        data->endTimestamp = endTimestamp;
    }
}

/*!
    Returns the number of values per node.
*/
quint32 QOpcUaHistoryReadEventRequest::numValuesPerNode() const
{
    return data->numValuesPerNode;
}

/*!
    Sets \a numValuesPerNode to indicate the number of values per node to be
    fetched.
*/
void QOpcUaHistoryReadEventRequest::setNumValuesPerNode(quint32 numValuesPerNode)
{
    if (!(data->numValuesPerNode == numValuesPerNode)) {
        data.detach();
        data->numValuesPerNode = numValuesPerNode;
    }
}

/*!
    Returns the event filter used to retrieve historical events.
*/
QOpcUaMonitoringParameters::EventFilter QOpcUaHistoryReadEventRequest::filter() const
{
    return data->filter;
}

/*!
    Sets the event filter used to retrieve historical events to \a filter.
*/
void QOpcUaHistoryReadEventRequest::setFilter(const QOpcUaMonitoringParameters::EventFilter &filter)
{
    if (!(filter == data->filter)) {
        data.detach();
        data->filter = filter;
    }
}

/*!
    Returns the list of nodes to read.
*/
QList<QOpcUaReadItem> QOpcUaHistoryReadEventRequest::nodesToRead() const
{
    return data->nodesToRead;
}

/*!
    Sets the \a nodesToRead list.
*/
void QOpcUaHistoryReadEventRequest::setNodesToRead(const QList<QOpcUaReadItem> &nodesToRead)
{
    if (nodesToRead != data->nodesToRead) {
        data.detach();
        data->nodesToRead = nodesToRead;
    }
}

/*!
    Adds a node to the \a nodeToRead list.
*/
void QOpcUaHistoryReadEventRequest::addNodeToRead(const QOpcUaReadItem &nodeToRead)
{
    data.detach();
    data->nodesToRead.append(nodeToRead);
}

/*!
    Sets the values from \a other in this QOpcUaHistoryReadEventRequest item.
*/
QOpcUaHistoryReadEventRequest &QOpcUaHistoryReadEventRequest::operator=(const QOpcUaHistoryReadEventRequest &other)
{
    if (this != &other)
        data.operator=(other.data);
    return *this;
}

/*!
    \fn bool QOpcUaHistoryReadEventRequest::operator==(const QOpcUaHistoryReadEventRequest& lhs,
                                                     const QOpcUaHistoryReadEventRequest &rhs)

    Returns \c true if \a lhs is equal to \a rhs; otherwise returns \c false.

    Two QOpcUaHistoryReadEventRequest items are considered equal if their \c startTimestamp,
    \c endTimestamp, \c numValuesPerNode, \c filter and \c nodesToRead are equal.
*/
bool comparesEqual(const QOpcUaHistoryReadEventRequest &lhs,
                   const QOpcUaHistoryReadEventRequest &rhs) noexcept
{
    return (lhs.data->startTimestamp == rhs.data->startTimestamp &&
            lhs.data->endTimestamp == rhs.data->endTimestamp &&
            lhs.data->numValuesPerNode == rhs.data->numValuesPerNode &&
            lhs.data->nodesToRead == rhs.data->nodesToRead);
}

/*!
    \fn bool QOpcUaHistoryReadEventRequest::operator!=(const QOpcUaHistoryReadEventRequest &lhs,
                                                     const QOpcUaHistoryReadEventRequest &rhs)

    Returns \c true if \a lhs is not equal to \a rhs; otherwise returns \c false.

    Two QOpcUaHistoryReadEventRequest items are considered not equal if their \c startTimestamp,
    \c endTimestamp, \c numValuesPerNode, \c filter or \c nodesToRead are not equal.
*/

QT_END_NAMESPACE

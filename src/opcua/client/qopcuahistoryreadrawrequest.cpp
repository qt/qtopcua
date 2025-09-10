// Copyright (C) 2019 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuahistoryreadrawrequest.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaHistoryReadRawRequest
    \inmodule QtOpcUa
    \brief This class stores the necessary information to request historic data from a server.
    \since 6.3

    This is the Qt OPC UA representation for the OPC UA ReadRawModifiedDetails for reading historical data
    defined in \l {https://reference.opcfoundation.org/Core/Part11/v104/docs/6.4.3} {OPC UA 1.04 part 11, 6.4.3}.

    When requesting historic data from a server, several values need to be provided to the server
    to know which data to collect. The QOpcUaHistoryReadRawRequest class provides the required values.
    \a startTimestamp and \a endTimestamp define the timerange where historic data should be collected from.
    \a nodesToRead defines from which nodes historic data should be collected.
    \a numValuesPerNode defines the maximum number of data values that should be returned per node.
    \a returnBounds defines if the bounding values should be included in the result.
*/
class QOpcUaHistoryReadRawRequestData : public QSharedData
{
public:
    QDateTime startTimestamp;
    QDateTime endTimestamp;
    quint32 numValuesPerNode = 0;
    bool returnBounds = false;
    QList<QOpcUaReadItem> nodesToRead;
    QOpcUa::TimestampsToReturn timestampsToReturn = QOpcUa::TimestampsToReturn::Both;
};

/*!
    Constructs an invalid QOpcUaHistoryReadRawRequest.
 */
QOpcUaHistoryReadRawRequest::QOpcUaHistoryReadRawRequest()
    : data(new QOpcUaHistoryReadRawRequestData)
{
}

/*!
    Constructs a QOpcUaHistoryReadRawRequest item with the given values.
*/
QOpcUaHistoryReadRawRequest::QOpcUaHistoryReadRawRequest(const QList<QOpcUaReadItem> &nodesToRead,
                                                         const QDateTime &startTimestamp,
                                                         const QDateTime &endTimestamp,
                                                         quint32 numValuesPerNode,
                                                         bool returnBounds)
    : data(new QOpcUaHistoryReadRawRequestData)
{
    data->startTimestamp = startTimestamp;
    data->endTimestamp = endTimestamp;
    data->numValuesPerNode = numValuesPerNode;
    data->returnBounds = returnBounds;
    data->nodesToRead = nodesToRead;
}

/*!
    Constructs a QOpcUaHistoryReadRawRequest item with the given values.
    The \a timestampsToReturn parameter determines the timestamps to return for each value.

    \since 6.7
*/
QOpcUaHistoryReadRawRequest::QOpcUaHistoryReadRawRequest(const QList<QOpcUaReadItem> &nodesToRead,
                                                         const QDateTime &startTimestamp, const QDateTime &endTimestamp,
                                                         QOpcUa::TimestampsToReturn timestampsToReturn)
    : data(new QOpcUaHistoryReadRawRequestData)
{
    data->startTimestamp = startTimestamp;
    data->endTimestamp = endTimestamp;
    data->nodesToRead = nodesToRead;
    data->timestampsToReturn = timestampsToReturn;
}

/*!
    Constructs a QOpcUaHistoryReadRawRequest item from \a other.
*/
QOpcUaHistoryReadRawRequest::QOpcUaHistoryReadRawRequest(const QOpcUaHistoryReadRawRequest &other)
    : data(other.data)
{
}

/*!
    Destroys the request object.
 */
QOpcUaHistoryReadRawRequest::~QOpcUaHistoryReadRawRequest()
{
}

/*!
    \fn QOpcUaHistoryReadRawRequest::swap(QOpcUaHistoryReadRawRequest &other)

    Swaps this request instance with \a other. This function is very
    fast and never fails.
 */

/*!
    Returns the start time stamp.
*/
QDateTime QOpcUaHistoryReadRawRequest::startTimestamp() const
{
    return data->startTimestamp;
}

/*!
    Sets \a startTimestamp for the historical data to be fetched.
*/
void QOpcUaHistoryReadRawRequest::setStartTimestamp(const QDateTime &startTimestamp)
{
    if (data->startTimestamp == startTimestamp)
        return;

    data->startTimestamp = startTimestamp;
}

/*!
    Returns the end time stamp.
*/
QDateTime QOpcUaHistoryReadRawRequest::endTimestamp() const
{
    return data->endTimestamp;
}

/*!
    Sets \a endTimestamp for the historical data to be fetched.
*/
void QOpcUaHistoryReadRawRequest::setEndTimestamp(const QDateTime &endTimestamp)
{
    if (data->endTimestamp == endTimestamp)
        return;

    data->endTimestamp = endTimestamp;
}

/*!
    Returns the number of values per node.
*/
quint32 QOpcUaHistoryReadRawRequest::numValuesPerNode() const
{
    return data->numValuesPerNode;
}

/*!
    Sets \a numValuesPerNode to indicate the number of values per node to be
    fetched.
*/
void QOpcUaHistoryReadRawRequest::setNumValuesPerNode(quint32 numValuesPerNode)
{
    if (data->numValuesPerNode == numValuesPerNode)
        return;

    data->numValuesPerNode = numValuesPerNode;
}

/*!
    Returns if the return bounds should be requested.
*/
bool QOpcUaHistoryReadRawRequest::returnBounds() const
{
    return data->returnBounds;
}

/*!
    Sets \a returnBounds to indicate if the return bounds should be requested.
*/
void QOpcUaHistoryReadRawRequest::setReturnBounds(bool returnBounds)
{
    data->returnBounds = returnBounds;
}

/*!
    Returns the selected timestamps to return for each value.

    \since 6.7
*/
QOpcUa::TimestampsToReturn QOpcUaHistoryReadRawRequest::timestampsToReturn() const
{
    return data->timestampsToReturn;
}

/*!
    Sets the selected timestamps to return for each value to \a timestampsToReturn.

    \since 6.7
*/
void QOpcUaHistoryReadRawRequest::setTimestampsToReturn(QOpcUa::TimestampsToReturn timestampsToReturn)
{
    data->timestampsToReturn = timestampsToReturn;
}

/*!
    Returns the list of nodes to read.
*/
QList<QOpcUaReadItem> QOpcUaHistoryReadRawRequest::nodesToRead() const
{
    return data->nodesToRead;
}

/*!
    Sets the \a nodesToRead list.
*/
void QOpcUaHistoryReadRawRequest::setNodesToRead(const QList<QOpcUaReadItem> &nodesToRead)
{
    data->nodesToRead = nodesToRead;
}

/*!
    Adds a node to the \a nodeToRead list.
*/
void QOpcUaHistoryReadRawRequest::addNodeToRead(const QOpcUaReadItem &nodeToRead)
{
    data->nodesToRead.append(nodeToRead);
}

/*!
    Sets the values from \a other in this QOpcUaHistoryReadRawRequest item.
*/
QOpcUaHistoryReadRawRequest &QOpcUaHistoryReadRawRequest::operator=(const QOpcUaHistoryReadRawRequest &other)
{
    if (this != &other)
        data.operator=(other.data);
    return *this;
}

/*!
    \fn bool QOpcUaHistoryReadRawRequest::operator==(const QOpcUaHistoryReadRawRequest& lhs,
                                                     const QOpcUaHistoryReadRawRequest &rhs)

    Returns \c true if \a lhs is equal to \a rhs; otherwise returns \c false.

    Two QOpcUaHistoryReadRawRequest items are considered equal if their \c startTimestamp,
    \c endTimestamp, \c numValuesPerNode, \c returnBounds and \c nodesToRead are equal.
*/
bool operator==(const QOpcUaHistoryReadRawRequest &lhs,
                const QOpcUaHistoryReadRawRequest &rhs) noexcept
{
    return (lhs.data->startTimestamp == rhs.data->startTimestamp &&
            lhs.data->endTimestamp == rhs.data->endTimestamp &&
            lhs.data->numValuesPerNode == rhs.data->numValuesPerNode &&
            lhs.data->returnBounds == rhs.data->returnBounds &&
            lhs.data->nodesToRead == rhs.data->nodesToRead);
}

/*!
    \fn bool QOpcUaHistoryReadRawRequest::operator!=(const QOpcUaHistoryReadRawRequest &lhs,
                                                     const QOpcUaHistoryReadRawRequest &rhs)

    Returns \c true if \a lhs is not equal to \a rhs; otherwise returns \c false.

    Two QOpcUaHistoryReadRawRequest items are considered not equal if their \c startTimestamp,
    \c endTimestamp, \c numValuesPerNode, \c returnBounds or \c nodesToRead are not equal.
*/

QT_END_NAMESPACE

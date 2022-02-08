/****************************************************************************
**
** Copyright (C) 2019 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qopcuahistoryreadrawrequest.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaHistoryReadRawRequest
    \inmodule QtOpcUa
    \brief This class stores the necessary information to request historic data from a server.
    \since 6.3

    This is the Qt OPC UA representation for the OPC UA ReadRawModifiedDetails for reading historical data
    defined in \l {https://reference.opcfoundation.org/Core/docs/Part11/6.4.3/} {OPC-UA part 11, 6.4.3}.

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
    quint32 numValuesPerNode;
    bool returnBounds;
    QList<QOpcUaReadItem> nodesToRead;
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

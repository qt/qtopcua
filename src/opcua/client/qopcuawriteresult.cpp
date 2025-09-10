// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuawriteresult.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaWriteResult
    \inmodule QtOpcUa
    \brief This class stores the result of a write operation.

    A write operation on an OPC UA server returns a status code which describes if the value could
    be written and if not, for what reason the write has failed.

    In addition to the status code returned by the server, this class also contains the node id, the attribute and the index
    range from the request to enable a client to match the result with a request.

    Objects of this class are returned in the \l QOpcUaClient::writeNodeAttributesFinished()
    signal and contain the result of a write operation that was part of a \l QOpcUaClient::writeNodeAttributes()
    request.

    \sa QOpcUaClient::writeNodeAttributes() QOpcUaClient::writeNodeAttributesFinished() QOpcUaWriteItem
*/
class QOpcUaWriteResultData : public QSharedData
{
public:
    QString nodeId;
    QOpcUa::NodeAttribute attribute {QOpcUa::NodeAttribute::Value};
    QString indexRange;
    QOpcUa::UaStatusCode statusCode {QOpcUa::UaStatusCode::Good};
};

/*!
    Default constructs a write result with no parameters set.
*/
QOpcUaWriteResult::QOpcUaWriteResult()
    : data(new QOpcUaWriteResultData)
{
}

/*!
    Constructs a write result from \a other.
*/
QOpcUaWriteResult::QOpcUaWriteResult(const QOpcUaWriteResult &other)
    : data(other.data)
{
}

/*!
    Sets the values from \a rhs in this write result.
*/
QOpcUaWriteResult &QOpcUaWriteResult::operator=(const QOpcUaWriteResult &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUaWriteResult::~QOpcUaWriteResult()
{
}

/*!
    Returns the node id of the write result.
*/
QString QOpcUaWriteResult::nodeId() const
{
    return data->nodeId;
}

/*!
    Sets the node id of the write result to \a nodeId.
*/
void QOpcUaWriteResult::setNodeId(const QString &nodeId)
{
    data->nodeId = nodeId;
}

/*!
    Returns the attribute of the write result.
*/
QOpcUa::NodeAttribute QOpcUaWriteResult::attribute() const
{
    return data->attribute;
}

/*!
    Sets the attribute of the write result to \a attribute.
*/
void QOpcUaWriteResult::setAttribute(QOpcUa::NodeAttribute attribute)
{
    data->attribute = attribute;
}

/*!
    Returns the index range of the write result.
*/
QString QOpcUaWriteResult::indexRange() const
{
    return data->indexRange;
}

/*!
    Sets the index range of the write result to \a indexRange.
*/
void QOpcUaWriteResult::setIndexRange(const QString &indexRange)
{
    data->indexRange = indexRange;
}

/*!
    Returns the status code of the write result.
*/
QOpcUa::UaStatusCode QOpcUaWriteResult::statusCode() const
{
    return data->statusCode;
}

/*!
    Sets the status code of the write result to \a statusCode.
*/
void QOpcUaWriteResult::setStatusCode(QOpcUa::UaStatusCode statusCode)
{
    data->statusCode = statusCode;
}

QT_END_NAMESPACE

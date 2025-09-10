// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuareadresult.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaReadResult
    \inmodule QtOpcUa
    \brief This class stores the result of a read operation.

    A read operation on an OPC UA server returns the value and timestamps which describe when a value was generated
    by the source and when the server obtained it. It also returns a status code which describes if the value could
    be read and if not, for what reason the read has failed.

    In addition to the data returned by the server, this class also contains the node id, the attribute and the index
    range from the request to enable a client to match the result with a request.

    Objects of this class are returned in the \l QOpcUaClient::readNodeAttributesFinished()
    signal and contain the result of a read operation that was part of a \l QOpcUaClient::readNodeAttributes()
    request.

    \sa QOpcUaClient::readNodeAttributes() QOpcUaClient::readNodeAttributesFinished() QOpcUaReadItem
*/
class QOpcUaReadResultData : public QSharedData
{
public:
    QDateTime serverTimestamp;
    QDateTime sourceTimestamp;
    QOpcUa::UaStatusCode statusCode {QOpcUa::UaStatusCode::Good};
    QString nodeId;
    QOpcUa::NodeAttribute attribute {QOpcUa::NodeAttribute::Value};
    QString indexRange;
    QVariant value;
};

/*!
    Default constructs a read result with no parameters set.
*/
QOpcUaReadResult::QOpcUaReadResult()
    : data(new QOpcUaReadResultData)
{
}

/*!
    Constructs a read result from \a other.
*/
QOpcUaReadResult::QOpcUaReadResult(const QOpcUaReadResult &other)
    : data(other.data)
{
}

/*!
    Sets the values from \a rhs in this read result.
*/
QOpcUaReadResult &QOpcUaReadResult::operator=(const QOpcUaReadResult &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUaReadResult::~QOpcUaReadResult()
{
}

/*!
    Returns the value.
*/
QVariant QOpcUaReadResult::value() const
{
    return data->value;
}

/*!
    Sets the value to \a value.
*/
void QOpcUaReadResult::setValue(const QVariant &value)
{
    data->value = value;
}

/*!
    Returns the attribute id.
*/
QOpcUa::NodeAttribute QOpcUaReadResult::attribute() const
{
    return data->attribute;
}

/*!
    Sets the attribute id to \a attribute.
*/
void QOpcUaReadResult::setAttribute(QOpcUa::NodeAttribute attribute)
{
    data->attribute = attribute;
}

/*!
    Returns the index range.
*/
QString QOpcUaReadResult::indexRange() const
{
    return data->indexRange;
}

/*!
    Sets the index range to \a indexRange.
*/
void QOpcUaReadResult::setIndexRange(const QString &indexRange)
{
    data->indexRange = indexRange;
}

/*!
    Returns the node id.
*/
QString QOpcUaReadResult::nodeId() const
{
    return data->nodeId;
}

/*!
    Sets the node id to \a nodeId.
*/
void QOpcUaReadResult::setNodeId(const QString &nodeId)
{
    data->nodeId = nodeId;
}

/*!
    Returns the status code for this element. If the status code is not \l {QOpcUa::UaStatusCode} {Good}, the
    value and the timestamps are invalid.
*/
QOpcUa::UaStatusCode QOpcUaReadResult::statusCode() const
{
    return data->statusCode;
}

/*!
    Sets the status code to \a statusCode.
*/
void QOpcUaReadResult::setStatusCode(QOpcUa::UaStatusCode statusCode)
{
    data->statusCode = statusCode;
}

/*!
    Returns the source timestamp for \l value().
*/
QDateTime QOpcUaReadResult::sourceTimestamp() const
{
    return data->sourceTimestamp;
}

/*!
    Sets the source timestamp to \a sourceTimestamp.
*/
void QOpcUaReadResult::setSourceTimestamp(const QDateTime &sourceTimestamp)
{
    data->sourceTimestamp = sourceTimestamp;
}

/*!
    Returns the server timestamp for \l value().
*/
QDateTime QOpcUaReadResult::serverTimestamp() const
{
    return data->serverTimestamp;
}

/*!
    Sets the server timestamp to \a serverTimestamp.
*/
void QOpcUaReadResult::setServerTimestamp(const QDateTime &serverTimestamp)
{
    data->serverTimestamp = serverTimestamp;
}

QT_END_NAMESPACE

// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuawriteitem.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaWriteItem
    \inmodule QtOpcUa
    \brief This class stores the options for a write operation.

    A write operation on an OPC UA server overwrites the entire value or a certain index range of the value of an
    attribute of a node on the server. This class contains the necessary information for the backend to make
    a write request to the server.

    One or multiple objects of this class make up the request of a \l QOpcUaClient::writeNodeAttributes() operation.

    \sa QOpcUaClient::writeNodeAttributes() QOpcUaWriteResult
*/
class QOpcUaWriteItemData : public QSharedData
{
public:
    QString nodeId;
    QOpcUa::NodeAttribute attribute {QOpcUa::NodeAttribute::Value};
    QString indexRange;
    QVariant value;
    QOpcUa::Types type {QOpcUa::Types::Undefined};
    QDateTime sourceTimestamp;
    QDateTime serverTimestamp;
    QOpcUa::UaStatusCode statusCode {QOpcUa::UaStatusCode::Good};
    bool hasStatusCode {false};
};

/*!
    Default constructs a write item with no parameters set.
*/
QOpcUaWriteItem::QOpcUaWriteItem()
    : data(new QOpcUaWriteItemData)
{
}

/*!
    Creates a new write item from \a other.
*/
QOpcUaWriteItem::QOpcUaWriteItem(const QOpcUaWriteItem &other)
    : data(other.data)
{
}

/*!
    Creates a write item for the attribute \a attribute from node \a nodeId.
    The value \a value of type \a type will be written at position \a indexRange of \a attribute.
*/
QOpcUaWriteItem::QOpcUaWriteItem(const QString &nodeId, QOpcUa::NodeAttribute attribute,
                                 const QVariant &value, QOpcUa::Types type, const QString &indexRange)
    : data(new QOpcUaWriteItemData)
{
    setNodeId(nodeId);
    setAttribute(attribute);
    setValue(value);
    setType(type);
    setIndexRange(indexRange);
}

/*!
    Sets the values from \a rhs in this write item.
*/
QOpcUaWriteItem &QOpcUaWriteItem::operator=(const QOpcUaWriteItem &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUaWriteItem::~QOpcUaWriteItem()
{
}

/*!
    Returns the node id of the write item.
*/
QString QOpcUaWriteItem::nodeId() const
{
    return data->nodeId;
}

/*!
    Sets the node id of the write item to \a nodeId.
*/
void QOpcUaWriteItem::setNodeId(const QString &nodeId)
{
    data->nodeId = nodeId;
}

/*!
    Returns the attribute of the write item.
*/
QOpcUa::NodeAttribute QOpcUaWriteItem::attribute() const
{
    return data->attribute;
}

/*!
    Sets the attribute of the write item to \a attribute.
*/
void QOpcUaWriteItem::setAttribute(QOpcUa::NodeAttribute attribute)
{
    data->attribute = attribute;
}

/*!
    Returns the index range of the write item.
*/
QString QOpcUaWriteItem::indexRange() const
{
    return data->indexRange;
}

/*!
    Sets the index range of the write item to \a indexRange.
*/
void QOpcUaWriteItem::setIndexRange(const QString &indexRange)
{
    data->indexRange = indexRange;
}

/*!
    Returns the value of the write item.
*/
QVariant QOpcUaWriteItem::value() const
{
    return data->value;
}

/*!
    Sets the value of the write item to \a value.
    If given, the type information from \l setType() will be used in converting
    the value to a SDK specific data type.

    \sa setType()
*/
void QOpcUaWriteItem::setValue(const QVariant &value)
{
    data->value = value;
}

/*!
    Sets the value of the write item to \value and the type of the value to \a type.
*/
void QOpcUaWriteItem::setValue(const QVariant &value, QOpcUa::Types type)
{
    data->value = value;
    data->type = type;
}

/*!
    Returns the type of the value of the write item.
*/
QOpcUa::Types QOpcUaWriteItem::type() const
{
    return data->type;
}

/*!
    Sets the type of the value of the write item to \a type.
*/
void QOpcUaWriteItem::setType(QOpcUa::Types type)
{
    data->type = type;
}

/*!
    Returns the source timestamp for the value to write.
*/
QDateTime QOpcUaWriteItem::sourceTimestamp() const
{
    return data->sourceTimestamp;
}

/*!
    Sets the source timestamp for the value to write to \a sourceTimestamp.
    If the source timestamp is invalid, it is ignored by the client and not sent to the server.
    If the server doesn't support writing timestamps, the write operation for this item
    will fail with status code \l {QOpcUa::UaStatusCode} {BadWriteNotSupported}.
*/
void QOpcUaWriteItem::setSourceTimestamp(const QDateTime &sourceTimestamp)
{
    data->sourceTimestamp = sourceTimestamp;
}

/*!
    Returns the server timestamp for the value to write.
*/
QDateTime QOpcUaWriteItem::serverTimestamp() const
{
    return data->serverTimestamp;
}

/*!
    Sets the server timestamp for the value to write to \a serverTimestamp.
    If the server timestamp is invalid, it is ignored by the client and not sent to the server.
    If the server doesn't support writing timestamps, the write operation for this item
    will fail with status code \l {QOpcUa::UaStatusCode} {BadWriteNotSupported}.
*/
void QOpcUaWriteItem::setServerTimestamp(const QDateTime &serverTimestamp)
{
    data->serverTimestamp = serverTimestamp;
}

/*!
    Returns the status code for the value to write.
*/
QOpcUa::UaStatusCode QOpcUaWriteItem::statusCode() const
{
    return data->statusCode;
}

/*!
    Returns true if a status code for the value to write has been set.
*/
bool QOpcUaWriteItem::hasStatusCode() const
{
    return data->statusCode;
}

/*!
    Sets the status code for the value to write to \a statusCode.
    If no status code is set, no status code is sent to the server.
*/
void QOpcUaWriteItem::setStatusCode(QOpcUa::UaStatusCode statusCode)
{
    data->statusCode = statusCode;
    data->hasStatusCode = true;
}

QT_END_NAMESPACE

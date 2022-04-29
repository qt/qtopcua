/****************************************************************************
**
** Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#include "qopcuareaditem.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaReadItem
    \inmodule QtOpcUa
    \brief This class stores the options for a read operation.

    A read operation on an OPC UA server returns the entire value or a certain index range of the value of an
    attribute of a node on the server. This class contains the necessary information for the backend to make
    a read request to the server.

    One or multiple objects of this class make up the request of a \l QOpcUaClient::readNodeAttributes() operation.

    \sa QOpcUaClient::readNodeAttributes() QOpcUaReadResult
*/

class QOpcUaReadItemData : public QSharedData
{
public:
    QString nodeId;
    QOpcUa::NodeAttribute attribute {QOpcUa::NodeAttribute::Value};
    QString indexRange;
};

QOpcUaReadItem::QOpcUaReadItem()
    : data(new QOpcUaReadItemData)
{

}

/*!
    Constructs a read item from \a other.
*/
QOpcUaReadItem::QOpcUaReadItem(const QOpcUaReadItem &other)
    : data(other.data)
{
}

/*!
    Constructs a read item for the index range \a indexRange of the attribute \a attr of node \a nodeId.
*/
QOpcUaReadItem::QOpcUaReadItem(const QString &nodeId, QOpcUa::NodeAttribute attr, const QString &indexRange)
    : data(new QOpcUaReadItemData)
{
    setNodeId(nodeId);
    setAttribute(attr);
    setIndexRange(indexRange);
}

/*!
    Sets the values from \a rhs in this read item.
*/
QOpcUaReadItem &QOpcUaReadItem::operator=(const QOpcUaReadItem &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUaReadItem::~QOpcUaReadItem()
{
}

/*!
    Returns the index range.
*/
QString QOpcUaReadItem::indexRange() const
{
    return data->indexRange;
}

/*!
    Sets the index range to \a indexRange.
*/
void QOpcUaReadItem::setIndexRange(const QString &indexRange)
{
    data->indexRange = indexRange;
}

/*!
    Returns the node attribute id.
*/
QOpcUa::NodeAttribute QOpcUaReadItem::attribute() const
{
    return data->attribute;
}

/*!
    Sets the node attribute id to \a attribute.
*/
void QOpcUaReadItem::setAttribute(QOpcUa::NodeAttribute attribute)
{
    data->attribute = attribute;
}

/*!
    Returns the node id.
*/
QString QOpcUaReadItem::nodeId() const
{
    return data->nodeId;
}

/*!
    Sets the node id to \a nodeId.
*/
void QOpcUaReadItem::setNodeId(const QString &nodeId)
{
    data->nodeId = nodeId;
}

QT_END_NAMESPACE

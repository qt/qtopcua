// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuaattributeoperand.h"
#include "qopcuarelativepathelement.h"
#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaAttributeOperand
    \inmodule QtOpcUa
    \brief The OPC UA AttributeOperand type.

    The AttributeOperand is defined in OPC-UA part 4, 7.4.4.4.
    It has the same purpose as \l QOpcUaSimpleAttributeOperand but has more configurable options.
*/

class QOpcUaAttributeOperandData : public QSharedData
{
public:
    QString nodeId;
    QString alias;
    QList<QOpcUaRelativePathElement> browsePath;
    QOpcUa::NodeAttribute attributeId {QOpcUa::NodeAttribute::Value};
    QString indexRange;
};

/*!
    Default constructs an attribute operand with no parameters set.
*/
QOpcUaAttributeOperand::QOpcUaAttributeOperand()
    : data(new QOpcUaAttributeOperandData)
{
}

/*!
    Constructs an attribute operand from \a rhs.
*/
QOpcUaAttributeOperand::QOpcUaAttributeOperand(const QOpcUaAttributeOperand &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this attribute operand.
*/
QOpcUaAttributeOperand &QOpcUaAttributeOperand::operator=(const QOpcUaAttributeOperand &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Converts this attribute operand to \l QVariant.
*/
QOpcUaAttributeOperand::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QOpcUaAttributeOperand::~QOpcUaAttributeOperand()
{
}

/*!
    Returns the index range string.
*/
QString QOpcUaAttributeOperand::indexRange() const
{
    return data->indexRange;
}

/*!
    Sets the index range string used to identify a single value or subset of the attribute's value to \a indexRange.
*/
void QOpcUaAttributeOperand::setIndexRange(const QString &indexRange)
{
    data->indexRange = indexRange;
}

/*!
    Returns the attribute id for an attribute of the node \l browsePath is pointing to.
*/
QOpcUa::NodeAttribute QOpcUaAttributeOperand::attributeId() const
{
    return data->attributeId;
}

/*!
    Sets the attribute id to \a attributeId.
*/
void QOpcUaAttributeOperand::setAttributeId(QOpcUa::NodeAttribute attributeId)
{
    data->attributeId = attributeId;
}

/*!
    Returns the browse path.
*/
QList<QOpcUaRelativePathElement> QOpcUaAttributeOperand::browsePath() const
{
    return data->browsePath;
}

/*!
    Returns a reference to the browse path.

    \sa browsePath()
*/
QList<QOpcUaRelativePathElement> &QOpcUaAttributeOperand::browsePathRef()
{
    return data->browsePath;
}

/*!
    Sets the relative path to a node starting from \l nodeId() to \a browsePath.
*/
void QOpcUaAttributeOperand::setBrowsePath(const QList<QOpcUaRelativePathElement> &browsePath)
{
    data->browsePath = browsePath;
}

/*!
    Returns the alias for this QAttributeOperand.
*/
QString QOpcUaAttributeOperand::alias() const
{
    return data->alias;
}

/*!
    Sets the alias to \a alias. This allows using this instance
    as operand for other operations in the filter.
*/
void QOpcUaAttributeOperand::setAlias(const QString &alias)
{
    data->alias = alias;
}

/*!
    Returns the node id of the type definition node.
*/
QString QOpcUaAttributeOperand::nodeId() const
{
    return data->nodeId;
}

/*!
    Sets the node id of the type definition node to \a nodeId.
*/
void QOpcUaAttributeOperand::setNodeId(const QString &nodeId)
{
    data->nodeId = nodeId;
}

QT_END_NAMESPACE

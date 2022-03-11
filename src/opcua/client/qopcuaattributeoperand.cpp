/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
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

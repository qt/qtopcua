/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qopcuacontentfilterelement.h"
#include "qopcuasimpleattributeoperand.h"
#include "qopcuaattributeoperand.h"
#include "qopcualiteraloperand.h"
#include "qopcuaelementoperand.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaContentFilterElement
    \inmodule QtOpcUa
    \brief The OPC UA ContentFilterElement.

    A content filter element contains an operator and a list of operands.
    There are four different operator types which contain literal values, references to
    attributes of nodes or to other content filter elements.

    A combination of one or more content filter elements makes a content filter which is used
    by the server to filter data for the criteria defined by the content filter elements.
    For example, the \c where clause of an event filter is a content filter which is used to decide
    if a notification is generated for an event.
*/

/*!
    \enum QOpcUaContentFilterElement::FilterOperator

    FilterOperator enumerates all possible operators for a ContentFilterElement that are specified in
    OPC-UA part 4, Tables 115 and 116.

    \value Equals
    \value IsNull
    \value GreaterThan
    \value LessThan
    \value GreaterThanOrEqual
    \value LessThanOrEqual
    \value Like
    \value Not
    \value Between
    \value InList
    \value And
    \value Or
    \value Cast
    \value InView
    \value OfType
    \value RelatedTo
    \value BitwiseAnd
    \value BitwiseOr
*/

class QOpcUaContentFilterElementData : public QSharedData
{
public:
    QOpcUaContentFilterElement::FilterOperator filterOperator;
    QVector<QVariant> filterOperands;
};

QOpcUaContentFilterElement::QOpcUaContentFilterElement()
    : data(new QOpcUaContentFilterElementData)
{
}

/*!
    Constructs a content filter element from \a rhs.
*/
QOpcUaContentFilterElement::QOpcUaContentFilterElement(const QOpcUaContentFilterElement &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this content filter element.
*/
QOpcUaContentFilterElement &QOpcUaContentFilterElement::operator=(const QOpcUaContentFilterElement &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this content filter element has the same value as \a rhs.
*/
bool QOpcUaContentFilterElement::operator==(const QOpcUaContentFilterElement &rhs) const
{
    return filterOperator() == rhs.filterOperator() && filterOperands() == rhs.filterOperands();
}

/*!
    Converts this content filter element to \l QVariant.
*/
QOpcUaContentFilterElement::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
    Returns the operands of the filter element.
*/
QVector<QVariant> QOpcUaContentFilterElement::filterOperands() const
{
    return data->filterOperands;
}

/*!
    Returns a reference to the filter operands.

    \sa filterOperands()
*/
QVector<QVariant> &QOpcUaContentFilterElement::filterOperandsRef()
{
   return data->filterOperands;
}

/*!
    Sets the filter operands for this content filter element to \a filterOperands.
    Supported classes are \l QOpcUaElementOperand, \l QOpcUaLiteralOperand,
    \l QOpcUaSimpleAttributeOperand and \l QOpcUaAttributeOperand.
*/
void QOpcUaContentFilterElement::setFilterOperands(const QVector<QVariant> &filterOperands)
{
    data->filterOperands = filterOperands;
}

/*!
    Returns the filter operator.
*/
QOpcUaContentFilterElement::FilterOperator QOpcUaContentFilterElement::filterOperator() const
{
    return data->filterOperator;
}

/*!
    Sets the operator that is applied to the filter operands to \a filterOperator.
*/
void QOpcUaContentFilterElement::setFilterOperator(QOpcUaContentFilterElement::FilterOperator filterOperator)
{
    data->filterOperator = filterOperator;
}

QOpcUaContentFilterElement::~QOpcUaContentFilterElement()
{
}

/*!
    Sets filter operator \a op in this content filter element.
    If multiple operators are streamed into one content filter element, only the last operator is used.
    All others are discarded.
*/
QOpcUaContentFilterElement &QOpcUaContentFilterElement::operator<<(QOpcUaContentFilterElement::FilterOperator op)
{
    setFilterOperator(op);
    return *this;
}

/*!
    Adds the simple attribute operand \a op to the operands list of this content filter element.
*/
QOpcUaContentFilterElement &QOpcUaContentFilterElement::operator<<(const QOpcUaSimpleAttributeOperand &op)
{
    filterOperandsRef().append(op);
    return *this;
}

/*!
    Adds the attribute operand \a op to the operands list of this content filter element.
*/
QOpcUaContentFilterElement &QOpcUaContentFilterElement::operator<<(const QOpcUaAttributeOperand &op)
{
    filterOperandsRef().append(op);
    return *this;
}

/*!
    Adds the literal operand \a op to the operands list of this content filter element.
*/
QOpcUaContentFilterElement &QOpcUaContentFilterElement::operator<<(const QOpcUaLiteralOperand &op)
{
    filterOperandsRef().append(op);
    return *this;
}

/*!
    Adds the element operand \a op to the operands list of this content filter element.
*/
QOpcUaContentFilterElement &QOpcUaContentFilterElement::operator<<(const QOpcUaElementOperand &op)
{
    filterOperandsRef().append(op);
    return *this;
}

QT_END_NAMESPACE

// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
    OPC UA 1.05 part 4, 7.7.3.

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
    QList<QVariant> filterOperands;
};

/*!
    Default constructs a content filter element with no parameters set.
*/
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

QOpcUaContentFilterElement::~QOpcUaContentFilterElement() = default;

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
    \fn bool QOpcUaContentFilterElement::operator==(const QOpcUaContentFilterElement &lhs,
                                                    const QOpcUaContentFilterElement &rhs)

    Returns \c true if \a lhs has the same value as \a rhs.
*/
bool comparesEqual(const QOpcUaContentFilterElement &lhs,
                   const QOpcUaContentFilterElement &rhs) noexcept
{
    return lhs.filterOperator() == rhs.filterOperator()
            && lhs.filterOperands() == rhs.filterOperands();
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
QList<QVariant> QOpcUaContentFilterElement::filterOperands() const
{
    return data->filterOperands;
}

/*!
    Returns a reference to the filter operands.

    \sa filterOperands()
*/
QList<QVariant> &QOpcUaContentFilterElement::filterOperandsRef()
{
   return data->filterOperands;
}

/*!
    Sets the filter operands for this content filter element to \a filterOperands.
    Supported classes are \l QOpcUaElementOperand, \l QOpcUaLiteralOperand,
    \l QOpcUaSimpleAttributeOperand and \l QOpcUaAttributeOperand.
*/
void QOpcUaContentFilterElement::setFilterOperands(const QList<QVariant> &filterOperands)
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

/*!
    \fn bool QOpcUaContentFilterElement::operator!=(const QOpcUaContentFilterElement &lhs,
                                                    const QOpcUaContentFilterElement &rhs)
    \since 6.7

    Returns \c true if \a lhs has a different value than \a rhs.
*/

QT_END_NAMESPACE

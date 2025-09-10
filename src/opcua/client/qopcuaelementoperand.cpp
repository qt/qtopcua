// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuaelementoperand.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaElementOperand
    \inmodule QtOpcUa
    \brief The OPC UA ElementOperand type.

    The ElementOperand is defined in OPC UA 1.05 part 4, 7.7.4.2.
    It is used to identify another element in the filter by its index
    (the first element has the index 0).

    This is required to create complex filters, for example to reference
    the two operands of the AND operation in ((Severity > 500) AND (Message == "TestString")).
    The first step is to create content filter elements for the two conditions (Severity > 500)
    and (Message == "TestString"). A third content filter element is required to create an AND
    combination of the two conditions. It consists of the AND operator and two element operands
    with the indices of the two conditions created before:

    \code
    QOpcUaMonitoringParameters::EventFilter filter;
    ...
    // setup select clauses
    ...
    QOpcUaContentFilterElement condition1;
    QOpcUaContentFilterElement condition2;
    QOpcUaContentFilterElement condition3;
    condition1 << QOpcUaContentFilterElement::FilterOperator::GreaterThan << QOpcUaSimpleAttributeOperand("Severity") <<
                    QOpcUaLiteralOperand(quint16(500), QOpcUa::Types::UInt16);
    condition2 << QOpcUaContentFilterElement::FilterOperator::Equals << QOpcUaSimpleAttributeOperand("Message") <<
                    QOpcUaLiteralOperand("TestString", QOpcUa::Types::String);
    condition3 << QOpcUaContentFilterElement::FilterOperator::And << QOpcUaElementOperand(0) << QOpcUaElementOperand(1);
    filter << condition1 << condition2 << condition3;
    \endcode
*/

class QOpcUaElementOperandData : public QSharedData
{
public:
    quint32 index{0};
};

/*!
    Default constructs an element operand with no parameters set.
*/
QOpcUaElementOperand::QOpcUaElementOperand()
    : data(new QOpcUaElementOperandData)
{
}

/*!
    Constructs an element operand from \a rhs.
*/
QOpcUaElementOperand::QOpcUaElementOperand(const QOpcUaElementOperand &rhs)
    : data(rhs.data)
{
}

/*!
    Constructs an element operand with index \a index.
*/
QOpcUaElementOperand::QOpcUaElementOperand(quint32 index)
    : data(new QOpcUaElementOperandData)
{
    setIndex(index);
}

/*!
    Sets the values from \a rhs in this element operand.
*/
QOpcUaElementOperand &QOpcUaElementOperand::operator=(const QOpcUaElementOperand &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Converts this element operand to \l QVariant.
*/
QOpcUaElementOperand::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QOpcUaElementOperand::~QOpcUaElementOperand()
{
}

/*!
    Returns the index of the filter element that is going to be used as operand.
*/
quint32 QOpcUaElementOperand::index() const
{
    return data->index;
}

/*!
    Sets the index of the filter element that is going to be used as operand to \a index.
*/
void QOpcUaElementOperand::setIndex(quint32 index)
{
    data->index = index;
}

/*!
    \fn bool QOpcUaElementOperand::operator==(const QOpcUaElementOperand &lhs,
                                              const QOpcUaElementOperand &rhs)
    \since 6.7

    Returns \c true if \a lhs has the same value as \a rhs.
*/
bool comparesEqual(const QOpcUaElementOperand &lhs, const QOpcUaElementOperand &rhs) noexcept
{
    return lhs.index() == rhs.index();
}

/*!
    \fn bool QOpcUaElementOperand::operator!=(const QOpcUaElementOperand &lhs,
                                              const QOpcUaElementOperand &rhs)
    \since 6.7

    Returns \c true if \a lhs a different value than \a rhs.
*/

QT_END_NAMESPACE

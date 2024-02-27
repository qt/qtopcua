// Copyright (C) 2019 The Qt Company Ltd.
// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcualiteraloperand.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaLiteralOperand
    \inmodule QtOpcUa
    \brief The OPC UA LiteralOperand type.

    The LiteralOperand is defined in OPC UA 1.05 part 4, 7.7.4.3.
    It contains a literal value that is to be used as operand.
*/
class QOpcUaLiteralOperandData : public QSharedData
{
public:
    QVariant value;
    QOpcUa::Types type {QOpcUa::Types::Undefined};
};

/*!
    Default constructs a literal operand with no parameters set.
*/
QOpcUaLiteralOperand::QOpcUaLiteralOperand()
    : data(new QOpcUaLiteralOperandData)
{
    data->type = QOpcUa::Types::Undefined;
}

/*!
    Constructs a literal operand from \a rhs.
*/
QOpcUaLiteralOperand::QOpcUaLiteralOperand(const QOpcUaLiteralOperand &rhs)
    : data(rhs.data)
{
}

/*!
    Constructs a literal operand of value \a value and type \a type.
*/
QOpcUaLiteralOperand::QOpcUaLiteralOperand(const QVariant &value, QOpcUa::Types type)
    : data(new QOpcUaLiteralOperandData)
{
    setValue(value);
    setType(type);
}

/*!
    Sets the values from \a rhs in this \l QOpcUaLiteralOperand.
*/
QOpcUaLiteralOperand &QOpcUaLiteralOperand::operator=(const QOpcUaLiteralOperand &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Converts this literal operand to \l QVariant.
*/
QOpcUaLiteralOperand::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QOpcUaLiteralOperand::~QOpcUaLiteralOperand()
{
}

/*!
    Returns the type of the value of the literal operand.
*/
QOpcUa::Types QOpcUaLiteralOperand::type() const
{
    return data->type;
}

/*!
    Sets the type of the value of the literal operand to \a type.
*/
void QOpcUaLiteralOperand::setType(QOpcUa::Types type)
{
    data->type = type;
}

/*!
    Returns the value of the literal operand.
*/
QVariant QOpcUaLiteralOperand::value() const
{
    return data->value;
}

/*!
    Sets the value of the literal operand to \a value.
*/
void QOpcUaLiteralOperand::setValue(const QVariant &value)
{
    data->value = value;
}

/*!
    \fn bool QOpcUaLiteralOperand::operator==(const QOpcUaLiteralOperand &lhs,
                                              const QOpcUaLiteralOperand &rhs)
    \since 6.7

    Returns \c true if \a lhs has the same value as \a rhs.
*/
bool comparesEqual(const QOpcUaLiteralOperand &lhs, const QOpcUaLiteralOperand &rhs) noexcept
{
    return lhs.value() == rhs.value() && lhs.type() == rhs.type();
}

/*!
    \fn bool QOpcUaLiteralOperand::operator!=(const QOpcUaLiteralOperand &lhs,
                                              const QOpcUaLiteralOperand &rhs)
    \since 6.7

    Returns \c true if \a lhs has a different value than \a rhs.
*/

QT_END_NAMESPACE

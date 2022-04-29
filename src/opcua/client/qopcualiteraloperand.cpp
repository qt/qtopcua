/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
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

#include "qopcualiteraloperand.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaLiteralOperand
    \inmodule QtOpcUa
    \brief The OPC UA LiteralOperand type.

    The LiteralOperand is defined in OPC-UA part 4, 7.4.4.3.
    It contains a literal value that is to be used as operand.
*/
class QOpcUaLiteralOperandData : public QSharedData
{
public:
    QVariant value;
    QOpcUa::Types type {QOpcUa::Types::Undefined};
};

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

QT_END_NAMESPACE

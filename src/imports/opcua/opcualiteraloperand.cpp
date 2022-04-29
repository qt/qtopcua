/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#include "opcualiteraloperand.h"

QT_BEGIN_NAMESPACE

/*!
    \qmltype LiteralOperand
    \inqmlmodule QtOpcUa
    \brief The OPC UA LiteralOperand type.
    \since QtOpcUa 5.13

    The LiteralOperand is defined in OPC-UA part 4, 7.4.4.3.
    It contains a literal value that is to be used as operand for filters.

    Setting the type should be done to match the expected types on the server.
    Otherwise the type will be guessed and may lead to errors because it does
    not match on the server.

    \code
    import QtOpcUa 5.13 as QtOpcUa

    QtOpcUa.LiteralOperand {
        value: 43.21
        type: QtOpcUa.Constants.Double
    }
    \endcode
*/

/*!
    \qmlproperty QOpcUa.Types LiteralOperand::type

    Type of the value of the literal operand.
*/

/*!
    \qmlproperty variant LiteralOperand::value

    Value of the literal operand.
*/

OpcUaLiteralOperand::OpcUaLiteralOperand(QObject *parent)
    : OpcUaOperandBase(parent)
    , m_type(QOpcUa::Undefined)
{
}

OpcUaLiteralOperand::~OpcUaLiteralOperand() = default;

QVariant OpcUaLiteralOperand::toCppVariant(QOpcUaClient *client) const
{
    Q_UNUSED(client);
    return QOpcUaLiteralOperand(m_value, m_type);
}

QVariant OpcUaLiteralOperand::value() const
{
    return m_value;
}

void OpcUaLiteralOperand::setValue(const QVariant &value)
{
    if (m_value != value) {
        m_value = value;
        emit dataChanged();
    }
}

QOpcUa::Types OpcUaLiteralOperand::type() const
{
    return m_type;
}

void OpcUaLiteralOperand::setType(QOpcUa::Types type)
{
    if (m_type != type) {
        m_type = type;
        emit dataChanged();
    }
}

QT_END_NAMESPACE

// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/opcualiteraloperand_p.h>

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
    import QtOpcUa as QtOpcUa

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

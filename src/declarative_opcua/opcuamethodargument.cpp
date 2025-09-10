// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/opcuamethodargument_p.h>

/*!
    \qmltype MethodArgument
    \inqmlmodule QtOpcUa
    \brief Arguments for OpcUa method calls.
    \since QtOpcUa 5.13

    When calling methods which require arguments, this type is used.

    This example shows how to call a method with two double arguments.
    \code
    QtOpcUa.MethodNode {
        ...
        inputArguments: [
            QtOpcUa.MethodArgument {
                value: 3
                type: QtOpcUa.Constants.Double
            },
            QtOpcUa.MethodArgument {
                value: 4
                type: QtOpcUa.Constants.Double
            }
        ]
    }
    \endcode
*/

/*!
    \qmlproperty variant MethodArgument::value

    The value of the argument.
*/

/*!
    \qmlproperty QOpcUa::Types MethodNode::type

    Sets the type of the argument that is expected by the server.
    The value variant is converted to that type when calling the method.
    The type has to match the method on the server exactly, otherwise
    the method call will fail.

    \sa MethodNode::callMethod
*/

OpcUaMethodArgument::OpcUaMethodArgument(QObject *parent) : QObject(parent)
{
}

QVariant OpcUaMethodArgument::value() const
{
    return m_value;
}

QOpcUa::Types OpcUaMethodArgument::type() const
{
    return m_type;
}

void OpcUaMethodArgument::setValue(QVariant value)
{
    m_value = value;
}

void OpcUaMethodArgument::setType(QOpcUa::Types type)
{
    m_type = type;
}

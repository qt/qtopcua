// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/opcuanodeid_p.h>

QT_BEGIN_NAMESPACE

/*!
    \qmltype NodeId
    \inqmlmodule QtOpcUa
    \brief Specifies a node by an identifier.
    \since QtOpcUa 5.12

    \code
    import QtOpcUa as QtOpcUa

    QtOpcUa.NodeId {
        identifier: "s=Example.Node"
        ns: "Example Namespace"
    }
    \endcode
*/

/*!
    \qmlproperty string NodeId::ns

    Namespace of the node identifier.
    The identifier can be the index as a number or the name as string.
    A string which can be converted to an integer is considered a namespace index.
*/

/*!
    \qmlproperty string NodeId::identifier

    Identifier of the node.
    The identifier has to be given in one of the following types.

    \table
    \header
        \li Type
        \li Example
    \row
        \li Numeric
        \li \c i=23
    \row
        \li String
        \li \c s=MyStringIdentifier
    \row
        \li GUID
        \li \c g=08081e75-8e5e-319b-954f-f3a7613dc29b
    \row
        \li Opaque (base64)
        \li \c b=UXQgZnR3IQ==
    \endtable

    It is possible but not recommended to include the namespace index \c {ns=X;s=...}.
    In this case the given namespace index is internally stripped off the identifier and set
    to the namespace property.
*/

/*!
    \qmlsignal NodeId::nodeChanged()

    Emitted when the underlying node has changed.
    This happens when the namespace or identifier has changed.
*/

OpcUaNodeId::OpcUaNodeId(QObject *parent)
    : OpcUaNodeIdType(parent)
{
    connect(this, &OpcUaNodeIdType::nodeNamespaceChanged, this, &OpcUaNodeId::nodeNamespaceChanged);
    connect(this, &OpcUaNodeIdType::identifierChanged, this, &OpcUaNodeId::identifierChanged);
    connect(this, &OpcUaNodeIdType::nodeNamespaceChanged, this, &OpcUaNodeId::nodeChanged);
    connect(this, &OpcUaNodeIdType::identifierChanged, this, &OpcUaNodeId::nodeChanged);
}

QT_END_NAMESPACE

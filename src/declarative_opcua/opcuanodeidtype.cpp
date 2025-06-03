// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/opcuanodeidtype_p.h>

QT_BEGIN_NAMESPACE

/*!
    \qmltype OpcUaNodeIdType
    \inqmlmodule QtOpcUa
    \brief Common base class for all node IDs.
    \deprecated [6.9]
    \internal

    This base class provides common functions for node ID related classes.
    It is not supposed to be used from QML directly.

    \sa NodeId, RelativeNodeId
*/

OpcUaNodeIdType::OpcUaNodeIdType(QObject *parent) : QObject(parent)
{
    connect(&m_universalNode, SIGNAL(namespaceNameChanged(QString)),
            this, SIGNAL(nodeNamespaceChanged(QString)));
    connect(&m_universalNode, SIGNAL(nodeIdentifierChanged(QString)),
            this, SIGNAL(identifierChanged(QString)));
    connect(&m_universalNode, SIGNAL(namespaceNameChanged(QString)),
            this, SIGNAL(nodeChanged()));
    connect(&m_universalNode, SIGNAL(nodeIdentifierChanged(QString)),
            this, SIGNAL(nodeChanged()));
}

void OpcUaNodeIdType::setNodeNamespace(const QString &name)
{
    m_universalNode.setNamespace(name);
}

const QString &OpcUaNodeIdType::nodeNamespace() const
{
    return m_universalNode.namespaceName();
}

void OpcUaNodeIdType::setIdentifier(const QString &name)
{
    m_universalNode.setNodeIdentifier(name);
}

void OpcUaNodeIdType::from(const OpcUaNodeIdType &other)
{
    m_universalNode.from(other);
}

void OpcUaNodeIdType::from(const UniversalNode &other)
{
    m_universalNode.from(other);
}

const QString &OpcUaNodeIdType::identifier() const
{
    return m_universalNode.nodeIdentifier();
}

QString OpcUaNodeIdType::fullNodePath() const
{
    return m_universalNode.fullNodeId();
}

QT_END_NAMESPACE

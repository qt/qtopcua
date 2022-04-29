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

#include "opcuanodeidtype.h"

QT_BEGIN_NAMESPACE

/*!
    \qmltype OpcUaNodeIdType
    \inqmlmodule QtOpcUa
    \brief Common base class for all node IDs.
    \internal

    This base class provides common functions for node ID related classes.
    It is not supposed to be used from QML directly.

    \sa NodeId, RelativeNodeId
*/

OpcUaNodeIdType::OpcUaNodeIdType(QObject *parent) : QObject(parent)
{
    connect(&m_universalNode, SIGNAL(namespaceNameChanged(const QString &)), this, SIGNAL(nodeNamespaceChanged(const QString &)));
    connect(&m_universalNode, SIGNAL(nodeIdentifierChanged(const QString &)), this, SIGNAL(identifierChanged(const QString &)));
    connect(&m_universalNode, SIGNAL(namespaceNameChanged(const QString &)), this, SIGNAL(nodeChanged()));
    connect(&m_universalNode, SIGNAL(nodeIdentifierChanged(const QString &)), this, SIGNAL(nodeChanged()));
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

/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <private/opcuanodeidtype_p.h>

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

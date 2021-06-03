/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <private/opcuarelativenodeid_p.h>
#include <private/opcuanodeid_p.h>
#include <private/opcuarelativenodepath_p.h>

QT_BEGIN_NAMESPACE

/*!
    \qmltype RelativeNodeId
    \inqmlmodule QtOpcUa
    \brief Specifies a relative node by a start node and a path.
    \since QtOpcUa 5.12

    Proceeding from the given start node, the server tries to match the content
    of the path property to any existing nodes on the server. Only if the path fully
    matches to exactly one single target node, it will be used. Partial or multiple
    matches can not be handled and are treated as error.

    \code
    import QtOpcUa 5.13 as QtOpcUa

    QtOpcUa.RelativeNodeId {
        startNode: QtOpcUa.RelativeNodeId {
                identifier: "s=Example.Node"
                ns: "Example Namespace"
             }
        path: [ QtOpcUa.RelativeNodePath {
                    ns: "Test Namespace"
                    browseName: "SomeName"
                }
                ...
            ]
        }
    }
    \endcode

    \sa NodeId, Node
*/

/*!
    \qmlproperty NodeId RelativeNodeId::startNode

    Namespace of the node identifier.
    The start node specifies the node where resolving the path starts.
    It can be an absolute or relative node ID.

    \sa NodeId, RelativeNodeId
*/

/*!
    \qmlproperty list RelativeNodeId::path

    Search path for the designated node.
    This is a list of \c OpcUaRelativeNodePath objects specifying the path where
    the target node can be found relative to the start node.
*/

/*!
    \qmlsignal Relative::nodeChanged()

    Emitted when the underlying node has changed.
    This happens when the namespace or identifier has changed.
*/

OpcUaRelativeNodeId::OpcUaRelativeNodeId(QObject *parent) : OpcUaNodeIdType(parent)
{
}

OpcUaNodeIdType *OpcUaRelativeNodeId::startNode() const
{
    return m_startNode;
}

void OpcUaRelativeNodeId::setStartNode(OpcUaNodeIdType *startNode)
{
    if (m_startNode == startNode)
        return;

    if (m_startNode)
        disconnect(m_startNode);

    m_startNode = startNode;
    connect(m_startNode, &OpcUaNodeIdType::nodeChanged, this, &OpcUaRelativeNodeId::nodeChanged);

    emit startNodeChanged(m_startNode);
    emit nodeChanged();
}

QQmlListProperty<OpcUaRelativeNodePath> OpcUaRelativeNodeId::paths()
{
    return QQmlListProperty<OpcUaRelativeNodePath>(this, this,
             &OpcUaRelativeNodeId::appendPath,
             &OpcUaRelativeNodeId::pathCount,
             &OpcUaRelativeNodeId::path,
             &OpcUaRelativeNodeId::clearPaths);
}

void OpcUaRelativeNodeId::appendPath(OpcUaRelativeNodePath* p) {
    if (!p)
        return;
    m_paths.append(p);
    emit pathChanged();
    emit nodeChanged();
}

int OpcUaRelativeNodeId::pathCount() const
{
    return m_paths.count();
}

OpcUaRelativeNodePath *OpcUaRelativeNodeId::path(int index) const
{
    return m_paths.at(index);
}

void OpcUaRelativeNodeId::clearPaths() {
    m_paths.clear();
    emit pathChanged();
    emit nodeChanged();
}

void OpcUaRelativeNodeId::appendPath(QQmlListProperty<OpcUaRelativeNodePath>* list, OpcUaRelativeNodePath* p) {
    reinterpret_cast< OpcUaRelativeNodeId* >(list->data)->appendPath(p);
}

void OpcUaRelativeNodeId::clearPaths(QQmlListProperty<OpcUaRelativeNodePath>* list) {
    reinterpret_cast< OpcUaRelativeNodeId* >(list->data)->clearPaths();
}

OpcUaRelativeNodePath* OpcUaRelativeNodeId::path(QQmlListProperty<OpcUaRelativeNodePath>* list, qsizetype i) {
    return reinterpret_cast< OpcUaRelativeNodeId* >(list->data)->path(i);
}

qsizetype OpcUaRelativeNodeId::pathCount(QQmlListProperty<OpcUaRelativeNodePath>* list) {
    return reinterpret_cast< OpcUaRelativeNodeId* >(list->data)->pathCount();
}

QT_END_NAMESPACE

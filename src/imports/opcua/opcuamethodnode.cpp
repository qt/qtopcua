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

#include "opcuamethodnode.h"
#include "opcuanodeidtype.h"
#include <QOpcUaNode>
#include <QLoggingCategory>

QT_BEGIN_NAMESPACE

/*!
    \qmltype MethodNode
    \inqmlmodule QtOpcUa
    \brief Calls a method on the server.
    \since QtOpcUa 5.12
    \inherits Node

    This QML element supports calling method nodes on a server.
    The target method node has to be specified by the \l objectNodeId property.

    \code
    import QtOpcUa 5.12 as QtOpcUa

    QtOpcUa.MethodNode {
        nodeId : QtOpcUa.NodeId {
            identifier: "s=Example.Method"
            ns: "Example Namespace"
        }
        objectNodeId : QtOpcUa.NodeId {
            identifier: "s=Example.Method"
            ns: "Example Namespace"
        }
        connection: myConnection
     }
     \endcode

     The actual function call can be triggered by a signal.

     \code
     Button {
           text: "Start"
           onClicked: myNode.callMethod
     }
     \endcode

     or by JavaScript

     \code
          myNode.callMethod()
     \endcode
*/

/*!
    \qmlmethod MethodNode::callMethod

    Calls the method on the connected server.
*/

/*!
    \qmlproperty OpcUaNodeId MethodNode::objectNodeId

    Determines the actual node on which the method is called.
    It can be a relative or absolute node ID.
*/

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_QML)

OpcUaMethodNode::OpcUaMethodNode(QObject *parent):
    OpcUaNode(parent)
{
}

OpcUaNodeIdType *OpcUaMethodNode::objectNodeId() const
{
    return m_objectNodeId;
}

void OpcUaMethodNode::setObjectNodeId(OpcUaNodeIdType *nodeId)
{
    m_objectNodeId = nodeId;
    m_objectNodePath.clear();
    emit objectNodeIdChanged(nodeId);

    retrieveObjectNodePath();
}

void OpcUaMethodNode::callMethod()
{
    if (!m_objectNodeId || !m_node) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "No node or no object ID";
        return;
    }

    if (m_objectNodePath.isEmpty()) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "object node path is empty";
        return;
    }

    m_node->callMethod(m_objectNodePath, QVector<QOpcUa::TypedVariant> ());
}

void OpcUaMethodNode::objectNodePathResolved(const QString &str)
{
    m_objectNodePath = str;
}

void OpcUaMethodNode::setupNode(const QString &absolutePath)
{
    OpcUaNode::setupNode(absolutePath);
    retrieveObjectNodePath();
}

void OpcUaMethodNode::retrieveObjectNodePath()
{
    retrieveAbsoluteNodePath(m_objectNodeId, [this](const QString &str) {
        this->objectNodePathResolved(str);
    });
}

QT_END_NAMESPACE

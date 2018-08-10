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

#include "opcuanode.h"
#include "opcuanodeidtype.h"
#include "opcuaconnection.h"
#include "opcuanodeid.h"
#include "opcuarelativenodepath.h"
#include "opcuarelativenodeid.h"
#include "opcuapathresolver.h"
#include "opcuaattributevalue.h"
#include <QOpcUaNode>
#include <QOpcUaClient>
#include <QLoggingCategory>

QT_BEGIN_NAMESPACE

/*!
    \qmltype Node
    \inqmlmodule QtOpcUa
    \brief Represents a node on a server.
    \since QtOpcUa 5.12

    \code
    import QtOpcUa 5.12 as QtOpcUa

    QtOpcUa.Node {
        nodeId : QtOpcUa.NodeId {
            identifier: "s=Example.Method"
            ns: "Example Namespace"
        }
        connection: myConnection
    }
    \endcode
*/

/*!
    \qmlproperty NodeId Node::nodeId

    ID of the node on the server to be used.
    This can be an absolute node ID or a relative node ID.

    \sa NodeId, RelativeNodeId
*/

/*!
    \qmlproperty Connection Node::connection

    The connection to be used for node instances.
    The node will automatically be accessible when the associated connection
    has established a connection to a server.

    If this property is not set, the default connection will be used, if any.

    \sa Connection, Connection::defaultConnection
*/

/*!
    \qmlproperty bool Node::readyToUse
    \readonly

    This property returns whether the node is ready to use.
    This happens once after a successful connection to a server was established
    and the node was successfully set up.
*/

/*!
    \qmlsignal Connection::nodeChanged()

    Emitted when the underlying node has changed.
    This happens when the namespace or identifier of the \l NodeId changed.
*/

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_QML)

OpcUaNode::OpcUaNode(QObject *parent):
    QObject(parent),
    m_nodeId(new OpcUaNodeIdType(this))
{
    connect(&m_resolvedNode, &UniversalNode::nodeChanged, this, &OpcUaNode::nodeChanged);
}

OpcUaNode::~OpcUaNode()
{
    delete m_node;
}

OpcUaNodeIdType *OpcUaNode::nodeId() const
{
    return m_nodeId;
}

OpcUaConnection *OpcUaNode::connection()
{
    if (!m_connection)
        setConnection(OpcUaConnection::defaultConnection());

    return m_connection;
}

bool OpcUaNode::readyToUse() const
{
    return m_readyToUse;
}

void OpcUaNode::setNodeId(OpcUaNodeIdType *nodeId)
{
    if (m_nodeId == nodeId)
        return;

    // This deletes the initial dummy object that was created in the
    // constructor in case a "real" nodeId is set.
    if (m_nodeId->parent() == this)
        m_nodeId->deleteLater();

    // Disconnect signals from old node
    m_nodeId->disconnect(this);
    m_nodeId = nodeId;
    connect(m_nodeId, &OpcUaNodeIdType::nodeChanged, this, &OpcUaNode::updateNode);
    connect(m_nodeId, &OpcUaNodeIdType::destroyed, this, [&]() { m_nodeId = nullptr; });

    updateNode();
}

void OpcUaNode::setConnection(OpcUaConnection *connection)
{
    if (connection == m_connection)
        return;

    m_connection = connection;
    connect(connection, SIGNAL(connectedChanged()), this, SLOT(updateNode()));
    // Existing namespaces are not allowed to change during a connection
    // connect(connection, &OpcUaConnection::namespacesChanged, this, &OpcUaNode::updateNode);

    updateNode();
    emit connectionChanged(connection);
}

void OpcUaNode::setupNode(const QString &absoluteNodePath)
{
    m_absoluteNodePath = absoluteNodePath;

    if (m_node) {
        m_node->deleteLater();
        m_node = nullptr;
    }

    if (m_absoluteNodePath.isEmpty())
        return;

    auto conn = connection();
    if (!conn || !m_nodeId || !conn->m_client)
        return;

    if (!conn->connected())
        return;

    m_node = conn->m_client->node(m_absoluteNodePath);
    if (!m_node) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "Invalid node:" << m_absoluteNodePath;
        return;
    }

    connect(m_node, &QOpcUaNode::attributeUpdated, &m_attributeCache, &OpcUaAttributeCache::setAttributeValue);
}

void OpcUaNode::updateNode()
{
    retrieveAbsoluteNodePath(m_nodeId, [this](const QString &absoluteNodePath) {setupNode(absoluteNodePath);});
}

void OpcUaNode::retrieveAbsoluteNodePath(OpcUaNodeIdType *node, std::function<void (const QString &)> functor)
{
    auto conn = connection();
    if (!conn || !m_nodeId || !conn->m_client) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "connection, nodeID or client is invalid";
        return;
    }

    if (!conn->connected()) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "not connected";
        return;
    }

    if (qobject_cast<const OpcUaNodeId *>(node)) {
        UniversalNode tmp(node);
        tmp.resolveNamespace(conn->m_client);
        m_resolvedNode.from(tmp);
        functor(m_resolvedNode.fullNodeId());
        emit nodeIdChanged(m_nodeId);
        emit nodeChanged();
    } else if (qobject_cast<OpcUaRelativeNodeId *>(node)) {
        auto nodeId = qobject_cast<OpcUaRelativeNodeId *>(node);
        OpcUaPathResolver *resolver = new OpcUaPathResolver(nodeId, conn->m_client, this);
        connect(resolver, &OpcUaPathResolver::resolvedNode, this, [this, functor, resolver](UniversalNode nodeToUse, const QString &errorMessage) {
            resolver->deleteLater();

            if (!errorMessage.isEmpty()) {
                qCWarning(QT_OPCUA_PLUGINS_QML) << "Failed to resolve node:" << errorMessage;
                functor(QString());
                return;
            }

            m_resolvedNode.from(nodeToUse);
            functor(m_resolvedNode.fullNodeId());
            emit nodeIdChanged(m_nodeId);
            emit nodeChanged();
        });
        resolver->startResolving();
    } else {
        functor(QString());
    }
}

void OpcUaNode::setReadyToUse(bool value)
{
    bool old = m_readyToUse;
    m_readyToUse = value;
    if (!old && value)
        emit readyToUseChanged();
}

QT_END_NAMESPACE

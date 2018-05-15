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

#include "opcuavaluenode.h"
#include "opcuaconnection.h"
#include "opcuanodeid.h"
#include <QLoggingCategory>

QT_BEGIN_NAMESPACE

/*!
    \qmltype ValueNode
    \inqmlmodule QtOpcUa
    \brief Represents a value node from a server.
    \inherits Node
    \since QtOpcUa 5.12

    \code
    import QtOpcUa 5.12 as QtOpcUa

    QtOpcUa.ValueNode {
        nodeId: QtOpcUa.NodeId {
            ns: "Test Namespace"
            identifier: "s=TestName"
        }
        connection: myConnection
    }
    \endcode

    A subscription will be created on the server in order to track value changes on the server.

    \sa NodeId, Connection, Node
*/

/*!
    \qmlproperty variant ValueNode::value

    Value of this node.
    Reading and writing this property will access the node on the server.
*/

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_QML)

OpcUaValueNode::OpcUaValueNode(QObject *parent):
    OpcUaNode(parent)
{
}

OpcUaValueNode::~OpcUaValueNode()
{

}

void OpcUaValueNode::setValue(const QVariant &value)
{
    m_node->writeAttribute(QOpcUa::NodeAttribute::Value, value, QOpcUa::Types::Undefined);
}

void OpcUaValueNode::setupNode(const QString &absolutePath)
{
    OpcUaNode::setupNode(absolutePath);
    if (!m_node)
        return;

    connect(m_node, &QOpcUaNode::attributeRead, this, &OpcUaValueNode::handleAttributeUpdate);
    connect(m_node, &QOpcUaNode::attributeUpdated, this, &OpcUaValueNode::handleAttributeUpdated);

    if (!m_node->readAttributes(QOpcUa::NodeAttribute::Value
                            | QOpcUa::NodeAttribute::NodeClass
                            | QOpcUa::NodeAttribute::Description
                            | QOpcUa::NodeAttribute::DataType
                            | QOpcUa::NodeAttribute::BrowseName
                            | QOpcUa::NodeAttribute::DisplayName
                            ))
        qCWarning(QT_OPCUA_PLUGINS_QML) << "Reading attributes" << m_node->nodeId() << "failed";

    if (!m_node->enableMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters(100)))
        qCWarning(QT_OPCUA_PLUGINS_QML) << "Failed monitoring" << m_node->nodeId();
}

void OpcUaValueNode::handleAttributeUpdate(QOpcUa::NodeAttributes attrs)
{
    /*
     if (attr & QOpcUa::NodeAttribute::NodeClass)
         mNodeClass = mOpcNode->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>();
     if (attr & QOpcUa::NodeAttribute::BrowseName)
         mNodeBrowseName = mOpcNode->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUa::QQualifiedName>().name();
     if (attr & QOpcUa::NodeAttribute::DisplayName)
         mNodeDisplayName = mOpcNode->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUa::QLocalizedText>().text();
     if (attr & QOpcUa::NodeAttribute::NodeId)
         mNodeDisplayName = mOpcNode->attribute(QOpcUa::NodeAttribute::NodeId).toString();
         */

    if (attrs & QOpcUa::NodeAttribute::Value) {
        const auto value = m_node->attribute(QOpcUa::NodeAttribute::Value);
        if (value != m_cachedValue) {
            m_cachedValue = value;
            emit valueChanged(value);
        }
    }

    setReadyToUse();
}

void OpcUaValueNode::handleAttributeUpdated(QOpcUa::NodeAttribute attr, QVariant value)
{
    if (attr == QOpcUa::NodeAttribute::Value) {
        if (value != m_cachedValue) {
            m_cachedValue = value;
            emit valueChanged(value);
        }
    }
}

QVariant OpcUaValueNode::value() const
{
    if (!m_node)
        return QVariant();
    return m_node->attribute(QOpcUa::NodeAttribute::Value);
}

QT_END_NAMESPACE

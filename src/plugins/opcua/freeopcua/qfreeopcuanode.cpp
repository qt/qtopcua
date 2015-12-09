/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
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

#include "qfreeopcuanode.h"

#include "qfreeopcuaclient.h"
#include "qfreeopcuavalueconverter.h"

#include <QtCore/qdebug.h>

QFreeOpcUaNode::QFreeOpcUaNode(OpcUa::Node node, QFreeOpcUaClient *client)
    : QOpcUaNode(client)
    , m_node(node)
    , m_client(client) // FIXME: it seems this is only used for getType() at the moment, check if it is really needed
{
}

QFreeOpcUaNode::~QFreeOpcUaNode()
{

}

QString QFreeOpcUaNode::name() const
{
    try {
        return QString::fromStdString(m_node.GetAttribute(
                                          OpcUa::AttributeId::DisplayName).Value.As<OpcUa::LocalizedText>().Text);
    } catch (const std::exception &ex) {
        qWarning() << "Failed to get BrowseName for node: " << ex.what();
    }

    return QString();
}

QString QFreeOpcUaNode::type() const
{
    try {
        OpcUa::NodeId idNode = m_node.GetDataType().As<OpcUa::NodeId>();
        return QString::fromStdString(m_client->GetNode(idNode)
                                      .GetAttribute(OpcUa::AttributeId::DisplayName)
                                      .Value.As<OpcUa::LocalizedText>()
                                      .Text);
    } catch (const std::exception &ex) {
        qWarning() << ex.what();
        return QString();
    }
}

QVariant QFreeOpcUaNode::value() const
{
    try {
        return QFreeOpcUaValueConverter::toQVariant(m_node.GetValue());
    } catch (const std::exception &ex) {
        qWarning() << ex.what();
        return QVariant();
    }
}

int QFreeOpcUaNode::nodeId() const
{
    try {
        return m_node.GetId().GetIntegerIdentifier();
    } catch (const std::exception &ex) {
        qWarning() << "Failed to get numeric ID for node:" << ex.what();
        return -1;
    }
}

int QFreeOpcUaNode::childCount() const
{
    try {
        return m_node.GetChildren().size();
    } catch (const std::exception &ex) {
        qWarning() << "Failed to get child count for node:" << ex.what();
        return 0;
    }
}

QList<QOpcUaNode *> QFreeOpcUaNode::children()
{
    QList<QOpcUaNode *> result;

    try {
        std::vector<OpcUa::Node> tmp = m_node.GetChildren();
        result.reserve(tmp.size());
        for (std::vector<OpcUa::Node>::const_iterator it = tmp.cbegin(); it != tmp.cend(); ++it) {
            // FIXME: leaks on the exception path!
            QFreeOpcUaNode* child = new QFreeOpcUaNode(*it, m_client);

            // set this node as parent for a natural parent/child tree
            // FIXME: but let's think about ownership here a bit more, because calling this method
            // multiple times will create a bunch of children
            child->setParent(this);

            result.append(child);
        }
    } catch (const std::exception &ex) {
        qWarning() << "Failed to get children for node:" << ex.what();
    }

    return result;
}

QStringList QFreeOpcUaNode::childIds() const
{
    QStringList result;

    try {
        std::vector<OpcUa::Node> tmp = m_node.GetChildren();
        result.reserve(tmp.size());
        for (std::vector<OpcUa::Node>::const_iterator it = tmp.cbegin(); it != tmp.end(); ++it)
            result.append(QFreeOpcUaValueConverter::nodeIdToString(it->GetId()));
    } catch (const std::exception &ex) {
        qWarning() << "Failed to get child ids for node:" << ex.what();
    }

    return result;
}

QString QFreeOpcUaNode::xmlNodeId() const
{
    try {
        return QFreeOpcUaValueConverter::nodeIdToString(m_node.GetId());
    } catch (const std::exception &ex) {
        qWarning() << "Failed to get XML id for node:" << ex.what();
        return QString();
    }
}

QString QFreeOpcUaNode::nodeClass() const
{
    try {
        int32_t temp = m_node.GetAttribute(OpcUa::AttributeId::NodeClass).Value.As<int32_t>();
        OpcUa::NodeClass nc = static_cast<OpcUa::NodeClass>(temp);

        switch (nc) {
        case OpcUa::NodeClass::Object:
            return QStringLiteral("Object");
        case OpcUa::NodeClass::Variable:
            return QStringLiteral("Variable");
        case OpcUa::NodeClass::Method:
            return QStringLiteral("Method");
        case OpcUa::NodeClass::ObjectType:
            return QStringLiteral("ObjectType");
        case OpcUa::NodeClass::VariableType:
            return QStringLiteral("VariableType");
        case OpcUa::NodeClass::ReferenceType:
            return QStringLiteral("ReferenceType");
        case OpcUa::NodeClass::DataType:
            return QStringLiteral("DataType");
        case OpcUa::NodeClass::View:
            return QStringLiteral("View");
        default:
            return QString();
        }
    } catch (const std::exception &ex) {
        qWarning() << "Failed to get node class for node:" << ex.what();
        return QString();
    }
}

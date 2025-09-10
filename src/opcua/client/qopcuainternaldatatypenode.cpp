// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuainternaldatatypenode_p.h"
#include "qopcuagenericstructhandler_p.h"

QT_BEGIN_NAMESPACE

QOpcUaInternalDataTypeNode::QOpcUaInternalDataTypeNode(QOpcUaClient *client)
    : m_client(client)
{
}

bool QOpcUaInternalDataTypeNode::initialize(const QString &nodeId)
{
    if (!m_client)
        return false;

    m_nodeId = nodeId;

    m_node.reset(m_client->node(nodeId));

    if (!m_node)
        return false;

    QObject::connect(m_node.get(), &QOpcUaNode::browseFinished, this,
                     [this](const QList<QOpcUaReferenceDescription> &results, QOpcUa::UaStatusCode result) {
                         Q_UNUSED(result);

                         for (const auto &entry : results) {
                             auto child = new QOpcUaInternalDataTypeNode(m_client);
                             const auto success = child->initialize(entry.targetNodeId().nodeId());

                             if (!success) {
                                 child->deleteLater();
                                 handleFinished(false);
                                 qCWarning(lcGenericStructHandler) << "Failed to init child" << entry.browseName().name();
                                 return;
                             }

                             m_children.push_back(std::unique_ptr<QOpcUaInternalDataTypeNode>(child));

                             QObject::connect(child, &QOpcUaInternalDataTypeNode::initializeFinished, this, &QOpcUaInternalDataTypeNode::handleFinished);
                         }

                         const auto success = m_node->readAttributes(QOpcUa::NodeAttribute::IsAbstract |
                                                                     QOpcUa::NodeAttribute::DataTypeDefinition |
                                                                     QOpcUa::NodeAttribute::BrowseName);

                         if (!success) {
                             qCWarning(lcGenericStructHandler) << "Failed to read attributes for" << m_node->nodeId();
                             handleFinished(false);
                         }
                     });

    QObject::connect(m_node.get(), &QOpcUaNode::attributeRead, this, [this]() {
        m_isAbstract = m_node->attribute(QOpcUa::NodeAttribute::IsAbstract).toBool();
        m_definition = m_node->attribute(QOpcUa::NodeAttribute::DataTypeDefinition);
        m_name = m_node->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>().name();
        handleFinished(true);
    });

    const auto success = m_node->browseChildren(QOpcUa::ReferenceTypeId::HasSubtype, QOpcUa::NodeClass::DataType);
    if (!success) {
        qCWarning(lcGenericStructHandler) << "Failed to start browse for" << m_node->nodeId();
        handleFinished(false);
    }

    return success;
}

QVariant QOpcUaInternalDataTypeNode::definition() const
{
    return m_definition;
}

bool QOpcUaInternalDataTypeNode::isAbstract() const
{
    return m_isAbstract;
}

QString QOpcUaInternalDataTypeNode::name() const
{
    return m_name;
}

QString QOpcUaInternalDataTypeNode::nodeId() const
{
    return m_nodeId;
}

const std::vector<std::unique_ptr<QOpcUaInternalDataTypeNode> > &QOpcUaInternalDataTypeNode::children() const
{
    return m_children;
}

void QOpcUaInternalDataTypeNode::handleFinished(bool success)
{
    if (m_hasError)
        return;

    m_finishedCount++;
    m_hasError = !success;

    if (m_finishedCount == m_children.size() + 1 || m_hasError)
        emit initializeFinished(!m_hasError);
}

QT_END_NAMESPACE

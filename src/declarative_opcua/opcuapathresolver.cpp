// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/opcuapathresolver_p.h>
#include <private/opcuarelativenodeid_p.h>
#include <private/opcuarelativenodepath_p.h>

#include <QOpcUaClient>
#include <QMetaEnum>
#include <QLoggingCategory>

QT_BEGIN_NAMESPACE

/*!
    \class OpcUaPathResolver
    \inqmlmodule QtOpcUa
    \internal
    \brief This class resolves relative nodes.

    This class is used to resolve relative node IDs. It will emit \c resolvedNode
    with the result and delete itself afterwards.
    In case of errors the resolved node is empty and the error message is set.

    This class is capable of resolving cascaded relative nodes by recursively instantiating
    further resolvers. The maximum recursion depth is 50.

    \sa RelativeNodeId, Node
*/
const int maxRecursionDepth = 50;
Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_QML)

OpcUaPathResolver::OpcUaPathResolver(OpcUaRelativeNodeId *relativeNode, QOpcUaClient *client, QObject *target)
    : QObject(target)
    , m_level(0)
    , m_relativeNode(relativeNode)
    , m_target(target)
    , m_client(client)
    , m_node(nullptr)
{
}

OpcUaPathResolver::OpcUaPathResolver(int level, OpcUaRelativeNodeId *relativeNode, QOpcUaClient *client, QObject *target)
    : QObject(target)
    , m_level(level)
    , m_relativeNode(relativeNode)
    , m_target(target)
    , m_client(client)
    , m_node(nullptr)
{
}

OpcUaPathResolver::~OpcUaPathResolver()
{
    if (m_node) {
        m_node->deleteLater();
        m_node = nullptr;
    }
}

void OpcUaPathResolver::startResolving()
{
    if (!m_relativeNode || !m_client || !m_target) {
        emit resolvedNode(UniversalNode(), QLatin1String("Member has been deleted"));
        deleteLater();
        return;
    }

    auto startNode = m_relativeNode->startNode();
    if (!startNode) {
        emit resolvedNode(UniversalNode(), QLatin1String("Aborted resolving because start node not present"));
        deleteLater();
        return;
    }

    if (m_relativeNode->pathCount() == 0) {
        emit resolvedNode(UniversalNode(), QLatin1String("Skipping to resolve relative node with empty path"));
        deleteLater();
        return;
    }

    if (qobject_cast<const OpcUaRelativeNodeId *>(startNode)) {
        // Trigger recursive resolving
        if (m_level >= maxRecursionDepth) {
            emit resolvedNode(UniversalNode(), QLatin1String("Maximum recursion depth reached during node resolution"));
            deleteLater();
            return;
        }
        auto node = qobject_cast<OpcUaRelativeNodeId *>(startNode);
        auto resolver = new OpcUaPathResolver(m_level + 1, node, m_client, this);
        connect(resolver, &OpcUaPathResolver::resolvedNode, this, &OpcUaPathResolver::startNodeResolved);
        resolver->startResolving();
        return;
    } else {
        startNodeResolved(startNode, QString());
    }
}

void OpcUaPathResolver::startNodeResolved(UniversalNode startNode, const QString &errorMessage)
{
    if (!m_relativeNode || !m_client || !m_target) {
        emit resolvedNode(UniversalNode(), QLatin1String("Member has been deleted"));
        deleteLater();
        return;
    }

    if (sender())
        sender()->deleteLater();

    if (!errorMessage.isEmpty()) {
        emit resolvedNode(startNode, errorMessage);
        deleteLater();
        return;
    }

    startNode.resolveNamespace(m_client);
    m_node = m_client->node(startNode.fullNodeId());
    if (!m_node) {
        emit resolvedNode(startNode, QStringLiteral("Could not create node from '%1'")
                          .arg(startNode.fullNodeId()));
        deleteLater();
        return;
    }

    // construct path vector
    QList<QOpcUaRelativePathElement> path;
    for (int i = 0; i < m_relativeNode->pathCount(); ++i)
        path.append(m_relativeNode->path(i)->toRelativePathElement(m_client));

    qCDebug(QT_OPCUA_PLUGINS_QML) << "Starting browse on" << m_node->nodeId();
    connect(m_node, &QOpcUaNode::resolveBrowsePathFinished, this, &OpcUaPathResolver::browsePathFinished);
    if (!m_node->resolveBrowsePath(path)) {
        emit resolvedNode(UniversalNode(), QStringLiteral("Failed to start browse"));
        deleteLater();
        return;
    }
}

void OpcUaPathResolver::browsePathFinished(QList<QOpcUaBrowsePathTarget> results, QList<QOpcUaRelativePathElement> path, QOpcUa::UaStatusCode status)
{
    Q_UNUSED(path);
    UniversalNode nodeToUse;

    if (status != QOpcUa::Good) {
        const QString name = QString::fromUtf8(
                    QMetaEnum::fromType<QOpcUa::UaStatusCode>().valueToKey(status));
        emit resolvedNode(UniversalNode(),
                          QStringLiteral("Resolving browse path return error code %1").arg(name));
        deleteLater();
        return;
    }

    if (results.size() == 0) {
        emit resolvedNode(UniversalNode(),
                          QStringLiteral("Relative path could not be resolved: Results are empty"));
        deleteLater();
        return;
    } else if (results.size() == 1) {
        if (results.at(0).targetId().serverIndex() > 0) {
            emit resolvedNode(UniversalNode(),
                              QStringLiteral("Relative path could not be resolved: "
                                             "Resulting node is located on a remote server"));
            deleteLater();
            return;
        }
        nodeToUse.from(results.at(0));

    } else { // greater than one
        UniversalNode tmp;
        QString message = QStringLiteral("No resolved node found");

        for (const auto &result : results) {
            if (result.isFullyResolved()) {
                if (result.targetId().serverIndex() > 0) {
                    message = QStringLiteral("Relative path could not be resolved: "
                                             "Resulting node is located on a remote server");
                    continue;
                }
                if (!tmp.nodeIdentifier().isEmpty()) {
                    emit resolvedNode(UniversalNode(), QLatin1String("There are multiple resolved nodes"));
                    deleteLater();
                    return;
                }
                tmp.from(result);
            }
        }

        if (!tmp.nodeIdentifier().isEmpty()) {
            nodeToUse = tmp;
        } else {
            emit resolvedNode(UniversalNode(), message);
            deleteLater();
            return;
        }
    }

    nodeToUse.resolveNamespace(m_client);
    qCDebug(QT_OPCUA_PLUGINS_QML) << "Relative node fully resolved to:" << nodeToUse.fullNodeId();
    emit resolvedNode(nodeToUse, QString());
    deleteLater();
}

QT_END_NAMESPACE

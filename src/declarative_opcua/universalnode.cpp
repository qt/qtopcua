// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/universalnode_p.h>
#include <private/opcuanodeidtype_p.h>

#include <QtOpcUa/qopcuaclient.h>
#include <QLoggingCategory>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_QML)

UniversalNode::UniversalNode(QObject *parent)
    : QObject(parent)
{
}

UniversalNode::UniversalNode()
    :QObject(nullptr)
{
}

UniversalNode::UniversalNode(const QString &nodeIdentifier, QObject *parent)
    : QObject(parent)
{
    setNodeIdentifier(nodeIdentifier);
}

UniversalNode::UniversalNode(const QString &namespaceName, const QString &nodeIdentifier, QObject *parent)
    : QObject(parent)
{
    setMembers(false, 0, true, namespaceName, true, nodeIdentifier);
}

UniversalNode::UniversalNode(quint16 namespaceIndex, const QString &nodeIdentifier, QObject *parent)
    : QObject(parent)
{
    setMembers(true, namespaceIndex, false, QString(), true, nodeIdentifier);
}

UniversalNode::UniversalNode(const UniversalNode &other, QObject *parent)
    : QObject(parent)
{
    setMembers(other.isNamespaceNameValid(), other.namespaceIndex(),
               !other.namespaceName().isEmpty(), other.namespaceName(),
               !other.nodeIdentifier().isEmpty(), other.nodeIdentifier());
}

UniversalNode::UniversalNode(const OpcUaNodeIdType *other, QObject *parent)
    : QObject(parent)
{
    if (other)
        from(*other);
}

const QString &UniversalNode::namespaceName() const
{
    return m_namespaceName;
}

void UniversalNode::setNamespace(const QString &namespaceName)
{
    bool ok;
    int index = namespaceName.toUInt(&ok);

    setMembers(ok, index, !ok, namespaceName, false, QString());
}

quint16 UniversalNode::namespaceIndex() const
{
    return m_namespaceIndex;
}

void UniversalNode::setNamespace(quint16 namespaceIndex)
{
    setMembers(true, namespaceIndex, false, QString(), false, QString());
}

const QString &UniversalNode::nodeIdentifier() const
{
    return m_nodeIdentifier;
}

void UniversalNode::setNodeIdentifier(const QString &nodeIdentifier)
{
    int index = 0;
    QString name;

    if (splitNodeIdAndNamespace(nodeIdentifier, &index, &name)) {
        setMembers(true, index, false, QString(), true, name);
    } else {
        setMembers(false, 0, false, QString(), true, nodeIdentifier);
    }
}

void UniversalNode::resolveNamespace(QOpcUaClient *client)
{
    if (!m_namespaceIndexValid)
        resolveNamespaceNameToIndex(client);
    else if (m_namespaceName.isEmpty())
        resolveNamespaceIndexToName(client);
}

void UniversalNode::resolveNamespaceIndexToName(QOpcUaClient *client)
{
    if (!m_namespaceIndexValid) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "Could not resolve namespace: Namespace index is not valid";
        return;
    }

    const auto namespaceArray = client->namespaceArray();

    if (!namespaceArray.size()) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "Namespaces table missing, unable to resolve namespace name.";
        return;
    }

    if (m_namespaceIndex >= namespaceArray.size()) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "Namespace index not in a valid range";
        return;
    }

    setMembers(true, m_namespaceIndex, true, namespaceArray.at(m_namespaceIndex), false, QString());
}

void UniversalNode::resolveNamespaceNameToIndex(QOpcUaClient *client)
{
    if (m_namespaceIndexValid)
        return; // Namespace index already resolved, nothing to do

    int index = resolveNamespaceNameToIndex(m_namespaceName, client);
    if (index < 0) {
        qCWarning(QT_OPCUA_PLUGINS_QML)
                << "Could not resolve namespace for node"
                << (m_nodeIdentifier.isEmpty() ? QString() : (u'(' + m_nodeIdentifier + u')'));
        return;
    }
    setMembers(true, index, true, m_namespaceName, false, QString());
}

int UniversalNode::resolveNamespaceNameToIndex(const QString &namespaceName, QOpcUaClient *client)
{
    const auto namespaceArray = client->namespaceArray();

    if (!namespaceArray.size()) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "Namespaces table missing, unable to resolve namespace name.";
        return -1;
    }

    if (namespaceName.isEmpty()) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "Could not resolve namespace: Namespace name is empty";
        return -1;
    }

    int index = namespaceArray.indexOf(namespaceName);
    if (index < 0) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "Could not resolve namespace: Namespace" << namespaceName << "not found in" << namespaceArray;
        return -1;
    }

    return index;
}

bool UniversalNode::isNamespaceNameValid() const
{
    return !m_namespaceName.isEmpty();
}

bool UniversalNode::isNamespaceIndexValid() const
{
    return m_namespaceIndexValid;
}

QOpcUaQualifiedName UniversalNode::toQualifiedName() const
{
    QOpcUaQualifiedName qualifiedName;

    if (!m_namespaceIndexValid || m_nodeIdentifier.isEmpty()) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "Insufficient information to create a QOpcUaQualifiedName";
        return qualifiedName;
    }

    qualifiedName.setNamespaceIndex(m_namespaceIndex);
    qualifiedName.setName(m_nodeIdentifier);
    return qualifiedName;
}

QOpcUaExpandedNodeId UniversalNode::toExpandedNodeId() const
{
    QOpcUaExpandedNodeId expandedNodeId;

    if (m_namespaceName.isEmpty() || m_nodeIdentifier.isEmpty()) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "Insufficient information to create a QOpcUaExpandedNodeId";
        return expandedNodeId;
    }

    expandedNodeId.setServerIndex(0);
    expandedNodeId.setNamespaceUri(m_namespaceName);
    expandedNodeId.setNodeId(m_nodeIdentifier);
    return expandedNodeId;
}

void UniversalNode::from(const QOpcUaQualifiedName &qualifiedName)
{
    setMembers(true, qualifiedName.namespaceIndex(), false, QString(), true, qualifiedName.name());
}

void UniversalNode::from(const QOpcUaExpandedNodeId &expandedNodeId)
{
    setMembers(false, 0, true, expandedNodeId.namespaceUri(), true, expandedNodeId.nodeId());
}

void UniversalNode::from(const QOpcUaBrowsePathTarget &browsePathTarget)
{
    // QExpandedNodeId is too unreliable and needs some casehandling around it to get a common information
    int index = 0;
    QString namespaceName = browsePathTarget.targetId().namespaceUri();
    QString identifier;
    bool namespaceIndexValid =  splitNodeIdAndNamespace(browsePathTarget.targetId().nodeId(), &index, &identifier);

    setMembers(namespaceIndexValid, index, !namespaceName.isEmpty(), namespaceName, true, identifier);
}

void UniversalNode::from(const OpcUaNodeIdType *other) {
    from(*other);
}

void UniversalNode::from(const OpcUaNodeIdType &other)
{
    setMembers(other.m_universalNode.m_namespaceIndexValid, other.m_universalNode.m_namespaceIndex,
               !other.nodeNamespace().isEmpty(), other.nodeNamespace(),
               !other.identifier().isEmpty(), other.identifier());
}

void UniversalNode::from(const UniversalNode &other)
{
    setMembers(other.isNamespaceIndexValid(), other.namespaceIndex(),
               true, other.namespaceName(), true, other.nodeIdentifier());
}

QString UniversalNode::fullNodeId() const
{
    if (!m_namespaceIndexValid || m_nodeIdentifier.isEmpty()) {
        QString message = QStringLiteral("Unable to construct a full node id");
        if (!m_nodeIdentifier.isEmpty())
            message += QStringLiteral(" for node ") + m_nodeIdentifier;
        else
            message += QStringLiteral(" because node id string is empty.");

        if (!m_namespaceIndexValid)
            message += QStringLiteral("; namespace index is not valid.");
        qCWarning(QT_OPCUA_PLUGINS_QML) << message;
        return QString();
    }

    return createNodeString(m_namespaceIndex, m_nodeIdentifier);
}

QOpcUaNode *UniversalNode::createNode(QOpcUaClient *client)
{
    return client->node(fullNodeId());
}

UniversalNode &UniversalNode::operator=(const UniversalNode &rhs)
{
    m_namespaceName = rhs.m_namespaceName;
    m_nodeIdentifier = rhs.m_nodeIdentifier;
    m_namespaceIndex = rhs.m_namespaceIndex;
    m_namespaceIndexValid = rhs.m_namespaceIndexValid;
    return *this;
}

/* This function sets the members to the desired values and emits changes signal only once after all variables
 * have already been set.
 * If the namespace index or name changes without setting the counterpart as well it will invalidate
 * not part not being set.
 */
void UniversalNode::setMembers(bool setNamespaceIndex, quint16 namespaceIndex,
                               bool setNamespaceName, const QString &namespaceName,
                               bool setNodeIdentifier, const QString &nodeIdentifier)
{
    // qCDebug(QT_OPCUA_PLUGINS_QML) << Q_FUNC_INFO << setNamespaceIndex << namespaceIndex << setNamespaceName << namespaceName << setNodeIdentifier << nodeIdentifier;
    bool emitNamespaceIndexChanged = false;
    bool emitNamespaceNameChanged = false;
    bool emitNodeIdentifierChanged = false;

    if (setNamespaceIndex && (namespaceIndex != m_namespaceIndex || !m_namespaceIndexValid)) {
        m_namespaceIndex = namespaceIndex;
        m_namespaceIndexValid = true;
        emitNamespaceIndexChanged = true;

        if (!setNamespaceName) // Index changed without name given: invalidate name
            m_namespaceName.clear();
    }

    if (setNamespaceName && namespaceName != m_namespaceName) {
        m_namespaceName = namespaceName;
        emitNamespaceNameChanged = true;

        if (!setNamespaceIndex) // Name changed without index given: invalidate index
            m_namespaceIndexValid = false;
    }

    if (setNodeIdentifier && nodeIdentifier != m_nodeIdentifier) {
        if (nodeIdentifier.startsWith(QLatin1String("ns=")))
            qCWarning(QT_OPCUA_PLUGINS_QML) << "Setting node identifier with namespace internally is not allowed.";

        m_nodeIdentifier = nodeIdentifier;
        emitNodeIdentifierChanged = true;
    }

    if (emitNamespaceIndexChanged)
        emit namespaceIndexChanged(m_namespaceIndex);
    if (emitNamespaceNameChanged)
        emit namespaceNameChanged(m_namespaceName);
    if (emitNodeIdentifierChanged)
        emit nodeIdentifierChanged(m_nodeIdentifier);
    if (emitNamespaceIndexChanged || emitNamespaceNameChanged)
        emit namespaceChanged();
    if (emitNamespaceIndexChanged || emitNamespaceNameChanged || emitNodeIdentifierChanged) {
        emit nodeChanged();
    }
}

/*
    This function splits up a node identifier into namespace index and node name.
    Returns true if successful, otherwise false.

    When passing \nullptr as pointer argument, the assignment of results to that
    pointer will be skipped.
 */
bool UniversalNode::splitNodeIdAndNamespace(const QString nodeIdentifier, int *namespaceIndex, QString *identifier)
{
    if (nodeIdentifier.startsWith(QLatin1String("ns="))) {
        const auto token = nodeIdentifier.split(QLatin1Char(';'));
        if (token.size() != 2) {
            qCWarning(QT_OPCUA_PLUGINS_QML) << "Invalid node identifier:" << nodeIdentifier;
            return false;
        }

        const QString ns = token[0].mid(3);
        bool ok = false;
        if (namespaceIndex)
            *namespaceIndex = ns.toUInt(&ok);
        if (!ok) {
            qCWarning(QT_OPCUA_PLUGINS_QML) << "Namespace index is not a number:" << nodeIdentifier;
            return false;
        }
        if (identifier)
            *identifier = token[1];
        return true;
    }
    return false;
}

bool UniversalNode::operator==(const UniversalNode &rhs) const
{
    return this->m_namespaceName == rhs.m_namespaceName &&
            this->m_nodeIdentifier == rhs.m_nodeIdentifier &&
            this->m_namespaceIndex == rhs.m_namespaceIndex &&
            this->m_namespaceIndexValid == rhs.m_namespaceIndexValid;
}

/*
    Constructs a full node id string out of id and namespace.
    A string based namespace will be preferred and resolved.
*/
QString UniversalNode::resolveNamespaceToNode(const QString &nodeId, const QString &namespaceName, QOpcUaClient *client)
{
    int namespaceIndex = 0;
    QString identifier;

    if (!splitNodeIdAndNamespace(nodeId, &namespaceIndex, &identifier)) {
        // Splitting of node id failed. Consider the whole node id as name.
        identifier = nodeId;
    }

    if (!namespaceName.isEmpty()) {
        namespaceIndex = resolveNamespaceNameToIndex(namespaceName, client);
        if (namespaceIndex < 0)
            return QString();
    }
    return createNodeString(namespaceIndex, identifier);
}

QT_END_NAMESPACE

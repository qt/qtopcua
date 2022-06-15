// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma once

#include <QObject>
#include <private/qglobal_p.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

QT_BEGIN_NAMESPACE

class QOpcUaClient;
class QOpcUaNode;
class QOpcUaQualifiedName;
class QOpcUaExpandedNodeId;
class QOpcUaBrowsePathTarget;
class QOpcUaLocalizedText;
class OpcUaNodeIdType;

class UniversalNode : public QObject
{
    Q_OBJECT
public:
    UniversalNode();
    UniversalNode(QObject *parent);
    UniversalNode(const QString &nodeIdentifier, QObject *parent = nullptr);
    UniversalNode(const QString &namespaceName, const QString &nodeIdentifier, QObject *parent = nullptr);
    UniversalNode(quint16 namespaceIndex, const QString &nodeIdentifier, QObject *parent = nullptr);
    UniversalNode(const UniversalNode &other, QObject *parent = nullptr);
    UniversalNode(const OpcUaNodeIdType *other, QObject *parent = nullptr);

    void setNamespace(quint16 namespaceIndex);
    void setNamespace(const QString &name);

    const QString &namespaceName() const;
    void setNamespaceName(const QString &namespaceName);
    bool isNamespaceNameValid() const;

    quint16 namespaceIndex() const;
    bool isNamespaceIndexValid() const;

    const QString &nodeIdentifier() const;
    void setNodeIdentifier(const QString &nodeIdentifier);

    void resolveNamespaceIndexToName(QOpcUaClient *client);
    void resolveNamespaceNameToIndex(QOpcUaClient *client);
    void resolveNamespace(QOpcUaClient *client);
    static int resolveNamespaceNameToIndex(const QString &namespaceName, QOpcUaClient *client);

    QOpcUaQualifiedName toQualifiedName() const;
    void from(const QOpcUaQualifiedName &qualifiedName);

    QOpcUaExpandedNodeId toExpandedNodeId() const;
    void from(const QOpcUaExpandedNodeId &expandedNodeId);

    void from(const QOpcUaBrowsePathTarget &browsePathTarget);
    void from(const OpcUaNodeIdType &);
    void from(const OpcUaNodeIdType *);
    void from(const UniversalNode &);

    QString fullNodeId() const;
    QOpcUaNode *createNode(QOpcUaClient *client);

    UniversalNode& operator=(const UniversalNode&);
    bool operator==(const UniversalNode &rhs) const;

    static QString resolveNamespaceToNode(const QString &nodeId, const QString &namespaceName, QOpcUaClient *client);
    inline static QString createNodeString(int namespaceIndex, const QString &nodeIdentifier) {
            return QStringLiteral("ns=%1;%2").arg(namespaceIndex).arg(nodeIdentifier);
    }
    static bool splitNodeIdAndNamespace(const QString nodeIdentifier, int *namespaceIndex, QString *identifier);

signals:
    void namespaceNameChanged(const QString &);
    void namespaceIndexChanged(quint16);
    void nodeIdentifierChanged(const QString &);
    void nodeChanged();
    void namespaceChanged();

private:
    void setMembers(bool setNamespaceIndex, quint16 namespaceIndex,
                    bool setNamespaceName, const QString &namespaceName,
                    bool setNodeIdentifier, const QString &nodeIdentifier);

    QString m_namespaceName;
    QString m_nodeIdentifier;
    quint16 m_namespaceIndex = 0;
    bool m_namespaceIndexValid = false;
};

QT_END_NAMESPACE

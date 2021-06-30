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

#pragma once

#include <QObject>

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

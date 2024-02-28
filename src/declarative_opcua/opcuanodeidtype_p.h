// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUA_OPCUANODEIDTYPE_P_H
#define QOPCUA_OPCUANODEIDTYPE_P_H

#include <private/universalnode_p.h>

#include <QObject>
#include <QtQml/qqml.h>

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

class OpcUaNodeIdType : public QObject
{
    Q_OBJECT

    QML_NAMED_ELEMENT(NodeIdType)
    QML_ADDED_IN_VERSION(5, 12)
    QML_UNCREATABLE("The type 'NodeIdType' is not creatable. "
                    "Use 'NodeId' or 'RelativeNodeId' instead.")
public:
    /* These functions are not exposed to QML intentionally.
       The reason is that some derived QML types have to prohibit access to some of these functions.
       Exposing is done selectively in the derived classes.
    */
    explicit OpcUaNodeIdType(QObject *parent = nullptr);
    void setNodeNamespace(const QString &);
    const QString &nodeNamespace() const;
    void setIdentifier(const QString &);
    const QString &identifier() const;
    QString fullNodePath() const;
    void from(const OpcUaNodeIdType &);
    void from(const UniversalNode &);

signals:
    void nodeNamespaceChanged(const QString &);
    void identifierChanged(const QString &);
    void nodeChanged();

private:
    UniversalNode m_universalNode;

    friend class UniversalNode;
};

QT_END_NAMESPACE

#endif // QOPCUA_OPCUANODEIDTYPE_P_H

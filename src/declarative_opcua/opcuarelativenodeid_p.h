// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUA_OPCUARELATIVENODEID_P_H
#define QOPCUA_OPCUARELATIVENODEID_P_H

#include <private/opcuanodeidtype_p.h>

#include <QList>
#include <QQmlListProperty>
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

class OpcUaRelativeNodePath;
class OpcUaNodeIdType;

class OpcUaRelativeNodeId : public OpcUaNodeIdType
{
    Q_OBJECT
    Q_DISABLE_COPY(OpcUaRelativeNodeId)
    Q_PROPERTY(OpcUaNodeIdType* startNode READ startNode WRITE setStartNode NOTIFY startNodeChanged)
    Q_PROPERTY(QQmlListProperty<OpcUaRelativeNodePath> path READ paths)

    QML_NAMED_ELEMENT(RelativeNodeId)
    QML_ADDED_IN_VERSION(5, 12)

public:
    explicit OpcUaRelativeNodeId(QObject *parent = nullptr);

    OpcUaNodeIdType *startNode() const;
    QQmlListProperty<OpcUaRelativeNodePath> paths();
    void appendPath(OpcUaRelativeNodePath *path);
    int pathCount() const;
    OpcUaRelativeNodePath *path(int) const;
    void clearPaths();

signals:
    void startNodeChanged(OpcUaNodeIdType *startNode);
    void pathChanged();

public slots:
    void setStartNode(OpcUaNodeIdType *startNode);

private:
    static void appendPath(QQmlListProperty<OpcUaRelativeNodePath>*, OpcUaRelativeNodePath *);
    static qsizetype pathCount(QQmlListProperty<OpcUaRelativeNodePath>*);
    static OpcUaRelativeNodePath* path(QQmlListProperty<OpcUaRelativeNodePath>*, qsizetype);
    static void clearPaths(QQmlListProperty<OpcUaRelativeNodePath>*);

    OpcUaNodeIdType *m_startNode = nullptr;
    QList<OpcUaRelativeNodePath *> m_paths;

};

QT_END_NAMESPACE

#endif // QOPCUA_OPCUARELATIVENODEID_P_H

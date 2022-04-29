/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#pragma once

#include "opcuanodeidtype.h"
#include <QList>
#include <QQmlListProperty>

QT_BEGIN_NAMESPACE

class OpcUaRelativeNodePath;
class OpcUaNodeIdType;

class OpcUaRelativeNodeId : public OpcUaNodeIdType
{
    Q_OBJECT
    Q_DISABLE_COPY(OpcUaRelativeNodeId)
    Q_PROPERTY(OpcUaNodeIdType* startNode READ startNode WRITE setStartNode NOTIFY startNodeChanged)
    Q_PROPERTY(QQmlListProperty<OpcUaRelativeNodePath> path READ paths)

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

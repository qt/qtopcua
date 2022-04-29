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

#include <QObject>
#include <QPointer>
#include "qopcuatype.h"
#include "universalnode.h"
#include "qopcuabrowsepathtarget.h"
#include "qopcuarelativepathelement.h"

QT_BEGIN_NAMESPACE

class QOpcUaNode;
class QOpcUaClient;
class OpcUaRelativeNodeId;
class QOpcUaClient;

class OpcUaPathResolver : public QObject
{
    Q_OBJECT
public:
    OpcUaPathResolver(OpcUaRelativeNodeId *relativeNode, QOpcUaClient *client, QObject *target);
    ~OpcUaPathResolver();
    void startResolving();

signals:
    void resolvedStartNode(const UniversalNode &nodeId, const QString &errorMessage);
    void resolvedNode(UniversalNode node, QString errorMessage);

private slots:
    void startNodeResolved(UniversalNode startNode, const QString &errorMessage);
    void browsePathFinished(QList<QOpcUaBrowsePathTarget> results, QList<QOpcUaRelativePathElement> path, QOpcUa::UaStatusCode status);

private:
    OpcUaPathResolver(int level, OpcUaRelativeNodeId *relativeNode, QOpcUaClient *client, QObject *target);

    int m_level;
    QPointer<OpcUaRelativeNodeId> m_relativeNode;
    QPointer<QObject> m_target;
    QPointer<QOpcUaClient> m_client;
    QOpcUaNode *m_node;
};

QT_END_NAMESPACE

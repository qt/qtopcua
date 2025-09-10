// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUA_OPCUAPATHRESOLVER_P_H
#define QOPCUA_OPCUAPATHRESOLVER_P_H

#include <private/universalnode_p.h>

#include <QObject>
#include <QPointer>

#include "qopcuatype.h"
#include "qopcuabrowsepathtarget.h"
#include "qopcuarelativepathelement.h"

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

#endif // QOPCUA_OPCUAPATHRESOLVER_P_H

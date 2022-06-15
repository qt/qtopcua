// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma once

#include <private/opcuanodeidtype_p.h>

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

class OpcUaNodeId : public OpcUaNodeIdType
{
    Q_OBJECT
    Q_PROPERTY(QString ns READ nodeNamespace WRITE setNodeNamespace NOTIFY nodeNamespaceChanged)
    Q_PROPERTY(QString identifier READ identifier WRITE setIdentifier NOTIFY identifierChanged)

    QML_NAMED_ELEMENT(NodeId)
    QML_ADDED_IN_VERSION(5, 12)

public:
    OpcUaNodeId(QObject *parent = nullptr);

signals:
    void nodeNamespaceChanged(const QString &);
    void identifierChanged(const QString &);
    void nodeChanged();
};

QT_END_NAMESPACE

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

QT_BEGIN_NAMESPACE

class OpcUaNodeId : public OpcUaNodeIdType
{
    Q_OBJECT
    Q_PROPERTY(QString ns READ nodeNamespace WRITE setNodeNamespace NOTIFY nodeNamespaceChanged)
    Q_PROPERTY(QString identifier READ identifier WRITE setIdentifier NOTIFY identifierChanged)

public:
    OpcUaNodeId(QObject *parent = nullptr);

signals:
    void nodeNamespaceChanged(const QString &);
    void identifierChanged(const QString &);
    void nodeChanged();
};

QT_END_NAMESPACE

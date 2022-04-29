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

#include "opcuanode.h"
#include "opcuamethodargument.h"
#include "opcuastatus.h"
#include <QQmlListProperty>
#include <QList>

QT_BEGIN_NAMESPACE

class OpcUaNodeIdType;

class OpcUaMethodNode : public OpcUaNode
{
    Q_OBJECT
    Q_PROPERTY(OpcUaNodeIdType* objectNodeId READ objectNodeId WRITE setObjectNodeId NOTIFY objectNodeIdChanged)
    Q_PROPERTY(QQmlListProperty<OpcUaMethodArgument> inputArguments READ inputArguments NOTIFY inputArgumentsChanged)
    Q_PROPERTY(QVariantList outputArguments READ outputArguments NOTIFY outputArgumentsChanged)
    Q_PROPERTY(OpcUaStatus resultStatus READ resultStatus NOTIFY resultStatusChanged)

public:
    OpcUaMethodNode(QObject *parent = nullptr);
    OpcUaNodeIdType *objectNodeId() const;

    QQmlListProperty<OpcUaMethodArgument> inputArguments();
    QVariantList outputArguments();
    OpcUaStatus resultStatus() const;

public slots:
    void setObjectNodeId(OpcUaNodeIdType *nodeId);
    void callMethod();

signals:
    void objectNodeIdChanged();
    void inputArgumentsChanged();
    void outputArgumentsChanged();

    void resultStatusChanged(OpcUaStatus status);

private slots:
    void handleObjectNodeIdChanged();
    void handleMethodCallFinished(QString methodNodeId, QVariant result, QOpcUa::UaStatusCode statusCode);

private:
    void setupNode(const QString &absolutePath) override;
    bool checkValidity() override;

    static void appendArgument(QQmlListProperty<OpcUaMethodArgument>*, OpcUaMethodArgument *);
    static qsizetype argumentCount(QQmlListProperty<OpcUaMethodArgument>*);
    static OpcUaMethodArgument* argument(QQmlListProperty<OpcUaMethodArgument>*, qsizetype);
    static void clearArguments(QQmlListProperty<OpcUaMethodArgument>*);

private:
    OpcUaNodeIdType *m_objectNodeId = nullptr;
    OpcUaNode *m_objectNode = nullptr;
    QList<OpcUaMethodArgument*> m_inputArguments;
    QVariantList m_outputArguments;
    OpcUaStatus m_resultStatus;
};

QT_END_NAMESPACE

// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUA_OPCUAMETHODNODE_P_H
#define QOPCUA_OPCUAMETHODNODE_P_H

#include <private/opcuanode_p.h>
#include <private/opcuamethodargument_p.h>
#include <private/opcuastatus_p.h>

#include <QQmlListProperty>
#include <QList>

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

class OpcUaNodeIdType;

class OpcUaMethodNode : public OpcUaNode
{
    Q_OBJECT
    Q_PROPERTY(OpcUaNodeIdType* objectNodeId READ objectNodeId WRITE setObjectNodeId NOTIFY objectNodeIdChanged)
    Q_PROPERTY(QQmlListProperty<OpcUaMethodArgument> inputArguments READ inputArguments NOTIFY inputArgumentsChanged)
    Q_PROPERTY(QVariantList outputArguments READ outputArguments NOTIFY outputArgumentsChanged)
    Q_PROPERTY(OpcUaStatus resultStatus READ resultStatus NOTIFY resultStatusChanged)

    QML_NAMED_ELEMENT(MethodNode)
    QML_ADDED_IN_VERSION(5, 12)

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

#endif // QOPCUA_OPCUAMETHODNODE_P_H

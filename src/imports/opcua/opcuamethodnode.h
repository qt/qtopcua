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

#include "opcuanode.h"
#include "opcuamethodargument.h"
#include "qopcuatype.h"
#include <QQmlListProperty>
#include <QVector>

QT_BEGIN_NAMESPACE

class OpcUaNodeIdType;

class OpcUaMethodNode : public OpcUaNode
{
    Q_OBJECT
    Q_PROPERTY(OpcUaNodeIdType* objectNodeId READ objectNodeId WRITE setObjectNodeId NOTIFY objectNodeIdChanged)
    Q_PROPERTY(QQmlListProperty<OpcUaMethodArgument> inputArguments READ inputArguments NOTIFY inputArgumentsChanged)
    Q_PROPERTY(QVariantList outputArguments READ outputArguments NOTIFY outputArgumentsChanged)
    Q_PROPERTY(QOpcUa::UaStatusCode resultStatusCode READ resultStatusCode NOTIFY resultStatusCodeChanged)

public:
    OpcUaMethodNode(QObject *parent = nullptr);
    OpcUaNodeIdType *objectNodeId() const;

    QQmlListProperty<OpcUaMethodArgument> inputArguments();
    QVariantList outputArguments();
    QOpcUa::UaStatusCode resultStatusCode() const;

public slots:
    void setObjectNodeId(OpcUaNodeIdType *nodeId);
    void callMethod();

signals:
    void objectNodeIdChanged();
    void inputArgumentsChanged();
    void outputArgumentsChanged();

    void resultStatusCodeChanged(QOpcUa::UaStatusCode statusCode);

private slots:
    void handleObjectNodeIdChanged();
    void handleMethodCallFinished(QString methodNodeId, QVariant result, QOpcUa::UaStatusCode statusCode);

private:
    void setupNode(const QString &absolutePath) override;
    bool checkValidity() override;

    static void appendArgument(QQmlListProperty<OpcUaMethodArgument>*, OpcUaMethodArgument *);
    static int argumentCount(QQmlListProperty<OpcUaMethodArgument>*);
    static OpcUaMethodArgument* argument(QQmlListProperty<OpcUaMethodArgument>*, int);
    static void clearArguments(QQmlListProperty<OpcUaMethodArgument>*);

private:
    OpcUaNodeIdType *m_objectNodeId = nullptr;
    OpcUaNode *m_objectNode = nullptr;
    QVector<OpcUaMethodArgument*> m_inputArguments;
    QVariantList m_outputArguments;
    QOpcUa::UaStatusCode m_resultStatusCode = QOpcUa::Good;
};

QT_END_NAMESPACE

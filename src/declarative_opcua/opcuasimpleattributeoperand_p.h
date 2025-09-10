// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef OPCUASIMPLEATTRIBUTEOPERAND_P_H
#define OPCUASIMPLEATTRIBUTEOPERAND_P_H

#include <private/opcuaoperandbase_p.h>

#include <QOpcUaSimpleAttributeOperand>
#include <QQmlListProperty>
#include <QList>
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

class OpcUaNodeId;
class QOpcUaClient;

class OpcUaSimpleAttributeOperand : public OpcUaOperandBase, private QOpcUaSimpleAttributeOperand {
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<OpcUaNodeId> browsePath READ browsePath)
    Q_PROPERTY(QString indexRange READ indexRange WRITE setIndexRange)
    Q_PROPERTY(QOpcUa::NodeAttribute nodeAttribute READ nodeAttribute WRITE setNodeAttribute)
    Q_PROPERTY(QString typeId READ typeId WRITE setTypeId)

    QML_NAMED_ELEMENT(SimpleAttributeOperand)
    QML_ADDED_IN_VERSION(5, 13)

public:
    explicit OpcUaSimpleAttributeOperand(QObject *parent = nullptr);
    ~OpcUaSimpleAttributeOperand();
    QOpcUaSimpleAttributeOperand toSimpleAttributeOperand(QOpcUaClient *client) const;
    virtual QVariant toCppVariant(QOpcUaClient *client) const override;

    QString indexRange() const;
    void setIndexRange(const QString &indexRange);

    QOpcUa::NodeAttribute nodeAttribute() const;
    void setNodeAttribute(QOpcUa::NodeAttribute nodeAttribute);

    QString typeId() const;
    void setTypeId(const QString &typeId);

    QQmlListProperty<OpcUaNodeId> browsePath();
    void appendBrowsePathElement(OpcUaNodeId*nodeId);
    int browsePathSize() const;
    OpcUaNodeId *browsePathElement(int index) const;
    void clearBrowsePath();

signals:
    void dataChanged();

private:
    static void appendBrowsePathElement(QQmlListProperty<OpcUaNodeId> *list, OpcUaNodeId *nodeId);
    static qsizetype browsePathSize(QQmlListProperty<OpcUaNodeId> *list);
    static OpcUaNodeId* browsePathElement(QQmlListProperty<OpcUaNodeId> *list, qsizetype index);
    static void clearBrowsePath(QQmlListProperty<OpcUaNodeId> *list);

    QList<OpcUaNodeId*> m_browsePath;
};

QT_END_NAMESPACE

#endif // OPCUASIMPLEATTRIBUTEOPERAND_P_H

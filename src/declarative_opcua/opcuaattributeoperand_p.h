// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef OPCUAATTRIBUTEOPERAND_P_H
#define OPCUAATTRIBUTEOPERAND_P_H

#include <private/opcuaoperandbase_p.h>
#include <private/opcuanodeid_p.h>

#include <QOpcUaAttributeOperand>
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

class OpcUaRelativeNodePath;
class QOpcUaClient;

class OpcUaAttributeOperand : public OpcUaOperandBase, private QOpcUaAttributeOperand {
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<OpcUaRelativeNodePath> browsePath READ browsePath)
    Q_PROPERTY(QString indexRange READ indexRange WRITE setIndexRange)
    Q_PROPERTY(QOpcUa::NodeAttribute nodeAttribute READ nodeAttribute WRITE setNodeAttribute)
    Q_PROPERTY(OpcUaNodeId* typeId READ typeId WRITE setTypeId)
    Q_PROPERTY(QString alias READ alias WRITE setAlias)

    QML_NAMED_ELEMENT(AttributeOperand)
    QML_ADDED_IN_VERSION(5, 13)

public:
    explicit OpcUaAttributeOperand(QObject *parent = nullptr);
    ~OpcUaAttributeOperand();
    virtual QVariant toCppVariant(QOpcUaClient *client) const override;

    OpcUaNodeId *typeId() const;
    void setTypeId(OpcUaNodeId *nodeId);

    QString alias() const;
    void setAlias(const QString &alias);

    QString indexRange() const;
    void setIndexRange(const QString &indexRange);

    QOpcUa::NodeAttribute nodeAttribute() const;
    void setNodeAttribute(QOpcUa::NodeAttribute nodeAttribute);

    QQmlListProperty<OpcUaRelativeNodePath> browsePath();
    void appendBrowsePathElement(OpcUaRelativeNodePath *nodeId);
    int browsePathSize() const;
    OpcUaRelativeNodePath *browsePathElement(int index) const;
    void clearBrowsePath();

signals:
    void dataChanged();

private:
    static void appendBrowsePathElement(QQmlListProperty<OpcUaRelativeNodePath> *list, OpcUaRelativeNodePath *nodeId);
    static qsizetype browsePathSize(QQmlListProperty<OpcUaRelativeNodePath> *list);
    static OpcUaRelativeNodePath* browsePathElement(QQmlListProperty<OpcUaRelativeNodePath> *list, qsizetype index);
    static void clearBrowsePath(QQmlListProperty<OpcUaRelativeNodePath> *list);

    QList<OpcUaRelativeNodePath*> m_browsePath;
    OpcUaNodeId *m_nodeId = nullptr;
};

QT_END_NAMESPACE

#endif // OPCUASIMPLEATTRIBUTEOPERAND_P_H

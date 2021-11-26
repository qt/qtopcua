/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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

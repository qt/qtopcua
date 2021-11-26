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

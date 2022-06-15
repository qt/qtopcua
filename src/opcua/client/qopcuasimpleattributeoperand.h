// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUASIMPLEATTRIBUTEOPERAND_H
#define QOPCUASIMPLEATTRIBUTEOPERAND_H

#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

class QOpcUaQualifiedName;

// OPC-UA part 4, 7.4.4.5
class QOpcUaSimpleAttributeOperandData;
class Q_OPCUA_EXPORT QOpcUaSimpleAttributeOperand
{
public:
    QOpcUaSimpleAttributeOperand();
    QOpcUaSimpleAttributeOperand(const QOpcUaSimpleAttributeOperand &);
    QOpcUaSimpleAttributeOperand(const QString &name, quint16 namespaceIndex = 0,
                            const QString &typeId = QStringLiteral("ns=0;i=2041"), // BaseEventType
                            QOpcUa::NodeAttribute attributeId = QOpcUa::NodeAttribute::Value);
    QOpcUaSimpleAttributeOperand(QOpcUa::NodeAttribute attributeId,
                            const QString &typeId = QStringLiteral("ns=0;i=2041")); // BaseEventType
    QOpcUaSimpleAttributeOperand &operator=(const QOpcUaSimpleAttributeOperand &);
    bool operator==(const QOpcUaSimpleAttributeOperand &rhs) const;
    operator QVariant() const;
    ~QOpcUaSimpleAttributeOperand();

    QString typeId() const;
    void setTypeId(const QString &typeId);

    QList<QOpcUaQualifiedName> browsePath() const;
    QList<QOpcUaQualifiedName> &browsePathRef();
    void setBrowsePath(const QList<QOpcUaQualifiedName> &browsePath);

    QOpcUa::NodeAttribute attributeId() const;
    void setAttributeId(QOpcUa::NodeAttribute attributeId);

    QString indexRange() const;
    void setIndexRange(const QString &indexRange);

private:
    QSharedDataPointer<QOpcUaSimpleAttributeOperandData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaSimpleAttributeOperand)

#endif // QOPCUASIMPLEATTRIBUTEOPERAND_H

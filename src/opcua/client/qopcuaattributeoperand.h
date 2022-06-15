// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAATTRIBUTEOPERAND_H
#define QOPCUAATTRIBUTEOPERAND_H

#include <QtOpcUa/qopcuaglobal.h>
#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaRelativePathElement;

class QVariant;
class QOpcUaRelativePathElement;

// OPC-UA part 4, 7.4.4.4
class QOpcUaAttributeOperandData;
class Q_OPCUA_EXPORT QOpcUaAttributeOperand
{
public:
    QOpcUaAttributeOperand();
    QOpcUaAttributeOperand(const QOpcUaAttributeOperand &);
    QOpcUaAttributeOperand &operator=(const QOpcUaAttributeOperand &);
    operator QVariant() const;
    ~QOpcUaAttributeOperand();

    QString nodeId() const;
    void setNodeId(const QString &nodeId);

    QString alias() const;
    void setAlias(const QString &alias);

    QList<QOpcUaRelativePathElement> browsePath() const;
    QList<QOpcUaRelativePathElement> &browsePathRef();
    void setBrowsePath(const QList<QOpcUaRelativePathElement> &browsePath);

    QOpcUa::NodeAttribute attributeId() const;
    void setAttributeId(QOpcUa::NodeAttribute attributeId);

    QString indexRange() const;
    void setIndexRange(const QString &indexRange);

private:
    QSharedDataPointer<QOpcUaAttributeOperandData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaAttributeOperand)

#endif // QOPCUAATTRIBUTEOPERAND_H

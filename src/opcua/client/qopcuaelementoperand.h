/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
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

#ifndef QOPCUAELEMENTOPERAND_H
#define QOPCUAELEMENTOPERAND_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

// OPC-UA part 4, 7.4.4.2
class QOpcUaElementOperandData;
class Q_OPCUA_EXPORT QOpcUaElementOperand
{
public:
    QOpcUaElementOperand();
    QOpcUaElementOperand(const QOpcUaElementOperand &);
    QOpcUaElementOperand(quint32 index);
    QOpcUaElementOperand &operator=(const QOpcUaElementOperand &);
    operator QVariant() const;
    ~QOpcUaElementOperand();

    quint32 index() const;
    void setIndex(quint32 index);

private:
    QSharedDataPointer<QOpcUaElementOperandData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaElementOperand)

#endif // QOPCUAELEMENTOPERAND_H

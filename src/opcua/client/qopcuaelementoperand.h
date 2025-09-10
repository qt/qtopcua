// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAELEMENTOPERAND_H
#define QOPCUAELEMENTOPERAND_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

// OPC UA 1.05 part 4, 7.7.4.2
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
    friend Q_OPCUA_EXPORT bool comparesEqual(const QOpcUaElementOperand &lhs,
                                             const QOpcUaElementOperand &rhs) noexcept;
    friend bool operator==(const QOpcUaElementOperand &lhs,
                           const QOpcUaElementOperand &rhs) noexcept
    { return comparesEqual(lhs, rhs); }
    friend bool operator!=(const QOpcUaElementOperand &lhs,
                           const QOpcUaElementOperand &rhs) noexcept
    {
        return !(lhs == rhs);
    }

    QSharedDataPointer<QOpcUaElementOperandData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaElementOperand)

#endif // QOPCUAELEMENTOPERAND_H

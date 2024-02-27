// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUACONTENTFILTERELEMENT_H
#define QOPCUACONTENTFILTERELEMENT_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>
#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

class QOpcUaSimpleAttributeOperand;
class QOpcUaAttributeOperand;
class QOpcUaElementOperand;
class QOpcUaLiteralOperand;

class QOpcUaContentFilterElementData;
class Q_OPCUA_EXPORT QOpcUaContentFilterElement
{
public:
    QOpcUaContentFilterElement();
    QOpcUaContentFilterElement(const QOpcUaContentFilterElement &);
    QOpcUaContentFilterElement &operator=(const QOpcUaContentFilterElement &);
#if QT_OPCUA_REMOVED_SINCE(6, 7)
    bool operator==(const QOpcUaContentFilterElement &rhs) const;
#endif
    operator QVariant() const;
    ~QOpcUaContentFilterElement();

    // Specified in OPC UA 1.05 part 4, 7.7.3
    enum FilterOperator : quint32 {
        Equals = 0,
        IsNull = 1,
        GreaterThan = 2,
        LessThan = 3,
        GreaterThanOrEqual = 4,
        LessThanOrEqual = 5,
        Like = 6,
        Not = 7,
        Between = 8,
        InList = 9,
        And = 10,
        Or = 11,
        Cast = 12,
        InView = 13,
        OfType = 14,
        RelatedTo = 15,
        BitwiseAnd = 16,
        BitwiseOr = 17,
    };

    QOpcUaContentFilterElement &operator<<(FilterOperator op);
    QOpcUaContentFilterElement &operator<<(const QOpcUaSimpleAttributeOperand &op);
    QOpcUaContentFilterElement &operator<<(const QOpcUaAttributeOperand &op);
    QOpcUaContentFilterElement &operator<<(const QOpcUaLiteralOperand &op);
    QOpcUaContentFilterElement &operator<<(const QOpcUaElementOperand &op);


    QOpcUaContentFilterElement::FilterOperator filterOperator() const;
    void setFilterOperator(QOpcUaContentFilterElement::FilterOperator filterOperator);

    QList<QVariant> filterOperands() const;
    QList<QVariant> &filterOperandsRef();
    void setFilterOperands(const QList<QVariant> &filterOperands);

private:
    QSharedDataPointer<QOpcUaContentFilterElementData> data;

    friend Q_OPCUA_EXPORT bool comparesEqual(const QOpcUaContentFilterElement &lhs,
                                             const QOpcUaContentFilterElement &rhs) noexcept;

    friend bool operator==(const QOpcUaContentFilterElement &lhs,
                           const QOpcUaContentFilterElement &rhs) noexcept
    { return comparesEqual(lhs, rhs); }
    friend bool operator!=(const QOpcUaContentFilterElement &lhs,
                           const QOpcUaContentFilterElement &rhs) noexcept
    { return !comparesEqual(lhs, rhs); }
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaContentFilterElement)

#endif // QOPCUACONTENTFILTERELEMENT_H

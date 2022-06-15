// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef OPCUAFILTERELEMENT_P_H
#define OPCUAFILTERELEMENT_P_H

#include <private/opcuaoperandbase_p.h>

#include <QOpcUaContentFilterElement>

#include <QObject>
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

class QOpcUaClient;

class OpcUaFilterElement : public QObject {
    Q_OBJECT
    Q_PROPERTY(FilterOperator operator READ operatorType WRITE setOperatorType)
    Q_PROPERTY(OpcUaOperandBase* firstOperand READ firstOperand WRITE setFirstOperand)
    Q_PROPERTY(OpcUaOperandBase* secondOperand READ secondOperand WRITE setSecondOperand)

    QML_NAMED_ELEMENT(FilterElement)
    QML_ADDED_IN_VERSION(5, 13)

public:
    // Same as in qopcuacontentfilterelement.h
    // Specified in OPC-UA part 4, Tables 115 and 116
    enum class FilterOperator : quint32 {
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
        BitwiseOr = 17
    };
    Q_ENUM(FilterOperator);

    explicit OpcUaFilterElement(QObject *parent = nullptr);
    ~OpcUaFilterElement();
    QOpcUaContentFilterElement toFilterElement(QOpcUaClient *);

    FilterOperator operatorType() const;
    void setOperatorType(FilterOperator filterOperator);

    OpcUaOperandBase *firstOperand() const;
    void setFirstOperand(OpcUaOperandBase *operand);

    OpcUaOperandBase *secondOperand() const;
    void setSecondOperand(OpcUaOperandBase *operand);

signals:
    void dataChanged();

private:
    FilterOperator m_filterOperator = FilterOperator::Equals;
    OpcUaOperandBase *m_firstOperand = nullptr;
    OpcUaOperandBase *m_secondOperand = nullptr;
};

QT_END_NAMESPACE

#endif // OPCUAFILTERELEMENT_P_H

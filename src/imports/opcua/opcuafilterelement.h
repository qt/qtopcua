/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#ifndef OPCUAFILTERELEMENT
#define OPCUAFILTERELEMENT

#include "opcuaoperandbase.h"

#include <QOpcUaContentFilterElement>

#include <QObject>

QT_BEGIN_NAMESPACE

class QOpcUaClient;

class OpcUaFilterElement : public QObject {
    Q_OBJECT
    Q_PROPERTY(FilterOperator operator READ operatorType WRITE setOperatorType)
    Q_PROPERTY(OpcUaOperandBase* firstOperand READ firstOperand WRITE setFirstOperand)
    Q_PROPERTY(OpcUaOperandBase* secondOperand READ secondOperand WRITE setSecondOperand)

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

#endif // OPCUAFILTERELEMENT

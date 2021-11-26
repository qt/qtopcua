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

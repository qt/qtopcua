/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QOPCUACONTENTFILTERELEMENT_H
#define QOPCUACONTENTFILTERELEMENT_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

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
    bool operator==(const QOpcUaContentFilterElement &rhs) const;
    operator QVariant() const;
    ~QOpcUaContentFilterElement();

    // Specified in OPC-UA part 4, Tables 115 and 116
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
        BitwiseOr = 17
    };

    QOpcUaContentFilterElement &operator<<(FilterOperator op);
    QOpcUaContentFilterElement &operator<<(const QOpcUaSimpleAttributeOperand &op);
    QOpcUaContentFilterElement &operator<<(const QOpcUaAttributeOperand &op);
    QOpcUaContentFilterElement &operator<<(const QOpcUaLiteralOperand &op);
    QOpcUaContentFilterElement &operator<<(const QOpcUaElementOperand &op);


    QOpcUaContentFilterElement::FilterOperator filterOperator() const;
    void setFilterOperator(QOpcUaContentFilterElement::FilterOperator filterOperator);

    QVector<QVariant> filterOperands() const;
    QVector<QVariant> &filterOperandsRef();
    void setFilterOperands(const QVector<QVariant> &filterOperands);

private:
    QSharedDataPointer<QOpcUaContentFilterElementData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaContentFilterElement)

#endif // QOPCUACONTENTFILTERELEMENT_H

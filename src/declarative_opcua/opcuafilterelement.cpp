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

#include <private/opcuafilterelement_p.h>
#include <private/opcuaoperandbase_p.h>

#include <QOpcUaContentFilterElement>
#include <QLoggingCategory>

QT_BEGIN_NAMESPACE

/*!
    \qmltype FilterElement
    \inqmlmodule QtOpcUa
    \brief The OPC UA ContentFilterElement.
    \since QtOpcUa 5.13

    A content filter element contains an operator and operands.
    There are four different operator types which contain literal values, references to
    attributes of nodes or to other content filter elements.

    A combination of one or more content filter elements makes a content filter which is used
    by the server to filter data for the criteria defined by the content filter elements.
    For example, the \c where clause of an event filter is a content filter which is used to decide
    if a notification is generated for an event.

    \code
    QtOpcUa.FilterElement {
        operator: QtOpcUa.FilterElement.GreaterThanOrEqual
        firstOperand: QtOpcUa.SimpleAttributeOperand { ... }
        secondOperand: QtOpcUa.LiteralOperand { ... }
    }
    \endcode

    \sa EventFilter
*/

/*!
    \qmlproperty enumeration FilterElement::operatorType

    The filter operator.

    Possible operators for a FilterElement that are specified in
    OPC-UA part 4, Tables 115 and 116.

    \value FilterElement.Equals
    \value FilterElement.IsNull
    \value FilterElement.GreaterThan
    \value FilterElement.LessThan
    \value FilterElement.GreaterThanOrEqual
    \value FilterElement.LessThanOrEqual
    \value FilterElement.Like
    \value FilterElement.Not
    \value FilterElement.Between
    \value FilterElement.InList
    \value FilterElement.And
    \value FilterElement.Or
    \value FilterElement.Cast
    \value FilterElement.InView
    \value FilterElement.OfType
    \value FilterElement.RelatedTo
    \value FilterElement.BitwiseAnd
    \value FilterElement.BitwiseOr
*/

/*!
    \qmlproperty variant FilterElement::firstOperand

    First operand to be used with the operator.
    This can be one of \l SimpleAttributeOperand, \l AttributeOperand,
    \l LiteralOperand or \l ElementOperand.
*/

/*!
    \qmlproperty variant FilterElement::secondOperand

    Second operand to be used with the operator.
    This can be one of \l SimpleAttributeOperand, \l AttributeOperand,
    \l LiteralOperand or \l ElementOperand.
*/


Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_QML)

OpcUaFilterElement::OpcUaFilterElement(QObject *parent)
    : QObject(parent)
{
}

OpcUaFilterElement::~OpcUaFilterElement() = default;

QOpcUaContentFilterElement OpcUaFilterElement::toFilterElement(QOpcUaClient *client)
{
    QOpcUaContentFilterElement value;
    value.setFilterOperator(static_cast<QOpcUaContentFilterElement::FilterOperator>(operatorType()));
    value.filterOperandsRef().append(m_firstOperand->toCppVariant(client));
    value.filterOperandsRef().append(m_secondOperand->toCppVariant(client));
    return value;
}

OpcUaFilterElement::FilterOperator OpcUaFilterElement::operatorType() const
{
    return m_filterOperator;
}

void OpcUaFilterElement::setOperatorType(OpcUaFilterElement::FilterOperator filterOperator)
{
    m_filterOperator = filterOperator;
}

OpcUaOperandBase *OpcUaFilterElement::firstOperand() const
{
    return m_firstOperand;
}

void OpcUaFilterElement::setFirstOperand(OpcUaOperandBase *operand)
{
    if (m_firstOperand != operand) {
        m_firstOperand = operand;
        emit dataChanged();
    }
}

OpcUaOperandBase *OpcUaFilterElement::secondOperand() const
{
    return m_secondOperand;
}

void OpcUaFilterElement::setSecondOperand(OpcUaOperandBase *operand)
{
    if (m_secondOperand != operand) {
        m_secondOperand = operand;
        emit dataChanged();
    }
}

QT_END_NAMESPACE

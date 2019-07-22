/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#include "opcuaelementoperand.h"

QT_BEGIN_NAMESPACE

/*!
    \qmltype ElementOperand
    \inqmlmodule QtOpcUa
    \brief The OPC UA ElementOperand type.
    \since QtOpcUa 5.13

    The ElementOperand is defined in OPC-UA part 4, 7.4.4.2.
    It is used to identify another element in the filter by its index
    (the first element has the index 0).

    This is required to create complex filters, for example to reference
    the two operands of the AND operation in ((Severity > 500) AND (Message == "TestString")).
    The first step is to create content filter elements for the two conditions (Severity > 500)
    and (Message == "TestString"). A third content filter element is required to create an AND
    combination of the two conditions. It consists of the AND operator and two element operands
    with the indices of the two conditions created before:

    \code
    import QtOpcUa 5.13 as QtOpcUa

    QtOpcUa.EventFilter {
        select : [ ... ]
        where: [
            QtOpcUa.FilterElement {
                operator: QtOpcUa.FilterElement.GreaterThan
                firstOperand: QtOpcUA.SimpleAttributeOperand {
                    browsePath: [
                        QtOpcUa.NodeId {
                            identifier: "Severity"
                            ns: "http://opcfoundation.org/UA/"
                        }
                    ]
                }
                secondOperand: QtOpcUa.LiteralOperand {
                    value: 500
                    type: QtOpcUa.Constants.UInt16
                }
            }
            QtOpcUa.FilterElement {
                operator: QtOpcUa.FilterElement.Equals
                firstOperand: QtOpcUA.SimpleAttributeOperand {
                    browsePath: [
                        QtOpcUa.NodeId {
                            identifier: "Message"
                            ns: "http://opcfoundation.org/UA/"
                        }
                    ]
                }
                secondOperand: QtOpcUa.LiteralOperand {
                    value: "TestString"
                    type: QtOpcUa.Constants.String
                }
            }
            QtOpcUa.FilterElement {
                operator: QtOpcUa.FilterElement.And
                firstOperand: QtOpcUA.ElementOperand {
                    index: 0
                }
                secondOperand: QtOpcUa.ElementOperand {
                    index: 1
                }
            }
        ]
    }
    \endcode
*/

/*!
    \qmlproperty int ElementOperand::index

    Index of the filter element that is going to be used as operand.
*/

OpcUaElementOperand::OpcUaElementOperand(QObject *parent)
    : OpcUaOperandBase(parent)
{
}

OpcUaElementOperand::~OpcUaElementOperand() = default;

QVariant OpcUaElementOperand::toCppVariant(QOpcUaClient* client) const
{
    Q_UNUSED(client);
    QOpcUaElementOperand value(*this);
    return value;
}

quint32 OpcUaElementOperand::index() const
{
    return QOpcUaElementOperand::index();
}

void OpcUaElementOperand::setIndex(quint32 index)
{
    if (index != QOpcUaElementOperand::index()) {
        QOpcUaElementOperand::setIndex(index);
        emit dataChanged();
    }
}

QT_END_NAMESPACE

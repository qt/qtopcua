// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/opcuaelementoperand_p.h>

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
    import QtOpcUa as QtOpcUa

    QtOpcUa.EventFilter {
        select : [ ... ]
        where: [
            QtOpcUa.FilterElement {
                operator: QtOpcUa.FilterElement.GreaterThan
                firstOperand: QtOpcUa.SimpleAttributeOperand {
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
                firstOperand: QtOpcUa.SimpleAttributeOperand {
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
                firstOperand: QtOpcUa.ElementOperand {
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

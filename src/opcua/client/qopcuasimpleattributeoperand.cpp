// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuasimpleattributeoperand.h"
#include "qopcuaqualifiedname.h"

QT_BEGIN_NAMESPACE

using namespace Qt::Literals::StringLiterals;

/*!
    \class QOpcUaSimpleAttributeOperand
    \inmodule QtOpcUa
    \brief The OPC UA SimpleAttributeOperand type.

    The SimpleAttributeOperand is specified in OPC UA 1.05 part 4, 7.7.4.5.
    It is used when a node attribute is required as operand.

    For example, the following simple attribute operand represents the value
    of the "Severity" field of the base event type:
    \code
        QOpcUaSimpleAttributeOperand("Severity");
    \endcode
*/
class QOpcUaSimpleAttributeOperandData : public QSharedData
{
public:
    QString typeId{u"ns=0;i=2041"_s}; // BaseEventType
    QList<QOpcUaQualifiedName> browsePath;
    QOpcUa::NodeAttribute attributeId {QOpcUa::NodeAttribute::Value};
    QString indexRange;
};

/*!
    Default constructs a simple attribute operand with no parameters set.
*/
QOpcUaSimpleAttributeOperand::QOpcUaSimpleAttributeOperand()
    : data(new QOpcUaSimpleAttributeOperandData)
{
}

/*!
    Constructs a simple attribute operand from \a rhs.
*/
QOpcUaSimpleAttributeOperand::QOpcUaSimpleAttributeOperand(const QOpcUaSimpleAttributeOperand &rhs)
    : data(rhs.data)
{
}

/*!
    Constructs a simple attribute operand for attribute \a attributeId of the direct child with the browse name
    \a name in namespace \a namespaceIndex. \a typeId is the node id of a type definition node. The operand will
    be restricted to instances of type \a typeId or a subtype.
*/
QOpcUaSimpleAttributeOperand::QOpcUaSimpleAttributeOperand(const QString &name, quint16 namespaceIndex, const QString &typeId, QOpcUa::NodeAttribute attributeId)
    : data(new QOpcUaSimpleAttributeOperandData)
{
    browsePathRef().append(QOpcUaQualifiedName(namespaceIndex, name));
    setTypeId(typeId);
    setAttributeId(attributeId);
}

/*!
    Constructs a simple attribute operand for the attribute \a attributeId of an object or variable of type \a typeId.
    This can be used for requesting the ConditionId in an event filter as described in OPC UA 1.05 part 9, 5.5.2.
*/
QOpcUaSimpleAttributeOperand::QOpcUaSimpleAttributeOperand(QOpcUa::NodeAttribute attributeId, const QString &typeId)
    : data(new QOpcUaSimpleAttributeOperandData)
{
    setTypeId(typeId);
    setAttributeId(attributeId);
}

/*!
    Sets the values from \a rhs in this simple attribute operand.
*/
QOpcUaSimpleAttributeOperand &QOpcUaSimpleAttributeOperand::operator=(const QOpcUaSimpleAttributeOperand &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    \fn bool QOpcUaSimpleAttributeOperand::operator==(const QOpcUaSimpleAttributeOperand &lhs,
                                                      const QOpcUaSimpleAttributeOperand &rhs)

    Returns \c true if \a lhs has the same value as \a rhs.
*/
bool comparesEqual(const QOpcUaSimpleAttributeOperand &lhs,
                   const QOpcUaSimpleAttributeOperand &rhs) noexcept
{
    return lhs.attributeId() == rhs.attributeId() && rhs.browsePath() == rhs.browsePath() &&
            lhs.indexRange() == rhs.indexRange() && lhs.typeId() == rhs.typeId();
}

/*!
    Converts this simple attribute operand to \l QVariant.
*/
QOpcUaSimpleAttributeOperand::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QOpcUaSimpleAttributeOperand::~QOpcUaSimpleAttributeOperand()
{
}

/*!
    Returns the index range string.
*/
QString QOpcUaSimpleAttributeOperand::indexRange() const
{
    return data->indexRange;
}

/*!
    Sets the index range string used to identify a single value or subset of the attribute's value to \a indexRange.
*/
void QOpcUaSimpleAttributeOperand::setIndexRange(const QString &indexRange)
{
    data->indexRange = indexRange;
}

/*!
    Returns the attribute of the node \l browsePath is pointing to.
*/
QOpcUa::NodeAttribute QOpcUaSimpleAttributeOperand::attributeId() const
{
    return data->attributeId;
}

/*!
    Sets the attribute id to \a attributeId.
*/
void QOpcUaSimpleAttributeOperand::setAttributeId(QOpcUa::NodeAttribute attributeId)
{
    data->attributeId = attributeId;
}

/*!
    Returns the relative path to a node starting from \l typeId.
*/
QList<QOpcUaQualifiedName> QOpcUaSimpleAttributeOperand::browsePath() const
{
    return data->browsePath;
}

/*!
    Returns a reference to the browse path.

    \sa browsePath()
*/
QList<QOpcUaQualifiedName> &QOpcUaSimpleAttributeOperand::browsePathRef()
{
    return data->browsePath;
}

/*!
    Sets the browse path to the node holding the attribute to \a browsePath.
*/
void QOpcUaSimpleAttributeOperand::setBrowsePath(const QList<QOpcUaQualifiedName> &browsePath)
{
    data->browsePath = browsePath;
}

/*!
    Returns the node id of a type definition node.
*/
QString QOpcUaSimpleAttributeOperand::typeId() const
{
    return data->typeId;
}

/*!
    Sets the node id of the type definition node to \a typeId. The operand will be of the type or one of its subtypes.
*/
void QOpcUaSimpleAttributeOperand::setTypeId(const QString &typeId)
{
    data->typeId = typeId;
}

/*!
    \fn bool QOpcUaSimpleAttributeOperand::operator!=(const QOpcUaSimpleAttributeOperand &lhs,
                                                      const QOpcUaSimpleAttributeOperand &rhs)
    \since 6.7

    Returns \c true if \a lhs has a different value than \a rhs.
*/

QT_END_NAMESPACE

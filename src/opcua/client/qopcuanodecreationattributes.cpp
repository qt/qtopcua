// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuanodecreationattributes.h"

#include "qopcuanodecreationattributes_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaNodeCreationAttributes
    \inmodule QtOpcUa
    \brief This class manages attribute values for node creation.

    This class holds values for node attributes that are passed to the server
    when creating a new node in the OPC UA address space.

    The following node attributes are supported by the different node classes:

    \table
    \header
    \li Attribute
    \li Object
    \li Variable
    \li Method
    \li ObjectType
    \li VariableType
    \li ReferenceType
    \li DataType
    \li View
    \row
    \li AccessLevel
    \li
    \li X
    \li
    \li
    \li
    \li
    \li
    \li
    \row
    \li ArrayDimensions
    \li
    \li X
    \li
    \li
    \li X
    \li
    \li
    \li
    \row
    \li ContainsNoLoops
    \li
    \li
    \li
    \li
    \li
    \li
    \li
    \li X
    \row
    \li DataType
    \li
    \li X
    \li
    \li
    \li X
    \li
    \li
    \li
    \row
    \li Description
    \li X
    \li X
    \li X
    \li X
    \li X
    \li X
    \li X
    \li X
    \row
    \li DisplayName
    \li X
    \li X
    \li X
    \li X
    \li X
    \li X
    \li X
    \li X
    \row
    \li EventNotifier
    \li X
    \li
    \li
    \li
    \li
    \li
    \li
    \li
    \row
    \li Executable
    \li
    \li
    \li X
    \li
    \li
    \li
    \li
    \li
    \row
    \li Historizing
    \li
    \li X
    \li
    \li
    \li
    \li
    \li
    \li
    \row
    \li InverseName
    \li
    \li
    \li
    \li
    \li
    \li X
    \li
    \li
    \row
    \li IsAbstract
    \li
    \li
    \li
    \li X
    \li X
    \li X
    \li X
    \li
    \row
    \li MinimumSamplingInterval
    \li
    \li X
    \li
    \li
    \li
    \li
    \li
    \li
    \row
    \li Symmetric
    \li
    \li
    \li
    \li
    \li
    \li X
    \li
    \li
    \row
    \li UserAccessLevel
    \li
    \li X
    \li
    \li
    \li
    \li
    \li
    \li
    \row
    \li UserExecutable
    \li
    \li
    \li X
    \li
    \li
    \li
    \li
    \li
    \row
    \li UserWriteMask
    \li X
    \li X
    \li X
    \li X
    \li X
    \li X
    \li X
    \li X
    \row
    \li Value
    \li
    \li X
    \li
    \li
    \li X
    \li
    \li
    \li
    \row
    \li ValueRank
    \li
    \li X
    \li
    \li
    \li X
    \li
    \li
    \li
    \row
    \li WriteMask
    \li X
    \li X
    \li X
    \li X
    \li X
    \li X
    \li X
    \li X
    \endtable

    \sa QOpcUaClient::addNode()
*/

/*!
    Default constructs a node creation attributes object with no parameters set.
*/
QOpcUaNodeCreationAttributes::QOpcUaNodeCreationAttributes()
    : data(new QOpcUaNodeCreationAttributesPrivate())
{}

/*!
    Constructs a node creation attributes object with the values of \a other.
*/
QOpcUaNodeCreationAttributes::QOpcUaNodeCreationAttributes(const QOpcUaNodeCreationAttributes &other)
    : data(other.data)
{}

/*!
    Assigns the value of \a rhs to this node creation attributes object.
*/
QOpcUaNodeCreationAttributes &QOpcUaNodeCreationAttributes::operator=(const QOpcUaNodeCreationAttributes &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

QOpcUaNodeCreationAttributes::~QOpcUaNodeCreationAttributes()
{}

/*!
    Returns the value for the AccessLevel attribute.
*/
QOpcUa::AccessLevel QOpcUaNodeCreationAttributes::accessLevel() const
{
    return data->accessLevel;
}

/*!
    Sets the value for the AccessLevel attribute to \a accessLevel.
*/
void QOpcUaNodeCreationAttributes::setAccessLevel(QOpcUa::AccessLevel accessLevel)
{
    data->accessLevel = accessLevel;
    data->setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask::AccessLevel);
}

/*!
    Returns \c true if a value for the AccessLevel attribute is set.
*/
bool QOpcUaNodeCreationAttributes::hasAccessLevel() const
{
    return data->attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask::AccessLevel);
}

/*!
    Returns the value for the ArrayDimensions attribute.
*/
QList<quint32> QOpcUaNodeCreationAttributes::arrayDimensions() const
{
    return data->arrayDimensions;
}

/*!
    Sets the value for the ArrayDimensions attribute to \a arrayDimensions.
*/
void QOpcUaNodeCreationAttributes::setArrayDimensions(const QList<quint32> &arrayDimensions)
{
    data->arrayDimensions = arrayDimensions;
    data->setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask::ArrayDimensions);
}

/*!
    Returns \c true if a value for the ArrayDimensions attribute is set.
*/
bool QOpcUaNodeCreationAttributes::hasArrayDimensions() const
{
    return data->attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask::ArrayDimensions);
}

/*!
    Returns the value for the ContainsNoLoops attribute.
*/
bool QOpcUaNodeCreationAttributes::containsNoLoops() const
{
    return data->containsNoLoops;
}

/*!
    Sets the value for the ContainsNoLoops attribute to \a containsNoLoops.
*/
void QOpcUaNodeCreationAttributes::setContainsNoLoops(bool containsNoLoops)
{
    data->containsNoLoops = containsNoLoops;
    data->setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask::ContainsNoLoops);
}

/*!
    Returns \c true if a value for the ContainsNoLoops attribute is set.
*/
bool QOpcUaNodeCreationAttributes::hasContainsNoLoops() const
{
    return data->attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask::ContainsNoLoops);
}

/*!
    Returns the value for the DataType attribute.
*/
QString QOpcUaNodeCreationAttributes::dataTypeId() const
{
    return data->dataTypeId;
}

/*!
    Sets the value for the DataType attribute to \a dataTypeId.
*/
void QOpcUaNodeCreationAttributes::setDataTypeId(const QString &dataTypeId)
{
    data->dataTypeId = dataTypeId;
    data->setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask::DataType);
}

/*!
    Returns \c true if a value for the DataType attribute is set.
*/
bool QOpcUaNodeCreationAttributes::hasDataTypeId() const
{
    return data->attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask::DataType);
}

/*!
    Returns the value for the Description attribute.
*/
QOpcUaLocalizedText QOpcUaNodeCreationAttributes::description() const
{
    return data->description;
}

/*!
    Sets the value for the Description attribute to \a description.
*/
void QOpcUaNodeCreationAttributes::setDescription(const QOpcUaLocalizedText &description)
{
    data->description = description;
    data->setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask::Description);
}

/*!
    Returns \c true if a value for the Description attribute is set.
*/
bool QOpcUaNodeCreationAttributes::hasDescription() const
{
    return data->attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask::Description);
}

/*!
    Returns the value for the DisplayName attribute.
*/
QOpcUaLocalizedText QOpcUaNodeCreationAttributes::displayName() const
{
    return data->displayName;
}

/*!
    Sets the value for the DisplayName attribute to \a displayName.
*/
void QOpcUaNodeCreationAttributes::setDisplayName(const QOpcUaLocalizedText &displayName)
{
    data->displayName = displayName;
    data->setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask::DisplayName);
}

/*!
    Returns \c true if a value for the DisplayName attribute is set.
*/
bool QOpcUaNodeCreationAttributes::hasDisplayName() const
{
    return data->attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask::DisplayName);
}

/*!
    Returns the value for the EventNotifier attribute.
*/
QOpcUa::EventNotifier QOpcUaNodeCreationAttributes::eventNotifier() const
{
    return data->eventNotifier;
}

/*!
    Sets the value for the EventNotifier attribute to \a eventNotifier.
*/
void QOpcUaNodeCreationAttributes::setEventNotifier(QOpcUa::EventNotifier eventNotifier)
{
    data->eventNotifier = eventNotifier;
    data->setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask::EventNotifier);
}

/*!
    Returns \c true if a value for the EventNotifier attribute is set.
*/
bool QOpcUaNodeCreationAttributes::hasEventNotifier() const
{
    return data->attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask::EventNotifier);
}

/*!
    Returns the value for the Executable attribute.
*/
bool QOpcUaNodeCreationAttributes::executable() const
{
    return data->executable;
}

/*!
    Sets the value for the Executable attribute to \a executable.
*/
void QOpcUaNodeCreationAttributes::setExecutable(bool executable)
{
    data->executable = executable;
    data->setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask::Executable);
}

/*!
    Returns \c true if a value for the Executable attribute is set.
*/
bool QOpcUaNodeCreationAttributes::hasExecutable() const
{
    return data->attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask::Executable);
}

/*!
    Returns the value for the Historizing attribute.
*/
bool QOpcUaNodeCreationAttributes::historizing() const
{
    return data->historizing;
}

/*!
    Sets the value for the Historizing attribute to \a historizing.
*/
void QOpcUaNodeCreationAttributes::setHistorizing(bool historizing)
{
    data->historizing = historizing;
    data->setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask::Historizing);
}

/*!
    Returns \c true if a value for the Historizing attribute is set.
*/
bool QOpcUaNodeCreationAttributes::hasHistorizing() const
{
    return data->attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask::Historizing);
}

/*!
    Returns the value for the InverseName attribute.
*/
QOpcUaLocalizedText QOpcUaNodeCreationAttributes::inverseName() const
{
    return data->inverseName;
}

/*!
    Sets the value for the InverseName attribute to \a inverseName.
*/
void QOpcUaNodeCreationAttributes::setInverseName(const QOpcUaLocalizedText &inverseName)
{
    data->inverseName = inverseName;
    data->setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask::InverseName);
}

/*!
    Returns \c true if a value for the InverseName attribute is set.
*/
bool QOpcUaNodeCreationAttributes::hasInverseName() const
{
    return data->attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask::InverseName);
}

/*!
    Returns the value for the IsAbstract attribute.
*/
bool QOpcUaNodeCreationAttributes::isAbstract() const
{
    return data->isAbstract;
}

/*!
    Sets the value for the IsAbstract attribute to \a isAbstract.
*/
void QOpcUaNodeCreationAttributes::setIsAbstract(bool isAbstract)
{
    data->isAbstract = isAbstract;
    data->setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask::IsAbstract);
}

/*!
    Returns \c true if a value for the IsAbstract attribute is set.
*/
bool QOpcUaNodeCreationAttributes::hasIsAbstract() const
{
    return data->attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask::IsAbstract);
}

/*!
    Returns the value for the MinimumSamplingInterval attribute.
*/
double QOpcUaNodeCreationAttributes::minimumSamplingInterval() const
{
    return data->minimumSamplingInterval;
}

/*!
    Sets the value for the MinimumSamplingInterval attribute to \a minimumSamplingInterval.
*/
void QOpcUaNodeCreationAttributes::setMinimumSamplingInterval(double minimumSamplingInterval)
{
    data->minimumSamplingInterval = minimumSamplingInterval;
    data->setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask::MinimumSamplingInterval);
}

/*!
    Returns \c true if a value for the MinimumSamplingInterval attribute is set.
*/
bool QOpcUaNodeCreationAttributes::hasMinimumSamplingInterval() const
{
    return data->attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask::MinimumSamplingInterval);
}

/*!
    Returns the value for the Symmetric attribute.
*/
bool QOpcUaNodeCreationAttributes::symmetric() const
{
    return data->symmetric;
}

/*!
    Sets the value for the Symmetric attribute to \a symmetric.
*/
void QOpcUaNodeCreationAttributes::setSymmetric(bool symmetric)
{
    data->symmetric = symmetric;
    data->setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask::Symmetric);
}

/*!
    Returns \c true if a value for the Symmetric attribute is set.
*/
bool QOpcUaNodeCreationAttributes::hasSymmetric() const
{
    return data->attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask::Symmetric);
}

/*!
    Returns the value for the UserAccessLevel attribute.
*/
QOpcUa::AccessLevel QOpcUaNodeCreationAttributes::userAccessLevel() const
{
    return data->userAccessLevel;
}

/*!
    Sets the value for the UserAccessLevel attribute to \a userAccessLevel.
*/
void QOpcUaNodeCreationAttributes::setUserAccessLevel(QOpcUa::AccessLevel userAccessLevel)
{
    data->userAccessLevel = userAccessLevel;
    data->setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask::UserAccessLevel);
}

/*!
    Returns \c true if a value for the UserAccessLevel attribute is set.
*/
bool QOpcUaNodeCreationAttributes::hasUserAccessLevel() const
{
    return data->attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask::UserAccessLevel);
}

/*!
    Returns the value for the UserExecutable attribute.
*/
bool QOpcUaNodeCreationAttributes::userExecutable() const
{
    return data->userExecutable;
}

/*!
    Sets the value for the UserExecutable attribute to \a userExecutable.
*/
void QOpcUaNodeCreationAttributes::setUserExecutable(bool userExecutable)
{
    data->userExecutable = userExecutable;
    data->setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask::UserExecutable);
}

/*!
    Returns \c true if a value for the UserExecutable attribute is set.
*/
bool QOpcUaNodeCreationAttributes::hasUserExecutable() const
{
    return data->attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask::UserExecutable);
}

/*!
    Returns the value for the UserWriteMask attribute.
*/
QOpcUa::WriteMask QOpcUaNodeCreationAttributes::userWriteMask() const
{
    return data->userWriteMask;
}

/*!
    Sets the value for the UserWriteMask attribute to \a userWriteMask.
*/
void QOpcUaNodeCreationAttributes::setUserWriteMask(QOpcUa::WriteMask userWriteMask)
{
    data->userWriteMask = userWriteMask;
    data->setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask::UserWriteMask);
}

/*!
    Returns \c true if a value for the UserWriteMask attribute is set.
*/
bool QOpcUaNodeCreationAttributes::hasUserWriteMask() const
{
    return data->attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask::UserWriteMask);
}

/*!
    Returns the value for the ValueRank attribute.
*/
qint32 QOpcUaNodeCreationAttributes::valueRank() const
{
    return data->valueRank;
}

/*!
    Sets the value for the ValueRank attribute to \a valueRank.
*/
void QOpcUaNodeCreationAttributes::setValueRank(qint32 valueRank)
{
    data->valueRank = valueRank;
    data->setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask::ValueRank);
}

/*!
    Returns \c true if a value for the ValueRank attribute is set.
*/
bool QOpcUaNodeCreationAttributes::hasValueRank() const
{
    return data->attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask::ValueRank);
}

/*!
    Returns the value for the WriteMask attribute.
*/
QOpcUa::WriteMask QOpcUaNodeCreationAttributes::writeMask() const
{
    return data->writeMask;
}

/*!
    Sets the value for the WriteMask attribute to \a writeMask.
*/
void QOpcUaNodeCreationAttributes::setWriteMask(QOpcUa::WriteMask writeMask)
{
    data->writeMask = writeMask;
    data->setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask::WriteMask);
}

/*!
    Returns \c true if a value for the WriteMask attribute is set.
*/
bool QOpcUaNodeCreationAttributes::hasWriteMask() const
{
    return data->attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask::WriteMask);
}

/*!
    Returns the value for the Value attribute.
*/
QVariant QOpcUaNodeCreationAttributes::value() const
{
    return data->value;
}

/*!
    Sets the value for the Value attribute to \a value of type \a type.
*/
void QOpcUaNodeCreationAttributes::setValue(const QVariant &value, QOpcUa::Types type)
{
    data->value = value;
    data->valueType = type;
    data->setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask::Value);
}

/*!
    Returns \c true if a value for the Value attribute is set.
*/
bool QOpcUaNodeCreationAttributes::hasValue() const
{
    return data->attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask::Value);
}

/*!
    Returns the type of the value attribute.
*/
QOpcUa::Types QOpcUaNodeCreationAttributes::valueType() const
{
    return data->valueType;
}

QT_END_NAMESPACE

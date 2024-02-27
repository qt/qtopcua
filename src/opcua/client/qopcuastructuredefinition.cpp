// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuastructuredefinition.h"

#include <QtOpcUa/qopcuastructurefield.h>

#include <QtCore/qlist.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaStructureDefinition
    \inmodule QtOpcUa
    \since 6.7
    \brief The OPC UA StructureDefinition type.

    This is the Qt OPC UA representation for the OPC UA StructureDefinition type defined in OPC UA part 3.
    It describes the structure of a structured type.
*/

class QOpcUaStructureDefinitionData : public QSharedData
{
public:
    QString defaultEncodingId;
    QString baseDataType;
    QOpcUaStructureDefinition::StructureType structureType = QOpcUaStructureDefinition::StructureType::Structure;
    QList<QOpcUaStructureField> fields;
};

QT_DEFINE_QESDP_SPECIALIZATION_DTOR(QOpcUaStructureDefinitionData);

/*!
    Default constructs a structure definition with no parameters set.
*/
QOpcUaStructureDefinition::QOpcUaStructureDefinition()
    : data(new QOpcUaStructureDefinitionData)
{
}

/*!
    Constructs a structure definition from \a rhs.
*/
QOpcUaStructureDefinition::QOpcUaStructureDefinition(const QOpcUaStructureDefinition &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this structure definition.
*/
QOpcUaStructureDefinition &QOpcUaStructureDefinition::operator=(const QOpcUaStructureDefinition &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    \fn QOpcUaStructureDefinition::QOpcUaStructureDefinition(QOpcUaStructureDefinition &&other)

    Move-constructs a new structure definition from \a other.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn QOpcUaStructureDefinition &QOpcUaStructureDefinition::operator=(QOpcUaStructureDefinition &&other)

    Move-assigns \a other to this QOpcUaStructureDefinition instance.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn void QOpcUaStructureDefinition::swap(QOpcUaStructureDefinition &other)

    Swaps structure definition object \a other with this structure definition
    object. This operation is very fast and never fails.
*/

/*!
    \fn bool QOpcUaStructureDefinition::operator!=(const QOpcUaStructureDefinition &lhs, const QOpcUaStructureDefinition &rhs) noexcept

    Returns \c true if \a lhs is not equal to \a rhs.
*/

/*!
    \fn bool QOpcUaStructureDefinition::operator==(const QOpcUaStructureDefinition &lhs, const QOpcUaStructureDefinition &rhs) noexcept

    Returns \c true if \a lhs is equal to \a rhs.
*/
bool comparesEqual(const QOpcUaStructureDefinition &lhs, const QOpcUaStructureDefinition &rhs) noexcept
{
    return lhs.data->defaultEncodingId == rhs.defaultEncodingId() &&
            lhs.data->baseDataType == rhs.baseDataType() &&
            lhs.data->structureType == rhs.structureType() &&
            lhs.data->fields == rhs.fields();
}

/*!
    Converts this structure definition to \l QVariant.
*/
QOpcUaStructureDefinition::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
    Destroys this structure definition object.
*/
QOpcUaStructureDefinition::~QOpcUaStructureDefinition()
{
}

/*!
    Returns the default encoding node ID for the struct type.
*/
QString QOpcUaStructureDefinition::defaultEncodingId() const
{
    return data->defaultEncodingId;
}

/*!
    Sets the default encoding node ID of the struct type to \a defaultEncodingId.
*/
void QOpcUaStructureDefinition::setDefaultEncodingId(const QString &defaultEncodingId)
{
    if (defaultEncodingId != data->defaultEncodingId) {
        data.detach();
        data->defaultEncodingId = defaultEncodingId;
    }
}

/*!
    Returns the node id of the base data type of the struct type.
*/
QString QOpcUaStructureDefinition::baseDataType() const
{
    return data->baseDataType;
}

/*!
    Sets the base data type node id of the type to \a baseDataType.
*/
void QOpcUaStructureDefinition::setBaseDataType(const QString &baseDataType)
{
    if (baseDataType != data->baseDataType) {
        data.detach();
        data->baseDataType = baseDataType;
    }
}

/*!
    Returns the structure type of the struct type.
*/
QOpcUaStructureDefinition::StructureType QOpcUaStructureDefinition::structureType() const
{
    return data->structureType;
}

/*!
    Sets the structure type to \a structureType.
*/
void QOpcUaStructureDefinition::setStructureType(const QOpcUaStructureDefinition::StructureType &structureType)
{
    if (structureType != data->structureType) {
        data.detach();
        data->structureType = structureType;
    }
}

/*!
    Returns the fields of the struct type.
*/
QList<QOpcUaStructureField> QOpcUaStructureDefinition::fields() const
{
    return data->fields;
}

/*!
    Sets the fields of the struct type to \a fields.
*/
void QOpcUaStructureDefinition::setFields(const QList<QOpcUaStructureField> &fields)
{
    if (fields != data->fields) {
        data.detach();
        data->fields = fields;
    }
}

QT_END_NAMESPACE

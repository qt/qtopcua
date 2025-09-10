// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuastructurefield.h"

#include <QtOpcUa/qopcualocalizedtext.h>

#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaStructureField
    \inmodule QtOpcUa
    \since 6.7
    \brief The OPC UA StructureField type.

    This is the Qt OPC UA representation for the OPC UA StructureField type defined in OPC UA part 3.
    It describes a field of a structured type.
*/

class QOpcUaStructureFieldData : public QSharedData
{
public:
    QString name;
    QOpcUaLocalizedText description;
    QString dataType;
    qint32 valueRank = 0;
    QList<quint32> arrayDimensions;
    quint32 maxStringLength = 0;
    bool isOptional = false;
};

QT_DEFINE_QESDP_SPECIALIZATION_DTOR(QOpcUaStructureFieldData);

/*!
    Constructs a structure field with \a valueRank.
*/
QOpcUaStructureField::QOpcUaStructureField(qint32 valueRank)
    : data(new QOpcUaStructureFieldData)
{
    data->valueRank = valueRank;
}

/*!
    Constructs a structure field from \a other.
*/
QOpcUaStructureField::QOpcUaStructureField(const QOpcUaStructureField &other)
    : data(other.data)
{
}

/*!
    Sets the values from \a other in this structure field.
*/
QOpcUaStructureField &QOpcUaStructureField::operator=(const QOpcUaStructureField &other)
{
    if (this != &other)
        data.operator=(other.data);
    return *this;
}

/*!
    \fn QOpcUaStructureField::QOpcUaStructureField(QOpcUaStructureField &&other)

    Move-constructs a new structure field from \a other.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn QOpcUaStructureField &QOpcUaStructureField::operator=(QOpcUaStructureField &&other)

    Move-assigns \a other to this QOpcUaStructureField instance.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn void QOpcUaStructureField::swap(QOpcUaStructureField &other)

    Swaps structure field object \a other with this structure field
    object. This operation is very fast and never fails.
*/

/*!
    \fn bool QOpcUaStructureField::operator!=(const QOpcUaStructureField &lhs, const QOpcUaStructureField &rhs)

    Returns \c true \a lhs is not equal to \a rhs.
*/

/*!
    \fn bool QOpcUaStructureField::operator==(const QOpcUaStructureField &lhs, const QOpcUaStructureField &rhs)

    Returns \c true \a lhs is equal to \a rhs.
*/
bool comparesEqual(const QOpcUaStructureField &lhs, const QOpcUaStructureField &rhs) noexcept
{
    return lhs.data->name == rhs.name() &&
           lhs.data->description == rhs.description() &&
           lhs.data->dataType == rhs.dataType() &&
           lhs.data->valueRank == rhs.valueRank() &&
           lhs.data->arrayDimensions == rhs.arrayDimensions() &&
           lhs.data->maxStringLength == rhs.maxStringLength() &&
           lhs.data->isOptional == rhs.isOptional();
}

/*!
    Converts this structure field to \l QVariant.
*/
QOpcUaStructureField::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
    Destroys this structure field object.
*/
QOpcUaStructureField::~QOpcUaStructureField()
{
}

/*!
    Returns the name of the struct field.
*/
QString QOpcUaStructureField::name() const
{
    return data->name;
}

/*!
    Sets the name of the struct field to \a name.
*/
void QOpcUaStructureField::setName(const QString &name)
{
    if (name != data->name) {
        data.detach();
        data->name = name;
    }
}

/*!
    Returns the description of the struct field.
*/
QOpcUaLocalizedText QOpcUaStructureField::description() const
{
    return data->description;
}

/*!
    Sets the description of the struct field to \a description.
*/
void QOpcUaStructureField::setDescription(const QOpcUaLocalizedText &description)
{
    if (!(description == data->description)) {
        data.detach();
        data->description = description;
    }
}

/*!
    Returns the data type node ID of the struct field.
*/
QString QOpcUaStructureField::dataType() const
{
    return data->dataType;
}

/*!
    Sets the data type node ID of the struct field to \a dataTypeId.
*/
void QOpcUaStructureField::setDataType(const QString &dataTypeId)
{
    if (dataTypeId != data->dataType) {
        data.detach();
        data->dataType = dataTypeId;
    }
}

/*!
    Returns the value rank of the struct field.
*/
qint32 QOpcUaStructureField::valueRank() const
{
    return data->valueRank;
}

/*!
    Sets the value rank of the struct field to \a valueRank.
*/
void QOpcUaStructureField::setValueRank(qint32 valueRank)
{
    if (valueRank != data->valueRank) {
        data.detach();
        data->valueRank = valueRank;
    }
}

/*!
    Returns the array dimenstions of the struct field.
*/
QList<quint32> QOpcUaStructureField::arrayDimensions() const
{
    return data->arrayDimensions;
}

/*!
    Sets the array dimensions of the struct field to \a arrayDimensions.
*/
void QOpcUaStructureField::setArrayDimensions(const QList<quint32> &arrayDimensions)
{
    if (arrayDimensions != data->arrayDimensions) {
        data.detach();
        data->arrayDimensions = arrayDimensions;
    }
}

/*!
    Returns the maximum string length of the struct field.
*/
quint32 QOpcUaStructureField::maxStringLength() const
{
    return data->maxStringLength;
}

/*!
    Sets the maximum string length of the struct field to \a maxStringLength.
*/
void QOpcUaStructureField::setMaxStringLength(quint32 maxStringLength)
{
    if (maxStringLength != data->maxStringLength) {
        data.detach();
        data->maxStringLength = maxStringLength;
    }
}

/*!
    Returns \c true if the struct field is optional.
*/
bool QOpcUaStructureField::isOptional() const
{
    return data->isOptional;
}

/*!
    Sets the optional flag of the struct field to \a isOptional.
*/
void QOpcUaStructureField::setIsOptional(bool isOptional)
{
    if (isOptional != data->isOptional) {
        data.detach();
        data->isOptional = isOptional;
    }
}

QT_END_NAMESPACE

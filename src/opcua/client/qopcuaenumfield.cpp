// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuaenumfield.h"

#include <QtOpcUa/qopcualocalizedtext.h>

#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaEnumField
    \inmodule QtOpcUa
    \since 6.7
    \brief The OPC UA StructureDefinition type.

    This is the Qt OPC UA representation for the OPC UA EnumField type defined in OPC UA part 3.
    It describes a field of an enum type.
*/

class QOpcUaEnumFieldData : public QSharedData
{
public:
    qint64 value = 0;
    QOpcUaLocalizedText displayName;
    QOpcUaLocalizedText description;
    QString name;
};

QT_DEFINE_QESDP_SPECIALIZATION_DTOR(QOpcUaEnumFieldData);

/*!
    Default constructs an enum field with no parameters set.
*/
QOpcUaEnumField::QOpcUaEnumField()
    : data(new QOpcUaEnumFieldData)
{
}

/*!
    Constructs an enum field from \a other.
*/
QOpcUaEnumField::QOpcUaEnumField(const QOpcUaEnumField &other)
    : data(other.data)
{
}

/*!
    Sets the values from \a rhs in this enum field.
*/
QOpcUaEnumField &QOpcUaEnumField::operator=(const QOpcUaEnumField &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    \fn QOpcUaEnumField::QOpcUaEnumField(QOpcUaEnumField &&other)

    Move-constructs a new enum field from \a other.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn QOpcUaEnumField &QOpcUaEnumField::operator=(QOpcUaEnumField &&other)

    Move-assigns \a other to this QOpcUaEnumField instance.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn void QOpcUaEnumField::swap(QOpcUaEnumField &other)

    Swaps enum field object \a other with this enum field
    object. This operation is very fast and never fails.
*/

/*!
    \fn bool QOpcUaEnumField::operator!=(const QOpcUaEnumField &lhs, const QOpcUaEnumField &rhs) noexcept

    Returns \c true if \a lhs is not equal to \a rhs.
*/

/*!
    \fn bool QOpcUaEnumField::operator==(const QOpcUaEnumField &lhs, const QOpcUaEnumField &rhs) noexcept

    Returns \c true if \a lhs is equal to \a rhs.
*/
bool comparesEqual(const QOpcUaEnumField &lhs, const QOpcUaEnumField &rhs) noexcept
{
    return lhs.data->name == rhs.name();
}

/*!
    Converts this enum field to \l QVariant.
*/
QOpcUaEnumField::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
    Destroy this enum definition object.
*/
QOpcUaEnumField::~QOpcUaEnumField()
{
}

/*!
    Returns the enum value of the enum field.
*/
qint64 QOpcUaEnumField::value() const
{
    return data->value;
}

/*!
    Sets the enum value of the enum field to \a value.
*/
void QOpcUaEnumField::setValue(qint64 value)
{
    if (value != data->value) {
        data.detach();
        data->value = value;
    }
}

/*!
    Returns the display name of the enum field.
*/
QOpcUaLocalizedText QOpcUaEnumField::displayName() const
{
    return data->displayName;
}

/*!
    Sets the display name of the enum field to \a displayName.
*/
void QOpcUaEnumField::setDisplayName(const QOpcUaLocalizedText &displayName)
{
    if (!(displayName == data->displayName)) {
        data.detach();
        data->displayName = displayName;
    }
}

/*!
    Returns the description of the enum field.
*/
QOpcUaLocalizedText QOpcUaEnumField::description() const
{
    return data->description;
}

/*!
    Sets the description of the enum field to \a description.
*/
void QOpcUaEnumField::setDescription(const QOpcUaLocalizedText &description)
{
    if (!(description == data->description)) {
        data.detach();
        data->description = description;
    }
}

/*!
    Returns the name of the enum field.
*/
QString QOpcUaEnumField::name() const
{
    return data->name;
}

/*!
    Sets the name of the enum field to \a name.
*/
void QOpcUaEnumField::setName(const QString &name)
{
    if (name != data->name) {
        data.detach();
        data->name = name;
    }
}

QT_END_NAMESPACE

// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuaenumdefinition.h"

#include <QtCore/qlist.h>
#include <QtCore/qvariant.h>

#include <QtOpcUa/qopcuaenumfield.h>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaEnumDefinition
    \inmodule QtOpcUa
    \since 6.7
    \brief The OPC UA EnumDefinition type.

    This is the Qt OPC UA representation for the OPC UA EnumDefinition type defined in OPC UA part 3.
    It describes an enumerated type.
*/

class QOpcUaEnumDefinitionData : public QSharedData
{
public:
    QList<QOpcUaEnumField> fields;
};

QT_DEFINE_QESDP_SPECIALIZATION_DTOR(QOpcUaEnumDefinitionData);

/*!
    Default constructs an enum definition with no parameters set.
*/
QOpcUaEnumDefinition::QOpcUaEnumDefinition()
    : data(new QOpcUaEnumDefinitionData)
{
}

/*!
    Constructs an enum definition from \a other.
*/
QOpcUaEnumDefinition::QOpcUaEnumDefinition(const QOpcUaEnumDefinition &other)
    : data(other.data)
{
}

/*!
    Sets the values from \a rhs in this enum definition.
*/
QOpcUaEnumDefinition &QOpcUaEnumDefinition::operator=(const QOpcUaEnumDefinition &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    \fn QOpcUaEnumDefinition::QOpcUaEnumDefinition(QOpcUaEnumDefinition &&other)

    Move-constructs a new enum definition from \a other.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn QOpcUaEnumDefinition &QOpcUaEnumDefinition::operator=(QOpcUaEnumDefinition &&other)

    Move-assigns \a other to this QOpcUaEnumDefinition instance.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn void QOpcUaEnumDefinition::swap(QOpcUaEnumDefinition &other)

    Swaps enum definition object \a other with this enum definition
    object. This operation is very fast and never fails.
*/

/*!
    \fn bool QOpcUaEnumDefinition::operator!=(const QOpcUaEnumDefinition &lhs, const QOpcUaEnumDefinition &rhs) noexcept

    Returns \c true if \a lhs is not equal to \a rhs.
*/

/*!
    \fn bool QOpcUaEnumDefinition::operator==(const QOpcUaEnumDefinition &lhs, const QOpcUaEnumDefinition &rhs) noexcept

    Returns \c true if \a lhs is equal to \a rhs.
*/
bool comparesEqual(const QOpcUaEnumDefinition &lhs, const QOpcUaEnumDefinition &rhs) noexcept
{
    return lhs.data->fields == rhs.fields();
}

/*!
    Converts this enum definition to \l QVariant.
*/
QOpcUaEnumDefinition::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
    Destroys this enum definition object.
*/
QOpcUaEnumDefinition::~QOpcUaEnumDefinition()
{
}

/*!
    Returns the fields of the enum type.
 */
QList<QOpcUaEnumField> QOpcUaEnumDefinition::fields() const
{
    return data->fields;
}

/*!
    Sets the fields of the enum type to \a fields.
*/
void QOpcUaEnumDefinition::setFields(const QList<QOpcUaEnumField> &fields)
{
    if (fields != data->fields) {
        data.detach();
        data->fields = fields;
    }
}

QT_END_NAMESPACE

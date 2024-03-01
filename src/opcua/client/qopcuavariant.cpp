// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuavariant.h"

#include <QtCore/qdatetime.h>
#include <QtCore/qlist.h>
#include <QtCore/quuid.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QOpcUaVariantData : public QSharedData
{
public:
    void updateIsArray()
    {
        const QMetaType metaType = value.metaType();
        const auto name = QUtf8StringView(metaType.name());
        isArray = (name.size() > 6 && name.first(6) == QUtf8StringView("QList<"))
                  || name == QUtf8StringView("QStringList")
                  || name == QUtf8StringView("QVariantList");
    }

    QVariant value;
    QOpcUaVariant::ValueType valueType = QOpcUaVariant::ValueType::Unknown;
    QList<qint32> arrayDimensions;
    bool isArray = false;
};

QT_DEFINE_QESDP_SPECIALIZATION_DTOR(QOpcUaVariantData)

/*!
    \class QOpcUaVariant
    \inmodule QtOpcUa
    \since 6.7
    \brief The OPC UA Variant.

    The OPC UA variant is an union of all built-in OPC UA types and also contains
    information about the array dimensions associated with the value.
    This class is currently only supported by \l QOpcUaBinaryDataEncoding and
    \l QOpcUaGenericStructHandler.
*/

/*!
    \enum QOpcUaVariant::ValueType

    This enum contains the possible value types of an OPC UA variant.

    \value Unknown
    \value Boolean
    \value SByte
    \value Byte
    \value Int16
    \value UInt16
    \value Int32
    \value UInt32,
    \value Int64
    \value UInt64
    \value Float
    \value Double
    \value String
    \value DateTime
    \value Guid
    \value ByteString
    \value XmlElement
    \value NodeId
    \value ExpandedNodeId
    \value StatusCode
    \value QualifiedName
    \value LocalizedText
    \value ExtensionObject
    \value DataValue
    \value Variant
    \value DiagnosticInfo
*/

/*!
    \fn QOpcUaVariant::QOpcUaVariant(QOpcUaVariant &&other)

    Move-constructs a new OPC UA variant from \a other.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn QOpcUaVariant &QOpcUaVariant::operator=(QOpcUaVariant &&other)

    Move-assigns \a other to this QOpcUaVariant instance.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn void QOpcUaVariant::swap(QOpcUaVariant &other)

    Swaps enum definition object \a other with this OPC UA variant
    object. This operation is very fast and never fails.
*/

/*!
    \fn bool QOpcUaVariant::operator!=(const QOpcUaVariant &lhs, const QOpcUaVariant &rhs) noexcept

    Returns \c true if \a lhs is not equal to \a rhs.
*/

/*!
    \fn bool QOpcUaVariant::operator==(const QOpcUaVariant &lhs, const QOpcUaVariant &rhs) noexcept

    Returns \c true if \a lhs is equal to \a rhs.
*/

/*!
    Default constructs a new OPC UA variant.
*/
QOpcUaVariant::QOpcUaVariant()
    : data(new QOpcUaVariantData)
{
}

/*!
    Constructs a new OPC UA variant from \a other.
*/
QOpcUaVariant::QOpcUaVariant(const QOpcUaVariant &other)
    : data(other.data)
{
}

/*!
    Constructs a new OPC UA variant of type \a type with value \a value.

    Scalar values must be passed as a \l QVariant containing a value of \a type.
    Array values must be passed as a \l QVariant containing a \l QList of \a type.
*/
QOpcUaVariant::QOpcUaVariant(ValueType type, const QVariant &value)
    : data(new QOpcUaVariantData)
{
    setValue(type, value);
}

/*!
    Constructs a new OPC UA variant of type \a type with value \a value and array dimensions \a arrayDimensions.

    Scalar values must be passed as a \l QVariant containing a value of \a type.
    Array values must be passed as a \l QVariant containing a \l QList of \a type.
*/
QOpcUaVariant::QOpcUaVariant(ValueType type, const QVariant &value, const QList<qint32> arrayDimensions)
    : data(new QOpcUaVariantData)
{
    setValue(type, value, arrayDimensions);
}

/*!
    Destroys this OPC UA variant.
*/
QOpcUaVariant::~QOpcUaVariant()
{
}

/*!
    Sets the values from \a rhs in this OPC UA variant.
*/
QOpcUaVariant &QOpcUaVariant::operator=(const QOpcUaVariant &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}


bool comparesEqual(const QOpcUaVariant &lhs, const QOpcUaVariant &rhs) noexcept
{
    return lhs.data->value == rhs.data->value &&
           lhs.data->valueType == rhs.data->valueType &&
           lhs.data->arrayDimensions == rhs.data->arrayDimensions;
}

/*!
    Returns the value of this OPC UA variant.
*/
QVariant QOpcUaVariant::value() const
{
    return data->value;
}

/*!
    Sets the value of this OPC UA variant to \a value and the type to \a type.

    Scalar values must be passed as a \l QVariant containing a value of \a type.
    Array values must be passed as a \l QVariant containing a \l QList of \a type.
*/
void QOpcUaVariant::setValue(ValueType type, const QVariant &value)
{
    if (type != data->valueType || value != data->value || !data->arrayDimensions.isEmpty()) {
        data.detach();
        data->valueType = type;
        data->value = value;
        data->updateIsArray();
    }
}

/*!
    Sets the value of this OPC UA variant to \a value, the type to \a type and the array dimensions to \a arrayDimensions.

    Scalar values must be passed as a \l QVariant containing a value of \a type.
    Array values must be passed as a \l QVariant containing a \l QList of \a type.
*/
void QOpcUaVariant::setValue(ValueType type, const QVariant &value, const QList<qint32> &arrayDimensions)
{
    if (type != data->valueType || value != data->value || arrayDimensions != data->arrayDimensions) {
        data.detach();
        data->valueType = type;
        data->value = value;
        data->arrayDimensions = arrayDimensions;
        data->updateIsArray();
    }
}

/*!
    Returns the value type of this OPC UA variant.
*/
QOpcUaVariant::ValueType QOpcUaVariant::type() const
{
    return data->valueType;
}

/*!
    Returns \c true if this OPC UA variant contains an array value.
*/
bool QOpcUaVariant::isArray() const
{
    return data->isArray;
}

/*!
    Returns the array dimensions of this OPC UA variant.
*/
QList<qint32> QOpcUaVariant::arrayDimensions() const
{
    return data->arrayDimensions;
}

/*!
    Sets the array dimensions to \a arrayDimensions.
*/
void QOpcUaVariant::setArrayDimensions(const QList<qint32> &arrayDimensions)
{
    if (data->arrayDimensions != arrayDimensions) {
        data.detach();
        data->arrayDimensions = arrayDimensions;
    }
}

/*!
    Returns a \l QVariant containing this OPC UA variant.
*/
QOpcUaVariant::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QT_END_NAMESPACE

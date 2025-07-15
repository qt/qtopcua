// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtOpcUa/qopcuastructuredefinition.h>
#include <QtOpcUa/qopcuagenericstructvalue.h>

#include <QtCore/qdebug.h>
#include <QtCore/qhash.h>
#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

using namespace Qt::Literals::StringLiterals;

/*!
    \class QOpcUaGenericStructValue
    \inmodule QtOpcUa
    \since 6.7

    \brief Holds the value of a generic OPC UA structured type.

    This class holds a generic OPC UA struct value which consists of named fields with their values
    which can be built-in types or their sub types as well as other nested generic structs.
    Some meta information like the type ID and the structure definition of the type contained in
    an object of this class is also included.

    It is used as return type for the decoding result and as input type for the encoding method of
    \l QOpcUaGenericStructHandler.

    For a struct with optional fields, only the fields that were specified are contained in the
    \a fields() list. To omit an optional field during encoding, it must not appear in the fields()
    list.
    For structs derived from the Union type, only one of the fields must be specified.
    The only entry in the fields() list of a decoded type determines which field (if any) of the
    type definition was specified.

    Example:
    \code
    // Decode an extension object
    QOpcUaGenericStructValue value = decoder.decode(extensionObject, success);

    // Print the value of a mandatory field
    qDebug() << value.fields().value("MyField").value<QOpcUaLocalizedText>();

    // Print the value of an optional field
    if (value.fields().contains("MyOptionalField"))
        qDebug() << value.fields().value("MyField").value<QOpcUaQualifiedName>();

    // Get a nested struct for a field
    const auto unionMember = value.fields().value("MyUnionMember").value<QOpcUaGenericStructValue>();

    // Print the specified field
    if (unionMember.fields().contains("UnionMember1"))
        qDebug() << unionMember.fields().value("UnionMember1").toInt();
    else if (unionMember.fields().contains("UnionMember2"))
        qDebug() << unionMember.fields().value("UnionMember2").toDouble();
    else
        qDebug() << "Empty union";
    \endcode
*/

class QOpcUaGenericStructValueData : public QSharedData
{
public:
    QOpcUaGenericStructValueData() = default;
    QOpcUaGenericStructValueData(const QString &typeName, const QString &typeId,
                                 const QOpcUaStructureDefinition &definition, const QHash<QString, QVariant> &fields)
        : typeName(typeName)
        , typeId(typeId)
        , structureDefinition(definition)
        , fields(fields)
    {}
    QString typeName;
    QString typeId;
    QOpcUaStructureDefinition structureDefinition;
    QHash<QString, QVariant> fields;
};

QT_DEFINE_QESDP_SPECIALIZATION_DTOR(QOpcUaGenericStructValueData)

/*!
   Constructs a generic struct value.
*/
QOpcUaGenericStructValue::QOpcUaGenericStructValue()
    : data(new QOpcUaGenericStructValueData())
{
}

/*!
    Destroys this generic struct value object.
*/
QOpcUaGenericStructValue::~QOpcUaGenericStructValue()
{
}

/*!
    Constructs a generic struct value from \a typeName, \a typeId and \a definition.
*/
QOpcUaGenericStructValue::QOpcUaGenericStructValue(const QString &typeName, const QString &typeId,
                                                   const QOpcUaStructureDefinition &definition)
    : data(new QOpcUaGenericStructValueData(typeName, typeId, definition, {}))
{
}

/*!
    Constructs a generic struct value from \a typeName, \a typeId, \a definition and \a fields.
*/
QOpcUaGenericStructValue::QOpcUaGenericStructValue(const QString &typeName, const QString &typeId,
                                                   const QOpcUaStructureDefinition &definition, const QHash<QString, QVariant> &fields)
    : data(new QOpcUaGenericStructValueData(typeName, typeId, definition, fields))
{
}

/*!
    Constructs a generic struct value from \a other.
*/
QOpcUaGenericStructValue::QOpcUaGenericStructValue(const QOpcUaGenericStructValue &other)
    : data(other.data)
{
}

/*!
    Sets the value of \a rhs in this generic struct value.
*/
QOpcUaGenericStructValue &QOpcUaGenericStructValue::operator=(const QOpcUaGenericStructValue &rhs)
{
    if (this != &rhs)
        this->data = rhs.data;

    return *this;
}

/*!
    \fn QOpcUaGenericStructValue::QOpcUaGenericStructValue(QOpcUaGenericStructValue &&other)

    Move-constructs a new generic struct value from \a other.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn QOpcUaGenericStructValue &QOpcUaGenericStructValue::operator=(QOpcUaGenericStructValue &&other)

    Move-assigns \a other to this QOpcUaGenericStructValue instance.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn void QOpcUaGenericStructValue::swap(QOpcUaGenericStructValue &other)

    Swaps generic struct value object \a other with this generic struct value
    object. This operation is very fast and never fails.
*/

/*!
    \fn bool QOpcUaGenericStructValue::operator!=(const QOpcUaGenericStructValue &lhs, const QOpcUaGenericStructValue &rhs)

    Returns \c true if \a lhs is not equal to \a rhs.
*/

/*!
    \fn bool QOpcUaGenericStructValue::operator==(const QOpcUaGenericStructValue &lhs, const QOpcUaGenericStructValue &rhs)

    Returns \c true if \a lhs is equal to \a rhs.
*/
bool comparesEqual(const QOpcUaGenericStructValue &lhs, const QOpcUaGenericStructValue &rhs) noexcept
{
    return lhs.typeName() == rhs.typeName() && lhs.typeId() == rhs.typeId() &&
            lhs.structureDefinition() == rhs.structureDefinition() && lhs.fields() == rhs.fields();
}

/*!
    Returns a \l QVariant containing this generic struct value.
*/
QOpcUaGenericStructValue::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
    Returns the type name for this generic struct value.
*/
QString QOpcUaGenericStructValue::typeName() const
{
    return data->typeName;
}

/*!
    Sets the type name for this generic struct value to \a typeName.
*/
void QOpcUaGenericStructValue::setTypeName(const QString &typeName)
{
    if (typeName != data->typeName) {
        data.detach();
        data->typeName = typeName;
    }
}

/*!
    Returns the type node id for this generic struct value.
*/
QString QOpcUaGenericStructValue::typeId() const
{
    return data->typeId;
}

/*!
    Sets the type node id for this generic struct value to \a typeId.
*/
void QOpcUaGenericStructValue::setTypeId(const QString &typeId)
{
    if (typeId != data->typeId) {
        data.detach();
        data->typeId = typeId;
    }
}

/*!
    Returns the structure definition for this generic struct value.
*/
QOpcUaStructureDefinition QOpcUaGenericStructValue::structureDefinition() const
{
    return data->structureDefinition;
}

/*!
    Sets the structure definition for this generic struct value to \a structureDefinition.
*/
void QOpcUaGenericStructValue::setStructureDefinition(const QOpcUaStructureDefinition &structureDefinition)
{
    if (structureDefinition != data->structureDefinition) {
        data.detach();
        data->structureDefinition = structureDefinition;
    }
}

/*!
    Returns the fields of this generic struct value.
*/
QHash<QString, QVariant> QOpcUaGenericStructValue::fields() const
{
    return data->fields;
}

/*!
    Returns a reference to the fields of this generic struct value.
*/
QHash<QString, QVariant> &QOpcUaGenericStructValue::fieldsRef()
{
    return data->fields;
}

/*!
    Sets the fields of this generic struct value to \a fields.
*/
void QOpcUaGenericStructValue::setFields(const QHash<QString, QVariant> &fields)
{
    if (fields != data->fields) {
        data.detach();
        data->fields = fields;
    }
}

#ifndef QT_NO_DEBUG_STREAM
/*!
    Returns a string representation for this generic struct value.
 */
QString QOpcUaGenericStructValue::toString() const
{
    QString out;
    QDebug dbg(&out);
    dbg.nospace().noquote() << *this;
    return out;
}

/*!
    \fn QDebug QOpcUaGenericStructValue::operator<<(QDebug debug, const QOpcUaGenericStructValue &s)
    Outputs the string representation of \a s into \a debug.
 */
QDebug operator<<(QDebug debug, const QOpcUaGenericStructValue &s)
{
    QDebugStateSaver saver(debug);

    QString structType = u"Struct"_s;
    if (s.structureDefinition().structureType() == QOpcUaStructureDefinition::StructureType::StructureWithOptionalFields)
        structType = u"StructWithOptionalFields"_s;
    else if (s.structureDefinition().structureType() == QOpcUaStructureDefinition::StructureType::Union)
        structType = u"Union"_s;

    debug.noquote().nospace();
    debug << structType << " " << s.typeName() << " (";
    for (auto it = s.data->fields.constBegin(); it != s.data->fields.constEnd(); ++it)
        debug << (it == s.data->fields.constBegin() ? "" : " ") << it.key() << ": " << it.value();
    debug << ")";
    return debug;
}
#endif

QT_END_NAMESPACE

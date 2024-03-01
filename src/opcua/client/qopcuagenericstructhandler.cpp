// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtOpcUa/qopcuastructuredefinition.h>
#include <QtOpcUa/qopcuagenericstructvalue.h>

#include "private/qopcuagenericstructhandler_p.h"
#include <QtOpcUa/qopcuabinarydataencoding.h>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaGenericStructHandler
    \inmodule QtOpcUa
    \since 6.7

    \brief Reads a server's data types and decodes/encodes generic structs from/to extension objects.

    The binary data encoding used in OPC UA was designed with a small message size in mind and doesn't
    contain any information about the structure of the data.
    This means that a decoder must known the structure of the encoded data in advance to be able to
    decode a data buffer.

    Since OPC UA 1.04, nodes of the DataType node class may have the DataTypeDefinition attribute
    which contain information about the fields of structured types and the mapping of enum values
    to names. Together with the knowledge about how to decode built-in types, this allows a client to
    decode generic custom structured types without relying on outside knowledge.

    QOpcUaGenericStructHandler traverses the type hierarchy of a server by following the HasSubtype
    references starting from BaseDataType and reads the DataTypeDefinition attribute of the nodes.

    For structured types where a \l QOpcUaStructureDefinition value is present in the DataTypeDefinition
    attribute, automatic decoding of extension objects containing them is available. Fields with a built-in
    type or a type where a C++ data class exists are deserialized to the corresponding Qt OPC UA type,
    other generic structs are serialized to a nested \l QOpcUaGenericStructValue.
    All nested generic struct values must have a \l QOpcUaStructureDefinition in the server or decoding fails.

    The same conditions apply to encoding a custom struct.

    Example for decoding a custom struct:

    \code
    QOpcUaGenericStructHandler handler(opcuaClient);
    handler.initialize();

    QObject::connect(&handler, &QOpcUaGenericStructHandler::initializedChanged, [opcuaClient, &handler](bool initialized) {
        if (!initialized)
            return;

        auto node = opcuaClient->node("ns=4;i=3003"); // A custom struct test node in the open62541-testserver
        node->readValueAttribute();

        QObject::connect(node, &QOpcUaNode::attributeRead, [node, &handler](QOpcUa::NodeAttributes attr) {
            if (!attr.testFlag(QOpcUa::NodeAttribute::Value) || node->valueAttributeError() != QOpcUa::UaStatusCode::Good)
                return;

            auto extObj = node->valueAttribute().value<QOpcUaExtensionObject>();
            qDebug() << "Got object of type" << handler.typeNameForBinaryEncodingId(extObj.encodingTypeId());

            const auto result = handler.decode(extObj);

            if (!result)
                return;

            qDebug() << *result;
        });
    });
    \endcode

    Example for encoding a custom struct:

    \code
    QOpcUaGenericStructHandler handler(opcuaClient);
    handler.initialize();

    QObject::connect(&handler, &QOpcUaGenericStructHandler::initializedChanged, [opcuaClient, &handler](bool initialized) {
        if (!initialized)
            return;

        QOpcUaGenericStructValue value = handler.createGenericStructValueForTypeId("ns=4;i=3006");
        value.fieldsRef()["MandatoryMember"] = 23.0;
        value.fieldsRef()["OptionalMember"] = 42.0;

        const auto ext = handler.encode(value);

        if (!ext)
            return;

        // Use the extension object to write a node's value attribute, in a method parameter, etc...
    });
    \endcode
*/

/*!
    \enum QOpcUaGenericStructHandler::DataTypeKind

    This enum type specifies data type kind of a data type node.

    \value Unknown
           The type node id is unknown.
    \value Struct
           The type node id belongs to a structured type.
    \value Enum
           The type node id belongs to an enum type.
    \value Other
           The type node id belongs to a type which is not a struct or enum (other built-in types or their subtypes)
*/

/*!
    \fn QOpcUaGenericStructHandler::initializedChanged(bool initialized)

    This signal is emitted when the initialization process has finished.
    \a initialized indicates if the initialization was successful.
*/

/*!
    Constructs a generic struct handler for \a client.
*/
QOpcUaGenericStructHandler::QOpcUaGenericStructHandler(QOpcUaClient *client, QObject *parent)
    : QObject(*new QOpcUaGenericStructHandlerPrivate(client), parent)
{

}

QOpcUaGenericStructHandler::~QOpcUaGenericStructHandler() = default;

/*!
    Starts the data type hierarchy traversal.
    Success or failure is reported in the \l initializedChanged signal.

    Returns \c false if the operation can't be started.
*/
bool QOpcUaGenericStructHandler::initialize()
{
    Q_D(QOpcUaGenericStructHandler);
    return d->initialize();
}

/*!
    Decodes \a extensionObject to a \l QOpcUaGenericStructValue. If the decoder fails, \c std::nullopt is returned.
*/
std::optional<QOpcUaGenericStructValue> QOpcUaGenericStructHandler::decode(const QOpcUaExtensionObject &extensionObject) const
{
    bool success = false;
    auto result = d_func()->decode(extensionObject, success);
    if (!success)
        return std::nullopt;
    return result;
}

/*!
    Returns \a value encoded as a \l QOpcUaExtensionObject, or \c std::nullopt if the value could not be encoded.
 */
std::optional<QOpcUaExtensionObject> QOpcUaGenericStructHandler::encode(const QOpcUaGenericStructValue &value)
{
    QOpcUaExtensionObject output;
    if (!d_func()->encode(value, output))
        return std::nullopt;
    return output;
}

/*!
    Returns a generic struct value pre-filled with the struct definition, type id and type name
    corresponding to \a typeId.
    For all mandatory fields, an invalid placeholder \l QVariant will be inserted.
 */
QOpcUaGenericStructValue QOpcUaGenericStructHandler::createGenericStructValueForTypeId(const QString &typeId)
{
    return d_func()->createGenericStructValueForTypeId(typeId);
}

/*!
    Returns the \l QOpcUaStructureDefinition for the binary encoding node id \a id.
    If the node id is unknown or does not belong to a struct type, a default constructed value is returned.
*/
QOpcUaStructureDefinition QOpcUaGenericStructHandler::structureDefinitionForBinaryEncodingId(const QString &id) const
{
    return d_func()->structureDefinitionForBinaryEncodingId(id);
}

/*!
    Returns the \l QOpcUaStructureDefinition for the type node id \a id.
    If the node id is unknown or does not belong to a struct type, a default constructed value is returned.
*/
QOpcUaStructureDefinition QOpcUaGenericStructHandler::structureDefinitionForTypeId(const QString &id) const
{
    return d_func()->structureDefinitionForTypeId(id);
}

/*!
    Returns the \ QOpcUaEnumDefinition for the type node id \a id.
    If the node id is unknown or does not belong to an enum type, a default constructed value is returned.
*/
QOpcUaEnumDefinition QOpcUaGenericStructHandler::enumDefinitionForTypeId(const QString &id) const
{
    return d_func()->enumDefinitionForTypeId(id);
}

/*!
    Returns the type name belonging to the binary encoding node id \a id.
    If the node id is unknown or does not belong to a struct type, an empty string is returned.
*/
QString QOpcUaGenericStructHandler::typeNameForBinaryEncodingId(const QString &id) const
{
    return d_func()->typeNameForBinaryEncodingId(id);
}

/*!
    Returns the type name belonging to a data type node identified by type node id \a id.
    If the node id is unknown, an empty string is returned.
*/
QString QOpcUaGenericStructHandler::typeNameForTypeId(const QString &id) const
{
    return d_func()->typeNameForTypeId(id);
}

/*!
 * Returns true if the data type described by \a id is abstract.
 */
bool QOpcUaGenericStructHandler::isAbstractTypeId(const QString &id) const
{
    return d_func()->isAbstractTypeId(id);
}

/*!
    Adds the custom structure definition \a definition to the known types.
    This can be used to support custom structures the server doesn't expose a StructureDefinition for.
    The parameters \a definition, \a typeId and \a name are required for proper decoding
    and encoding. If \a isAbstract is set, the type can't be encoded and decoded.

    Returns \c true if the structure definition was successfully added.
 */
bool QOpcUaGenericStructHandler::addCustomStructureDefinition(const QOpcUaStructureDefinition &definition, const QString &typeId,
                                                              const QString &name, QOpcUa::IsAbstract isAbstract)
{
    return d_func()->addCustomStructureDefinition(definition, typeId, name, isAbstract);
}

/*!
    Adds the custom enum definition \a definition to the known types.
    This can be used to support custom structures the server doesn't expose a StructureDefinition for.
    The parameters \a definition, \a typeId and \a name are required for proper decoding
    and encoding. If \a isAbstract is set, the type can't be encoded and decoded.

    Returns \c true if the enum definition was successfully added.
 */
bool QOpcUaGenericStructHandler::addCustomEnumDefinition(const QOpcUaEnumDefinition &definition, const QString &typeId,
                                                         const QString &name, QOpcUa::IsAbstract isAbstract)
{
    return d_func()->addCustomEnumDefinition(definition, typeId, name, isAbstract);
}

/*!
    \since 6.7

    Returns \c true if the generic struct handler is initialized.
*/
bool QOpcUaGenericStructHandler::initialized() const
{
    return d_func()->initialized();
}

/*!
    Returns the data type kind for the type node id \a id.
*/
QOpcUaGenericStructHandler::DataTypeKind QOpcUaGenericStructHandler::dataTypeKindForTypeId(const QString &id) const
{
    return d_func()->dataTypeKindForTypeId(id);
}

/*!
    Returns the type node id associated with the binary encoding id \a id.
*/
QString QOpcUaGenericStructHandler::typeIdForBinaryEncodingId(const QString &id) const
{
    return d_func()->typeIdForBinaryEncodingId(id);
}

QT_END_NAMESPACE

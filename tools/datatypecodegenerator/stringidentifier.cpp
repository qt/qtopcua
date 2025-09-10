// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "stringidentifier.h"

const QString StringIdentifier::baseTypeIdentifier = "BaseType";
const QString StringIdentifier::binaryTypeIdentifier = "opc";
const QString StringIdentifier::bitIdentifier = "Bit";
const QString StringIdentifier::booleanIdentifier = "Boolean";
const QString StringIdentifier::byteIdentifier = "Byte";
const QString StringIdentifier::sbyteIdentifier = "SByte";
const QString StringIdentifier::byteStringIdentifier = "ByteString";
const QString StringIdentifier::charArrayIdentifier = "CharArray";
const QString StringIdentifier::cppIdentifier = ".cpp";
const QString StringIdentifier::datetimeIdentifier = "DateTime";
const QString StringIdentifier::defaultByteOrderIdentifier = "DefaultByteOrder";
const QString StringIdentifier::documentationIdentifier = "Documentation";
const QString StringIdentifier::doubleIdentifier = "Double";
const QString StringIdentifier::enumeratedTypeIdentifier = "EnumeratedType";
const QString StringIdentifier::enumeratedValueIdentifier = "EnumeratedValue";
const QString StringIdentifier::fieldIdentifier = "Field";
const QString StringIdentifier::floatIdentifier = "Float";
const QString StringIdentifier::guidIdentifier = "Guid";
const QString StringIdentifier::headerIdentifier = ".h";
const QString StringIdentifier::importIdentifier = "Import";
const QString StringIdentifier::integerIdentifier = "Int";
const QString StringIdentifier::lengthIdentifier = "Length";
const QString StringIdentifier::lengthFieldIdentifier = "LengthField";
const QString StringIdentifier::lengthInBitsIdentifier = "LengthInBits";
const QString StringIdentifier::locationIdentifier = "Location";
const QString StringIdentifier::mainIdentifier = "main.cpp";
const QString StringIdentifier::makelistIdentifier = "CMakeLists.txt";
const QString StringIdentifier::nameIdentifier = "Name";
const QString StringIdentifier::nameSpaceIdentifier = "Namespace";
const QString StringIdentifier::namespaceZeroIdentifier = "http://opcfoundation.org/UA/";
const QString StringIdentifier::opaqueTypeIdentifier = "OpaqueType";
const QString StringIdentifier::projectIdentifier = "cmakeqt6";
const QString StringIdentifier::qByteArrayIdentifier = "QByteArray";
const QString StringIdentifier::qStringIdentifier = "QString";
const QString StringIdentifier::uaStatusCodeIdentifier = "StatusCode";
const QString StringIdentifier::reservedIdentifier = "Reserved";
const QString StringIdentifier::specifiedIdentifier = "Specified";
const QString StringIdentifier::structuredTypeIdentifier = "StructuredType";
const QString StringIdentifier::switchFieldIdentifier = "SwitchField";
const QString StringIdentifier::switchValueIdentifier = "SwitchValue";
const QString StringIdentifier::targetNamespaceIdentifier = "TargetNamespace";
const QString StringIdentifier::typeDictionaryIdentifier = "TypeDictionary";
const QString StringIdentifier::typeNameIdentifier = "TypeName";
const QString StringIdentifier::valueIdentifier = "Value";
const QString StringIdentifier::xmlElementIdentifier = "XmlElement";
const QString StringIdentifier::opcBitIdentifier = "opc:Bit";

const QList<StringIdentifier::OpcUaPrecodedType> StringIdentifier::opcUaPrecodedTypes{
    OpcUaPrecodedType("ApplicationRecordDataType", "QOpcUaApplicationRecordDataType",
                      "QOpcUaApplicationRecordDataType"),
    OpcUaPrecodedType("Argument", "QOpcUaArgument", "QOpcUaArgument"),
    OpcUaPrecodedType("AxisInformation", "QOpcUaAxisInformation", "QOpcUaAxisInformation"),
    OpcUaPrecodedType("ComplexNumber", "QOpcUaComplexNumber", "QOpcUaComplexNumber"),
    OpcUaPrecodedType("DoubleComplexNumber", "QOpcUaDoubleComplexNumber", "QOpcUaDoubleComplexNumber"),
    OpcUaPrecodedType("EUInformation", "QOpcUaEUInformation", "QOpcUaEUInformation"),
    OpcUaPrecodedType("ExpandedNodeId", "QOpcUaExpandedNodeId", "QOpcUaExpandedNodeId"),
    OpcUaPrecodedType("ExtensionObject", "QOpcUaExtensionObject", "QOpcUaExtensionObject"),
    OpcUaPrecodedType("LocalizedText", "QOpcUaLocalizedText", "QOpcUaLocalizedText"),
    OpcUaPrecodedType("NodeId", QString(), "QString", "QString"),
    OpcUaPrecodedType("QualifiedName", "QOpcUaQualifiedName", "QOpcUaQualifiedName"),
    OpcUaPrecodedType("Range", "QOpcUaRange", "QOpcUaRange"),
    OpcUaPrecodedType("StatusCode", "qopcuatype.h", "QOpcUa::UaStatusCode"),
    OpcUaPrecodedType("XVType", "QOpcUaXValue", "QOpcUaXValue"),
    OpcUaPrecodedType("DiagnosticInfo", "QOpcUaDiagnosticInfo", "QOpcUaDiagnosticInfo"),
    OpcUaPrecodedType("StructureField", "QOpcUaStructureField", "QOpcUaStructureField"),
    OpcUaPrecodedType("StructureDefinition", "QOpcUaStructureDefinition", "QOpcUaStructureDefinition"),
    OpcUaPrecodedType("EnumField", "QOpcUaEnumField", "QOpcUaEnumField"),
    OpcUaPrecodedType("EnumDefinition", "QOpcUaEnumDefinition", "QOpcUaEnumDefinition"),
    OpcUaPrecodedType("DataValue", "QOpcUaDataValue", "QOpcUaDataValue"),
    OpcUaPrecodedType("Variant", "QOpcUaVariant", "QOpcUaVariant")
};

const QList<QString> StringIdentifier::buildInTypesWithBitMask = {"DiagnosticInfo",
                                                                  "LocalizedText",
                                                                  "Variant",
                                                                  "DataValue",
                                                                  "Variant"};

const QMap<QString, QString> StringIdentifier::typeNameDataTypeConverter
    = {{"opc:Bit", "bool"},
       {"opc:Boolean", "bool"},
       {"opc:Byte", "quint8"},
       {"opc:ByteString", "QByteArray"},
       {"opc:CharArray", "QString"},
       {"opc:DateTime", "QDateTime"},
       {"opc:Double", "double"},
       {"opc:Float", "float"},
       {"opc:Int16", "qint16"},
       {"opc:Int32", "qint32"},
       {"opc:Int64", "qint64"},
       {"opc:SByte", "qint8"},
       {"opc:String", "QString"},
       {"opc:UInt16", "quint16"},
       {"opc:UInt32", "quint32"},
       {"opc:UInt64", "quint64"},
       {"opc:Guid", "QUuid"},
       {"ua:XmlElement", "QString"}};

const QList<QString> StringIdentifier::illegalNames
    = {"Boolean",  "boolean",  "Int16",      "int16",      "Float",      "float",
       "Datetime", "datetime", "byteString", "ByteString", "XmlElement", "xmlElement",
       "byte",     "Byte",     "SByte",      "sByte",      "Int32",      "int32",
       "Int64",    "int64",    "Double",     "double",     "String",     "string"};

const QSet<QString> StringIdentifier::precodedTypesWithDebugOperator = { "LocalizedText", "NodeId", "QualifiedName" };

QString StringIdentifier::OpcUaPrecodedType::name() const
{
    return m_name;
}

QString StringIdentifier::OpcUaPrecodedType::filename() const
{
    return m_filename;
}

QString StringIdentifier::OpcUaPrecodedType::className() const
{
    return m_className;
}

QString StringIdentifier::OpcUaPrecodedType::deEncoderName() const
{
    return m_deEncoderName;
}

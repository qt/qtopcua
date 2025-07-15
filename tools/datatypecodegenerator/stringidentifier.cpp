// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "stringidentifier.h"

using namespace Qt::Literals::StringLiterals;

const QString StringIdentifier::baseTypeIdentifier = u"BaseType"_s;
const QString StringIdentifier::binaryTypeIdentifier = u"opc"_s;
const QString StringIdentifier::bitIdentifier = u"Bit"_s;
const QString StringIdentifier::booleanIdentifier = u"Boolean"_s;
const QString StringIdentifier::byteIdentifier = u"Byte"_s;
const QString StringIdentifier::sbyteIdentifier = u"SByte"_s;
const QString StringIdentifier::byteStringIdentifier = u"ByteString"_s;
const QString StringIdentifier::charArrayIdentifier = u"CharArray"_s;
const QString StringIdentifier::cppIdentifier = u".cpp"_s;
const QString StringIdentifier::datetimeIdentifier = u"DateTime"_s;
const QString StringIdentifier::defaultByteOrderIdentifier = u"DefaultByteOrder"_s;
const QString StringIdentifier::documentationIdentifier = u"Documentation"_s;
const QString StringIdentifier::doubleIdentifier = u"Double"_s;
const QString StringIdentifier::enumeratedTypeIdentifier = u"EnumeratedType"_s;
const QString StringIdentifier::enumeratedValueIdentifier = u"EnumeratedValue"_s;
const QString StringIdentifier::fieldIdentifier = u"Field"_s;
const QString StringIdentifier::floatIdentifier = u"Float"_s;
const QString StringIdentifier::guidIdentifier = u"Guid"_s;
const QString StringIdentifier::headerIdentifier = u".h"_s;
const QString StringIdentifier::importIdentifier = u"Import"_s;
const QString StringIdentifier::integerIdentifier = u"Int"_s;
const QString StringIdentifier::lengthIdentifier = u"Length"_s;
const QString StringIdentifier::lengthFieldIdentifier = u"LengthField"_s;
const QString StringIdentifier::lengthInBitsIdentifier = u"LengthInBits"_s;
const QString StringIdentifier::locationIdentifier = u"Location"_s;
const QString StringIdentifier::mainIdentifier = u"main.cpp"_s;
const QString StringIdentifier::makelistIdentifier = u"CMakeLists.txt"_s;
const QString StringIdentifier::nameIdentifier = u"Name"_s;
const QString StringIdentifier::nameSpaceIdentifier = u"Namespace"_s;
const QString StringIdentifier::namespaceZeroIdentifier = u"http://opcfoundation.org/UA/"_s;
const QString StringIdentifier::opaqueTypeIdentifier = u"OpaqueType"_s;
const QString StringIdentifier::projectIdentifier = u"cmakeqt6"_s;
const QString StringIdentifier::qByteArrayIdentifier = u"QByteArray"_s;
const QString StringIdentifier::qStringIdentifier = u"QString"_s;
const QString StringIdentifier::uaStatusCodeIdentifier = u"StatusCode"_s;
const QString StringIdentifier::reservedIdentifier = u"Reserved"_s;
const QString StringIdentifier::specifiedIdentifier = u"Specified"_s;
const QString StringIdentifier::structuredTypeIdentifier = u"StructuredType"_s;
const QString StringIdentifier::switchFieldIdentifier = u"SwitchField"_s;
const QString StringIdentifier::switchValueIdentifier = u"SwitchValue"_s;
const QString StringIdentifier::targetNamespaceIdentifier = u"TargetNamespace"_s;
const QString StringIdentifier::typeDictionaryIdentifier = u"TypeDictionary"_s;
const QString StringIdentifier::typeNameIdentifier = u"TypeName"_s;
const QString StringIdentifier::valueIdentifier = u"Value"_s;
const QString StringIdentifier::xmlElementIdentifier = u"XmlElement"_s;
const QString StringIdentifier::opcBitIdentifier = u"opc:Bit"_s;

const QList<StringIdentifier::OpcUaPrecodedType> StringIdentifier::opcUaPrecodedTypes{
    OpcUaPrecodedType(u"ApplicationRecordDataType"_s, u"QOpcUaApplicationRecordDataType"_s,
                      u"QOpcUaApplicationRecordDataType"_s),
    OpcUaPrecodedType(u"Argument"_s, u"QOpcUaArgument"_s, u"QOpcUaArgument"_s),
    OpcUaPrecodedType(u"AxisInformation"_s, u"QOpcUaAxisInformation"_s, u"QOpcUaAxisInformation"_s),
    OpcUaPrecodedType(u"ComplexNumber"_s, u"QOpcUaComplexNumber"_s, u"QOpcUaComplexNumber"_s),
    OpcUaPrecodedType(u"DoubleComplexNumber"_s, u"QOpcUaDoubleComplexNumber"_s, u"QOpcUaDoubleComplexNumber"_s),
    OpcUaPrecodedType(u"EUInformation"_s, u"QOpcUaEUInformation"_s, u"QOpcUaEUInformation"_s),
    OpcUaPrecodedType(u"ExpandedNodeId"_s, u"QOpcUaExpandedNodeId"_s, u"QOpcUaExpandedNodeId"_s),
    OpcUaPrecodedType(u"ExtensionObject"_s, u"QOpcUaExtensionObject"_s, u"QOpcUaExtensionObject"_s),
    OpcUaPrecodedType(u"LocalizedText"_s, u"QOpcUaLocalizedText"_s, u"QOpcUaLocalizedText"_s),
    OpcUaPrecodedType(u"NodeId"_s, QString(), u"QString"_s, u"QString"_s),
    OpcUaPrecodedType(u"QualifiedName"_s, u"QOpcUaQualifiedName"_s, u"QOpcUaQualifiedName"_s),
    OpcUaPrecodedType(u"Range"_s, u"QOpcUaRange"_s, u"QOpcUaRange"_s),
    OpcUaPrecodedType(u"StatusCode"_s, u"qopcuatype.h"_s, u"QOpcUa::UaStatusCode"_s),
    OpcUaPrecodedType(u"XVType"_s, u"QOpcUaXValue"_s, u"QOpcUaXValue"_s),
    OpcUaPrecodedType(u"DiagnosticInfo"_s, u"QOpcUaDiagnosticInfo"_s, u"QOpcUaDiagnosticInfo"_s),
    OpcUaPrecodedType(u"StructureField"_s, u"QOpcUaStructureField"_s, u"QOpcUaStructureField"_s),
    OpcUaPrecodedType(u"StructureDefinition"_s, u"QOpcUaStructureDefinition"_s, u"QOpcUaStructureDefinition"_s),
    OpcUaPrecodedType(u"EnumField"_s, u"QOpcUaEnumField"_s, u"QOpcUaEnumField"_s),
    OpcUaPrecodedType(u"EnumDefinition"_s, u"QOpcUaEnumDefinition"_s, u"QOpcUaEnumDefinition"_s),
    OpcUaPrecodedType(u"DataValue"_s, u"QOpcUaDataValue"_s, u"QOpcUaDataValue"_s),
    OpcUaPrecodedType(u"Variant"_s, u"QOpcUaVariant"_s, u"QOpcUaVariant"_s)
};

const QList<QString> StringIdentifier::buildInTypesWithBitMask = {u"DiagnosticInfo"_s,
                                                                  u"LocalizedText"_s,
                                                                  u"Variant"_s,
                                                                  u"DataValue"_s,
                                                                  u"Variant"_s};

const QMap<QString, QString> StringIdentifier::typeNameDataTypeConverter
    = {{u"opc:Bit"_s, u"bool"_s},
       {u"opc:Boolean"_s, u"bool"_s},
       {u"opc:Byte"_s, u"quint8"_s},
       {u"opc:ByteString"_s, u"QByteArray"_s},
       {u"opc:CharArray"_s, u"QString"_s},
       {u"opc:DateTime"_s, u"QDateTime"_s},
       {u"opc:Double"_s, u"double"_s},
       {u"opc:Float"_s, u"float"_s},
       {u"opc:Int16"_s, u"qint16"_s},
       {u"opc:Int32"_s, u"qint32"_s},
       {u"opc:Int64"_s, u"qint64"_s},
       {u"opc:SByte"_s, u"qint8"_s},
       {u"opc:String"_s, u"QString"_s},
       {u"opc:UInt16"_s, u"quint16"_s},
       {u"opc:UInt32"_s, u"quint32"_s},
       {u"opc:UInt64"_s, u"quint64"_s},
       {u"opc:Guid"_s, u"QUuid"_s},
       {u"ua:XmlElement"_s, u"QString"_s}};

const QList<QString> StringIdentifier::illegalNames
    = {u"Boolean"_s,  u"boolean"_s,  u"Int16"_s,      u"int16"_s,      u"Float"_s,      u"float"_s,
       u"Datetime"_s, u"datetime"_s, u"byteString"_s, u"ByteString"_s, u"XmlElement"_s, u"xmlElement"_s,
       u"byte"_s,     u"Byte"_s,     u"SByte"_s,      u"sByte"_s,      u"Int32"_s,      u"int32"_s,
       u"Int64"_s,    u"int64"_s,    u"Double"_s,     u"double"_s,     u"String"_s,     u"string"_s};

const QSet<QString> StringIdentifier::precodedTypesWithDebugOperator = { u"LocalizedText"_s, u"NodeId"_s, u"QualifiedName"_s };

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

// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "testserver.h"
#include "qopen62541utils.h"
#include "generated/namespace_qtopcuatestmodel_generated.h"

#include <QtCore/QCommandLineParser>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QVariant>
#include <QUuid>

#include <QtOpcUa/QOpcUaArgument>
#include <QtOpcUa/QOpcUaAxisInformation>
#include <QtOpcUa/QOpcUaComplexNumber>
#include <QtOpcUa/QOpcUaDoubleComplexNumber>
#include <QtOpcUa/QOpcUaExpandedNodeId>
#include <QtOpcUa/QOpcUaExtensionObject>
#include <QtOpcUa/QOpcUaEUInformation>
#include <QtOpcUa/QOpcUaLocalizedText>
#include <QtOpcUa/QOpcUaMultiDimensionalArray>
#include <QtOpcUa/QOpcUaQualifiedName>
#include <QtOpcUa/QOpcUaRange>
#include <QtOpcUa/QOpcUaXValue>
#include <QtOpcUa/QOpcUaStructureDefinition>
#include <QtOpcUa/QOpcUaStructureField>
#include <QtOpcUa/QOpcUaEnumDefinition>
#include <QtOpcUa/QOpcUaEnumField>
#include <QtOpcUa/QOpcUaDiagnosticInfo>

#include <csignal>

using namespace Qt::Literals::StringLiterals;

static volatile bool running = true;

static void signalHandler(int sig) {
    qDebug() << "Signal " << sig << " received, shutting down";
    running = false;
}

int main(int argc, char **argv)
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
#ifdef Q_OS_MACOS
    // If SIGPIPE is not handled, the server exits from time to time
    // on some versions of macOS shortly after a client disconnects
    signal(SIGPIPE, SIG_IGN);
#endif

    QCommandLineParser parser;
    const QCommandLineOption noNonePasswordOption(u"noNonePolicyPassword"_s);
    parser.addOption(noNonePasswordOption);

    QStringList args;
    for (int i = 0; i < argc; ++i)
        args.push_back(QString::fromUtf8(argv[i]));
    parser.parse(args);

    TestServer server;
    if (!server.init(parser.isSet(noNonePasswordOption))) {
        qCritical() << "Could not initialize server.";
        return -1;
    }

    int idx = server.registerNamespace(u"http://qt-project.org"_s);
    if (idx != 2) {
        qWarning() << "Unexpected namespace index for qt-project namespace";
    }
    int ns2 = server.registerNamespace(u"Test Namespace"_s);
    if (ns2 != 3) {
        qWarning() << "Unexpected namespace index for Test namespace";
    }

    const auto largeTestFolder = server.addFolder(u"ns=1;s=Large.Folder"_s, u"Large_Folder"_s);

    for (int x = 0; x < 100; ++x)
        server.addObject(largeTestFolder, idx, {});

    const auto testFolder = server.addFolder(u"ns=3;s=TestFolder"_s, u"TestFolder"_s);

    server.addVariable(testFolder, u"ns=3;s=TestNode.ReadWrite"_s, u"TestNode.ReadWrite"_s, 0.1, QOpcUa::Types::Double);

//    // TODO: Create Event

    // Test variables containing arrays of various types
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.Boolean"_s, u"BoolArrayTest"_s, QVariantList({true, false}), QOpcUa::Types::Boolean);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.Byte"_s, u"ByteArrayTest"_s, QVariantList({1, 2}), QOpcUa::Types::Byte);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.SByte"_s, u"SByteArrayTest"_s, QVariantList({1, 2}), QOpcUa::Types::SByte);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.Double"_s, u"DoubleArrayTest"_s, QVariantList({1.0, 2.0}), QOpcUa::Types::Double);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.Float"_s, u"FloatArrayTest"_s, QVariantList({1.0f, 2.0f}), QOpcUa::Types::Float);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.Int16"_s, u"Int16ArrayTest"_s, QVariantList({1, 2}), QOpcUa::Types::Int16);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.Int32"_s, u"Int32ArrayTest"_s, QVariantList({1, 2}), QOpcUa::Types::Int32);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.Int64"_s, u"Int64ArrayTest"_s, QVariantList({1, 2}), QOpcUa::Types::Int64);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.UInt16"_s, u"UInt16ArrayTest"_s, QVariantList({1, 2}), QOpcUa::Types::UInt16);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.UInt32"_s, u"UInt32ArrayTest"_s, QVariantList({1, 2}), QOpcUa::Types::UInt32);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.UInt64"_s, u"UInt64ArrayTest"_s, QVariantList({1, 2}), QOpcUa::Types::UInt64);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.String"_s, u"StringArrayTest"_s,
                       QVariantList({u"Value 1"_s, u"Value 2"_s}), QOpcUa::Types::String);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.LocalizedText"_s, u"LocalizedTextArrayTest"_s,
                       QVariantList({QVariant::fromValue(QOpcUaLocalizedText(u"en-US"_s, u"Value 1"_s)),
                                     QVariant::fromValue(QOpcUaLocalizedText(u"en-US"_s, u"Value 2"_s))}),
                       QOpcUa::Types::LocalizedText);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.ByteString"_s, u"ByteStringArrayTest"_s,
                       QVariantList({QByteArray("Value 1"), QByteArray("Value 2")}), QOpcUa::Types::ByteString);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.DateTime"_s, u"DateTimeArrayTest"_s,
                       QVariantList({QDateTime::currentDateTime(), QDateTime::currentDateTime().addDays(1), QDateTime::currentDateTime().addDays(2)}),
                       QOpcUa::Types::DateTime);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.NodeId"_s, u"NodeIdArrayTest"_s,
                       QVariantList({u"ns=0;i=84"_s, u"ns=0;i=85"_s}), QOpcUa::Types::NodeId);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.Guid"_s, u"GuidArrayTest"_s,
                       QVariantList({QUuid::createUuid(), QUuid::createUuid()}), QOpcUa::Types::Guid);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.XmlElement"_s, u"XmlElementArrayTest"_s,
                       QVariantList({u"<?xml version=\"1\" encoding=\"UTF-8\"?>"_s,
                                     u"<?xml version=\"1\" encoding=\"UTF-8\"?>"_s}), QOpcUa::Types::XmlElement);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.QualifiedName"_s, u"QualifiedNameArrayTest"_s,
                       QVariantList({QVariant::fromValue(QOpcUaQualifiedName(0, u"Value 1"_s)),
                                     QVariant::fromValue(QOpcUaQualifiedName(0, u"Value 2"_s))}), QOpcUa::Types::QualifiedName);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.StatusCode"_s, u"StatusCodeArrayTest"_s,
                       QVariantList({QVariant::fromValue(QOpcUa::UaStatusCode::Good),
                                     QVariant::fromValue(QOpcUa::UaStatusCode::BadInvalidArgument)}), QOpcUa::Types::StatusCode);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.Range"_s, u"RangeArrayTest"_s,
                       QVariantList({QVariant::fromValue(QOpcUaRange(0, 100)),
                                     QVariant::fromValue(QOpcUaRange(100, 200))}), QOpcUa::Types::Range);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.EUInformation"_s, u"EUInformationArrayTest"_s,
                       QVariantList({QVariant::fromValue(QOpcUaEUInformation()),
                                     QVariant::fromValue(QOpcUaEUInformation())}), QOpcUa::Types::EUInformation);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.ComplexNumber"_s, u"ComplexNumberArrayTest"_s,
                       QVariantList({QVariant::fromValue(QOpcUaComplexNumber(1, 2)),
                                     QVariant::fromValue(QOpcUaComplexNumber(2, 3))}), QOpcUa::Types::ComplexNumber);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.DoubleComplexNumber"_s, u"DoubleComplexNumberArrayTest"_s,
                       QVariantList({QVariant::fromValue(QOpcUaDoubleComplexNumber(1, 2)),
                                     QVariant::fromValue(QOpcUaDoubleComplexNumber(2, 3))}), QOpcUa::Types::DoubleComplexNumber);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.AxisInformation"_s, u"AxisInformationArrayTest"_s,
                       QVariantList({QVariant::fromValue(QOpcUaAxisInformation()),
                                     QVariant::fromValue(QOpcUaAxisInformation())}), QOpcUa::Types::AxisInformation);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.XV"_s, u"XVArrayTest"_s,
                       QVariantList({QVariant::fromValue(QOpcUaXValue(1, 2)),
                                     QVariant::fromValue(QOpcUaXValue(2, 3))}), QOpcUa::Types::XV);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.ExpandedNodeId"_s, u"ExpandedNodeIdArrayTest"_s,
                       QVariantList({QOpcUaExpandedNodeId(u"ns1"_s, u"ns=0;i=64"_s, 1),
                                     QOpcUaExpandedNodeId(QString(), u"ns=1;i=84"_s)}),
                       QOpcUa::Types::ExpandedNodeId);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.Argument"_s, u"ArgumentArrayTest"_s,
                       QVariantList({QOpcUaArgument(u"Argument1"_s, u"ns=0;i=12"_s, -1,
                                                       {},QOpcUaLocalizedText(u"en"_s, u"Description1"_s)),
                                     QOpcUaArgument(u"Argument2"_s, u"ns=0;i=12"_s, 2,
                                                       {2, 2}, QOpcUaLocalizedText(u"en"_s, u"Description2"_s))}),
                       QOpcUa::Types::Argument);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.EventFilter"_s, u"EventFilterArrayTest"_s,
                       QVariantList{QOpcUaMonitoringParameters::EventFilter(), QOpcUaMonitoringParameters::EventFilter()}, QOpcUa::Types::EventFilter);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.ExtensionObject"_s, u"ExtensionObjectArrayTest"_s,
                                                    QVariantList({QOpcUaExtensionObject(), QOpcUaExtensionObject()}),
                       QOpcUa::Types::ExtensionObject);

    // Test variables containing scalar values of various types
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.Boolean"_s, u"BoolScalarTest"_s, false, QOpcUa::Types::Boolean);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.Byte"_s, u"ByteScalarTest"_s, 1, QOpcUa::Types::Byte);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.SByte"_s, u"SByteScalarTest"_s, 2, QOpcUa::Types::SByte);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.Double"_s, u"DoubleScalarTest"_s, 1.0, QOpcUa::Types::Double);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.Float"_s, u"FloatScalarTest"_s, 1.0f, QOpcUa::Types::Float);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.Int16"_s, u"Int16ScalarTest"_s, 1, QOpcUa::Types::Int16);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.Int32"_s, u"Int32ScalarTest"_s, 1, QOpcUa::Types::Int32);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.Int64"_s, u"Int64ScalarTest"_s, 1, QOpcUa::Types::Int64);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.UInt16"_s, u"UInt16ScalarTest"_s, 1, QOpcUa::Types::UInt16);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.UInt32"_s, u"UInt32ScalarTest"_s, 1, QOpcUa::Types::UInt32);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.UInt64"_s, u"UInt64ScalarTest"_s, 1, QOpcUa::Types::UInt64);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.String"_s, u"StringScalarTest"_s, u"Value"_s, QOpcUa::Types::String);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.LocalizedText"_s, u"LocalizedTextScalarTest"_s,
                                                  QVariant::fromValue(QOpcUaLocalizedText(u"en-US"_s, u"Value"_s)),
                       QOpcUa::Types::LocalizedText);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.ByteString"_s, u"ByteStringScalarTest"_s, QByteArray("Value 1"), QOpcUa::Types::ByteString);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.DateTime"_s, u"DateTimeScalarTest"_s, QDateTime::currentDateTime(), QOpcUa::Types::DateTime);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.NodeId"_s, u"NodeIdScalarTest"_s, u"ns=0;i=84"_s, QOpcUa::Types::NodeId);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.Guid"_s, u"GuidScalarTest"_s, QUuid::createUuid(), QOpcUa::Types::Guid);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.XmlElement"_s, u"XmlElementScalarTest"_s,
                       u"<?xml version=\"1\" encoding=\"UTF-8\"?>"_s, QOpcUa::Types::XmlElement);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.QualifiedName"_s, u"QualifiedNameScalarTest"_s,
                                                  QVariant::fromValue(QOpcUaQualifiedName(0, u"Value"_s)), QOpcUa::Types::QualifiedName);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.StatusCode"_s, u"StatusCodeScalarTest"_s, QVariant::fromValue(QOpcUa::UaStatusCode::Good),
                       QOpcUa::Types::StatusCode);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.Range"_s, u"RangeScalarTest"_s, QVariant::fromValue(QOpcUaRange(0, 100)), QOpcUa::Types::Range);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.EUInformation"_s, u"EUInformationScalarTest"_s,
                                                    QVariant::fromValue(QOpcUaEUInformation()), QOpcUa::Types::EUInformation);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.ComplexNumber"_s, u"ComplexNumberScalarTest"_s,
                                                    QVariant::fromValue(QOpcUaComplexNumber(1, 2)), QOpcUa::Types::ComplexNumber);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.DoubleComplexNumber"_s, u"DoubleComplexNumberScalarTest"_s,
                                                    QVariant::fromValue(QOpcUaDoubleComplexNumber(1, 2)), QOpcUa::Types::DoubleComplexNumber);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.AxisInformation"_s, u"AxisInformationScalarTest"_s,
                                                    QVariant::fromValue(QOpcUaAxisInformation()), QOpcUa::Types::AxisInformation);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.XV"_s, u"XVScalarTest"_s,
                                                    QVariant::fromValue(QOpcUaXValue(1, 2)), QOpcUa::Types::XV);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.ExpandedNodeId"_s, u"ExpandedNodeIdScalarTest"_s,
                                                    QOpcUaExpandedNodeId(u"ns1"_s, u"ns=0;i=64"_s, 1),
                       QOpcUa::Types::ExpandedNodeId);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.Argument"_s, u"ArgumentScalarTest"_s,
                       QOpcUaArgument(u"Argument1"_s, u"ns=0;i=12"_s, -1,
                                         {},QOpcUaLocalizedText(u"en"_s, u"Description1"_s)),
                       QOpcUa::Types::Argument);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.EventFilter"_s, u"EventFilterScalarTest"_s, QOpcUaMonitoringParameters::EventFilter(),
                       QOpcUa::Types::EventFilter);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.ExtensionObject"_s, u"ExtensionObjectScalarTest"_s,
                                                    QOpcUaExtensionObject(), QOpcUa::Types::ExtensionObject);
    server.addNodeWithFixedTimestamp(testFolder, u"ns=2;s=Demo.Static.FixedTimestamp"_s, u"FixedTimestamp"_s);

    // Add variables for historizing
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Historizing1"_s, u"HistorizingTest1"_s, 0, QOpcUa::Types::Int32,
                       QList<quint32>(), UA_VALUERANK_ANY, true);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Historizing1.ContinuationPoint"_s, u"HistorizingContinuationPointTest1"_s, 0, QOpcUa::Types::Int32,
                       QList<quint32>(), UA_VALUERANK_ANY, true, 5);

    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Historizing2"_s, u"HistorizingTest2"_s, 0, QOpcUa::Types::Int32,
                       QList<quint32>(), UA_VALUERANK_ANY, true);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Historizing2.ContinuationPoint"_s, u"HistorizingContinuationPointTest2"_s, 0, QOpcUa::Types::Int32,
                       QList<quint32>(), UA_VALUERANK_ANY, true, 5);

    // DataTypeDefinition nodes
    QOpcUaStructureField structureField;
    structureField.setArrayDimensions({1, 2, 3});
    structureField.setDataType(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Double));
    structureField.setDescription(QOpcUaLocalizedText(u"en"_s, u"This is a test"_s));
    structureField.setIsOptional(false);
    structureField.setMaxStringLength(100);
    structureField.setName(u"MyDouble"_s);
    structureField.setValueRank(-1);

    QOpcUaStructureDefinition structureDefinition;
    structureDefinition.setBaseDataType(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Structure));
    structureDefinition.setDefaultEncodingId(u"ns=2;i=1234"_s);
    structureDefinition.setStructureType(QOpcUaStructureDefinition::StructureType::Structure);
    structureDefinition.setFields({structureField});

    QOpcUaEnumField enumField;
    enumField.setDescription(QOpcUaLocalizedText(u"en"_s, u"This is a Test"_s));
    enumField.setDisplayName(QOpcUaLocalizedText(u"en"_s, u"Test"_s));
    enumField.setName(u"MyOption"_s);
    enumField.setValue(0)
            ;
    QOpcUaEnumDefinition enumDefinition;
    enumDefinition.setFields({enumField});

    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.StructureField"_s, u"StructureField"_s, structureField, QOpcUa::Types::StructureField);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.StructureDefinition"_s, u"StructureDefinition"_s, structureDefinition, QOpcUa::Types::StructureDefinition);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.EnumField"_s, u"EnumField"_s, enumField, QOpcUa::Types::EnumField);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.EnumDefinition"_s, u"EnumDefinition"_s, enumDefinition, QOpcUa::Types::EnumDefinition);


    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.StructureField"_s, u"StructureFieldArray"_s, QVariantList{structureField}, QOpcUa::Types::StructureField, {0}, UA_VALUERANK_ONE_DIMENSION);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.StructureDefinition"_s, u"StructureDefinitionArray"_s, QVariantList{structureDefinition}, QOpcUa::Types::StructureDefinition, {0}, UA_VALUERANK_ONE_DIMENSION);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.EnumField"_s, u"EnumFieldArray"_s, QVariantList{enumField}, QOpcUa::Types::EnumField, {0}, UA_VALUERANK_ONE_DIMENSION);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.EnumDefinition"_s, u"EnumDefinitionArray"_s, QVariantList{enumDefinition}, QOpcUa::Types::EnumDefinition, {0}, UA_VALUERANK_ONE_DIMENSION);

    QOpcUaDiagnosticInfo diagnosticInfo;
    diagnosticInfo.setHasSymbolicId(true);
    diagnosticInfo.setSymbolicId(1);
    diagnosticInfo.setHasNamespaceUri(true);
    diagnosticInfo.setNamespaceUri(2);
    diagnosticInfo.setHasLocale(true);
    diagnosticInfo.setLocale(3);
    diagnosticInfo.setHasLocalizedText(true);
    diagnosticInfo.setLocalizedText(4);
    diagnosticInfo.setHasAdditionalInfo(true);
    diagnosticInfo.setAdditionalInfo(u"My additional info"_s);
    diagnosticInfo.setHasInnerStatusCode(true);
    diagnosticInfo.setInnerStatusCode(QOpcUa::UaStatusCode::BadInternalError);
    diagnosticInfo.setHasInnerDiagnosticInfo(true);
    diagnosticInfo.innerDiagnosticInfoRef().setHasAdditionalInfo(true);
    diagnosticInfo.innerDiagnosticInfoRef().setAdditionalInfo(u"My inner additional info"_s);

    QOpcUaDiagnosticInfo diagnosticInfo2;
    diagnosticInfo2.setHasLocale(true);
    diagnosticInfo2.setLocale(1);
    diagnosticInfo2.setHasInnerStatusCode(true);
    diagnosticInfo2.setInnerStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);

    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Scalar.DiagnosticInfo"_s, u"DiagnosticInfo"_s, diagnosticInfo, QOpcUa::Types::DiagnosticInfo);
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.DiagnosticInfo"_s, u"DiagnosticInfoArray"_s, QVariantList{ diagnosticInfo, diagnosticInfo2 },
                       QOpcUa::Types::DiagnosticInfo, {0}, UA_VALUERANK_ONE_DIMENSION);

    // Create folders containing child nodes with string, guid and opaque node ids
    const auto testStringIdsFolder = server.addFolder(u"ns=3;s=testStringIdsFolder"_s, u"testStringIdsFolder"_s);
    server.addVariable(testStringIdsFolder, u"ns=3;s=theStringId"_s, u"theStringId"_s, u"Value"_s, QOpcUa::Types::String);
    const auto testGuidIdsFolder = server.addFolder(u"ns=3;s=testGuidIdsFolder"_s, u"testGuidIdsFolder"_s);
    server.addVariable(testGuidIdsFolder, u"ns=3;g=08081e75-8e5e-319b-954f-f3a7613dc29b"_s, u"theGuidId"_s, u"Value"_s, QOpcUa::Types::String);
    const auto testOpaqueIdsFolder = server.addFolder(u"ns=3;s=testOpaqueIdsFolder"_s, u"testOpaqueIdsFolder"_s);
    server.addVariable(testOpaqueIdsFolder, u"ns=3;b=UXQgZnR3IQ=="_s, u"theOpaqueId"_s, u"Value"_s, QOpcUa::Types::String);

    const auto testUmlautIdFolder = server.addFolder(u"ns=3;s=ümläutFölderNödeId"_s, u"ümläutFölderNödeId"_s);
    server.addVariable(testUmlautIdFolder, u"ns=3;s=ümläutVäriableNödeId"_s, u"ümläutVäriableNödeId"_s, u"Value"_s, QOpcUa::Types::String);

    // Add a method to the test folder
    server.addMultiplyMethod(testFolder, u"ns=3;s=Test.Method.Multiply"_s, u"MultiplyDoubles"_s);
    server.addMultipleOutputArgumentsMethod(testFolder, u"ns=3;s=Test.Method.MultipleOutputArguments"_s, u"MultipleOutputArguments"_s);
    server.addEmptyArrayVariable(testFolder, u"ns=2;s=EmptyBoolArray"_s, u"EmptyBoolArrayTest"_s);

    const QList<quint32> arrayDimensions({2, 2, 3});
    const QVariantList value({0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0});
    server.addVariable(testFolder, u"ns=2;s=Demo.Static.Arrays.MultiDimensionalDouble"_s, u"MultiDimensionalDoubleTest"_s,
                       QOpcUaMultiDimensionalArray(value, arrayDimensions), QOpcUa::Types::Double, QList<quint32>({2, 2, 3}), 3);

    // Add folders for relative nodes
    const auto testFolder2 = server.addFolder(u"ns=3;s=TestFolder2"_s, u"TestFolder2"_s);
    server.addVariable(testFolder2, u"ns=3;s=TestNode2.ReadWrite"_s, u"TestNode.ReadWrite"_s, 0.1, QOpcUa::Types::Double);

    // Add a method for adding namespaces
    server.addAddNamespaceMethod(Open62541Utils::nodeIdFromQString(u"ns=0;i=2253"_s), u"ns=3;s=Test.Method.AddNamespace"_s, u"AddNamespace"_s);

    // Add full writable node
    quint32 fullWritableMask = UA_WRITEMASK_ACCESSLEVEL | UA_WRITEMASK_ARRRAYDIMENSIONS | UA_WRITEMASK_BROWSENAME | UA_WRITEMASK_CONTAINSNOLOOPS |
                                UA_WRITEMASK_DATATYPE | UA_WRITEMASK_DESCRIPTION | UA_WRITEMASK_DISPLAYNAME | UA_WRITEMASK_EVENTNOTIFIER | UA_WRITEMASK_EXECUTABLE |
                                UA_WRITEMASK_HISTORIZING | UA_WRITEMASK_INVERSENAME | UA_WRITEMASK_ISABSTRACT | UA_WRITEMASK_MINIMUMSAMPLINGINTERVAL | UA_WRITEMASK_NODECLASS |
                                UA_WRITEMASK_NODEID | UA_WRITEMASK_SYMMETRIC | UA_WRITEMASK_USERACCESSLEVEL | UA_WRITEMASK_USEREXECUTABLE | UA_WRITEMASK_USERWRITEMASK |
                                UA_WRITEMASK_VALUERANK | UA_WRITEMASK_WRITEMASK | UA_WRITEMASK_VALUEFORVARIABLETYPE;

    server.addVariableWithWriteMask(testFolder, u"ns=3;s=Demo.Static.Scalar.FullyWritable"_s, u"FullyWritableTest"_s, 1.0, QOpcUa::Types::Double, fullWritableMask);

    server.addLocalizedTextNodeWithCallback(testFolder);

    server.addEventTrigger(testFolder);

    server.addEventHistorian(UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER));

    server.addServerStatusTypeTestNodes(testFolder);

    server.addUnreadableVariableNode(testFolder);
    server.addByteStringNodeIdWithNullIdVariableNode(testFolder);

    // Add test nodes for the generic type decoder
    auto result = server.addEncoderTestModel();

    if (result != UA_STATUSCODE_GOOD)
        qFatal("Failed to initialize decoder test nodeset: %s", UA_StatusCode_name(result));

    result = server.run(&running);

    if (result != UA_STATUSCODE_GOOD)
        qFatal("Failed to launch open62541 test server");

    return 0;
}

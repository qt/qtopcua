// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtOpcUa/qopcuaargument.h>
#include <QtOpcUa/QOpcUaAuthenticationInformation>
#include <QtOpcUa/qopcuaaxisinformation.h>
#include <QtOpcUa/QOpcUaClient>
#include <QtOpcUa/QOpcUaConnectionSettings>
#include <QtOpcUa/QOpcUaNode>
#include <QtOpcUa/QOpcUaProvider>
#include <QtOpcUa/qopcuabinarydataencoding.h>
#include <QtOpcUa/QOpcUaHistoryReadResponse>
#include <QtOpcUa/QOpcUaLiteralOperand>
#include <QtOpcUa/qopcuamultidimensionalarray.h>
#include <QtOpcUa/QOpcUaStructureDefinition>
#include <QtOpcUa/QOpcUaEnumDefinition>
#include <QtOpcUa/qopcuaenumfield.h>
#include <QtOpcUa/qopcuaextensionobject.h>
#include <QtOpcUa/qopcuacomplexnumber.h>
#include <QtOpcUa/qopcuadiagnosticinfo.h>
#include <QtOpcUa/qopcuadoublecomplexnumber.h>
#include <QtOpcUa/qopcuaeuinformation.h>
#include <QtOpcUa/qopcuagenericstructhandler.h>
#include <QtOpcUa/qopcuagenericstructvalue.h>
#include <QtOpcUa/qopcuaattributeoperand.h>
#include <QtOpcUa/qopcuaelementoperand.h>
#include <QtOpcUa/qopcuarange.h>
#include <QtOpcUa/qopcuastructuredefinition.h>
#include <QtOpcUa/qopcuastructurefield.h>
#include <QtOpcUa/qopcuaxvalue.h>

#include <private/qopcuasecuritypolicyuris_p.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtCore/QScopedPointer>
#include <QtCore/QThread>
#include <QtCore/QTimer>

#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>
#include <QTcpSocket>
#include <QTcpServer>
#include <QVariantMap>

using namespace Qt::Literals::StringLiterals;

const int signalSpyTimeout = 10000;

class OpcuaConnector
{
public:
    OpcuaConnector(QOpcUaClient *client, const QOpcUaEndpointDescription &endPoint)
        : opcuaClient(client)
    {
        QVERIFY(opcuaClient != nullptr);
        QSignalSpy connectedSpy(opcuaClient, &QOpcUaClient::connected);
        QSignalSpy disconnectedSpy(opcuaClient, &QOpcUaClient::disconnected);
        QSignalSpy stateSpy(opcuaClient, &QOpcUaClient::stateChanged);

        opcuaClient->connectToEndpoint(endPoint);
        stateSpy.wait();
        if (stateSpy.size() != 2)
            stateSpy.wait();

        QCOMPARE(connectedSpy.size(), 1); // one connected signal fired
        QCOMPARE(disconnectedSpy.size(), 0); // zero disconnected signals fired
        QCOMPARE(stateSpy.size(), 2);
        QCOMPARE(stateSpy.at(0).at(0), QOpcUaClient::ClientState::Connecting);
        QCOMPARE(stateSpy.at(1).at(0), QOpcUaClient::ClientState::Connected);

        QCOMPARE(stateSpy.at(0).at(0).value<QOpcUaClient::ClientState>(),
                 QOpcUaClient::ClientState::Connecting);
        QCOMPARE(stateSpy.at(1).at(0).value<QOpcUaClient::ClientState>(),
                 QOpcUaClient::ClientState::Connected);

        stateSpy.clear();
        connectedSpy.clear();
        disconnectedSpy.clear();

        QVERIFY(opcuaClient->endpoint() == endPoint);
    }

    ~OpcuaConnector()
    {
        QSignalSpy connectedSpy(opcuaClient, &QOpcUaClient::connected);
        QSignalSpy disconnectedSpy(opcuaClient, &QOpcUaClient::disconnected);
        QSignalSpy stateSpy(opcuaClient, &QOpcUaClient::stateChanged);
        QSignalSpy errorSpy(opcuaClient, &QOpcUaClient::errorChanged);

        QVERIFY(opcuaClient != nullptr);
        if (opcuaClient->state() == QOpcUaClient::Connected) {

            QSignalSpy stateSpy(opcuaClient, &QOpcUaClient::stateChanged);

            opcuaClient->disconnectFromEndpoint();
            stateSpy.wait(signalSpyTimeout);

            // Once the test has failed, QSignalSpy::wait() returns right away without actually waiting for anything.
            // Processing events manually satisfies the checks below and prevents all following tests from failing
            // because of an unexpected initial client state.
            if (stateSpy.size() < 2) {
                QElapsedTimer t;
                t.start();
                do {
                    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
                } while (t.elapsed() < signalSpyTimeout && stateSpy.size() < 2);
            }

            QVERIFY(opcuaClient->state() == QOpcUaClient::Disconnected);

            QCOMPARE(connectedSpy.size(), 0);
            QCOMPARE(disconnectedSpy.size(), 1);
            QCOMPARE(stateSpy.size(), 2);
            QCOMPARE(stateSpy.at(0).at(0).value<QOpcUaClient::ClientState>(),
                     QOpcUaClient::ClientState::Closing);
            QCOMPARE(stateSpy.at(1).at(0).value<QOpcUaClient::ClientState>(),
                     QOpcUaClient::ClientState::Disconnected);
            QCOMPARE(errorSpy.size(), 0);
        }

        opcuaClient = nullptr;
    }

    QOpcUaClient *opcuaClient;
};

const QString readWriteNode = u"ns=3;s=TestNode.ReadWrite"_s;
const QList<QString> xmlElements = {
    u"<?xml version=\"1\" encoding=\"UTF-8\"?>"_s,
    u"<?xml version=\"2\" encoding=\"UTF-8\"?>"_s,
    u"<?xml version=\"3\" encoding=\"UTF-8\"?>"_s};
const QList<QOpcUaLocalizedText> localizedTexts = {
    QOpcUaLocalizedText(u"en"_s, u"English"_s),
    QOpcUaLocalizedText(u"de"_s, u"German"_s),
    QOpcUaLocalizedText(u"fr"_s, u"French"_s)};
const QList<QOpcUaRange> testRanges = {
    QOpcUaRange(-100, 100),
    QOpcUaRange(0, 100),
    QOpcUaRange(-200, -100)
};
const QList<QOpcUaEUInformation> testEUInfos = {
    QOpcUaEUInformation(u"http://www.opcfoundation.org/UA/units/un/cefact"_s,
        4408652, QOpcUaLocalizedText(QString(), u"°C"_s), QOpcUaLocalizedText(QString(), u"degree Celsius"_s)),
    QOpcUaEUInformation(u"http://www.opcfoundation.org/UA/units/un/cefact"_s,
        4604232, QOpcUaLocalizedText(QString(), u"°F"_s), QOpcUaLocalizedText(QString(), u"degree Fahrenheit"_s)),
    QOpcUaEUInformation(u"http://www.opcfoundation.org/UA/units/un/cefact"_s,
        5067858, QOpcUaLocalizedText(QString(), u"m"_s), QOpcUaLocalizedText(QString(), u"metre"_s)),
};
const QList<QOpcUaComplexNumber> testComplex = {
    QOpcUaComplexNumber(1,2),
    QOpcUaComplexNumber(-1,2),
    QOpcUaComplexNumber(1,-2)
};
const QList<QOpcUaDoubleComplexNumber> testDoubleComplex = {
    QOpcUaDoubleComplexNumber(1,2),
    QOpcUaDoubleComplexNumber(-1,2),
    QOpcUaDoubleComplexNumber(1,-2)
};
const QList<QOpcUaAxisInformation> testAxisInfo = {
    QOpcUaAxisInformation(testEUInfos[0], testRanges[0], localizedTexts[0], QOpcUa::AxisScale::Linear, QList<double>({1, 2, 3})),
    QOpcUaAxisInformation(testEUInfos[1], testRanges[1], localizedTexts[1], QOpcUa::AxisScale::Ln, QList<double>({4, 5, 6})),
    QOpcUaAxisInformation(testEUInfos[2], testRanges[2], localizedTexts[2], QOpcUa::AxisScale::Log, QList<double>({7, 8, 9}))
};
const QList<QOpcUaXValue> testXV = {
    QOpcUaXValue(0, 100),
    QOpcUaXValue(-10, 100.5),
    QOpcUaXValue(10, -100.5)
};
const QList<QUuid> testUuid = {
    QUuid("e0bd5ccd-f571-4545-9352-61a0f8cb9216"),
    QUuid("460ebe04-89d8-42f3-a0e0-7b45940f1a4e"),
    QUuid("72962b91-fa75-4ae6-8d28-b404dc7daf63")
};
const QList<QString> testNodeId = {
    u"ns=1;i=42"_s,
    u"ns=2;s=TestString"_s,
    u"ns=3;g=72962b91-fa75-4ae6-8d28-b404dc7daf63"_s
};
const QList<QDateTime> testDateTime = {
    QDateTime(QDate(1601, 1, 1), QTime(0, 0)),
    QDateTime(QDate(2014, 01, 23), QTime(21, 0)),
    QDateTime(QDate(2300, 01, 01), QTime(12, 0)),
};
const QList<QOpcUa::UaStatusCode> testStatusCode = {
    QOpcUa::UaStatusCode::Good,
    QOpcUa::UaStatusCode::BadInvalidArgument,
    QOpcUa::UaStatusCode::BadNodeIdUnknown
};
const QList<QOpcUaExpandedNodeId> testExpandedId = {
    QOpcUaExpandedNodeId(QString(), u"ns=1;i=23"_s),
    QOpcUaExpandedNodeId(u"MyNamespace"_s, u"ns=2;s=MyNode"_s),
    QOpcUaExpandedNodeId(u"RemoteNamespace"_s, u"ns=3;b=UXQgZnR3IQ=="_s, 1)
};
const QList<QOpcUaExpandedNodeId> testExpandedNodeId = {
    QOpcUaExpandedNodeId(u"namespace1"_s, u"ns=0;i=99"_s, 1),
    QOpcUaExpandedNodeId(QString(), u"ns=1;i=99"_s),
    QOpcUaExpandedNodeId(QString(), u"ns=1;s=test"_s)
};
const QList<QOpcUaArgument> testArguments = {
    QOpcUaArgument(u"Argument1"_s, u"ns=0;i=12"_s, -1,
                      {},QOpcUaLocalizedText(u"en"_s, u"Description1"_s)),
    QOpcUaArgument(u"Argument2"_s, u"ns=0;i=12"_s, 2,
                      {2, 2}, QOpcUaLocalizedText(u"en"_s, u"Description2"_s)),
    QOpcUaArgument(u"Argument3"_s, u"ns=0;i=12"_s, 3,
                      {3, 3, 3}, QOpcUaLocalizedText(u"en"_s, u"Description3"_s))
};

// Test data for DataTypeDefinition types

QList<QOpcUaStructureField> testStructureFields = {};
QList<QOpcUaStructureDefinition> testStructureDefinitions = {};
QList<QOpcUaEnumField> testEnumFields = {};
QList<QOpcUaEnumDefinition> testEnumDefinitions = {};
QList<QOpcUaDiagnosticInfo> testDiagnosticInfos = {};
QList<QOpcUaVariant> testVariants = {};
QList<QOpcUaDataValue> testDataValues = {};

void populateDataTypeDefinitionTestData()
{
    QOpcUaStructureField sf1;
    sf1.setArrayDimensions({1, 2, 3});
    sf1.setDataType(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Double));
    sf1.setDescription(QOpcUaLocalizedText(u"en"_s, u"This is a test"_s));
    sf1.setIsOptional(false);
    sf1.setMaxStringLength(100);
    sf1.setName(u"MyDouble"_s);
    sf1.setValueRank(-1);

    QOpcUaStructureField sf2;
    sf2.setArrayDimensions({1, 2, 3, 4});
    sf2.setDataType(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Double));
    sf2.setDescription(QOpcUaLocalizedText(u"en"_s, u"This is a test"_s));
    sf2.setIsOptional(true);
    sf2.setMaxStringLength(100);
    sf2.setName(u"MyDouble"_s);
    sf2.setValueRank(0);

    QOpcUaStructureDefinition sd1;
    sd1.setBaseDataType(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Structure));
    sd1.setDefaultEncodingId(u"ns=2;i=1234"_s);
    sd1.setStructureType(QOpcUaStructureDefinition::StructureType::Structure);
    sd1.setFields({sf1});

    QOpcUaStructureDefinition sd2;
    sd2.setBaseDataType(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Structure));
    sd2.setDefaultEncodingId(u"ns=2;i=1235"_s);
    sd2.setStructureType(QOpcUaStructureDefinition::StructureType::StructureWithOptionalFields);
    sd2.setFields({sf1, sf2});

    QOpcUaEnumField ef1;
    ef1.setDescription(QOpcUaLocalizedText(u"en"_s, u"This is a Test"_s));
    ef1.setDisplayName(QOpcUaLocalizedText(u"en"_s, u"Test"_s));
    ef1.setName(u"MyOption1"_s);
    ef1.setValue(0);

    QOpcUaEnumField ef2;
    ef2.setDescription(QOpcUaLocalizedText(u"en"_s, u"This is a Test"_s));
    ef2.setDisplayName(QOpcUaLocalizedText(u"en"_s, u"Test2"_s));
    ef2.setName(u"MyOption2"_s);
    ef2.setValue(1);

    QOpcUaEnumDefinition ed1;
    ed1.setFields({ef1});

    QOpcUaEnumDefinition ed2;
    ed1.setFields({ef1, ef2});

    testStructureFields = { sf1, sf2, sf2 };
    testStructureDefinitions = { sd1, sd2, sd2 };
    testEnumFields = { ef1, ef2, ef2 };
    testEnumDefinitions = { ed1, ed2, ed2 };
}

void populateDiagnosticInfoTestData()
{
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

    testDiagnosticInfos = { diagnosticInfo, diagnosticInfo2 };
}

void populateVariantTestData()
{
    testVariants = { {QOpcUaVariant::ValueType::String, QList<QString>{ u"Test1"_s, u"Test2"_s, u"Test3"_s, u"Test4"_s}, { 2, 2 }},
                     {QOpcUaVariant::ValueType::Int16, 23}};

}

void populateDataValuesTestData()
{
    QOpcUaDataValue d1;
    d1.setServerTimestamp(QDateTime::currentDateTime());
    d1.setServerPicoseconds(23);
    d1.setSourceTimestamp(QDateTime::currentDateTime());
    d1.setSourcePicoseconds(42);
    d1.setStatusCode(QOpcUa::UaStatusCode::BadInternalError);
    d1.setValue(testVariants.at(0));

    testDataValues = { d1 };
}

QList<QOpcUaMonitoringParameters::EventFilter> testEventFilters = {};

void populateEventFilterTestData()
{
    QOpcUaMonitoringParameters::EventFilter f;
    f << QOpcUaSimpleAttributeOperand(u"Message"_s);
    f << QOpcUaSimpleAttributeOperand(u"Severity"_s);

    QOpcUaSimpleAttributeOperand sao(u"Name"_s);
    sao.setIndexRange(u"0-2"_s);

    const QOpcUaLiteralOperand lo(42, QOpcUa::Types::Int16);

    QOpcUaAttributeOperand ao;
    ao.setAttributeId(QOpcUa::NodeAttribute::Value);
    ao.setNodeId(u"ns=0;i=42"_s);
    ao.setAlias(u"Alias"_s);
    ao.setBrowsePath({ QOpcUaRelativePathElement(QOpcUaQualifiedName(0, u"Name"_s),
                                                QOpcUa::ReferenceTypeId::HasProperty) });
    ao.setIndexRange(u"0-5"_s);

    const QOpcUaElementOperand eo(2);

    f << (QOpcUaContentFilterElement() << QOpcUaContentFilterElement::FilterOperator::LessThan << sao << lo << ao << eo);
    f << (QOpcUaContentFilterElement() << QOpcUaContentFilterElement::FilterOperator::LessThan << lo << sao << ao << eo);

    QOpcUaMonitoringParameters::EventFilter f2;
    f2 << QOpcUaSimpleAttributeOperand(u"Message"_s);
    f2 << QOpcUaSimpleAttributeOperand(u"Severity"_s);

    f2 << (QOpcUaContentFilterElement() << QOpcUaContentFilterElement::FilterOperator::LessThan << sao << lo << ao << eo);
    f2 << (QOpcUaContentFilterElement() << QOpcUaContentFilterElement::FilterOperator::LessThan << eo << lo << ao << sao);

    testEventFilters.push_back(f);
    testEventFilters.push_back(f2);
}


#define ENCODE_EXTENSION_OBJECT(obj, index) \
{ \
    QVERIFY(index < 3); \
    obj.setEncoding(QOpcUaExtensionObject::Encoding::ByteString); \
    obj.setEncodingTypeId(u"ns=2;s=MyEncoding%1"_s.arg(index)); \
    QOpcUaBinaryDataEncoding encoder(obj); \
    encoder.encode<quint8>(quint8(index)); \
    encoder.encode<qint8>(qint8(index)); \
    encoder.encode<quint16>(quint16(index)); \
    encoder.encode<qint16>(qint16(index)); \
    encoder.encode<quint32>(quint32(index)); \
    encoder.encode<qint32>(qint32(index)); \
    encoder.encode<quint64>(quint64(index)); \
    encoder.encode<qint64>(qint64(index)); \
    encoder.encode<float>(float(index)); \
    encoder.encode<double>(double(index)); \
    encoder.encode<bool>(bool(index)); \
    encoder.encode<QString>(u"String %1"_s.arg(index)); \
    encoder.encode<QOpcUaQualifiedName>(QOpcUaQualifiedName(2, u"QualifiedName %1"_s.arg(index))); \
    encoder.encode<QOpcUaLocalizedText>(localizedTexts.at(index)); \
    encoder.encode<QOpcUaRange>(testRanges.at(index)); \
    encoder.encode<QOpcUaEUInformation>(testEUInfos.at(index)); \
    encoder.encode<QOpcUaComplexNumber>(testComplex.at(index)); \
    encoder.encode<QOpcUaDoubleComplexNumber>(testDoubleComplex.at(index)); \
    encoder.encode<QOpcUaAxisInformation>(testAxisInfo.at(index)); \
    encoder.encode<QOpcUaXValue>(testXV.at(index)); \
    encoder.encode<QUuid>(testUuid.at(index)); \
    encoder.encode<QString, QOpcUa::Types::NodeId>(testNodeId.at(index)); \
    encoder.encode<QDateTime>(testDateTime.at(index)); \
    encoder.encode<QOpcUa::UaStatusCode>(testStatusCode.at(index)); \
    encoder.encode<QOpcUaExpandedNodeId>(testExpandedId.at(index)); \
    QOpcUaExtensionObject ext; \
    ext.setEncodingTypeId(obj.encodingTypeId()); \
    ext.setEncoding(obj.encoding()); \
    QOpcUaBinaryDataEncoding encoding2(&ext.encodedBodyRef()); \
    encoding2.encode<QString>(u"String %1"_s.arg(index)); \
    encoder.encode<QOpcUaExtensionObject>(ext); \
    encoder.encode<QOpcUaArgument>(testArguments.at(index)); \
    encoder.encode<QOpcUaStructureDefinition>(testStructureDefinitions.at(index)); \
    encoder.encode<QOpcUaEnumDefinition>(testEnumDefinitions.at(index)); \
}

#define VERIFY_EXTENSION_OBJECT(obj, index) \
{ \
    QVERIFY(index < 3); \
    QCOMPARE(obj.encoding(), QOpcUaExtensionObject::Encoding::ByteString); \
    QCOMPARE(obj.encodingTypeId(), u"ns=2;s=MyEncoding%1"_s.arg(index)); \
    bool success = false; \
    QOpcUaBinaryDataEncoding decoder(obj); \
    QCOMPARE(decoder.offset(), 0); \
    QCOMPARE(decoder.decode<quint8>(success), quint8(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<qint8>(success), qint8(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<quint16>(success), quint16(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<qint16>(success), qint16(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<quint32>(success), quint32(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<qint32>(success), qint32(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<quint64>(success), quint64(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<qint64>(success), qint64(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<float>(success), float(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<double>(success), double(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<bool>(success), bool(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<QString>(success), u"String %1"_s.arg(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<QOpcUaQualifiedName>(success), \
             QOpcUaQualifiedName(2, u"QualifiedName %1"_s.arg(index))); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<QOpcUaLocalizedText>(success), localizedTexts.at(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<QOpcUaRange>(success), testRanges.at(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<QOpcUaEUInformation>(success), testEUInfos.at(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<QOpcUaComplexNumber>(success), testComplex.at(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<QOpcUaDoubleComplexNumber>(success), testDoubleComplex.at(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<QOpcUaAxisInformation>(success), testAxisInfo.at(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<QOpcUaXValue>(success), testXV.at(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<QUuid>(success), testUuid.at(index)); \
    QVERIFY(success == true); \
    QString temp = decoder.decode<QString, QOpcUa::Types::NodeId>(success); \
    QCOMPARE(temp, testNodeId.at(index)); \
    QVERIFY(success == true); \
    QDateTime dt = decoder.decode<QDateTime>(success); \
    if (index == 0) \
        QCOMPARE(dt, QDateTime()); \
    else \
        QCOMPARE(dt, testDateTime.at(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<QOpcUa::UaStatusCode>(success), testStatusCode.at(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<QOpcUaExpandedNodeId>(success), testExpandedId.at(index)); \
    QVERIFY(success == true); \
    QOpcUaExtensionObject ext = decoder.decode<QOpcUaExtensionObject>(success); \
    QCOMPARE(ext.encodingTypeId(), obj.encodingTypeId()); \
    QCOMPARE(ext.encoding(), obj.encoding()); \
    QVERIFY(success == true); \
    QOpcUaBinaryDataEncoding decoder2(&ext.encodedBodyRef()); \
    QCOMPARE(decoder2.decode<QString>(success), u"String %1"_s.arg(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<QOpcUaArgument>(success), testArguments.at(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<QOpcUaStructureDefinition>(success), testStructureDefinitions.at(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.decode<QOpcUaEnumDefinition>(success), testEnumDefinitions.at(index)); \
    QVERIFY(success == true); \
    QCOMPARE(decoder.offset(), obj.encodedBody().size()); \
}

#define defineDataMethod(name) void name()\
{\
    QTest::addColumn<QOpcUaClient *>("opcuaClient");\
    for (auto *client : std::as_const(m_clients))\
        QTest::newRow(client->backend().toLatin1().constData()) << client;\
}

class Tst_QOpcUaClient: public QObject
{
    Q_OBJECT

public:
    Tst_QOpcUaClient();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void loadInvalidPlugin();

    // connect with invalid configuration
    defineDataMethod(connectToInvalid_data)
    void connectToInvalid();
    defineDataMethod(connectWithEmptyUrl_data)
    void connectWithEmptyUrl();
    defineDataMethod(connectWithMalformedUrl_data)
    void connectWithMalformedUrl();
    defineDataMethod(connectWithoutTokens_data);
    void connectWithoutTokens();
    defineDataMethod(connectWithInvalidSecurityMode_data);
    void connectWithInvalidSecurityMode();
    defineDataMethod(connectWithInvalidSecurityPolicyUri_data);
    void connectWithInvalidSecurityPolicyUri();
    defineDataMethod(connectSecureEndpointWithoutPki_data);
    void connectSecureEndpointWithoutPki();
    defineDataMethod(connectEncryptedTokenWithoutPki_data);
    void connectEncryptedTokenWithoutPki();
    defineDataMethod(connectCertificateAuthWithoutPki_data);
    void connectCertificateAuthWithoutPki();

    // connect & disconnect
    defineDataMethod(connectAndDisconnect_data)
    void connectAndDisconnect();
    defineDataMethod(checkSessionLocaleIds_data)
    void checkSessionLocaleIds();
    defineDataMethod(connectionSettings_data)
    void connectionSettings();

    // Password
    defineDataMethod(connectInvalidPassword_data)
    void connectInvalidPassword();
    defineDataMethod(connectAndDisconnectPassword_data)
    void connectAndDisconnectPassword();

    // Server discovery
    defineDataMethod(findServers_data)
    void findServers();

    // Endpoint discovery
    defineDataMethod(requestEndpoints_data)
    void requestEndpoints();

    defineDataMethod(compareNodeIds_data)
    void compareNodeIds();
    defineDataMethod(getNodeForIdTypes_data)
    void getNodeForIdTypes();
    defineDataMethod(readNodeWithByteStringIdWithNullByte_data)
    void readNodeWithByteStringIdWithNullByte();
    defineDataMethod(readNS0OmitNode_data)
    void readNS0OmitNode();
    defineDataMethod(readInvalidNode_data)
    void readInvalidNode();
    defineDataMethod(requestNotInCache_data)
    void requestNotInCache();
    defineDataMethod(writeInvalidNode_data)
    void writeInvalidNode();
    defineDataMethod(writeMultipleAttributes_data)
    void writeMultipleAttributes();
    defineDataMethod(readEmptyArrayVariable_data)
    void readEmptyArrayVariable();
    defineDataMethod(writeNodeAttributes_data)
    void writeNodeAttributes();
    defineDataMethod(readNodeAttributes_data)
    void readNodeAttributes();

    defineDataMethod(readDataTypeDefinition_data)
    void readDataTypeDefinition();
    defineDataMethod(readAccessLevelEx_data)
    void readAccessLevelEx();
    defineDataMethod(readNewPermissionAttributes_data)
    void readNewPermissionAttributes();

    defineDataMethod(getRootNode_data)
    void getRootNode();
    defineDataMethod(getChildren_data)
    void getChildren();
    defineDataMethod(childrenIdsString_data)
    void childrenIdsString();
    defineDataMethod(childrenIdsGuidNodeId_data)
    void childrenIdsGuidNodeId();
    defineDataMethod(childrenIdsOpaqueNodeId_data)
    void childrenIdsOpaqueNodeId();
    defineDataMethod(testSpecialCharStringNodeIds_data)
    void testSpecialCharStringNodeIds();
    defineDataMethod(inverseBrowse_data)
    void inverseBrowse();

    defineDataMethod(addAndRemoveObjectNode_data)
    void addAndRemoveObjectNode();
    defineDataMethod(addAndRemoveVariableNode_data)
    void addAndRemoveVariableNode();
    defineDataMethod(addAndRemoveMethodNode_data)
    void addAndRemoveMethodNode();
    defineDataMethod(addAndRemoveObjectTypeNode_data)
    void addAndRemoveObjectTypeNode();
    defineDataMethod(addAndRemoveVariableTypeNode_data)
    void addAndRemoveVariableTypeNode();
    defineDataMethod(addAndRemoveReferenceTypeNode_data)
    void addAndRemoveReferenceTypeNode();
    defineDataMethod(addAndRemoveDataTypeNode_data)
    void addAndRemoveDataTypeNode();
    defineDataMethod(addAndRemoveViewNode_data)
    void addAndRemoveViewNode();
    defineDataMethod(addAndRemoveReference_data)
    void addAndRemoveReference();

    defineDataMethod(dataChangeSubscription_data)
    void dataChangeSubscription();
    defineDataMethod(dataChangeSubscriptionInvalidNode_data)
    void dataChangeSubscriptionInvalidNode();
    defineDataMethod(dataChangeSubscriptionSharing_data)
    void dataChangeSubscriptionSharing();
    defineDataMethod(methodCall_data)
    void dataChangeSubscriptionTriggering();
    defineDataMethod(dataChangeSubscriptionTriggering_data);
    void dataChangeSubscriptionModifyTriggering();
    defineDataMethod(dataChangeSubscriptionModifyTriggering_data);
    void methodCall();
    defineDataMethod(methodCallInvalid_data)
    void methodCallInvalid();
    defineDataMethod(readMethodArguments_data)
    void readMethodArguments();
    defineDataMethod(malformedNodeString_data)
    void malformedNodeString();
    defineDataMethod(nodeIdGeneration_data)
    void nodeIdGeneration();

    defineDataMethod(multipleClients_data)
    void multipleClients();
    defineDataMethod(nodeClass_data)
    void nodeClass();
    defineDataMethod(writeArray_data)
    void writeArray();
    defineDataMethod(readArray_data)
    void readArray();
    defineDataMethod(writeScalar_data)
    void writeScalar();
    defineDataMethod(readScalar_data)
    void readScalar();
    defineDataMethod(readReencodedExtensionObject_data)
    void readReencodedExtensionObject();
    defineDataMethod(indexRange_data)
    void indexRange();
    defineDataMethod(invalidIndexRange_data)
    void invalidIndexRange();
    defineDataMethod(subscriptionIndexRange_data)
    void subscriptionIndexRange();
    defineDataMethod(subscriptionDataChangeFilter_data)
    void subscriptionDataChangeFilter();
    defineDataMethod(modifyPublishingMode_data)
    void modifyPublishingMode();
    defineDataMethod(modifyMonitoringMode_data)
    void modifyMonitoringMode();
    defineDataMethod(modifyMonitoredItem_data)
    void modifyMonitoredItem();
    defineDataMethod(addDuplicateMonitoredItem_data)
    void addDuplicateMonitoredItem();
    defineDataMethod(subscriptionUnreadableNode_data);
    void subscriptionUnreadableNode();
    defineDataMethod(checkMonitoredItemCleanup_data);
    void checkMonitoredItemCleanup();
    defineDataMethod(checkAttributeUpdated_data);
    void checkAttributeUpdated();
    defineDataMethod(checkValueAttributeUpdated_data);
    void checkValueAttributeUpdated();
    defineDataMethod(checkMonitoringInvalidRequests_data);
    void checkMonitoringInvalidRequests();

    defineDataMethod(stringCharset_data)
    void stringCharset();

    defineDataMethod(namespaceArray_data)
    void namespaceArray();

    defineDataMethod(multiDimensionalArray_data)
    void multiDimensionalArray();

    defineDataMethod(dateTimeConversion_data)
    void dateTimeConversion();
    defineDataMethod(timeStamps_data)
    void timeStamps();

    defineDataMethod(createNodeFromExpandedId_data)
    void createNodeFromExpandedId();
    defineDataMethod(checkExpandedIdConversion_data)
    void checkExpandedIdConversion();
    defineDataMethod(checkExpandedIdConversionNoOk_data)
    void checkExpandedIdConversionNoOk();
    defineDataMethod(createQualifiedName_data)
    void createQualifiedName();
    defineDataMethod(createQualifiedNameNoOk_data)
    void createQualifiedNameNoOk();
    defineDataMethod(addNamespace_data)
    void addNamespace();

    void fixedTimestamp();
    defineDataMethod(fixedTimestamp_data)

    defineDataMethod(resolveBrowsePath_data)
    void resolveBrowsePath();

    defineDataMethod(extensionObjectWithGuid_data)
    void extensionObjectWithGuid();

    defineDataMethod(encodeEmptyStringNodeId_data)
    void encodeEmptyStringNodeId();

    void statusStrings();

    defineDataMethod(readHistoryDataFromNode_data)
    void readHistoryDataFromNode();

    defineDataMethod(readHistoryDataFromClient_data)
    void readHistoryDataFromClient();

    defineDataMethod(readHistoryEventsFromNode_data)
    void readHistoryEventsFromNode();

    defineDataMethod(readHistoryEventsFromClient_data)
    void readHistoryEventsFromClient();

    // Events
    defineDataMethod(eventSubscription_data)
    void eventSubscription();

    defineDataMethod(decodeGenericStruct_data)
    void decodeGenericStruct();

    defineDataMethod(encodeGenericStruct_data)
    void encodeGenericStruct();

    defineDataMethod(encodeCustomGenericStruct_data)
    void encodeCustomGenericStruct();

    defineDataMethod(registerUnregisterNodes_data)
    void registerUnregisterNodes();

    // This test case restarts the server. It must be run last to avoid
    // destroying state required by other test cases.
    defineDataMethod(connectionLost_data)
    void connectionLost();

private:
    QString envOrDefault(const char *env, QString def)
    {
        return qEnvironmentVariableIsSet(env) ? QString::fromUtf8(qgetenv(env).constData()) : def;
    }

    QString m_discoveryEndpoint;
    QOpcUaProvider m_opcUa;
    QStringList m_backends;
    QList<QOpcUaClient *> m_clients;
    QProcess m_serverProcess;
    QString m_testServerPath;
    QOpcUaEndpointDescription m_endpoint;
};

#define READ_MANDATORY_BASE_NODE(NODE) \
    { \
    QSignalSpy attributeReadSpy(NODE.data(), &QOpcUaNode::attributeRead);\
    NODE->readAttributes(QOpcUaNode::mandatoryBaseAttributes()); \
    attributeReadSpy.wait(signalSpyTimeout); \
    QCOMPARE(attributeReadSpy.count(), 1); \
    QCOMPARE(attributeReadSpy.at(0).at(0).value<QOpcUa::NodeAttributes>(), QOpcUaNode::mandatoryBaseAttributes()); \
    QVERIFY(QOpcUa::isSuccessStatus(NODE->attributeError(QOpcUa::NodeAttribute::NodeId)) == true); \
    QVERIFY(QOpcUa::isSuccessStatus(NODE->attributeError(QOpcUa::NodeAttribute::NodeClass)) == true); \
    QVERIFY(QOpcUa::isSuccessStatus(NODE->attributeError(QOpcUa::NodeAttribute::BrowseName)) == true); \
    QVERIFY(QOpcUa::isSuccessStatus(NODE->attributeError(QOpcUa::NodeAttribute::DisplayName)) == true); \
    }

#define READ_MANDATORY_VARIABLE_NODE(NODE) \
    { \
    QSignalSpy attributeReadSpy(NODE.data(), &QOpcUaNode::attributeRead);\
    NODE->readAttributes(QOpcUaNode::mandatoryBaseAttributes() | QOpcUa::NodeAttribute::Value); \
    attributeReadSpy.wait(signalSpyTimeout); \
    QCOMPARE(attributeReadSpy.count(), 1); \
    QCOMPARE(attributeReadSpy.at(0).at(0).value<QOpcUa::NodeAttributes>(), (QOpcUaNode::mandatoryBaseAttributes() | QOpcUa::NodeAttribute::Value)); \
    QVERIFY(QOpcUa::isSuccessStatus(NODE->attributeError(QOpcUa::NodeAttribute::NodeId)) == true); \
    QVERIFY(QOpcUa::isSuccessStatus(NODE->attributeError(QOpcUa::NodeAttribute::NodeClass)) == true); \
    QVERIFY(QOpcUa::isSuccessStatus(NODE->attributeError(QOpcUa::NodeAttribute::BrowseName)) == true); \
    QVERIFY(QOpcUa::isSuccessStatus(NODE->attributeError(QOpcUa::NodeAttribute::DisplayName)) == true); \
    QVERIFY(QOpcUa::isSuccessStatus(NODE->attributeError(QOpcUa::NodeAttribute::Value)) == true); \
    }

#define WRITE_VALUE_ATTRIBUTE(NODE, VALUE, TYPE) \
{ \
    QSignalSpy resultSpy(NODE.data(), &QOpcUaNode::attributeWritten); \
    NODE->writeAttribute(QOpcUa::NodeAttribute::Value, VALUE, TYPE); \
    resultSpy.wait(signalSpyTimeout); \
    QCOMPARE(resultSpy.size(), 1); \
    QCOMPARE(resultSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value); \
    QCOMPARE(resultSpy.at(0).at(1), QOpcUa::UaStatusCode::Good); \
}

Tst_QOpcUaClient::Tst_QOpcUaClient()
{
    m_backends = QOpcUaProvider::availableBackends();
}

void Tst_QOpcUaClient::initTestCase()
{
    const quint16 defaultPort = 43344;
    const QHostAddress defaultHost(QHostAddress::LocalHost);

    for (const auto &backend: std::as_const(m_backends)) {
        QVariantMap backendOptions;

        QOpcUaClient *client = m_opcUa.createClient(backend, backendOptions);
        QVERIFY2(client != nullptr,
                 u"Loading backend failed: %1"_s.arg(backend).toLatin1().data());
        client->setParent(this);
        qDebug() << "Using SDK plugin:" << client->backend();
        m_clients.append(client);
    }

    if (qEnvironmentVariableIsEmpty("OPCUA_HOST") && qEnvironmentVariableIsEmpty("OPCUA_PORT")) {
        m_testServerPath = qApp->applicationDirPath()

#if defined(Q_OS_MACOS)
                                     + "/../../open62541-testserver/open62541-testserver.app/Contents/MacOS/open62541-testserver"_L1
#else

#if defined(Q_OS_WIN) && !defined(TESTS_CMAKE_SPECIFIC_PATH)
                                     + "/.."_L1
#endif
                                     + "/../../open62541-testserver/open62541-testserver"_L1
#ifdef Q_OS_WIN
                                     + ".exe"_L1
#endif

#endif
                ;
        if (!QFile::exists(m_testServerPath)) {
            qDebug() << "Server Path:" << m_testServerPath;
            QSKIP("all auto tests rely on an open62541-based test-server");
        }

        // In this case the test is supposed to open its own server.
        // Unfortunately there is no way to check if the server has started up successfully
        // because of missing error handling.
        // This checks will detect other servers blocking the port.

        // Check for running server
        QTcpSocket socket;
        socket.connectToHost(defaultHost, defaultPort);
        QVERIFY2(socket.waitForConnected(1500) == false, "Server is already running");

        // Check for running server which does not respond
        QTcpServer server;
        QVERIFY2(server.listen(defaultHost, defaultPort) == true, "Port is occupied by another process. Check for defunct server.");
        server.close();

        m_serverProcess.start(m_testServerPath);
        QVERIFY2(m_serverProcess.waitForStarted(), qPrintable(m_serverProcess.errorString()));

        // Check if server is up and running
        QVERIFY(m_serverProcess.state() == QProcess::Running);

        QTest::qSleep(100);
        socket.connectToHost(defaultHost, defaultPort);
        if (!socket.waitForConnected(5000))
        {
            bool success = false;
            for (int i = 0; i < 50; ++i) {
                QTest::qSleep(100);
                socket.connectToHost(defaultHost, defaultPort);
                if (socket.waitForConnected(5000)) {
                    success = true;
                    break;
                }
            }

            if (!success)
                QFAIL("Server does not run");
        }

        socket.disconnectFromHost();
    }
    QString host = envOrDefault("OPCUA_HOST", defaultHost.toString());
    QString port = envOrDefault("OPCUA_PORT", QString::number(defaultPort));
    m_discoveryEndpoint = u"opc.tcp://%1:%2"_s.arg(host, port);
    qDebug() << "Using endpoint:" << m_discoveryEndpoint;

    QOpcUaClient *client = m_clients.first();
    if (client) {
        QSignalSpy endpointSpy(m_clients.first(), &QOpcUaClient::endpointsRequestFinished);

        client->requestEndpoints(m_discoveryEndpoint);
        endpointSpy.wait(signalSpyTimeout);
        QCOMPARE(endpointSpy.size(), 1);

        const auto desc = endpointSpy.at(0).at(0).value<QList<QOpcUaEndpointDescription>>();
        QVERIFY(desc.size() > 0);
        QCOMPARE(endpointSpy.at(0).at(2).value<QUrl>(), m_discoveryEndpoint);

        m_endpoint = desc.first();
    }
}

void Tst_QOpcUaClient::connectToInvalid()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    QOpcUaEndpointDescription invalidEndpoint;
    invalidEndpoint.setEndpointUrl(u"opc.tcp:127.0.0.1:1234"_s);

    QSignalSpy connectSpy(opcuaClient, &QOpcUaClient::stateChanged);

    opcuaClient->connectToEndpoint(invalidEndpoint);

    connectSpy.wait();
    if (connectSpy.size() < 2)
        connectSpy.wait();
    QCOMPARE(connectSpy.size(), 2);

    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);

    QCOMPARE(opcuaClient->state(), QOpcUaClient::Disconnected);

    QCOMPARE(opcuaClient->endpoint(), invalidEndpoint);
}

void Tst_QOpcUaClient::connectWithEmptyUrl()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    auto endpoint = m_endpoint;
    endpoint.setEndpointUrl({});

    QSignalSpy connectSpy(opcuaClient, &QOpcUaClient::stateChanged);
    QSignalSpy errorSpy(opcuaClient, &QOpcUaClient::errorChanged);

    opcuaClient->connectToEndpoint(endpoint);

    connectSpy.wait();
    if (connectSpy.size() < 2)
        connectSpy.wait();
    QCOMPARE(connectSpy.size(), 2);

    if (errorSpy.isEmpty())
        errorSpy.wait();

    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);

    QCOMPARE(errorSpy.size(), 1);
    QCOMPARE(errorSpy.at(0).at(0), QOpcUaClient::ClientError::InvalidUrl);

    QCOMPARE(opcuaClient->state(), QOpcUaClient::Disconnected);
    QCOMPARE(opcuaClient->error(), QOpcUaClient::ClientError::InvalidUrl);
}

void Tst_QOpcUaClient::connectWithMalformedUrl()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    auto endpoint = m_endpoint;
    endpoint.setEndpointUrl(u"asdf://127.0.0.1"_s);

    QSignalSpy connectSpy(opcuaClient, &QOpcUaClient::stateChanged);
    QSignalSpy errorSpy(opcuaClient, &QOpcUaClient::errorChanged);

    opcuaClient->connectToEndpoint(endpoint);

    connectSpy.wait();
    if (connectSpy.size() < 2)
        connectSpy.wait();
    QCOMPARE(connectSpy.size(), 2);

    if (errorSpy.isEmpty())
        errorSpy.wait();

    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);

    QCOMPARE(errorSpy.size(), 1);
    QCOMPARE(errorSpy.at(0).at(0), QOpcUaClient::ClientError::InvalidUrl);

    QCOMPARE(opcuaClient->state(), QOpcUaClient::Disconnected);
    QCOMPARE(opcuaClient->error(), QOpcUaClient::ClientError::InvalidUrl);
}

void Tst_QOpcUaClient::connectWithoutTokens()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    auto endpoint = m_endpoint;
    endpoint.setUserIdentityTokens({});

    QSignalSpy connectSpy(opcuaClient, &QOpcUaClient::stateChanged);
    QSignalSpy errorSpy(opcuaClient, &QOpcUaClient::errorChanged);

    opcuaClient->connectToEndpoint(endpoint);

    connectSpy.wait();
    if (connectSpy.size() < 2)
        connectSpy.wait();
    QCOMPARE(connectSpy.size(), 2);

    if (errorSpy.isEmpty())
        errorSpy.wait();

    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);

    QCOMPARE(errorSpy.size(), 1);
    QCOMPARE(errorSpy.at(0).at(0), QOpcUaClient::ClientError::NoMatchingUserIdentityTokenFound);

    QCOMPARE(opcuaClient->state(), QOpcUaClient::Disconnected);
    QCOMPARE(opcuaClient->error(), QOpcUaClient::ClientError::NoMatchingUserIdentityTokenFound);
}

void Tst_QOpcUaClient::connectWithInvalidSecurityMode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    auto endpoint = m_endpoint;
    endpoint.setSecurityMode(QOpcUaEndpointDescription::MessageSecurityMode::Invalid);

    QSignalSpy connectSpy(opcuaClient, &QOpcUaClient::stateChanged);
    QSignalSpy errorSpy(opcuaClient, &QOpcUaClient::errorChanged);

    opcuaClient->connectToEndpoint(endpoint);

    connectSpy.wait();
    if (connectSpy.size() < 2)
        connectSpy.wait();
    QCOMPARE(connectSpy.size(), 2);

    if (errorSpy.isEmpty())
        errorSpy.wait();

    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);

    QCOMPARE(errorSpy.size(), 1);
    QCOMPARE(errorSpy.at(0).at(0), QOpcUaClient::ClientError::InvalidEndpointDescription);

    QCOMPARE(opcuaClient->state(), QOpcUaClient::Disconnected);
    QCOMPARE(opcuaClient->error(), QOpcUaClient::ClientError::InvalidEndpointDescription);
}

void Tst_QOpcUaClient::connectWithInvalidSecurityPolicyUri()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    auto endpoint = m_endpoint;
    endpoint.setSecurityPolicy("http://notavalidsecuritypolicyuri"_L1);

    QSignalSpy connectSpy(opcuaClient, &QOpcUaClient::stateChanged);
    QSignalSpy errorSpy(opcuaClient, &QOpcUaClient::errorChanged);

    opcuaClient->connectToEndpoint(endpoint);

    connectSpy.wait();
    if (connectSpy.size() < 2)
        connectSpy.wait();
    QCOMPARE(connectSpy.size(), 2);

    if (errorSpy.isEmpty())
        errorSpy.wait();

    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);

    QCOMPARE(errorSpy.size(), 1);
    QCOMPARE(errorSpy.at(0).at(0), QOpcUaClient::ClientError::UnsupportedSecurityPolicy);

    QCOMPARE(opcuaClient->state(), QOpcUaClient::Disconnected);
    QCOMPARE(opcuaClient->error(), QOpcUaClient::ClientError::UnsupportedSecurityPolicy);
}

void Tst_QOpcUaClient::connectSecureEndpointWithoutPki()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    auto endpoint = m_endpoint;
    endpoint.setSecurityPolicy(QOpcUa::Basic256Sha256Policy);

    QSignalSpy connectSpy(opcuaClient, &QOpcUaClient::stateChanged);
    QSignalSpy errorSpy(opcuaClient, &QOpcUaClient::errorChanged);

    opcuaClient->connectToEndpoint(endpoint);

    connectSpy.wait();
    if (connectSpy.size() < 2)
        connectSpy.wait();
    QCOMPARE(connectSpy.size(), 2);

    if (errorSpy.isEmpty())
        errorSpy.wait();

    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);

    QCOMPARE(errorSpy.size(), 1);
    QCOMPARE(errorSpy.at(0).at(0), QOpcUaClient::ClientError::UnsupportedSecurityPolicy);

    QCOMPARE(opcuaClient->state(), QOpcUaClient::Disconnected);
    QCOMPARE(opcuaClient->error(), QOpcUaClient::ClientError::UnsupportedSecurityPolicy);
}

void Tst_QOpcUaClient::connectEncryptedTokenWithoutPki()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    auto endpoint = m_endpoint;
    auto tokens = endpoint.userIdentityTokens();
    for (auto &entry : tokens)
        entry.setSecurityPolicy(QOpcUa::Basic256Sha256Policy);
    endpoint.setUserIdentityTokens(tokens);

    QSignalSpy connectSpy(opcuaClient, &QOpcUaClient::stateChanged);
    QSignalSpy errorSpy(opcuaClient, &QOpcUaClient::errorChanged);

    opcuaClient->connectToEndpoint(endpoint);

    connectSpy.wait();
    if (connectSpy.size() < 2)
        connectSpy.wait();
    QCOMPARE(connectSpy.size(), 2);

    if (errorSpy.isEmpty())
        errorSpy.wait();

    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);

    QCOMPARE(errorSpy.size(), 1);
    QCOMPARE(errorSpy.at(0).at(0), QOpcUaClient::ClientError::NoMatchingUserIdentityTokenFound);

    QCOMPARE(opcuaClient->state(), QOpcUaClient::Disconnected);
    QCOMPARE(opcuaClient->error(), QOpcUaClient::ClientError::NoMatchingUserIdentityTokenFound);
}

void Tst_QOpcUaClient::connectCertificateAuthWithoutPki()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    // Use a new client for the test to avoid keeping the auth information
    QScopedPointer<QOpcUaClient> tempClient(m_opcUa.createClient(opcuaClient->backend()));
    QVERIFY(tempClient);

    auto endpoint = m_endpoint;
    QOpcUaAuthenticationInformation authInfo;
    authInfo.setCertificateAuthentication();
    tempClient->setAuthenticationInformation(authInfo);

    QSignalSpy connectSpy(tempClient.get(), &QOpcUaClient::stateChanged);
    QSignalSpy errorSpy(tempClient.get(), &QOpcUaClient::errorChanged);

    tempClient->connectToEndpoint(endpoint);

    connectSpy.wait();
    if (connectSpy.size() < 2)
        connectSpy.wait();
    QCOMPARE(connectSpy.size(), 2);

    if (errorSpy.isEmpty())
        errorSpy.wait();

    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);

    QCOMPARE(tempClient->state(), QOpcUaClient::Disconnected);

    QCOMPARE(errorSpy.size(), 1);
#ifdef SERVER_SUPPORTS_SECURITY
    QCOMPARE(errorSpy.at(0).at(0), QOpcUaClient::ClientError::InvalidAuthenticationInformation);
    QCOMPARE(tempClient->error(), QOpcUaClient::ClientError::InvalidAuthenticationInformation);
#else
    QCOMPARE(errorSpy.at(0).at(0), QOpcUaClient::ClientError::UnsupportedAuthenticationInformation);
    QCOMPARE(tempClient->error(), QOpcUaClient::ClientError::UnsupportedAuthenticationInformation);
#endif
}

void Tst_QOpcUaClient::connectAndDisconnect()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);
}

void Tst_QOpcUaClient::checkSessionLocaleIds()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    // Empty locale id => fallback to en
    {
        OpcuaConnector connector(opcuaClient, m_endpoint);

        QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=LocalizedTextWithCallback"_s));
        QVERIFY(node != nullptr);
        READ_MANDATORY_VARIABLE_NODE(node);
        QCOMPARE(node->valueAttribute(), QOpcUaLocalizedText(u"en"_s, u"Hello"_s));
    }

    // Invalid locale id => fallback to en
    {
        QOpcUaConnectionSettings settings;
        settings.setSessionLocaleIds({ u"xx"_s });
        opcuaClient->setConnectionSettings(settings);
        OpcuaConnector connector(opcuaClient, m_endpoint);

        QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=LocalizedTextWithCallback"_s));
        QVERIFY(node != nullptr);
        READ_MANDATORY_VARIABLE_NODE(node);
        QCOMPARE(node->valueAttribute(), QOpcUaLocalizedText(u"en"_s, u"Hello"_s));
    }

    // German locale id (short)
    {
        QOpcUaConnectionSettings settings;
        settings.setSessionLocaleIds({ u"de"_s });
        opcuaClient->setConnectionSettings(settings);
        OpcuaConnector connector(opcuaClient, m_endpoint);

        QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=LocalizedTextWithCallback"_s));
        READ_MANDATORY_VARIABLE_NODE(node);
        QCOMPARE(node->valueAttribute(), QOpcUaLocalizedText(u"de"_s, u"Guten Tag"_s));
    }

    // German locale id
    {
        QOpcUaConnectionSettings settings;
        settings.setSessionLocaleIds({ u"de-DE"_s });
        opcuaClient->setConnectionSettings(settings);
        OpcuaConnector connector(opcuaClient, m_endpoint);

        QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=LocalizedTextWithCallback"_s));
        QVERIFY(node != nullptr);
        READ_MANDATORY_VARIABLE_NODE(node);
        QCOMPARE(node->valueAttribute(), QOpcUaLocalizedText(u"de"_s, u"Guten Tag"_s));
    }

    // French locale id (short)
    {
        QOpcUaConnectionSettings settings;
        settings.setSessionLocaleIds({ u"fr"_s });
        opcuaClient->setConnectionSettings(settings);
        OpcuaConnector connector(opcuaClient, m_endpoint);

        QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=LocalizedTextWithCallback"_s));
        READ_MANDATORY_VARIABLE_NODE(node);
        QCOMPARE(node->valueAttribute(), QOpcUaLocalizedText(u"fr"_s, u"Bonjour"_s));
    }

    // French locale id
    {
        QOpcUaConnectionSettings settings;
        settings.setSessionLocaleIds({ u"fr-FR"_s });
        opcuaClient->setConnectionSettings(settings);
        OpcuaConnector connector(opcuaClient, m_endpoint);

        QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=LocalizedTextWithCallback"_s));
        QVERIFY(node != nullptr);
        READ_MANDATORY_VARIABLE_NODE(node);
        QCOMPARE(node->valueAttribute(), QOpcUaLocalizedText(u"fr"_s, u"Bonjour"_s));
    }
}

void Tst_QOpcUaClient::connectionSettings()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    QOpcUaConnectionSettings resetSettings;
    resetSettings.setSessionLocaleIds({ u"en"_s });
    opcuaClient->setConnectionSettings(resetSettings);

    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=LocalizedTextWithCallback"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QCOMPARE(node->valueAttribute(), QOpcUaLocalizedText(u"en"_s, u"Hello"_s));

    auto settings = opcuaClient->connectionSettings();
    settings.setSessionLocaleIds({ u"de"_s });
    opcuaClient->setConnectionSettings(settings);

    READ_MANDATORY_VARIABLE_NODE(node);
    QCOMPARE(node->valueAttribute(), QOpcUaLocalizedText(u"de"_s, u"Guten Tag"_s));
}

void Tst_QOpcUaClient::connectInvalidPassword()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    QOpcUaAuthenticationInformation authInfo;
    authInfo.setUsernameAuthentication(u"invaliduser"_s, u"wrongpassword"_s);
    opcuaClient->setAuthenticationInformation(authInfo);

    QSignalSpy connectSpy(opcuaClient, &QOpcUaClient::stateChanged);

    opcuaClient->connectToEndpoint(m_endpoint);
    connectSpy.wait();
    if (connectSpy.size() < 2)
        connectSpy.wait();
    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);

    QCOMPARE(opcuaClient->endpoint(), m_endpoint);
    QCOMPARE(opcuaClient->error(), QOpcUaClient::AccessDenied);
}

void Tst_QOpcUaClient::connectAndDisconnectPassword()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    QOpcUaAuthenticationInformation authInfo;
    authInfo.setUsernameAuthentication(u"user1"_s, u"password"_s);
    opcuaClient->setAuthenticationInformation(authInfo);

    QSignalSpy connectSpy(opcuaClient, &QOpcUaClient::stateChanged);

    opcuaClient->connectToEndpoint(m_endpoint);
    connectSpy.wait(signalSpyTimeout);
    if (connectSpy.size() != 2)
        connectSpy.wait();

    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connected);

    QCOMPARE(opcuaClient->endpoint(), m_endpoint);
    QCOMPARE(opcuaClient->error(), QOpcUaClient::NoError);

    connectSpy.clear();
    opcuaClient->disconnectFromEndpoint();
    connectSpy.wait(signalSpyTimeout);
    if (connectSpy.size() != 2)
        connectSpy.wait();
    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Closing);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);
}

void Tst_QOpcUaClient::findServers()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    QSignalSpy discoverySpy(opcuaClient, &QOpcUaClient::findServersFinished);

    opcuaClient->findServers(m_discoveryEndpoint);

    discoverySpy.wait(signalSpyTimeout);

    QCOMPARE(discoverySpy.size(), 1);
    QCOMPARE(discoverySpy.at(0).at(2).value<QUrl>(), m_discoveryEndpoint);

    QCOMPARE(discoverySpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    const auto servers = discoverySpy.at(0).at(0).value<QList<QOpcUaApplicationDescription>>();
    QCOMPARE(servers.size(), 1);

    QCOMPARE(servers.at(0).applicationName(), QOpcUaLocalizedText(u"en"_s, u"open62541-based OPC UA Application"_s));
    QCOMPARE(servers.at(0).applicationUri(), u"urn:open62541.server.application"_s);
    QVERIFY(servers.at(0).discoveryUrls().size() >= 1);
}

void Tst_QOpcUaClient::requestEndpoints()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    QSignalSpy endpointSpy(opcuaClient, &QOpcUaClient::endpointsRequestFinished);

    opcuaClient->requestEndpoints(m_discoveryEndpoint);
    endpointSpy.wait(signalSpyTimeout);
    QCOMPARE(endpointSpy.size(), 1);
    QCOMPARE(endpointSpy.at(0).at(2).value<QUrl>(), m_discoveryEndpoint);

    auto desc = endpointSpy.at(0).at(0).value<QList<QOpcUaEndpointDescription>>();
    QVERIFY(desc.size() > 0);

    QCOMPARE(QUrl(desc[0].endpointUrl()).port(), 43344);
    QCOMPARE(desc[0].securityPolicy(), u"http://opcfoundation.org/UA/SecurityPolicy#None"_s);
    QCOMPARE(desc[0].transportProfileUri(), u"http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary"_s);
    QCOMPARE(desc[0].securityLevel(), 0);
    QCOMPARE(desc[0].securityMode(), QOpcUaEndpointDescription::MessageSecurityMode::None);
#ifdef SERVER_SUPPORTS_SECURITY
    QFile file(u":/open62541-testserver/pki/own/certs/open62541-testserver.der"_s);
    QVERIFY(file.open(QFile::ReadOnly));
    const auto serverCertificate = file.readAll();
    QVERIFY(serverCertificate.size() > 0);
    file.close();
    QCOMPARE(desc[0].serverCertificate(), serverCertificate);
#else
    QCOMPARE(desc[0].serverCertificate(), QByteArray());
#endif

#ifdef SERVER_SUPPORTS_SECURITY
    const int numTokensExpected = opcuaClient->supportedSecurityPolicies()
            .contains(u"http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15"_s)
            ?  21 : 15;
    QCOMPARE(desc[0].userIdentityTokens().size(), numTokensExpected);
#else
    QCOMPARE(desc[0].userIdentityTokens().size(), 6);
#endif
    QCOMPARE(desc[0].userIdentityTokens()[0].policyId(), u"open62541-anonymous-policy-none#None"_s);
    QCOMPARE(desc[0].userIdentityTokens()[0].tokenType(), QOpcUaUserTokenPolicy::TokenType::Anonymous);

#ifdef SERVER_SUPPORTS_SECURITY
    QCOMPARE(desc[0].userIdentityTokens()[1].policyId(), u"open62541-certificate-policy-none#Basic256Sha256"_s);
    QCOMPARE(desc[0].userIdentityTokens()[1].tokenType(), QOpcUaUserTokenPolicy::TokenType::Certificate);
    QCOMPARE(desc[0].userIdentityTokens()[2].policyId(), u"open62541-username-policy-none#None"_s);
    QCOMPARE(desc[0].userIdentityTokens()[2].tokenType(), QOpcUaUserTokenPolicy::TokenType::Username);
#endif

    QCOMPARE(desc[0].serverRef().applicationName().text(), u"open62541-based OPC UA Application"_s);
    QCOMPARE(desc[0].serverRef().applicationType(), QOpcUaApplicationDescription::ApplicationType::Server);
    QCOMPARE(desc[0].serverRef().applicationUri(), u"urn:open62541.server.application"_s);
    QCOMPARE(desc[0].serverRef().productUri(), u"http://open62541.org"_s);
}

void Tst_QOpcUaClient::compareNodeIds()
{
    const QString numericId = u"i=42"_s;
    const QString stringId = u"s=TestString"_s;
    const QString guidId = u"g=72962b91-fa75-4ae6-8d28-b404dc7daf63"_s;
    const QString opaqueId = u"b=UXQgZnR3IQ=="_s;

    const QString prefix = u"ns=0;"_s;

    QVERIFY(QOpcUa::nodeIdEquals(numericId, prefix + numericId));
    QVERIFY(QOpcUa::nodeIdEquals(stringId, prefix + stringId));
    QVERIFY(QOpcUa::nodeIdEquals(guidId, prefix + guidId));
    QVERIFY(QOpcUa::nodeIdEquals(opaqueId, prefix + opaqueId));

    {
        quint16 namespaceIndex = 1;
        char identifierType = 0;
        QString identifier;
        QVERIFY(QOpcUa::nodeIdStringSplit(numericId, &namespaceIndex, &identifier, &identifierType));
        QCOMPARE(namespaceIndex, 0);
        QCOMPARE(identifierType, 'i');
        QCOMPARE(identifier, u"42"_s);
    }
    {
        quint16 namespaceIndex = 1;
        char identifierType = 0;
        QString identifier;
        QVERIFY(QOpcUa::nodeIdStringSplit(stringId, &namespaceIndex, &identifier, &identifierType));
        QCOMPARE(namespaceIndex, 0);
        QCOMPARE(identifierType, 's');
        QCOMPARE(identifier, u"TestString"_s);
    }
    {
        quint16 namespaceIndex = 1;
        char identifierType = 0;
        QString identifier;
        QVERIFY(QOpcUa::nodeIdStringSplit(guidId, &namespaceIndex, &identifier, &identifierType));
        QCOMPARE(namespaceIndex, 0);
        QCOMPARE(identifierType, 'g');
        QCOMPARE(identifier, u"72962b91-fa75-4ae6-8d28-b404dc7daf63"_s);
    }
    {
        quint16 namespaceIndex = 1;
        char identifierType = 0;
        QString identifier;
        QVERIFY(QOpcUa::nodeIdStringSplit(opaqueId, &namespaceIndex, &identifier, &identifierType));
        QCOMPARE(namespaceIndex, 0);
        QCOMPARE(identifierType, 'b');
        QCOMPARE(identifier, u"UXQgZnR3IQ=="_s);
    }

#ifndef QT_OPCUA_NO_NS0IDNAMES
    // This ID is a duplicate since the last generator run
    const auto enumEntry = QOpcUa::namespace0IdFromNodeId(u"ns=0;i=25584"_s);
    QCOMPARE(enumEntry, QOpcUa::NodeIds::Namespace0::WellKnownRole_SecurityKeyServerPush);
    const auto nameStr = QOpcUa::namespace0IdName(QOpcUa::NodeIds::Namespace0::WellKnownRole_SecurityKeyServerPush);
    QCOMPARE(nameStr, u"WellKnownRole_SecurityKeyServer"_s);
#endif
}

void Tst_QOpcUaClient::getNodeForIdTypes()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=1;i=42"_s));
    QVERIFY(node == nullptr);

    OpcuaConnector connector(opcuaClient, m_endpoint);

    node.reset(opcuaClient->node(u"ns=1;i=42"_s));
    QVERIFY(node != nullptr);

    node.reset(opcuaClient->node(u"ns=1;i="_s));
    QVERIFY(node == nullptr);

    node.reset(opcuaClient->node(u"ns=1;s=TestString"_s));
    QVERIFY(node != nullptr);

    node.reset(opcuaClient->node(u"ns=1;s="_s));
    QVERIFY(node == nullptr);

    node.reset(opcuaClient->node(u"ns=1;g=4365723d-39fb-4139-bbea-bbfe8202b0f9"_s));
    QVERIFY(node != nullptr);

    node.reset(opcuaClient->node(u"ns=1;g=xxxxxxxxx"_s));
    QVERIFY(node == nullptr);

    node.reset(opcuaClient->node(u"ns=1;b=VGVzdElkZW50aWZpZXI="_s));
    QVERIFY(node != nullptr);

    node.reset(opcuaClient->node(u"ns=1;b="_s));
    QVERIFY(node == nullptr);

    node.reset(opcuaClient->node(u"ns=1;q=42"_s));
    QVERIFY(node == nullptr);

    node.reset(opcuaClient->node(u"xxxxxxxxxxxxx"_s));
    QVERIFY(node == nullptr);
}

void Tst_QOpcUaClient::readNodeWithByteStringIdWithNullByte()
{
    QFETCH(QOpcUaClient*, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=1;b=AAABAAIADoo="_s));
    QVERIFY(node);

    READ_MANDATORY_VARIABLE_NODE(node);
}

void Tst_QOpcUaClient::readNS0OmitNode()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"i=84"_s)); // Root node
    QVERIFY(node != nullptr);

    READ_MANDATORY_BASE_NODE(node);

    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>(),
             QOpcUaQualifiedName(0, u"Root"_s));
}

void Tst_QOpcUaClient::readInvalidNode()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=0;s=doesnotexist"_s));
    QVERIFY(node != nullptr);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUaLocalizedText>().text(), QString());

    QSignalSpy attributeReadSpy(node.data(), &QOpcUaNode::attributeRead);

    node->readAttributes(QOpcUaNode::mandatoryBaseAttributes());
    attributeReadSpy.wait(signalSpyTimeout);

    QCOMPARE(attributeReadSpy.size(), 1);
    QCOMPARE(attributeReadSpy.at(0).at(0).value<QOpcUa::NodeAttributes>(), QOpcUaNode::mandatoryBaseAttributes());

    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::DisplayName), QVariant());
    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::DisplayName), QOpcUa::UaStatusCode::BadNodeIdUnknown);
    QCOMPARE(QOpcUa::errorCategory(node->attributeError(QOpcUa::NodeAttribute::DisplayName)), QOpcUa::ErrorCategory::NodeError);
    QVERIFY(QOpcUa::isSuccessStatus(node->attributeError(QOpcUa::NodeAttribute::DisplayName)) == false);
}

void Tst_QOpcUaClient::requestNotInCache()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::RootFolder)));
    QVERIFY(node != nullptr);

    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::BrowseName), QOpcUa::UaStatusCode::BadNoEntryExists);
    QCOMPARE(node->valueAttributeError(), QOpcUa::UaStatusCode::BadNoEntryExists);
    QCOMPARE(node->monitoringStatus(QOpcUa::NodeAttribute::Value).statusCode(), QOpcUa::UaStatusCode::BadNoEntryExists);
}

void Tst_QOpcUaClient::writeInvalidNode()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=0;s=doesnotexist"_s));
    QVERIFY(node != nullptr);

    QSignalSpy responseSpy(node.data(),&QOpcUaNode::attributeWritten);
    bool result = node->writeAttribute(QOpcUa::NodeAttribute::Value, 10, QOpcUa::Types::Int32);

    QCOMPARE(result, true);

    responseSpy.wait(signalSpyTimeout);

    QCOMPARE(responseSpy.size(), 1);
    QCOMPARE(responseSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(responseSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadNodeIdUnknown);
}

void Tst_QOpcUaClient::writeMultipleAttributes()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.QualifiedName"_s));
    QVERIFY(node != nullptr);

    QOpcUaNode::AttributeMap map;
    map[QOpcUa::NodeAttribute::DisplayName] = QOpcUaLocalizedText(u"en"_s, u"NewDisplayName"_s);
    map[QOpcUa::NodeAttribute::Value] = QOpcUaQualifiedName(2, u"TestString"_s);

    QSignalSpy writeSpy(node.data(), &QOpcUaNode::attributeWritten);

    node->writeAttributes(map, QOpcUa::Types::QualifiedName);

    writeSpy.wait(signalSpyTimeout);
    if (writeSpy.size() < 2)
        writeSpy.wait(signalSpyTimeout);

    QCOMPARE(writeSpy.size(), 2);
    QCOMPARE(writeSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::DisplayName);
    QCOMPARE(writeSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadUserAccessDenied);
    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::DisplayName), QOpcUa::UaStatusCode::BadUserAccessDenied);
    QCOMPARE(QOpcUa::errorCategory(node->attributeError(QOpcUa::NodeAttribute::DisplayName)), QOpcUa::ErrorCategory::PermissionError);

    QCOMPARE(writeSpy.at(1).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(writeSpy.at(1).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::Value), QOpcUa::UaStatusCode::Good);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::Value).value<QOpcUaQualifiedName>(), QOpcUaQualifiedName(2, u"TestString"_s));

    writeSpy.clear();
    node->writeAttributes({});

    writeSpy.wait(signalSpyTimeout);
    QCOMPARE(writeSpy.size(), 1);
    QCOMPARE(writeSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadNothingToDo);

    node.reset(opcuaClient->node(u"ns=42;s=idonotexist"_s));
    QVERIFY(node != nullptr);
    QSignalSpy writeSpy2(node.data(), &QOpcUaNode::attributeWritten);
    node->writeAttributes(map, QOpcUa::Types::QualifiedName);
    writeSpy2.wait(signalSpyTimeout);
    if (writeSpy2.size() < 2)
        writeSpy2.wait(signalSpyTimeout);
    QCOMPARE(writeSpy2.size(), 2);
    QCOMPARE(writeSpy2.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadNodeIdUnknown);
    QCOMPARE(writeSpy2.at(1).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadNodeIdUnknown);
}

void Tst_QOpcUaClient::readEmptyArrayVariable()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    // open62541 indicates an empty array with an array length of 0 and a data pointer with value 0x1.
    // This test makes sure that empty arrays are handled correctly without causing a segfault.
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=EmptyBoolArray"_s));
    QVERIFY(node != nullptr);

    READ_MANDATORY_VARIABLE_NODE(node);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::Value).metaType().id(), QMetaType::QVariantList);
    QVERIFY(node->attribute(QOpcUa::NodeAttribute::Value).toList().isEmpty());
}

void Tst_QOpcUaClient::writeNodeAttributes()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QList<QOpcUaWriteItem> request;

    request.append(QOpcUaWriteItem(u"ns=2;s=Demo.Static.Scalar.Double"_s, QOpcUa::NodeAttribute::Value,
                                      23.0, QOpcUa::Types::Double));
    request.front().setSourceTimestamp(QDateTime::fromString(u"2018-08-03 01:00:00"_s, Qt::ISODate));
    request.front().setServerTimestamp(QDateTime::fromString(u"2018-08-03T01:01:00"_s, Qt::ISODate));
    request.append(QOpcUaWriteItem(u"ns=2;s=Demo.Static.Arrays.UInt32"_s, QOpcUa::NodeAttribute::Value,
                                      QVariantList({0, 1, 2}), QOpcUa::Types::UInt32, u"0:2"_s));
    request.append(QOpcUaWriteItem(u"ns=2;s=Demo.Static.Arrays.UInt32"_s, QOpcUa::NodeAttribute::Value,
                                      QVariantList({0, 1, 2}), QOpcUa::Types::UInt32, u"0:2"_s));
    // Trigger a write error by trying to update an index range with a status code mismatch.
    request.back().setStatusCode(QOpcUa::UaStatusCode::BadDependentValueChanged);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.UInt32"_s));
    QVERIFY (node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, QVariantList({1, 2, 3, 4, 5}), QOpcUa::Types::UInt32);

    QSignalSpy writeNodeAttributesSpy(opcuaClient, &QOpcUaClient::writeNodeAttributesFinished);

    opcuaClient->writeNodeAttributes(request);

    writeNodeAttributesSpy.wait(signalSpyTimeout);

    QCOMPARE(writeNodeAttributesSpy.size(), 1);

    QCOMPARE(writeNodeAttributesSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    const auto result = writeNodeAttributesSpy.at(0).at(0).value<QList<QOpcUaWriteResult>>();

    QCOMPARE(result.size(), 3);

    for (int i = 0; i < result.size(); ++i) {
        if (i == result.size() - 1)
            QCOMPARE(result[i].statusCode(), QOpcUa::UaStatusCode::BadIndexRangeInvalid); // Status code mismatch
        else
            QCOMPARE(result[i].statusCode(), QOpcUa::UaStatusCode::Good);
        QCOMPARE(result[i].nodeId(), request[i].nodeId());
        QCOMPARE(result[i].attribute(), request[i].attribute());
        QCOMPARE(result[i].indexRange(), request[i].indexRange());
    }
}

void Tst_QOpcUaClient::readNodeAttributes()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QList<QOpcUaReadItem> request;

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.UInt32"_s));
    QVERIFY (node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, QVariantList({0, 1, 2, 3, 4}), QOpcUa::Types::UInt32);

    request.push_back(QOpcUaReadItem(u"ns=2;s=Demo.Static.Scalar.Double"_s,
                                     QOpcUa::NodeAttribute::DisplayName));
    request.push_back(QOpcUaReadItem(u"ns=2;s=Demo.Static.Scalar.Double"_s));
    request.push_back(QOpcUaReadItem(u"ns=2;s=Demo.Static.Arrays.UInt32"_s,
                                     QOpcUa::NodeAttribute::Value, u"0:2"_s));

    QSignalSpy readNodeAttributesSpy(opcuaClient, &QOpcUaClient::readNodeAttributesFinished);

    opcuaClient->readNodeAttributes(request);

    readNodeAttributesSpy.wait(signalSpyTimeout);

    QCOMPARE(readNodeAttributesSpy.size(), 1);

    QCOMPARE(readNodeAttributesSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    const auto result = readNodeAttributesSpy.at(0).at(0).value<QList<QOpcUaReadResult>>();

    QCOMPARE(result.size(), 3);

    for (int i = 0; i < result.size(); ++i) {
        QCOMPARE(result[i].statusCode(), QOpcUa::UaStatusCode::Good);
        QVERIFY(result[i].serverTimestamp().isValid());
        QCOMPARE(result[i].nodeId(), request[i].nodeId());
        QCOMPARE(result[i].attribute(), request[i].attribute());
        QCOMPARE(result[i].indexRange(), request[i].indexRange());
    }
    QVERIFY(!result[0].sourceTimestamp().isValid()); // The initial DisplayName attribute doesn't have a source timestamp
    QVERIFY(result[1].sourceTimestamp().isValid());
    QVERIFY(result[2].sourceTimestamp().isValid());
    QCOMPARE(result[0].value().value<QOpcUaLocalizedText>().text(), u"DoubleScalarTest"_s);
    QCOMPARE(result[1].value(), 23.0);
    QCOMPARE(result[2].value(), QVariantList({0, 1, 2}));
    // Only check the source timestamp, the server timestamp is replaced with the current DateTime in the open62541
    // server's Read service.
    QCOMPARE(result[1].sourceTimestamp(), QDateTime::fromString(u"2018-08-03 01:00:00"_s, Qt::ISODate));
}

void Tst_QOpcUaClient::readDataTypeDefinition()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Range)));
    QVERIFY (node != nullptr);

    node->readAttributes(QOpcUa::NodeAttribute::DataTypeDefinition);

    QSignalSpy spy(node.get(), &QOpcUaNode::attributeRead);
    spy.wait();

    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy.at(0).at(0).value<QOpcUa::NodeAttributes>().testFlag(QOpcUa::NodeAttribute::DataTypeDefinition), true);

    qDebug() << node->valueAttribute();
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::DataTypeDefinition).canConvert<QOpcUaStructureDefinition>(), true);
}

void Tst_QOpcUaClient::readAccessLevelEx()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.UInt32"_s));
    QVERIFY (node != nullptr);

    QSignalSpy spy(node.get(), &QOpcUaNode::attributeRead);
    node->readAttributes(QOpcUa::NodeAttribute::AccessLevelEx);

    spy.wait();

    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy.at(0).at(0).value<QOpcUa::NodeAttributes>().testFlag(QOpcUa::NodeAttribute::AccessLevelEx), true);

    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::AccessLevelEx), QOpcUa::UaStatusCode::Good);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::AccessLevelEx).value<quint32>(), 3);
    QOpcUa::AccessLevelEx ex(node->attribute(QOpcUa::NodeAttribute::AccessLevelEx).value<quint32>());
    QVERIFY(ex.testFlag(QOpcUa::AccessLevelExBit::CurrentRead));
    QVERIFY(ex.testFlag(QOpcUa::AccessLevelExBit::CurrentWrite));
}

void Tst_QOpcUaClient::readNewPermissionAttributes()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.UInt32"_s));
    QVERIFY (node != nullptr);

    QSignalSpy spy(node.get(), &QOpcUaNode::attributeRead);

    node->readAttributes(QOpcUa::NodeAttribute::RolePermissions | QOpcUa::NodeAttribute::UserRolePermissions
                         | QOpcUa::NodeAttribute::AccessRestrictions);

    spy.wait();

    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy.at(0).at(0).value<QOpcUa::NodeAttributes>().testFlag(QOpcUa::NodeAttribute::RolePermissions), true);
    QCOMPARE(spy.at(0).at(0).value<QOpcUa::NodeAttributes>().testFlag(QOpcUa::NodeAttribute::UserRolePermissions), true);
    QCOMPARE(spy.at(0).at(0).value<QOpcUa::NodeAttributes>().testFlag(QOpcUa::NodeAttribute::AccessRestrictions), true);

    // The open62541 server doesn't yet support the three attributes, so we can only check if a read was attempted
    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::RolePermissions), QOpcUa::UaStatusCode::BadAttributeIdInvalid);
    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::UserRolePermissions), QOpcUa::UaStatusCode::BadAttributeIdInvalid);
    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::AccessRestrictions), QOpcUa::UaStatusCode::BadAttributeIdInvalid);
}

void Tst_QOpcUaClient::getRootNode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> root(opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::RootFolder)));
    QVERIFY(root != nullptr);
    QVERIFY(root->client() == opcuaClient);

    READ_MANDATORY_BASE_NODE(root)
    QCOMPARE(root->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUaLocalizedText>().text(), u"Root"_s);

    QString nodeId = root->nodeId();
    QCOMPARE(nodeId, QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::RootFolder));
}

void Tst_QOpcUaClient::getChildren()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=1;s=Large.Folder"_s));
    QVERIFY(node != nullptr);
    QSignalSpy spy(node.data(), &QOpcUaNode::browseFinished);
    node->browseChildren(QOpcUa::ReferenceTypeId::HierarchicalReferences, QOpcUa::NodeClass::Object);
    spy.wait(signalSpyTimeout);
    QCOMPARE(spy.size(), 1);
    const auto ref = spy.at(0).at(0).value<QList<QOpcUaReferenceDescription>>();
    QCOMPARE(ref.size(), 100);
}

void Tst_QOpcUaClient::childrenIdsString()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=3;s=testStringIdsFolder"_s));
    QVERIFY(node != nullptr);
    QSignalSpy spy(node.data(), &QOpcUaNode::browseFinished);
    node->browseChildren(QOpcUa::ReferenceTypeId::Organizes, QOpcUa::NodeClass::Variable);
    spy.wait(signalSpyTimeout);
    QCOMPARE(spy.size(), 1);
    const auto ref = spy.at(0).at(0).value<QList<QOpcUaReferenceDescription>>();
    QCOMPARE(ref.size(), 1);
    QCOMPARE(ref.at(0).targetNodeId().nodeId(), u"ns=3;s=theStringId"_s);
}

void Tst_QOpcUaClient::childrenIdsGuidNodeId()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=3;s=testGuidIdsFolder"_s));
    QVERIFY(node != nullptr);
    QSignalSpy spy(node.data(), &QOpcUaNode::browseFinished);
    node->browseChildren(QOpcUa::ReferenceTypeId::Organizes, QOpcUa::NodeClass::Variable);
    spy.wait(signalSpyTimeout);
    QCOMPARE(spy.size(), 1);
    const auto ref = spy.at(0).at(0).value<QList<QOpcUaReferenceDescription>>();
    QCOMPARE(ref.size(), 1);
    QCOMPARE(ref.at(0).targetNodeId().nodeId(), u"ns=3;g=08081e75-8e5e-319b-954f-f3a7613dc29b"_s);
}

void Tst_QOpcUaClient::childrenIdsOpaqueNodeId()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=3;s=testOpaqueIdsFolder"_s));
    QVERIFY(node != nullptr);
    QSignalSpy spy(node.data(), &QOpcUaNode::browseFinished);
    node->browseChildren(QOpcUa::ReferenceTypeId::Organizes, QOpcUa::NodeClass::Variable);
    spy.wait(signalSpyTimeout);
    QCOMPARE(spy.size(), 1);
    const auto ref = spy.at(0).at(0).value<QList<QOpcUaReferenceDescription>>();
    QCOMPARE(ref.size(), 1);
    QCOMPARE(ref.at(0).targetNodeId().nodeId(), u"ns=3;b=UXQgZnR3IQ=="_s);
}

void Tst_QOpcUaClient::testSpecialCharStringNodeIds()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    // Get child node id

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=3;s=ümläutFölderNödeId"_s));
    QVERIFY(node != nullptr);
    QSignalSpy spy(node.data(), &QOpcUaNode::browseFinished);
    node->browseChildren(QOpcUa::ReferenceTypeId::Organizes, QOpcUa::NodeClass::Variable);
    spy.wait(signalSpyTimeout);
    QCOMPARE(spy.size(), 1);
    QList<QOpcUaReferenceDescription> ref = spy.at(0).at(0).value<QList<QOpcUaReferenceDescription>>();
    QCOMPARE(ref.size(), 1);
    QCOMPARE(ref.at(0).targetNodeId().nodeId(), u"ns=3;s=ümläutVäriableNödeId"_s);
    QCOMPARE(ref.at(0).browseName().namespaceIndex(), 3);
    QCOMPARE(ref.at(0).browseName().name(), u"ümläutVäriableNödeId"_s);

    // Make sure the id can be used for subsequent requests

    node.reset(opcuaClient->node(ref.at(0).targetNodeId().nodeId()));
    QSignalSpy readSpy(node.data(), &QOpcUaNode::attributeUpdated);

    node->readAttributes(QOpcUa::NodeAttribute::BrowseName);
    readSpy.wait(signalSpyTimeout);
    QCOMPARE(readSpy.size(), 1);

    QOpcUaQualifiedName nameFromSignal = readSpy.at(0).at(1).value<QOpcUaQualifiedName>();
    QOpcUaQualifiedName nameFromAttributes = node->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>();

    QCOMPARE(nameFromSignal.namespaceIndex(), 3);
    QCOMPARE(nameFromSignal.name(), u"ümläutVäriableNödeId"_s);

    QCOMPARE(nameFromAttributes.namespaceIndex(), 3);
    QCOMPARE(nameFromAttributes.name(), u"ümläutVäriableNödeId"_s);
}

void Tst_QOpcUaClient::inverseBrowse()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Boolean)));
    QVERIFY(node != nullptr);
    QSignalSpy spy(node.data(), &QOpcUaNode::browseFinished);

    QOpcUaBrowseRequest request;
    request.setReferenceTypeId(QOpcUa::ReferenceTypeId::References);
    request.setIncludeSubtypes(true);
    request.setNodeClassMask(QOpcUa::NodeClass::Undefined);
    request.setBrowseDirection(QOpcUaBrowseRequest::BrowseDirection::Inverse);
    node->browse(request);
    spy.wait(signalSpyTimeout);
    QCOMPARE(spy.size(), 1);
    const auto ref = spy.at(0).at(0).value<QList<QOpcUaReferenceDescription>>();
    QCOMPARE(ref.size(), 1);
    QCOMPARE(ref.at(0).targetNodeId().nodeId(), QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::BaseDataType));
    QCOMPARE(ref.at(0).isForwardReference(), false);
    QCOMPARE(ref.at(0).browseName().name(), u"BaseDataType"_s);
    QCOMPARE(ref.at(0).displayName().text(), u"BaseDataType"_s);
    QCOMPARE(ref.at(0).refTypeId(), QOpcUa::nodeIdFromReferenceType(QOpcUa::ReferenceTypeId::HasSubtype));
    QCOMPARE(ref.at(0).nodeClass(), QOpcUa::NodeClass::DataType);
}

void Tst_QOpcUaClient::addAndRemoveObjectNode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QSignalSpy addNodeSpy(opcuaClient, &QOpcUaClient::addNodeFinished);

    QOpcUaExpandedNodeId parent;
    parent.setNodeId(u"ns=3;s=TestFolder"_s);
    QOpcUaExpandedNodeId requestedNewId;
    requestedNewId.setNodeId(u"ns=3;s=DynamicObjectNode_%1"_s.arg(opcuaClient->backend()));
    const quint16 namespaceIndex = 3;
    QOpcUaQualifiedName browseName(namespaceIndex, u"DynamicObjectNode_%1"_s.arg(opcuaClient->backend()));

    QOpcUaNodeCreationAttributes attributes;

    QOpcUaLocalizedText displayName(u"en"_s, browseName.name());
    QOpcUaLocalizedText description(u"en"_s, u"Node added at runtime by %1"_s.arg(opcuaClient->backend()));

    attributes.setDisplayName(displayName);
    attributes.setDescription(description);
    attributes.setEventNotifier(QOpcUa::EventNotifierBit::SubscribeToEvents);
    attributes.setWriteMask(QOpcUa::WriteMaskBit::DisplayName);
    attributes.setUserWriteMask(QOpcUa::WriteMaskBit::DisplayName);

    QOpcUaAddNodeItem nodeInfo;
    nodeInfo.setParentNodeId(parent);
    nodeInfo.setReferenceTypeId(QOpcUa::nodeIdFromReferenceType(QOpcUa::ReferenceTypeId::Organizes));
    nodeInfo.setRequestedNewNodeId(requestedNewId);
    nodeInfo.setBrowseName(browseName);
    nodeInfo.setNodeClass(QOpcUa::NodeClass::Object);
    nodeInfo.setNodeAttributes(attributes);

    opcuaClient->addNode(nodeInfo);

    addNodeSpy.wait(signalSpyTimeout);

    QCOMPARE(addNodeSpy.size(), 1);

    QCOMPARE(addNodeSpy.at(0).at(0).value<QOpcUaExpandedNodeId>().nodeId(), requestedNewId.nodeId());
    QCOMPARE(addNodeSpy.at(0).at(1), requestedNewId.nodeId());
    QCOMPARE(addNodeSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    QScopedPointer<QOpcUaNode> newNode(opcuaClient->node(requestedNewId.nodeId()));
    QVERIFY(newNode != nullptr);

    QSignalSpy readSpy(newNode.data(), &QOpcUaNode::attributeRead);
    newNode->readAttributes(QOpcUa::NodeAttribute::BrowseName | QOpcUa::NodeAttribute::DisplayName |
                            QOpcUa::NodeAttribute::Description | QOpcUa::NodeAttribute::EventNotifier |
                            QOpcUa::NodeAttribute::WriteMask | QOpcUa::NodeAttribute::UserWriteMask);
    readSpy.wait(signalSpyTimeout);

    QCOMPARE(readSpy.size(), 1);

    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::BrowseName), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::DisplayName), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::Description), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::EventNotifier), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::WriteMask), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::UserWriteMask), QOpcUa::UaStatusCode::Good);

    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>(), browseName);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUaLocalizedText>(), displayName);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::Description).value<QOpcUaLocalizedText>(), description);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::EventNotifier).value<quint8>(),
             static_cast<quint8>(QOpcUa::EventNotifierBit::SubscribeToEvents));
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::WriteMask).value<quint8>(),
             static_cast<quint8>(QOpcUa::WriteMaskBit::DisplayName));
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::UserWriteMask).value<quint8>(),
             static_cast<quint8>(QOpcUa::WriteMaskBit::DisplayName));

    QSignalSpy removeNodeSpy(opcuaClient, &QOpcUaClient::deleteNodeFinished);
    opcuaClient->deleteNode(requestedNewId.nodeId(), true);
    removeNodeSpy.wait(signalSpyTimeout);

    QCOMPARE(removeNodeSpy.size(), 1);
    QCOMPARE(removeNodeSpy.at(0).at(0), requestedNewId.nodeId());
    QCOMPARE(removeNodeSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(requestedNewId.nodeId()));
    QVERIFY(node != nullptr);

    QSignalSpy unsuccessfulReadSpy(node.data(), &QOpcUaNode::attributeRead);

    node->readAttributes(QOpcUa::NodeAttribute::NodeId);
    unsuccessfulReadSpy.wait(signalSpyTimeout);
    QCOMPARE(unsuccessfulReadSpy.size(), 1);
    QVERIFY(unsuccessfulReadSpy.at(0).at(0).value<QOpcUa::NodeAttributes>() & QOpcUa::NodeAttribute::NodeId);
    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::NodeId), QOpcUa::UaStatusCode::BadNodeIdUnknown);
}

void Tst_QOpcUaClient::addAndRemoveVariableNode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QSignalSpy addNodeSpy(opcuaClient, &QOpcUaClient::addNodeFinished);

    QOpcUaExpandedNodeId parent;
    parent.setNodeId(u"ns=3;s=TestFolder"_s);
    QOpcUaExpandedNodeId requestedNewId;
    requestedNewId.setNodeId(u"ns=3;s=DynamicVariableNode_%1"_s.arg(opcuaClient->backend()));
    const quint16 namespaceIndex = 3;
    QOpcUaQualifiedName browseName(namespaceIndex, u"DynamicVariableNode_%1"_s.arg(opcuaClient->backend()));

    QOpcUaNodeCreationAttributes attributes;

    QOpcUaLocalizedText displayName(u"en"_s, browseName.name());
    QOpcUaLocalizedText description(u"en"_s, u"Node added at runtime by %1"_s.arg(opcuaClient->backend()));

    attributes.setDisplayName(displayName);
    attributes.setDescription(description);
    attributes.setValue(QOpcUaMultiDimensionalArray({1,2,3,4}, {2,2}), QOpcUa::Types::Double);
    attributes.setDataTypeId(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Double));
    attributes.setValueRank(2); // Two-dimensional array
    attributes.setArrayDimensions({ 2, 2 });
    attributes.setMinimumSamplingInterval(23.0);
    attributes.setHistorizing(true);

    attributes.setAccessLevel(QOpcUa::AccessLevelBit::CurrentRead);
    attributes.setUserAccessLevel(QOpcUa::AccessLevelBit::CurrentRead);

    QOpcUaAddNodeItem nodeInfo;
    nodeInfo.setParentNodeId(parent);
    nodeInfo.setReferenceTypeId(QOpcUa::nodeIdFromReferenceType(QOpcUa::ReferenceTypeId::Organizes));
    nodeInfo.setRequestedNewNodeId(requestedNewId);
    nodeInfo.setBrowseName(browseName);
    nodeInfo.setNodeClass(QOpcUa::NodeClass::Variable);
    nodeInfo.setNodeAttributes(attributes);

    opcuaClient->addNode(nodeInfo);

    addNodeSpy.wait(signalSpyTimeout);

    QCOMPARE(addNodeSpy.size(), 1);

    QCOMPARE(addNodeSpy.at(0).at(0).value<QOpcUaExpandedNodeId>().nodeId(), requestedNewId.nodeId());
    QCOMPARE(addNodeSpy.at(0).at(1), requestedNewId.nodeId());
    QCOMPARE(addNodeSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    QScopedPointer<QOpcUaNode> newNode(opcuaClient->node(requestedNewId.nodeId()));
    QVERIFY(newNode != nullptr);

    QSignalSpy readSpy(newNode.data(), &QOpcUaNode::attributeRead);
    newNode->readAttributes(QOpcUa::NodeAttribute::BrowseName | QOpcUa::NodeAttribute::DisplayName |
                            QOpcUa::NodeAttribute::Description | QOpcUa::NodeAttribute::Value |
                            QOpcUa::NodeAttribute::ValueRank | QOpcUa::NodeAttribute::ArrayDimensions |
                            QOpcUa::NodeAttribute::MinimumSamplingInterval | QOpcUa::NodeAttribute::Historizing);
    readSpy.wait(signalSpyTimeout);

    QCOMPARE(readSpy.size(), 1);

    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::BrowseName), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::DisplayName), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::Description), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::Value), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::ValueRank), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::ArrayDimensions), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::MinimumSamplingInterval), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::Historizing), QOpcUa::UaStatusCode::Good);


    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>(), browseName);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUaLocalizedText>(), displayName);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::Description).value<QOpcUaLocalizedText>(), description);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::Value).value<QOpcUaMultiDimensionalArray>().valueArray(), QVariantList({1, 2, 3, 4}));
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::ValueRank), 2);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::ArrayDimensions).toList(), QVariantList({ 2, 2 }));
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::MinimumSamplingInterval).toDouble(), 23.0);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::Historizing), true);

    QSignalSpy removeNodeSpy(opcuaClient, &QOpcUaClient::deleteNodeFinished);
    opcuaClient->deleteNode(requestedNewId.nodeId(), true);
    removeNodeSpy.wait(signalSpyTimeout);

    QCOMPARE(removeNodeSpy.size(), 1);
    QCOMPARE(removeNodeSpy.at(0).at(0), requestedNewId.nodeId());
    QCOMPARE(removeNodeSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(requestedNewId.nodeId()));
    QVERIFY(node != nullptr);

    QSignalSpy unsuccessfulReadSpy(node.data(), &QOpcUaNode::attributeRead);

    node->readAttributes(QOpcUa::NodeAttribute::NodeId);
    unsuccessfulReadSpy.wait(signalSpyTimeout);
    QCOMPARE(unsuccessfulReadSpy.size(), 1);
    QVERIFY(unsuccessfulReadSpy.at(0).at(0).value<QOpcUa::NodeAttributes>() & QOpcUa::NodeAttribute::NodeId);
    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::NodeId), QOpcUa::UaStatusCode::BadNodeIdUnknown);
}

void Tst_QOpcUaClient::addAndRemoveMethodNode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QSignalSpy addNodeSpy(opcuaClient, &QOpcUaClient::addNodeFinished);

    QOpcUaExpandedNodeId parent;
    parent.setNodeId(u"ns=3;s=TestFolder"_s);
    QOpcUaExpandedNodeId requestedNewId;
    requestedNewId.setNodeId(u"ns=3;s=DynamicMethodNode_%1"_s.arg(opcuaClient->backend()));
    const quint16 namespaceIndex = 3;
    QOpcUaQualifiedName browseName(namespaceIndex, u"DynamicMethodNode_%1"_s.arg(opcuaClient->backend()));

    QOpcUaNodeCreationAttributes attributes;

    QOpcUaLocalizedText displayName(u"en"_s, browseName.name());
    QOpcUaLocalizedText description(u"en"_s, u"Node added at runtime by %1"_s.arg(opcuaClient->backend()));

    attributes.setDisplayName(displayName);
    attributes.setDescription(description);
    attributes.setExecutable(true);
    attributes.setUserExecutable(true);

    QOpcUaAddNodeItem nodeInfo;
    nodeInfo.setParentNodeId(parent);
    nodeInfo.setReferenceTypeId(QOpcUa::nodeIdFromReferenceType(QOpcUa::ReferenceTypeId::Organizes));
    nodeInfo.setRequestedNewNodeId(requestedNewId);
    nodeInfo.setBrowseName(browseName);
    nodeInfo.setNodeClass(QOpcUa::NodeClass::Method);
    nodeInfo.setNodeAttributes(attributes);

    opcuaClient->addNode(nodeInfo);

    addNodeSpy.wait(signalSpyTimeout);

    QCOMPARE(addNodeSpy.size(), 1);

    QCOMPARE(addNodeSpy.at(0).at(0).value<QOpcUaExpandedNodeId>().nodeId(), requestedNewId.nodeId());
    QCOMPARE(addNodeSpy.at(0).at(1), requestedNewId.nodeId());
    QCOMPARE(addNodeSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    QScopedPointer<QOpcUaNode> newNode(opcuaClient->node(requestedNewId.nodeId()));
    QVERIFY(newNode != nullptr);

    QSignalSpy readSpy(newNode.data(), &QOpcUaNode::attributeRead);
    newNode->readAttributes(QOpcUa::NodeAttribute::BrowseName | QOpcUa::NodeAttribute::DisplayName |
                            QOpcUa::NodeAttribute::Description | QOpcUa::NodeAttribute::Executable |
                            QOpcUa::NodeAttribute::UserExecutable);
    readSpy.wait(signalSpyTimeout);

    QCOMPARE(readSpy.size(), 1);

    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::BrowseName), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::DisplayName), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::Description), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::Executable), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::UserExecutable), QOpcUa::UaStatusCode::Good);

    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>(), browseName);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUaLocalizedText>(), displayName);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::Description).value<QOpcUaLocalizedText>(), description);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::Executable), true);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::UserExecutable), true);

    QSignalSpy removeNodeSpy(opcuaClient, &QOpcUaClient::deleteNodeFinished);
    opcuaClient->deleteNode(requestedNewId.nodeId(), true);
    removeNodeSpy.wait(signalSpyTimeout);

    QCOMPARE(removeNodeSpy.size(), 1);
    QCOMPARE(removeNodeSpy.at(0).at(0), requestedNewId.nodeId());
    QCOMPARE(removeNodeSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(requestedNewId.nodeId()));
    QVERIFY(node != nullptr);

    QSignalSpy unsuccessfulReadSpy(node.data(), &QOpcUaNode::attributeRead);

    node->readAttributes(QOpcUa::NodeAttribute::NodeId);
    unsuccessfulReadSpy.wait(signalSpyTimeout);
    QCOMPARE(unsuccessfulReadSpy.size(), 1);
    QVERIFY(unsuccessfulReadSpy.at(0).at(0).value<QOpcUa::NodeAttributes>() & QOpcUa::NodeAttribute::NodeId);
    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::NodeId), QOpcUa::UaStatusCode::BadNodeIdUnknown);
}

void Tst_QOpcUaClient::addAndRemoveObjectTypeNode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QSignalSpy addNodeSpy(opcuaClient, &QOpcUaClient::addNodeFinished);

    QOpcUaExpandedNodeId parent;
    parent.setNodeId(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::BaseObjectType));
    QOpcUaExpandedNodeId requestedNewId;
    requestedNewId.setNodeId(u"ns=3;s=DynamicObjectTypeNode_%1"_s.arg(opcuaClient->backend()));
    const quint16 namespaceIndex = 3;
    QOpcUaQualifiedName browseName(namespaceIndex, u"DynamicObjectTypeNode_%1"_s.arg(opcuaClient->backend()));

    QOpcUaNodeCreationAttributes attributes;

    QOpcUaLocalizedText displayName(u"en"_s, browseName.name());
    QOpcUaLocalizedText description(u"en"_s, u"Node added at runtime by %1"_s.arg(opcuaClient->backend()));

    attributes.setDisplayName(displayName);
    attributes.setDescription(description);
    attributes.setIsAbstract(true);

    QOpcUaAddNodeItem nodeInfo;
    nodeInfo.setParentNodeId(parent);
    nodeInfo.setReferenceTypeId(QOpcUa::nodeIdFromReferenceType(QOpcUa::ReferenceTypeId::HasSubtype));
    nodeInfo.setRequestedNewNodeId(requestedNewId);
    nodeInfo.setBrowseName(browseName);
    nodeInfo.setNodeClass(QOpcUa::NodeClass::ObjectType);
    nodeInfo.setNodeAttributes(attributes);

    opcuaClient->addNode(nodeInfo);

    addNodeSpy.wait(signalSpyTimeout);

    QCOMPARE(addNodeSpy.size(), 1);

    QCOMPARE(addNodeSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(addNodeSpy.at(0).at(0).value<QOpcUaExpandedNodeId>().nodeId(), requestedNewId.nodeId());
    QCOMPARE(addNodeSpy.at(0).at(1), requestedNewId.nodeId());

    QScopedPointer<QOpcUaNode> newNode(opcuaClient->node(requestedNewId.nodeId()));
    QVERIFY(newNode != nullptr);

    QSignalSpy readSpy(newNode.data(), &QOpcUaNode::attributeRead);
    newNode->readAttributes(QOpcUa::NodeAttribute::BrowseName | QOpcUa::NodeAttribute::DisplayName |
                            QOpcUa::NodeAttribute::Description | QOpcUa::NodeAttribute::IsAbstract);
    readSpy.wait(signalSpyTimeout);

    QCOMPARE(readSpy.size(), 1);

    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::BrowseName), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::DisplayName), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::Description), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::IsAbstract), QOpcUa::UaStatusCode::Good);

    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>(), browseName);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUaLocalizedText>(), displayName);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::Description).value<QOpcUaLocalizedText>(), description);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::IsAbstract), true);

    QSignalSpy removeNodeSpy(opcuaClient, &QOpcUaClient::deleteNodeFinished);
    opcuaClient->deleteNode(requestedNewId.nodeId(), true);
    removeNodeSpy.wait(signalSpyTimeout);

    QCOMPARE(removeNodeSpy.size(), 1);
    QCOMPARE(removeNodeSpy.at(0).at(0), requestedNewId.nodeId());
    QCOMPARE(removeNodeSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(requestedNewId.nodeId()));
    QVERIFY(node != nullptr);

    QSignalSpy unsuccessfulReadSpy(node.data(), &QOpcUaNode::attributeRead);

    node->readAttributes(QOpcUa::NodeAttribute::NodeId);
    unsuccessfulReadSpy.wait(signalSpyTimeout);
    QCOMPARE(unsuccessfulReadSpy.size(), 1);
    QVERIFY(unsuccessfulReadSpy.at(0).at(0).value<QOpcUa::NodeAttributes>() & QOpcUa::NodeAttribute::NodeId);
    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::NodeId), QOpcUa::UaStatusCode::BadNodeIdUnknown);
}

void Tst_QOpcUaClient::addAndRemoveVariableTypeNode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QSignalSpy addNodeSpy(opcuaClient, &QOpcUaClient::addNodeFinished);

    QOpcUaExpandedNodeId parent;
    parent.setNodeId(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::BaseVariableType));
    QOpcUaExpandedNodeId requestedNewId;
    requestedNewId.setNodeId(u"ns=3;s=DynamicVariableTypeNode_%1"_s.arg(opcuaClient->backend()));
    const quint16 namespaceIndex = 3;
    QOpcUaQualifiedName browseName(namespaceIndex, u"DynamicVariableTypeNode_%1"_s.arg(opcuaClient->backend()));

    QOpcUaNodeCreationAttributes attributes;

    QOpcUaLocalizedText displayName(u"en"_s, browseName.name());
    QOpcUaLocalizedText description(u"en"_s, u"Node added at runtime by %1"_s.arg(opcuaClient->backend()));

    attributes.setDisplayName(displayName);
    attributes.setDescription(description);
    attributes.setValue(QOpcUaMultiDimensionalArray({1,2,3,4}, {2,2}), QOpcUa::Types::Double);
    attributes.setDataTypeId(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Double));
    attributes.setValueRank(2); // Two-dimensional array
    attributes.setArrayDimensions({ 2, 2 });
    attributes.setIsAbstract(true);

    attributes.setAccessLevel(QOpcUa::AccessLevelBit::CurrentRead);
    attributes.setUserAccessLevel(QOpcUa::AccessLevelBit::CurrentRead);

    QOpcUaAddNodeItem nodeInfo;
    nodeInfo.setParentNodeId(parent);
    nodeInfo.setReferenceTypeId(QOpcUa::nodeIdFromReferenceType(QOpcUa::ReferenceTypeId::HasSubtype));
    nodeInfo.setRequestedNewNodeId(requestedNewId);
    nodeInfo.setBrowseName(browseName);
    nodeInfo.setNodeClass(QOpcUa::NodeClass::VariableType);
    nodeInfo.setNodeAttributes(attributes);

    opcuaClient->addNode(nodeInfo);

    addNodeSpy.wait(signalSpyTimeout);

    QCOMPARE(addNodeSpy.size(), 1);

    QCOMPARE(addNodeSpy.at(0).at(0).value<QOpcUaExpandedNodeId>().nodeId(), requestedNewId.nodeId());
    QCOMPARE(addNodeSpy.at(0).at(1), requestedNewId.nodeId());
    QCOMPARE(addNodeSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    QScopedPointer<QOpcUaNode> newNode(opcuaClient->node(requestedNewId.nodeId()));
    QVERIFY(newNode != nullptr);

    QSignalSpy readSpy(newNode.data(), &QOpcUaNode::attributeRead);
    newNode->readAttributes(QOpcUa::NodeAttribute::BrowseName | QOpcUa::NodeAttribute::DisplayName |
                            QOpcUa::NodeAttribute::Description | QOpcUa::NodeAttribute::Value |
                            QOpcUa::NodeAttribute::ValueRank | QOpcUa::NodeAttribute::ArrayDimensions |
                            QOpcUa::NodeAttribute::IsAbstract);
    readSpy.wait(signalSpyTimeout);

    QCOMPARE(readSpy.size(), 1);

    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::BrowseName), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::DisplayName), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::Description), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::Value), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::ValueRank), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::ArrayDimensions), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::IsAbstract), QOpcUa::UaStatusCode::Good);


    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>(), browseName);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUaLocalizedText>(), displayName);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::Description).value<QOpcUaLocalizedText>(), description);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::Value).value<QOpcUaMultiDimensionalArray>().valueArray(), QVariantList({1, 2, 3, 4}));
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::ValueRank), 2);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::ArrayDimensions).toList(), QVariantList({ 2, 2 }));
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::IsAbstract), true);

    QSignalSpy removeNodeSpy(opcuaClient, &QOpcUaClient::deleteNodeFinished);
    opcuaClient->deleteNode(requestedNewId.nodeId(), true);
    removeNodeSpy.wait(signalSpyTimeout);

    QCOMPARE(removeNodeSpy.size(), 1);
    QCOMPARE(removeNodeSpy.at(0).at(0), requestedNewId.nodeId());
    QCOMPARE(removeNodeSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(requestedNewId.nodeId()));
    QVERIFY(node != nullptr);

    QSignalSpy unsuccessfulReadSpy(node.data(), &QOpcUaNode::attributeRead);

    node->readAttributes(QOpcUa::NodeAttribute::NodeId);
    unsuccessfulReadSpy.wait(signalSpyTimeout);
    QCOMPARE(unsuccessfulReadSpy.size(), 1);
    QVERIFY(unsuccessfulReadSpy.at(0).at(0).value<QOpcUa::NodeAttributes>() & QOpcUa::NodeAttribute::NodeId);
    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::NodeId), QOpcUa::UaStatusCode::BadNodeIdUnknown);
}

void Tst_QOpcUaClient::addAndRemoveReferenceTypeNode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QSignalSpy addNodeSpy(opcuaClient, &QOpcUaClient::addNodeFinished);

    QOpcUaExpandedNodeId parent;
    parent.setNodeId(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Organizes));
    QOpcUaExpandedNodeId requestedNewId;
    requestedNewId.setNodeId(u"ns=3;s=DynamicReferenceTypeNode_%1"_s.arg(opcuaClient->backend()));
    const quint16 namespaceIndex = 3;
    QOpcUaQualifiedName browseName(namespaceIndex, u"DynamicReferenceTypeNode_%1"_s.arg(opcuaClient->backend()));

    QOpcUaNodeCreationAttributes attributes;

    QOpcUaLocalizedText displayName(u"en"_s, browseName.name());
    QOpcUaLocalizedText description(u"en"_s, u"Node added at runtime by %1"_s.arg(opcuaClient->backend()));

    attributes.setDisplayName(displayName);
    attributes.setDescription(description);
    attributes.setIsAbstract(true);
    attributes.setSymmetric(false);
    attributes.setInverseName({ u"en"_s, u"Inverse name"_s });

    QOpcUaAddNodeItem nodeInfo;
    nodeInfo.setParentNodeId(parent);
    nodeInfo.setReferenceTypeId(QOpcUa::nodeIdFromReferenceType(QOpcUa::ReferenceTypeId::HasSubtype));
    nodeInfo.setRequestedNewNodeId(requestedNewId);
    nodeInfo.setBrowseName(browseName);
    nodeInfo.setNodeClass(QOpcUa::NodeClass::ReferenceType);
    nodeInfo.setNodeAttributes(attributes);

    opcuaClient->addNode(nodeInfo);

    addNodeSpy.wait(signalSpyTimeout);

    QCOMPARE(addNodeSpy.size(), 1);

    QCOMPARE(addNodeSpy.at(0).at(0).value<QOpcUaExpandedNodeId>().nodeId(), requestedNewId.nodeId());
    QCOMPARE(addNodeSpy.at(0).at(1), requestedNewId.nodeId());
    QCOMPARE(addNodeSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    QScopedPointer<QOpcUaNode> newNode(opcuaClient->node(requestedNewId.nodeId()));
    QVERIFY(newNode != nullptr);

    QSignalSpy readSpy(newNode.data(), &QOpcUaNode::attributeRead);
    newNode->readAttributes(QOpcUa::NodeAttribute::BrowseName | QOpcUa::NodeAttribute::DisplayName |
                            QOpcUa::NodeAttribute::Description | QOpcUa::NodeAttribute::IsAbstract |
                            QOpcUa::NodeAttribute::Symmetric | QOpcUa::NodeAttribute::InverseName);
    readSpy.wait(signalSpyTimeout);

    QCOMPARE(readSpy.size(), 1);

    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::BrowseName), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::DisplayName), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::Description), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::IsAbstract), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::Symmetric), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::InverseName), QOpcUa::UaStatusCode::Good);

    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>(), browseName);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUaLocalizedText>(), displayName);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::Description).value<QOpcUaLocalizedText>(), description);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::IsAbstract), true);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::Symmetric), false);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::InverseName), QOpcUaLocalizedText(u"en"_s, u"Inverse name"_s));

    QSignalSpy removeNodeSpy(opcuaClient, &QOpcUaClient::deleteNodeFinished);
    opcuaClient->deleteNode(requestedNewId.nodeId(), true);
    removeNodeSpy.wait(signalSpyTimeout);

    QCOMPARE(removeNodeSpy.size(), 1);
    QCOMPARE(removeNodeSpy.at(0).at(0), requestedNewId.nodeId());
    QCOMPARE(removeNodeSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(requestedNewId.nodeId()));
    QVERIFY(node != nullptr);

    QSignalSpy unsuccessfulReadSpy(node.data(), &QOpcUaNode::attributeRead);

    node->readAttributes(QOpcUa::NodeAttribute::NodeId);
    unsuccessfulReadSpy.wait(signalSpyTimeout);
    QCOMPARE(unsuccessfulReadSpy.size(), 1);
    QVERIFY(unsuccessfulReadSpy.at(0).at(0).value<QOpcUa::NodeAttributes>() & QOpcUa::NodeAttribute::NodeId);
    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::NodeId), QOpcUa::UaStatusCode::BadNodeIdUnknown);
}

void Tst_QOpcUaClient::addAndRemoveDataTypeNode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QSignalSpy addNodeSpy(opcuaClient, &QOpcUaClient::addNodeFinished);

    QOpcUaExpandedNodeId parent;
    parent.setNodeId(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::BaseDataType));
    QOpcUaExpandedNodeId requestedNewId;
    requestedNewId.setNodeId(u"ns=3;s=DynamicDataTypeNode_%1"_s.arg(opcuaClient->backend()));
    const quint16 namespaceIndex = 3;
    QOpcUaQualifiedName browseName(namespaceIndex, u"DynamicDataTypeNode_%1"_s.arg(opcuaClient->backend()));

    QOpcUaNodeCreationAttributes attributes;

    QOpcUaLocalizedText displayName(u"en"_s, browseName.name());
    QOpcUaLocalizedText description(u"en"_s, u"Node added at runtime by %1"_s.arg(opcuaClient->backend()));

    attributes.setDisplayName(displayName);
    attributes.setDescription(description);
    attributes.setIsAbstract(true);

    QOpcUaAddNodeItem nodeInfo;
    nodeInfo.setParentNodeId(parent);
    nodeInfo.setReferenceTypeId(QOpcUa::nodeIdFromReferenceType(QOpcUa::ReferenceTypeId::HasSubtype));
    nodeInfo.setRequestedNewNodeId(requestedNewId);
    nodeInfo.setBrowseName(browseName);
    nodeInfo.setNodeClass(QOpcUa::NodeClass::DataType);
    nodeInfo.setNodeAttributes(attributes);

    opcuaClient->addNode(nodeInfo);

    addNodeSpy.wait(signalSpyTimeout);

    QCOMPARE(addNodeSpy.size(), 1);

    QCOMPARE(addNodeSpy.at(0).at(0).value<QOpcUaExpandedNodeId>().nodeId(), requestedNewId.nodeId());
    QCOMPARE(addNodeSpy.at(0).at(1), requestedNewId.nodeId());
    QCOMPARE(addNodeSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    QScopedPointer<QOpcUaNode> newNode(opcuaClient->node(requestedNewId.nodeId()));
    QVERIFY(newNode != nullptr);

    QSignalSpy readSpy(newNode.data(), &QOpcUaNode::attributeRead);
    newNode->readAttributes(QOpcUa::NodeAttribute::BrowseName | QOpcUa::NodeAttribute::DisplayName |
                            QOpcUa::NodeAttribute::Description | QOpcUa::NodeAttribute::IsAbstract);
    readSpy.wait(signalSpyTimeout);

    QCOMPARE(readSpy.size(), 1);

    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::BrowseName), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::DisplayName), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::Description), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::IsAbstract), QOpcUa::UaStatusCode::Good);

    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>(), browseName);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUaLocalizedText>(), displayName);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::Description).value<QOpcUaLocalizedText>(), description);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::IsAbstract), true);

    QSignalSpy removeNodeSpy(opcuaClient, &QOpcUaClient::deleteNodeFinished);
    opcuaClient->deleteNode(requestedNewId.nodeId(), true);
    removeNodeSpy.wait(signalSpyTimeout);

    QCOMPARE(removeNodeSpy.size(), 1);
    QCOMPARE(removeNodeSpy.at(0).at(0), requestedNewId.nodeId());
    QCOMPARE(removeNodeSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(requestedNewId.nodeId()));
    QVERIFY(node != nullptr);

    QSignalSpy unsuccessfulReadSpy(node.data(), &QOpcUaNode::attributeRead);

    node->readAttributes(QOpcUa::NodeAttribute::NodeId);
    unsuccessfulReadSpy.wait(signalSpyTimeout);
    QCOMPARE(unsuccessfulReadSpy.size(), 1);
    QVERIFY(unsuccessfulReadSpy.at(0).at(0).value<QOpcUa::NodeAttributes>() & QOpcUa::NodeAttribute::NodeId);
    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::NodeId), QOpcUa::UaStatusCode::BadNodeIdUnknown);
}

void Tst_QOpcUaClient::addAndRemoveViewNode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QSignalSpy addNodeSpy(opcuaClient, &QOpcUaClient::addNodeFinished);

    QOpcUaExpandedNodeId parent;
    parent.setNodeId(u"ns=3;s=TestFolder"_s);
    QOpcUaExpandedNodeId requestedNewId;
    requestedNewId.setNodeId(u"ns=3;s=DynamicViewNode_%1"_s.arg(opcuaClient->backend()));
    const quint16 namespaceIndex = 3;
    QOpcUaQualifiedName browseName(namespaceIndex, u"DynamicViewNode_%1"_s.arg(opcuaClient->backend()));

    QOpcUaNodeCreationAttributes attributes;

    QOpcUaLocalizedText displayName(u"en"_s, browseName.name());
    QOpcUaLocalizedText description(u"en"_s, u"Node added at runtime by %1"_s.arg(opcuaClient->backend()));

    attributes.setDisplayName(displayName);
    attributes.setDescription(description);
    attributes.setContainsNoLoops(true);
    attributes.setEventNotifier(QOpcUa::EventNotifierBit::SubscribeToEvents);

    QOpcUaAddNodeItem nodeInfo;
    nodeInfo.setParentNodeId(parent);
    nodeInfo.setReferenceTypeId(QOpcUa::nodeIdFromReferenceType(QOpcUa::ReferenceTypeId::Organizes));
    nodeInfo.setRequestedNewNodeId(requestedNewId);
    nodeInfo.setBrowseName(browseName);
    nodeInfo.setNodeClass(QOpcUa::NodeClass::View);
    nodeInfo.setNodeAttributes(attributes);

    opcuaClient->addNode(nodeInfo);

    addNodeSpy.wait(signalSpyTimeout);

    QCOMPARE(addNodeSpy.size(), 1);

    QCOMPARE(addNodeSpy.at(0).at(0).value<QOpcUaExpandedNodeId>().nodeId(), requestedNewId.nodeId());
    QCOMPARE(addNodeSpy.at(0).at(1), requestedNewId.nodeId());
    QCOMPARE(addNodeSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    QScopedPointer<QOpcUaNode> newNode(opcuaClient->node(requestedNewId.nodeId()));
    QVERIFY(newNode != nullptr);

    QSignalSpy readSpy(newNode.data(), &QOpcUaNode::attributeRead);
    newNode->readAttributes(QOpcUa::NodeAttribute::BrowseName | QOpcUa::NodeAttribute::DisplayName |
                            QOpcUa::NodeAttribute::Description | QOpcUa::NodeAttribute::ContainsNoLoops |
                            QOpcUa::NodeAttribute::EventNotifier);
    readSpy.wait(signalSpyTimeout);

    QCOMPARE(readSpy.size(), 1);

    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::BrowseName), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::DisplayName), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::Description), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::ContainsNoLoops), QOpcUa::UaStatusCode::Good);
    QCOMPARE(newNode->attributeError(QOpcUa::NodeAttribute::EventNotifier), QOpcUa::UaStatusCode::Good);

    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>(), browseName);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUaLocalizedText>(), displayName);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::Description).value<QOpcUaLocalizedText>(), description);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::ContainsNoLoops), true);
    QCOMPARE(newNode->attribute(QOpcUa::NodeAttribute::EventNotifier), true);

    QSignalSpy removeNodeSpy(opcuaClient, &QOpcUaClient::deleteNodeFinished);
    opcuaClient->deleteNode(requestedNewId.nodeId(), true);
    removeNodeSpy.wait(signalSpyTimeout);

    QCOMPARE(removeNodeSpy.size(), 1);
    QCOMPARE(removeNodeSpy.at(0).at(0), requestedNewId.nodeId());
    QCOMPARE(removeNodeSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(requestedNewId.nodeId()));
    QVERIFY(node != nullptr);

    QSignalSpy unsuccessfulReadSpy(node.data(), &QOpcUaNode::attributeRead);

    node->readAttributes(QOpcUa::NodeAttribute::NodeId);
    unsuccessfulReadSpy.wait(signalSpyTimeout);
    QCOMPARE(unsuccessfulReadSpy.size(), 1);
    QVERIFY(unsuccessfulReadSpy.at(0).at(0).value<QOpcUa::NodeAttributes>() & QOpcUa::NodeAttribute::NodeId);
    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::NodeId), QOpcUa::UaStatusCode::BadNodeIdUnknown);
}

void Tst_QOpcUaClient::addAndRemoveReference()
{

    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QSignalSpy addReferenceSpy(opcuaClient, &QOpcUaClient::addReferenceFinished);

    QOpcUaExpandedNodeId target;
    target.setNodeId(u"ns=3;s=TestFolder"_s);
    QString referenceType = QOpcUa::nodeIdFromInteger(0, static_cast<quint32>(QOpcUa::ReferenceTypeId::Organizes));

    QOpcUaAddReferenceItem refInfo;
    refInfo.setSourceNodeId(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::RootFolder));
    refInfo.setReferenceTypeId(referenceType);
    refInfo.setIsForwardReference(true);
    refInfo.setTargetNodeId(target);
    refInfo.setTargetNodeClass(QOpcUa::NodeClass::Variable);

    opcuaClient->addReference(refInfo);
    addReferenceSpy.wait(signalSpyTimeout);

    QCOMPARE(addReferenceSpy.size(), 1);
    QCOMPARE(addReferenceSpy.at(0).at(0), QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::RootFolder));
    QCOMPARE(addReferenceSpy.at(0).at(1), referenceType);
    QCOMPARE(addReferenceSpy.at(0).at(2).value<QOpcUaExpandedNodeId>().nodeId(), target.nodeId());
    QCOMPARE(addReferenceSpy.at(0).at(3).value<bool>(), true);
    QCOMPARE(addReferenceSpy.at(0).at(4).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    // Check if the reference has been added
    {
        QScopedPointer<QOpcUaNode> node(opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::RootFolder)));
        QVERIFY(node != nullptr);

        QSignalSpy browseSpy(node.data(), &QOpcUaNode::browseFinished);

        node->browseChildren(QOpcUa::ReferenceTypeId::Organizes);
        browseSpy.wait(signalSpyTimeout);
        QCOMPARE(browseSpy.size(), 1);
        QCOMPARE(browseSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
        const auto results = browseSpy.at(0).at(0).value<QList<QOpcUaReferenceDescription>>();
        QVERIFY(!results.isEmpty());
        bool referenceExists = false;
        for (const auto &item : results) {
            if (item.targetNodeId().nodeId() == target.nodeId()) {
                referenceExists = true;
                break;
            }
        }
        QVERIFY(referenceExists);
    }

    QSignalSpy deleteReferenceSpy(opcuaClient, &QOpcUaClient::deleteReferenceFinished);

    QOpcUaDeleteReferenceItem refDelInfo;
    refDelInfo.setSourceNodeId(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::RootFolder));
    refDelInfo.setReferenceTypeId(referenceType);
    refDelInfo.setIsForwardReference(true);
    refDelInfo.setTargetNodeId(target);
    refDelInfo.setDeleteBidirectional(true);

    opcuaClient->deleteReference(refDelInfo);
    deleteReferenceSpy.wait(signalSpyTimeout);

    QCOMPARE(deleteReferenceSpy.size(), 1);
    QCOMPARE(deleteReferenceSpy.at(0).at(0), QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::RootFolder));
    QCOMPARE(deleteReferenceSpy.at(0).at(1), referenceType);
    QCOMPARE(deleteReferenceSpy.at(0).at(2).value<QOpcUaExpandedNodeId>().nodeId(), target.nodeId());
    QCOMPARE(deleteReferenceSpy.at(0).at(3).value<bool>(), true);
    QCOMPARE(deleteReferenceSpy.at(0).at(4).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    // Check if the reference has been deleted
    {
        QScopedPointer<QOpcUaNode> node(opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::RootFolder)));
        QVERIFY(node != nullptr);

        QSignalSpy browseSpy(node.data(), &QOpcUaNode::browseFinished);

        node->browseChildren(QOpcUa::ReferenceTypeId::Organizes);
        browseSpy.wait(signalSpyTimeout);
        QCOMPARE(browseSpy.size(), 1);
        QCOMPARE(browseSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
        const auto results = browseSpy.at(0).at(0).value<QList<QOpcUaReferenceDescription>>();
        QVERIFY(!results.isEmpty());
        for (const auto &item : results)
            QVERIFY(item.targetNodeId().nodeId() != target.nodeId());
    }

    // Check error paths
    addReferenceSpy.clear();
    refInfo.setTargetNodeId(QOpcUaExpandedNodeId(u"ns=42;i=1234"_s));
    opcuaClient->addReference(refInfo);
    addReferenceSpy.wait(signalSpyTimeout);
    QCOMPARE(addReferenceSpy.size(), 1);
    QCOMPARE(addReferenceSpy.at(0).at(4), QOpcUa::UaStatusCode::BadTargetNodeIdInvalid);

    deleteReferenceSpy.clear();
    refDelInfo.setTargetNodeId(QOpcUaExpandedNodeId(u"ns=42;i=1234"_s));
    opcuaClient->deleteReference(refDelInfo);
    deleteReferenceSpy.wait(signalSpyTimeout);
    QCOMPARE(deleteReferenceSpy.size(), 1);
    QCOMPARE(addReferenceSpy.at(0).at(4), QOpcUa::UaStatusCode::BadTargetNodeIdInvalid);
}

void Tst_QOpcUaClient::dataChangeSubscription()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(readWriteNode));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, QVariant(double(0)), QOpcUa::Types::Double);
    READ_MANDATORY_VARIABLE_NODE(node);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::Value).toDouble(), 0.0);

    QSignalSpy dataChangeSpy(node.data(), &QOpcUaNode::dataChangeOccurred);
    QSignalSpy monitoringEnabledSpy(node.data(), &QOpcUaNode::enableMonitoringFinished);

    node->enableMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters(100, QOpcUaMonitoringParameters::SubscriptionType::Exclusive));
    monitoringEnabledSpy.wait(signalSpyTimeout);

    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(node->monitoringStatus(QOpcUa::NodeAttribute::Value).statusCode(), QOpcUa::UaStatusCode::Good);

    QOpcUaMonitoringParameters valueStatus = node->monitoringStatus(QOpcUa::NodeAttribute::Value);
    QVERIFY(valueStatus.subscriptionId() != 0);
    QVERIFY(valueStatus.monitoredItemId() != 0);
    QCOMPARE(valueStatus.statusCode(), QOpcUa::UaStatusCode::Good);

    WRITE_VALUE_ATTRIBUTE(node, QVariant(double(42)), QOpcUa::Types::Double);
    dataChangeSpy.wait(signalSpyTimeout);
    if (dataChangeSpy.empty() || !qFuzzyCompare(dataChangeSpy.at(0).at(1).toDouble(), 42.0))
        dataChangeSpy.wait(signalSpyTimeout);

    QVERIFY(dataChangeSpy.size() >= 1);

    int index = dataChangeSpy.size() == 1 ? 0 : 1;
    QCOMPARE(dataChangeSpy.at(index).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(dataChangeSpy.at(index).at(1), double(42));

    monitoringEnabledSpy.clear();
    dataChangeSpy.clear();

    node->enableMonitoring(QOpcUa::NodeAttribute::DisplayName, QOpcUaMonitoringParameters(100,QOpcUaMonitoringParameters::SubscriptionType::Exclusive,
                                                                                              valueStatus.subscriptionId()));
    monitoringEnabledSpy.wait(signalSpyTimeout);

    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::DisplayName);
    QCOMPARE(node->monitoringStatus(QOpcUa::NodeAttribute::DisplayName).statusCode(), QOpcUa::UaStatusCode::Good);

    QOpcUaMonitoringParameters displayNameStatus = node->monitoringStatus(QOpcUa::NodeAttribute::DisplayName);
    QCOMPARE(displayNameStatus.subscriptionId(), valueStatus.subscriptionId());
    QVERIFY(displayNameStatus.monitoredItemId() != 0);
    QCOMPARE(displayNameStatus.statusCode(), QOpcUa::UaStatusCode::Good);

    dataChangeSpy.wait(signalSpyTimeout);
    QCOMPARE(dataChangeSpy.size(), 1);
    QCOMPARE(dataChangeSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::DisplayName);
    QCOMPARE(dataChangeSpy.at(0).at(1).value<QOpcUaLocalizedText>().text(), u"TestNode.ReadWrite"_s);

    monitoringEnabledSpy.clear();
    dataChangeSpy.clear();
    node->enableMonitoring(QOpcUa::NodeAttribute::NodeId, QOpcUaMonitoringParameters(100));
    monitoringEnabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::NodeId);
    QVERIFY(node->monitoringStatus(QOpcUa::NodeAttribute::NodeId).subscriptionId() != valueStatus.subscriptionId());
    QCOMPARE(node->monitoringStatus(QOpcUa::NodeAttribute::NodeId).statusCode(), QOpcUa::UaStatusCode::Good);

    QOpcUaMonitoringParameters nodeIdStatus = node->monitoringStatus(QOpcUa::NodeAttribute::NodeId);
    QVERIFY(nodeIdStatus.subscriptionId() != valueStatus.subscriptionId());
    QVERIFY(nodeIdStatus.monitoredItemId() != 0);
    QCOMPARE(nodeIdStatus.statusCode(), QOpcUa::UaStatusCode::Good);

    dataChangeSpy.wait(signalSpyTimeout);
    QCOMPARE(dataChangeSpy.size(), 1);
    QCOMPARE(dataChangeSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::NodeId);
    QCOMPARE(dataChangeSpy.at(0).at(1), u"ns=3;s=TestNode.ReadWrite"_s);

    QList<QOpcUa::NodeAttribute> attrs;

    QSignalSpy monitoringModifiedSpy(node.data(), &QOpcUaNode::monitoringStatusChanged);
    node->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::PublishingInterval, 200);
    node->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::LifetimeCount, valueStatus.lifetimeCount() + 1);
    node->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::MaxKeepAliveCount, valueStatus.maxKeepAliveCount() + 1);
    node->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::Priority, valueStatus.priority() + 1);
    node->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::MaxNotificationsPerPublish,
                           valueStatus.maxNotificationsPerPublish());

    for (int i = 0; i < 10; ++i) {
        monitoringModifiedSpy.wait(signalSpyTimeout);

        if (monitoringModifiedSpy.size() == 10)
            break;
    }

    QCOMPARE(monitoringModifiedSpy.size(), 10);

    const QOpcUaMonitoringParameters::Parameters params =
        QOpcUaMonitoringParameters::Parameter::PublishingInterval |
        QOpcUaMonitoringParameters::Parameter::LifetimeCount |
        QOpcUaMonitoringParameters::Parameter::MaxKeepAliveCount |
        QOpcUaMonitoringParameters::Parameter::Priority |
        QOpcUaMonitoringParameters::Parameter::MaxNotificationsPerPublish;

    QHash<QOpcUa::NodeAttribute, QOpcUaMonitoringParameters::Parameters> expectedEntries = {
        { QOpcUa::NodeAttribute::Value, params },
        { QOpcUa::NodeAttribute::DisplayName, params }
    };

    for (auto &it : std::as_const(monitoringModifiedSpy)) {
        QOpcUa::NodeAttribute temp = it.at(0).value<QOpcUa::NodeAttribute>();
        QVERIFY(expectedEntries.contains(temp));
        const auto includedFlags = it.at(1).value<QOpcUaMonitoringParameters::Parameters>();
        QCOMPARE(includedFlags.testAnyFlags(expectedEntries.value(temp)), true);
        QCOMPARE(it.at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
        expectedEntries[temp].setFlag(QOpcUaMonitoringParameters::Parameter(includedFlags.toInt()), false);
    }

    QCOMPARE(node->monitoringStatus(QOpcUa::NodeAttribute::Value).publishingInterval(),  200.0);
    QCOMPARE(node->monitoringStatus(QOpcUa::NodeAttribute::DisplayName).publishingInterval(), 200.0);
    QCOMPARE(expectedEntries.value(QOpcUa::NodeAttribute::Value).toInt(), 0);
    QCOMPARE(expectedEntries.value(QOpcUa::NodeAttribute::DisplayName).toInt(), 0);

    QSignalSpy monitoringDisabledSpy(node.data(), &QOpcUaNode::disableMonitoringFinished);

    node->disableMonitoring(QOpcUa::NodeAttribute::Value | QOpcUa::NodeAttribute::DisplayName | QOpcUa::NodeAttribute::NodeId);
    monitoringDisabledSpy.wait(signalSpyTimeout);
    if (monitoringDisabledSpy.size() < 2)
        monitoringDisabledSpy.wait(signalSpyTimeout);
    if (monitoringDisabledSpy.size() < 3)
        monitoringDisabledSpy.wait(signalSpyTimeout);

    QCOMPARE(monitoringDisabledSpy.size(), 3);

    attrs = {QOpcUa::NodeAttribute::Value, QOpcUa::NodeAttribute::DisplayName, QOpcUa::NodeAttribute::NodeId};
    for (const auto &it : std::as_const(monitoringDisabledSpy)) {
        QOpcUa::NodeAttribute temp = it.at(0).value<QOpcUa::NodeAttribute>();
        QVERIFY(attrs.contains(temp));
        QCOMPARE(node->monitoringStatus(temp).subscriptionId(), (quint32)0);
        QCOMPARE(node->monitoringStatus(temp).monitoredItemId(), (quint32)0);
        QCOMPARE(node->monitoringStatus(temp).statusCode(), QOpcUa::UaStatusCode::BadNoEntryExists);
        attrs.remove(attrs.indexOf(temp));
    }
    QCOMPARE(attrs.size(), 0);

    // Enable the monitoring again, after it has been disabled. This triggered a bug where a previous subscription
    // was not completeley removed from the internal data structures
    monitoringEnabledSpy.clear();
    node->enableMonitoring(QOpcUa::NodeAttribute::NodeId, QOpcUaMonitoringParameters(100));
    monitoringEnabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::NodeId);
    QVERIFY(node->monitoringStatus(QOpcUa::NodeAttribute::NodeId).subscriptionId() != valueStatus.subscriptionId());
    QCOMPARE(node->monitoringStatus(QOpcUa::NodeAttribute::NodeId).statusCode(), QOpcUa::UaStatusCode::Good);
}

void Tst_QOpcUaClient::dataChangeSubscriptionInvalidNode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> noDataNode(opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::RootFolder)));
    QVERIFY(noDataNode != nullptr);
    QSignalSpy monitoringEnabledSpy(noDataNode.data(), &QOpcUaNode::enableMonitoringFinished);

    QOpcUaMonitoringParameters settings;
    settings.setPublishingInterval(100);
    noDataNode->enableMonitoring(QOpcUa::NodeAttribute::Value, settings);
    monitoringEnabledSpy.wait(signalSpyTimeout);

    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(noDataNode->monitoringStatus(QOpcUa::NodeAttribute::Value).statusCode(), QOpcUa::UaStatusCode::BadAttributeIdInvalid);
    QCOMPARE(noDataNode->monitoringStatus(QOpcUa::NodeAttribute::Value).subscriptionId(), (quint32)0);
}

void Tst_QOpcUaClient::dataChangeSubscriptionSharing()
{
    // The open62541 test server has a minimum publishing interval of 100ms.
    // This test verifies that monitorings with smaller requested publishing interval and shared flag
    // share the same subscription.

    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(readWriteNode));
    QVERIFY(node != nullptr);
    QSignalSpy monitoringEnabledSpy(node.data(), &QOpcUaNode::enableMonitoringFinished);

    node->enableMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters(50));
    monitoringEnabledSpy.wait(signalSpyTimeout);

    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(node->monitoringStatus(QOpcUa::NodeAttribute::Value).statusCode(), QOpcUa::UaStatusCode::Good);

    QOpcUaMonitoringParameters valueStatus = node->monitoringStatus(QOpcUa::NodeAttribute::Value);
    QVERIFY(valueStatus.subscriptionId() != 0);
    QCOMPARE(valueStatus.statusCode(), QOpcUa::UaStatusCode::Good);

    monitoringEnabledSpy.clear();

    node->enableMonitoring(QOpcUa::NodeAttribute::DisplayName, QOpcUaMonitoringParameters(25));
    monitoringEnabledSpy.wait(signalSpyTimeout);

    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::DisplayName);
    QCOMPARE(node->monitoringStatus(QOpcUa::NodeAttribute::DisplayName).statusCode(), QOpcUa::UaStatusCode::Good);

    QOpcUaMonitoringParameters displayNameStatus = node->monitoringStatus(QOpcUa::NodeAttribute::DisplayName);
    QCOMPARE(displayNameStatus.subscriptionId(), valueStatus.subscriptionId());
    QCOMPARE(displayNameStatus.statusCode(), QOpcUa::UaStatusCode::Good);

    QCOMPARE(valueStatus.subscriptionId(), displayNameStatus.subscriptionId());
    QCOMPARE(valueStatus.publishingInterval(), displayNameStatus.publishingInterval());
    QCOMPARE(valueStatus.publishingInterval(), 100.0);

    QSignalSpy monitoringDisabledSpy(node.data(), &QOpcUaNode::disableMonitoringFinished);

    node->disableMonitoring(QOpcUa::NodeAttribute::Value | QOpcUa::NodeAttribute::DisplayName);
    monitoringDisabledSpy.wait(signalSpyTimeout);
    if (monitoringDisabledSpy.size() < 2)
        monitoringDisabledSpy.wait(signalSpyTimeout);

    QCOMPARE(monitoringDisabledSpy.size(), 2);

    QList<QOpcUa::NodeAttribute> attrs = {QOpcUa::NodeAttribute::Value, QOpcUa::NodeAttribute::DisplayName};
    for (const auto &it : std::as_const(monitoringDisabledSpy)) {
        auto temp = it.at(0).value<QOpcUa::NodeAttribute>();
        QVERIFY(attrs.contains(temp));
        QCOMPARE(node->monitoringStatus(temp).subscriptionId(), (quint32)0);
        QCOMPARE(node->monitoringStatus(temp).statusCode(), QOpcUa::UaStatusCode::BadNoEntryExists);
        attrs.removeOne(temp);
    }
    QCOMPARE(attrs.size(), 0);
}

void Tst_QOpcUaClient::dataChangeSubscriptionTriggering()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    // Setup triggered node and write node
    QScopedPointer<QOpcUaNode> writeNode(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Int32"_s));
    QVERIFY(writeNode != nullptr);
    QScopedPointer<QOpcUaNode> triggeredNode(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Int32"_s));
    QVERIFY(triggeredNode != nullptr);

    WRITE_VALUE_ATTRIBUTE(writeNode, 1, QOpcUa::Types::Int32);

    QOpcUaMonitoringParameters triggeredParams(50);
    triggeredParams.setMonitoringMode(QOpcUaMonitoringParameters::MonitoringMode::Sampling);
    triggeredParams.setQueueSize(10);

    triggeredNode->enableMonitoring(QOpcUa::NodeAttribute::Value, triggeredParams);
    QSignalSpy monitoringEnabledSpy2(triggeredNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QSignalSpy valueChangedSpy(triggeredNode.data(), &QOpcUaNode::attributeUpdated);
    monitoringEnabledSpy2.wait(signalSpyTimeout);

    QCOMPARE(monitoringEnabledSpy2.size(), 1);
    QCOMPARE(monitoringEnabledSpy2.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    const auto triggeredStatus = triggeredNode->monitoringStatus(QOpcUa::NodeAttribute::Value);
    QCOMPARE(triggeredStatus.statusCode(), QOpcUa::UaStatusCode::Good);

    // Setup trigger
    QScopedPointer<QOpcUaNode> triggeringNode(opcuaClient->node(readWriteNode));
    QVERIFY(triggeringNode != nullptr);
    QSignalSpy monitoringEnabledSpy(triggeringNode.data(), &QOpcUaNode::enableMonitoringFinished);

    QOpcUaMonitoringParameters triggerParameters(50);
    triggerParameters.setSubscriptionId(triggeredStatus.subscriptionId());
    triggerParameters.setTriggeredItemIds({ triggeredStatus.monitoredItemId(), 10, 11 });
    triggeringNode->enableMonitoring(QOpcUa::NodeAttribute::Value, triggerParameters);
    monitoringEnabledSpy.wait(signalSpyTimeout);

    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(triggeringNode->monitoringStatus(QOpcUa::NodeAttribute::Value).statusCode(), QOpcUa::UaStatusCode::Good);

    const auto triggerStatus = triggeringNode->monitoringStatus(QOpcUa::NodeAttribute::Value);
    QVERIFY(triggerStatus.subscriptionId() != 0);
    QCOMPARE(triggerStatus.statusCode(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(triggerStatus.subscriptionId(), triggeredStatus.subscriptionId());
    QCOMPARE(triggerStatus.triggeredItemIds(), QSet<quint32>{ triggeredStatus.monitoredItemId() });
    QCOMPARE(triggerStatus.failedTriggeredItemsStatus(),
             (QHash<quint32, QOpcUa::UaStatusCode>{
                 { 10, QOpcUa::UaStatusCode::BadMonitoredItemIdInvalid },
                 { 11, QOpcUa::UaStatusCode::BadMonitoredItemIdInvalid }
             }));

    // Create value change, trigger and check results

    WRITE_VALUE_ATTRIBUTE(writeNode, 2, QOpcUa::Types::Int32);

    valueChangedSpy.wait(1000);
    QCOMPARE(valueChangedSpy.size(), 0);

    WRITE_VALUE_ATTRIBUTE(triggeringNode, 1234.0, QOpcUa::Types::Double);

    valueChangedSpy.wait(signalSpyTimeout);
    QCOMPARE(valueChangedSpy.size(), 1);
    QCOMPARE(valueChangedSpy.at(0).at(1).value<quint32>(), 2U);
}

void Tst_QOpcUaClient::dataChangeSubscriptionModifyTriggering()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    // Setup triggered and write nodes
    QScopedPointer<QOpcUaNode> writeNode(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Int32"_s));
    QVERIFY(writeNode != nullptr);
    QScopedPointer<QOpcUaNode> triggeredNode(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Int32"_s));
    QVERIFY(triggeredNode != nullptr);

    WRITE_VALUE_ATTRIBUTE(writeNode, 1, QOpcUa::Types::Int32);

    QOpcUaMonitoringParameters triggeredParams(50);
    triggeredParams.setMonitoringMode(QOpcUaMonitoringParameters::MonitoringMode::Sampling);
    triggeredParams.setQueueSize(10);

    triggeredNode->enableMonitoring(QOpcUa::NodeAttribute::Value, triggeredParams);
    QSignalSpy monitoringEnabledSpy2(triggeredNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QSignalSpy valueChangedSpy(triggeredNode.data(), &QOpcUaNode::attributeUpdated);
    monitoringEnabledSpy2.wait();

    QCOMPARE(monitoringEnabledSpy2.size(), 1);
    QCOMPARE(monitoringEnabledSpy2.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    const auto triggeredStatus = triggeredNode->monitoringStatus(QOpcUa::NodeAttribute::Value);
    QCOMPARE(triggeredStatus.statusCode(), QOpcUa::UaStatusCode::Good);

    // Setup triggering node
    QScopedPointer<QOpcUaNode> triggeringNode(opcuaClient->node(readWriteNode));
    QVERIFY(triggeringNode != nullptr);
    QSignalSpy monitoringEnabledSpy(triggeringNode.data(), &QOpcUaNode::enableMonitoringFinished);

    QOpcUaMonitoringParameters triggerSettings(50);
    triggerSettings.setSubscriptionId(triggeredStatus.subscriptionId());
    triggeringNode->enableMonitoring(QOpcUa::NodeAttribute::Value, triggerSettings);
    monitoringEnabledSpy.wait(signalSpyTimeout);

    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    auto triggerStatus = triggeringNode->monitoringStatus(QOpcUa::NodeAttribute::Value);
    QCOMPARE(triggerStatus.statusCode(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(triggerStatus.subscriptionId(), triggeredStatus.subscriptionId());
    QCOMPARE(triggerStatus.statusCode(), QOpcUa::UaStatusCode::Good);

    // Write, nothing should happen
    WRITE_VALUE_ATTRIBUTE(writeNode, 2, QOpcUa::Types::Int32);

    valueChangedSpy.wait(1000);
    QCOMPARE(valueChangedSpy.size(), 0);

    WRITE_VALUE_ATTRIBUTE(triggeringNode, 1235.0, QOpcUa::Types::Double);

    valueChangedSpy.wait(1000);
    QCOMPARE(valueChangedSpy.size(), 0);

    // Modify triggering node to actually trigger
    QSignalSpy modifySpy(triggeringNode.get(), &QOpcUaNode::monitoringStatusChanged);
    triggeringNode->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::TriggeredItemIds,
                                     QVariant::fromValue(QSet<quint32>{triggeredStatus.monitoredItemId(), 10, 11}));
    modifySpy.wait(signalSpyTimeout);
    QCOMPARE(modifySpy.size(), 1);
    QCOMPARE(modifySpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(triggeringNode->monitoringStatus(QOpcUa::NodeAttribute::Value).triggeredItemIds(),
             QSet<quint32>{triggeredStatus.monitoredItemId()});
    QCOMPARE(triggeringNode->monitoringStatus(QOpcUa::NodeAttribute::Value).failedTriggeredItemsStatus(),
             (QHash<quint32, QOpcUa::UaStatusCode>{
                 { 10, QOpcUa::UaStatusCode::BadMonitoredItemIdInvalid },
                 { 11, QOpcUa::UaStatusCode::BadMonitoredItemIdInvalid }
             }));

    WRITE_VALUE_ATTRIBUTE(writeNode, 3, QOpcUa::Types::Int32);

    valueChangedSpy.wait(1000);
    QCOMPARE(valueChangedSpy.size(), 0);

    WRITE_VALUE_ATTRIBUTE(triggeringNode, 1236.0, QOpcUa::Types::Double);

    valueChangedSpy.wait(signalSpyTimeout);
    QCOMPARE(valueChangedSpy.size(), 1);
    QCOMPARE(valueChangedSpy.at(0).at(1).value<quint32>(), 3U);

    modifySpy.clear();
    triggeringNode->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::TriggeredItemIds,
                                     QVariant::fromValue(QSet<quint32>()));
    modifySpy.wait();
    QCOMPARE(modifySpy.size(), 1);
    QCOMPARE(modifySpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(triggeredNode->monitoringStatus(QOpcUa::NodeAttribute::Value).triggeredItemIds(), QSet<quint32>());

    // There seems to be a slight delay when removing the link with open62541 1.4
    for (int i = 0; i < 10; ++i) {
        valueChangedSpy.clear();
        WRITE_VALUE_ATTRIBUTE(writeNode, 4 + i, QOpcUa::Types::Int32);

        valueChangedSpy.wait(1000);
        if (valueChangedSpy.empty())
            break;

        QTest::qWait(10);
    }

    QCOMPARE(valueChangedSpy.size(), 0);

    WRITE_VALUE_ATTRIBUTE(triggeringNode, 1237.0, QOpcUa::Types::Double);

    valueChangedSpy.wait(1000);
    QCOMPARE(valueChangedSpy.size(), 0);
}

void Tst_QOpcUaClient::methodCall()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QList<QOpcUa::TypedVariant> args;
    for (int i = 0; i < 2; i++)
        args.push_back(QOpcUa::TypedVariant(double(4), QOpcUa::Double));

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=3;s=TestFolder"_s));
    QVERIFY(node != nullptr);

    QSignalSpy methodSpy(node.data(), &QOpcUaNode::methodCallFinished);

    bool success = node->callMethod(u"ns=3;s=Test.Method.Multiply"_s, args);
    QVERIFY(success == true);

    methodSpy.wait(signalSpyTimeout);

    QCOMPARE(methodSpy.size(), 1);
    QCOMPARE(methodSpy.at(0).at(0).value<QString>(), u"ns=3;s=Test.Method.Multiply"_s);
    QCOMPARE(methodSpy.at(0).at(1).value<double>(), 16.0);
    QCOMPARE(QOpcUa::isSuccessStatus(methodSpy.at(0).at(2).value<QOpcUa::UaStatusCode>()), true);
}

void Tst_QOpcUaClient::methodCallInvalid()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QList<QOpcUa::TypedVariant> args;
    for (int i = 0; i < 3; i++)
        args.push_back(QOpcUa::TypedVariant(double(4), QOpcUa::Double));

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=3;s=TestFolder"_s));
    QVERIFY(node != nullptr);

    QSignalSpy methodSpy(node.data(), &QOpcUaNode::methodCallFinished);

    bool success = node->callMethod(u"ns=3;s=Test.Method.Divide"_s, args); // Does not exist
    QVERIFY(success == true);
    methodSpy.wait(signalSpyTimeout);
    QCOMPARE(methodSpy.size(), 1);
    QCOMPARE(QOpcUa::errorCategory(methodSpy.at(0).at(2).value<QOpcUa::UaStatusCode>()), QOpcUa::ErrorCategory::NodeError);

    methodSpy.clear();
    success = node->callMethod(u"ns=3;s=Test.Method.Multiply"_s, args); // One excess argument
    QVERIFY(success == true);
    methodSpy.wait(signalSpyTimeout);
    QCOMPARE(methodSpy.size(), 1);
    QCOMPARE(methodSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadTooManyArguments);

    methodSpy.clear();
    args.resize(1);
    success = node->callMethod(u"ns=3;s=Test.Method.Multiply"_s, args); // One argument missing
    QVERIFY(success == true);
    methodSpy.wait(signalSpyTimeout);
    QCOMPARE(methodSpy.size(), 1);
    QCOMPARE(methodSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadArgumentsMissing);
}

void Tst_QOpcUaClient::readMethodArguments()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(
                opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Server_GetMonitoredItems_InputArguments)));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);

    QOpcUaArgument argument = node->attribute(QOpcUa::NodeAttribute::Value).value<QOpcUaArgument>();
    QCOMPARE(argument.name(), u"SubscriptionId"_s);
    QCOMPARE(argument.dataTypeId(), QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::UInt32));
    QCOMPARE(argument.valueRank(), -1);
    QVERIFY(argument.arrayDimensions().isEmpty());
    QCOMPARE(argument.description(), QOpcUaLocalizedText());

    node.reset(opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Server_GetMonitoredItems_OutputArguments)));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);

    QVariantList list = node->attribute(QOpcUa::NodeAttribute::Value).toList();
    QCOMPARE(list.size(), 2);

    argument = list.at(0).value<QOpcUaArgument>();
    QCOMPARE(argument.name(), u"ServerHandles"_s);
    QCOMPARE(argument.dataTypeId(), QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::UInt32));
    QCOMPARE(argument.valueRank(), 1);
    QCOMPARE(argument.arrayDimensions().size(), 1);
    QCOMPARE(argument.description(), QOpcUaLocalizedText());

    argument = list.at(1).value<QOpcUaArgument>();
    QCOMPARE(argument.name(), u"ClientHandles"_s);
    QCOMPARE(argument.dataTypeId(), QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::UInt32));
    QCOMPARE(argument.valueRank(), 1);
    QCOMPARE(argument.arrayDimensions().size(), 1);
    QCOMPARE(argument.description(), QOpcUaLocalizedText());
}

void Tst_QOpcUaClient::malformedNodeString()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> invalidNode(opcuaClient->node(u"justsomerandomstring"_s));
    QVERIFY(invalidNode == nullptr);

    invalidNode.reset(opcuaClient->node(u"ns=a;i=b"_s));
    QVERIFY(invalidNode == nullptr);

    invalidNode.reset(opcuaClient->node(u"ns=;i="_s));
    QVERIFY(invalidNode == nullptr);

    invalidNode.reset(opcuaClient->node(u"ns=0;x=123"_s));
    QVERIFY(invalidNode == nullptr);

    invalidNode.reset(opcuaClient->node(u"ns=0,i=31;"_s));
    QVERIFY(invalidNode == nullptr);

    invalidNode.reset(opcuaClient->node(u"ns:0;i:31;"_s));
    QVERIFY(invalidNode == nullptr);
}

void Tst_QOpcUaClient::nodeIdGeneration()
{
    QString nodeId = QOpcUa::nodeIdFromString(1, u"TestString"_s);
    QCOMPARE(nodeId, u"ns=1;s=TestString"_s);
    nodeId = QOpcUa::nodeIdFromInteger(1, 10);
    QCOMPARE(nodeId, u"ns=1;i=10"_s);
    nodeId = QOpcUa::nodeIdFromGuid(1, QUuid("08081e75-8e5e-319b-954f-f3a7613dc29b"));
    QCOMPARE(nodeId, u"ns=1;g=08081e75-8e5e-319b-954f-f3a7613dc29b"_s);
    nodeId = QOpcUa::nodeIdFromByteString(1, QByteArray::fromBase64("UXQgZnR3IQ=="));
    QCOMPARE(nodeId, u"ns=1;b=UXQgZnR3IQ=="_s);
    nodeId = QOpcUa::nodeIdFromReferenceType(QOpcUa::ReferenceTypeId::HasComponent);
    QCOMPARE(nodeId, QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::HasComponent));
}

void Tst_QOpcUaClient::multipleClients()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector1(opcuaClient, m_endpoint);
    QScopedPointer<QOpcUaNode> node(opcuaClient->node(readWriteNode));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, 42.0, QOpcUa::Types::Double);
    READ_MANDATORY_VARIABLE_NODE(node);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::Value).toDouble(), 42.0);

    QScopedPointer<QOpcUaClient> b(m_opcUa.createClient(opcuaClient->backend()));
    OpcuaConnector connector2(b.get(), m_endpoint);
    node.reset(b->node(readWriteNode));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::Value).toDouble(), 42.0);

    QScopedPointer<QOpcUaClient> d(m_opcUa.createClient(opcuaClient->backend()));
    OpcuaConnector connector3(d.get(), m_endpoint);
    node.reset(d->node(readWriteNode));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::Value).toDouble(), 42.0);
}

void Tst_QOpcUaClient::nodeClass()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    // Root -> Types -> ReferenceTypes -> References
    QScopedPointer<QOpcUaNode> node(opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::References)));
    QVERIFY(node != nullptr);
    READ_MANDATORY_BASE_NODE(node)
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>(), QOpcUa::NodeClass::ReferenceType);

    // Root -> Types -> DataTypes -> BaseDataTypes -> Boolean
    node.reset(opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Boolean)));
    QVERIFY(node != nullptr);
    READ_MANDATORY_BASE_NODE(node)
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>(), QOpcUa::NodeClass::DataType);

    // Root -> Types -> DataTypes -> ObjectTypes -> BaseObjectTypes -> FolderType
    node.reset(opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::FolderType)));
    QVERIFY(node != nullptr);
    READ_MANDATORY_BASE_NODE(node)
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>(), QOpcUa::NodeClass::ObjectType);

    // Root -> Types -> DataTypes -> VariableTypes -> BaseVariableType -> PropertyType
    node.reset(opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::PropertyType)));
    QVERIFY(node != nullptr);
    READ_MANDATORY_BASE_NODE(node)
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>(), QOpcUa::NodeClass::VariableType);

    // Root -> Objects
    node.reset(opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::ObjectsFolder)));
    QVERIFY(node != nullptr);
    READ_MANDATORY_BASE_NODE(node)
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>(), QOpcUa::NodeClass::Object);

    // Root -> Objects -> Server -> NamespaceArray
    node.reset(opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Server_NamespaceArray)));
    QVERIFY(node != nullptr);
    READ_MANDATORY_BASE_NODE(node)
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>(), QOpcUa::NodeClass::Variable);
}

void Tst_QOpcUaClient::writeArray()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QVariantList list;

    list.append(true);
    list.append(false);
    list.append(true);
    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Boolean"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Boolean);

    list.clear();
    list.append(std::numeric_limits<quint8>::min());
    list.append((std::numeric_limits<quint8>::max)());
    list.append(10);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Byte"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Byte);

    list.clear();
    list.append(QDateTime::currentDateTime());
    list.append(QDateTime::currentDateTime());
    list.append(QDateTime::currentDateTime());
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.DateTime"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::DateTime);

    list.clear();
    list.append(23.5);
    list.append(23.6);
    list.append(23.7);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Double"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Double);

    list.clear();
    list.append(23.5);
    list.append(23.6);
    list.append(23.7);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Float"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Float);

    list.clear();
    list.append(std::numeric_limits<qint16>::min());
    list.append((std::numeric_limits<qint16>::max)());
    list.append(10);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Int16"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Int16);

    list.clear();
    list.append(std::numeric_limits<qint32>::min());
    list.append((std::numeric_limits<qint32>::max)());
    list.append(10);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Int32"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Int32);

    list.clear();
    list.append(std::numeric_limits<qint64>::min());
    list.append((std::numeric_limits<qint64>::max)());
    list.append(10);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Int64"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Int64);

    list.clear();
    list.append(std::numeric_limits<qint8>::min());
    list.append((std::numeric_limits<qint8>::max)());
    list.append(10);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.SByte"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::SByte);

    list.clear();
    list.append(u"Test1"_s);
    list.append(u"Test2"_s);
    list.append(u"Test3"_s);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.String"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::String);

    list.clear();
    list.append(std::numeric_limits<quint16>::min());
    list.append((std::numeric_limits<quint16>::max)());
    list.append(10);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.UInt16"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::UInt16);

    list.clear();
    list.append(std::numeric_limits<quint32>::min());
    list.append((std::numeric_limits<quint32>::max)());
    list.append(10);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.UInt32"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::UInt32);

    list.clear();
    list.append(std::numeric_limits<quint64>::min());
    list.append((std::numeric_limits<quint64>::max)());
    list.append(10);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.UInt64"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::UInt64);

    list.clear();
    list.append(localizedTexts[0]);
    list.append(localizedTexts[1]);
    list.append(localizedTexts[2]);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.LocalizedText"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::LocalizedText);

    list.clear();
    list.append(u"abc"_s);
    list.append(u"def"_s);
    QByteArray withNull("gh");
    withNull.append('\0');
    withNull.append("i");
    list.append(withNull);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.ByteString"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::ByteString);

    list.clear();
    list.append(testUuid[0]);
    list.append(testUuid[1]);
    list.append(testUuid[2]);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Guid"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Guid);

    list.clear();
    list.append(u"ns=0;i=0"_s);
    list.append(u"ns=0;i=1"_s);
    list.append(u"ns=0;i=2"_s);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.NodeId"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::NodeId);

    list.clear();
    list.append(QOpcUaQualifiedName(0, u"Test0"_s));
    list.append(QOpcUaQualifiedName(1, u"Test1"_s));
    list.append(QOpcUaQualifiedName(2, u"Test2"_s));
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.QualifiedName"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::QualifiedName);

    list.clear();
    list.append(QOpcUa::UaStatusCode::Good);
    list.append(QOpcUa::UaStatusCode::BadUnexpectedError);
    list.append(QOpcUa::UaStatusCode::BadInternalError);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.StatusCode"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Types::StatusCode);

    list.clear();
    list.append(QVariant::fromValue(testRanges[0]));
    list.append(QVariant::fromValue(testRanges[1]));
    list.append(QVariant::fromValue(testRanges[2]));
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Range"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Range);

    list.clear();
    list.append(QVariant::fromValue(testEUInfos[0]));
    list.append(QVariant::fromValue(testEUInfos[1]));
    list.append(QVariant::fromValue(testEUInfos[2]));
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.EUInformation"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::EUInformation);

    list.clear();
    list.append(QVariant::fromValue(testComplex[0]));
    list.append(QVariant::fromValue(testComplex[1]));
    list.append(QVariant::fromValue(testComplex[2]));
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.ComplexNumber"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::ComplexNumber);

    list.clear();
    list.append(QVariant::fromValue(testDoubleComplex[0]));
    list.append(QVariant::fromValue(testDoubleComplex[1]));
    list.append(QVariant::fromValue(testDoubleComplex[2]));
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.DoubleComplexNumber"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::DoubleComplexNumber);

    list.clear();
    list.append(QVariant::fromValue(testAxisInfo[0]));
    list.append(QVariant::fromValue(testAxisInfo[1]));
    list.append(QVariant::fromValue(testAxisInfo[2]));
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.AxisInformation"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::AxisInformation);

    list.clear();
    list.append(QVariant::fromValue(testXV[0]));
    list.append(QVariant::fromValue(testXV[1]));
    list.append(QVariant::fromValue(testXV[2]));
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.XV"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::XV);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.ExtensionObject"_s));
    QVERIFY(node != nullptr);

    QVariantList value;

    for (int i = 0; i < testRanges.size(); ++i) {
        QOpcUaExtensionObject obj;
        ENCODE_EXTENSION_OBJECT(obj, i);
        value.append(obj);
    }

    WRITE_VALUE_ATTRIBUTE(node, value, QOpcUa::Types::ExtensionObject); // Write value to check for

    list.clear();
    list.append(xmlElements[0]);
    list.append(xmlElements[1]);
    list.append(xmlElements[2]);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.XmlElement"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::XmlElement);

    list.clear();
    list.append(testExpandedNodeId[0]);
    list.append(testExpandedNodeId[1]);
    list.append(testExpandedNodeId[2]);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.ExpandedNodeId"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::ExpandedNodeId);

    list.clear();
    list.append(testArguments[0]);
    list.append(testArguments[1]);
    list.append(testArguments[2]);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Argument"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Argument);

    list.clear();
    list.append(testStructureFields[0]);
    list.append(testStructureFields[1]);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.StructureField"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::StructureField);

    list.clear();
    list.append(testStructureDefinitions[0]);
    list.append(testStructureDefinitions[1]);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.StructureDefinition"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::StructureDefinition);

    list.clear();
    list.append(testEnumFields[0]);
    list.append(testEnumFields[1]);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.EnumField"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::EnumField);

    list.clear();
    list.append(testEnumDefinitions[0]);
    list.append(testEnumDefinitions[1]);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.EnumDefinition"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::EnumDefinition);

    list.clear();
    list.append(testDiagnosticInfos[0]);
    list.append(testDiagnosticInfos[1]);
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.DiagnosticInfo"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::DiagnosticInfo);

    list.clear();
    list.append(QVariant::fromValue(testEventFilters[0]));
    list.append(QVariant::fromValue(testEventFilters[1]));
    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.EventFilter"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::EventFilter);
}

void Tst_QOpcUaClient::readArray()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Boolean"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant booleanArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(booleanArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(booleanArray.toList().size(), 3);
    QCOMPARE(booleanArray.toList()[0].metaType().id(), QMetaType::Bool);
    QCOMPARE(booleanArray.toList()[0].toBool(), true);
    QCOMPARE(booleanArray.toList()[1].toBool(), false);
    QCOMPARE(booleanArray.toList()[2].toBool(), true);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Int32"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant int32Array = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(int32Array.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(int32Array.toList().size(), 3);
    QCOMPARE(int32Array.toList()[0].metaType().id(), QMetaType::Int);
    QCOMPARE(int32Array.toList()[0].toInt(), std::numeric_limits<qint32>::min());
    QCOMPARE(int32Array.toList()[1].toInt(), (std::numeric_limits<qint32>::max)());
    QCOMPARE(int32Array.toList()[2].toInt(), 10);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.UInt32"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant uint32Array = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(uint32Array.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(uint32Array.toList().size(), 3);
    QCOMPARE(uint32Array.toList()[0].metaType().id(), QMetaType::UInt);
    QCOMPARE(uint32Array.toList()[0].toUInt(), std::numeric_limits<quint32>::min());
    QCOMPARE(uint32Array.toList()[1].toUInt(), (std::numeric_limits<quint32>::max)());
    QCOMPARE(uint32Array.toList()[2].toUInt(), quint32(10));

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Double"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant doubleArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(doubleArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(doubleArray.toList().size(), 3);
    QCOMPARE(doubleArray.toList()[0].metaType().id(), QMetaType::Double);
    QCOMPARE(doubleArray.toList()[0].toDouble(), double(23.5));
    QCOMPARE(doubleArray.toList()[1].toDouble(), double(23.6));
    QCOMPARE(doubleArray.toList()[2].toDouble(), double(23.7));

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Float"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant floatArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(floatArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(floatArray.toList().size(), 3);
    QCOMPARE(floatArray.toList()[0].userType(), QMetaType::Float);
    QCOMPARE(floatArray.toList()[0].toFloat(), float(23.5));
    QCOMPARE(floatArray.toList()[1].toFloat(), float(23.6));
    QCOMPARE(floatArray.toList()[2].toFloat(), float(23.7));

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.String"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant stringArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(stringArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(stringArray.toList().size(), 3);
    QCOMPARE(stringArray.toList()[0].metaType().id(), QMetaType::QString);
    QCOMPARE(stringArray.toList()[0].toString(), u"Test1"_s);
    QCOMPARE(stringArray.toList()[1].toString(), u"Test2"_s);
    QCOMPARE(stringArray.toList()[2].toString(), u"Test3"_s);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.DateTime"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant dateTimeArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(dateTimeArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(dateTimeArray.toList().size(), 3);
    QCOMPARE(dateTimeArray.toList()[0].metaType().id(), QMetaType::QDateTime);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.LocalizedText"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant ltArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(ltArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(ltArray.toList().size(), 3);
    QCOMPARE(ltArray.toList()[0].value<QOpcUaLocalizedText>(), localizedTexts[0]);
    QCOMPARE(ltArray.toList()[1].value<QOpcUaLocalizedText>(), localizedTexts[1]);
    QCOMPARE(ltArray.toList()[2].value<QOpcUaLocalizedText>(), localizedTexts[2]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.UInt16"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant uint16Array = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(uint16Array.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(uint16Array.toList().size(), 3);
    QCOMPARE(uint16Array.toList()[0].userType(), QMetaType::UShort);
    QCOMPARE(uint16Array.toList()[0], std::numeric_limits<quint16>::min());
    QCOMPARE(uint16Array.toList()[1], (std::numeric_limits<quint16>::max)());
    QCOMPARE(uint16Array.toList()[2], quint16(10));

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Int16"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant int16Array = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(int16Array.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(int16Array.toList().size(), 3);
    QCOMPARE(int16Array.toList()[0].userType(), QMetaType::Short);
    QCOMPARE(int16Array.toList()[0], std::numeric_limits<qint16>::min());
    QCOMPARE(int16Array.toList()[1], (std::numeric_limits<qint16>::max)());
    QCOMPARE(int16Array.toList()[2], qint16(10));

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.UInt64"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant uint64Array = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(uint64Array.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(uint64Array.toList().size(), 3);
    QCOMPARE(uint64Array.toList()[0].metaType().id(), QMetaType::ULongLong);
    QCOMPARE(uint64Array.toList()[0], std::numeric_limits<quint64>::min());
    QCOMPARE(uint64Array.toList()[1], (std::numeric_limits<quint64>::max)());
    QCOMPARE(uint64Array.toList()[2], quint64(10));

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Int64"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant int64Array = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(int64Array.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(int64Array.toList().size(), 3);
    QCOMPARE(int64Array.toList()[0].metaType().id(), QMetaType::LongLong);
    QCOMPARE(int64Array.toList()[0], std::numeric_limits<qint64>::min());
    QCOMPARE(int64Array.toList()[1], (std::numeric_limits<qint64>::max)());
    QCOMPARE(int64Array.toList()[2], qint64(10));

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Byte"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant byteArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(byteArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(byteArray.toList().size(), 3);
    QCOMPARE(byteArray.toList()[0].userType(), QMetaType::UChar);
    QCOMPARE(byteArray.toList()[0], std::numeric_limits<quint8>::min());
    QCOMPARE(byteArray.toList()[1], (std::numeric_limits<quint8>::max)());
    QCOMPARE(byteArray.toList()[2], quint8(10));

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.ByteString"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant byteStringArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(byteStringArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(byteStringArray.toList().size(), 3);
    QCOMPARE(byteStringArray.toList()[0].userType(), QMetaType::QByteArray);
    QCOMPARE(byteStringArray.toList()[0].value<QByteArray>(), "abc");
    QCOMPARE(byteStringArray.toList()[1].value<QByteArray>(), "def");
    QByteArray withNull("gh");
    withNull.append('\0');
    withNull.append("i");
    QCOMPARE(byteStringArray.toList()[2].value<QByteArray>(), withNull);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Guid"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant guidArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(guidArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(guidArray.toList().size(), 3);
    QCOMPARE(guidArray.toList()[0].toUuid(), testUuid[0]);
    QCOMPARE(guidArray.toList()[1].toUuid(), testUuid[1]);
    QCOMPARE(guidArray.toList()[2].toUuid(), testUuid[2]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.SByte"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant sbyteArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(sbyteArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(sbyteArray.toList().size(), 3);
    QCOMPARE(sbyteArray.toList()[0].userType(), QMetaType::SChar);
    QCOMPARE(sbyteArray.toList()[0], std::numeric_limits<qint8>::min());
    QCOMPARE(sbyteArray.toList()[1], (std::numeric_limits<qint8>::max)());
    QCOMPARE(sbyteArray.toList()[2], qint8(10));

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.NodeId"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant nodeIdArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(nodeIdArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(nodeIdArray.toList().size(), 3);
    QCOMPARE(nodeIdArray.toList()[0].metaType().id(), QMetaType::QString);
    QCOMPARE(nodeIdArray.toList()[0].toString(), u"ns=0;i=0"_s);
    QCOMPARE(nodeIdArray.toList()[1].toString(), u"ns=0;i=1"_s);
    QCOMPARE(nodeIdArray.toList()[2].toString(), u"ns=0;i=2"_s);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.QualifiedName"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    QVariant qualifiedNameArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(qualifiedNameArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(qualifiedNameArray.toList().size(), 3);
    QCOMPARE(qualifiedNameArray.toList()[0].value<QOpcUaQualifiedName>(), QOpcUaQualifiedName(0, u"Test0"_s));
    QCOMPARE(qualifiedNameArray.toList()[1].value<QOpcUaQualifiedName>(), QOpcUaQualifiedName(1, u"Test1"_s));
    QCOMPARE(qualifiedNameArray.toList()[2].value<QOpcUaQualifiedName>(), QOpcUaQualifiedName(2, u"Test2"_s));

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.StatusCode"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant statusCodeArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(statusCodeArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(statusCodeArray.toList().size(), 3);
    QCOMPARE(statusCodeArray.toList()[0].value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(statusCodeArray.toList()[1].value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadUnexpectedError);
    QCOMPARE(statusCodeArray.toList()[2].value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadInternalError);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Range"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant rangeArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(rangeArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(rangeArray.toList().size(), 3);
    QCOMPARE(rangeArray.toList()[0].value<QOpcUaRange>(), testRanges[0]);
    QCOMPARE(rangeArray.toList()[1].value<QOpcUaRange>(), testRanges[1]);
    QCOMPARE(rangeArray.toList()[2].value<QOpcUaRange>(), testRanges[2]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.EUInformation"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant euiArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(euiArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(euiArray.toList().size(), 3);
    QCOMPARE(euiArray.toList()[0].value<QOpcUaEUInformation>(), testEUInfos[0]);
    QCOMPARE(euiArray.toList()[1].value<QOpcUaEUInformation>(), testEUInfos[1]);
    QCOMPARE(euiArray.toList()[2].value<QOpcUaEUInformation>(), testEUInfos[2]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.ComplexNumber"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant complexArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(complexArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(complexArray.toList().size(), 3);
    QCOMPARE(complexArray.toList()[0].value<QOpcUaComplexNumber>(), testComplex[0]);
    QCOMPARE(complexArray.toList()[1].value<QOpcUaComplexNumber>(), testComplex[1]);
    QCOMPARE(complexArray.toList()[2].value<QOpcUaComplexNumber>(), testComplex[2]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.DoubleComplexNumber"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant doubleComplexArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(doubleComplexArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(doubleComplexArray.toList().size(), 3);
    QCOMPARE(doubleComplexArray.toList()[0].value<QOpcUaDoubleComplexNumber>(), testDoubleComplex[0]);
    QCOMPARE(doubleComplexArray.toList()[1].value<QOpcUaDoubleComplexNumber>(), testDoubleComplex[1]);
    QCOMPARE(doubleComplexArray.toList()[2].value<QOpcUaDoubleComplexNumber>(), testDoubleComplex[2]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.AxisInformation"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant axisInfoArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(axisInfoArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(axisInfoArray.toList().size(), 3);

    QCOMPARE(axisInfoArray.toList()[0].value<QOpcUaAxisInformation>(), testAxisInfo[0]);
    QCOMPARE(axisInfoArray.toList()[1].value<QOpcUaAxisInformation>(), testAxisInfo[1]);
    QCOMPARE(axisInfoArray.toList()[2].value<QOpcUaAxisInformation>(), testAxisInfo[2]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.XV"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant xVArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(xVArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(xVArray.toList().size(), 3);
    QCOMPARE(xVArray.toList()[0].value<QOpcUaXValue>(), testXV[0]);
    QCOMPARE(xVArray.toList()[1].value<QOpcUaXValue>(), testXV[1]);
    QCOMPARE(xVArray.toList()[2].value<QOpcUaXValue>(), testXV[2]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.ExtensionObject"_s));
    QVERIFY(node != nullptr);

    READ_MANDATORY_VARIABLE_NODE(node);

    QVariantList list = node->attribute(QOpcUa::NodeAttribute::Value).toList();
    QCOMPARE(list.size(), testRanges.size());

    for (int i = 0; i < testRanges.size(); ++i) {
        QOpcUaExtensionObject obj = list.at(i).value<QOpcUaExtensionObject>();
        VERIFY_EXTENSION_OBJECT(obj, i);
    }

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.XmlElement"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    QVariant xmlElementArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(xmlElementArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(xmlElementArray.toList().size(), 3);
    QCOMPARE(xmlElementArray.toList()[0].metaType().id(), QMetaType::QString);
    QCOMPARE(xmlElementArray.toList()[0].toString(), xmlElements[0]);
    QCOMPARE(xmlElementArray.toList()[1].toString(), xmlElements[1]);
    QCOMPARE(xmlElementArray.toList()[2].toString(), xmlElements[2]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.ExpandedNodeId"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    QVariant expandedNodeIdArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(expandedNodeIdArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(expandedNodeIdArray.toList().size(), 3);
    QCOMPARE(expandedNodeIdArray.toList()[0].value<QOpcUaExpandedNodeId>(), testExpandedNodeId[0]);
    QCOMPARE(expandedNodeIdArray.toList()[1].value<QOpcUaExpandedNodeId>(), testExpandedNodeId[1]);
    QCOMPARE(expandedNodeIdArray.toList()[2].value<QOpcUaExpandedNodeId>(), testExpandedNodeId[2]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Argument"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    QVariant argumentArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(argumentArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(argumentArray.toList().size(), 3);
    QCOMPARE(argumentArray.toList()[0].value<QOpcUaArgument>(), testArguments[0]);
    QCOMPARE(argumentArray.toList()[1].value<QOpcUaArgument>(), testArguments[1]);
    QCOMPARE(argumentArray.toList()[2].value<QOpcUaArgument>(), testArguments[2]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.StructureField"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    const QVariant structureFieldsArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(structureFieldsArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(structureFieldsArray.toList().length(), 2);
    QCOMPARE(structureFieldsArray.toList()[0].value<QOpcUaStructureField>(), testStructureFields[0]);
    QCOMPARE(structureFieldsArray.toList()[1].value<QOpcUaStructureField>(), testStructureFields[1]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.StructureDefinition"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    const QVariant structureDefinitionsArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(structureDefinitionsArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(structureDefinitionsArray.toList().length(), 2);
    QCOMPARE(structureDefinitionsArray.toList()[0].value<QOpcUaStructureDefinition>(), testStructureDefinitions[0]);
    QCOMPARE(structureDefinitionsArray.toList()[1].value<QOpcUaStructureDefinition>(), testStructureDefinitions[1]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.EnumField"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    const QVariant enumFieldsArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(enumFieldsArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(enumFieldsArray.toList().length(), 2);
    QCOMPARE(enumFieldsArray.toList()[0].value<QOpcUaEnumField>(), testEnumFields[0]);
    QCOMPARE(enumFieldsArray.toList()[1].value<QOpcUaEnumField>(), testEnumFields[1]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.EnumDefinition"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    const QVariant enumDefinitionsArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(enumDefinitionsArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(enumDefinitionsArray.toList().length(), 2);
    QCOMPARE(enumDefinitionsArray.toList()[0].value<QOpcUaEnumDefinition>(), testEnumDefinitions[0]);
    QCOMPARE(enumDefinitionsArray.toList()[1].value<QOpcUaEnumDefinition>(), testEnumDefinitions[1]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.DiagnosticInfo"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    const QVariant diagnosticInfoArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(diagnosticInfoArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(diagnosticInfoArray.toList().length(), 2);
    QCOMPARE(diagnosticInfoArray.toList()[0].value<QOpcUaDiagnosticInfo>(), testDiagnosticInfos[0]);
    QCOMPARE(diagnosticInfoArray.toList()[1].value<QOpcUaDiagnosticInfo>(), testDiagnosticInfos[1]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.EventFilter"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    QVariant eventFilterArray = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(eventFilterArray.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(eventFilterArray.toList().size(), 2);
    QCOMPARE(eventFilterArray.toList()[0].value<QOpcUaMonitoringParameters::EventFilter>(), testEventFilters[0]);
    QCOMPARE(eventFilterArray.toList()[1].value<QOpcUaMonitoringParameters::EventFilter>(), testEventFilters[1]);
}

void Tst_QOpcUaClient::writeScalar()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Boolean"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, true, QOpcUa::Types::Boolean);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Int32"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, std::numeric_limits<qint32>::min(), QOpcUa::Types::Int32);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.UInt32"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, (std::numeric_limits<quint32>::max)(), QOpcUa::UInt32);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Double"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, 42, QOpcUa::Double);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Float"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, 42, QOpcUa::Float);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.String"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, u"QOpcUa Teststring"_s, QOpcUa::String);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.DateTime"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, QDateTime::currentDateTime(), QOpcUa::DateTime);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.UInt16"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, (std::numeric_limits<quint16>::max)(), QOpcUa::UInt16);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Int16"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, std::numeric_limits<qint16>::min(), QOpcUa::Int16);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.UInt64"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, (std::numeric_limits<quint64>::max)(), QOpcUa::UInt64);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Int64"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, std::numeric_limits<qint64>::min(), QOpcUa::Int64);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Byte"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, (std::numeric_limits<quint8>::max)(), QOpcUa::Byte);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.SByte"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, std::numeric_limits<qint8>::min(), QOpcUa::SByte);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.LocalizedText"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, localizedTexts[0], QOpcUa::LocalizedText);

    QByteArray withNull("gh");
    withNull.append('\0');
    withNull.append("i");

    QVariant data = withNull;

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.ByteString"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, data, QOpcUa::ByteString);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Guid"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, testUuid[0], QOpcUa::Guid);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.NodeId"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, u"ns=42;s=Test"_s, QOpcUa::NodeId);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.QualifiedName"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, QOpcUaQualifiedName(0, u"Test0"_s), QOpcUa::QualifiedName);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.StatusCode"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, QOpcUa::UaStatusCode::BadInternalError, QOpcUa::StatusCode);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Range"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, QVariant::fromValue(testRanges[0]), QOpcUa::Range);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.EUInformation"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, QVariant::fromValue(testEUInfos[0]), QOpcUa::EUInformation);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.ComplexNumber"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, QVariant::fromValue(testComplex[0]), QOpcUa::ComplexNumber);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.DoubleComplexNumber"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, QVariant::fromValue(testDoubleComplex[0]), QOpcUa::DoubleComplexNumber);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.AxisInformation"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, QVariant::fromValue(testAxisInfo[0]), QOpcUa::AxisInformation);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.XV"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, QVariant::fromValue(testXV[0]), QOpcUa::XV);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.ExtensionObject"_s));
    QVERIFY(node != nullptr);

    QOpcUaExtensionObject obj;
    ENCODE_EXTENSION_OBJECT(obj, 0);

    WRITE_VALUE_ATTRIBUTE(node, obj, QOpcUa::Types::ExtensionObject); // Write value to check for

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.XmlElement"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, xmlElements[0], QOpcUa::XmlElement);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.ExpandedNodeId"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, testExpandedNodeId[0], QOpcUa::ExpandedNodeId);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Argument"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, testArguments[0], QOpcUa::Argument);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.StructureField"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, testStructureFields[0], QOpcUa::StructureField);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.StructureDefinition"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, testStructureDefinitions[0], QOpcUa::StructureDefinition);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.EnumField"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, testEnumFields[0], QOpcUa::EnumField);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.EnumDefinition"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, testEnumDefinitions[0], QOpcUa::EnumDefinition);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.DiagnosticInfo"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, testDiagnosticInfos[0], QOpcUa::DiagnosticInfo);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.EventFilter"_s));
    QVERIFY(node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, testEventFilters[0], QOpcUa::EventFilter);
}

void Tst_QOpcUaClient::readScalar()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Boolean"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant booleanScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(booleanScalar.isValid());
    QCOMPARE(booleanScalar.metaType().id(), QMetaType::Bool);
    QCOMPARE(booleanScalar.toBool(), true);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Int32"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant int32Scalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(int32Scalar.isValid());
    QCOMPARE(int32Scalar.metaType().id(), QMetaType::Int);
    QCOMPARE(int32Scalar.toInt(), std::numeric_limits<qint32>::min());

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.UInt32"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant uint32Scalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(uint32Scalar.isValid());
    QCOMPARE(uint32Scalar.metaType().id(), QMetaType::UInt);
    QCOMPARE(uint32Scalar.toUInt(), (std::numeric_limits<quint32>::max)());

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Double"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant doubleScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(doubleScalar.isValid());
    QCOMPARE(doubleScalar.metaType().id(), QMetaType::Double);
    QCOMPARE(doubleScalar.toDouble(), double(42));

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Float"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant floatScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(floatScalar.isValid());
    QCOMPARE(floatScalar.userType(), QMetaType::Float);
    QCOMPARE(floatScalar.toFloat(), float(42));

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.String"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant stringScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(stringScalar.isValid());
    QCOMPARE(stringScalar.metaType().id(), QMetaType::QString);
    QCOMPARE(stringScalar.toString(), u"QOpcUa Teststring"_s);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.DateTime"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant dateTimeScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(dateTimeScalar.metaType().id(), QMetaType::QDateTime);
    QVERIFY(dateTimeScalar.isValid());

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.LocalizedText"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant ltScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(ltScalar.isValid());
    QCOMPARE(ltScalar.value<QOpcUaLocalizedText>(), localizedTexts[0]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.UInt16"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant uint16Scalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(uint16Scalar.isValid());
    QCOMPARE(uint16Scalar.userType(), QMetaType::UShort);
    QCOMPARE(uint16Scalar, (std::numeric_limits<quint16>::max)());

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Int16"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant int16Scalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(int16Scalar.isValid());
    QCOMPARE(int16Scalar.userType(), QMetaType::Short);
    QCOMPARE(int16Scalar, std::numeric_limits<qint16>::min());

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.UInt64"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant uint64Scalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(uint64Scalar.isValid());
    QCOMPARE(uint64Scalar.metaType().id(), QMetaType::ULongLong);
    QCOMPARE(uint64Scalar, (std::numeric_limits<quint64>::max)());

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Int64"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant int64Scalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(int64Scalar.isValid());
    QCOMPARE(int64Scalar.metaType().id(), QMetaType::LongLong);
    QCOMPARE(int64Scalar, std::numeric_limits<qint64>::min());

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Byte"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant byteScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(byteScalar.isValid());
    QCOMPARE(byteScalar.userType(), QMetaType::UChar);
    QCOMPARE(byteScalar, (std::numeric_limits<quint8>::max)());

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.SByte"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant sbyteScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(sbyteScalar.isValid());
    QCOMPARE(sbyteScalar.userType(), QMetaType::SChar);
    QCOMPARE(sbyteScalar, std::numeric_limits<qint8>::min());

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.ByteString"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant byteStringScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(byteStringScalar.isValid());
    QCOMPARE(byteStringScalar.userType(), QMetaType::QByteArray);
    QByteArray withNull("gh");
    withNull.append('\0');
    withNull.append("i");
    QCOMPARE(byteStringScalar, withNull);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Guid"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant guidScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(guidScalar.isValid());
    QCOMPARE(guidScalar.userType(), QMetaType::QUuid);
    QCOMPARE(guidScalar.toUuid(), testUuid[0]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.NodeId"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant nodeIdScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(nodeIdScalar.isValid());
    QCOMPARE(nodeIdScalar.metaType().id(), QMetaType::QString);
    QCOMPARE(nodeIdScalar.toString(), u"ns=42;s=Test"_s);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.QualifiedName"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    QVariant qualifiedNameScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(qualifiedNameScalar.value<QOpcUaQualifiedName>(), QOpcUaQualifiedName(0, u"Test0"_s));

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.StatusCode"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant statusCodeScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(statusCodeScalar.isValid());
    QCOMPARE(statusCodeScalar.value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadInternalError);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Range"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::Value).value<QOpcUaRange>(), testRanges[0]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.EUInformation"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::Value).value<QOpcUaEUInformation>(), testEUInfos[0]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.ComplexNumber"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::Value).value<QOpcUaComplexNumber>(), testComplex[0]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.DoubleComplexNumber"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::Value).value<QOpcUaDoubleComplexNumber>(), testDoubleComplex[0]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.AxisInformation"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::Value).value<QOpcUaAxisInformation>(), testAxisInfo[0]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.XV"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::Value).value<QOpcUaXValue>(), testXV[0]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.ExtensionObject"_s));
    QVERIFY(node != nullptr);

    READ_MANDATORY_VARIABLE_NODE(node);

    QOpcUaExtensionObject obj = node->attribute(QOpcUa::NodeAttribute::Value).value<QOpcUaExtensionObject>();
    VERIFY_EXTENSION_OBJECT(obj, 0);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.XmlElement"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    QVariant xmlElementScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(xmlElementScalar.isValid());
    QCOMPARE(xmlElementScalar.metaType().id(), QMetaType::QString);
    QCOMPARE(xmlElementScalar.toString(), xmlElements[0]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.ExpandedNodeId"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    QVariant expandedNodeIdScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(expandedNodeIdScalar.isValid());
    QCOMPARE(expandedNodeIdScalar.value<QOpcUaExpandedNodeId>(), testExpandedNodeId[0]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Argument"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    QVariant argumentScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(argumentScalar.isValid());
    QCOMPARE(argumentScalar.value<QOpcUaArgument>(), testArguments[0]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.StructureField"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    const QVariant structureFieldScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(structureFieldScalar.isValid());
    QCOMPARE(structureFieldScalar.value<QOpcUaStructureField>(), testStructureFields[0]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.StructureDefinition"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    const QVariant structureDefinitionScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(structureDefinitionScalar.isValid());
    QCOMPARE(structureDefinitionScalar.value<QOpcUaStructureDefinition>(), testStructureDefinitions[0]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.EnumField"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    const QVariant enumFieldScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(enumFieldScalar.isValid());
    QCOMPARE(enumFieldScalar.value<QOpcUaEnumField>(), testEnumFields[0]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.EnumDefinition"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    const QVariant enumDefinitionScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(enumDefinitionScalar.isValid());
    QCOMPARE(enumDefinitionScalar.value<QOpcUaEnumDefinition>(), testEnumDefinitions[0]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.DiagnosticInfo"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    const QVariant diagnosticInfoScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(diagnosticInfoScalar.isValid());
    QCOMPARE(diagnosticInfoScalar.value<QOpcUaDiagnosticInfo>(), testDiagnosticInfos[0]);

    node.reset(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.EventFilter"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node)
    QVariant eventFilterScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(eventFilterScalar.isValid());
    QCOMPARE(eventFilterScalar.value<QOpcUaMonitoringParameters::EventFilter>(), testEventFilters[0]);
}

void Tst_QOpcUaClient::readReencodedExtensionObject()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    const auto checkAndDecodeFirstMember = [](const QOpcUaExtensionObject &obj, int index) {
        QCOMPARE(obj.encoding(), QOpcUaExtensionObject::Encoding::ByteString);
        QCOMPARE(obj.encodingTypeId(), QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::ServerStatusDataType_Encoding_DefaultBinary));
        QCOMPARE(obj.encodedBody().size(), 140);
        auto data = obj.encodedBody();
        auto decoder = QOpcUaBinaryDataEncoding(&data);
        bool success = false;
        const auto start = decoder.decode<QDateTime>(success);
        QVERIFY(success);

        QCOMPARE(start, QDateTime::fromMSecsSinceEpoch(1691996809123 + index));
    };

    // Scalar case
    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=3;s=ServerStatusScalar"_s));
    QVERIFY(node != nullptr);

    QSignalSpy scalarSpy(node.data(), &QOpcUaNode::attributeRead);
    node->readValueAttribute();
    scalarSpy.wait(signalSpyTimeout);
    QCOMPARE(scalarSpy.size(), 1);

    QCOMPARE(node->valueAttributeError(), QOpcUa::UaStatusCode::Good);
    checkAndDecodeFirstMember(node->valueAttribute().value<QOpcUaExtensionObject>(), 0);

    // Array case
    node.reset(opcuaClient->node(u"ns=3;s=ServerStatusArray"_s));

    QSignalSpy arraySpy(node.data(), &QOpcUaNode::attributeRead);
    node->readValueAttribute();
    arraySpy.wait(signalSpyTimeout);
    QCOMPARE(arraySpy.size(), 1);

    QCOMPARE(node->valueAttributeError(), QOpcUa::UaStatusCode::Good);
    const auto arrayContent = node->valueAttribute().toList();
    QCOMPARE(arrayContent.size(), 2);

    for (int i = 0; i < arrayContent.size(); ++i) {
        checkAndDecodeFirstMember(arrayContent.at(i).value<QOpcUaExtensionObject>(), i);
    }

    // Multi dimensional array case
    node.reset(opcuaClient->node(u"ns=3;s=ServerStatusMultiDimensionalArray"_s));

    QSignalSpy multiArraySpy(node.data(), &QOpcUaNode::attributeRead);
    node->readValueAttribute();
    multiArraySpy.wait(signalSpyTimeout);
    QCOMPARE(multiArraySpy.size(), 1);

    QCOMPARE(node->valueAttributeError(), QOpcUa::UaStatusCode::Good);
    const auto multiArray = node->valueAttribute().value<QOpcUaMultiDimensionalArray>();
    QVERIFY(multiArray.isValid());
    QCOMPARE(multiArray.arrayDimensions(), QList<quint32>({2, 2}));

    checkAndDecodeFirstMember(multiArray.value({0, 0}).value<QOpcUaExtensionObject>(), 0);
    checkAndDecodeFirstMember(multiArray.value({0, 1}).value<QOpcUaExtensionObject>(), 1);
    checkAndDecodeFirstMember(multiArray.value({1, 0}).value<QOpcUaExtensionObject>(), 2);
    checkAndDecodeFirstMember(multiArray.value({1, 1}).value<QOpcUaExtensionObject>(), 3);
}

void Tst_QOpcUaClient::indexRange()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Int32"_s));
    QVERIFY(node != nullptr);

    QVariantList list({0, 1, 2, 3, 4, 5, 6, 7});

    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Types::Int32);

    QSignalSpy attributeWrittenSpy(node.data(), &QOpcUaNode::attributeWritten);

    node->writeAttributeRange(QOpcUa::NodeAttribute::Value, QVariantList({10, 11, 12, 13}), u"0:3"_s, QOpcUa::Types::Int32);
    attributeWrittenSpy.wait(signalSpyTimeout);
    QCOMPARE(attributeWrittenSpy.size(), 1);
    QCOMPARE(attributeWrittenSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    QSignalSpy attributeReadSpy(node.data(), &QOpcUaNode::attributeRead);
    node->readAttributeRange(QOpcUa::NodeAttribute::Value, u"0:6"_s);
    attributeReadSpy.wait(signalSpyTimeout);
    QCOMPARE(attributeReadSpy.size(), 1);

    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::Value).toList(), QVariantList({10, 11, 12, 13, 4, 5, 6}));
}

void Tst_QOpcUaClient::invalidIndexRange()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Int32"_s));
    QVERIFY(node != nullptr);

    QVariantList list({0, 1, 2, 3, 4, 5, 6, 7});

    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Types::Int32);

    QSignalSpy attributeWrittenSpy(node.data(), &QOpcUaNode::attributeWritten);

    node->writeAttributeRange(QOpcUa::NodeAttribute::Value, QVariantList({10, 11, 12, 13}), u"notavalidrange"_s, QOpcUa::Types::Int32);
    attributeWrittenSpy.wait(signalSpyTimeout);
    QCOMPARE(attributeWrittenSpy.size(), 1);
    QCOMPARE(attributeWrittenSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadIndexRangeInvalid);

    QSignalSpy attributeReadSpy(node.data(), &QOpcUaNode::attributeRead);
    node->readAttributeRange(QOpcUa::NodeAttribute::Value, u"notavalidrange"_s);
    attributeReadSpy.wait(signalSpyTimeout);
    QCOMPARE(attributeReadSpy.size(), 1);

    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::Value), QOpcUa::UaStatusCode::BadIndexRangeInvalid);
}

void Tst_QOpcUaClient::subscriptionIndexRange()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> integerArrayNode(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.Int32"_s));
    QVERIFY(integerArrayNode != nullptr);

    QOpcUaMonitoringParameters p(100);
    p.setIndexRange(u"1"_s);
    QSignalSpy monitoringEnabledSpy(integerArrayNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QSignalSpy monitoringDisabledSpy(integerArrayNode.data(), &QOpcUaNode::disableMonitoringFinished);
    QSignalSpy writeSpy(integerArrayNode.data(), &QOpcUaNode::attributeWritten);
    QSignalSpy dataChangeSpy(integerArrayNode.data(), &QOpcUaNode::dataChangeOccurred);

    QVariantList l({0, 1});
    WRITE_VALUE_ATTRIBUTE(integerArrayNode, l, QOpcUa::Types::Int32);
    writeSpy.clear();

    integerArrayNode->enableMonitoring(QOpcUa::NodeAttribute::Value, p);
    monitoringEnabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringEnabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    dataChangeSpy.wait(signalSpyTimeout); // Wait for the initial data change
    dataChangeSpy.clear();
    integerArrayNode->writeAttributeRange(QOpcUa::NodeAttribute::Value, 10, u"0"_s, QOpcUa::Types::Int32); // Write the first element of the array
    writeSpy.wait(signalSpyTimeout);
    QCOMPARE(writeSpy.size(), 1);
    QCOMPARE(writeSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(writeSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    dataChangeSpy.wait(1000);
    QCOMPARE(dataChangeSpy.size(), 0);

    writeSpy.clear();
    integerArrayNode->writeAttributeRange(QOpcUa::NodeAttribute::Value, 10, u"1"_s, QOpcUa::Types::Int32); // Write the second element of the array
    writeSpy.wait(signalSpyTimeout);
    QCOMPARE(writeSpy.size(), 1);
    QCOMPARE(writeSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(writeSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    dataChangeSpy.wait(signalSpyTimeout);
    QCOMPARE(dataChangeSpy.size(), 1);
    QCOMPARE(integerArrayNode->attribute(QOpcUa::NodeAttribute::Value).toDouble(), 10.0);

    integerArrayNode->disableMonitoring(QOpcUa::NodeAttribute::Value);
    monitoringDisabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringDisabledSpy.size(), 1);
    QCOMPARE(monitoringDisabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringDisabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
}

void Tst_QOpcUaClient::subscriptionDataChangeFilter()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> doubleWriteNode(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Double"_s));
    QVERIFY(doubleWriteNode != nullptr);

    QScopedPointer<QOpcUaNode> doubleNode(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Double"_s));
    QVERIFY(doubleNode != nullptr);

    QOpcUaMonitoringParameters p(100);

    QSignalSpy monitoringEnabledSpy(doubleNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QSignalSpy monitoringDisabledSpy(doubleNode.data(), &QOpcUaNode::disableMonitoringFinished);
    QSignalSpy dataChangeSpy(doubleNode.data(), &QOpcUaNode::dataChangeOccurred);
    QSignalSpy attributeUpdatedSpy(doubleNode.data(), &QOpcUaNode::attributeUpdated);
    QSignalSpy monitoringModifiedSpy(doubleNode.data(), &QOpcUaNode::monitoringStatusChanged);

    WRITE_VALUE_ATTRIBUTE(doubleWriteNode, 1.0, QOpcUa::Types::Double);

    doubleNode->enableMonitoring(QOpcUa::NodeAttribute::Value, p);
    monitoringEnabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringEnabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    dataChangeSpy.wait(signalSpyTimeout); // Wait for the initial data change
    if (attributeUpdatedSpy.empty())
        attributeUpdatedSpy.wait(signalSpyTimeout);
    QCOMPARE(dataChangeSpy.size(), 1);
    QCOMPARE(attributeUpdatedSpy.size(), 1);
    dataChangeSpy.clear();
    attributeUpdatedSpy.clear();

    WRITE_VALUE_ATTRIBUTE(doubleWriteNode, 1.5, QOpcUa::Types::Double);

    dataChangeSpy.wait(signalSpyTimeout);
    if (attributeUpdatedSpy.empty())
        attributeUpdatedSpy.wait(signalSpyTimeout);
    QCOMPARE(dataChangeSpy.size(), 1); // Data change without filter
    QCOMPARE(attributeUpdatedSpy.size(), 1);
    QCOMPARE(doubleNode->attribute(QOpcUa::NodeAttribute::Value), 1.5);
    dataChangeSpy.clear();
    attributeUpdatedSpy.clear();

    QOpcUaMonitoringParameters::DataChangeFilter filter;
    filter.setDeadbandType(QOpcUaMonitoringParameters::DataChangeFilter::DeadbandType::Absolute);
    filter.setTrigger(QOpcUaMonitoringParameters::DataChangeFilter::DataChangeTrigger::StatusOrValue);
    filter.setDeadbandValue(1.0);
    doubleNode->modifyDataChangeFilter(QOpcUa::NodeAttribute::Value, filter);
    monitoringModifiedSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringModifiedSpy.size(), 1);
    QCOMPARE(monitoringModifiedSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QVERIFY(monitoringModifiedSpy.at(0).at(1).value<QOpcUaMonitoringParameters::Parameters>() & QOpcUaMonitoringParameters::Parameter::Filter);
    QCOMPARE(monitoringModifiedSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    WRITE_VALUE_ATTRIBUTE(doubleWriteNode, 2.0, QOpcUa::Types::Double);

    dataChangeSpy.wait(1000);
    if (attributeUpdatedSpy.empty())
        attributeUpdatedSpy.wait(100);
    QCOMPARE(dataChangeSpy.size(), 0); // Filter is active and delta is < 1
    QCOMPARE(attributeUpdatedSpy.size(), 0);
    attributeUpdatedSpy.clear();

    WRITE_VALUE_ATTRIBUTE(doubleWriteNode, 3.0, QOpcUa::Types::Double);

    dataChangeSpy.wait(signalSpyTimeout);
    if (attributeUpdatedSpy.empty())
        attributeUpdatedSpy.wait(signalSpyTimeout);
    QCOMPARE(dataChangeSpy.size(), 1); // delta == 1, a data change is expected
    QCOMPARE(attributeUpdatedSpy.size(), 1);
    QCOMPARE(doubleNode->attribute(QOpcUa::NodeAttribute::Value), 3.0);

    doubleNode->disableMonitoring(QOpcUa::NodeAttribute::Value);
    monitoringDisabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringDisabledSpy.size(), 1);
    QCOMPARE(monitoringDisabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringDisabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
}

void Tst_QOpcUaClient::modifyPublishingMode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> doubleNode(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Double"_s));
    QVERIFY(doubleNode != nullptr);

    QOpcUaMonitoringParameters p(100);

    QSignalSpy monitoringEnabledSpy(doubleNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QSignalSpy monitoringDisabledSpy(doubleNode.data(), &QOpcUaNode::disableMonitoringFinished);
    QSignalSpy dataChangeSpy(doubleNode.data(), &QOpcUaNode::dataChangeOccurred);
    QSignalSpy monitoringStatusSpy(doubleNode.data(), &QOpcUaNode::monitoringStatusChanged);

    WRITE_VALUE_ATTRIBUTE(doubleNode, 1.0, QOpcUa::Types::Double);

    doubleNode->enableMonitoring(QOpcUa::NodeAttribute::Value, p);
    monitoringEnabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringEnabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    dataChangeSpy.wait(signalSpyTimeout); // Wait for the initial data change
    QCOMPARE(dataChangeSpy.size(), 1);
    dataChangeSpy.clear();

    WRITE_VALUE_ATTRIBUTE(doubleNode, 1.5, QOpcUa::Types::Double);

    dataChangeSpy.wait(signalSpyTimeout);
    QCOMPARE(dataChangeSpy.size(), 1);
    dataChangeSpy.clear();

    doubleNode->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::PublishingEnabled, false);
    monitoringStatusSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringStatusSpy.size(), 1);
    QCOMPARE(monitoringStatusSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    WRITE_VALUE_ATTRIBUTE(doubleNode, 3.0, QOpcUa::Types::Double);

    dataChangeSpy.wait(1000);
    QCOMPARE(dataChangeSpy.size(), 0);

    doubleNode->disableMonitoring(QOpcUa::NodeAttribute::Value);
    monitoringDisabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringDisabledSpy.size(), 1);
    QCOMPARE(monitoringDisabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringDisabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
}

void Tst_QOpcUaClient::modifyMonitoringMode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> doubleNode(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Double"_s));
    QVERIFY(doubleNode != nullptr);

    QOpcUaMonitoringParameters p(100);

    QSignalSpy monitoringEnabledSpy(doubleNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QSignalSpy monitoringDisabledSpy(doubleNode.data(), &QOpcUaNode::disableMonitoringFinished);
    QSignalSpy dataChangeSpy(doubleNode.data(), &QOpcUaNode::dataChangeOccurred);
    QSignalSpy monitoringStatusSpy(doubleNode.data(), &QOpcUaNode::monitoringStatusChanged);

    WRITE_VALUE_ATTRIBUTE(doubleNode, 1.0, QOpcUa::Types::Double);

    doubleNode->enableMonitoring(QOpcUa::NodeAttribute::Value, p);
    monitoringEnabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringEnabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    dataChangeSpy.wait(signalSpyTimeout); // Wait for the initial data change
    QCOMPARE(dataChangeSpy.size(), 1);
    dataChangeSpy.clear();

    WRITE_VALUE_ATTRIBUTE(doubleNode, 1.5, QOpcUa::Types::Double);

    dataChangeSpy.wait(signalSpyTimeout);
    QCOMPARE(dataChangeSpy.size(), 1);
    dataChangeSpy.clear();

    doubleNode->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::MonitoringMode,
                                 QVariant::fromValue(QOpcUaMonitoringParameters::MonitoringMode::Disabled));
    monitoringStatusSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringStatusSpy.size(), 1);
    QCOMPARE(monitoringStatusSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    WRITE_VALUE_ATTRIBUTE(doubleNode, 3.0, QOpcUa::Types::Double);

    dataChangeSpy.wait(1000);
    QCOMPARE(dataChangeSpy.size(), 0);

    doubleNode->disableMonitoring(QOpcUa::NodeAttribute::Value);
    monitoringDisabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringDisabledSpy.size(), 1);
    QCOMPARE(monitoringDisabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringDisabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
}

void Tst_QOpcUaClient::modifyMonitoredItem()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> doubleNode(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Double"_s));
    QVERIFY(doubleNode != nullptr);

    QOpcUaMonitoringParameters p(100);

    QSignalSpy monitoringEnabledSpy(doubleNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QSignalSpy monitoringDisabledSpy(doubleNode.data(), &QOpcUaNode::disableMonitoringFinished);
    QSignalSpy dataChangeSpy(doubleNode.data(), &QOpcUaNode::dataChangeOccurred);
    QSignalSpy monitoringStatusSpy(doubleNode.data(), &QOpcUaNode::monitoringStatusChanged);

    WRITE_VALUE_ATTRIBUTE(doubleNode, 1.0, QOpcUa::Types::Double);

    doubleNode->enableMonitoring(QOpcUa::NodeAttribute::Value, p);
    monitoringEnabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringEnabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    dataChangeSpy.wait(signalSpyTimeout); // Wait for the initial data change
    QCOMPARE(dataChangeSpy.size(), 1);
    dataChangeSpy.clear();

    WRITE_VALUE_ATTRIBUTE(doubleNode, 1.5, QOpcUa::Types::Double);

    dataChangeSpy.wait(signalSpyTimeout);
    QCOMPARE(dataChangeSpy.size(), 1);
    QCOMPARE(dataChangeSpy.at(0).at(1).value<double>(), 1.5);
    dataChangeSpy.clear();

    doubleNode->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::SamplingInterval, 50.0);
    monitoringStatusSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringStatusSpy.size(), 1);
    QCOMPARE(monitoringStatusSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(monitoringStatusSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QVERIFY(monitoringStatusSpy.at(0).at(1).value<QOpcUaMonitoringParameters::Parameters>() & QOpcUaMonitoringParameters::Parameter::SamplingInterval);

    monitoringStatusSpy.clear();
    doubleNode->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::DiscardOldest, true);
    monitoringStatusSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringStatusSpy.size(), 1);
    QCOMPARE(monitoringStatusSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(monitoringStatusSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QVERIFY(monitoringStatusSpy.at(0).at(1).value<QOpcUaMonitoringParameters::Parameters>() & QOpcUaMonitoringParameters::Parameter::DiscardOldest);

    monitoringStatusSpy.clear();
    doubleNode->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::QueueSize, quint32(42));
    monitoringStatusSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringStatusSpy.size(), 1);
    QCOMPARE(monitoringStatusSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(monitoringStatusSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QVERIFY(monitoringStatusSpy.at(0).at(1).value<QOpcUaMonitoringParameters::Parameters>() & QOpcUaMonitoringParameters::Parameter::QueueSize);

    monitoringStatusSpy.clear();
    doubleNode->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::DiscardOldest, true);
    monitoringStatusSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringStatusSpy.size(), 1);
    QCOMPARE(monitoringStatusSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(monitoringStatusSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QVERIFY(monitoringStatusSpy.at(0).at(1).value<QOpcUaMonitoringParameters::Parameters>() & QOpcUaMonitoringParameters::Parameter::DiscardOldest);

    WRITE_VALUE_ATTRIBUTE(doubleNode, 3.0, QOpcUa::Types::Double);

    dataChangeSpy.wait(signalSpyTimeout);
    QCOMPARE(dataChangeSpy.size(), 1);

    doubleNode->disableMonitoring(QOpcUa::NodeAttribute::Value);
    monitoringDisabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringDisabledSpy.size(), 1);
    QCOMPARE(monitoringDisabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringDisabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
}

void Tst_QOpcUaClient::addDuplicateMonitoredItem()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> doubleNode(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.Double"_s));
    QVERIFY(doubleNode != nullptr);

    QOpcUaMonitoringParameters p(100);

    QSignalSpy monitoringEnabledSpy(doubleNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QSignalSpy monitoringDisabledSpy(doubleNode.data(), &QOpcUaNode::disableMonitoringFinished);

    doubleNode->enableMonitoring(QOpcUa::NodeAttribute::Value, p);
    monitoringEnabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringEnabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(doubleNode->monitoringStatus(QOpcUa::NodeAttribute::Value).statusCode(), QOpcUa::UaStatusCode::Good);

    monitoringEnabledSpy.clear();
    doubleNode->enableMonitoring(QOpcUa::NodeAttribute::Value, p);
    monitoringEnabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringEnabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadEntryExists);
    QCOMPARE(doubleNode->monitoringStatus(QOpcUa::NodeAttribute::Value).statusCode(), QOpcUa::UaStatusCode::Good);

    doubleNode->disableMonitoring(QOpcUa::NodeAttribute::Value);
    monitoringDisabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringDisabledSpy.size(), 1);
    QCOMPARE(monitoringDisabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringDisabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
}

void Tst_QOpcUaClient::subscriptionUnreadableNode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> unreadableNode(opcuaClient->node(u"ns=3;s=VariableWithoutReadPermission"_s));
    QVERIFY(unreadableNode != nullptr);

    QOpcUaMonitoringParameters p(100);
    p.setIndexRange(u"1"_s);
    QSignalSpy monitoringEnabledSpy(unreadableNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QSignalSpy monitoringDisabledSpy(unreadableNode.data(), &QOpcUaNode::disableMonitoringFinished);
    QSignalSpy dataChangeSpy(unreadableNode.data(), &QOpcUaNode::dataChangeOccurred);

    unreadableNode->enableMonitoring(QOpcUa::NodeAttribute::Value, p);
    monitoringEnabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringEnabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

     // Wait for the initial data change
    dataChangeSpy.wait(signalSpyTimeout);
    QCOMPARE(dataChangeSpy.size(), 1);
    QCOMPARE(dataChangeSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(dataChangeSpy.at(0).at(1), QVariant());
    QCOMPARE(unreadableNode->valueAttribute(), QVariant());
    QCOMPARE(unreadableNode->valueAttributeError(), QOpcUa::UaStatusCode::BadNotReadable);

    unreadableNode->disableMonitoring(QOpcUa::NodeAttribute::Value);
    monitoringDisabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringDisabledSpy.size(), 1);
}

void Tst_QOpcUaClient::checkMonitoredItemCleanup()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> readWriteNode(opcuaClient->node(u"ns=3;s=TestNode.ReadWrite"_s));
    QVERIFY(readWriteNode != nullptr);

    QScopedPointer<QOpcUaNode> serverNode(opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Server)));
    QVERIFY(serverNode != nullptr);

    QSignalSpy monitoringEnabledSpy(readWriteNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QOpcUa::NodeAttributes attr = QOpcUa::NodeAttribute::Value | QOpcUa::NodeAttribute::BrowseName;
    readWriteNode->enableMonitoring(attr, QOpcUaMonitoringParameters(100, QOpcUaMonitoringParameters::SubscriptionType::Exclusive));
    monitoringEnabledSpy.wait(signalSpyTimeout);
    if (monitoringEnabledSpy.size() != 2)
        monitoringEnabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringEnabledSpy.size(), 2);

    for (const auto &entry : monitoringEnabledSpy) {
        QVERIFY(attr & entry.at(0).value<QOpcUa::NodeAttribute>());
        QCOMPARE(entry.at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    }

    quint32 subscriptionId = readWriteNode->monitoringStatus(QOpcUa::NodeAttribute::Value).subscriptionId();
    QCOMPARE(subscriptionId, readWriteNode->monitoringStatus(QOpcUa::NodeAttribute::BrowseName).subscriptionId());

    QSignalSpy methodSpy(serverNode.data(), &QOpcUaNode::methodCallFinished);
    QList<QOpcUa::TypedVariant> parameter;
    parameter.append(QOpcUa::TypedVariant(QVariant(quint32(subscriptionId)), QOpcUa::Types::UInt32));
    serverNode->callMethod(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Server_GetMonitoredItems), parameter);

    methodSpy.wait(signalSpyTimeout);
    QCOMPARE(methodSpy.size(), 1);

    for (const auto &entry : methodSpy) {
        QCOMPARE(entry.at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
        QCOMPARE(entry.at(1).toList().size(), 2); // Two monitored items
        QCOMPARE(entry.at(1).toList().at(0).toList().size(), 2); // One server handle for each monitored item
        QCOMPARE(entry.at(1).toList().at(1).toList().size(), 2); // One client handle for each monitored item
    }

    readWriteNode.reset(); // Delete the node object

    bool success = false;
    for (int i = 0; i < 15; ++i) {
        methodSpy.clear();
        serverNode->callMethod(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Server_GetMonitoredItems), parameter);
        methodSpy.wait(signalSpyTimeout);
        QCOMPARE(methodSpy.size(), 1);

        if (methodSpy.at(0).at(2).value<QOpcUa::UaStatusCode>() != QOpcUa::UaStatusCode::BadSubscriptionIdInvalid) {
            QTest::qWait(100);
            continue;
        }

        QCOMPARE(methodSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadSubscriptionIdInvalid);
        success = true;
        break;
    }

    QVERIFY(success);
}

void Tst_QOpcUaClient::checkAttributeUpdated()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=3;s=TestNode.ReadWrite"_s));
    QVERIFY(node != nullptr);

    QSignalSpy spy(node.data(), &QOpcUaNode::attributeUpdated);

    node->readAttributes(QOpcUa::NodeAttribute::Value);
    spy.wait(signalSpyTimeout);
    QCOMPARE(spy.size(), 1);

    node->writeValueAttribute(23.0, QOpcUa::Types::Double);
    spy.wait(signalSpyTimeout);
    QCOMPARE(spy.size(), 2);

    node->enableMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters(100));
    spy.wait(signalSpyTimeout);
    QCOMPARE(spy.size(), 3);

    for (const auto &it : spy) {
        QCOMPARE(it.at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
        QVERIFY(it.at(1).isValid());
    }
}

void Tst_QOpcUaClient::checkValueAttributeUpdated()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=3;s=TestNode.ReadWrite"_s));
    QVERIFY(node != nullptr);

    QSignalSpy spy(node.data(), &QOpcUaNode::valueAttributeUpdated);

    node->writeValueAttribute(23.0, QOpcUa::Types::Double);
    spy.wait(signalSpyTimeout);
    QCOMPARE(spy.size(), 1);
    spy.clear();

    node->readAttributes(QOpcUa::NodeAttribute::Value);
    spy.wait(signalSpyTimeout);
    QCOMPARE(spy.size(), 1);

    node->writeValueAttribute(23.0, QOpcUa::Types::Double);
    spy.wait(signalSpyTimeout);
    QCOMPARE(spy.size(), 2);

    node->enableMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters(100));
    spy.wait(signalSpyTimeout);
    QCOMPARE(spy.size(), 3);

    QCOMPARE(spy.at(0).at(0), 23.0);
    QCOMPARE(spy.at(1).at(0), 23.0);
    QCOMPARE(spy.at(2).at(0), 23.0);
}

void Tst_QOpcUaClient::checkMonitoringInvalidRequests()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=3;s=TestNode.ReadWrite"_s));
    QVERIFY(node != nullptr);

    QSignalSpy enableMonitoringSpy(node.get(), &QOpcUaNode::enableMonitoringFinished);

    // Explicit subscription id without existing subscription
    QOpcUaMonitoringParameters param(200);
    param.setSubscriptionId(42);
    node->enableMonitoring(QOpcUa::NodeAttribute::Value, param);
    enableMonitoringSpy.wait(signalSpyTimeout);
    QCOMPARE(enableMonitoringSpy.size(), 1);
    QCOMPARE(enableMonitoringSpy.at(0).at(1), QOpcUa::UaStatusCode::BadSubscriptionIdInvalid);

    // Attempt to modify a monitoring that doesn't exist
    QSignalSpy monitoringChangedSpy(node.get(), &QOpcUaNode::monitoringStatusChanged);
    node->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::QueueSize, 23);
    monitoringChangedSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringChangedSpy.size(), 1);
    qDebug() << monitoringChangedSpy;
    QCOMPARE(monitoringChangedSpy.at(0).at(2), QOpcUa::UaStatusCode::BadMonitoredItemIdInvalid);

    QSignalSpy disableMonitoringSpy(node.get(), &QOpcUaNode::disableMonitoringFinished);
    node->disableMonitoring(QOpcUa::NodeAttribute::Value);
    disableMonitoringSpy.wait(signalSpyTimeout);
    QCOMPARE(disableMonitoringSpy.size(), 1);
    QCOMPARE(disableMonitoringSpy.at(0).at(1), QOpcUa::UaStatusCode::BadMonitoredItemIdInvalid);
}

void Tst_QOpcUaClient::stringCharset()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> stringScalarNode(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.String"_s));
    QScopedPointer<QOpcUaNode> localizedScalarNode(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.LocalizedText"_s));
    QScopedPointer<QOpcUaNode> stringArrayNode(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.String"_s));
    QScopedPointer<QOpcUaNode> localizedArrayNode(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.LocalizedText"_s));

    QVERIFY(stringScalarNode != nullptr);
    QVERIFY(localizedScalarNode != nullptr);
    QVERIFY(stringArrayNode != nullptr);
    QVERIFY(localizedArrayNode != nullptr);

    QString testString = u"🞀🞁🞂🞃"_s;
    QOpcUaLocalizedText lt1(u"en"_s, testString);
    QOpcUaLocalizedText lt2(u"de"_s, testString);

    WRITE_VALUE_ATTRIBUTE(stringScalarNode, testString, QOpcUa::String);
    WRITE_VALUE_ATTRIBUTE(localizedScalarNode, localizedTexts[0], QOpcUa::LocalizedText);

    QVariantList l;
    l.append(testString);
    l.append(testString);

    WRITE_VALUE_ATTRIBUTE(stringArrayNode, l, QOpcUa::String);

    l.clear();
    l.append(lt1);
    l.append(lt2);

    WRITE_VALUE_ATTRIBUTE(localizedArrayNode, l, QOpcUa::LocalizedText);

    READ_MANDATORY_VARIABLE_NODE(stringArrayNode);
    READ_MANDATORY_VARIABLE_NODE(localizedArrayNode);
    READ_MANDATORY_VARIABLE_NODE(stringScalarNode);
    READ_MANDATORY_VARIABLE_NODE(localizedScalarNode);

    QVariant result = stringScalarNode->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(result.toString(), testString);
    result = localizedScalarNode->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(result.value<QOpcUaLocalizedText>(), localizedTexts[0]);

    result = stringArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(result.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(result.toList().size(), 2);
    QCOMPARE(result.toList()[0].metaType().id(), QMetaType::QString);
    QCOMPARE(result.toList()[0].toString(), testString);
    QCOMPARE(result.toList()[1].metaType().id(), QMetaType::QString);
    QCOMPARE(result.toList()[1].toString(), testString);

    result = localizedArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(result.metaType().id(), QMetaType::QVariantList);
    QCOMPARE(result.toList().size(), 2);
    QCOMPARE(result.toList()[0].value<QOpcUaLocalizedText>(), lt1);
    QCOMPARE(result.toList()[1].value<QOpcUaLocalizedText>(), lt2);
}

void Tst_QOpcUaClient::namespaceArray()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    QSignalSpy spy(opcuaClient, &QOpcUaClient::namespaceArrayUpdated);

    OpcuaConnector connector(opcuaClient, m_endpoint);

    // Catch the initial update after connect
    if (spy.empty())
        spy.wait(signalSpyTimeout);
    QCOMPARE(spy.size(), 1);
    spy.clear();

    QCOMPARE(opcuaClient->updateNamespaceArray(), true);

    spy.wait(signalSpyTimeout);
    QCOMPARE(spy.size(), 1);

    QStringList namespaces = opcuaClient->namespaceArray();
    QCOMPARE(namespaces.size(), 5);

    int nsIndex = namespaces.indexOf("http://qt-project.org"_L1);
    QVERIFY(nsIndex > 0);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(QOpcUa::nodeIdFromString(nsIndex, u"Demo.Static.Scalar.String"_s)));
    READ_MANDATORY_BASE_NODE(node);

    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUaLocalizedText>().text(), u"StringScalarTest"_s);
}

void Tst_QOpcUaClient::multiDimensionalArray()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=Demo.Static.Arrays.MultiDimensionalDouble"_s));
    QVERIFY(node != nullptr);

    QList<quint32> arrayDimensions({2, 2, 3});
    QVariantList value({0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0});
    QOpcUaMultiDimensionalArray arr(value, arrayDimensions);
    QVERIFY(arr.isValid());
    WRITE_VALUE_ATTRIBUTE(node, arr , QOpcUa::Double);
    READ_MANDATORY_VARIABLE_NODE(node);

    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::Value), QOpcUa::UaStatusCode::Good);
    QOpcUaMultiDimensionalArray readBack = node->attribute(QOpcUa::NodeAttribute::Value).value<QOpcUaMultiDimensionalArray>();

    QVERIFY(readBack.isValid());
    QCOMPARE(readBack.value({0, 0, 0}), 0.0);
    QCOMPARE(readBack.value({0, 0, 1}), 1.0);
    QCOMPARE(readBack.value({0, 0, 2}), 2.0);
    QCOMPARE(readBack.value({0, 1, 0}), 3.0);
    QCOMPARE(readBack.value({0, 1, 1}), 4.0);
    QCOMPARE(readBack.value({0, 1, 2}), 5.0);
    QCOMPARE(readBack.value({1, 0, 0}), 6.0);
    QCOMPARE(readBack.value({1, 0, 1}), 7.0);
    QCOMPARE(readBack.value({1, 0, 2}), 8.0);
    QCOMPARE(readBack.value({1, 1, 0}), 9.0);
    QCOMPARE(readBack.value({1, 1, 1}), 10.0);
    QCOMPARE(readBack.value({1, 1, 2}), 11.0);

    QCOMPARE(arr, readBack);
}

void Tst_QOpcUaClient::dateTimeConversion()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> dateTimeScalarNode(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.DateTime"_s));

    QVERIFY(dateTimeScalarNode != nullptr);

    QDateTime dt = QDateTime::currentDateTime();

    WRITE_VALUE_ATTRIBUTE(dateTimeScalarNode, dt, QOpcUa::Types::DateTime);
    READ_MANDATORY_VARIABLE_NODE(dateTimeScalarNode);

    QDateTime result = dateTimeScalarNode->attribute(QOpcUa::NodeAttribute::Value).toDateTime();
    QCOMPARE(dt, result);
}

void Tst_QOpcUaClient::timeStamps()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> stringScalarNode(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.String"_s));

    QVERIFY(stringScalarNode != nullptr);

    QCOMPARE(stringScalarNode->sourceTimestamp(QOpcUa::NodeAttribute::Value).isValid(), false);
    QCOMPARE(stringScalarNode->serverTimestamp(QOpcUa::NodeAttribute::Value).isValid(), false);

    READ_MANDATORY_VARIABLE_NODE(stringScalarNode);

    const QDateTime sourceRead = stringScalarNode->sourceTimestamp(QOpcUa::NodeAttribute::Value);
    const QDateTime serverRead = stringScalarNode->serverTimestamp(QOpcUa::NodeAttribute::Value);

    QVERIFY(sourceRead.isValid());
    QVERIFY(serverRead.isValid());

    QOpcUaMonitoringParameters p(100);
    QSignalSpy monitoringEnabledSpy(stringScalarNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QSignalSpy monitoringDisabledSpy(stringScalarNode.data(), &QOpcUaNode::disableMonitoringFinished);
    QSignalSpy dataChangeSpy(stringScalarNode.data(), &QOpcUaNode::dataChangeOccurred);

    QTest::qWait(10); // Make sure the timestamp has a chance to change

    WRITE_VALUE_ATTRIBUTE(stringScalarNode, u"Reset"_s, QOpcUa::Types::String);

    stringScalarNode->enableMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters(100));

    monitoringEnabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringEnabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    dataChangeSpy.wait(signalSpyTimeout);
    const QDateTime sourceDataChange = stringScalarNode->sourceTimestamp(QOpcUa::NodeAttribute::Value);
    const QDateTime serverDataChange = stringScalarNode->serverTimestamp(QOpcUa::NodeAttribute::Value);

    QVERIFY(sourceDataChange.isValid());
    QVERIFY(serverDataChange.isValid());
    QVERIFY(sourceRead < sourceDataChange);
    QVERIFY(serverRead < serverDataChange);

    stringScalarNode->disableMonitoring(QOpcUa::NodeAttribute::Value);
    monitoringDisabledSpy.wait(signalSpyTimeout);
    QCOMPARE(monitoringDisabledSpy.size(), 1);
    QCOMPARE(monitoringDisabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringDisabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
}

void Tst_QOpcUaClient::createNodeFromExpandedId()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QSignalSpy updateSpy(opcuaClient, &QOpcUaClient::namespaceArrayUpdated);
    opcuaClient->updateNamespaceArray();
    updateSpy.wait(signalSpyTimeout);
    QVERIFY(updateSpy.size() > 0);
    QVERIFY(opcuaClient->namespaceArray().size() > 0);

    // Node on a remote server, nullptr expected
    QOpcUaExpandedNodeId id;
    id.setNodeId(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::RootFolder));
    id.setServerIndex(1);
    QScopedPointer<QOpcUaNode> node(opcuaClient->node(id));
    QVERIFY(node == nullptr);

    // Root node on the local server, valid pointer expected
    id.setServerIndex(0);
    node.reset(opcuaClient->node(id));
    QVERIFY(node != nullptr);
    READ_MANDATORY_BASE_NODE(node);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>(), QOpcUaQualifiedName(0, u"Root"_s));

    // Successful namespace substitution, valid pointer expected
    id.setNodeId(u"ns=0;s=TestNode.ReadWrite"_s);
    id.setNamespaceUri(u"Test Namespace"_s);
    node.reset(opcuaClient->node(id));
    QVERIFY(node != nullptr);
    QCOMPARE(node->nodeId(), u"ns=3;s=TestNode.ReadWrite"_s);
    READ_MANDATORY_BASE_NODE(node);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>(),
             QOpcUaQualifiedName(3, u"TestNode.ReadWrite"_s));

    // Invalid namespace, nullptr expected
    id.setNodeId(u"ns=0;s=TestNode.ReadWrite"_s);
    id.setNamespaceUri(u"InvalidNamespace"_s);
    node.reset(opcuaClient->node(id));
    QVERIFY(node == nullptr);
}

void Tst_QOpcUaClient::checkExpandedIdConversion()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    // Before the namespace array is populated, error expected
    bool ok = true;
    QOpcUaExpandedNodeId id;
    id.setNodeId(u"ns=0;i=84"_s);
    id.setNamespaceUri(u"MyNameSpace"_s);
    QString result = opcuaClient->resolveExpandedNodeId(id, &ok);
    QVERIFY(ok == false);
    QCOMPARE(result, QString());
    id.setNamespaceUri(QString());

    QSignalSpy updateSpy(opcuaClient, &QOpcUaClient::namespaceArrayUpdated);

    OpcuaConnector connector(opcuaClient, m_endpoint);

    if (updateSpy.empty())
        updateSpy.wait(signalSpyTimeout);
    QVERIFY(updateSpy.size() > 0);
    QVERIFY(!opcuaClient->namespaceArray().isEmpty());

    // Root node on the local server, valid string expected
    id.setServerIndex(0);
    result = opcuaClient->resolveExpandedNodeId(id, &ok);
    QVERIFY(ok == true);
    QCOMPARE(result, u"ns=0;i=84"_s);

    // Successful namespace substitution, valid string expected
    ok = false;
    id.setNodeId(u"ns=0;s=TestNode.ReadWrite"_s);
    id.setNamespaceUri(u"Test Namespace"_s);
    result = opcuaClient->resolveExpandedNodeId(id, &ok);
    QVERIFY(ok == true);
    QCOMPARE(result, u"ns=3;s=TestNode.ReadWrite"_s);

    // Invalid namespace, empty string expected
    id.setNodeId(u"ns=0;s=TestNode.ReadWrite"_s);
    id.setNamespaceUri(u"InvalidNamespace"_s);
    result = opcuaClient->resolveExpandedNodeId(id, &ok);
    QVERIFY(ok == false);
    QCOMPARE(result, QString());

    // Malformed node id string, empty string expected
    ok = true;
    id.setNodeId(u"ns=0,s=TestNode.ReadWrite"_s);
    result = opcuaClient->resolveExpandedNodeId(id, &ok);
    QVERIFY(ok == false);
    QCOMPARE(result, QString());

    // Resolving id on different server is expected to fail
    ok = false;
    id.setNodeId(u"ns=0;s=TestNode.ReadWrite"_s);
    id.setNamespaceUri(u"Test Namespace"_s);
    id.setServerIndex(42);
    result = opcuaClient->resolveExpandedNodeId(id, &ok);
    QVERIFY(ok == false);

    // Invalid node id is expected to fail
    ok = false;
    id.setNodeId(u"xxxxxxxxxxx"_s);
    id.setServerIndex(0);
    id.setNamespaceUri(u"Test Namespace"_s);
    result = opcuaClient->resolveExpandedNodeId(id, &ok);
    QVERIFY(ok == false);
}

void Tst_QOpcUaClient::checkExpandedIdConversionNoOk()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    // Before the namespace array is populated, empty string expected
    QOpcUaExpandedNodeId id;
    id.setNodeId(u"ns=0;i=84"_s);
    id.setNamespaceUri(u"MyNameSpace"_s);
    QString result = opcuaClient->resolveExpandedNodeId(id);
    QCOMPARE(result, QString());
    id.setNamespaceUri(QString());

    QSignalSpy updateSpy(opcuaClient, &QOpcUaClient::namespaceArrayUpdated);

    OpcuaConnector connector(opcuaClient, m_endpoint);

    if (updateSpy.empty())
        updateSpy.wait(signalSpyTimeout);
    QVERIFY(updateSpy.size() > 0);
    QVERIFY(!opcuaClient->namespaceArray().isEmpty());

    // Root node on the local server, valid string expected
    id.setServerIndex(0);
    result = opcuaClient->resolveExpandedNodeId(id);
    QCOMPARE(result, u"ns=0;i=84"_s);

    // Successful namespace substitution, valid string expected
    id.setNodeId(u"ns=0;s=TestNode.ReadWrite"_s);
    id.setNamespaceUri(u"Test Namespace"_s);
    result = opcuaClient->resolveExpandedNodeId(id);
    QCOMPARE(result, u"ns=3;s=TestNode.ReadWrite"_s);

    // Invalid namespace, empty string expected
    id.setNodeId(u"ns=0;s=TestNode.ReadWrite"_s);
    id.setNamespaceUri(u"InvalidNamespace"_s);
    result = opcuaClient->resolveExpandedNodeId(id);
    QCOMPARE(result, QString());

    // Malformed node id string, empty string expected
    id.setNodeId(u"ns=0,s=TestNode.ReadWrite"_s);
    result = opcuaClient->resolveExpandedNodeId(id);
    QCOMPARE(result, QString());

    // Resolving id on different server is expected to fail
    id.setNodeId(u"ns=0;s=TestNode.ReadWrite"_s);
    id.setNamespaceUri(u"Test Namespace"_s);
    id.setServerIndex(42);
    result = opcuaClient->resolveExpandedNodeId(id);
    QCOMPARE(result, QString());

    // Invalid node id is expected to fail
    id.setNodeId(u"xxxxxxxxxxx"_s);
    id.setNamespaceUri(u"Test Namespace"_s);
    id.setServerIndex(0);
    result = opcuaClient->resolveExpandedNodeId(id);
    QCOMPARE(result, QString());
}

void Tst_QOpcUaClient::createQualifiedName()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    // Before the namespace array is populated, empty qualified name expected
    bool ok = true;
    QOpcUaQualifiedName name = opcuaClient->qualifiedNameFromNamespaceUri(u"Test Namespace"_s, u"Name"_s, &ok);
    QVERIFY(ok == false);
    QCOMPARE(name, QOpcUaQualifiedName());

    QSignalSpy updateSpy(opcuaClient, &QOpcUaClient::namespaceArrayUpdated);

    OpcuaConnector connector(opcuaClient, m_endpoint);

    if (updateSpy.empty())
        updateSpy.wait(signalSpyTimeout);
    QVERIFY(updateSpy.size() > 0);
    QVERIFY(!opcuaClient->namespaceArray().isEmpty());

    // Valid namespace, valid qualified name expected
    name = opcuaClient->qualifiedNameFromNamespaceUri(u"Test Namespace"_s, u"Name"_s, &ok);
    QVERIFY(ok == true);
    QCOMPARE(name, QOpcUaQualifiedName(3, u"Name"_s));

    // Invalid namespace, empty qualified name expected
    name = opcuaClient->qualifiedNameFromNamespaceUri(u"InvalidNamespace"_s, u"Name"_s, &ok);
    QVERIFY(ok == false);
    QCOMPARE(name, QOpcUaQualifiedName());
}

void Tst_QOpcUaClient::createQualifiedNameNoOk()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    // Before the namespace array is populated, empty qualified name expected
    QOpcUaQualifiedName name = opcuaClient->qualifiedNameFromNamespaceUri(u"Test Namespace"_s, u"Name"_s);
    QCOMPARE(name, QOpcUaQualifiedName());

    QSignalSpy updateSpy(opcuaClient, &QOpcUaClient::namespaceArrayUpdated);

    OpcuaConnector connector(opcuaClient, m_endpoint);

    if (updateSpy.empty())
        updateSpy.wait(signalSpyTimeout);
    QVERIFY(updateSpy.size() > 0);
    QVERIFY(!opcuaClient->namespaceArray().isEmpty());

    // Valid namespace, valid qualified name expected
    name = opcuaClient->qualifiedNameFromNamespaceUri(u"Test Namespace"_s, u"Name"_s);
    QCOMPARE(name, QOpcUaQualifiedName(3, u"Name"_s));

    // Invalid namespace, empty qualified name expected
    name = opcuaClient->qualifiedNameFromNamespaceUri(u"InvalidNamespace"_s, u"Name"_s);
    QCOMPARE(name, QOpcUaQualifiedName());
}

void Tst_QOpcUaClient::resolveBrowsePath()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> typesNode(opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::TypesFolder)));
    QVERIFY(typesNode != nullptr);

    QSignalSpy spy(typesNode.data(), &QOpcUaNode::resolveBrowsePathFinished);

    QList<QOpcUaRelativePathElement> path;
    const QString referenceTypeId = QOpcUa::nodeIdFromReferenceType(QOpcUa::ReferenceTypeId::Organizes);
    path.append(QOpcUaRelativePathElement(QOpcUaQualifiedName(0, u"DataTypes"_s), referenceTypeId));
    path.append(QOpcUaRelativePathElement(QOpcUaQualifiedName(0, u"BaseDataType"_s), referenceTypeId));
    bool success = typesNode->resolveBrowsePath(path);
    QVERIFY(success == true);

    spy.wait(signalSpyTimeout);
    QCOMPARE(spy.size(), 1);
    const auto results = spy.at(0).at(0).value<QList<QOpcUaBrowsePathTarget>>();
    QCOMPARE(results.size(), 1);
    QCOMPARE(results.at(0).remainingPathIndex(), (std::numeric_limits<quint32>::max)());
    QCOMPARE(results.at(0).targetId().nodeId(), QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::BaseDataType));
    QVERIFY(results.at(0).targetId().namespaceUri().isEmpty());
    QCOMPARE(results.at(0).isFullyResolved(), true);
    QCOMPARE(results.at(0).targetId().serverIndex(), 0U);
    QCOMPARE(spy.at(0).at(1).value<QList<QOpcUaRelativePathElement>>(), path);
    QCOMPARE(spy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
}

void Tst_QOpcUaClient::extensionObjectWithGuid()
{
    const QByteArray uuidWireData = QByteArray::fromHex("f827ce6cbeb61f48a5a888fd2bbc4fb7");
    const QByteArray opcuaWireData = QByteArray::fromHex("040400") + uuidWireData;
    const QString uuidId = u"6cce27f8-b6be-481f-a5a8-88fd2bbc4fb7"_s;
    const QString sampleNodeId = u"ns=4;g="_s + uuidId;

    const auto uuid = QUuid::fromString(uuidId);
    QCOMPARE(uuid.toString(QUuid::WithoutBraces), uuidId);

    QOpcUaExtensionObject obj;
    obj.setEncoding(QOpcUaExtensionObject::Encoding::ByteString);
    obj.setEncodingTypeId(u"ns=2;s=MyEncoding1"_s);
    QOpcUaBinaryDataEncoding encoder(obj);
    encoder.encode<QString, QOpcUa::Types::NodeId>(sampleNodeId);
    QCOMPARE(obj.encodedBody().toHex(), opcuaWireData.toHex());

    QByteArray buffer = obj.encodedBody();
    QOpcUaBinaryDataEncoding decoder(&buffer);

    bool success = false;
    const auto decodedNodeId = decoder.decode<QString, QOpcUa::Types::NodeId>(success);
    QVERIFY(success);
    QCOMPARE(decodedNodeId, sampleNodeId);
}

void Tst_QOpcUaClient::encodeEmptyStringNodeId()
{
    QByteArray data;
    QOpcUaBinaryDataEncoding encoder(&data);

    auto success = encoder.encode<QString, QOpcUa::Types::NodeId>(QString());
    QCOMPARE(success, true);

    const auto result = encoder.decode<QString, QOpcUa::Types::NodeId>(success);
    QCOMPARE(success, true);
    QCOMPARE(result, u"ns=0;i=0"_s);
}

void Tst_QOpcUaClient::statusStrings()
{
    QCOMPARE(statusToString(QOpcUa::Good), u"Good"_s);
    QCOMPARE(statusToString(QOpcUa::BadAggregateConfigurationRejected), u"BadAggregateConfigurationRejected"_s);
}

void Tst_QOpcUaClient::addNamespace()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    QSignalSpy namespaceUpdatedSpy(opcuaClient, &QOpcUaClient::namespaceArrayUpdated);
    QSignalSpy namespaceChangedSpy(opcuaClient, &QOpcUaClient::namespaceArrayChanged);

    OpcuaConnector connector(opcuaClient, m_endpoint);

    opcuaClient->updateNamespaceArray();
    namespaceUpdatedSpy.wait(signalSpyTimeout);
    if (namespaceUpdatedSpy.size() != 2)
        namespaceUpdatedSpy.wait(signalSpyTimeout);
    QVERIFY(namespaceUpdatedSpy.size() > 0);
    QCOMPARE(namespaceChangedSpy.size(), 1);

    // Update second time: No change signal emitted
    namespaceChangedSpy.clear();
    namespaceUpdatedSpy.clear();
    opcuaClient->updateNamespaceArray();
    namespaceUpdatedSpy.wait(signalSpyTimeout);
    // The signal is emitted roughly at the same time
    // as namespaceArrayUpdated()
    namespaceChangedSpy.wait(100);

    QCOMPARE(namespaceUpdatedSpy.size(), 1);
    QCOMPARE(namespaceChangedSpy.size(), 0);

    auto namespaceArray = opcuaClient->namespaceArray();
    QString newNamespaceName = u"DynamicTestNamespace#%1"_s.arg(namespaceArray.size());

    QVERIFY(!namespaceArray.isEmpty());
    QVERIFY(!namespaceArray.contains(newNamespaceName));

    QList<QOpcUa::TypedVariant> args;
    args.push_back(QOpcUa::TypedVariant(newNamespaceName, QOpcUa::String));

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Server)));
    QVERIFY(node != nullptr);

    QSignalSpy methodSpy(node.data(), &QOpcUaNode::methodCallFinished);

    opcuaClient->setNamespaceAutoupdate(true);
    namespaceUpdatedSpy.clear();
    namespaceChangedSpy.clear();

    bool success = node->callMethod(u"ns=3;s=Test.Method.AddNamespace"_s, args);
    QVERIFY(success == true);

    methodSpy.wait(signalSpyTimeout);

    QCOMPARE(methodSpy.size(), 1);
    QCOMPARE(methodSpy.at(0).at(0).value<QString>(), u"ns=3;s=Test.Method.AddNamespace"_s);
    QCOMPARE(methodSpy.at(0).at(1).value<quint16>(), namespaceArray.size());
    QCOMPARE(QOpcUa::isSuccessStatus(methodSpy.at(0).at(2).value<QOpcUa::UaStatusCode>()), true);

    // Do not call updateNamespaceArray()
    if (namespaceChangedSpy.isEmpty())
        namespaceChangedSpy.wait(signalSpyTimeout);

    QVERIFY(namespaceUpdatedSpy.size() > 0);
    QCOMPARE(namespaceChangedSpy.size(), 1);
    auto updatedNamespaceArray = opcuaClient->namespaceArray();
    QCOMPARE(updatedNamespaceArray.size(), namespaceArray.size() + 1);
    QVERIFY(updatedNamespaceArray.contains(newNamespaceName));
    QCOMPARE(methodSpy.at(0).at(1).value<quint16>(), updatedNamespaceArray.indexOf(newNamespaceName));
}

void Tst_QOpcUaClient::fixedTimestamp()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=Demo.Static.FixedTimestamp"_s));
    QVERIFY(node != nullptr);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant value = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(value.isValid());
    QCOMPARE(value.metaType().id(), QMetaType::QDateTime);
    QCOMPARE(value.toDateTime(), QDateTime(QDate(2012, 12, 19), QTime(13, 37)));
}

void Tst_QOpcUaClient::eventSubscription()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> serverNode(opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Server)));
    QVERIFY(serverNode != nullptr);

    QScopedPointer<QOpcUaNode> serverNode2(opcuaClient->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Server)));
    QVERIFY(serverNode2 != nullptr);

    QScopedPointer<QOpcUaNode> testFolderNode(opcuaClient->node(u"ns=3;s=TestFolder"_s));
    QVERIFY(testFolderNode != nullptr);

    QSignalSpy enabledSpy(serverNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QSignalSpy enabledSpy2(serverNode2.data(), &QOpcUaNode::enableMonitoringFinished);
    QSignalSpy eventSpy(serverNode.data(), &QOpcUaNode::eventOccurred);
    QSignalSpy eventSpy2(serverNode2.data(), &QOpcUaNode::eventOccurred);

    QOpcUaMonitoringParameters::EventFilter filter;
    filter << QOpcUaSimpleAttributeOperand(u"Severity"_s);
    filter << QOpcUaSimpleAttributeOperand(u"Message"_s);

    QOpcUaMonitoringParameters p(0);
    p.setQueueSize(10); // Without setting the queue size, we get a queue overflow event after triggering both events without waiting
    p.setFilter(filter);

    serverNode->enableMonitoring(QOpcUa::NodeAttribute::EventNotifier, p);
    enabledSpy.wait();
    QCOMPARE(enabledSpy.size(), 1);
    QCOMPARE(enabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::EventNotifier);
    QCOMPARE(enabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    QCOMPARE(serverNode->monitoringStatus(QOpcUa::NodeAttribute::EventNotifier).filter().value<QOpcUaMonitoringParameters::EventFilter>(),
             filter);

    // Add a second monitoring with where clause
    QOpcUaMonitoringParameters::EventFilter filterWithWhereClause = filter;

    // Only events with severity >= 700
    QOpcUaContentFilterElement whereElement;
    whereElement << QOpcUaContentFilterElement::FilterOperator::GreaterThanOrEqual << QOpcUaSimpleAttributeOperand(u"Severity"_s) <<
        QOpcUaLiteralOperand(quint16(700), QOpcUa::Types::UInt16);
    filterWithWhereClause << whereElement;
    p.setFilter(filterWithWhereClause);

    serverNode2->enableMonitoring(QOpcUa::NodeAttribute::EventNotifier, p);
    enabledSpy2.wait();
    QCOMPARE(enabledSpy2.size(), 1);
    QCOMPARE(enabledSpy2.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::EventNotifier);
    QCOMPARE(enabledSpy2.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    QCOMPARE(serverNode2->monitoringStatus(QOpcUa::NodeAttribute::EventNotifier).filter().value<QOpcUaMonitoringParameters::EventFilter>(),
             filterWithWhereClause);

//    Disabled because the open62541 server does not currently return an EventFilterResult
//    QVERIFY(serverNode->monitoringStatus(QOpcUa::NodeAttribute::EventNotifier).filterResult().isValid());
//    QOpcUaEventFilterResult res = serverNode->monitoringStatus(QOpcUa::NodeAttribute::EventNotifier).filterResult().value<QOpcUaEventFilterResult>();
//    QVERIFY(res.isGood() == true);

    testFolderNode->callMethod(u"ns=2;s=TriggerEvent"_s, { QOpcUa::TypedVariant(750, QOpcUa::Types::UInt16) }); // Trigger event over the severity threshold
    testFolderNode->callMethod(u"ns=2;s=TriggerEvent"_s, { QOpcUa::TypedVariant(699, QOpcUa::Types::UInt16) }); // Trigger event below the severity threshold

    eventSpy.wait();
    if (eventSpy.size() != 2)
        eventSpy.wait();

    if (eventSpy2.size() < 1)
        eventSpy2.wait();
    if (eventSpy2.size() < 1)
        eventSpy2.wait();

    QCOMPARE(eventSpy.size(), 2);
    QCOMPARE(eventSpy.at(0).at(0).toList().size(), 2);

    const auto expectedMessage = QOpcUaLocalizedText(u"en"_s, u"An event has been generated"_s);

    QCOMPARE(eventSpy.at(0).at(0).toList().at(0).value<quint16>(), 750);
    QCOMPARE(eventSpy.at(0).at(0).toList().at(1).value<QOpcUaLocalizedText>(), expectedMessage);

    QCOMPARE(eventSpy.at(1).at(0).toList().size(), 2);
    QCOMPARE(eventSpy.at(1).at(0).toList().at(0).value<quint16>(), 699);
    QCOMPARE(eventSpy.at(1).at(0).toList().at(1).value<QOpcUaLocalizedText>(), expectedMessage);

    QCOMPARE(eventSpy2.size(), 1);
    QCOMPARE(eventSpy2.at(0).at(0).toList().size(), 2);
    QCOMPARE(eventSpy2.at(0).at(0).toList().at(0).value<quint16>(), 750);
    QCOMPARE(eventSpy2.at(0).at(0).toList().at(1).value<QOpcUaLocalizedText>(), expectedMessage);

    eventSpy.clear();

    QSignalSpy modifySpy(serverNode.data(), &QOpcUaNode::monitoringStatusChanged);
    filter << QOpcUaSimpleAttributeOperand(u"SourceNode"_s);
    serverNode->modifyEventFilter(filter);
    modifySpy.wait();
    QCOMPARE(modifySpy.size(), 1);
    QCOMPARE(modifySpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::EventNotifier);
    QVERIFY(modifySpy.at(0).at(1).value<QOpcUaMonitoringParameters::Parameters>().testFlag(QOpcUaMonitoringParameters::Parameter::Filter) == true);
    QCOMPARE(modifySpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(serverNode->monitoringStatus(QOpcUa::NodeAttribute::EventNotifier).filter().value<QOpcUaMonitoringParameters::EventFilter>(),
             filter);

    testFolderNode->callMethod(u"ns=2;s=TriggerEvent"_s, { QOpcUa::TypedVariant(800, QOpcUa::Types::UInt16) }); // Trigger event
    eventSpy.wait();
    QCOMPARE(eventSpy.size(), 1);
    QCOMPARE(eventSpy.at(0).at(0).toList().size(), 3);
    QCOMPARE(eventSpy.at(0).at(0).toList().at(0).value<quint16>(), 800);
    QCOMPARE(eventSpy.at(0).at(0).toList().at(1).value<QOpcUaLocalizedText>(), expectedMessage);
    QCOMPARE(eventSpy.at(0).at(0).toList().at(2).toString(), u"ns=0;i=2253"_s);

    QSignalSpy disabledSpy(serverNode.data(), &QOpcUaNode::disableMonitoringFinished);
    serverNode->disableMonitoring(QOpcUa::NodeAttribute::EventNotifier);
    disabledSpy.wait();
    QCOMPARE(disabledSpy.size(), 1);
    QCOMPARE(disabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::EventNotifier);
    QCOMPARE(disabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    disabledSpy.clear();
    serverNode->disableMonitoring(QOpcUa::NodeAttribute::Value);
    disabledSpy.wait(signalSpyTimeout);
    QCOMPARE(disabledSpy.size(), 1);
    QCOMPARE(disabledSpy.at(0).at(1), QOpcUa::UaStatusCode::BadMonitoredItemIdInvalid);
}

void Tst_QOpcUaClient::readHistoryDataFromNode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=Demo.Static.Historizing1"_s));
    QVERIFY (node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, 0, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(node, 1, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(node, 2, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(node, 3, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(node, 4, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(node, 5, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(node, 6, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(node, 7, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(node, 8, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(node, 9, QOpcUa::Types::Int32);

    QScopedPointer<QOpcUaNode> nodeWithLimit(opcuaClient->node(u"ns=2;s=Demo.Static.Historizing1.ContinuationPoint"_s));
    QVERIFY (nodeWithLimit != nullptr);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 0, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 1, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 2, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 3, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 4, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 5, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 6, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 7, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 8, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 9, QOpcUa::Types::Int32);
    QTest::qWait(1);

    // Due to extensive optimizations, using currentDateTime() without adding 1 ms sometimes doesn't get
    // all values because of the 100 ns resolution of the OPC UA timestamp used by the server.

    // All available data in chronological order
    {
        QScopedPointer<QOpcUaHistoryReadResponse> response(node->readHistoryRaw(QDateTime::currentDateTime(),
                                                                                QDateTime::currentDateTime().addDays(-1),
                                                                                15, false, QOpcUa::TimestampsToReturn::Server));

        QVERIFY(response != nullptr);

        QSignalSpy readHistoryDataSpy(response.get(), &QOpcUaHistoryReadResponse::readHistoryDataFinished);
        readHistoryDataSpy.wait(signalSpyTimeout);

        QCOMPARE(readHistoryDataSpy.size(), 1);
        QCOMPARE(response->hasMoreData(), false);
        QCOMPARE(readHistoryDataSpy.size(), 1);

        QCOMPARE(readHistoryDataSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
        auto result = readHistoryDataSpy.at(0).at(0).value<QList<QOpcUaHistoryData>>();
        QCOMPARE(result.size(), 1);
        QCOMPARE(result[0].statusCode(), QOpcUa::UaStatusCode::Good);
        QCOMPARE(result[0].count(), 10);

        QCOMPARE(result[0].result()[0].value(), 9);
        QCOMPARE(result[0].result()[1].value(), 8);
        QCOMPARE(result[0].result()[2].value(), 7);
        QCOMPARE(result[0].result()[3].value(), 6);
        QCOMPARE(result[0].result()[4].value(), 5);
        QCOMPARE(result[0].result()[5].value(), 4);
        QCOMPARE(result[0].result()[6].value(), 3);
        QCOMPARE(result[0].result()[7].value(), 2);
        QCOMPARE(result[0].result()[8].value(), 1);
        QCOMPARE(result[0].result()[9].value(), 0);
    }

    // All available data in chronological order with continuation point
    {
        QScopedPointer<QOpcUaHistoryReadResponse> response(nodeWithLimit->readHistoryRaw(QDateTime::currentDateTime(),
                                                                                         QDateTime::currentDateTime().addDays(-1),
                                                                                         15, false, QOpcUa::TimestampsToReturn::Server));

        QVERIFY(response != nullptr);
        QCOMPARE(response->state(), QOpcUaHistoryReadResponse::State::Reading);

        QSignalSpy readHistoryDataSpy(response.get(), &QOpcUaHistoryReadResponse::readHistoryDataFinished);
        QSignalSpy responseStateSpy(response.get(), &QOpcUaHistoryReadResponse::stateChanged);
        responseStateSpy.wait(signalSpyTimeout);
        if (readHistoryDataSpy.empty())
            readHistoryDataSpy.wait(signalSpyTimeout);

        QCOMPARE(responseStateSpy.size(), 1);
        QCOMPARE(responseStateSpy.at(0).at(0).value<QOpcUaHistoryReadResponse::State>(),
                 QOpcUaHistoryReadResponse::State::MoreDataAvailable);
        responseStateSpy.clear();

        QCOMPARE(readHistoryDataSpy.size(), 1);

        QCOMPARE(readHistoryDataSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
        auto result = readHistoryDataSpy.at(0).at(0).value<QList<QOpcUaHistoryData>>();
        QCOMPARE(result.size(), 1);
        QCOMPARE(result[0].statusCode(), QOpcUa::UaStatusCode::Good);
        QCOMPARE(result[0].count(), 5);
        QCOMPARE(result[0].result()[0].value(), 9);
        QCOMPARE(result[0].result()[1].value(), 8);
        QCOMPARE(result[0].result()[2].value(), 7);
        QCOMPARE(result[0].result()[3].value(), 6);
        QCOMPARE(result[0].result()[4].value(), 5);

        readHistoryDataSpy.clear();
        QCOMPARE(response->hasMoreData(), true);
        response->readMoreData();
        responseStateSpy.wait(signalSpyTimeout);
        if (readHistoryDataSpy.empty())
            readHistoryDataSpy.wait(signalSpyTimeout);

        QCOMPARE(responseStateSpy.size(), 1);
        QCOMPARE(responseStateSpy.at(0).at(0).value<QOpcUaHistoryReadResponse::State>(),
                 QOpcUaHistoryReadResponse::State::Finished);

        QCOMPARE(readHistoryDataSpy.size(), 1);

        QCOMPARE(readHistoryDataSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
        result = readHistoryDataSpy.at(0).at(0).value<QList<QOpcUaHistoryData>>();
        QCOMPARE(result.size(), 1);
        QCOMPARE(result[0].statusCode(), QOpcUa::UaStatusCode::Good);
        QCOMPARE(result[0].count(), 10);
        QCOMPARE(result[0].result()[0].value(), 9);
        QCOMPARE(result[0].result()[1].value(), 8);
        QCOMPARE(result[0].result()[2].value(), 7);
        QCOMPARE(result[0].result()[3].value(), 6);
        QCOMPARE(result[0].result()[4].value(), 5);
        QCOMPARE(result[0].result()[5].value(), 4);
        QCOMPARE(result[0].result()[6].value(), 3);
        QCOMPARE(result[0].result()[7].value(), 2);
        QCOMPARE(result[0].result()[8].value(), 1);
        QCOMPARE(result[0].result()[9].value(), 0);

        readHistoryDataSpy.clear();

        QCOMPARE(response->hasMoreData(), false);
    }


    // All available data in reverse order
    {
        QScopedPointer<QOpcUaHistoryReadResponse> response(node->readHistoryRaw(QDateTime::currentDateTime().addDays(-1),
                                                                                QDateTime::currentDateTime(), 15, false,
                                                                                QOpcUa::TimestampsToReturn::Server));

        QVERIFY(response != nullptr);

        QSignalSpy readHistoryDataSpy(response.get(), &QOpcUaHistoryReadResponse::readHistoryDataFinished);

        readHistoryDataSpy.wait(signalSpyTimeout);

        QCOMPARE(readHistoryDataSpy.size(), 1);

        QCOMPARE(readHistoryDataSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
        const auto result = readHistoryDataSpy.at(0).at(0).value<QList<QOpcUaHistoryData>>();
        QCOMPARE(result.size(), 1);
        QCOMPARE(result[0].statusCode(), QOpcUa::UaStatusCode::Good);
        QCOMPARE(result[0].count(), 10);
        QCOMPARE(result[0].result()[0].value(), 0);
        QCOMPARE(result[0].result()[1].value(), 1);
        QCOMPARE(result[0].result()[2].value(), 2);
        QCOMPARE(result[0].result()[3].value(), 3);
        QCOMPARE(result[0].result()[4].value(), 4);
        QCOMPARE(result[0].result()[5].value(), 5);
        QCOMPARE(result[0].result()[6].value(), 6);
        QCOMPARE(result[0].result()[7].value(), 7);
        QCOMPARE(result[0].result()[8].value(), 8);
        QCOMPARE(result[0].result()[9].value(), 9);

        QCOMPARE(response->hasMoreData(), false);
    }

    // Empty data set for a time range without data
    {
        QScopedPointer<QOpcUaHistoryReadResponse> response(node->readHistoryRaw(QDateTime::currentDateTime().addDays(-3),
                                                                                QDateTime::currentDateTime().addDays(-2),
                                                                                5, false, QOpcUa::TimestampsToReturn::Server));

        QVERIFY(response != nullptr);
        QSignalSpy readHistoryDataSpy(response.get(), &QOpcUaHistoryReadResponse::readHistoryDataFinished);

        readHistoryDataSpy.wait(signalSpyTimeout);

        QCOMPARE(readHistoryDataSpy.size(), 1);

        QCOMPARE(readHistoryDataSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
        const auto resultNoData = readHistoryDataSpy.at(0).at(0).value<QList<QOpcUaHistoryData>>();
        QCOMPARE(resultNoData.size(), 1);
        QCOMPARE(resultNoData[0].statusCode(), QOpcUa::UaStatusCode::Good);
        QCOMPARE(resultNoData[0].count(), 0);

        QCOMPARE(response->hasMoreData(), false);
    }

    // Only a starting time
    {
        QScopedPointer<QOpcUaHistoryReadResponse> response(node->readHistoryRaw(QDateTime::currentDateTime().addDays(-1),
                                                                                QDateTime(), 15, false, QOpcUa::TimestampsToReturn::Server));

        QVERIFY(response != nullptr);
        QSignalSpy readHistoryDataSpy(response.get(), &QOpcUaHistoryReadResponse::readHistoryDataFinished);

        readHistoryDataSpy.wait(signalSpyTimeout);

        QCOMPARE(readHistoryDataSpy.size(), 1);

        QCOMPARE(readHistoryDataSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
        const auto result = readHistoryDataSpy.at(0).at(0).value<QList<QOpcUaHistoryData>>();
        QCOMPARE(result.size(), 1);
        QCOMPARE(result[0].statusCode(), QOpcUa::UaStatusCode::Good);
        QCOMPARE(result[0].count(), 10);
        QCOMPARE(result[0].result()[0].value(), 0);
        QCOMPARE(result[0].result()[1].value(), 1);
        QCOMPARE(result[0].result()[2].value(), 2);
        QCOMPARE(result[0].result()[3].value(), 3);
        QCOMPARE(result[0].result()[4].value(), 4);
        QCOMPARE(result[0].result()[5].value(), 5);
        QCOMPARE(result[0].result()[6].value(), 6);
        QCOMPARE(result[0].result()[7].value(), 7);
        QCOMPARE(result[0].result()[8].value(), 8);
        QCOMPARE(result[0].result()[9].value(), 9);

        QCOMPARE(response->hasMoreData(), false);
    }


    // Only an end time
    {
        QScopedPointer<QOpcUaHistoryReadResponse> response(node->readHistoryRaw(QDateTime(),
                                                                                QDateTime::currentDateTime().addDays(1), 15, false,
                                                                                QOpcUa::TimestampsToReturn::Server));

        QVERIFY(response != nullptr);
        QSignalSpy readHistoryDataSpy(response.get(), &QOpcUaHistoryReadResponse::readHistoryDataFinished);

        readHistoryDataSpy.wait(signalSpyTimeout);

        QCOMPARE(readHistoryDataSpy.size(), 1);

        QCOMPARE(readHistoryDataSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
        const auto result = readHistoryDataSpy.at(0).at(0).value<QList<QOpcUaHistoryData>>();
        QCOMPARE(result.size(), 1);
        QCOMPARE(result[0].statusCode(), QOpcUa::UaStatusCode::Good);
        QCOMPARE(result[0].count(), 10);
        QCOMPARE(result[0].result()[0].value(), 9);
        QCOMPARE(result[0].result()[1].value(), 8);
        QCOMPARE(result[0].result()[2].value(), 7);
        QCOMPARE(result[0].result()[3].value(), 6);
        QCOMPARE(result[0].result()[4].value(), 5);
        QCOMPARE(result[0].result()[5].value(), 4);
        QCOMPARE(result[0].result()[6].value(), 3);
        QCOMPARE(result[0].result()[7].value(), 2);
        QCOMPARE(result[0].result()[8].value(), 1);
        QCOMPARE(result[0].result()[9].value(), 0);

        QCOMPARE(response->hasMoreData(), false);
    }

    // Return bounds
    {
        QScopedPointer<QOpcUaHistoryReadResponse> response(node->readHistoryRaw(QDateTime::currentDateTime().addDays(-1),
                                                                                QDateTime(), 15, true,
                                                                                QOpcUa::TimestampsToReturn::Server));

        QVERIFY(response != nullptr);
        QSignalSpy readHistoryDataSpy(response.get(), &QOpcUaHistoryReadResponse::readHistoryDataFinished);

        readHistoryDataSpy.wait(signalSpyTimeout);

        QCOMPARE(readHistoryDataSpy.size(), 1);

        QCOMPARE(readHistoryDataSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
        const auto result = readHistoryDataSpy.at(0).at(0).value<QList<QOpcUaHistoryData>>();
        QCOMPARE(result.size(), 1);
        QCOMPARE(result[0].statusCode(), QOpcUa::UaStatusCode::Good);
        QCOMPARE(result[0].count(), 12);
        QCOMPARE(result[0].result()[0].statusCode(), QOpcUa::UaStatusCode::BadBoundNotFound);
        QCOMPARE(result[0].result()[0].value(), QVariant());
        QCOMPARE(result[0].result()[1].value(), 0);
        QCOMPARE(result[0].result()[2].value(), 1);
        QCOMPARE(result[0].result()[3].value(), 2);
        QCOMPARE(result[0].result()[4].value(), 3);
        QCOMPARE(result[0].result()[5].value(), 4);
        QCOMPARE(result[0].result()[6].value(), 5);
        QCOMPARE(result[0].result()[7].value(), 6);
        QCOMPARE(result[0].result()[8].value(), 7);
        QCOMPARE(result[0].result()[9].value(), 8);
        QCOMPARE(result[0].result()[10].value(), 9);

        QCOMPARE(response->hasMoreData(), false);
    }
}

void Tst_QOpcUaClient::readHistoryDataFromClient()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=Demo.Static.Historizing2"_s));
    QVERIFY (node != nullptr);
    WRITE_VALUE_ATTRIBUTE(node, 0, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(node, 1, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(node, 2, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(node, 3, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(node, 4, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(node, 5, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(node, 6, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(node, 7, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(node, 8, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(node, 9, QOpcUa::Types::Int32);

    QScopedPointer<QOpcUaNode> nodeWithLimit(opcuaClient->node(u"ns=2;s=Demo.Static.Historizing2.ContinuationPoint"_s));
    QVERIFY (nodeWithLimit != nullptr);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 10, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 11, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 12, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 13, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 14, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 15, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 16, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 17, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 18, QOpcUa::Types::Int32);
    QTest::qWait(1);
    WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 19, QOpcUa::Types::Int32);
    QTest::qWait(1);

    // Values for two nodes in chronological order
    // One of the nodes will require two calls to get all values
    {
        QOpcUaHistoryReadRawRequest request(
                    {QOpcUaReadItem(node->nodeId()), QOpcUaReadItem(nodeWithLimit->nodeId())},
                    QDateTime::currentDateTime(),
                    QDateTime::currentDateTime().addDays(-1),
                    15,
                    false);
        request.setTimestampsToReturn(QOpcUa::TimestampsToReturn::Server);

        QScopedPointer<QOpcUaHistoryReadResponse> response(opcuaClient->readHistoryData(request));
        QVERIFY(response != nullptr);

        QSignalSpy readHistoryDataSpy(response.get(), &QOpcUaHistoryReadResponse::readHistoryDataFinished);
        readHistoryDataSpy.wait(signalSpyTimeout);

        QCOMPARE(readHistoryDataSpy.size(), 1);

        QCOMPARE(readHistoryDataSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
        auto result = readHistoryDataSpy.at(0).at(0).value<QList<QOpcUaHistoryData>>();
        QCOMPARE(result.size(), 2);
        QCOMPARE(result[0].nodeId(), u"ns=2;s=Demo.Static.Historizing2"_s);
        QCOMPARE(result[0].statusCode(), QOpcUa::UaStatusCode::Good);
        QCOMPARE(result[0].count(), 10);
        QCOMPARE(result[0].result()[0].value(), 9);
        QCOMPARE(result[0].result()[1].value(), 8);
        QCOMPARE(result[0].result()[2].value(), 7);
        QCOMPARE(result[0].result()[3].value(), 6);
        QCOMPARE(result[0].result()[4].value(), 5);
        QCOMPARE(result[0].result()[5].value(), 4);
        QCOMPARE(result[0].result()[6].value(), 3);
        QCOMPARE(result[0].result()[7].value(), 2);
        QCOMPARE(result[0].result()[8].value(), 1);
        QCOMPARE(result[0].result()[9].value(), 0);
        QCOMPARE(result[1].count(), 5);
        QCOMPARE(result[1].nodeId(), u"ns=2;s=Demo.Static.Historizing2.ContinuationPoint"_s);
        QCOMPARE(result[1].result()[0].value(), 19);
        QCOMPARE(result[1].result()[1].value(), 18);
        QCOMPARE(result[1].result()[2].value(), 17);
        QCOMPARE(result[1].result()[3].value(), 16);
        QCOMPARE(result[1].result()[4].value(), 15);

        QCOMPARE(response->hasMoreData(), true);
        readHistoryDataSpy.clear();

        response->readMoreData();
        readHistoryDataSpy.wait(signalSpyTimeout);

        QCOMPARE(readHistoryDataSpy.size(), 1);

        QCOMPARE(readHistoryDataSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
        result = readHistoryDataSpy.at(0).at(0).value<QList<QOpcUaHistoryData>>();
        QCOMPARE(result.size(), 2);
        QCOMPARE(result[0].nodeId(), u"ns=2;s=Demo.Static.Historizing2"_s);
        QCOMPARE(result[0].statusCode(), QOpcUa::UaStatusCode::Good);
        QCOMPARE(result[0].count(), 10);
        QCOMPARE(result[0].result()[0].value(), 9);
        QCOMPARE(result[0].result()[1].value(), 8);
        QCOMPARE(result[0].result()[2].value(), 7);
        QCOMPARE(result[0].result()[3].value(), 6);
        QCOMPARE(result[0].result()[4].value(), 5);
        QCOMPARE(result[0].result()[5].value(), 4);
        QCOMPARE(result[0].result()[6].value(), 3);
        QCOMPARE(result[0].result()[7].value(), 2);
        QCOMPARE(result[0].result()[8].value(), 1);
        QCOMPARE(result[0].result()[9].value(), 0);
        QCOMPARE(result[1].count(), 10);
        QCOMPARE(result[1].nodeId(), u"ns=2;s=Demo.Static.Historizing2.ContinuationPoint"_s);
        QCOMPARE(result[1].result()[0].value(), 19);
        QCOMPARE(result[1].result()[1].value(), 18);
        QCOMPARE(result[1].result()[2].value(), 17);
        QCOMPARE(result[1].result()[3].value(), 16);
        QCOMPARE(result[1].result()[4].value(), 15);
        QCOMPARE(result[1].result()[5].value(), 14);
        QCOMPARE(result[1].result()[6].value(), 13);
        QCOMPARE(result[1].result()[7].value(), 12);
        QCOMPARE(result[1].result()[8].value(), 11);
        QCOMPARE(result[1].result()[9].value(), 10);

        QCOMPARE(response->hasMoreData(), false);
    }

    // Don't follow the continuation points to the end and release remaining
    {
        WRITE_VALUE_ATTRIBUTE(nodeWithLimit, 20, QOpcUa::Types::Int32);
        QTest::qWait(1);

        QOpcUaHistoryReadRawRequest request(
            {QOpcUaReadItem(node->nodeId()), QOpcUaReadItem(nodeWithLimit->nodeId())},
            QDateTime::currentDateTime(),
            QDateTime::currentDateTime().addDays(-1),
            15,
            false);
        request.setTimestampsToReturn(QOpcUa::TimestampsToReturn::Server);
        QScopedPointer<QOpcUaHistoryReadResponse> response(opcuaClient->readHistoryData(request));
        QVERIFY(response != nullptr);

        QSignalSpy readHistoryDataSpy(response.get(), &QOpcUaHistoryReadResponse::readHistoryDataFinished);
        readHistoryDataSpy.wait(signalSpyTimeout);

        QCOMPARE(readHistoryDataSpy.size(), 1);
        QCOMPARE(readHistoryDataSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
        QCOMPARE(response->hasMoreData(), true);
        readHistoryDataSpy.clear();
        response->readMoreData();
        readHistoryDataSpy.wait(signalSpyTimeout);
        QCOMPARE(readHistoryDataSpy.size(), 1);
        QCOMPARE(response->hasMoreData(), true);

        const auto data = response.data();

        response->releaseContinuationPoints();
        readHistoryDataSpy.clear();
        readHistoryDataSpy.wait(signalSpyTimeout);
        QCOMPARE(readHistoryDataSpy.size(), 1);
        QCOMPARE(readHistoryDataSpy.at(0).at(1), QOpcUa::UaStatusCode::Good);
        QCOMPARE(response.data(), data);
    }
}

void Tst_QOpcUaClient::readHistoryEventsFromNode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=2;s=EventHistorian"_s));
    QVERIFY(node != nullptr);

    QOpcUaMonitoringParameters::EventFilter filter;
    filter << QOpcUaSimpleAttributeOperand(u"Message"_s);
    filter << QOpcUaSimpleAttributeOperand(u"Time"_s);

    const auto firstAvailableEventTime = QDateTime::fromMSecsSinceEpoch(1694153836000 - 120 * 60 * 1000);
    const auto lastAvailableEventTime = QDateTime::fromMSecsSinceEpoch(1694153836000);

    // Read all at once
    {
        QScopedPointer<QOpcUaHistoryReadResponse> response(node->readHistoryEvents(firstAvailableEventTime, lastAvailableEventTime, filter, 10));
        QVERIFY(response != nullptr);

        QSignalSpy spy(response.get(), &QOpcUaHistoryReadResponse::readHistoryEventsFinished);
        spy.wait();
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.at(0).at(1), QOpcUa::UaStatusCode::Good);

        QVERIFY(!response->hasMoreData());
        QCOMPARE(response->state(), QOpcUaHistoryReadResponse::State::Finished);

        QCOMPARE(response->events().size(), 1);
        QCOMPARE(response->events().at(0).events().size(), 3);
        QCOMPARE(response->events().at(0).events().at(0).size(), 2);
        QCOMPARE(response->events().at(0).events().at(0).at(0), QOpcUaLocalizedText(u"en"_s, u"Message 1"_s));
        QCOMPARE(response->events().at(0).events().at(0).at(1), QDateTime::fromMSecsSinceEpoch(1694146636000));
        QCOMPARE(response->events().at(0).events().at(1).size(), 2);
        QCOMPARE(response->events().at(0).events().at(1).at(0), QOpcUaLocalizedText(u"en"_s, u"Message 2"_s));
        QCOMPARE(response->events().at(0).events().at(1).at(1), QDateTime::fromMSecsSinceEpoch(1694150236000));
        QCOMPARE(response->events().at(0).events().at(2).size(), 2);
        QCOMPARE(response->events().at(0).events().at(2).at(0), QOpcUaLocalizedText(u"en"_s, u"Message 3"_s));
        QCOMPARE(response->events().at(0).events().at(2).at(1), QDateTime::fromMSecsSinceEpoch(1694153836000));

        QCOMPARE(spy.at(0).at(0).value<QList<QOpcUaHistoryEvent>>(), response->events());
    }

    // Test continuation points
    {
        QScopedPointer<QOpcUaHistoryReadResponse> response(node->readHistoryEvents(firstAvailableEventTime, lastAvailableEventTime, filter, 1));
        QVERIFY(response != nullptr);

        QSignalSpy spy(response.get(), &QOpcUaHistoryReadResponse::readHistoryEventsFinished);
        spy.wait();
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.at(0).at(1), QOpcUa::UaStatusCode::Good);

        QVERIFY(response->hasMoreData());
        QCOMPARE(response->state(), QOpcUaHistoryReadResponse::State::MoreDataAvailable);
        spy.clear();
        response->readMoreData();
        spy.wait();
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.at(0).at(1), QOpcUa::UaStatusCode::Good);

        QVERIFY(response->hasMoreData());
        QCOMPARE(response->state(), QOpcUaHistoryReadResponse::State::MoreDataAvailable);
        spy.clear();
        response->readMoreData();
        spy.wait();
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.at(0).at(1), QOpcUa::UaStatusCode::Good);

        QVERIFY(!response->hasMoreData());
        QCOMPARE(response->state(), QOpcUaHistoryReadResponse::State::Finished);

        QCOMPARE(response->events().size(), 1);
        QCOMPARE(response->events().at(0).events().size(), 3);
        QCOMPARE(response->events().at(0).events().at(0).size(), 2);
        QCOMPARE(response->events().at(0).events().at(0).at(0), QOpcUaLocalizedText(u"en"_s, u"Message 1"_s));
        QCOMPARE(response->events().at(0).events().at(0).at(1), QDateTime::fromMSecsSinceEpoch(1694146636000));
        QCOMPARE(response->events().at(0).events().at(1).size(), 2);
        QCOMPARE(response->events().at(0).events().at(1).at(0), QOpcUaLocalizedText(u"en"_s, u"Message 2"_s));
        QCOMPARE(response->events().at(0).events().at(1).at(1), QDateTime::fromMSecsSinceEpoch(1694150236000));
        QCOMPARE(response->events().at(0).events().at(2).size(), 2);
        QCOMPARE(response->events().at(0).events().at(2).at(0), QOpcUaLocalizedText(u"en"_s, u"Message 3"_s));
        QCOMPARE(response->events().at(0).events().at(2).at(1), QDateTime::fromMSecsSinceEpoch(1694153836000));
    }
}

void Tst_QOpcUaClient::readHistoryEventsFromClient()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QOpcUaMonitoringParameters::EventFilter filter;
    filter << QOpcUaSimpleAttributeOperand(u"Message"_s);
    filter << QOpcUaSimpleAttributeOperand(u"Time"_s);

    const auto firstAvailableEventTime = QDateTime::fromMSecsSinceEpoch(1694153836000 - 120 * 60 * 1000);
    const auto lastAvailableEventTime = QDateTime::fromMSecsSinceEpoch(1694153836000);

    // Read all at once
    {
        QOpcUaHistoryReadEventRequest request({ QOpcUaReadItem(u"ns=2;s=EventHistorian"_s), QOpcUaReadItem(u"ns=2;s=EventHistorian2"_s) },
                                                    firstAvailableEventTime, lastAvailableEventTime, filter);
        request.setNumValuesPerNode(10);

        QScopedPointer<QOpcUaHistoryReadResponse> response(opcuaClient->readHistoryEvents(request));
        QVERIFY(response != nullptr);

        QSignalSpy spy(response.get(), &QOpcUaHistoryReadResponse::readHistoryEventsFinished);
        spy.wait();
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.at(0).at(1), QOpcUa::UaStatusCode::Good);

        QVERIFY(!response->hasMoreData());
        QCOMPARE(response->state(), QOpcUaHistoryReadResponse::State::Finished);

        QCOMPARE(response->events().size(), 2);
        QCOMPARE(response->events().at(0).events().size(), 3);
        QCOMPARE(response->events().at(0).events().at(0).size(), 2);
        QCOMPARE(response->events().at(0).events().at(0).at(0), QOpcUaLocalizedText(u"en"_s, u"Message 1"_s));
        QCOMPARE(response->events().at(0).events().at(0).at(1), QDateTime::fromMSecsSinceEpoch(1694146636000));
        QCOMPARE(response->events().at(0).events().at(1).size(), 2);
        QCOMPARE(response->events().at(0).events().at(1).at(0), QOpcUaLocalizedText(u"en"_s, u"Message 2"_s));
        QCOMPARE(response->events().at(0).events().at(1).at(1), QDateTime::fromMSecsSinceEpoch(1694150236000));
        QCOMPARE(response->events().at(0).events().at(2).size(), 2);
        QCOMPARE(response->events().at(0).events().at(2).at(0), QOpcUaLocalizedText(u"en"_s, u"Message 3"_s));
        QCOMPARE(response->events().at(0).events().at(2).at(1), QDateTime::fromMSecsSinceEpoch(1694153836000));

        QCOMPARE(response->events().at(1).events().size(), 2);
        QCOMPARE(response->events().at(1).events().at(0).size(), 2);
        QCOMPARE(response->events().at(1).events().at(0).at(0), QOpcUaLocalizedText(u"en"_s, u"Message 1"_s));
        QCOMPARE(response->events().at(1).events().at(0).at(1), QDateTime::fromMSecsSinceEpoch(1694146636000));
        QCOMPARE(response->events().at(1).events().at(1).size(), 2);
        QCOMPARE(response->events().at(1).events().at(1).at(0), QOpcUaLocalizedText(u"en"_s, u"Message 2"_s));
        QCOMPARE(response->events().at(1).events().at(1).at(1), QDateTime::fromMSecsSinceEpoch(1694150236000));

        QCOMPARE(spy.at(0).at(0).value<QList<QOpcUaHistoryEvent>>(), response->events());
    }

    // Test continuation points
    {
        QOpcUaHistoryReadEventRequest request({ QOpcUaReadItem(u"ns=2;s=EventHistorian"_s), QOpcUaReadItem(u"ns=2;s=EventHistorian2"_s) },
                                                    firstAvailableEventTime, lastAvailableEventTime, filter);
        request.setNumValuesPerNode(1);

        QScopedPointer<QOpcUaHistoryReadResponse> response(opcuaClient->readHistoryEvents(request));
        QVERIFY(response != nullptr);

        QSignalSpy spy(response.get(), &QOpcUaHistoryReadResponse::readHistoryEventsFinished);
        spy.wait();
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.at(0).at(1), QOpcUa::UaStatusCode::Good);

        QVERIFY(response->hasMoreData());
        QCOMPARE(response->state(), QOpcUaHistoryReadResponse::State::MoreDataAvailable);
        spy.clear();
        response->readMoreData();
        spy.wait();
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.at(0).at(1), QOpcUa::UaStatusCode::Good);

        QVERIFY(response->hasMoreData());
        QCOMPARE(response->state(), QOpcUaHistoryReadResponse::State::MoreDataAvailable);
        spy.clear();
        response->readMoreData();
        spy.wait();
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.at(0).at(1), QOpcUa::UaStatusCode::Good);

        QVERIFY(!response->hasMoreData());
        QCOMPARE(response->state(), QOpcUaHistoryReadResponse::State::Finished);

        QCOMPARE(response->events().size(), 2);
        QCOMPARE(response->events().at(0).events().size(), 3);
        QCOMPARE(response->events().at(0).events().at(0).size(), 2);
        QCOMPARE(response->events().at(0).events().at(0).at(0), QOpcUaLocalizedText(u"en"_s, u"Message 1"_s));
        QCOMPARE(response->events().at(0).events().at(0).at(1), QDateTime::fromMSecsSinceEpoch(1694146636000));
        QCOMPARE(response->events().at(0).events().at(1).size(), 2);
        QCOMPARE(response->events().at(0).events().at(1).at(0), QOpcUaLocalizedText(u"en"_s, u"Message 2"_s));
        QCOMPARE(response->events().at(0).events().at(1).at(1), QDateTime::fromMSecsSinceEpoch(1694150236000));
        QCOMPARE(response->events().at(0).events().at(2).size(), 2);
        QCOMPARE(response->events().at(0).events().at(2).at(0), QOpcUaLocalizedText(u"en"_s, u"Message 3"_s));
        QCOMPARE(response->events().at(0).events().at(2).at(1), QDateTime::fromMSecsSinceEpoch(1694153836000));

        QCOMPARE(response->events().at(1).events().size(), 2);
        QCOMPARE(response->events().at(1).events().at(0).size(), 2);
        QCOMPARE(response->events().at(1).events().at(0).at(0), QOpcUaLocalizedText(u"en"_s, u"Message 1"_s));
        QCOMPARE(response->events().at(1).events().at(0).at(1), QDateTime::fromMSecsSinceEpoch(1694146636000));
        QCOMPARE(response->events().at(1).events().at(1).size(), 2);
        QCOMPARE(response->events().at(1).events().at(1).at(0), QOpcUaLocalizedText(u"en"_s, u"Message 2"_s));
        QCOMPARE(response->events().at(1).events().at(1).at(1), QDateTime::fromMSecsSinceEpoch(1694150236000));
    }

    // Test continuation points release
    {
        QOpcUaHistoryReadEventRequest request({ QOpcUaReadItem(u"ns=2;s=EventHistorian"_s), QOpcUaReadItem(u"ns=2;s=EventHistorian2"_s) },
                                                    firstAvailableEventTime, lastAvailableEventTime, filter);
        request.setNumValuesPerNode(1);

        QScopedPointer<QOpcUaHistoryReadResponse> response(opcuaClient->readHistoryEvents(request));
        QVERIFY(response != nullptr);

        QSignalSpy spy(response.get(), &QOpcUaHistoryReadResponse::readHistoryEventsFinished);
        spy.wait();
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.at(0).at(1), QOpcUa::UaStatusCode::Good);

        QVERIFY(response->hasMoreData());
        QCOMPARE(response->state(), QOpcUaHistoryReadResponse::State::MoreDataAvailable);
        spy.clear();
        response->readMoreData();
        spy.wait();
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.at(0).at(1), QOpcUa::UaStatusCode::Good);

        QVERIFY(response->hasMoreData());
        QCOMPARE(response->state(), QOpcUaHistoryReadResponse::State::MoreDataAvailable);

        const auto events = response->events();

        response->releaseContinuationPoints();
        QCOMPARE(response->state(), QOpcUaHistoryReadResponse::State::Finished);
        spy.clear();
        spy.wait();
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.at(0).at(1), QOpcUa::UaStatusCode::Good);
        QCOMPARE(response->events(), events);
    }

    // Check invalid node id
    {
        QOpcUaHistoryReadEventRequest request({ QOpcUaReadItem(u"ns=2;s=EventHistorian"_s), QOpcUaReadItem(u"ns=2;s=EventHistorian3"_s) },
                                                    firstAvailableEventTime, lastAvailableEventTime, filter);
        request.setNumValuesPerNode(10);
        QScopedPointer<QOpcUaHistoryReadResponse> response(opcuaClient->readHistoryEvents(request));
        QVERIFY(response != nullptr);

        QSignalSpy spy(response.get(), &QOpcUaHistoryReadResponse::readHistoryEventsFinished);
        spy.wait();
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.at(0).at(1), QOpcUa::UaStatusCode::Good);

        QVERIFY(!response->hasMoreData());
        QCOMPARE(response->state(), QOpcUaHistoryReadResponse::State::Finished);

        QCOMPARE(response->events().size(), 2);
        QCOMPARE(response->events().at(0).events().size(), 3);
        QCOMPARE(response->events().at(0).events().at(0).size(), 2);
        QCOMPARE(response->events().at(0).events().at(0).at(0), QOpcUaLocalizedText(u"en"_s, u"Message 1"_s));
        QCOMPARE(response->events().at(0).events().at(0).at(1), QDateTime::fromMSecsSinceEpoch(1694146636000));
        QCOMPARE(response->events().at(0).events().at(1).size(), 2);
        QCOMPARE(response->events().at(0).events().at(1).at(0), QOpcUaLocalizedText(u"en"_s, u"Message 2"_s));
        QCOMPARE(response->events().at(0).events().at(1).at(1), QDateTime::fromMSecsSinceEpoch(1694150236000));
        QCOMPARE(response->events().at(0).events().at(2).size(), 2);
        QCOMPARE(response->events().at(0).events().at(2).at(0), QOpcUaLocalizedText(u"en"_s, u"Message 3"_s));
        QCOMPARE(response->events().at(0).events().at(2).at(1), QDateTime::fromMSecsSinceEpoch(1694153836000));

        QCOMPARE(response->events().at(1).events().size(), 0);
        QCOMPARE(response->events().at(1).statusCode(), QOpcUa::UaStatusCode::BadNodeIdInvalid);
    }
}

void Tst_QOpcUaClient::decodeGenericStruct()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    // Initialization should fail if the client is not connected
    QOpcUaGenericStructHandler decoder(opcuaClient);

    auto success = decoder.initialize();
    QCOMPARE(success, false);

    OpcuaConnector connector(opcuaClient, m_endpoint);

    // After the connection was established, initialization should work
    QSignalSpy spy(&decoder, &QOpcUaGenericStructHandler::initializedChanged);

    success = decoder.initialize();
    QCOMPARE(success, true);

    spy.wait();

    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);

    const auto testEnumerationTypeId = u"ns=4;i=3002"_s;
    const auto testStructTypeId = u"ns=4;i=3003"_s;
    const auto testStructEncodingId = u"ns=4;i=5001"_s;
    const auto abstractBaseStructTypeId = u"ns=4;i=3008"_s;

    QCOMPARE(decoder.typeNameForTypeId(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::StatusCode)), u"StatusCode"_s);
    QCOMPARE(decoder.typeNameForTypeId(testEnumerationTypeId), u"QtTestEnumeration"_s);
    QCOMPARE(decoder.typeNameForTypeId(testStructTypeId), u"QtTestStructType"_s);
    QCOMPARE(decoder.typeNameForTypeId(abstractBaseStructTypeId), u"QtAbstractBaseStruct"_s);
    QCOMPARE(decoder.typeNameForTypeId(u"ns=10;i=1234"_s), QString());

    QCOMPARE(decoder.dataTypeKindForTypeId(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::StatusCode)),
             QOpcUaGenericStructHandler::DataTypeKind::Other);
    QCOMPARE(decoder.dataTypeKindForTypeId(testEnumerationTypeId), QOpcUaGenericStructHandler::DataTypeKind::Enum);
    QCOMPARE(decoder.dataTypeKindForTypeId(testStructTypeId), QOpcUaGenericStructHandler::DataTypeKind::Struct);
    QCOMPARE(decoder.dataTypeKindForTypeId(u"ns=10;i=1234"_s), QOpcUaGenericStructHandler::DataTypeKind::Unknown);

    QCOMPARE(decoder.isAbstractTypeId(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Number)), true);
    QCOMPARE(decoder.isAbstractTypeId(testEnumerationTypeId), false);
    QCOMPARE(decoder.isAbstractTypeId(testStructTypeId), false);
    QCOMPARE(decoder.isAbstractTypeId(abstractBaseStructTypeId), true);

    QCOMPARE(decoder.structureDefinitionForBinaryEncodingId(testStructEncodingId).structureType(),
             QOpcUaStructureDefinition::StructureType::Structure);

    QCOMPARE(decoder.typeNameForBinaryEncodingId(decoder.structureDefinitionForBinaryEncodingId(testStructEncodingId).defaultEncodingId()),
             u"QtTestStructType"_s);

    QOpcUaReadItem readNestedStruct(u"ns=4;i=6009"_s);
    QOpcUaReadItem readUnion1(u"ns=4;i=6011"_s);
    QOpcUaReadItem readUnion2(u"ns=4;i=6003"_s);
    QOpcUaReadItem readStructWithOptField(u"ns=4;i=6010"_s);
    QOpcUaReadItem readStructWithoutOptField(u"ns=4;i=6002"_s);
    QOpcUaReadItem readStructWithDiagnosticInfo(u"ns=4;i=6006"_s);
    QOpcUaReadItem readStructWithDataValue(u"ns=4;i=6027"_s);

    QSignalSpy readSpy(opcuaClient, &QOpcUaClient::readNodeAttributesFinished);

    const auto readDispatch = opcuaClient->readNodeAttributes({readNestedStruct, readUnion1, readUnion2,
                                                               readStructWithOptField, readStructWithoutOptField,
                                                               readStructWithDiagnosticInfo, readStructWithDataValue});
    QCOMPARE(readDispatch, true);

    readSpy.wait();

    QCOMPARE(readSpy.size(), 1);
    QCOMPARE(readSpy.at(0).size(), 2);
    QCOMPARE(readSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    auto results = readSpy.at(0).at(0).value<QList<QOpcUaReadResult>>();

    QCOMPARE(results.size(), 7);
    QCOMPARE(results.at(0).statusCode(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(results.at(1).statusCode(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(results.at(2).statusCode(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(results.at(3).statusCode(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(results.at(4).statusCode(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(results.at(5).statusCode(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(results.at(6).statusCode(), QOpcUa::UaStatusCode::Good);

    {
        auto nestedStructExt = results.at(0).value().value<QOpcUaExtensionObject>();
        auto decodedData = decoder.decode(nestedStructExt);
        QVERIFY(decodedData);
        qDebug() << *decodedData;

        QCOMPARE(decodedData->typeName(), u"QtTestStructType"_s);

        QCOMPARE(decodedData->fields().value(u"EnumMember"_s).toInt(), 1);
        QCOMPARE(decodedData->fields().value(u"Int64ArrayMember"_s).value<QList<qint64>>(),
                 QList<qint64>({std::numeric_limits<qint64>::max(),
                                std::numeric_limits<qint64>::max() - 1,
                                std::numeric_limits<qint64>::min()}));

        QCOMPARE(decodedData->fields().value(u"LocalizedTextMember"_s).value<QOpcUaLocalizedText>().locale(), u"en"_s);
        QCOMPARE(decodedData->fields().value(u"LocalizedTextMember"_s).value<QOpcUaLocalizedText>().text(), u"TestText"_s);

        QCOMPARE(decodedData->fields().value(u"QualifiedNameMember"_s).value<QOpcUaQualifiedName>().namespaceIndex(), 1);
        QCOMPARE(decodedData->fields().value(u"QualifiedNameMember"_s).value<QOpcUaQualifiedName>().name(), u"TestName"_s);

        QCOMPARE(decodedData->fields().value(u"StringMember"_s).value<QString>(), u"TestString"_s);

        auto nestedStructValue = decodedData->fields().value(u"NestedStructMember"_s).value<QOpcUaGenericStructValue>();
        QCOMPARE(nestedStructValue.typeName(), u"QtInnerTestStructType"_s);
        QCOMPARE(nestedStructValue.fields().value(u"DoubleSubtypeMember"_s).toDouble(), 42.0);

        auto nestedStructArrayValue = decodedData->fields().value(u"NestedStructArrayMember"_s).value<QList<QOpcUaGenericStructValue>>();
        QCOMPARE(nestedStructArrayValue.size(), 2);
        QCOMPARE(nestedStructArrayValue.at(0).typeName(), u"QtInnerTestStructType"_s);
        QCOMPARE(nestedStructArrayValue.at(0).fields().value(u"DoubleSubtypeMember"_s).toDouble(), 23.0);
        QCOMPARE(nestedStructArrayValue.at(1).typeName(), u"QtInnerTestStructType"_s);
        QCOMPARE(nestedStructArrayValue.at(1).fields().value(u"DoubleSubtypeMember"_s).toDouble(), 42.0);
    }

    {
        auto unionExt = results.at(1).value().value<QOpcUaExtensionObject>();
        auto decodedData = decoder.decode(unionExt);
        QVERIFY(decodedData);
        qDebug() << *decodedData;

        QCOMPARE(decodedData->typeName(), u"QtTestUnionType"_s);
        QCOMPARE(decodedData->fields().value(u"Member1"_s), 42);
    }

    {
        auto unionExt = results.at(2).value().value<QOpcUaExtensionObject>();
        auto decodedData = decoder.decode(unionExt);
        QVERIFY(decodedData);
        qDebug() << *decodedData;

        QCOMPARE(decodedData->typeName(), u"QtTestUnionType"_s);

        auto unionFieldValue = decodedData->fields().value(u"Member2"_s).value<QOpcUaGenericStructValue>();
        QCOMPARE(unionFieldValue.fields().value(u"DoubleSubtypeMember"_s).toDouble(), 23.0);
    }

    // Struct type with optional field with optional field set
    {
        auto optionalFieldExt = results.at(3).value().value<QOpcUaExtensionObject>();
        auto decodedData = decoder.decode(optionalFieldExt);
        QVERIFY(decodedData);
        qDebug() << *decodedData;

        QCOMPARE(decodedData->typeName(), u"QtStructWithOptionalFieldType"_s);

        QCOMPARE(decodedData->fields().value(u"MandatoryMember"_s).toDouble(), 42.0);
        QCOMPARE(decodedData->fields().value(u"OptionalMember"_s).toDouble(), 23.0);
    }

    // Struct type with optional field without optional field set
    {
        auto optionalFieldExt = results.at(4).value().value<QOpcUaExtensionObject>();
        auto decodedData = decoder.decode(optionalFieldExt);
        QVERIFY(decodedData);
        qDebug() << *decodedData;

        QCOMPARE(decodedData->typeName(), u"QtStructWithOptionalFieldType"_s);

        QCOMPARE(decodedData->fields().value(u"MandatoryMember"_s).toDouble(), 42.0);
        QCOMPARE(decodedData->fields().contains(u"OptionalMember"_s), false);
    }

    {
        auto diagnosticInfoExt = results.at(5).value().value<QOpcUaExtensionObject>();
        auto decodedData = decoder.decode(diagnosticInfoExt);
        QVERIFY(decodedData);
        qDebug() << *decodedData;
        QCOMPARE(decodedData->fields().contains(u"DiagnosticInfoMember"_s), true);
        QCOMPARE(decodedData->fields().value(u"DiagnosticInfoMember"_s).value<QOpcUaDiagnosticInfo>(), testDiagnosticInfos.at(0));
        QCOMPARE(decodedData->fields().contains(u"DiagnosticInfoArrayMember"_s), true);
        QCOMPARE(decodedData->fields().value(u"DiagnosticInfoArrayMember"_s).value<QList<QOpcUaDiagnosticInfo>>(), testDiagnosticInfos);
    }

    {
        auto dataValueExt = results.at(6).value().value<QOpcUaExtensionObject>();
        auto decodedData = decoder.decode(dataValueExt);
        QVERIFY(decodedData);
        qDebug() << *decodedData;

        QVERIFY(decodedData->fields().contains(u"DataValueMember"_s));
        const auto dv = decodedData->fields().value(u"DataValueMember"_s).value<QOpcUaDataValue>();
        QCOMPARE(dv.statusCode(), QOpcUa::UaStatusCode::BadInternalError);
        QCOMPARE(dv.serverTimestamp(), QDateTime::fromMSecsSinceEpoch(1698655307000));
        QCOMPARE(dv.sourceTimestamp(), QDateTime::fromMSecsSinceEpoch(1698655306000));
        QCOMPARE(dv.sourcePicoseconds(), 42);
        QCOMPARE(dv.serverPicoseconds(), 23);
        const auto v = dv.value().value<QOpcUaVariant>();
        QCOMPARE(v.value(), (QList<QString>{u"TestString 1"_s, u"TestString 2"_s, u"TestString 3"_s, u"TestString 4"_s}));
        QCOMPARE(v.arrayDimensions(), (QList<qint32>{ 2, 2 }));

        QVERIFY(decodedData->fields().contains(u"VariantMember"_s));
        const auto var = decodedData->fields().value(u"VariantMember"_s).value<QOpcUaVariant>();
        QVERIFY(var.arrayDimensions().isEmpty());
        QCOMPARE(var.type(), QOpcUaVariant::ValueType::UInt64);
        QCOMPARE(var.value(), 42);
    }
}

void Tst_QOpcUaClient::encodeGenericStruct()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QOpcUaGenericStructHandler handler(opcuaClient);

    QSignalSpy spy(&handler, &QOpcUaGenericStructHandler::initializedChanged);

    auto success = handler.initialize();
    QCOMPARE(success, true);

    spy.wait();

    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);

    {
        auto value = handler.createGenericStructValueForTypeId(u"ns=4;i=3003"_s);
        value.fieldsRef()[u"StringMember"_s] = u"StringMember"_s;
        value.fieldsRef()[u"LocalizedTextMember"_s] = QOpcUaLocalizedText(u"en"_s, u"Localized text"_s);
        value.fieldsRef()[u"QualifiedNameMember"_s] = QOpcUaQualifiedName(1, u"Qualified name"_s);
        value.fieldsRef()[u"Int64ArrayMember"_s] = QVariant::fromValue(QList<qint64>({1, 2, 3, 4}));
        value.fieldsRef()[u"EnumMember"_s] = 42;

        auto innerValue = handler.createGenericStructValueForTypeId(u"ns=4;i=3004"_s);
        innerValue.fieldsRef()[u"DoubleSubtypeMember"_s] = 42.0;

        value.fieldsRef()[u"NestedStructMember"_s] = innerValue;

        auto innerValue1 = handler.createGenericStructValueForTypeId(u"ns=4;i=3004"_s);
        innerValue1.fieldsRef()[u"DoubleSubtypeMember"_s] = 23.0;

        auto innerValue2 = handler.createGenericStructValueForTypeId(u"ns=4;i=3004"_s);
        innerValue2.fieldsRef()[u"DoubleSubtypeMember"_s] = 42.0;

        value.fieldsRef()[u"NestedStructArrayMember"_s] =
            QVariant::fromValue(QList<QOpcUaGenericStructValue>({innerValue1, innerValue2}));

        auto ext = handler.encode(value);
        QVERIFY(ext);

        const auto decoded = handler.decode(*ext);
        QVERIFY(decoded);

        QCOMPARE(decoded->fields().value(u"StringMember"_s), u"StringMember"_s);
        QCOMPARE(decoded->fields().value(u"LocalizedTextMember"_s), QOpcUaLocalizedText(u"en"_s, u"Localized text"_s));
        QCOMPARE(decoded->fields().value(u"QualifiedNameMember"_s), QOpcUaQualifiedName(1, u"Qualified name"_s));
        QCOMPARE(decoded->fields().value(u"Int64ArrayMember"_s), QVariant::fromValue(QList<qint64>({1, 2, 3, 4})));
        QCOMPARE(decoded->fields().value(u"EnumMember"_s), 42);

        const auto innerDecoded = decoded->fields().value(u"NestedStructMember"_s).value<QOpcUaGenericStructValue>();
        QCOMPARE(innerDecoded.fields().value(u"DoubleSubtypeMember"_s), 42.0);

        auto nestedStructArrayValue = decoded->fields().value(u"NestedStructArrayMember"_s).value<QList<QOpcUaGenericStructValue>>();
        QCOMPARE(nestedStructArrayValue.size(), 2);
        QCOMPARE(nestedStructArrayValue.at(0).typeName(), u"QtInnerTestStructType"_s);
        QCOMPARE(nestedStructArrayValue.at(0).fields().value(u"DoubleSubtypeMember"_s).toDouble(), 23.0);
        QCOMPARE(nestedStructArrayValue.at(1).typeName(), u"QtInnerTestStructType"_s);
        QCOMPARE(nestedStructArrayValue.at(1).fields().value(u"DoubleSubtypeMember"_s).toDouble(), 42.0);

        QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=4;i=6009"_s));
        WRITE_VALUE_ATTRIBUTE(node, *ext, QOpcUa::Types::ExtensionObject);
    }

    {
        auto unionValue = handler.createGenericStructValueForTypeId(u"ns=4;i=3005"_s);
        unionValue.fieldsRef()[u"Member1"_s] = 23;

        auto ext = handler.encode(unionValue);
        QVERIFY(ext);
        QCOMPARE(ext->encodingTypeId(), unionValue.structureDefinition().defaultEncodingId());

        const auto decoded = handler.decode(*ext);
        QVERIFY(decoded);

        QCOMPARE(decoded->fields().value(u"Member1"_s), 23);

        QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=4;i=6011"_s));
        WRITE_VALUE_ATTRIBUTE(node, *ext, QOpcUa::Types::ExtensionObject);
    }

    {
        auto unionValue = handler.createGenericStructValueForTypeId(u"ns=4;i=3005"_s);
        auto innerValue = handler.createGenericStructValueForTypeId(u"ns=4;i=3004"_s);
        innerValue.fieldsRef()[u"DoubleSubtypeMember"_s] = 42.0;
        unionValue.fieldsRef()[u"Member2"_s] = innerValue;

        auto ext = handler.encode(unionValue);
        QVERIFY(ext);
        QCOMPARE(ext->encodingTypeId(), unionValue.structureDefinition().defaultEncodingId());

        const auto decoded = handler.decode(*ext);
        QVERIFY(decoded);

        QVERIFY(decoded->fields().contains(u"Member2"_s));

        const auto innerDecoded = decoded->fields().value(u"Member2"_s).value<QOpcUaGenericStructValue>();
        QCOMPARE(innerDecoded.fields().value(u"DoubleSubtypeMember"_s), 42.0);

        QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=4;i=6003"_s));
        WRITE_VALUE_ATTRIBUTE(node, *ext, QOpcUa::Types::ExtensionObject);
    }

    {
        auto value = handler.createGenericStructValueForTypeId(u"ns=4;i=3006"_s);
        value.fieldsRef()[u"MandatoryMember"_s] = 23.0;
        value.fieldsRef()[u"OptionalMember"_s] = 42.0;

        auto ext = handler.encode(value);
        QVERIFY(ext);
        QCOMPARE(ext->encodingTypeId(), value.structureDefinition().defaultEncodingId());

        const auto decoded = handler.decode(*ext);

        QCOMPARE(decoded->fields().value(u"MandatoryMember"_s), 23.0);
        QCOMPARE(decoded->fields().value(u"OptionalMember"_s), 42.0);

        QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=4;i=6010"_s));
        WRITE_VALUE_ATTRIBUTE(node, *ext, QOpcUa::Types::ExtensionObject);
    }

    {
        auto value = handler.createGenericStructValueForTypeId(u"ns=4;i=3006"_s);
        value.fieldsRef()[u"MandatoryMember"_s] = 23.0;

        auto ext = handler.encode(value);
        QVERIFY(ext);
        QCOMPARE(ext->encodingTypeId(), value.structureDefinition().defaultEncodingId());

        const auto decoded = handler.decode(*ext);

        QCOMPARE(decoded->fields().value(u"MandatoryMember"_s), 23.0);
        QCOMPARE(decoded->fields().contains(u"OptionalMember"_s), false);

        QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=4;i=6002"_s));
        WRITE_VALUE_ATTRIBUTE(node, *ext, QOpcUa::Types::ExtensionObject);
    }

    {
        auto value = handler.createGenericStructValueForTypeId(u"ns=4;i=3009"_s);
        value.fieldsRef()[u"DiagnosticInfoMember"_s] = testDiagnosticInfos.at(0);
        value.fieldsRef()[u"DiagnosticInfoArrayMember"_s] = QVariant::fromValue(testDiagnosticInfos);

        auto ext = handler.encode(value);
        QVERIFY(ext);
        QCOMPARE(ext->encodingTypeId(), value.structureDefinition().defaultEncodingId());

        const auto decoded = handler.decode(*ext);
        QCOMPARE(decoded->fields().value(u"DiagnosticInfoMember"_s), testDiagnosticInfos.at(0));
        QCOMPARE(decoded->fields().value(u"DiagnosticInfoArrayMember"_s).value<QList<QOpcUaDiagnosticInfo>>(), testDiagnosticInfos);

        QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=4;i=6006"_s));
        WRITE_VALUE_ATTRIBUTE(node, *ext, QOpcUa::Types::ExtensionObject);
    }

    {
        auto value = handler.createGenericStructValueForTypeId(u"ns=4;i=3010"_s);
        value.fieldsRef()[u"DataValueMember"_s] = testDataValues.at(0);
        value.fieldsRef()[u"VariantMember"_s] = testVariants.at(0);

        auto ext = handler.encode(value);
        QVERIFY(ext);
        QCOMPARE(ext->encodingTypeId(), value.structureDefinition().defaultEncodingId());

        const auto decoded = handler.decode(*ext);

        QCOMPARE(decoded->fields().value(u"DataValueMember"_s), testDataValues.at(0));
        QCOMPARE(decoded->fields().value(u"VariantMember"_s), testVariants.at(0));

        QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=4;i=6027"_s));
        WRITE_VALUE_ATTRIBUTE(node, *ext, QOpcUa::Types::ExtensionObject);
    }

    {
        auto innermost = handler.createGenericStructValueForTypeId(u"ns=4;i=3012"_s);
        innermost.fieldsRef()[u"StringMember"_s] = u"Custom innermost string"_s;
        innermost.fieldsRef()[u"RecursiveArrayMember"_s] = QVariant::fromValue(QList<QOpcUaGenericStructValue>());

        auto inner = handler.createGenericStructValueForTypeId(u"ns=4;i=3012"_s);
        inner.fieldsRef()[u"StringMember"_s] = u"Custom inner string"_s;
        inner.fieldsRef()[u"RecursiveArrayMember"_s] = QVariant::fromValue(QList<QOpcUaGenericStructValue>{ innermost });

        auto value = handler.createGenericStructValueForTypeId(u"ns=4;i=3012"_s);
        value.fieldsRef()[u"StringMember"_s] = u"Custom outer string"_s;
        value.fieldsRef()[u"RecursiveArrayMember"_s] = QVariant::fromValue(QList<QOpcUaGenericStructValue>{ inner });

        auto ext = handler.encode(value);
        QVERIFY(ext);
        QCOMPARE(ext->encodingTypeId(), value.structureDefinition().defaultEncodingId());

        const auto decoded = handler.decode(*ext);

        QCOMPARE(decoded->fields().value(u"StringMember"_s), u"Custom outer string"_s);
        const auto innerDecoded = decoded->fields().value(u"RecursiveArrayMember"_s).value<QList<QOpcUaGenericStructValue>>();
        QCOMPARE(innerDecoded.size(), 1);
        QCOMPARE(innerDecoded.at(0).fields().value(u"StringMember"_s), u"Custom inner string"_s);
        const auto innermostDecoded = innerDecoded.at(0).fields().value(u"RecursiveArrayMember"_s).value<QList<QOpcUaGenericStructValue>>();
        QCOMPARE(innermostDecoded.size(), 1);
        QCOMPARE(innermostDecoded.at(0).fields().value(u"StringMember"_s), u"Custom innermost string"_s);

        QScopedPointer<QOpcUaNode> node(opcuaClient->node(u"ns=4;i=6029"_s));
        QVERIFY(node != nullptr);
        WRITE_VALUE_ATTRIBUTE(node, *ext, QOpcUa::Types::ExtensionObject);
    }
}

void Tst_QOpcUaClient::encodeCustomGenericStruct()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QOpcUaGenericStructHandler handler(opcuaClient);

    QSignalSpy spy(&handler, &QOpcUaGenericStructHandler::initializedChanged);

    auto success = handler.initialize();
    QCOMPARE(success, true);

    spy.wait();

    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);

    {
        QOpcUaStructureDefinition def;
        def.setStructureType(QOpcUaStructureDefinition::StructureType::Structure);
        def.setBaseDataType(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::BaseDataType));
        def.setDefaultEncodingId(u"ns=5;i=42"_s);

        QOpcUaStructureField field1;
        field1.setArrayDimensions({2, 2});
        field1.setValueRank(2);
        field1.setDataType(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Int32));
        field1.setIsOptional(false);
        field1.setName(u"MatrixMember"_s);

        def.setFields({field1});

        auto success = handler.addCustomStructureDefinition(def, u"ns=5;i=23"_s, u"MyMatrixTestStruct"_s);
        QVERIFY(success);

        auto value = handler.createGenericStructValueForTypeId(u"ns=5;i=23"_s);
        value.fieldsRef()[u"MatrixMember"_s] = QOpcUaMultiDimensionalArray({1, 2, 3, 4}, {2, 2});

        auto ext = handler.encode(value);
        QVERIFY(ext);
        QCOMPARE(ext->encodingTypeId(), value.structureDefinition().defaultEncodingId());

        const auto decoded = handler.decode(*ext);
        QVERIFY(success);

        const auto matrix = decoded->fields().value(u"MatrixMember"_s).value<QOpcUaMultiDimensionalArray>();
        QCOMPARE(matrix.arrayDimensions(), QList<quint32>({2, 2}));
        QCOMPARE(matrix.value(QList<quint32>({0, 0})), 1);
        QCOMPARE(matrix.value(QList<quint32>({0, 1})), 2);
        QCOMPARE(matrix.value(QList<quint32>({1, 0})), 3);
        QCOMPARE(matrix.value(QList<quint32>({1, 1})), 4);
    }

    {
        QOpcUaStructureDefinition def;
        def.setStructureType(QOpcUaStructureDefinition::StructureType::Structure);
        def.setBaseDataType(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::BaseDataType));
        def.setDefaultEncodingId(u"ns=5;i=43"_s);

        QOpcUaStructureField field1;
        field1.setArrayDimensions({2, 2});
        field1.setValueRank(2);
        field1.setDataType(u"ns=4;i=3004"_s);
        field1.setIsOptional(false);
        field1.setName(u"StructMatrixMember"_s);

        def.setFields({field1});

        auto success = handler.addCustomStructureDefinition(def, u"ns=5;i=24"_s, u"MyStructMatrixTestStruct"_s);
        QVERIFY(success);

        auto value = handler.createGenericStructValueForTypeId(u"ns=5;i=24"_s);

        auto innerValue1 = handler.createGenericStructValueForTypeId(u"ns=4;i=3004"_s);
        innerValue1.fieldsRef()[u"DoubleSubtypeMember"_s] = 1;
        auto innerValue2 = handler.createGenericStructValueForTypeId(u"ns=4;i=3004"_s);
        innerValue2.fieldsRef()[u"DoubleSubtypeMember"_s] = 2;
        auto innerValue3 = handler.createGenericStructValueForTypeId(u"ns=4;i=3004"_s);
        innerValue3.fieldsRef()[u"DoubleSubtypeMember"_s] = 3;
        auto innerValue4 = handler.createGenericStructValueForTypeId(u"ns=4;i=3004"_s);
        innerValue4.fieldsRef()[u"DoubleSubtypeMember"_s] = 4;

        value.fieldsRef()[u"StructMatrixMember"_s] =
            QOpcUaMultiDimensionalArray(QVariantList({innerValue1, innerValue2, innerValue3, innerValue4}), {2, 2});

        auto ext = handler.encode(value);
        QVERIFY(ext);
        QCOMPARE(ext->encodingTypeId(), value.structureDefinition().defaultEncodingId());

        const auto decoded = handler.decode(*ext);
        QVERIFY(decoded);

        const auto matrix = decoded->fields().value(u"StructMatrixMember"_s).value<QOpcUaMultiDimensionalArray>();
        QCOMPARE(matrix.arrayDimensions(), QList<quint32>({2, 2}));
        QCOMPARE(matrix.value(QList<quint32>({0, 0})).value<QOpcUaGenericStructValue>().fields().value(u"DoubleSubtypeMember"_s), 1);
        QCOMPARE(matrix.value(QList<quint32>({0, 1})).value<QOpcUaGenericStructValue>().fields().value(u"DoubleSubtypeMember"_s), 2);
        QCOMPARE(matrix.value(QList<quint32>({1, 0})).value<QOpcUaGenericStructValue>().fields().value(u"DoubleSubtypeMember"_s), 3);
        QCOMPARE(matrix.value(QList<quint32>({1, 1})).value<QOpcUaGenericStructValue>().fields().value(u"DoubleSubtypeMember"_s), 4);
    }
}

void Tst_QOpcUaClient::registerUnregisterNodes()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    const QStringList nodesToRegister { u"ns=2;s=Demo.Static.Scalar.Boolean"_s, u"ns=2;s=Demo.Static.Scalar.Int32"_s };

    QSignalSpy registerNodesSpy(opcuaClient, &QOpcUaClient::registerNodesFinished);
    opcuaClient->registerNodes(nodesToRegister);
    registerNodesSpy.wait(signalSpyTimeout);

    QCOMPARE(registerNodesSpy.size(), 1);
    QCOMPARE(registerNodesSpy.at(0).at(0), nodesToRegister);
    const auto registeredIds = registerNodesSpy.at(0).at(1).value<QStringList>();
    QCOMPARE(registeredIds.size(), 2);
    QCOMPARE(registerNodesSpy.at(0).at(2), QOpcUa::UaStatusCode::Good);

    QScopedPointer<QOpcUaNode> node1(opcuaClient->node(registeredIds.at(0)));
    QScopedPointer<QOpcUaNode> node2(opcuaClient->node(registeredIds.at(1)));
    QVERIFY(node1 != nullptr);
    QVERIFY(node2 != nullptr);

    READ_MANDATORY_BASE_NODE(node1);
    READ_MANDATORY_BASE_NODE(node2);

    QCOMPARE(node1->attribute(QOpcUa::NodeAttribute::BrowseName), QOpcUaQualifiedName(2, u"BoolScalarTest"_s));
    QCOMPARE(node2->attribute(QOpcUa::NodeAttribute::BrowseName), QOpcUaQualifiedName(2, u"Int32ScalarTest"_s));

    QSignalSpy unregisterNodesSpy(opcuaClient, &QOpcUaClient::unregisterNodesFinished);
    opcuaClient->unregisterNodes(registeredIds);
    unregisterNodesSpy.wait(signalSpyTimeout);

    QCOMPARE(unregisterNodesSpy.size(), 1);
    QCOMPARE(unregisterNodesSpy.at(0).at(0), registeredIds);
    QCOMPARE(unregisterNodesSpy.at(0).at(1), QOpcUa::UaStatusCode::Good);
}

void Tst_QOpcUaClient::connectionLost()
{
    // Restart the test server if necessary
    if (m_serverProcess.state() != QProcess::ProcessState::Running) {
        m_serverProcess.start(m_testServerPath);
        QVERIFY2(m_serverProcess.waitForStarted(), qPrintable(m_serverProcess.errorString()));
        QTest::qSleep(2000);
    }

    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QCOMPARE(opcuaClient->state(), QOpcUaClient::ClientState::Connected);

    QScopedPointer<QOpcUaNode> stringNode(opcuaClient->node(u"ns=2;s=Demo.Static.Scalar.String"_s));

    QSignalSpy stateSpy(opcuaClient, &QOpcUaClient::stateChanged);
    QSignalSpy readSpy(stringNode.data(), &QOpcUaNode::attributeRead);

    m_serverProcess.kill();
    m_serverProcess.waitForFinished();
    QCOMPARE(m_serverProcess.state(), QProcess::ProcessState::NotRunning);

    // Read twice to speed up broken pipe detection for the asynchronous requests
    stringNode->readAttributes(QOpcUa::NodeAttribute::BrowseName);
    stringNode->readAttributes(QOpcUa::NodeAttribute::BrowseName);

    readSpy.wait(signalSpyTimeout);
    if (stateSpy.size() != 2)
        stateSpy.wait(15000); // open62541 uses a timeout of 5 seconds for service calls, better be safe.
    QCOMPARE(readSpy.size(), 2);
    QVERIFY(readSpy.at(0).at(0).value<QOpcUa::NodeAttributes>() & QOpcUa::NodeAttribute::BrowseName);

    const auto statusCode = stringNode->attributeError(QOpcUa::NodeAttribute::BrowseName);
    const auto metaEnum = QMetaEnum::fromType<QOpcUa::UaStatusCode>();
    const auto stringCandidate = metaEnum.valueToKey(statusCode);
    const auto statusCodeString = u"Unexpected status code: %1"_s.arg(
                stringCandidate ? QString::fromUtf8(stringCandidate)
                                : QString::number(statusCode, 16));

    // open62541 returns a different status code depending on when after the disconnect the request is made
    QVERIFY2(statusCode == QOpcUa::UaStatusCode::BadInternalError ||
             statusCode == QOpcUa::UaStatusCode::BadSecureChannelClosed ||
             statusCode == QOpcUa::UaStatusCode::BadSessionClosed ||
             statusCode == QOpcUa::UaStatusCode::BadConnectionClosed ||
             statusCode == QOpcUa::UaStatusCode::BadServerNotConnected ||
             statusCode == QOpcUa::UaStatusCode::BadDisconnect,
             qUtf8Printable(statusCodeString));

    QCOMPARE(stateSpy.size(), 1);
    QCOMPARE(stateSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::ClientState::Disconnected);
}

void Tst_QOpcUaClient::cleanupTestCase()
{
    if (m_serverProcess.state() == QProcess::Running) {
        m_serverProcess.kill();
        m_serverProcess.waitForFinished(2000);
    }
}

void Tst_QOpcUaClient::loadInvalidPlugin()
{
    QOpcUaProvider provider;
    QScopedPointer<QOpcUaClient> client(provider.createClient(u"doesnotexist"_s));
    QCOMPARE(client, nullptr);
}

int main(int argc, char *argv[])
{
    populateDataTypeDefinitionTestData();
    populateDiagnosticInfoTestData();
    populateVariantTestData();
    populateDataValuesTestData();
    populateEventFilterTestData();

    QCoreApplication app(argc, argv);

    QTEST_SET_MAIN_SOURCE_PATH

    // run tests for all available backends
    QStringList availableBackends = QOpcUaProvider::availableBackends();
    if (availableBackends.empty()) {
        qDebug("No OPCUA backends found, skipping tests.");
        return EXIT_SUCCESS;
    }

    Tst_QOpcUaClient tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "tst_client.moc"


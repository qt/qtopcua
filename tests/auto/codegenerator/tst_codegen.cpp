// Copyright (C) 2024 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <generated/codegentestbinarydeencoder.h>
#include <generated/codegentestdatatypes.h>
#include <generated/codegentestnodeids.h>
#include <generated2/csvonlytestnodeids.h>

#include <QtOpcUa/QOpcUaClient>
#include <QtOpcUa/QOpcUaNode>
#include <QtOpcUa/QOpcUaProvider>

#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtCore/QScopedPointer>
#include <QtNetwork/QTcpServer>
#include <QtNetwork//QTcpSocket>
#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>

using namespace Qt::Literals::StringLiterals;

const auto signalSpyTimeout = std::chrono::milliseconds(10000);

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
                } while (t.durationElapsed() < signalSpyTimeout && stateSpy.size() < 2);
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

#define defineDataMethod(name) void name()\
{\
    QTest::addColumn<QOpcUaClient *>("opcuaClient");\
    for (auto *client : std::as_const(m_clients))\
        QTest::newRow(client->backend().toLatin1().constData()) << client;\
}

class Tst_GeneratedDataTypes : public QObject
{
    Q_OBJECT

public:
    Tst_GeneratedDataTypes();

private slots:
    void initTestCase();
    void cleanupTestCase();

    defineDataMethod(checkGeneratedDecoding_data)
    void checkGeneratedDecoding();
    defineDataMethod(checkGeneratedEncoding_data)
    void checkGeneratedEncoding();
    defineDataMethod(testNodeIdsOnly_data)
    void testNodeIdsOnly();

private:
    QString envOrDefault(const char *env, QString def)
    {
        return qEnvironmentVariableIsSet(env) ? QString::fromUtf8(qgetenv(env).constData()) : def;
    }

    QString makeTestModelId(TestModelNodeId id)
    {
        return QOpcUa::nodeIdFromInteger(4, static_cast<int>(id));
    }

    QString m_discoveryEndpoint;
    QOpcUaProvider m_opcUa;
    QStringList m_backends;
    QList<QOpcUaClient *> m_clients;
    QProcess m_serverProcess;
    QString m_testServerPath;
    QOpcUaEndpointDescription m_endpoint;
};

#define READ_VARIABLE_NODE_VALUE(NODE) \
{ \
    QSignalSpy attributeReadSpy(NODE.data(), &QOpcUaNode::attributeRead);\
    NODE->readAttributes(QOpcUa::NodeAttribute::Value); \
    attributeReadSpy.wait(signalSpyTimeout); \
    QCOMPARE(attributeReadSpy.count(), 1); \
    QCOMPARE(attributeReadSpy.at(0).at(0).value<QOpcUa::NodeAttributes>(), QOpcUa::NodeAttribute::Value); \
    QCOMPARE(NODE->attributeError(QOpcUa::NodeAttribute::Value), QOpcUa::UaStatusCode::Good); \
}

#define WRITE_VALUE_ATTRIBUTE(NODE, VALUE, TYPE) \
{ \
    QSignalSpy resultSpy(NODE.data(), &QOpcUaNode::attributeWritten); \
    NODE->writeAttribute(QOpcUa::NodeAttribute::Value, VALUE, TYPE); \
    resultSpy.wait(signalSpyTimeout); \
    QCOMPARE(resultSpy.size(), 1); \
    QCOMPARE(resultSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value); \
    QCOMPARE(resultSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good); \
}

Tst_GeneratedDataTypes::Tst_GeneratedDataTypes()
{
    m_backends = QOpcUaProvider::availableBackends();
}

void Tst_GeneratedDataTypes::initTestCase()
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

void Tst_GeneratedDataTypes::checkGeneratedDecoding()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    // Check against the default values of the test nodes on the server
    // They must not have been modified by previous test runs if OPCUA_HOST and OPCUA_PORT is used

    {
        QScopedPointer<QOpcUaNode> node(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_NestedStruct)));
        QVERIFY(node != nullptr);
        READ_VARIABLE_NODE_VALUE(node);

        auto ext = node->valueAttribute().value<QOpcUaExtensionObject>();
        QCOMPARE(ext.encodingTypeId(), makeTestModelId(TestModelNodeId::QtTestStructType_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder codec(ext);

        bool success = false;
        const auto decoded = codec.decode<CodegenTestQtTestStructType>(success);
        QVERIFY(success);

        QCOMPARE(decoded.stringMember(), u"TestString"_s);
        QCOMPARE(decoded.localizedTextMember(), QOpcUaLocalizedText(u"en"_s, u"TestText"_s));
        QCOMPARE(decoded.qualifiedNameMember(), QOpcUaQualifiedName(1, u"TestName"_s));
        QCOMPARE(decoded.int64ArrayMember(), QList<qint64>({ std::numeric_limits<qint64>::max(),
                                                            std::numeric_limits<qint64>::max() - 1,
                                                            std::numeric_limits<qint64>::min() }));
        QCOMPARE(decoded.nestedStructMember().doubleSubtypeMember(), 42);
        QCOMPARE(decoded.nestedStructArrayMember().size(), 2);
        QCOMPARE(decoded.nestedStructArrayMember().at(0).doubleSubtypeMember(), 23);
        QCOMPARE(decoded.nestedStructArrayMember().at(1).doubleSubtypeMember(), 42);
    }

    {
        QScopedPointer<QOpcUaNode> node(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_UnionWithFirstMember)));
        QVERIFY(node != nullptr);
        READ_VARIABLE_NODE_VALUE(node);

        auto ext = node->valueAttribute().value<QOpcUaExtensionObject>();
        QCOMPARE(ext.encodingTypeId(), makeTestModelId(TestModelNodeId::QtTestUnionType_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder codec(ext);

        bool success = false;
        const auto decoded = codec.decode<CodegenTestQtTestUnionType>(success);
        QVERIFY(success);
        QCOMPARE(decoded.switchField(), CodegenTestQtTestUnionType::SwitchField::Member1);
        QCOMPARE(decoded.member1(), 42);
    }

    {
        QScopedPointer<QOpcUaNode> node(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_UnionWithSecondMember)));
        QVERIFY(node != nullptr);
        READ_VARIABLE_NODE_VALUE(node);

        auto ext = node->valueAttribute().value<QOpcUaExtensionObject>();
        QCOMPARE(ext.encodingTypeId(), makeTestModelId(TestModelNodeId::QtTestUnionType_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder codec(ext);

        bool success = false;
        const auto decoded = codec.decode<CodegenTestQtTestUnionType>(success);
        QVERIFY(success);
        QCOMPARE(decoded.switchField(), CodegenTestQtTestUnionType::SwitchField::Member2);
        QCOMPARE(decoded.member2().doubleSubtypeMember(), 23);
    }

    {
        QScopedPointer<QOpcUaNode> node(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_StructWithOptionalField)));
        QVERIFY(node != nullptr);
        READ_VARIABLE_NODE_VALUE(node);

        auto ext = node->valueAttribute().value<QOpcUaExtensionObject>();
        QCOMPARE(ext.encodingTypeId(), makeTestModelId(TestModelNodeId::QtStructWithOptionalFieldType_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder codec(ext);

        bool success = false;
        const auto decoded = codec.decode<CodegenTestQtStructWithOptionalFieldType>(success);
        QVERIFY(success);
        QCOMPARE(decoded.mandatoryMember(), 42);
        QVERIFY(decoded.optionalMemberSpecified());
        QCOMPARE(decoded.optionalMember(), 23);
    }

    {
        QScopedPointer<QOpcUaNode> node(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_StructWihoutOptionalField)));
        QVERIFY(node != nullptr);
        READ_VARIABLE_NODE_VALUE(node);

        auto ext = node->valueAttribute().value<QOpcUaExtensionObject>();
        QCOMPARE(ext.encodingTypeId(), makeTestModelId(TestModelNodeId::QtStructWithOptionalFieldType_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder codec(ext);

        bool success = false;
        const auto decoded = codec.decode<CodegenTestQtStructWithOptionalFieldType>(success);
        QVERIFY(success);
        QCOMPARE(decoded.mandatoryMember(), 42);
        QVERIFY(!decoded.optionalMemberSpecified());
    }

    {
        QScopedPointer<QOpcUaNode> node(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_StructWithDiagnosticInfo)));
        QVERIFY(node != nullptr);
        READ_VARIABLE_NODE_VALUE(node);

        auto ext = node->valueAttribute().value<QOpcUaExtensionObject>();
        QCOMPARE(ext.encodingTypeId(), makeTestModelId(TestModelNodeId::QtTestStructWithDiagnosticInfo_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder codec(ext);

        bool success = false;
        const auto decoded = codec.decode<CodegenTestQtTestStructWithDiagnosticInfo>(success);
        QVERIFY(success);

        QCOMPARE(decoded.diagnosticInfoMember().hasSymbolicId(), true);
        QCOMPARE(decoded.diagnosticInfoMember().symbolicId(), 1);
        QCOMPARE(decoded.diagnosticInfoMember().hasNamespaceUri(), true);
        QCOMPARE(decoded.diagnosticInfoMember().namespaceUri(), 2);
        QCOMPARE(decoded.diagnosticInfoMember().hasLocalizedText(), true);
        QCOMPARE(decoded.diagnosticInfoMember().localizedText(), 4); // FIXME Swapped with locale due to a bug in open62541
        QCOMPARE(decoded.diagnosticInfoMember().hasLocale(), true);
        QCOMPARE(decoded.diagnosticInfoMember().locale(), 3); // FIXME Swapped with localizedText due to a bug in open62541
        QCOMPARE(decoded.diagnosticInfoMember().hasAdditionalInfo(), true);
        QCOMPARE(decoded.diagnosticInfoMember().additionalInfo(), u"My additional info"_s);
        QCOMPARE(decoded.diagnosticInfoMember().hasInnerStatusCode(), true);
        QCOMPARE(decoded.diagnosticInfoMember().innerStatusCode(), QOpcUa::UaStatusCode::BadInternalError);
        QCOMPARE(decoded.diagnosticInfoMember().hasInnerDiagnosticInfo(), true);

        QCOMPARE(decoded.diagnosticInfoMember().innerDiagnosticInfo().hasAdditionalInfo(), true);
        QCOMPARE(decoded.diagnosticInfoMember().innerDiagnosticInfo().additionalInfo(), u"My inner additional info"_s);

        QCOMPARE(decoded.diagnosticInfoArrayMember().size(), 2);
        QCOMPARE(decoded.diagnosticInfoArrayMember().at(0), decoded.diagnosticInfoMember());
        QCOMPARE(decoded.diagnosticInfoArrayMember().at(1).hasLocale(), true);
        QCOMPARE(decoded.diagnosticInfoArrayMember().at(1).locale(), 1);
        QCOMPARE(decoded.diagnosticInfoArrayMember().at(1).hasInnerStatusCode(), true);
        QCOMPARE(decoded.diagnosticInfoArrayMember().at(1).innerStatusCode(), QOpcUa::UaStatusCode::BadTypeMismatch);
    }

    {
        QScopedPointer<QOpcUaNode> node(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_StructWithDataValue)));
        QVERIFY(node != nullptr);
        READ_VARIABLE_NODE_VALUE(node);

        auto ext = node->valueAttribute().value<QOpcUaExtensionObject>();
        QCOMPARE(ext.encodingTypeId(), makeTestModelId(TestModelNodeId::QtTestStructWithDataValue_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder codec(ext);

        bool success = false;
        const auto decoded = codec.decode<CodegenTestQtTestStructWithDataValue>(success);
        QVERIFY(success);

        const QStringList expectedValue = {
            u"TestString 1"_s, u"TestString 2"_s,
            u"TestString 3"_s, u"TestString 4"_s
        };

        const auto variant = decoded.dataValueMember().value().value<QOpcUaVariant>();
        QCOMPARE(variant.isArray(), true);
        QList<qint32> expectedDimensions = {2, 2};
        QCOMPARE(variant.arrayDimensions(), expectedDimensions);
        QCOMPARE(variant.type(), QOpcUaVariant::ValueType::String);
        QCOMPARE(variant.value(), expectedValue);

        QCOMPARE(decoded.dataValueMember().statusCode(), QOpcUa::UaStatusCode::BadInternalError);
        QCOMPARE(decoded.dataValueMember().serverTimestamp(), QDateTime::fromMSecsSinceEpoch(1698655307000));
        QCOMPARE(decoded.dataValueMember().sourceTimestamp(), QDateTime::fromMSecsSinceEpoch(1698655306000));
        QCOMPARE(decoded.dataValueMember().serverPicoseconds(), 23);
        QCOMPARE(decoded.dataValueMember().sourcePicoseconds(), 42);

        QCOMPARE(decoded.variantMember().isArray(), false);
        QVERIFY(decoded.variantMember().arrayDimensions().empty());
        QCOMPARE(decoded.variantMember().type(), QOpcUaVariant::ValueType::UInt64);
        QCOMPARE(decoded.variantMember().value().value<quint64>(), 42);
    }

    {
        QScopedPointer<QOpcUaNode> node(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_RecursiveStruct)));
        QVERIFY(node != nullptr);
        READ_VARIABLE_NODE_VALUE(node);

        auto ext = node->valueAttribute().value<QOpcUaExtensionObject>();
        QCOMPARE(ext.encodingTypeId(), makeTestModelId(TestModelNodeId::QtRecursiveTestStruct_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder codec(ext);

        bool success = false;
        const auto decoded = codec.decode<CodegenTestQtRecursiveTestStruct>(success);
        QVERIFY(success);

        QCOMPARE(decoded.stringMember(), u"Outer string"_s);
        QCOMPARE(decoded.recursiveArrayMember().size(), 2);
        QCOMPARE(decoded.recursiveArrayMember().at(0).stringMember(), u"Nested string 1"_s);
        QCOMPARE(decoded.recursiveArrayMember().at(0).recursiveArrayMember().size(), 1);
        QCOMPARE(decoded.recursiveArrayMember().at(0).recursiveArrayMember().at(0).stringMember(), u"Innermost string"_s);
        QCOMPARE(decoded.recursiveArrayMember().at(0).recursiveArrayMember().at(0).recursiveArrayMember().size(), 0);
        QCOMPARE(decoded.recursiveArrayMember().at(1).stringMember(), u"Nested string 2"_s);
        QCOMPARE(decoded.recursiveArrayMember().at(1).recursiveArrayMember().size(), 0);
    }
}

void Tst_GeneratedDataTypes::checkGeneratedEncoding()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);


    {
        QScopedPointer<QOpcUaNode> node(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_NestedStruct)));
        QVERIFY(node != nullptr);

        CodegenTestQtTestStructType data;
        data.setStringMember(u"TestString"_s);
        data.setLocalizedTextMember(QOpcUaLocalizedText(u"en"_s, u"TestText"_s));
        data.setQualifiedNameMember(QOpcUaQualifiedName(1, u"TestName"_s));
        data.setInt64ArrayMember({ std::numeric_limits<qint64>::max(),
                                                            std::numeric_limits<qint64>::max() - 1,
                                                            std::numeric_limits<qint64>::min() });

        CodegenTestQtInnerTestStructType i1, i2;
        i1.setDoubleSubtypeMember(24.0);
        i2.setDoubleSubtypeMember(43.0);

        data.setNestedStructMember(i2);
        data.setNestedStructArrayMember({ i1, i2 });

        QOpcUaExtensionObject ext(makeTestModelId(TestModelNodeId::QtTestStructType_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder encoder(ext);
        QCOMPARE(encoder.encode<CodegenTestQtTestStructType>(data), true);

        WRITE_VALUE_ATTRIBUTE(node, ext, QOpcUa::Types::ExtensionObject);
        node.reset(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_NestedStruct)));
        QVERIFY(node != nullptr);
        READ_VARIABLE_NODE_VALUE(node);

        auto ext2 = node->valueAttribute().value<QOpcUaExtensionObject>();
        QCOMPARE(ext2.encodingTypeId(), makeTestModelId(TestModelNodeId::QtTestStructType_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder decoder(ext2);

        bool success = false;
        const auto decoded = decoder.decode<CodegenTestQtTestStructType>(success);
        QVERIFY(success);
        QCOMPARE(decoded, data);
    }

    {
        QScopedPointer<QOpcUaNode> node(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_UnionWithFirstMember)));
        QVERIFY(node != nullptr);

        CodegenTestQtTestUnionType data;
        data.setMember1(43);

        QOpcUaExtensionObject ext(makeTestModelId(TestModelNodeId::QtTestUnionType_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder encoder(ext);
        QCOMPARE(encoder.encode<CodegenTestQtTestUnionType>(data), true);

        WRITE_VALUE_ATTRIBUTE(node, ext, QOpcUa::Types::ExtensionObject);
        node.reset(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_UnionWithFirstMember)));
        QVERIFY(node != nullptr);
        READ_VARIABLE_NODE_VALUE(node);

        auto ext2 = node->valueAttribute().value<QOpcUaExtensionObject>();
        QCOMPARE(ext2.encodingTypeId(), makeTestModelId(TestModelNodeId::QtTestUnionType_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder decoder(ext2);

        bool success = false;
        const auto decoded = decoder.decode<CodegenTestQtTestUnionType>(success);
        QVERIFY(success);
        QCOMPARE(decoded, data);
    }

    {
        QScopedPointer<QOpcUaNode> node(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_UnionWithSecondMember)));
        QVERIFY(node != nullptr);

        CodegenTestQtInnerTestStructType inner;
        inner.setDoubleSubtypeMember(43);
        CodegenTestQtTestUnionType data;
        data.setMember2(inner);

        QOpcUaExtensionObject ext(makeTestModelId(TestModelNodeId::QtTestUnionType_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder encoder(ext);
        QCOMPARE(encoder.encode<CodegenTestQtTestUnionType>(data), true);

        WRITE_VALUE_ATTRIBUTE(node, ext, QOpcUa::Types::ExtensionObject);
        node.reset(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_UnionWithSecondMember)));
        QVERIFY(node != nullptr);
        READ_VARIABLE_NODE_VALUE(node);

        auto ext2 = node->valueAttribute().value<QOpcUaExtensionObject>();
        QCOMPARE(ext2.encodingTypeId(), makeTestModelId(TestModelNodeId::QtTestUnionType_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder decoder(ext2);

        bool success = false;
        const auto decoded = decoder.decode<CodegenTestQtTestUnionType>(success);
        QVERIFY(success);
        QCOMPARE(decoded, data);
    }

    {
        QScopedPointer<QOpcUaNode> node(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_StructWithOptionalField)));
        QVERIFY(node != nullptr);

        CodegenTestQtStructWithOptionalFieldType data;
        data.setMandatoryMember(24);
        data.setOptionalMemberSpecified(true);
        data.setOptionalMember(43);

        QOpcUaExtensionObject ext(makeTestModelId(TestModelNodeId::QtStructWithOptionalFieldType_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder encoder(ext);
        QCOMPARE(encoder.encode<CodegenTestQtStructWithOptionalFieldType>(data), true);

        WRITE_VALUE_ATTRIBUTE(node, ext, QOpcUa::Types::ExtensionObject);
        node.reset(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_StructWithOptionalField)));
        QVERIFY(node != nullptr);
        READ_VARIABLE_NODE_VALUE(node);

        auto ext2 = node->valueAttribute().value<QOpcUaExtensionObject>();
        QCOMPARE(ext2.encodingTypeId(), makeTestModelId(TestModelNodeId::QtStructWithOptionalFieldType_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder decoder(ext2);

        bool success = false;
        const auto decoded = decoder.decode<CodegenTestQtStructWithOptionalFieldType>(success);
        QVERIFY(success);
        QCOMPARE(decoded, data);
    }

    {
        QScopedPointer<QOpcUaNode> node(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_StructWihoutOptionalField)));
        QVERIFY(node != nullptr);

        CodegenTestQtStructWithOptionalFieldType data;
        data.setMandatoryMember(24);

        QOpcUaExtensionObject ext(makeTestModelId(TestModelNodeId::QtStructWithOptionalFieldType_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder encoder(ext);
        QCOMPARE(encoder.encode<CodegenTestQtStructWithOptionalFieldType>(data), true);

        WRITE_VALUE_ATTRIBUTE(node, ext, QOpcUa::Types::ExtensionObject);
        node.reset(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_StructWihoutOptionalField)));
        QVERIFY(node != nullptr);
        READ_VARIABLE_NODE_VALUE(node);

        auto ext2 = node->valueAttribute().value<QOpcUaExtensionObject>();
        QCOMPARE(ext2.encodingTypeId(), makeTestModelId(TestModelNodeId::QtStructWithOptionalFieldType_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder decoder(ext2);

        bool success = false;
        const auto decoded = decoder.decode<CodegenTestQtStructWithOptionalFieldType>(success);
        QVERIFY(success);
        QCOMPARE(decoded, data);
    }

    {
        QScopedPointer<QOpcUaNode> node(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_StructWithDiagnosticInfo)));
        QVERIFY(node != nullptr);

        CodegenTestQtTestStructWithDiagnosticInfo data;

        QOpcUaDiagnosticInfo di1;
        di1.setHasSymbolicId(true);
        di1.setSymbolicId(2);
        di1.setHasNamespaceUri(true);
        di1.setNamespaceUri(3);
        di1.setHasLocalizedText(true);
        di1.setLocalizedText(4);
        di1.setHasLocale(true);
        di1.setLocale(5);
        di1.setHasAdditionalInfo(true);
        di1.setAdditionalInfo(u"My additional info"_s);
        di1.setHasInnerStatusCode(true);
        di1.setInnerStatusCode(QOpcUa::UaStatusCode::BadInternalError);
        di1.setHasInnerDiagnosticInfo(true);

        QOpcUaDiagnosticInfo inner;
        inner.setHasAdditionalInfo(true);
        inner.setAdditionalInfo(u"My inner additional info"_s);
        di1.setInnerDiagnosticInfo(inner);

        QOpcUaDiagnosticInfo di2;
        di2.setHasLocale(true);
        di2.setLocale(1);
        di2.setHasInnerStatusCode(true);
        di2.setInnerStatusCode(QOpcUa::UaStatusCode::BadAlreadyExists);

        data.setDiagnosticInfoMember(di1);
        data.setDiagnosticInfoArrayMember({ di1, di2 });

        QOpcUaExtensionObject ext(makeTestModelId(TestModelNodeId::QtTestStructWithDiagnosticInfo_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder encoder(ext);
        QCOMPARE(encoder.encode<CodegenTestQtTestStructWithDiagnosticInfo>(data), true);

        WRITE_VALUE_ATTRIBUTE(node, ext, QOpcUa::Types::ExtensionObject);
        node.reset(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_StructWithDiagnosticInfo)));
        QVERIFY(node != nullptr);
        READ_VARIABLE_NODE_VALUE(node);

        auto ext2 = node->valueAttribute().value<QOpcUaExtensionObject>();
        QCOMPARE(ext2.encodingTypeId(), makeTestModelId(TestModelNodeId::QtTestStructWithDiagnosticInfo_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder decoder(ext2);

        bool success = false;
        const auto decoded = decoder.decode<CodegenTestQtTestStructWithDiagnosticInfo>(success);
        QVERIFY(success);
        QCOMPARE(decoded, data);
    }

    {
        QScopedPointer<QOpcUaNode> node(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_StructWithDataValue)));
        QVERIFY(node != nullptr);

        CodegenTestQtTestStructWithDataValue data;

        QOpcUaDataValue dv;
        dv.setValue(QOpcUaVariant(QOpcUaVariant::ValueType::Double, 23.0));
        dv.setStatusCode(QOpcUa::UaStatusCode::UncertainInitialValue);
        dv.setSourceTimestamp(QDateTime::currentDateTime());
        dv.setSourcePicoseconds(24);
        dv.setServerTimestamp(QDateTime::currentDateTime());
        dv.setServerPicoseconds(43);
        data.setDataValueMember(dv);

        QOpcUaVariant var;
        var.setArrayDimensions({ 3, 3});
        var.setValue(QOpcUaVariant::ValueType::Double,
                     QVariant::fromValue(QList<double>{ 1, 2, 3, 4, 5, 6, 7, 8, 9 }));
        data.setVariantMember(var);

        QOpcUaExtensionObject ext(makeTestModelId(TestModelNodeId::QtTestStructWithDataValue_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder encoder(ext);
        QCOMPARE(encoder.encode<CodegenTestQtTestStructWithDataValue>(data), true);

        WRITE_VALUE_ATTRIBUTE(node, ext, QOpcUa::Types::ExtensionObject);
        node.reset(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_StructWithDataValue)));
        QVERIFY(node != nullptr);
        READ_VARIABLE_NODE_VALUE(node);

        auto ext2 = node->valueAttribute().value<QOpcUaExtensionObject>();
        QCOMPARE(ext2.encodingTypeId(), makeTestModelId(TestModelNodeId::QtTestStructWithDataValue_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder decoder(ext2);

        bool success = false;
        const auto decoded = decoder.decode<CodegenTestQtTestStructWithDataValue>(success);
        QVERIFY(success);
        QCOMPARE(decoded, data);
    }

    {
        QScopedPointer<QOpcUaNode> node(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_RecursiveStruct)));
        QVERIFY(node != nullptr);

        CodegenTestQtRecursiveTestStruct data;
        CodegenTestQtRecursiveTestStruct nested1;
        CodegenTestQtRecursiveTestStruct innermost;
        CodegenTestQtRecursiveTestStruct nested2;

        data.setStringMember(u"My outer string"_s);
        nested1.setStringMember(u"My nested string 1"_s);
        nested2.setStringMember(u"My nested string 2"_s);
        innermost.setStringMember(u"My innermost string"_s);
        nested2.setRecursiveArrayMember({ innermost });
        data.setRecursiveArrayMember({ nested1, nested2 });

        QOpcUaExtensionObject ext(makeTestModelId(TestModelNodeId::QtRecursiveTestStruct_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder encoder(ext);
        QCOMPARE(encoder.encode<CodegenTestQtRecursiveTestStruct>(data), true);

        WRITE_VALUE_ATTRIBUTE(node, ext, QOpcUa::Types::ExtensionObject);
        node.reset(opcuaClient->node(makeTestModelId(TestModelNodeId::DecoderTestNodes_RecursiveStruct)));
        QVERIFY(node != nullptr);
        READ_VARIABLE_NODE_VALUE(node);

        auto ext2 = node->valueAttribute().value<QOpcUaExtensionObject>();
        QCOMPARE(ext2.encodingTypeId(), makeTestModelId(TestModelNodeId::QtRecursiveTestStruct_Encoding_DefaultBinary));
        CodegenTestBinaryDeEncoder decoder(ext2);

        bool success = false;
        const auto decoded = decoder.decode<CodegenTestQtRecursiveTestStruct>(success);
        QVERIFY(success);
        QCOMPARE(decoded, data);
    }
}

void Tst_GeneratedDataTypes::testNodeIdsOnly()
{
    QCOMPARE(static_cast<int>(CsvOnlyTestNodeId::Unknown), 0);
    QCOMPARE(static_cast<int>(CsvOnlyTestNodeId::TestId1), 1);
    QCOMPARE(static_cast<int>(CsvOnlyTestNodeId::TestId2), 2);
    QCOMPARE(static_cast<int>(CsvOnlyTestNodeId::TestId3), 3);
}

void Tst_GeneratedDataTypes::cleanupTestCase()
{
    if (m_serverProcess.state() == QProcess::Running) {
        m_serverProcess.kill();
        m_serverProcess.waitForFinished(2000);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QTEST_SET_MAIN_SOURCE_PATH

    // run tests for all available backends
    QStringList availableBackends = QOpcUaProvider::availableBackends();
    if (availableBackends.empty()) {
        qDebug("No OPCUA backends found, skipping tests.");
        return EXIT_SUCCESS;
    }

    Tst_GeneratedDataTypes tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "tst_codegen.moc"


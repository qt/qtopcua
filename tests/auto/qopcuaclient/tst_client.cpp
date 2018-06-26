/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtOpcUa/QOpcUaClient>
#include <QtOpcUa/QOpcUaNode>
#include <QtOpcUa/QOpcUaProvider>

#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtCore/QScopedPointer>
#include <QtCore/QThread>
#include <QtCore/QTimer>

#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>

class OpcuaConnector
{
public:
    OpcuaConnector(QOpcUaClient *client, const QString &endPoint)
        : opcuaClient(client)
    {
        QVERIFY(opcuaClient != nullptr);
        QSignalSpy connectedSpy(opcuaClient, &QOpcUaClient::connected);
        QSignalSpy disconnectedSpy(opcuaClient, &QOpcUaClient::disconnected);
        QSignalSpy stateSpy(opcuaClient, &QOpcUaClient::stateChanged);

        QTest::qWait(500);
        opcuaClient->connectToEndpoint(QUrl(endPoint));
        QTRY_VERIFY2(opcuaClient->state() == QOpcUaClient::Connected, "Could not connect to server");

        QCOMPARE(connectedSpy.count(), 1); // one connected signal fired
        QCOMPARE(disconnectedSpy.count(), 0); // zero disconnected signals fired
        QCOMPARE(stateSpy.count(), 2);

        QCOMPARE(stateSpy.at(0).at(0).value<QOpcUaClient::ClientState>(),
                 QOpcUaClient::ClientState::Connecting);
        QCOMPARE(stateSpy.at(1).at(0).value<QOpcUaClient::ClientState>(),
                 QOpcUaClient::ClientState::Connected);

        stateSpy.clear();
        connectedSpy.clear();
        disconnectedSpy.clear();

        QVERIFY(opcuaClient->url() == QUrl(endPoint));
    }

    ~OpcuaConnector()
    {
        QSignalSpy connectedSpy(opcuaClient, &QOpcUaClient::connected);
        QSignalSpy disconnectedSpy(opcuaClient, &QOpcUaClient::disconnected);
        QSignalSpy stateSpy(opcuaClient, &QOpcUaClient::stateChanged);
        QSignalSpy errorSpy(opcuaClient, &QOpcUaClient::errorChanged);

        QVERIFY(opcuaClient != nullptr);
        if (opcuaClient->state() == QOpcUaClient::Connected) {

            opcuaClient->disconnectFromEndpoint();

            QTRY_VERIFY(opcuaClient->state() == QOpcUaClient::Disconnected);

            QCOMPARE(connectedSpy.count(), 0);
            QCOMPARE(disconnectedSpy.count(), 1);
            QCOMPARE(stateSpy.count(), 2);
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

const QString readWriteNode = QStringLiteral("ns=3;s=TestNode.ReadWrite");
const QVector<QString> xmlElements = {
    QStringLiteral("<?xml version=\"1\" encoding=\"UTF-8\"?>"),
    QStringLiteral("<?xml version=\"2\" encoding=\"UTF-8\"?>"),
    QStringLiteral("<?xml version=\"3\" encoding=\"UTF-8\"?>")};
const QVector<QOpcUa::QLocalizedText> localizedTexts = {
    QOpcUa::QLocalizedText("en", "English"),
    QOpcUa::QLocalizedText("de", "German"),
    QOpcUa::QLocalizedText("fr", "French")};
const QVector<QOpcUa::QRange> testRanges = {
    QOpcUa::QRange(-100, 100),
    QOpcUa::QRange(0, 100),
    QOpcUa::QRange(-200, -100)
};
const QVector<QOpcUa::QEUInformation> testEUInfos = {
    QOpcUa::QEUInformation(QLatin1String("http://www.opcfoundation.org/UA/units/un/cefact"),
        4408652, QOpcUa::QLocalizedText(QString(), QStringLiteral("°C")), QOpcUa::QLocalizedText(QString(), QLatin1String("degree Celsius"))),
    QOpcUa::QEUInformation(QLatin1String("http://www.opcfoundation.org/UA/units/un/cefact"),
        4604232, QOpcUa::QLocalizedText(QString(), QStringLiteral("°F")), QOpcUa::QLocalizedText(QString(), QLatin1String("degree Fahrenheit"))),
    QOpcUa::QEUInformation(QLatin1String("http://www.opcfoundation.org/UA/units/un/cefact"),
        5067858, QOpcUa::QLocalizedText(QString(), QLatin1String("m")), QOpcUa::QLocalizedText(QString(), QLatin1String("metre"))),
};
const QVector<QOpcUa::QComplexNumber> testComplex = {
    QOpcUa::QComplexNumber(1,2),
    QOpcUa::QComplexNumber(-1,2),
    QOpcUa::QComplexNumber(1,-2)
};
const QVector<QOpcUa::QDoubleComplexNumber> testDoubleComplex = {
    QOpcUa::QDoubleComplexNumber(1,2),
    QOpcUa::QDoubleComplexNumber(-1,2),
    QOpcUa::QDoubleComplexNumber(1,-2)
};
const QVector<QOpcUa::QAxisInformation> testAxisInfo = {
    QOpcUa::QAxisInformation(testEUInfos[0], testRanges[0], localizedTexts[0], QOpcUa::AxisScale::Linear, QVector<double>({1, 2, 3})),
    QOpcUa::QAxisInformation(testEUInfos[1], testRanges[1], localizedTexts[1], QOpcUa::AxisScale::Ln, QVector<double>({4, 5, 6})),
    QOpcUa::QAxisInformation(testEUInfos[2], testRanges[2], localizedTexts[2], QOpcUa::AxisScale::Log, QVector<double>({7, 8, 9}))
};
const QVector<QOpcUa::QXValue> testXV = {
    QOpcUa::QXValue(0, 100),
    QOpcUa::QXValue(-10, 100.5),
    QOpcUa::QXValue(10, -100.5)
};
const int numberOfOperations = 1000;

#define defineDataMethod(name) void name()\
{\
    QTest::addColumn<QOpcUaClient *>("opcuaClient");\
    for (auto *client: m_clients)\
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

    // connect & disconnect
    defineDataMethod(connectToInvalid_data)
    void connectToInvalid();
    defineDataMethod(connectAndDisconnect_data)
    void connectAndDisconnect();

    // Password
    defineDataMethod(connectInvalidPassword_data)
    void connectInvalidPassword();
    defineDataMethod(connectAndDisconnectPassword_data)
    void connectAndDisconnectPassword();

    defineDataMethod(readInvalidNode_data)
    void readInvalidNode();
    defineDataMethod(writeInvalidNode_data)
    void writeInvalidNode();
    defineDataMethod(writeMultipleAttributes_data)
    void writeMultipleAttributes();
    defineDataMethod(readEmptyArrayVariable_data)
    void readEmptyArrayVariable();

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

    defineDataMethod(dataChangeSubscription_data)
    void dataChangeSubscription();
    defineDataMethod(dataChangeSubscriptionInvalidNode_data)
    void dataChangeSubscriptionInvalidNode();
    defineDataMethod(dataChangeSubscriptionSharing_data)
    void dataChangeSubscriptionSharing();
    defineDataMethod(methodCall_data)
    void methodCall();
    defineDataMethod(methodCallInvalid_data)
    void methodCallInvalid();
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
    defineDataMethod(checkMonitoredItemCleanup_data);
    void checkMonitoredItemCleanup();

    defineDataMethod(stringCharset_data)
    void stringCharset();

    defineDataMethod(namespaceArray_data)
    void namespaceArray();

    defineDataMethod(dateTimeConversion_data)
    void dateTimeConversion();
    defineDataMethod(timeStamps_data)
    void timeStamps();

    // This test case restarts the server. It must be run last to avoid
    // destroying state required by other test cases.
    defineDataMethod(connectionLost_data)
    void connectionLost();

private:
    QString envOrDefault(const char *env, QString def)
    {
        return qEnvironmentVariableIsSet(env) ? qgetenv(env).constData() : def;
    }

    QString m_endpoint;
    QOpcUaProvider m_opcUa;
    QStringList m_backends;
    QVector<QOpcUaClient *> m_clients;
    QProcess m_serverProcess;
    QString m_testServerPath;
};

#define READ_MANDATORY_BASE_NODE(NODE) \
    { \
    QSignalSpy attributeReadSpy(NODE.data(), &QOpcUaNode::attributeRead);\
    NODE->readAttributes(QOpcUaNode::mandatoryBaseAttributes()); \
    attributeReadSpy.wait(); \
    QCOMPARE(attributeReadSpy.count(), 1); \
    QVERIFY(attributeReadSpy.at(0).at(0).value<QOpcUa::NodeAttributes>() == QOpcUaNode::mandatoryBaseAttributes()); \
    QVERIFY(QOpcUa::isSuccessStatus(NODE->attributeError(QOpcUa::NodeAttribute::NodeId)) == true); \
    QVERIFY(QOpcUa::isSuccessStatus(NODE->attributeError(QOpcUa::NodeAttribute::NodeClass)) == true); \
    QVERIFY(QOpcUa::isSuccessStatus(NODE->attributeError(QOpcUa::NodeAttribute::BrowseName)) == true); \
    QVERIFY(QOpcUa::isSuccessStatus(NODE->attributeError(QOpcUa::NodeAttribute::DisplayName)) == true); \
    }

#define READ_MANDATORY_VARIABLE_NODE(NODE) \
    { \
    QSignalSpy attributeReadSpy(NODE.data(), &QOpcUaNode::attributeRead);\
    NODE->readAttributes(QOpcUaNode::mandatoryBaseAttributes() | QOpcUa::NodeAttribute::Value); \
    attributeReadSpy.wait(); \
    QCOMPARE(attributeReadSpy.count(), 1); \
    QVERIFY(attributeReadSpy.at(0).at(0).value<QOpcUa::NodeAttributes>() == (QOpcUaNode::mandatoryBaseAttributes() | QOpcUa::NodeAttribute::Value)); \
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
    resultSpy.wait(); \
    QCOMPARE(resultSpy.size(), 1); \
    QCOMPARE(resultSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value); \
    QCOMPARE(resultSpy.at(0).at(1).toUInt(), uint(0)); \
}

Tst_QOpcUaClient::Tst_QOpcUaClient()
{
    m_backends = QOpcUaProvider::availableBackends();
}

void Tst_QOpcUaClient::initTestCase()
{
    for (const auto &backend: m_backends) {
        if (backend == QStringLiteral("freeopcua") && qEnvironmentVariable("QTEST_ENVIRONMENT").split(' ').contains(QLatin1String("ci"))) {
            qDebug() << "The freeopcua tests randomly fail in the CI and are disabled by default";
            continue;
        }

        QOpcUaClient *client = m_opcUa.createClient(backend);
        QVERIFY2(client != nullptr,
                 QString("Loading backend failed: %1").arg(backend).toLatin1().data());
        client->setParent(this);
        qDebug() << "Using SDK plugin:" << client->backend();
        m_clients.append(client);
    }

    if (qEnvironmentVariableIsEmpty("OPCUA_HOST") && qEnvironmentVariableIsEmpty("OPCUA_PORT")) {
        m_testServerPath = qApp->applicationDirPath()
#ifdef Q_OS_WIN
                                     + QLatin1String("/..")
#endif
                                     + QLatin1String("/../../open62541-testserver/open62541-testserver")
#ifdef Q_OS_WIN
                                     + QLatin1String(".exe")
#endif
                ;
        if (!QFile::exists(m_testServerPath)) {
            qDebug() << "Server Path:" << m_testServerPath;
            QSKIP("all auto tests rely on an open62541-based test-server");
        }

        m_serverProcess.start(m_testServerPath);
        QVERIFY2(m_serverProcess.waitForStarted(), qPrintable(m_serverProcess.errorString()));
        // Let the server come up
        QTest::qSleep(2000);
    }
    QString host = envOrDefault("OPCUA_HOST", "localhost");
    QString port = envOrDefault("OPCUA_PORT", "43344");
    m_endpoint = QString("opc.tcp://%1:%2").arg(host).arg(port);
    qDebug() << "Using endpoint:" << m_endpoint;
}

void Tst_QOpcUaClient::connectToInvalid()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    opcuaClient->connectToEndpoint(QUrl("opc.tcp:127.0.0.1:1234"));
    // Depending on the event loop the client might have switched to Disconnected already
    QVERIFY(opcuaClient->state() == QOpcUaClient::Connecting || opcuaClient->state() == QOpcUaClient::Disconnected);

    for (int i = 0; i < 10; ++i) {
        QTest::qWait(50);
        if (opcuaClient->state() == QOpcUaClient::Disconnected)
            break;
        QVERIFY(opcuaClient->state() == QOpcUaClient::Connecting);
    }
    QVERIFY(opcuaClient->state() == QOpcUaClient::Connected ||
            opcuaClient->state() == QOpcUaClient::Disconnected);

    QUrl url = opcuaClient->url();
    QVERIFY(url == QUrl("opc.tcp:127.0.0.1:1234"));
}

void Tst_QOpcUaClient::connectAndDisconnect()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);
}

void Tst_QOpcUaClient::connectInvalidPassword()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    QUrl url(m_endpoint);
    url.setUserName("invaliduser");
    url.setPassword("wrongpassword");

    QSignalSpy connectSpy(opcuaClient, &QOpcUaClient::stateChanged);

    opcuaClient->connectToEndpoint(url);
    connectSpy.wait();

    QVERIFY(connectSpy.count() == 2);
    QVERIFY(connectSpy.at(0).at(0) == QOpcUaClient::Connecting);
    QVERIFY(connectSpy.at(1).at(0) == QOpcUaClient::Disconnected);

    QVERIFY(opcuaClient->url() == url);
    QVERIFY(opcuaClient->error() == QOpcUaClient::AccessDenied);
}

void Tst_QOpcUaClient::connectAndDisconnectPassword()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    QUrl url(m_endpoint);
    url.setUserName("user1");
    url.setPassword("password");

    QSignalSpy connectSpy(opcuaClient, &QOpcUaClient::stateChanged);

    opcuaClient->connectToEndpoint(url);
    connectSpy.wait();

    QVERIFY(connectSpy.count() == 2);
    QVERIFY(connectSpy.at(0).at(0) == QOpcUaClient::Connecting);
    QVERIFY(connectSpy.at(1).at(0) == QOpcUaClient::Connected);

    QVERIFY(opcuaClient->url() == url);
    QVERIFY(opcuaClient->error() == QOpcUaClient::NoError);

    connectSpy.clear();
    opcuaClient->disconnectFromEndpoint();
    connectSpy.wait();
    QVERIFY(connectSpy.count() == 2);
    QVERIFY(connectSpy.at(0).at(0) == QOpcUaClient::Closing);
    QVERIFY(connectSpy.at(1).at(0) == QOpcUaClient::Disconnected);
}

void Tst_QOpcUaClient::readInvalidNode()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=0;s=doesnotexist"));
    QVERIFY(node != 0);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUa::QLocalizedText>().text, QString());

    QSignalSpy attributeReadSpy(node.data(), &QOpcUaNode::attributeRead);

    node->readAttributes(QOpcUaNode::mandatoryBaseAttributes());
    attributeReadSpy.wait();

    QCOMPARE(attributeReadSpy.count(), 1);
    QCOMPARE(attributeReadSpy.at(0).at(0).value<QOpcUa::NodeAttributes>(), QOpcUaNode::mandatoryBaseAttributes());

    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::DisplayName), QVariant());
    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::DisplayName), QOpcUa::UaStatusCode::BadNodeIdUnknown);
    QVERIFY(QOpcUa::errorCategory(node->attributeError(QOpcUa::NodeAttribute::DisplayName)) == QOpcUa::ErrorCategory::NodeError);
    QVERIFY(QOpcUa::isSuccessStatus(node->attributeError(QOpcUa::NodeAttribute::DisplayName)) == false);
}

void Tst_QOpcUaClient::writeInvalidNode()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=0;s=doesnotexist"));
    QVERIFY(node != 0);

    QSignalSpy responseSpy(node.data(),&QOpcUaNode::attributeWritten);
    bool result = node->writeAttribute(QOpcUa::NodeAttribute::Value, 10, QOpcUa::Types::Int32);

    QCOMPARE(result, true);

    responseSpy.wait();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(responseSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadNodeIdUnknown);
}

void Tst_QOpcUaClient::writeMultipleAttributes()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.QualifiedName"));
    QVERIFY(node != 0);

    QOpcUaNode::AttributeMap map;
    map[QOpcUa::NodeAttribute::DisplayName] = QVariant::fromValue(QOpcUa::QLocalizedText(QStringLiteral("en"), QStringLiteral("NewDisplayName")));
    map[QOpcUa::NodeAttribute::Value] = QVariant::fromValue(QOpcUa::QQualifiedName(2, QStringLiteral("TestString")));

    QSignalSpy writeSpy(node.data(), &QOpcUaNode::attributeWritten);

    node->writeAttributes(map, QOpcUa::Types::QualifiedName);

    writeSpy.wait();
    if (writeSpy.size() < 2)
        writeSpy.wait();

    QVERIFY(writeSpy.size() == 2);
    QVERIFY(writeSpy.at(0).at(0).value<QOpcUa::NodeAttribute>() == QOpcUa::NodeAttribute::DisplayName);
    QVERIFY(writeSpy.at(0).at(1).value<QOpcUa::UaStatusCode>() == QOpcUa::UaStatusCode::BadUserAccessDenied);
    QVERIFY(node->attributeError(QOpcUa::NodeAttribute::DisplayName) == QOpcUa::UaStatusCode::BadUserAccessDenied);
    QVERIFY(QOpcUa::errorCategory(node->attributeError(QOpcUa::NodeAttribute::DisplayName)) == QOpcUa::ErrorCategory::PermissionError);

    QVERIFY(writeSpy.at(1).at(0).value<QOpcUa::NodeAttribute>() == QOpcUa::NodeAttribute::Value);
    QVERIFY(writeSpy.at(1).at(1).value<QOpcUa::UaStatusCode>() == QOpcUa::UaStatusCode::Good);
    QVERIFY(node->attributeError(QOpcUa::NodeAttribute::Value) == QOpcUa::UaStatusCode::Good);
    QVERIFY(node->attribute(QOpcUa::NodeAttribute::Value).value<QOpcUa::QQualifiedName>() == QOpcUa::QQualifiedName(2, QStringLiteral("TestString")));
}

void Tst_QOpcUaClient::readEmptyArrayVariable()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=2;s=EmptyBoolArray"));
    QVERIFY(node != 0);

    READ_MANDATORY_VARIABLE_NODE(node);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::Value).type(), QVariant::List);
    QVERIFY(node->attribute(QOpcUa::NodeAttribute::Value).toList().isEmpty());
}

void Tst_QOpcUaClient::getRootNode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> root(opcuaClient->node("ns=0;i=84"));
    QVERIFY(root != 0);

    READ_MANDATORY_BASE_NODE(root)
    QVERIFY(root->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUa::QLocalizedText>().text == QLatin1String("Root"));

    QString nodeId = root->nodeId();
    QCOMPARE(nodeId, QStringLiteral("ns=0;i=84"));
}

void Tst_QOpcUaClient::getChildren()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=1;s=Large.Folder"));
    QVERIFY(node != 0);
    QSignalSpy spy(node.data(), &QOpcUaNode::browseFinished);
    node->browseChildren(QOpcUa::ReferenceTypeId::HierarchicalReferences, QOpcUa::NodeClass::Object);
    spy.wait();
    QCOMPARE(spy.size(), 1);
    QVector<QOpcUaReferenceDescription> ref = spy.at(0).at(0).value<QVector<QOpcUaReferenceDescription>>();
    QCOMPARE(ref.size(), 100);
}

void Tst_QOpcUaClient::childrenIdsString()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=3;s=testStringIdsFolder"));
    QVERIFY(node != 0);
    QSignalSpy spy(node.data(), &QOpcUaNode::browseFinished);
    node->browseChildren(QOpcUa::ReferenceTypeId::Organizes, QOpcUa::NodeClass::Variable);
    spy.wait();
    QCOMPARE(spy.size(), 1);
    QVector<QOpcUaReferenceDescription> ref = spy.at(0).at(0).value<QVector<QOpcUaReferenceDescription>>();
    QCOMPARE(ref.size(), 1);
    QCOMPARE(ref.at(0).nodeId(), QStringLiteral("ns=3;s=theStringId"));
}

void Tst_QOpcUaClient::childrenIdsGuidNodeId()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=3;s=testGuidIdsFolder"));
    QVERIFY(node != 0);
    QSignalSpy spy(node.data(), &QOpcUaNode::browseFinished);
    node->browseChildren(QOpcUa::ReferenceTypeId::Organizes, QOpcUa::NodeClass::Variable);
    spy.wait();
    QCOMPARE(spy.size(), 1);
    QVector<QOpcUaReferenceDescription> ref = spy.at(0).at(0).value<QVector<QOpcUaReferenceDescription>>();
    QCOMPARE(ref.size(), 1);
    QCOMPARE(ref.at(0).nodeId(), QStringLiteral("ns=3;g=08081e75-8e5e-319b-954f-f3a7613dc29b"));
}

void Tst_QOpcUaClient::childrenIdsOpaqueNodeId()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=3;s=testOpaqueIdsFolder"));
    QVERIFY(node != 0);
    QSignalSpy spy(node.data(), &QOpcUaNode::browseFinished);
    node->browseChildren(QOpcUa::ReferenceTypeId::Organizes, QOpcUa::NodeClass::Variable);
    spy.wait();
    QCOMPARE(spy.size(), 1);
    QVector<QOpcUaReferenceDescription> ref = spy.at(0).at(0).value<QVector<QOpcUaReferenceDescription>>();
    QCOMPARE(ref.size(), 1);
    QCOMPARE(ref.at(0).nodeId(), QStringLiteral("ns=3;b=UXQgZnR3IQ=="));
}

void Tst_QOpcUaClient::dataChangeSubscription()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(readWriteNode));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, QVariant(double(0)), QOpcUa::Types::Double);
    READ_MANDATORY_VARIABLE_NODE(node);
    QTRY_COMPARE(node->attribute(QOpcUa::NodeAttribute::Value).toDouble(), 0.0);

    WRITE_VALUE_ATTRIBUTE(node, QVariant(double(0)), QOpcUa::Types::Double);

    QSignalSpy dataChangeSpy(node.data(), &QOpcUaNode::attributeUpdated);
    QSignalSpy monitoringEnabledSpy(node.data(), &QOpcUaNode::enableMonitoringFinished);

    node->enableMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters(100, QOpcUaMonitoringParameters::SubscriptionType::Exclusive));
    monitoringEnabledSpy.wait();

    QVERIFY(monitoringEnabledSpy.size() == 1);
    QVERIFY(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>() == QOpcUa::NodeAttribute::Value);
    QVERIFY(node->monitoringStatus(QOpcUa::NodeAttribute::Value).statusCode() == 0);

    QOpcUaMonitoringParameters valueStatus = node->monitoringStatus(QOpcUa::NodeAttribute::Value);
    QVERIFY(valueStatus.subscriptionId() != 0);
    QVERIFY(valueStatus.statusCode() == 0);

    WRITE_VALUE_ATTRIBUTE(node, QVariant(double(42)), QOpcUa::Types::Double);
    dataChangeSpy.wait();
    if (dataChangeSpy.size() < 2)
        dataChangeSpy.wait();

    QVERIFY(dataChangeSpy.size() >= 1);

    int index = dataChangeSpy.size() == 1 ? 0 : 1;
    QVERIFY(dataChangeSpy.at(index).at(0).value<QOpcUa::NodeAttribute>() == QOpcUa::NodeAttribute::Value);
    QVERIFY(dataChangeSpy.at(index).at(1) == double(42));

    monitoringEnabledSpy.clear();
    dataChangeSpy.clear();

    node->enableMonitoring(QOpcUa::NodeAttribute::DisplayName, QOpcUaMonitoringParameters(100,QOpcUaMonitoringParameters::SubscriptionType::Exclusive,
                                                                                              valueStatus.subscriptionId()));
    monitoringEnabledSpy.wait();

    QVERIFY(monitoringEnabledSpy.size() == 1);
    QVERIFY(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>() == QOpcUa::NodeAttribute::DisplayName);
    QVERIFY(node->monitoringStatus(QOpcUa::NodeAttribute::DisplayName).statusCode() == 0);

    QOpcUaMonitoringParameters displayNameStatus = node->monitoringStatus(QOpcUa::NodeAttribute::DisplayName);
    QVERIFY(displayNameStatus.subscriptionId() == valueStatus.subscriptionId());
    QVERIFY(displayNameStatus.statusCode() == 0);

    dataChangeSpy.wait();
    QVERIFY(dataChangeSpy.size() == 1);
    QVERIFY(dataChangeSpy.at(0).at(0).value<QOpcUa::NodeAttribute>() == QOpcUa::NodeAttribute::DisplayName);
    QVERIFY(dataChangeSpy.at(0).at(1).value<QOpcUa::QLocalizedText>().text == QLatin1String("TestNode.ReadWrite"));

    monitoringEnabledSpy.clear();
    dataChangeSpy.clear();
    node->enableMonitoring(QOpcUa::NodeAttribute::NodeId, QOpcUaMonitoringParameters(100));
    monitoringEnabledSpy.wait();
    QVERIFY(monitoringEnabledSpy.size() == 1);
    QVERIFY(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>() == QOpcUa::NodeAttribute::NodeId);
    QVERIFY(node->monitoringStatus(QOpcUa::NodeAttribute::NodeId).subscriptionId() != valueStatus.subscriptionId());
    QVERIFY(node->monitoringStatus(QOpcUa::NodeAttribute::NodeId).statusCode() == 0);

    QOpcUaMonitoringParameters nodeIdStatus = node->monitoringStatus(QOpcUa::NodeAttribute::NodeId);
    QVERIFY(nodeIdStatus.subscriptionId() != valueStatus.subscriptionId());
    QVERIFY(nodeIdStatus.statusCode() == 0);

    dataChangeSpy.wait();
    QVERIFY(dataChangeSpy.size() == 1);
    QVERIFY(dataChangeSpy.at(0).at(0).value<QOpcUa::NodeAttribute>() == QOpcUa::NodeAttribute::NodeId);
    QVERIFY(dataChangeSpy.at(0).at(1) == QLatin1String("ns=3;s=TestNode.ReadWrite"));

    QVector<QOpcUa::NodeAttribute> attrs;

    if (opcuaClient->backend() == QLatin1String("open62541") ||
            opcuaClient->backend() == QLatin1String("uacpp")) {
        QSignalSpy monitoringModifiedSpy(node.data(), &QOpcUaNode::monitoringStatusChanged);
        node->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::PublishingInterval, 200);

        monitoringModifiedSpy.wait();
        if (monitoringModifiedSpy.size() < 2)
            monitoringModifiedSpy.wait();

        attrs = {QOpcUa::NodeAttribute::Value, QOpcUa::NodeAttribute::DisplayName};
        for (auto it : qAsConst(monitoringModifiedSpy)) {
            QOpcUa::NodeAttribute temp = it.at(0).value<QOpcUa::NodeAttribute>();
            QVERIFY(attrs.contains(temp));
            QVERIFY(it.at(1).value<QOpcUaMonitoringParameters::Parameters>() &  QOpcUaMonitoringParameters::Parameter::PublishingInterval);
            QVERIFY(it.at(2) == QOpcUa::UaStatusCode::Good);
            QCOMPARE(node->monitoringStatus(temp).publishingInterval(), 200.0);
            attrs.remove(attrs.indexOf(temp));
        }
        QVERIFY(attrs.size() == 0);

        QCOMPARE(node->monitoringStatus(QOpcUa::NodeAttribute::Value).publishingInterval(),  200.0);
        QCOMPARE(node->monitoringStatus(QOpcUa::NodeAttribute::DisplayName).publishingInterval(), 200.0);
    } else {
        qDebug() << "Modifying monitoring settings is not supported by the freeopcua backend";
    }

    QSignalSpy monitoringDisabledSpy(node.data(), &QOpcUaNode::disableMonitoringFinished);

    node->disableMonitoring(QOpcUa::NodeAttribute::Value | QOpcUa::NodeAttribute::DisplayName | QOpcUa::NodeAttribute::NodeId);
    monitoringDisabledSpy.wait();
    if (monitoringDisabledSpy.size() < 2)
        monitoringDisabledSpy.wait();
    if (monitoringDisabledSpy.size() < 3)
        monitoringDisabledSpy.wait();

    QVERIFY(monitoringDisabledSpy.size() == 3);

    attrs = {QOpcUa::NodeAttribute::Value, QOpcUa::NodeAttribute::DisplayName, QOpcUa::NodeAttribute::NodeId};
    for (auto it : qAsConst(monitoringDisabledSpy)) {
        QOpcUa::NodeAttribute temp = it.at(0).value<QOpcUa::NodeAttribute>();
        QVERIFY(attrs.contains(temp));
        QVERIFY(node->monitoringStatus(temp).subscriptionId() == 0);
        QVERIFY(node->monitoringStatus(temp).statusCode() == QOpcUa::UaStatusCode::BadAttributeIdInvalid);
        attrs.remove(attrs.indexOf(temp));
    }
    QVERIFY(attrs.size() == 0);
}

void Tst_QOpcUaClient::dataChangeSubscriptionInvalidNode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> noDataNode(opcuaClient->node("ns=0;i=84"));
    QSignalSpy monitoringEnabledSpy(noDataNode.data(), &QOpcUaNode::enableMonitoringFinished);

    QOpcUaMonitoringParameters settings;
    settings.setPublishingInterval(100);
    noDataNode->enableMonitoring(QOpcUa::NodeAttribute::Value, settings);
    monitoringEnabledSpy.wait();

    QVERIFY(monitoringEnabledSpy.size() == 1);
    QVERIFY(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>() == QOpcUa::NodeAttribute::Value);
    QVERIFY(noDataNode->monitoringStatus(QOpcUa::NodeAttribute::Value).statusCode() == QOpcUa::UaStatusCode::BadAttributeIdInvalid);
    QVERIFY(noDataNode->monitoringStatus(QOpcUa::NodeAttribute::Value).subscriptionId() == 0);
}

void Tst_QOpcUaClient::dataChangeSubscriptionSharing()
{
    // The open62541 test server has a minimum publishing interval of 100ms.
    // This test verifies that monitorings with smaller requested publishing interval and shared flag
    // share the same subscription.

    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(readWriteNode));
    QVERIFY(node != 0);
    QSignalSpy monitoringEnabledSpy(node.data(), &QOpcUaNode::enableMonitoringFinished);

    node->enableMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters(50));
    monitoringEnabledSpy.wait();

    QVERIFY(monitoringEnabledSpy.size() == 1);
    QVERIFY(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>() == QOpcUa::NodeAttribute::Value);
    QVERIFY(node->monitoringStatus(QOpcUa::NodeAttribute::Value).statusCode() == QOpcUa::UaStatusCode::Good);

    QOpcUaMonitoringParameters valueStatus = node->monitoringStatus(QOpcUa::NodeAttribute::Value);
    QVERIFY(valueStatus.subscriptionId() != 0);
    QVERIFY(valueStatus.statusCode() == QOpcUa::UaStatusCode::Good);

    monitoringEnabledSpy.clear();

    node->enableMonitoring(QOpcUa::NodeAttribute::DisplayName, QOpcUaMonitoringParameters(25));
    monitoringEnabledSpy.wait();

    QVERIFY(monitoringEnabledSpy.size() == 1);
    QVERIFY(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>() == QOpcUa::NodeAttribute::DisplayName);
    QVERIFY(node->monitoringStatus(QOpcUa::NodeAttribute::DisplayName).statusCode() == QOpcUa::UaStatusCode::Good);

    QOpcUaMonitoringParameters displayNameStatus = node->monitoringStatus(QOpcUa::NodeAttribute::DisplayName);
    QVERIFY(displayNameStatus.subscriptionId() == valueStatus.subscriptionId());
    QVERIFY(displayNameStatus.statusCode() == QOpcUa::UaStatusCode::Good);

    QVERIFY(valueStatus.subscriptionId() == displayNameStatus.subscriptionId());
    QCOMPARE(valueStatus.publishingInterval(), displayNameStatus.publishingInterval());
    QCOMPARE(valueStatus.publishingInterval(), 100.0);

    QSignalSpy monitoringDisabledSpy(node.data(), &QOpcUaNode::disableMonitoringFinished);

    node->disableMonitoring(QOpcUa::NodeAttribute::Value | QOpcUa::NodeAttribute::DisplayName | QOpcUa::NodeAttribute::NodeId);
    monitoringDisabledSpy.wait();
    if (monitoringDisabledSpy.size() < 2)
        monitoringDisabledSpy.wait();

    QVERIFY(monitoringDisabledSpy.size() == 2);

    QVector<QOpcUa::NodeAttribute> attrs = {QOpcUa::NodeAttribute::Value, QOpcUa::NodeAttribute::DisplayName};
    for (auto it : qAsConst(monitoringDisabledSpy)) {
        auto temp = it.at(0).value<QOpcUa::NodeAttribute>();
        QVERIFY(attrs.contains(temp));
        QVERIFY(node->monitoringStatus(temp).subscriptionId() == 0);
        QVERIFY(node->monitoringStatus(temp).statusCode() == QOpcUa::UaStatusCode::BadAttributeIdInvalid);
        attrs.removeOne(temp);
    }
    QVERIFY(attrs.size() == 0);
}

void Tst_QOpcUaClient::methodCall()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QVector<QOpcUa::TypedVariant> args;
    QVector<QVariant> ret;
    for (int i = 0; i < 2; i++)
        args.push_back(QOpcUa::TypedVariant(double(4), QOpcUa::Double));

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=3;s=TestFolder"));
    QVERIFY(node != 0);

    QSignalSpy methodSpy(node.data(), &QOpcUaNode::methodCallFinished);

    bool success = node->callMethod("ns=3;s=Test.Method.Multiply", args);
    QVERIFY(success == true);

    methodSpy.wait();

    QCOMPARE(methodSpy.size(), 1);
    QCOMPARE(methodSpy.at(0).at(0).value<QString>(), QStringLiteral("ns=3;s=Test.Method.Multiply"));
    QCOMPARE(methodSpy.at(0).at(1).value<double>(), 16.0);
    QCOMPARE(QOpcUa::isSuccessStatus(methodSpy.at(0).at(2).value<QOpcUa::UaStatusCode>()), true);
}

void Tst_QOpcUaClient::methodCallInvalid()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QVector<QOpcUa::TypedVariant> args;
    QVector<QVariant> ret;
    for (int i = 0; i < 3; i++)
        args.push_back(QOpcUa::TypedVariant(double(4), QOpcUa::Double));

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=3;s=TestFolder"));
    QVERIFY(node != 0);

    QSignalSpy methodSpy(node.data(), &QOpcUaNode::methodCallFinished);

    bool success = node->callMethod("ns=3;s=Test.Method.Divide", args); // Does not exist
    QVERIFY(success == true);
    methodSpy.wait();
    QCOMPARE(methodSpy.size(), 1);
    QCOMPARE(QOpcUa::errorCategory(methodSpy.at(0).at(2).value<QOpcUa::UaStatusCode>()), QOpcUa::ErrorCategory::NodeError);

    methodSpy.clear();
    success = node->callMethod("ns=3;s=Test.Method.Multiply", args); // One excess argument
    QVERIFY(success == true);
    methodSpy.wait();
    QCOMPARE(methodSpy.size(), 1);
    QCOMPARE(methodSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadTooManyArguments);

    methodSpy.clear();
    args.resize(1);
    success = node->callMethod("ns=3;s=Test.Method.Multiply", args); // One argument missing
    QVERIFY(success == true);
    methodSpy.wait();
    QCOMPARE(methodSpy.size(), 1);
    QCOMPARE(methodSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadArgumentsMissing);
}

void Tst_QOpcUaClient::malformedNodeString()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> invalidNode(opcuaClient->node("justsomerandomstring"));
    QVERIFY(invalidNode == 0);

    invalidNode.reset(opcuaClient->node("ns=a;i=b"));
    QVERIFY(invalidNode == 0);

    invalidNode.reset(opcuaClient->node("ns=;i="));
    QVERIFY(invalidNode == 0);

    invalidNode.reset(opcuaClient->node("ns=0;x=123"));
    QVERIFY(invalidNode == 0);

    invalidNode.reset(opcuaClient->node("ns=0,i=31;"));
    QVERIFY(invalidNode == 0);

    invalidNode.reset(opcuaClient->node("ns:0;i:31;"));
    QVERIFY(invalidNode == 0);
}

void Tst_QOpcUaClient::nodeIdGeneration()
{
    QString nodeId = QOpcUa::nodeIdFromString(1, QStringLiteral("TestString"));
    QCOMPARE(nodeId, QStringLiteral("ns=1;s=TestString"));
    nodeId = QOpcUa::nodeIdFromInteger(1, 10);
    QCOMPARE(nodeId, QStringLiteral("ns=1;i=10"));
    nodeId = QOpcUa::nodeIdFromGuid(1, QUuid("08081e75-8e5e-319b-954f-f3a7613dc29b"));
    QCOMPARE(nodeId, QStringLiteral("ns=1;g=08081e75-8e5e-319b-954f-f3a7613dc29b"));
    nodeId = QOpcUa::nodeIdFromByteString(1, QByteArray::fromBase64("UXQgZnR3IQ=="));
    QCOMPARE(nodeId, QStringLiteral("ns=1;b=UXQgZnR3IQ=="));
}

void Tst_QOpcUaClient::multipleClients()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    opcuaClient->connectToEndpoint(m_endpoint);
    QTRY_VERIFY2(opcuaClient->state() == QOpcUaClient::Connected, "Could not connect to server");
    QScopedPointer<QOpcUaNode> node(opcuaClient->node(readWriteNode));
    WRITE_VALUE_ATTRIBUTE(node, 42.0, QOpcUa::Types::Double);
    READ_MANDATORY_VARIABLE_NODE(node);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::Value).toDouble(), 42.0);
    QScopedPointer<QOpcUaClient> b(m_opcUa.createClient(opcuaClient->backend()));
    b->connectToEndpoint(m_endpoint);
    QTRY_VERIFY2(b->state() == QOpcUaClient::Connected, "Could not connect to server");
    node.reset(b->node(readWriteNode));
    READ_MANDATORY_VARIABLE_NODE(node);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::Value).toDouble(), 42.0);
    QScopedPointer<QOpcUaClient> d(m_opcUa.createClient(opcuaClient->backend()));
    d->connectToEndpoint(m_endpoint);
    QTRY_VERIFY2(d->state() == QOpcUaClient::Connected, "Could not connect to server");
    node.reset(d->node(readWriteNode));
    READ_MANDATORY_VARIABLE_NODE(node);
    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::Value).toDouble(), 42.0);
    d->disconnectFromEndpoint();
    QTRY_VERIFY2(d->state() == QOpcUaClient::Disconnected, "Could not disconnect from server");
    opcuaClient->disconnectFromEndpoint();
    QTRY_VERIFY2(opcuaClient->state() == QOpcUaClient::Disconnected, "Could not disconnect from server");
    b->disconnectFromEndpoint();
    QTRY_VERIFY2(b->state() == QOpcUaClient::Disconnected, "Could not disconnect from server");
}

void Tst_QOpcUaClient::nodeClass()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    // Root -> Types -> ReferenceTypes -> References
    {
        QScopedPointer<QOpcUaNode> refNode(opcuaClient->node("ns=0;i=31"));
        QVERIFY(refNode != 0);
        READ_MANDATORY_BASE_NODE(refNode)
        QCOMPARE(refNode->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>(), QOpcUa::NodeClass::ReferenceType);
    }

    // Root -> Types -> DataTypes -> BaseDataTypes -> Boolean
    {
        QScopedPointer<QOpcUaNode> dataTypeNode(opcuaClient->node("ns=0;i=1"));
        QVERIFY(dataTypeNode != 0);
        READ_MANDATORY_BASE_NODE(dataTypeNode)
        QCOMPARE(dataTypeNode->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>(), QOpcUa::NodeClass::DataType);
    }

    // Root -> Types -> DataTypes -> ObjectTypes -> BaseObjectTypes -> FolderType
    {
        QScopedPointer<QOpcUaNode> objectTypeNode(opcuaClient->node("ns=0;i=61"));
        QVERIFY(objectTypeNode != 0);
        READ_MANDATORY_BASE_NODE(objectTypeNode)
        QCOMPARE(objectTypeNode->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>(), QOpcUa::NodeClass::ObjectType);
    }

    // Root -> Types -> DataTypes -> VariableTypes -> BaseVariableType -> PropertyType
    {
        QScopedPointer<QOpcUaNode> variableTypeNode(opcuaClient->node("ns=0;i=68"));
        QVERIFY(variableTypeNode != 0);
        READ_MANDATORY_BASE_NODE(variableTypeNode)
        QCOMPARE(variableTypeNode->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>(), QOpcUa::NodeClass::VariableType);
    }

    // Root -> Objects
    {
        QScopedPointer<QOpcUaNode> objectNode(opcuaClient->node("ns=0;i=85"));
        QVERIFY(objectNode != 0);
        READ_MANDATORY_BASE_NODE(objectNode)
        QCOMPARE(objectNode->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>(), QOpcUa::NodeClass::Object);
    }

    // Root -> Objects -> Server -> NamespaceArray
    {
        QScopedPointer<QOpcUaNode> variableNode(opcuaClient->node("ns=0;i=2255"));
        QVERIFY(variableNode != 0);
        READ_MANDATORY_BASE_NODE(variableNode)
        QCOMPARE(variableNode->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>(), QOpcUa::NodeClass::Variable);
    }
}

void Tst_QOpcUaClient::writeArray()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QVariantList list;

    list.append(true);
    list.append(false);
    list.append(true);
    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Boolean"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Boolean);

    list.clear();
    list.append(std::numeric_limits<quint8>::min());
    list.append(std::numeric_limits<quint8>::max());
    list.append(10);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Byte"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Byte);

    list.clear();
    list.append(QDateTime::currentDateTime());
    list.append(QDateTime::currentDateTime());
    list.append(QDateTime::currentDateTime());
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.DateTime"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::DateTime);

    list.clear();
    list.append(23.5);
    list.append(23.6);
    list.append(23.7);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Double"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Double);

    list.clear();
    list.append(23.5);
    list.append(23.6);
    list.append(23.7);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Float"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Float);

    list.clear();
    list.append(std::numeric_limits<qint16>::min());
    list.append(std::numeric_limits<qint16>::max());
    list.append(10);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int16"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Int16);

    list.clear();
    list.append(std::numeric_limits<qint32>::min());
    list.append(std::numeric_limits<qint32>::max());
    list.append(10);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int32"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Int32);

    list.clear();
    list.append(std::numeric_limits<qint64>::min());
    list.append(std::numeric_limits<qint64>::max());
    list.append(10);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int64"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Int64);

    list.clear();
    list.append(std::numeric_limits<qint8>::min());
    list.append(std::numeric_limits<qint8>::max());
    list.append(10);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.SByte"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::SByte);

    list.clear();
    list.append("Test1");
    list.append("Test2");
    list.append("Test3");
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.String"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::String);

    list.clear();
    list.append(std::numeric_limits<quint16>::min());
    list.append(std::numeric_limits<quint16>::max());
    list.append(10);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.UInt16"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::UInt16);

    list.clear();
    list.append(std::numeric_limits<quint32>::min());
    list.append(std::numeric_limits<quint32>::max());
    list.append(10);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.UInt32"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::UInt32);

    list.clear();
    list.append(std::numeric_limits<quint64>::min());
    list.append(std::numeric_limits<quint64>::max());
    list.append(10);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.UInt64"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::UInt64);

    list.clear();
    list.append(QVariant::fromValue(localizedTexts[0]));
    list.append(QVariant::fromValue(localizedTexts[1]));
    list.append(QVariant::fromValue(localizedTexts[2]));
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.LocalizedText"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::LocalizedText);

    list.clear();
    list.append("abc");
    list.append("def");
    QByteArray withNull("gh");
    withNull.append('\0');
    withNull.append("i");
    list.append(withNull);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.ByteString"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::ByteString);

    list.clear();
    list.append(QUuid("e0bd5ccd-f571-4545-9352-61a0f8cb9216"));
    list.append(QUuid("460ebe04-89d8-42f3-a0e0-7b45940f1a4e4"));
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Guid"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Guid);

    list.clear();
    list.append("ns=0;i=0");
    list.append("ns=0;i=1");
    list.append("ns=0;i=2");
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.NodeId"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::NodeId);

    list.clear();
    list.append(QVariant::fromValue(QOpcUa::QQualifiedName(0, "Test0")));
    list.append(QVariant::fromValue(QOpcUa::QQualifiedName(1, "Test1")));
    list.append(QVariant::fromValue(QOpcUa::QQualifiedName(2, "Test2")));
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.QualifiedName"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::QualifiedName);

    list.clear();
    list.append(QOpcUa::UaStatusCode::Good);
    list.append(QOpcUa::UaStatusCode::BadUnexpectedError);
    list.append(QOpcUa::UaStatusCode::BadInternalError);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.StatusCode"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Types::StatusCode);

    if (opcuaClient->backend() != QLatin1String("freeopcua")) {
        list.clear();
        list.append(QVariant::fromValue(testRanges[0]));
        list.append(QVariant::fromValue(testRanges[1]));
        list.append(QVariant::fromValue(testRanges[2]));
        node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Range"));
        QVERIFY(node != 0);
        WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Range);

        list.clear();
        list.append(QVariant::fromValue(testEUInfos[0]));
        list.append(QVariant::fromValue(testEUInfos[1]));
        list.append(QVariant::fromValue(testEUInfos[2]));
        node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.EUInformation"));
        QVERIFY(node != 0);
        WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::EUInformation);

        list.clear();
        list.append(QVariant::fromValue(testComplex[0]));
        list.append(QVariant::fromValue(testComplex[1]));
        list.append(QVariant::fromValue(testComplex[2]));
        node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.ComplexNumber"));
        QVERIFY(node != 0);
        WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::ComplexNumber);

        list.clear();
        list.append(QVariant::fromValue(testDoubleComplex[0]));
        list.append(QVariant::fromValue(testDoubleComplex[1]));
        list.append(QVariant::fromValue(testDoubleComplex[2]));
        node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.DoubleComplexNumber"));
        QVERIFY(node != 0);
        WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::DoubleComplexNumber);

        list.clear();
        list.append(QVariant::fromValue(testAxisInfo[0]));
        list.append(QVariant::fromValue(testAxisInfo[1]));
        list.append(QVariant::fromValue(testAxisInfo[2]));
        node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.AxisInformation"));
        QVERIFY(node != 0);
        WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::AxisInformation);

        list.clear();
        list.append(QVariant::fromValue(testXV[0]));
        list.append(QVariant::fromValue(testXV[1]));
        list.append(QVariant::fromValue(testXV[2]));
        node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.XV"));
        QVERIFY(node != 0);
        WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::XV);
    } else {
        QWARN("ExtensionObject types are not supported by freeopcua");
    }

    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("XmlElement support is not yet implemented in the freeopcua library");

    list.clear();
    list.append(xmlElements[0]);
    list.append(xmlElements[1]);
    list.append(xmlElements[2]);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.XmlElement"));
    QVERIFY(node != 0);
    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::XmlElement);
}

void Tst_QOpcUaClient::readArray()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> booleanArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Boolean"));
    QVERIFY(booleanArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(booleanArrayNode);
    QVariant booleanArray = booleanArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(booleanArray.type() == QVariant::List);
    QVERIFY(booleanArray.toList().length() == 3);
    QCOMPARE(booleanArray.toList()[0].type(), QVariant::Bool);
    QCOMPARE(booleanArray.toList()[0].toBool(), true);
    QCOMPARE(booleanArray.toList()[1].toBool(), false);
    QCOMPARE(booleanArray.toList()[2].toBool(), true);

    QScopedPointer<QOpcUaNode> int32ArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int32"));
    QVERIFY(int32ArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(int32ArrayNode);
    QVariant int32Array = int32ArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(int32Array.type() == QVariant::List);
    QVERIFY(int32Array.toList().length() == 3);
    QCOMPARE(int32Array.toList()[0].type(), QVariant::Int);
    QCOMPARE(int32Array.toList()[0].toInt(), std::numeric_limits<qint32>::min());
    QCOMPARE(int32Array.toList()[1].toInt(), std::numeric_limits<qint32>::max());
    QCOMPARE(int32Array.toList()[2].toInt(), 10);

    QScopedPointer<QOpcUaNode> uint32ArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.UInt32"));
    QVERIFY(uint32ArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(uint32ArrayNode);
    QVariant uint32Array = uint32ArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(uint32Array.type() == QVariant::List);
    QVERIFY(uint32Array.toList().length() == 3);
    QCOMPARE(uint32Array.toList()[0].type(), QVariant::UInt);
    QCOMPARE(uint32Array.toList()[0].toUInt(), std::numeric_limits<quint32>::min());
    QCOMPARE(uint32Array.toList()[1].toUInt(), std::numeric_limits<quint32>::max());
    QCOMPARE(uint32Array.toList()[2].toUInt(), quint32(10));

    QScopedPointer<QOpcUaNode> doubleArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Double"));
    QVERIFY(doubleArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(doubleArrayNode);
    QVariant doubleArray = doubleArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(doubleArray.type() == QVariant::List);
    QVERIFY(doubleArray.toList().length() == 3);
    QCOMPARE(doubleArray.toList()[0].type(), QVariant::Double);
    QCOMPARE(doubleArray.toList()[0].toDouble(), double(23.5));
    QCOMPARE(doubleArray.toList()[1].toDouble(), double(23.6));
    QCOMPARE(doubleArray.toList()[2].toDouble(), double(23.7));

    QScopedPointer<QOpcUaNode> floatArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Float"));
    QVERIFY(floatArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(floatArrayNode);
    QVariant floatArray = floatArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(floatArray.type() == QVariant::List);
    QVERIFY(floatArray.toList().length() == 3);
    QVERIFY(floatArray.toList()[0].userType() == QMetaType::Float);
    QCOMPARE(floatArray.toList()[0].toFloat(), float(23.5));
    QCOMPARE(floatArray.toList()[1].toFloat(), float(23.6));
    QCOMPARE(floatArray.toList()[2].toFloat(), float(23.7));

    QScopedPointer<QOpcUaNode> stringArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.String"));
    QVERIFY(stringArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(stringArrayNode);
    QVariant stringArray = stringArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(stringArray.type() == QVariant::List);
    QVERIFY(stringArray.toList().length() == 3);
    QCOMPARE(stringArray.toList()[0].type(), QVariant::String);
    QCOMPARE(stringArray.toList()[0].toString(), QStringLiteral("Test1"));
    QCOMPARE(stringArray.toList()[1].toString(), QStringLiteral("Test2"));
    QCOMPARE(stringArray.toList()[2].toString(), QStringLiteral("Test3"));

    QScopedPointer<QOpcUaNode> dateTimeArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.DateTime"));
    QVERIFY(dateTimeArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(dateTimeArrayNode);
    QVariant dateTimeArray = dateTimeArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(dateTimeArray.type() == QVariant::List);
    QVERIFY(dateTimeArray.toList().length() == 3);
    QCOMPARE(dateTimeArray.toList()[0].type(), QVariant::DateTime);

    QScopedPointer<QOpcUaNode> ltArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.LocalizedText"));
    QVERIFY(ltArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(ltArrayNode);
    QVariant ltArray = ltArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(ltArray.type() == QVariant::List);
    QVERIFY(ltArray.toList().length() == 3);
    QVERIFY(ltArray.toList()[0].value<QOpcUa::QLocalizedText>() == localizedTexts[0]);
    QVERIFY(ltArray.toList()[1].value<QOpcUa::QLocalizedText>() == localizedTexts[1]);
    QVERIFY(ltArray.toList()[2].value<QOpcUa::QLocalizedText>() == localizedTexts[2]);

    QScopedPointer<QOpcUaNode> uint16ArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.UInt16"));
    QVERIFY(uint16ArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(uint16ArrayNode);
    QVariant uint16Array = uint16ArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(uint16Array.type() == QVariant::List);
    QVERIFY(uint16Array.toList().length() == 3);
    QVERIFY(uint16Array.toList()[0].userType() == QMetaType::UShort);
    QVERIFY(uint16Array.toList()[0] == std::numeric_limits<quint16>::min());
    QVERIFY(uint16Array.toList()[1] == std::numeric_limits<quint16>::max());
    QVERIFY(uint16Array.toList()[2] == quint16(10));

    QScopedPointer<QOpcUaNode> int16ArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int16"));
    QVERIFY(int16ArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(int16ArrayNode);
    QVariant int16Array = int16ArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(int16Array.type() == QVariant::List);
    QVERIFY(int16Array.toList().length() == 3);
    QVERIFY(int16Array.toList()[0].userType() == QMetaType::Short);
    QVERIFY(int16Array.toList()[0] == std::numeric_limits<qint16>::min());
    QVERIFY(int16Array.toList()[1] == std::numeric_limits<qint16>::max());
    QVERIFY(int16Array.toList()[2] == qint16(10));

    QScopedPointer<QOpcUaNode> uint64ArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.UInt64"));
    QVERIFY(uint64ArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(uint64ArrayNode);
    QVariant uint64Array = uint64ArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(uint64Array.type(), QVariant::List);
    QCOMPARE(uint64Array.toList().length(), 3);
    QCOMPARE(uint64Array.toList()[0].type(), QVariant::ULongLong);
    QVERIFY(uint64Array.toList()[0] == std::numeric_limits<quint64>::min());
    QVERIFY(uint64Array.toList()[1] == std::numeric_limits<quint64>::max());
    QVERIFY(uint64Array.toList()[2] == quint64(10));

    QScopedPointer<QOpcUaNode> int64ArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int64"));
    QVERIFY(int64ArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(int64ArrayNode);
    QVariant int64Array = int64ArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(int64Array.type() == QVariant::List && int64Array.toList().length() == 3);
    QCOMPARE(int64Array.toList()[0].type(), QVariant::LongLong);
    QVERIFY(int64Array.toList()[0] == std::numeric_limits<qint64>::min());
    QVERIFY(int64Array.toList()[1] == std::numeric_limits<qint64>::max());
    QVERIFY(int64Array.toList()[2] == qint64(10));

    QScopedPointer<QOpcUaNode> byteArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Byte"));
    QVERIFY(byteArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(byteArrayNode);
    QVariant byteArray = byteArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(byteArray.type() == QVariant::List);
    QVERIFY(byteArray.toList().length() == 3);
    QVERIFY(byteArray.toList()[0].userType() == QMetaType::UChar);
    QVERIFY(byteArray.toList()[0] == std::numeric_limits<quint8>::min());
    QVERIFY(byteArray.toList()[1] == std::numeric_limits<quint8>::max());
    QVERIFY(byteArray.toList()[2] == quint8(10));

    QScopedPointer<QOpcUaNode> byteStringArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.ByteString"));
    QVERIFY(byteStringArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(byteStringArrayNode);
    QVariant byteStringArray = byteStringArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(byteStringArray.type() == QVariant::List);
    QVERIFY(byteStringArray.toList().length() == 3);
    QVERIFY(byteStringArray.toList()[0].userType() == QMetaType::QByteArray);
    QVERIFY(byteStringArray.toList()[0] == "abc");
    QVERIFY(byteStringArray.toList()[1] == "def");
    QByteArray withNull("gh");
    withNull.append('\0');
    withNull.append("i");
    QVERIFY(byteStringArray.toList()[2] == withNull);

    QScopedPointer<QOpcUaNode> guidArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Guid"));
    QVERIFY(guidArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(guidArrayNode);
    QVariant guidArray = guidArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(guidArray.type() == QVariant::List);
    QVERIFY(guidArray.toList().length() == 2);
    QCOMPARE(guidArray.toList()[0].toUuid(), QUuid("e0bd5ccd-f571-4545-9352-61a0f8cb9216}"));
    QCOMPARE(guidArray.toList()[1].toUuid(), QUuid("460ebe04-89d8-42f3-a0e0-7b45940f1a4e4"));

    QScopedPointer<QOpcUaNode> sbyteArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.SByte"));
    QVERIFY(sbyteArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(sbyteArrayNode);
    QVariant sbyteArray = sbyteArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(sbyteArray.type() == QVariant::List);
    QVERIFY(sbyteArray.toList().length() == 3);
    QVERIFY(sbyteArray.toList()[0].userType() == QMetaType::SChar);
    QVERIFY(sbyteArray.toList()[0] == std::numeric_limits<qint8>::min());
    QVERIFY(sbyteArray.toList()[1] == std::numeric_limits<qint8>::max());
    QVERIFY(sbyteArray.toList()[2] == qint8(10));

    QScopedPointer<QOpcUaNode> nodeIdArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.NodeId"));
    QVERIFY(nodeIdArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(nodeIdArrayNode);
    QVariant nodeIdArray = nodeIdArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(nodeIdArray.type() == QVariant::List);
    QVERIFY(nodeIdArray.toList().length() == 3);
    QCOMPARE(nodeIdArray.toList()[0].type(), QVariant::String);
    QCOMPARE(nodeIdArray.toList()[0].toString(), QStringLiteral("ns=0;i=0"));
    QCOMPARE(nodeIdArray.toList()[1].toString(), QStringLiteral("ns=0;i=1"));
    QCOMPARE(nodeIdArray.toList()[2].toString(), QStringLiteral("ns=0;i=2"));

    QScopedPointer<QOpcUaNode> qualifiedNameArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.QualifiedName"));
    QVERIFY(nodeIdArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(qualifiedNameArrayNode)
    QVariant qualifiedNameArray = qualifiedNameArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(qualifiedNameArray.type() == QVariant::List);
    QVERIFY(qualifiedNameArray.toList().length() == 3);
    QVERIFY(qualifiedNameArray.toList()[0].value<QOpcUa::QQualifiedName>()  == QOpcUa::QQualifiedName(0, "Test0"));
    QVERIFY(qualifiedNameArray.toList()[1].value<QOpcUa::QQualifiedName>()  == QOpcUa::QQualifiedName(1, "Test1"));
    QVERIFY(qualifiedNameArray.toList()[2].value<QOpcUa::QQualifiedName>()  == QOpcUa::QQualifiedName(2, "Test2"));

    QScopedPointer<QOpcUaNode> statusCodeArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.StatusCode"));
    QVERIFY(statusCodeArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(statusCodeArrayNode);
    QVariant statusCodeArray = statusCodeArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(statusCodeArray.type() == QVariant::List);
    QVERIFY(statusCodeArray.toList().length() == 3);
    QCOMPARE(statusCodeArray.toList()[0].value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(statusCodeArray.toList()[1].value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadUnexpectedError);
    QCOMPARE(statusCodeArray.toList()[2].value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadInternalError);

    if (opcuaClient->backend() != QLatin1String("freeopcua")) {
        QScopedPointer<QOpcUaNode> rangeArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Range"));
        QVERIFY(rangeArrayNode != 0);
        READ_MANDATORY_VARIABLE_NODE(rangeArrayNode);
        QVariant rangeArray = rangeArrayNode->attribute(QOpcUa::NodeAttribute::Value);
        QVERIFY(rangeArray.type() == QVariant::List);
        QVERIFY(rangeArray.toList().length() == 3);
        QCOMPARE(rangeArray.toList()[0].value<QOpcUa::QRange>(), testRanges[0]);
        QCOMPARE(rangeArray.toList()[1].value<QOpcUa::QRange>(), testRanges[1]);
        QCOMPARE(rangeArray.toList()[2].value<QOpcUa::QRange>(), testRanges[2]);

        QScopedPointer<QOpcUaNode> euiArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.EUInformation"));
        QVERIFY(euiArrayNode != 0);
        READ_MANDATORY_VARIABLE_NODE(euiArrayNode);
        QVariant euiArray = euiArrayNode->attribute(QOpcUa::NodeAttribute::Value);
        QVERIFY(euiArray.type() == QVariant::List);
        QVERIFY(euiArray.toList().length() == 3);
        QCOMPARE(euiArray.toList()[0].value<QOpcUa::QEUInformation>(), testEUInfos[0]);
        QCOMPARE(euiArray.toList()[1].value<QOpcUa::QEUInformation>(), testEUInfos[1]);
        QCOMPARE(euiArray.toList()[2].value<QOpcUa::QEUInformation>(), testEUInfos[2]);

        QScopedPointer<QOpcUaNode> complexArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.ComplexNumber"));
        QVERIFY(complexArrayNode != 0);
        READ_MANDATORY_VARIABLE_NODE(complexArrayNode);
        QVariant complexArray = complexArrayNode->attribute(QOpcUa::NodeAttribute::Value);
        QVERIFY(complexArray.type() == QVariant::List);
        QVERIFY(complexArray.toList().length() == 3);
        QCOMPARE(complexArray.toList()[0].value<QOpcUa::QComplexNumber>(), testComplex[0]);
        QCOMPARE(complexArray.toList()[1].value<QOpcUa::QComplexNumber>(), testComplex[1]);
        QCOMPARE(complexArray.toList()[2].value<QOpcUa::QComplexNumber>(), testComplex[2]);

        QScopedPointer<QOpcUaNode> doubleComplexArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.DoubleComplexNumber"));
        QVERIFY(doubleComplexArrayNode != 0);
        READ_MANDATORY_VARIABLE_NODE(doubleComplexArrayNode);
        QVariant doubleComplexArray = doubleComplexArrayNode->attribute(QOpcUa::NodeAttribute::Value);
        QVERIFY(doubleComplexArray.type() == QVariant::List);
        QVERIFY(doubleComplexArray.toList().length() == 3);
        QCOMPARE(doubleComplexArray.toList()[0].value<QOpcUa::QDoubleComplexNumber>(), testDoubleComplex[0]);
        QCOMPARE(doubleComplexArray.toList()[1].value<QOpcUa::QDoubleComplexNumber>(), testDoubleComplex[1]);
        QCOMPARE(doubleComplexArray.toList()[2].value<QOpcUa::QDoubleComplexNumber>(), testDoubleComplex[2]);

        QScopedPointer<QOpcUaNode> axisInfoArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.AxisInformation"));
        QVERIFY(axisInfoArrayNode != 0);
        READ_MANDATORY_VARIABLE_NODE(axisInfoArrayNode);
        QVariant axisInfoArray = axisInfoArrayNode->attribute(QOpcUa::NodeAttribute::Value);
        QVERIFY(axisInfoArray.type() == QVariant::List);
        QVERIFY(axisInfoArray.toList().length() == 3);
        QCOMPARE(axisInfoArray.toList()[0].value<QOpcUa::QAxisInformation>(), testAxisInfo[0]);
        QCOMPARE(axisInfoArray.toList()[1].value<QOpcUa::QAxisInformation>(), testAxisInfo[1]);
        QCOMPARE(axisInfoArray.toList()[2].value<QOpcUa::QAxisInformation>(), testAxisInfo[2]);

        QScopedPointer<QOpcUaNode> xVArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.XV"));
        QVERIFY(xVArrayNode != 0);
        READ_MANDATORY_VARIABLE_NODE(xVArrayNode);
        QVariant xVArray = xVArrayNode->attribute(QOpcUa::NodeAttribute::Value);
        QVERIFY(xVArray.type() == QVariant::List);
        QVERIFY(xVArray.toList().length() == 3);
        QCOMPARE(xVArray.toList()[0].value<QOpcUa::QXValue>(), testXV[0]);
        QCOMPARE(xVArray.toList()[1].value<QOpcUa::QXValue>(), testXV[1]);
        QCOMPARE(xVArray.toList()[2].value<QOpcUa::QXValue>(), testXV[2]);
    } else {
        QWARN("ExtensionObject types are not supported by freeopcua");
    }

    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("XmlElement support is not yet implemented in the freeopcua backend");

    QScopedPointer<QOpcUaNode> xmlElementArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.XmlElement"));
    QVERIFY(xmlElementArrayNode != 0);
    READ_MANDATORY_VARIABLE_NODE(xmlElementArrayNode)
    QVariant xmlElementArray = xmlElementArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(xmlElementArray.type() == QVariant::List);
    QVERIFY(xmlElementArray.toList().length() == 3);
    QCOMPARE(xmlElementArray.toList()[0].type(), QVariant::String);
    QCOMPARE(xmlElementArray.toList()[0].toString(), xmlElements[0]);
    QCOMPARE(xmlElementArray.toList()[1].toString(), xmlElements[1]);
    QCOMPARE(xmlElementArray.toList()[2].toString(), xmlElements[2]);
}

void Tst_QOpcUaClient::writeScalar()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> booleanNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Boolean"));
    QVERIFY(booleanNode != 0);
    WRITE_VALUE_ATTRIBUTE(booleanNode, true, QOpcUa::Types::Boolean);

    QScopedPointer<QOpcUaNode> int32Node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Int32"));
    QVERIFY(int32Node != 0);
    WRITE_VALUE_ATTRIBUTE(int32Node, std::numeric_limits<qint32>::min(), QOpcUa::Types::Int32);

    QScopedPointer<QOpcUaNode> uint32Node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.UInt32"));
    QVERIFY(uint32Node != 0);
    WRITE_VALUE_ATTRIBUTE(uint32Node, std::numeric_limits<quint32>::max(), QOpcUa::UInt32);

    QScopedPointer<QOpcUaNode> doubleNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Double"));
    QVERIFY(doubleNode != 0);
    WRITE_VALUE_ATTRIBUTE(doubleNode, 42, QOpcUa::Double);

    QScopedPointer<QOpcUaNode> floatNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Float"));
    QVERIFY(floatNode != 0);
    WRITE_VALUE_ATTRIBUTE(floatNode, 42, QOpcUa::Float);

    QScopedPointer<QOpcUaNode> stringNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.String"));
    QVERIFY(stringNode != 0);
    WRITE_VALUE_ATTRIBUTE(stringNode, "QOpcUa Teststring", QOpcUa::String);

    QScopedPointer<QOpcUaNode> dtNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.DateTime"));
    QVERIFY(dtNode != 0);
    WRITE_VALUE_ATTRIBUTE(dtNode, QDateTime::currentDateTime(), QOpcUa::DateTime);

    QScopedPointer<QOpcUaNode> uint16Node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.UInt16"));
    QVERIFY(uint16Node != 0);
    WRITE_VALUE_ATTRIBUTE(uint16Node, std::numeric_limits<quint16>::max(), QOpcUa::UInt16);

    QScopedPointer<QOpcUaNode> int16Node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Int16"));
    QVERIFY(int16Node != 0);
    WRITE_VALUE_ATTRIBUTE(int16Node, std::numeric_limits<qint16>::min(), QOpcUa::Int16);

    QScopedPointer<QOpcUaNode> uint64Node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.UInt64"));
    QVERIFY(uint64Node != 0);
    WRITE_VALUE_ATTRIBUTE(uint64Node, std::numeric_limits<quint64>::max(), QOpcUa::UInt64);

    QScopedPointer<QOpcUaNode> int64Node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Int64"));
    QVERIFY(int64Node != 0);
    WRITE_VALUE_ATTRIBUTE(int64Node, std::numeric_limits<qint64>::min(), QOpcUa::Int64);

    QScopedPointer<QOpcUaNode> byteNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Byte"));
    QVERIFY(byteNode != 0);
    WRITE_VALUE_ATTRIBUTE(byteNode, std::numeric_limits<quint8>::max(), QOpcUa::Byte);

    QScopedPointer<QOpcUaNode> sbyteNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.SByte"));
    QVERIFY(sbyteNode != 0);
    WRITE_VALUE_ATTRIBUTE(sbyteNode, std::numeric_limits<qint8>::min(), QOpcUa::SByte);

    QScopedPointer<QOpcUaNode> ltNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.LocalizedText"));
    QVERIFY(ltNode != 0);
    WRITE_VALUE_ATTRIBUTE(ltNode, QVariant::fromValue(localizedTexts[0]), QOpcUa::LocalizedText);

    QByteArray withNull("gh");
    withNull.append('\0');
    withNull.append("i");

    QVariant data = withNull;

    QScopedPointer<QOpcUaNode> byteStringNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.ByteString"));
    QVERIFY(byteStringNode != 0);
    WRITE_VALUE_ATTRIBUTE(byteStringNode, data, QOpcUa::ByteString);

    QScopedPointer<QOpcUaNode> guidNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Guid"));
    QVERIFY(guidNode != 0);
    data = QUuid("e0bd5ccd-f571-4545-9352-61a0f8cb9216");
    WRITE_VALUE_ATTRIBUTE(guidNode, data, QOpcUa::Guid);

    QScopedPointer<QOpcUaNode> nodeIdNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.NodeId"));
    QVERIFY(nodeIdNode != 0);
    WRITE_VALUE_ATTRIBUTE(nodeIdNode, "ns=42;s=Test", QOpcUa::NodeId);

    QScopedPointer<QOpcUaNode> qualifiedNameNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.QualifiedName"));
    QVERIFY(qualifiedNameNode != 0);
    WRITE_VALUE_ATTRIBUTE(qualifiedNameNode, QVariant::fromValue(QOpcUa::QQualifiedName(0, QLatin1String("Test0"))), QOpcUa::QualifiedName);

    QScopedPointer<QOpcUaNode> statusCodeNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.StatusCode"));
    QVERIFY(statusCodeNode != 0);
    WRITE_VALUE_ATTRIBUTE(statusCodeNode, QOpcUa::UaStatusCode::BadInternalError, QOpcUa::StatusCode);

    if (opcuaClient->backend() != QLatin1String("freeopcua")) {
        QScopedPointer<QOpcUaNode> rangeNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Range"));
        QVERIFY(rangeNode != 0);
        WRITE_VALUE_ATTRIBUTE(rangeNode, QVariant::fromValue(testRanges[0]), QOpcUa::Range);

        QScopedPointer<QOpcUaNode> euiNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.EUInformation"));
        QVERIFY(euiNode != 0);
        WRITE_VALUE_ATTRIBUTE(euiNode, QVariant::fromValue(testEUInfos[0]), QOpcUa::EUInformation);

        QScopedPointer<QOpcUaNode> complexNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.ComplexNumber"));
        QVERIFY(complexNode != 0);
        WRITE_VALUE_ATTRIBUTE(complexNode, QVariant::fromValue(testComplex[0]), QOpcUa::ComplexNumber);

        QScopedPointer<QOpcUaNode> doubleComplexNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.DoubleComplexNumber"));
        QVERIFY(doubleComplexNode != 0);
        WRITE_VALUE_ATTRIBUTE(doubleComplexNode, QVariant::fromValue(testDoubleComplex[0]), QOpcUa::DoubleComplexNumber);

        QScopedPointer<QOpcUaNode> axisInfoNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.AxisInformation"));
        QVERIFY(axisInfoNode != 0);
        WRITE_VALUE_ATTRIBUTE(axisInfoNode, QVariant::fromValue(testAxisInfo[0]), QOpcUa::AxisInformation);

        QScopedPointer<QOpcUaNode> xVNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.XV"));
        QVERIFY(xVNode != 0);
        WRITE_VALUE_ATTRIBUTE(xVNode, QVariant::fromValue(testXV[0]), QOpcUa::XV);
    } else {
        QWARN("ExtensionObject types are not supported by freeopcua");
    }

    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("XmlElement support is not yet implemented in the freeopcua backend");

    QScopedPointer<QOpcUaNode> xmlElementNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.XmlElement"));
    QVERIFY(xmlElementNode != 0);
    WRITE_VALUE_ATTRIBUTE(xmlElementNode, xmlElements[0], QOpcUa::XmlElement);
}

void Tst_QOpcUaClient::readScalar()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Boolean"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant booleanScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(booleanScalar.isValid());
    QCOMPARE(booleanScalar.type(), QVariant::Bool);
    QCOMPARE(booleanScalar.toBool(), true);

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Int32"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant int32Scalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(int32Scalar.isValid());
    QCOMPARE(int32Scalar.type(), QVariant::Int);
    QCOMPARE(int32Scalar.toInt(), std::numeric_limits<qint32>::min());

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.UInt32"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant uint32Scalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(uint32Scalar.isValid());
    QCOMPARE(uint32Scalar.type(), QVariant::UInt);
    QCOMPARE(uint32Scalar.toUInt(), std::numeric_limits<quint32>::max());

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Double"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant doubleScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(doubleScalar.isValid());
    QCOMPARE(doubleScalar.type(), QVariant::Double);
    QCOMPARE(doubleScalar.toDouble(), double(42));

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Float"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant floatScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(floatScalar.isValid());
    QVERIFY(floatScalar.userType() == QMetaType::Float);
    QCOMPARE(floatScalar.toFloat(), float(42));

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.String"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant stringScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(stringScalar.isValid());
    QCOMPARE(stringScalar.type(), QVariant::String);
    QCOMPARE(stringScalar.toString(), QStringLiteral("QOpcUa Teststring"));

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.DateTime"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant dateTimeScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QCOMPARE(dateTimeScalar.type(), QVariant::DateTime);
    QVERIFY(dateTimeScalar.isValid());

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.LocalizedText"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant ltScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(ltScalar.isValid());
    QVERIFY(ltScalar.value<QOpcUa::QLocalizedText>() == localizedTexts[0]);

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.UInt16"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant uint16Scalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(uint16Scalar.isValid());
    QVERIFY(uint16Scalar.userType() == QMetaType::UShort);
    QVERIFY(uint16Scalar == std::numeric_limits<quint16>::max());

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Int16"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant int16Scalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(int16Scalar.isValid());
    QVERIFY(int16Scalar.userType() == QMetaType::Short);
    QVERIFY(int16Scalar == std::numeric_limits<qint16>::min());

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.UInt64"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant uint64Scalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(uint64Scalar.isValid());
    QCOMPARE(uint64Scalar.type(), QVariant::ULongLong);
    QVERIFY(uint64Scalar == std::numeric_limits<quint64>::max());

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Int64"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant int64Scalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(int64Scalar.isValid());
    QCOMPARE(int64Scalar.type(), QVariant::LongLong);
    QVERIFY(int64Scalar == std::numeric_limits<qint64>::min());

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Byte"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant byteScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(byteScalar.isValid());
    QVERIFY(byteScalar.userType() == QMetaType::UChar);
    QVERIFY(byteScalar == std::numeric_limits<quint8>::max());

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.SByte"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant sbyteScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(sbyteScalar.isValid());
    QVERIFY(sbyteScalar.userType() == QMetaType::SChar);
    QVERIFY(sbyteScalar == std::numeric_limits<qint8>::min());

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.ByteString"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant byteStringScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(byteStringScalar.isValid());
    QVERIFY(byteStringScalar.userType() == QMetaType::QByteArray);
    QByteArray withNull("gh");
    withNull.append('\0');
    withNull.append("i");
    QVERIFY(byteStringScalar == withNull);

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Guid"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant guidScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(guidScalar.isValid());
    QVERIFY(guidScalar.userType() == QMetaType::QUuid);
    QCOMPARE(guidScalar.toUuid(), QUuid("e0bd5ccd-f571-4545-9352-61a0f8cb9216"));

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.NodeId"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant nodeIdScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(nodeIdScalar.isValid());
    QCOMPARE(nodeIdScalar.type(), QVariant::String);
    QCOMPARE(nodeIdScalar.toString(), QStringLiteral("ns=42;s=Test"));

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.QualifiedName"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node)
    QVariant qualifiedNameScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(qualifiedNameScalar.value<QOpcUa::QQualifiedName>() == QOpcUa::QQualifiedName(0, "Test0"));

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.StatusCode"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node);
    QVariant statusCodeScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(statusCodeScalar.isValid());
    QCOMPARE(statusCodeScalar.value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadInternalError);

    if (opcuaClient->backend() != QLatin1String("freeopcua")) {
        node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Range"));
        QVERIFY(node != 0);
        READ_MANDATORY_VARIABLE_NODE(node);
        QVERIFY(node->attribute(QOpcUa::NodeAttribute::Value).value<QOpcUa::QRange>() == testRanges[0]);

        node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.EUInformation"));
        QVERIFY(node != 0);
        READ_MANDATORY_VARIABLE_NODE(node);
        QVERIFY(node->attribute(QOpcUa::NodeAttribute::Value).value<QOpcUa::QEUInformation>() == testEUInfos[0]);

        node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.ComplexNumber"));
        QVERIFY(node != 0);
        READ_MANDATORY_VARIABLE_NODE(node);
        QVERIFY(node->attribute(QOpcUa::NodeAttribute::Value).value<QOpcUa::QComplexNumber>() == testComplex[0]);

        node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.DoubleComplexNumber"));
        QVERIFY(node != 0);
        READ_MANDATORY_VARIABLE_NODE(node);
        QVERIFY(node->attribute(QOpcUa::NodeAttribute::Value).value<QOpcUa::QDoubleComplexNumber>() == testDoubleComplex[0]);

        node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.AxisInformation"));
        QVERIFY(node != 0);
        READ_MANDATORY_VARIABLE_NODE(node);
        QVERIFY(node->attribute(QOpcUa::NodeAttribute::Value).value<QOpcUa::QAxisInformation>() == testAxisInfo[0]);

        node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.XV"));
        QVERIFY(node != 0);
        READ_MANDATORY_VARIABLE_NODE(node);
        QVERIFY(node->attribute(QOpcUa::NodeAttribute::Value).value<QOpcUa::QXValue>() == testXV[0]);
    } else {
        QWARN("ExtensionObject types are not supported by freeopcua");
    }

    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("XmlElement support is not yet implemented in the freeopcua backend");

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.XmlElement"));
    QVERIFY(node != 0);
    READ_MANDATORY_VARIABLE_NODE(node)
    QVariant xmlElementScalar = node->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(xmlElementScalar.isValid());
    QCOMPARE(xmlElementScalar.type(), QVariant::String);
    QCOMPARE(xmlElementScalar.toString(), xmlElements[0]);
}

void Tst_QOpcUaClient::indexRange()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int32"));

    QVariantList list({0, 1, 2, 3, 4, 5, 6, 7});

    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Types::Int32);

    QSignalSpy attributeWrittenSpy(node.data(), &QOpcUaNode::attributeWritten);

    node->writeAttributeRange(QOpcUa::NodeAttribute::Value, QVariantList({10, 11, 12, 13}), "0:3", QOpcUa::Types::Int32);
    attributeWrittenSpy.wait();
    QVERIFY(attributeWrittenSpy.size() == 1);
    QVERIFY(attributeWrittenSpy.at(0).at(1).value<QOpcUa::UaStatusCode>() == QOpcUa::UaStatusCode::Good);

    QSignalSpy attributeReadSpy(node.data(), &QOpcUaNode::attributeRead);
    node->readAttributeRange(QOpcUa::NodeAttribute::Value, "0:6");
    attributeReadSpy.wait();
    QCOMPARE(attributeReadSpy.count(), 1);

    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::Value).toList(), QVariantList({10, 11, 12, 13, 4, 5, 6}));
}

void Tst_QOpcUaClient::invalidIndexRange()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int32"));

    QVariantList list({0, 1, 2, 3, 4, 5, 6, 7});

    WRITE_VALUE_ATTRIBUTE(node, list, QOpcUa::Types::Int32);

    QSignalSpy attributeWrittenSpy(node.data(), &QOpcUaNode::attributeWritten);

    node->writeAttributeRange(QOpcUa::NodeAttribute::Value, QVariantList({10, 11, 12, 13}), "notavalidrange", QOpcUa::Types::Int32);
    attributeWrittenSpy.wait();
    QVERIFY(attributeWrittenSpy.size() == 1);
    QVERIFY(attributeWrittenSpy.at(0).at(1).value<QOpcUa::UaStatusCode>() == QOpcUa::UaStatusCode::BadIndexRangeInvalid);

    QSignalSpy attributeReadSpy(node.data(), &QOpcUaNode::attributeRead);
    node->readAttributeRange(QOpcUa::NodeAttribute::Value, "notavalidrange");
    attributeReadSpy.wait();
    QCOMPARE(attributeReadSpy.count(), 1);

    QCOMPARE(node->attributeError(QOpcUa::NodeAttribute::Value), QOpcUa::UaStatusCode::BadIndexRangeInvalid);
}

void Tst_QOpcUaClient::subscriptionIndexRange()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> integerArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int32"));
    QVERIFY(integerArrayNode != 0);

    QOpcUaMonitoringParameters p(100);
    p.setIndexRange(QStringLiteral("1"));
    QSignalSpy monitoringEnabledSpy(integerArrayNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QSignalSpy monitoringDisabledSpy(integerArrayNode.data(), &QOpcUaNode::disableMonitoringFinished);
    QSignalSpy writeSpy(integerArrayNode.data(), &QOpcUaNode::attributeWritten);
    QSignalSpy dataChangeSpy(integerArrayNode.data(), &QOpcUaNode::attributeUpdated);

    QVariantList l({0, 1});
    WRITE_VALUE_ATTRIBUTE(integerArrayNode, l, QOpcUa::Types::Int32);
    writeSpy.clear();

    integerArrayNode->enableMonitoring(QOpcUa::NodeAttribute::Value, p);
    monitoringEnabledSpy.wait();
    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringEnabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    dataChangeSpy.wait(); // Wait for the initial data change
    dataChangeSpy.clear();
    integerArrayNode->writeAttributeRange(QOpcUa::NodeAttribute::Value, 10, "0", QOpcUa::Types::Int32); // Write the first element of the array
    writeSpy.wait();
    QCOMPARE(writeSpy.size(), 1);
    QCOMPARE(writeSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(writeSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    dataChangeSpy.wait();
    QCOMPARE(dataChangeSpy.size(), 0);

    writeSpy.clear();
    integerArrayNode->writeAttributeRange(QOpcUa::NodeAttribute::Value, 10, "1", QOpcUa::Types::Int32); // Write the second element of the array
    writeSpy.wait();
    QCOMPARE(writeSpy.size(), 1);
    QCOMPARE(writeSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(writeSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    dataChangeSpy.wait();
    QCOMPARE(dataChangeSpy.size(), 1);
    QCOMPARE(integerArrayNode->attribute(QOpcUa::NodeAttribute::Value).toDouble(), 10.0);

    integerArrayNode->disableMonitoring(QOpcUa::NodeAttribute::Value);
    monitoringDisabledSpy.wait();
    QCOMPARE(monitoringDisabledSpy.size(), 1);
    QCOMPARE(monitoringDisabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringDisabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
}

void Tst_QOpcUaClient::subscriptionDataChangeFilter()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("DataChangeFilter support is not implemented in the freeopcua plugin");

    QScopedPointer<QOpcUaNode> doubleNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Double"));
    QVERIFY(doubleNode != 0);

    QOpcUaMonitoringParameters p(100);

    QSignalSpy monitoringEnabledSpy(doubleNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QSignalSpy monitoringDisabledSpy(doubleNode.data(), &QOpcUaNode::disableMonitoringFinished);
    QSignalSpy dataChangeSpy(doubleNode.data(), &QOpcUaNode::attributeUpdated);
    QSignalSpy monitoringModifiedSpy(doubleNode.data(), &QOpcUaNode::monitoringStatusChanged);

    WRITE_VALUE_ATTRIBUTE(doubleNode, 1.0, QOpcUa::Types::Double);

    doubleNode->enableMonitoring(QOpcUa::NodeAttribute::Value, p);
    monitoringEnabledSpy.wait();
    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringEnabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    dataChangeSpy.wait(); // Wait for the initial data change
    QCOMPARE(dataChangeSpy.size(), 1);
    dataChangeSpy.clear();

    WRITE_VALUE_ATTRIBUTE(doubleNode, 1.5, QOpcUa::Types::Double);

    dataChangeSpy.wait();
    QCOMPARE(dataChangeSpy.size(), 1); // Data change without filter
    QCOMPARE(doubleNode->attribute(QOpcUa::NodeAttribute::Value), 1.5);
    dataChangeSpy.clear();

    QOpcUaMonitoringParameters::DataChangeFilter filter;
    filter.deadbandType = QOpcUaMonitoringParameters::DataChangeFilter::DeadbandType::Absolute;
    filter.trigger = QOpcUaMonitoringParameters::DataChangeFilter::DataChangeTrigger::StatusValue;
    filter.deadbandValue = 1.0;
    doubleNode->modifyDataChangeFilter(QOpcUa::NodeAttribute::Value, filter);
    monitoringModifiedSpy.wait();
    QVERIFY(monitoringModifiedSpy.size() == 1);
    QVERIFY(monitoringModifiedSpy.at(0).at(0).value<QOpcUa::NodeAttribute>() == QOpcUa::NodeAttribute::Value);
    QVERIFY(monitoringModifiedSpy.at(0).at(1).value<QOpcUaMonitoringParameters::Parameters>() & QOpcUaMonitoringParameters::Parameter::Filter);
    QVERIFY(monitoringModifiedSpy.at(0).at(2).value<QOpcUa::UaStatusCode>() == QOpcUa::UaStatusCode::Good);

    WRITE_VALUE_ATTRIBUTE(doubleNode, 2.0, QOpcUa::Types::Double);

    dataChangeSpy.wait();
    QCOMPARE(dataChangeSpy.size(), 0); // Filter is active and delta is < 1

    WRITE_VALUE_ATTRIBUTE(doubleNode, 3.0, QOpcUa::Types::Double);

    dataChangeSpy.wait();
    QCOMPARE(dataChangeSpy.size(), 1); // delta == 1, a data change is expected
    QCOMPARE(doubleNode->attribute(QOpcUa::NodeAttribute::Value), 3.0);

    doubleNode->disableMonitoring(QOpcUa::NodeAttribute::Value);
    monitoringDisabledSpy.wait();
    QCOMPARE(monitoringDisabledSpy.size(), 1);
    QCOMPARE(monitoringDisabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringDisabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
}

void Tst_QOpcUaClient::modifyPublishingMode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("Modification of monitoring is not supported in the freeopcua plugin");

    QScopedPointer<QOpcUaNode> doubleNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Double"));
    QVERIFY(doubleNode != 0);

    QOpcUaMonitoringParameters p(100);

    QSignalSpy monitoringEnabledSpy(doubleNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QSignalSpy monitoringDisabledSpy(doubleNode.data(), &QOpcUaNode::disableMonitoringFinished);
    QSignalSpy dataChangeSpy(doubleNode.data(), &QOpcUaNode::attributeUpdated);
    QSignalSpy monitoringStatusSpy(doubleNode.data(), &QOpcUaNode::monitoringStatusChanged);

    WRITE_VALUE_ATTRIBUTE(doubleNode, 1.0, QOpcUa::Types::Double);

    doubleNode->enableMonitoring(QOpcUa::NodeAttribute::Value, p);
    monitoringEnabledSpy.wait();
    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringEnabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    dataChangeSpy.wait(); // Wait for the initial data change
    QCOMPARE(dataChangeSpy.size(), 1);
    dataChangeSpy.clear();

    WRITE_VALUE_ATTRIBUTE(doubleNode, 1.5, QOpcUa::Types::Double);

    dataChangeSpy.wait();
    QCOMPARE(dataChangeSpy.size(), 1);
    dataChangeSpy.clear();

    doubleNode->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::PublishingEnabled, false);
    monitoringStatusSpy.wait();
    QCOMPARE(monitoringStatusSpy.size(), 1);
    QCOMPARE(monitoringStatusSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    WRITE_VALUE_ATTRIBUTE(doubleNode, 3.0, QOpcUa::Types::Double);

    dataChangeSpy.wait();
    QCOMPARE(dataChangeSpy.size(), 0);

    doubleNode->disableMonitoring(QOpcUa::NodeAttribute::Value);
    monitoringDisabledSpy.wait();
    QCOMPARE(monitoringDisabledSpy.size(), 1);
    QCOMPARE(monitoringDisabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringDisabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
}

void Tst_QOpcUaClient::modifyMonitoringMode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("Modification of monitoring is not supported in the freeopcua plugin");

    QScopedPointer<QOpcUaNode> doubleNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Double"));
    QVERIFY(doubleNode != 0);

    QOpcUaMonitoringParameters p(100);

    QSignalSpy monitoringEnabledSpy(doubleNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QSignalSpy monitoringDisabledSpy(doubleNode.data(), &QOpcUaNode::disableMonitoringFinished);
    QSignalSpy dataChangeSpy(doubleNode.data(), &QOpcUaNode::attributeUpdated);
    QSignalSpy monitoringStatusSpy(doubleNode.data(), &QOpcUaNode::monitoringStatusChanged);

    WRITE_VALUE_ATTRIBUTE(doubleNode, 1.0, QOpcUa::Types::Double);

    doubleNode->enableMonitoring(QOpcUa::NodeAttribute::Value, p);
    monitoringEnabledSpy.wait();
    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringEnabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    dataChangeSpy.wait(); // Wait for the initial data change
    QCOMPARE(dataChangeSpy.size(), 1);
    dataChangeSpy.clear();

    WRITE_VALUE_ATTRIBUTE(doubleNode, 1.5, QOpcUa::Types::Double);

    dataChangeSpy.wait();
    QCOMPARE(dataChangeSpy.size(), 1);
    dataChangeSpy.clear();

    doubleNode->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::MonitoringMode,
                                 QVariant::fromValue(QOpcUaMonitoringParameters::MonitoringMode::Disabled));
    monitoringStatusSpy.wait();
    QCOMPARE(monitoringStatusSpy.size(), 1);
    QCOMPARE(monitoringStatusSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    WRITE_VALUE_ATTRIBUTE(doubleNode, 3.0, QOpcUa::Types::Double);

    dataChangeSpy.wait();
    QCOMPARE(dataChangeSpy.size(), 0);

    doubleNode->disableMonitoring(QOpcUa::NodeAttribute::Value);
    monitoringDisabledSpy.wait();
    QCOMPARE(monitoringDisabledSpy.size(), 1);
    QCOMPARE(monitoringDisabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringDisabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
}

void Tst_QOpcUaClient::modifyMonitoredItem()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("Modification of monitoring is not supported in the freeopcua plugin");

    QScopedPointer<QOpcUaNode> doubleNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Double"));
    QVERIFY(doubleNode != 0);

    QOpcUaMonitoringParameters p(100);

    QSignalSpy monitoringEnabledSpy(doubleNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QSignalSpy monitoringDisabledSpy(doubleNode.data(), &QOpcUaNode::disableMonitoringFinished);
    QSignalSpy dataChangeSpy(doubleNode.data(), &QOpcUaNode::attributeUpdated);
    QSignalSpy monitoringStatusSpy(doubleNode.data(), &QOpcUaNode::monitoringStatusChanged);

    WRITE_VALUE_ATTRIBUTE(doubleNode, 1.0, QOpcUa::Types::Double);

    doubleNode->enableMonitoring(QOpcUa::NodeAttribute::Value, p);
    monitoringEnabledSpy.wait();
    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringEnabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    dataChangeSpy.wait(); // Wait for the initial data change
    QCOMPARE(dataChangeSpy.size(), 1);
    dataChangeSpy.clear();

    WRITE_VALUE_ATTRIBUTE(doubleNode, 1.5, QOpcUa::Types::Double);

    dataChangeSpy.wait();
    QCOMPARE(dataChangeSpy.size(), 1);
    QCOMPARE(dataChangeSpy.at(0).at(1).value<double>(), 1.5);
    dataChangeSpy.clear();

    doubleNode->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::SamplingInterval, 50.0);
    monitoringStatusSpy.wait();
    QCOMPARE(monitoringStatusSpy.size(), 1);
    QCOMPARE(monitoringStatusSpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(monitoringStatusSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QVERIFY(monitoringStatusSpy.at(0).at(1).value<QOpcUaMonitoringParameters::Parameters>() & QOpcUaMonitoringParameters::Parameter::SamplingInterval);

    WRITE_VALUE_ATTRIBUTE(doubleNode, 3.0, QOpcUa::Types::Double);

    dataChangeSpy.wait();
    QCOMPARE(dataChangeSpy.size(), 1);

    doubleNode->disableMonitoring(QOpcUa::NodeAttribute::Value);
    monitoringDisabledSpy.wait();
    QCOMPARE(monitoringDisabledSpy.size(), 1);
    QCOMPARE(monitoringDisabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringDisabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
}

void Tst_QOpcUaClient::addDuplicateMonitoredItem()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> doubleNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Double"));
    QVERIFY(doubleNode != 0);

    QOpcUaMonitoringParameters p(100);

    QSignalSpy monitoringEnabledSpy(doubleNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QSignalSpy monitoringDisabledSpy(doubleNode.data(), &QOpcUaNode::disableMonitoringFinished);

    doubleNode->enableMonitoring(QOpcUa::NodeAttribute::Value, p);
    monitoringEnabledSpy.wait();
    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringEnabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(doubleNode->monitoringStatus(QOpcUa::NodeAttribute::Value).statusCode(), QOpcUa::UaStatusCode::Good);

    monitoringEnabledSpy.clear();
    doubleNode->enableMonitoring(QOpcUa::NodeAttribute::Value, p);
    monitoringEnabledSpy.wait();
    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringEnabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadEntryExists);
    QCOMPARE(doubleNode->monitoringStatus(QOpcUa::NodeAttribute::Value).statusCode(), QOpcUa::UaStatusCode::Good);

    doubleNode->disableMonitoring(QOpcUa::NodeAttribute::Value);
    monitoringDisabledSpy.wait();
    QCOMPARE(monitoringDisabledSpy.size(), 1);
    QCOMPARE(monitoringDisabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringDisabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
}

void Tst_QOpcUaClient::checkMonitoredItemCleanup()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> readWriteNode(opcuaClient->node("ns=3;s=TestNode.ReadWrite"));
    QVERIFY(readWriteNode != nullptr);

    QScopedPointer<QOpcUaNode> serverNode(opcuaClient->node("ns=0;i=2253"));
    QVERIFY(serverNode != nullptr);

    QSignalSpy monitoringEnabledSpy(readWriteNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QOpcUa::NodeAttributes attr = QOpcUa::NodeAttribute::Value | QOpcUa::NodeAttribute::BrowseName;
    readWriteNode->enableMonitoring(attr, QOpcUaMonitoringParameters(100, QOpcUaMonitoringParameters::SubscriptionType::Exclusive));
    monitoringEnabledSpy.wait();
    if (monitoringEnabledSpy.size() != 2)
        monitoringEnabledSpy.wait();
    QCOMPARE(monitoringEnabledSpy.size(), 2);

    for (auto entry : monitoringEnabledSpy) {
        QVERIFY(attr & entry.at(0).value<QOpcUa::NodeAttribute>());
        QCOMPARE(entry.at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    }

    quint32 subscriptionId = readWriteNode->monitoringStatus(QOpcUa::NodeAttribute::Value).subscriptionId();
    QCOMPARE(subscriptionId, readWriteNode->monitoringStatus(QOpcUa::NodeAttribute::BrowseName).subscriptionId());

    QSignalSpy methodSpy(serverNode.data(), &QOpcUaNode::methodCallFinished);
    QVector<QOpcUa::TypedVariant> parameter;
    parameter.append(QOpcUa::TypedVariant(QVariant(quint32(subscriptionId)), QOpcUa::Types::UInt32));
    serverNode->callMethod("ns=0;i=11492", parameter); // Call the getMonitoredItems method

    methodSpy.wait();
    QCOMPARE(methodSpy.size(), 1);

    for (auto entry : methodSpy) {
        QCOMPARE(entry.at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
        QCOMPARE(entry.at(1).toList().size(), 2); // Two monitored items
        QCOMPARE(entry.at(1).toList().at(0).toList().size(), 2); // One server handle for each monitored item
        QCOMPARE(entry.at(1).toList().at(1).toList().size(), 2); // One client handle for each monitored item
    }

    readWriteNode.reset(); // Delete the node object

    methodSpy.wait(); // Give the backend some time to process the deletion request
    methodSpy.clear();
    serverNode->callMethod("ns=0;i=11492", parameter); // Call the getMonitoredItems method
    methodSpy.wait();
    QCOMPARE(methodSpy.size(), 1);

    for (auto entry : methodSpy) {
        QCOMPARE(entry.at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::BadSubscriptionIdInvalid);
    }
}

void Tst_QOpcUaClient::stringCharset()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> stringScalarNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.String"));
    QScopedPointer<QOpcUaNode> localizedScalarNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.LocalizedText"));
    QScopedPointer<QOpcUaNode> stringArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.String"));
    QScopedPointer<QOpcUaNode> localizedArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.LocalizedText"));

    QVERIFY(stringScalarNode != 0);
    QVERIFY(localizedScalarNode != 0);
    QVERIFY(stringArrayNode != 0);
    QVERIFY(localizedArrayNode != 0);

    QString testString = QString::fromUtf8("🞀🞁🞂🞃");
    QOpcUa::QLocalizedText lt1("en", testString);
    QOpcUa::QLocalizedText lt2("de", testString);

    WRITE_VALUE_ATTRIBUTE(stringScalarNode, testString, QOpcUa::String);
    WRITE_VALUE_ATTRIBUTE(localizedScalarNode, QVariant::fromValue(localizedTexts[0]), QOpcUa::LocalizedText);

    QVariantList l;
    l.append(testString);
    l.append(testString);

    WRITE_VALUE_ATTRIBUTE(stringArrayNode, l, QOpcUa::String);

    l.clear();
    l.append(QVariant::fromValue(lt1));
    l.append(QVariant::fromValue(lt2));

    WRITE_VALUE_ATTRIBUTE(localizedArrayNode, l, QOpcUa::LocalizedText);

    READ_MANDATORY_VARIABLE_NODE(stringArrayNode);
    READ_MANDATORY_VARIABLE_NODE(localizedArrayNode);
    READ_MANDATORY_VARIABLE_NODE(stringScalarNode);
    READ_MANDATORY_VARIABLE_NODE(localizedScalarNode);

    QVariant result = stringScalarNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(result.toString() == testString);
    result = localizedScalarNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(result.value<QOpcUa::QLocalizedText>() == localizedTexts[0]);

    result = stringArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(result.type() == QVariant::List);
    QVERIFY(result.toList().length() == 2);
    QVERIFY(result.toList()[0].type() == QVariant::String);
    QVERIFY(result.toList()[0].toString() == testString);
    QVERIFY(result.toList()[1].type() == QVariant::String);
    QVERIFY(result.toList()[1].toString() == testString);

    result = localizedArrayNode->attribute(QOpcUa::NodeAttribute::Value);
    QVERIFY(result.type() == QVariant::List);
    QVERIFY(result.toList().length() == 2);
    QVERIFY(result.toList()[0].value<QOpcUa::QLocalizedText>() == lt1);
    QVERIFY(result.toList()[1].value<QOpcUa::QLocalizedText>() == lt2);
}

void Tst_QOpcUaClient::namespaceArray()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QCOMPARE(opcuaClient->namespaceArray().size(), 0);

    QSignalSpy spy(opcuaClient, &QOpcUaClient::namespaceArrayUpdated);
    QCOMPARE(opcuaClient->updateNamespaceArray(), true);

    spy.wait();
    QCOMPARE(spy.size(), 1);

    QStringList namespaces = opcuaClient->namespaceArray();
    QVERIFY(namespaces.size() == 4);

    int nsIndex = namespaces.indexOf("http://qt-project.org");
    QVERIFY(nsIndex > 0);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(QOpcUa::nodeIdFromString(nsIndex, QStringLiteral("Demo.Static.Scalar.String"))));
    READ_MANDATORY_BASE_NODE(node);

    QCOMPARE(node->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUa::QLocalizedText>().text, QStringLiteral("StringScalarTest"));
}

void Tst_QOpcUaClient::dateTimeConversion()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> dateTimeScalarNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.DateTime"));

    QVERIFY(dateTimeScalarNode != 0);

    QDateTime dt = QDateTime::currentDateTime();

    WRITE_VALUE_ATTRIBUTE(dateTimeScalarNode, dt, QOpcUa::Types::DateTime);
    READ_MANDATORY_VARIABLE_NODE(dateTimeScalarNode);

    QDateTime result = dateTimeScalarNode->attribute(QOpcUa::NodeAttribute::Value).toDateTime();
    QVERIFY(dt == result);
}

void Tst_QOpcUaClient::timeStamps()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> stringScalarNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.String"));

    QVERIFY(stringScalarNode != 0);

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
    QSignalSpy dataChangeSpy(stringScalarNode.data(), &QOpcUaNode::attributeUpdated);

    QTest::qWait(10); // Make sure the timestamp has a chance to change

    WRITE_VALUE_ATTRIBUTE(stringScalarNode, "Reset", QOpcUa::Types::String);

    stringScalarNode->enableMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters(100));

    monitoringEnabledSpy.wait();
    QCOMPARE(monitoringEnabledSpy.size(), 1);
    QCOMPARE(monitoringEnabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringEnabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    dataChangeSpy.wait();
    const QDateTime sourceDataChange = stringScalarNode->sourceTimestamp(QOpcUa::NodeAttribute::Value);
    const QDateTime serverDataChange = stringScalarNode->serverTimestamp(QOpcUa::NodeAttribute::Value);

    QVERIFY(sourceDataChange.isValid());
    QVERIFY(serverDataChange.isValid());
    QVERIFY(sourceRead < sourceDataChange);
    QVERIFY(serverRead < serverDataChange);

    stringScalarNode->disableMonitoring(QOpcUa::NodeAttribute::Value);
    monitoringDisabledSpy.wait();
    QCOMPARE(monitoringDisabledSpy.size(), 1);
    QCOMPARE(monitoringDisabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::Value);
    QCOMPARE(monitoringDisabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
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

    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("State change on connection loss is not implemented in the freeopcua plugin");

    QCOMPARE(opcuaClient->state(), QOpcUaClient::ClientState::Connected);

    QScopedPointer<QOpcUaNode> stringNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.String"));

    QSignalSpy stateSpy(opcuaClient, &QOpcUaClient::stateChanged);
    QSignalSpy readSpy(stringNode.data(), &QOpcUaNode::attributeRead);

    m_serverProcess.kill();
    m_serverProcess.waitForFinished();
    QCOMPARE(m_serverProcess.state(), QProcess::ProcessState::NotRunning);

    stringNode->readAttributes(QOpcUa::NodeAttribute::BrowseName);

    readSpy.wait();
    stateSpy.wait(10000); // uacpp and open62541 use a timeout of 5 seconds for service calls, better be safe.
    QCOMPARE(readSpy.size(), 1);
    QVERIFY(readSpy.at(0).at(0).value<QOpcUa::NodeAttributes>() & QOpcUa::NodeAttribute::BrowseName);
    QCOMPARE(stringNode->attributeError(QOpcUa::NodeAttribute::BrowseName), QOpcUa::UaStatusCode::BadConnectionClosed);

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

    Tst_QOpcUaClient tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "tst_client.moc"


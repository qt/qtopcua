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
#include <QtOpcUa/QOpcUaMonitoredEvent>
#include <QtOpcUa/QOpcUaMonitoredValue>
#include <QtOpcUa/QOpcUaNode>
#include <QtOpcUa/QOpcUaProvider>

#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtCore/QScopedPointer>
#include <QtCore/QThread>
#include <QtCore/QTimer>

#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>

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
    defineDataMethod(secureConnect_data)
    void secureConnect();
    defineDataMethod(secureConnectToInvalid_data)
    void secureConnectToInvalid();
    defineDataMethod(connectAndDisconnect_data)
    void connectAndDisconnect();

    // Password
    defineDataMethod(connectInvalidPassword_data)
    void connectInvalidPassword();
    defineDataMethod(connectAndDisconnectPassword_data)
    void connectAndDisconnectPassword();

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

    // read & write
    defineDataMethod(read_data)
    void read();
    defineDataMethod(readWrite_data)
    void readWrite();

    defineDataMethod(dataChangeSubscription_data)
    void dataChangeSubscription();
    defineDataMethod(dataChangeSubscriptionInvalidNode_data)
    void dataChangeSubscriptionInvalidNode();
    defineDataMethod(methodCall_data)
    void methodCall();
    defineDataMethod(eventSubscription_data)
    void eventSubscription();
    defineDataMethod(eventSubscribeInvalidNode_data)
    void eventSubscribeInvalidNode();
    defineDataMethod(readRange_data)
    void readRange();
    defineDataMethod(readEui_data)
    void readEui();
    defineDataMethod(readHistorical_data)
    void readHistorical();
    defineDataMethod(writeHistorical_data)
    void writeHistorical();
    defineDataMethod(malformedNodeString_data)
    void malformedNodeString();

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

    defineDataMethod(stringCharset_data)
    void stringCharset();

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
};

Tst_QOpcUaClient::Tst_QOpcUaClient()
{
    qRegisterMetaType<QOpcUaClient::ClientState>("ClientState");

    m_backends = QOpcUaProvider::availableBackends();
}

void Tst_QOpcUaClient::initTestCase()
{
    for (const auto backend: m_backends) {
        QOpcUaClient *client = m_opcUa.createClient(backend);
        QVERIFY2(client != nullptr,
                 QString("Loading backend failed: %1").arg(backend).toLatin1().data());
        client->setParent(this);
        qDebug() << "Using SDK plugin:" << client->backend();
        m_clients.append(client);
    }

    if (qEnvironmentVariableIsEmpty("OPCUA_HOST") && qEnvironmentVariableIsEmpty("OPCUA_PORT")) {
        const QString testServerPath = QDir::currentPath()
                                     + QLatin1String("/../../open62541-testserver/open62541-testserver")
#ifdef Q_OS_WIN
                                     + QLatin1String(".exe")
#endif
                ;
        if (!QFile::exists(testServerPath)) {
            qDebug() << "Server Path:" << testServerPath;
            QSKIP("all auto tests rely on a freeopcua based test-server");
        }

        m_serverProcess.start(testServerPath);
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
    QVERIFY(opcuaClient->state() == QOpcUaClient::Connecting);

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

void Tst_QOpcUaClient::secureConnect()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    QSKIP("Secure connections are not supported by freeopcua-based testserver");
    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("Secure connections are not supported with the freeopcua backend");

    QVERIFY(opcuaClient != 0);

    opcuaClient->secureConnectToEndpoint(QUrl(m_endpoint));
    QVERIFY(opcuaClient->state() == QOpcUaClient::Connecting);
    QTRY_VERIFY2(opcuaClient->state() == QOpcUaClient::Connected, "Could not connect to server");

    QVERIFY(opcuaClient->url() == QUrl(m_endpoint));

    opcuaClient->disconnectFromEndpoint();
    QTRY_VERIFY2(opcuaClient->state() == QOpcUaClient::Disconnected, "Could not disconnect from server");
}

void Tst_QOpcUaClient::secureConnectToInvalid()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    QSKIP("Secure connections are not supported by freeopcua-based testserver");
    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("Secure connections are not supported with the freeopcua backend");

    opcuaClient->secureConnectToEndpoint(QUrl("opc.tcp:127.0.0.1:1234"));
    QVERIFY(opcuaClient->state() == QOpcUaClient::Connecting);

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

void Tst_QOpcUaClient::getRootNode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> root(opcuaClient->node("ns=0;i=84"));
    QVERIFY(root != 0);
    QVERIFY(root->displayName() == QLatin1String("Root"));

    QString nodeId = root->nodeId();
    QCOMPARE(nodeId, QStringLiteral("ns=0;i=84"));
}

void Tst_QOpcUaClient::getChildren()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=1;s=Large.Folder"));
    QVERIFY(node != 0);
    QCOMPARE(node->displayName(), QLatin1String("Large_Folder"));
    QCOMPARE(node->childrenIds().size(), 1001);
}

void Tst_QOpcUaClient::childrenIdsString()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=3;s=testStringIdsFolder"));
    QVERIFY(node != 0);
    QStringList childrenIds = node->childrenIds();
    QCOMPARE(childrenIds.size(), 1);
    QCOMPARE(childrenIds.at(0), "ns=3;s=theStringId");
}

void Tst_QOpcUaClient::childrenIdsGuidNodeId()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=3;s=testGuidIdsFolder"));
    QVERIFY(node != 0);
    const QStringList childrenIds = node->childrenIds();
    QCOMPARE(childrenIds.size(), 1);
    QCOMPARE(childrenIds.at(0), "ns=3;g=08081e75-8e5e-319b-954f-f3a7613dc29b");
}

void Tst_QOpcUaClient::childrenIdsOpaqueNodeId()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=3;s=testOpaqueIdsFolder"));
    QVERIFY(node != 0);
    const QStringList childrenIds = node->childrenIds();
    QCOMPARE(childrenIds.size(), 1);
    QCOMPARE(childrenIds.at(0), "ns=3;b=UXQgZnR3IQ==");
}

void Tst_QOpcUaClient::read()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(readWriteNode));
    QVERIFY(node != 0);
    for (int i = 0; i < numberOfOperations; i++) {
        QCOMPARE(node->type(), QOpcUa::Types::Double);
        QCOMPARE(node->value().type(), QVariant::Double);
    }
}

void Tst_QOpcUaClient::readWrite()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(readWriteNode));
    QVERIFY(node != 0);
    for (int i = 0; i < numberOfOperations; i++) {
        bool result = node->setValue(QVariant(double(i)));
        QVERIFY(result==true);
        QVERIFY(node->value().toInt() ==  i);
    }
}

void Tst_QOpcUaClient::dataChangeSubscription()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(readWriteNode));
    QVERIFY(node != 0);
    bool result = node->setValue(QVariant(double(0)));
    QVERIFY(result == true);
    QTRY_COMPARE(node->value(), 0);

    QScopedPointer<QOpcUaSubscription> subscription(opcuaClient->createSubscription(100));
    QScopedPointer<QOpcUaMonitoredValue> monitoredValue(subscription->addValue(node.data()));
    QVERIFY(monitoredValue != nullptr);
    if (!monitoredValue)
        QFAIL("can not monitor value");

    QSignalSpy valueSpy(monitoredValue.data(), &QOpcUaMonitoredValue::valueChanged);

    result = node->setValue(QVariant(double(42)));
    QVERIFY(result == true);

    valueSpy.wait();
    QCOMPARE(valueSpy.count(), 1);
    QCOMPARE(valueSpy.at(0).at(0).toDouble(), double(42));
}

void Tst_QOpcUaClient::dataChangeSubscriptionInvalidNode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> noDataNode(opcuaClient->node("ns=0;i=84"));
    QVERIFY(noDataNode != 0);
    QScopedPointer<QOpcUaSubscription> subscription(opcuaClient->createSubscription(100));
    QOpcUaMonitoredValue *result = subscription->addValue(noDataNode.data());
    QVERIFY(result == 0);
}

void Tst_QOpcUaClient::methodCall()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QSKIP("Method calls are not implemented in freeopcua-based testserver");
    QVector<QOpcUa::TypedVariant> args;
    QVector<QVariant> ret;
    for (int i = 0; i < 2; i++)
        args.push_back(QOpcUa::TypedVariant(double(4), QOpcUa::Double));

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=3;s=TestFolder"));
    QVERIFY(node != 0);

    bool success = node->call("ns=0;s=IDoNotExist", &args, &ret);
    QVERIFY(success == false);

    success = node->call("ns=3;s=Test.Method.Multiply", &args, &ret);
    QVERIFY(success == true);
    QVERIFY(ret.size() == 1);
    QVERIFY(ret[0].type() == QVariant::Double && ret[0].value<double>() == 16);
}

void Tst_QOpcUaClient::eventSubscription()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QSKIP("Does not reliably work with the testserver");
    if (opcuaClient->backend() == QLatin1String("freeopcua")) {
        QSKIP("Event subscriptions do not yet work with the freeopcua backend");
    }

    QScopedPointer<QOpcUaNode> triggerNode(opcuaClient->node("ns=3;s=TriggerNode"));
    QVERIFY(triggerNode != 0);

    QScopedPointer<QOpcUaSubscription> subscription(opcuaClient->createSubscription(100));
    QOpcUaMonitoredEvent *monitoredEvent = subscription->addEvent(triggerNode.data());
    QVERIFY(monitoredEvent != 0);

    if (!monitoredEvent)
        QFAIL("can not monitor event");

    QSignalSpy monitorSpy(monitoredEvent, &QOpcUaMonitoredEvent::newEvent);

    QScopedPointer<QOpcUaNode> triggerVariable(opcuaClient->node("ns=3;s=TriggerVariable"));
    QVERIFY(triggerVariable != 0);
    bool result = triggerVariable->setValue(QVariant(double(0)));
    QVERIFY(result == true);
    result = triggerVariable->setValue(QVariant(double(1)));
    QVERIFY(result == true);

    QVERIFY(monitorSpy.wait());
    QVector<QVariant> val = monitorSpy.at(0).at(0).toList().toVector();
    QCOMPARE(val.size(), 3);
    QCOMPARE(val.at(0).type(), QVariant::String);
    QCOMPARE(val.at(1).type(), QVariant::String);
    QCOMPARE(val.at(2).type(), QVariant::Int);

    delete monitoredEvent;
}

void Tst_QOpcUaClient::eventSubscribeInvalidNode()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> noEventNode(opcuaClient->node(readWriteNode));
    QVERIFY(noEventNode != 0);
    QScopedPointer<QOpcUaSubscription> subscription(opcuaClient->createSubscription(100));
    QOpcUaMonitoredEvent *monitoredEvent = subscription->addEvent(noEventNode.data());
    QVERIFY(monitoredEvent == 0);
}

void Tst_QOpcUaClient::readRange()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QSKIP("No ranges supported in freeopcua-based testserver");
    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("Ranges are not yet implemented in the freeopcua backend");

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=3;s=ACControl.CurrentTemp.EURange"));
    QVERIFY(node != 0);
    QPair<double, double> range = node->readEuRange();
    QVERIFY(range.first == 0 && range.second == 100);
}

void Tst_QOpcUaClient::readEui()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QSKIP("No engineering unit information supported in freeopcua-based testserver");
    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("Engineering unit information are not yet implemented in the freeopcua backend");

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=3;s=ACControl.CurrentTemp.EngineeringUnits"));
    QVERIFY(node != 0);

    QPair<QString, QString> range = node->readEui();
    QVERIFY(range.first == QString::fromUtf8("°C") && range.second == "Degree fahrenheit");
}

void Tst_QOpcUaClient::readHistorical()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QSKIP("History is not supported in freeopcua-based testserver");
    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("History not yet implemented in the freeopcua backend");

    // TODO test with backend/server that supports this
    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=3;s=ACControl.CurrentTemp"));
    QVERIFY(node != 0);

    QVector<QPair<QVariant, QDateTime> > result;
    result = node->readHistorical(5, QDateTime::currentDateTime(), QDateTime::currentDateTime());
    QVERIFY(result.size() != 0);
}

void Tst_QOpcUaClient::writeHistorical()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QSKIP("History is not supported in freeopcua-based testserver");
    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("History not yet implemented in the freeopcua backend");

    // TODO test with backend/server that supports this
    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=3;s=ACControl.CurrentTemp"));
    QVERIFY(node != 0);

    QVector<QPair<QVariant, QDateTime> > data;
    bool result = node->writeHistorical(QOpcUa::Double, data);
    QVERIFY(result == true);
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

void Tst_QOpcUaClient::multipleClients()
{
    QScopedPointer<QOpcUaClient> a(m_opcUa.createClient(m_backends[0]));
    a->connectToEndpoint(m_endpoint);
    QTRY_VERIFY2(a->state() == QOpcUaClient::Connected, "Could not connect to server");
    QScopedPointer<QOpcUaNode> node(a->node(readWriteNode));
    QCOMPARE(node->value().toDouble(), 42.0);
    QScopedPointer<QOpcUaClient> b(m_opcUa.createClient(m_backends[0]));
    b->connectToEndpoint(m_endpoint);
    QTRY_VERIFY2(b->state() == QOpcUaClient::Connected, "Could not connect to server");
    node.reset(b->node(readWriteNode));
    QCOMPARE(node->value().toDouble(), 42.0);
    QScopedPointer<QOpcUaClient> d(m_opcUa.createClient(m_backends[0]));
    d->connectToEndpoint(m_endpoint);
    QTRY_VERIFY2(d->state() == QOpcUaClient::Connected, "Could not connect to server");
    node.reset(d->node(readWriteNode));
    QCOMPARE(node->value().toDouble(), 42.0);
    d->disconnectFromEndpoint();
    QTRY_VERIFY2(d->state() == QOpcUaClient::Disconnected, "Could not disconnect from server");
    a->disconnectFromEndpoint();
    QTRY_VERIFY2(a->state() == QOpcUaClient::Disconnected, "Could not disconnect from server");
    b->disconnectFromEndpoint();
    QTRY_VERIFY2(b->state() == QOpcUaClient::Disconnected, "Could not disconnect from server");
}

void Tst_QOpcUaClient::nodeClass()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    // Root -> Types -> ReferenceTypes -> References
    QScopedPointer<QOpcUaNode> refNode(opcuaClient->node("ns=0;i=31"));
    QVERIFY(refNode != 0);
    QCOMPARE(refNode->nodeClass(), QOpcUaNode::NodeClass::ReferenceType);

    // Root -> Types -> DataTypes -> BaseDataTypes -> Boolean
    QScopedPointer<QOpcUaNode> dataTypeNode(opcuaClient->node("ns=0;i=1"));
    QVERIFY(dataTypeNode != 0);
    QCOMPARE(dataTypeNode->nodeClass(), QOpcUaNode::NodeClass::DataType);

    // Root -> Types -> DataTypes -> ObjectTypes -> BaseObjectTypes -> FolderType
    QScopedPointer<QOpcUaNode> objectTypeNode(opcuaClient->node("ns=0;i=61"));
    QVERIFY(objectTypeNode != 0);
    QCOMPARE(objectTypeNode->nodeClass(), QOpcUaNode::NodeClass::ObjectType);

    // Root -> Types -> DataTypes -> VariableTypes -> BaseVariableType -> PropertyType
    QScopedPointer<QOpcUaNode> variableTypeNode(opcuaClient->node("ns=0;i=68"));
    QVERIFY(variableTypeNode != 0);
    QCOMPARE(variableTypeNode->nodeClass(), QOpcUaNode::NodeClass::VariableType);

    // Root -> Objects
    QScopedPointer<QOpcUaNode> objectNode(opcuaClient->node("ns=0;i=85"));
    QVERIFY(objectNode != 0);
    QCOMPARE(objectNode->nodeClass(), QOpcUaNode::NodeClass::Object);

    // Root -> Objects -> Server -> NamespaceArray
    QScopedPointer<QOpcUaNode> variableNode(opcuaClient->node("ns=0;i=2255"));
    QVERIFY(variableNode != 0);
    QCOMPARE(variableNode->nodeClass(), QOpcUaNode::NodeClass::Variable);
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
    bool success = node->setValue(list, QOpcUa::Boolean);
    QVERIFY(success == true);

    list.clear();
    list.append(std::numeric_limits<quint8>::min());
    list.append(std::numeric_limits<quint8>::max());
    list.append(10);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Byte"));
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::Byte);
    QVERIFY(success == true);

    list.clear();
    list.append(QDateTime::currentDateTime());
    list.append(QDateTime::currentDateTime());
    list.append(QDateTime::currentDateTime());
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.DateTime"));
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::DateTime);
    QVERIFY(success == true);

    list.clear();
    list.append(23.5);
    list.append(23.6);
    list.append(23.7);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Double"));
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::Double);
    QVERIFY(success == true);

    list.clear();
    list.append(23.5);
    list.append(23.6);
    list.append(23.7);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Float"));
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::Float);
    QVERIFY(success == true);

    list.clear();
    list.append(std::numeric_limits<qint16>::min());
    list.append(std::numeric_limits<qint16>::max());
    list.append(10);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int16"));
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::Int16);
    QVERIFY(success == true);

    list.clear();
    list.append(std::numeric_limits<qint32>::min());
    list.append(std::numeric_limits<qint32>::max());
    list.append(10);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int32"));
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::Int32);
    QVERIFY(success == true);

    list.clear();
    list.append(std::numeric_limits<qint64>::min());
    list.append(std::numeric_limits<qint64>::max());
    list.append(10);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int64"));
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::Int64);
    QVERIFY(success == true);

    list.clear();
    list.append(std::numeric_limits<qint8>::min());
    list.append(std::numeric_limits<qint8>::max());
    list.append(10);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.SByte"));
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::SByte);
    QVERIFY(success == true);

    list.clear();
    list.append("Test1");
    list.append("Test2");
    list.append("Test3");
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.String"));
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::String);
    QVERIFY(success == true);

    list.clear();
    list.append(std::numeric_limits<quint16>::min());
    list.append(std::numeric_limits<quint16>::max());
    list.append(10);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.UInt16"));
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::UInt16);
    QVERIFY(success == true);

    list.clear();
    list.append(std::numeric_limits<quint32>::min());
    list.append(std::numeric_limits<quint32>::max());
    list.append(10);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.UInt32"));
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::UInt32);
    QVERIFY(success == true);

    list.clear();
    list.append(std::numeric_limits<quint64>::min());
    list.append(std::numeric_limits<quint64>::max());
    list.append(10);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.UInt64"));
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::UInt64);
    QVERIFY(success == true);

    list.clear();
    list.append("Localized Text 1");
    list.append("Localized Text 2");
    list.append("Localized Text 3");
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.LocalizedText"));
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::LocalizedText);
    QVERIFY(success == true);

    list.clear();
    list.append("abc");
    list.append("def");
    QByteArray withNull("gh");
    withNull.append('\0');
    withNull.append("i");
    list.append(withNull);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.ByteString"));
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::ByteString);
    QVERIFY(success == true);

    list.clear();
    list.append(QUuid("e0bd5ccd-f571-4545-9352-61a0f8cb9216"));
    list.append(QUuid("460ebe04-89d8-42f3-a0e0-7b45940f1a4e4"));
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Guid"));
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::Guid);
    QVERIFY(success == true);

    list.clear();
    list.append("ns=0;i=0");
    list.append("ns=0;i=1");
    list.append("ns=0;i=2");
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.NodeId"));
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::NodeId);
    QVERIFY(success == true);

    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("XmlElement support is not yet implemented in the freeopcua library");

    list.clear();
    list.append(xmlElements[0]);
    list.append(xmlElements[1]);
    list.append(xmlElements[2]);
    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Arrays.XmlElement"));
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::XmlElement);
    QVERIFY(success == true);
}

void Tst_QOpcUaClient::readArray()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> booleanArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Boolean"));
    QVERIFY(booleanArrayNode != 0);
    QVariant booleanArray = booleanArrayNode->value();
    QVERIFY(booleanArray.type() == QVariant::List);
    QVERIFY(booleanArray.toList().length() == 3);
    QCOMPARE(booleanArray.toList()[0].type(), QVariant::Bool);
    QCOMPARE(booleanArray.toList()[0].toBool(), true);
    QCOMPARE(booleanArray.toList()[1].toBool(), false);
    QCOMPARE(booleanArray.toList()[2].toBool(), true);

    QScopedPointer<QOpcUaNode> int32ArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int32"));
    QVERIFY(int32ArrayNode != 0);
    QVariant int32Array = int32ArrayNode->value();
    QVERIFY(int32Array.type() == QVariant::List);
    QVERIFY(int32Array.toList().length() == 3);
    QCOMPARE(int32Array.toList()[0].type(), QVariant::Int);
    QCOMPARE(int32Array.toList()[0].toInt(), std::numeric_limits<qint32>::min());
    QCOMPARE(int32Array.toList()[1].toInt(), std::numeric_limits<qint32>::max());
    QCOMPARE(int32Array.toList()[2].toInt(), 10);

    QScopedPointer<QOpcUaNode> uint32ArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.UInt32"));
    QVERIFY(uint32ArrayNode != 0);
    QVariant uint32Array = uint32ArrayNode->value();
    QVERIFY(uint32Array.type() == QVariant::List);
    QVERIFY(uint32Array.toList().length() == 3);
    QCOMPARE(uint32Array.toList()[0].type(), QVariant::UInt);
    QCOMPARE(uint32Array.toList()[0].toUInt(), std::numeric_limits<quint32>::min());
    QCOMPARE(uint32Array.toList()[1].toUInt(), std::numeric_limits<quint32>::max());
    QCOMPARE(uint32Array.toList()[2].toUInt(), quint32(10));

    QScopedPointer<QOpcUaNode> doubleArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Double"));
    QVERIFY(doubleArrayNode != 0);
    QVariant doubleArray = doubleArrayNode->value();
    QVERIFY(doubleArray.type() == QVariant::List);
    QVERIFY(doubleArray.toList().length() == 3);
    QCOMPARE(doubleArray.toList()[0].type(), QVariant::Double);
    QCOMPARE(doubleArray.toList()[0].toDouble(), double(23.5));
    QCOMPARE(doubleArray.toList()[1].toDouble(), double(23.6));
    QCOMPARE(doubleArray.toList()[2].toDouble(), double(23.7));

    QScopedPointer<QOpcUaNode> floatArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Float"));
    QVERIFY(floatArrayNode != 0);
    QVariant floatArray = floatArrayNode->value();
    QVERIFY(floatArray.type() == QVariant::List);
    QVERIFY(floatArray.toList().length() == 3);
    QVERIFY(floatArray.toList()[0].userType() == QMetaType::Float);
    QCOMPARE(floatArray.toList()[0].toFloat(), float(23.5));
    QCOMPARE(floatArray.toList()[1].toFloat(), float(23.6));
    QCOMPARE(floatArray.toList()[2].toFloat(), float(23.7));

    QScopedPointer<QOpcUaNode> stringArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.String"));
    QVERIFY(stringArrayNode != 0);
    QVariant stringArray = stringArrayNode->value();
    QVERIFY(stringArray.type() == QVariant::List);
    QVERIFY(stringArray.toList().length() == 3);
    QCOMPARE(stringArray.toList()[0].type(), QVariant::String);
    QCOMPARE(stringArray.toList()[0].toString(), QStringLiteral("Test1"));
    QCOMPARE(stringArray.toList()[1].toString(), QStringLiteral("Test2"));
    QCOMPARE(stringArray.toList()[2].toString(), QStringLiteral("Test3"));

    QScopedPointer<QOpcUaNode> dateTimeArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.DateTime"));
    QVERIFY(dateTimeArrayNode != 0);
    QVariant dateTimeArray = dateTimeArrayNode->value();
    QVERIFY(dateTimeArray.type() == QVariant::List);
    QVERIFY(dateTimeArray.toList().length() == 3);
    QCOMPARE(dateTimeArray.toList()[0].type(), QVariant::DateTime);

    QScopedPointer<QOpcUaNode> ltArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.LocalizedText"));
    QVERIFY(ltArrayNode != 0);
    QVariant ltArray = ltArrayNode->value();
    QVERIFY(ltArray.type() == QVariant::List);
    QVERIFY(ltArray.toList().length() == 3);
    QCOMPARE(ltArray.toList()[0].type(), QVariant::String);
    QCOMPARE(ltArray.toList()[0].toString(), QStringLiteral("Localized Text 1"));
    QCOMPARE(ltArray.toList()[1].toString(), QStringLiteral("Localized Text 2"));
    QCOMPARE(ltArray.toList()[2].toString(), QStringLiteral("Localized Text 3"));

    QScopedPointer<QOpcUaNode> uint16ArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.UInt16"));
    QVERIFY(uint16ArrayNode != 0);
    QVariant uint16Array = uint16ArrayNode->value();
    QVERIFY(uint16Array.type() == QVariant::List);
    QVERIFY(uint16Array.toList().length() == 3);
    QVERIFY(uint16Array.toList()[0].userType() == QMetaType::UShort);
    QVERIFY(uint16Array.toList()[0] == std::numeric_limits<quint16>::min());
    QVERIFY(uint16Array.toList()[1] == std::numeric_limits<quint16>::max());
    QVERIFY(uint16Array.toList()[2] == quint16(10));


    QScopedPointer<QOpcUaNode> int16ArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int16"));
    QVERIFY(int16ArrayNode != 0);
    QVariant int16Array = int16ArrayNode->value();
    QVERIFY(int16Array.type() == QVariant::List);
    QVERIFY(int16Array.toList().length() == 3);
    QVERIFY(int16Array.toList()[0].userType() == QMetaType::Short);
    QVERIFY(int16Array.toList()[0] == std::numeric_limits<qint16>::min());
    QVERIFY(int16Array.toList()[1] == std::numeric_limits<qint16>::max());
    QVERIFY(int16Array.toList()[2] == qint16(10));

    QScopedPointer<QOpcUaNode> uint64ArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.UInt64"));
    QVERIFY(uint64ArrayNode != 0);
    QVariant uint64Array = uint64ArrayNode->value();
    QCOMPARE(uint64Array.type(), QVariant::List);
    QCOMPARE(uint64Array.toList().length(), 3);
    QCOMPARE(uint64Array.toList()[0].type(), QVariant::ULongLong);
    QVERIFY(uint64Array.toList()[0] == std::numeric_limits<quint64>::min());
    QVERIFY(uint64Array.toList()[1] == std::numeric_limits<quint64>::max());
    QVERIFY(uint64Array.toList()[2] == quint64(10));

    QScopedPointer<QOpcUaNode> int64ArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int64"));
    QVERIFY(int64ArrayNode != 0);
    QVariant int64Array = int64ArrayNode->value();
    QVERIFY(int64Array.type() == QVariant::List && int64Array.toList().length() == 3);
    QCOMPARE(int64Array.toList()[0].type(), QVariant::LongLong);
    QVERIFY(int64Array.toList()[0] == std::numeric_limits<qint64>::min());
    QVERIFY(int64Array.toList()[1] == std::numeric_limits<qint64>::max());
    QVERIFY(int64Array.toList()[2] == qint64(10));

    QScopedPointer<QOpcUaNode> byteArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Byte"));
    QVERIFY(byteArrayNode != 0);
    QVariant byteArray = byteArrayNode->value();
    QVERIFY(byteArray.type() == QVariant::List);
    QVERIFY(byteArray.toList().length() == 3);
    QVERIFY(byteArray.toList()[0].userType() == QMetaType::UChar);
    QVERIFY(byteArray.toList()[0] == std::numeric_limits<quint8>::min());
    QVERIFY(byteArray.toList()[1] == std::numeric_limits<quint8>::max());
    QVERIFY(byteArray.toList()[2] == quint8(10));

    QScopedPointer<QOpcUaNode> byteStringArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.ByteString"));
    QVERIFY(byteStringArrayNode != 0);
    QVariant byteStringArray = byteStringArrayNode->value();
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
    QVariant guidArray = guidArrayNode->value();
    QVERIFY(guidArray.type() == QVariant::List);
    QVERIFY(guidArray.toList().length() == 2);
    QCOMPARE(guidArray.toList()[0], QUuid("e0bd5ccd-f571-4545-9352-61a0f8cb9216}"));
    QCOMPARE(guidArray.toList()[1], QUuid("460ebe04-89d8-42f3-a0e0-7b45940f1a4e4"));

    QScopedPointer<QOpcUaNode> sbyteArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.SByte"));
    QVERIFY(sbyteArrayNode != 0);
    QVariant sbyteArray = sbyteArrayNode->value();
    QVERIFY(sbyteArray.type() == QVariant::List);
    QVERIFY(sbyteArray.toList().length() == 3);
    QVERIFY(sbyteArray.toList()[0].userType() == QMetaType::SChar);
    QVERIFY(sbyteArray.toList()[0] == std::numeric_limits<qint8>::min());
    QVERIFY(sbyteArray.toList()[1] == std::numeric_limits<qint8>::max());
    QVERIFY(sbyteArray.toList()[2] == qint8(10));

    QScopedPointer<QOpcUaNode> nodeIdArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.NodeId"));
    QVERIFY(nodeIdArrayNode != 0);
    QVariant nodeIdArray = nodeIdArrayNode->value();
    QVERIFY(nodeIdArray.type() == QVariant::List);
    QVERIFY(nodeIdArray.toList().length() == 3);
    QCOMPARE(nodeIdArray.toList()[0].type(), QVariant::String);
    QCOMPARE(nodeIdArray.toList()[0].toString(), QStringLiteral("ns=0;i=0"));
    QCOMPARE(nodeIdArray.toList()[1].toString(), QStringLiteral("ns=0;i=1"));
    QCOMPARE(nodeIdArray.toList()[2].toString(), QStringLiteral("ns=0;i=2"));

    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("XmlElement support is not yet implemented in the freeopcua backend");

    QScopedPointer<QOpcUaNode> xmlElementArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.XmlElement"));
    QVERIFY(nodeIdArrayNode != 0);
    QVariant xmlElementArray = xmlElementArrayNode->value();
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
    bool success;

    QScopedPointer<QOpcUaNode> booleanNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Boolean"));
    QVERIFY(booleanNode != 0);
    success = booleanNode->setValue(true, QOpcUa::Boolean);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> int32Node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Int32"));
    QVERIFY(int32Node != 0);
    success = int32Node->setValue(std::numeric_limits<qint32>::min(), QOpcUa::Int32);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> uint32Node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.UInt32"));
    QVERIFY(uint32Node != 0);
    success = uint32Node->setValue(std::numeric_limits<quint32>::max(), QOpcUa::UInt32);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> doubleNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Double"));
    QVERIFY(doubleNode != 0);
    success = doubleNode->setValue(42, QOpcUa::Double);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> floatNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Float"));
    QVERIFY(floatNode != 0);
    success = floatNode->setValue(42, QOpcUa::Float);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> stringNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.String"));
    QVERIFY(stringNode != 0);
    success = stringNode->setValue("QOpcUa Teststring", QOpcUa::String);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> dtNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.DateTime"));
    QVERIFY(dtNode != 0);
    success = dtNode->setValue(QDateTime::currentDateTime(), QOpcUa::DateTime);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> uint16Node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.UInt16"));
    QVERIFY(uint16Node != 0);
    success = uint16Node->setValue(std::numeric_limits<quint16>::max(), QOpcUa::UInt16);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> int16Node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Int16"));
    QVERIFY(int16Node != 0);
    success = int16Node->setValue(std::numeric_limits<qint16>::min(), QOpcUa::Int16);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> uint64Node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.UInt64"));
    QVERIFY(uint64Node != 0);
    success = uint64Node->setValue(std::numeric_limits<quint64>::max(), QOpcUa::UInt64);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> int64Node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Int64"));
    QVERIFY(int64Node != 0);
    success = int64Node->setValue(std::numeric_limits<qint64>::min(), QOpcUa::Int64);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> byteNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Byte"));
    QVERIFY(byteNode != 0);
    success = byteNode->setValue(std::numeric_limits<quint8>::max(), QOpcUa::Byte);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> sbyteNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.SByte"));
    QVERIFY(sbyteNode != 0);
    success = sbyteNode->setValue(std::numeric_limits<qint8>::min(), QOpcUa::SByte);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> ltNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.LocalizedText"));
    QVERIFY(ltNode != 0);
    success = ltNode->setValue(QStringLiteral("QOpcUaClient Localized Text"), QOpcUa::LocalizedText);
    QVERIFY(success == true);


    QByteArray withNull("gh");
    withNull.append('\0');
    withNull.append("i");

    QVariant data = withNull;

    QScopedPointer<QOpcUaNode> byteStringNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.ByteString"));
    QVERIFY(byteStringNode != 0);
    success = byteStringNode->setValue(data, QOpcUa::ByteString);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> guidNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Guid"));
    QVERIFY(guidNode != 0);
    success = guidNode->setValue(QUuid("e0bd5ccd-f571-4545-9352-61a0f8cb9216"), QOpcUa::Guid);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> nodeIdNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.NodeId"));
    QVERIFY(nodeIdNode != 0);
    success = nodeIdNode->setValue("ns=42;s=Test", QOpcUa::NodeId);
    QVERIFY(success == true);

    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("XmlElement support is not yet implemented in the freeopcua backend");

    QScopedPointer<QOpcUaNode> xmlElementNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.XmlElement"));
    QVERIFY(xmlElementNode != 0);
    success = xmlElementNode->setValue(xmlElements[0], QOpcUa::XmlElement);
    QVERIFY(success == true);
}

void Tst_QOpcUaClient::readScalar()
{
    QFETCH(QOpcUaClient *, opcuaClient);
    OpcuaConnector connector(opcuaClient, m_endpoint);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Boolean"));
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QOpcUa::Types::Boolean);
    QVariant booleanScalar = node->value();
    QVERIFY(booleanScalar.isValid());
    QCOMPARE(booleanScalar.type(), QVariant::Bool);
    QCOMPARE(booleanScalar.toBool(), true);

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Int32"));
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QOpcUa::Types::Int32);
    QVariant int32Scalar = node->value();

    QVERIFY(int32Scalar.isValid());
    QCOMPARE(int32Scalar.type(), QVariant::Int);
    QCOMPARE(int32Scalar.toInt(), std::numeric_limits<qint32>::min());

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.UInt32"));
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QOpcUa::Types::UInt32);
    QVariant uint32Scalar = node->value();
    QVERIFY(uint32Scalar.isValid());
    QCOMPARE(uint32Scalar.type(), QVariant::UInt);
    QCOMPARE(uint32Scalar.toUInt(), std::numeric_limits<quint32>::max());

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Double"));
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QOpcUa::Types::Double);
    QVariant doubleScalar = node->value();
    QVERIFY(doubleScalar.isValid());
    QCOMPARE(doubleScalar.type(), QVariant::Double);
    QCOMPARE(doubleScalar.toDouble(), double(42));

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Float"));
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QOpcUa::Types::Float);
    QVariant floatScalar = node->value();
    QVERIFY(floatScalar.isValid());
    QVERIFY(floatScalar.userType() == QMetaType::Float);
    QCOMPARE(floatScalar.toFloat(), float(42));

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.String"));
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QOpcUa::Types::String);
    QVariant stringScalar = node->value();
    QVERIFY(stringScalar.isValid());
    QCOMPARE(stringScalar.type(), QVariant::String);
    QCOMPARE(stringScalar.toString(), QStringLiteral("QOpcUa Teststring"));

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.DateTime"));
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QOpcUa::Types::DateTime);
    QVariant dateTimeScalar = node->value();
    QCOMPARE(dateTimeScalar.type(), QVariant::DateTime);
    QVERIFY(dateTimeScalar.isValid());

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.LocalizedText"));
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QOpcUa::Types::LocalizedText);
    QVariant ltScalar = node->value();
    QVERIFY(ltScalar.isValid());
    QCOMPARE(ltScalar.type(), QVariant::String);
    QCOMPARE(ltScalar.toString(), QStringLiteral("QOpcUaClient Localized Text"));

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.UInt16"));
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QOpcUa::Types::UInt16);
    QVariant uint16Scalar = node->value();
    QVERIFY(uint16Scalar.isValid());
    QVERIFY(uint16Scalar.userType() == QMetaType::UShort);
    QVERIFY(uint16Scalar == std::numeric_limits<quint16>::max());

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Int16"));
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QOpcUa::Types::Int16);
    QVariant int16Scalar = node->value();
    QVERIFY(int16Scalar.isValid());
    QVERIFY(int16Scalar.userType() == QMetaType::Short);
    QVERIFY(int16Scalar == std::numeric_limits<qint16>::min());

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.UInt64"));
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QOpcUa::Types::UInt64);
    QVariant uint64Scalar = node->value();
    QVERIFY(uint64Scalar.isValid());
    QCOMPARE(uint64Scalar.type(), QVariant::ULongLong);
    QVERIFY(uint64Scalar == std::numeric_limits<quint64>::max());

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Int64"));
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QOpcUa::Types::Int64);
    QVariant int64Scalar = node->value();
    QVERIFY(int64Scalar.isValid());
    QCOMPARE(int64Scalar.type(), QVariant::LongLong);
    QVERIFY(int64Scalar == std::numeric_limits<qint64>::min());

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Byte"));
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QOpcUa::Types::Byte);
    QVariant byteScalar = node->value();
    QVERIFY(byteScalar.isValid());
    QVERIFY(byteScalar.userType() == QMetaType::UChar);
    QVERIFY(byteScalar == std::numeric_limits<quint8>::max());

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.SByte"));
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QOpcUa::Types::SByte);
    QVariant sbyteScalar = node->value();
    QVERIFY(sbyteScalar.isValid());
    QVERIFY(sbyteScalar.userType() == QMetaType::SChar);
    QVERIFY(sbyteScalar == std::numeric_limits<qint8>::min());

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.ByteString"));
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QOpcUa::Types::ByteString);
    QVariant byteStringScalar = node->value();
    QVERIFY(byteStringScalar.isValid());
    QVERIFY(byteStringScalar.userType() == QMetaType::QByteArray);
    QByteArray withNull("gh");
    withNull.append('\0');
    withNull.append("i");
    QVERIFY(byteStringScalar == withNull);

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Guid"));
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QOpcUa::Types::Guid);
    QVariant guidScalar = node->value();
    QVERIFY(guidScalar.isValid());
    QVERIFY(guidScalar.userType() == QMetaType::QUuid);
    QCOMPARE(guidScalar, QUuid("e0bd5ccd-f571-4545-9352-61a0f8cb9216"));

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.NodeId"));
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QOpcUa::Types::NodeId);
    QVariant nodeIdScalar = node->value();
    QVERIFY(nodeIdScalar.isValid());
    QCOMPARE(nodeIdScalar.type(), QVariant::String);
    QCOMPARE(nodeIdScalar.toString(), QStringLiteral("ns=42;s=Test"));

    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("XmlElement support is not yet implemented in the freeopcua backend");

    node.reset(opcuaClient->node("ns=2;s=Demo.Static.Scalar.XmlElement"));
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QOpcUa::Types::XmlElement);
    QVariant xmlElementScalar = node->value();
    QVERIFY(xmlElementScalar.isValid());
    QCOMPARE(xmlElementScalar.type(), QVariant::String);
    QCOMPARE(xmlElementScalar.toString(), xmlElements[0]);
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

    bool success = stringScalarNode->setValue(testString, QOpcUa::String);
    QVERIFY(success == true);
    success = localizedScalarNode->setValue(testString, QOpcUa::LocalizedText);
    QVERIFY(success == true);

    QVariantList l;
    l.append(testString);
    l.append(testString);

    success = stringArrayNode->setValue(l, QOpcUa::String);
    QVERIFY(success == true);
    success = localizedArrayNode->setValue(l, QOpcUa::LocalizedText);
    QVERIFY(success == true);

    QVariant result = stringScalarNode->value();
    QVERIFY(result.toString() == testString);
    result = localizedScalarNode->value();
    QVERIFY(result.toString() == testString);

    result = stringArrayNode->value();
    QVERIFY(result.type() == QVariant::List);
    QVERIFY(result.toList().length() == 2);
    QVERIFY(result.toList()[0].type() == QVariant::String);
    QVERIFY(result.toList()[0].toString() == testString);
    QVERIFY(result.toList()[1].type() == QVariant::String);
    QVERIFY(result.toList()[1].toString() == testString);

    result = localizedArrayNode->value();
    QVERIFY(result.type() == QVariant::List);
    QVERIFY(result.toList().length() == 2);
    QVERIFY(result.toList()[0].type() == QVariant::String);
    QVERIFY(result.toList()[0].toString() == testString);
    QVERIFY(result.toList()[1].type() == QVariant::String);
    QVERIFY(result.toList()[1].toString() == testString);
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


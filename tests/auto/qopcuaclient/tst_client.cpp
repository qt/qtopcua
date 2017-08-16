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

const QString readWriteNode = QStringLiteral("ns=3;s=TestNode.ReadWrite");
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
    defineDataMethod(connect_data)
    void connect();

    defineDataMethod(getRootNode_data)
    void getRootNode();
    defineDataMethod(getChildren_data)
    void getChildren();

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
    defineDataMethod(invalidNodeAccess_data)
    void invalidNodeAccess();

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
    defineDataMethod(disconnect_data)
    void disconnect();

public slots:
    void processConnected();
    void processDisconnected();
    void processSubscription(QVariant val);
    void processEventSubscription(QVector<QVariant> val);

private:
    QString envOrDefault(const char *env, QString def)
    {
        return qEnvironmentVariableIsSet(env) ? qgetenv(env).constData() : def;
    }

    QString m_endpoint;
    bool m_connected;
    double m_value;
    bool m_event;
    QOpcUaProvider m_opcUa;
    QStringList m_backends;
    QVector<QOpcUaClient *> m_clients;
    QProcess m_serverProcess;
};

Tst_QOpcUaClient::Tst_QOpcUaClient()
    : m_connected(false)
{
    m_backends = QOpcUaProvider::availableBackends();
}

void Tst_QOpcUaClient::initTestCase()
{
    for (const auto backend: m_backends) {
        QOpcUaClient *client = m_opcUa.createClient(backend);
        client->setParent(this);
        QVERIFY2(client != nullptr,
                 QString("Loading backend failed: %1").arg(backend).toLatin1().data());
        qDebug() << "Using SDK plugin:" << client->backend();
        m_clients.append(client);
    }

    if (qEnvironmentVariableIsEmpty("OPCUA_HOST") && qEnvironmentVariableIsEmpty("OPCUA_PORT")) {
        const QString testServerPath = QDir::currentPath()
                                     + QLatin1String("/../../freeopcua-testserver/freeopcua-testserver")
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
    QFETCH(QOpcUaClient*, opcuaClient);
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
    QVERIFY( url == QUrl("opc.tcp:127.0.0.1:1234"));
}

void Tst_QOpcUaClient::secureConnect()
{
    QFETCH(QOpcUaClient*, opcuaClient);
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
    QFETCH(QOpcUaClient*, opcuaClient);

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
    QVERIFY( url == QUrl("opc.tcp:127.0.0.1:1234"));
}

void Tst_QOpcUaClient::connect()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    QVERIFY(opcuaClient != 0);
    QVERIFY(m_connected == false);

    QObject::connect(opcuaClient, &QOpcUaClient::connected, this, &Tst_QOpcUaClient::processConnected);
    QObject::connect(opcuaClient, &QOpcUaClient::disconnected, this, &Tst_QOpcUaClient::processDisconnected);

    opcuaClient->connectToEndpoint(QUrl(m_endpoint));
    QVERIFY(opcuaClient->state() == QOpcUaClient::Connecting);
    QTRY_VERIFY2(opcuaClient->state() == QOpcUaClient::Connected, "Could not connect to server");

    QVERIFY(opcuaClient->url() == QUrl(m_endpoint));

    QTRY_VERIFY_WITH_TIMEOUT(m_connected == true, 1250); // Wait 1.25 seconds for state change
}

void Tst_QOpcUaClient::getRootNode()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    QScopedPointer<QOpcUaNode> root(opcuaClient->node("ns=0;i=84"));
    QVERIFY(root != 0);
    QVERIFY(root->name() == QLatin1String("Root"));

    QString nodeId = root->nodeId();
    QCOMPARE(nodeId, QStringLiteral("ns=0;i=84"));
}

void Tst_QOpcUaClient::getChildren()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=1;s=Large.Folder"));
    QVERIFY(node != 0);
    QVERIFY(node->name() == QLatin1String("Large_Folder"));
    QVERIFY(node->childIds().size() == 1001);
}

void Tst_QOpcUaClient::read()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(readWriteNode));
    QVERIFY(node != 0);
    for (int i = 0; i < numberOfOperations; i++)
        QVERIFY(node->value().type() == QVariant::Double);
}

void Tst_QOpcUaClient::readWrite()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(readWriteNode));
    QVERIFY(node != 0);
    for (int i = 0; i < numberOfOperations; i++) {
        bool result = node->setValue(QVariant((double) i));
        QVERIFY(result==true);
        QVERIFY(node->value().toInt() ==  i);
    }
}

void Tst_QOpcUaClient::dataChangeSubscription()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    m_value = 0;
    QScopedPointer<QOpcUaNode> node(opcuaClient->node(readWriteNode));
    QVERIFY(node != 0);
    bool result = node->setValue(QVariant((double ) 0));
    QVERIFY(result == true);

    QScopedPointer<QOpcUaSubscription> subscription(opcuaClient->createSubscription(100));
    QScopedPointer<QOpcUaMonitoredValue> monitoredValue(subscription->addValue(node.data()));
    QVERIFY(monitoredValue != nullptr);
    if (!monitoredValue)
        QFAIL("can not monitor value");
    QObject::connect(monitoredValue.data(), &QOpcUaMonitoredValue::valueChanged,
                     this, &Tst_QOpcUaClient::processSubscription);

    result = node->setValue(QVariant((double ) 42));
    QVERIFY(result == true);
    int i = 0;
    while (m_value == 0 && i++ < 50) {
        QTest::qWait(25); // wait n periods for data change
    }
    QVERIFY(m_value == (double) 42);
}

void Tst_QOpcUaClient::dataChangeSubscriptionInvalidNode()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    QScopedPointer<QOpcUaNode> noDataNode(opcuaClient->node("ns=0;i=84"));
    QVERIFY(noDataNode != 0);
    QScopedPointer<QOpcUaSubscription> subscription(opcuaClient->createSubscription(100));
    QOpcUaMonitoredValue *result = subscription->addValue(noDataNode.data());
    QVERIFY(result == 0);
}

void Tst_QOpcUaClient::methodCall()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    QSKIP("Method calls are not implemented in freeopcua-based testserver");
    QVector<QOpcUa::TypedVariant> args;
    QVector<QVariant> ret;
    for (int i = 0; i < 2; i++)
        args.push_back(QOpcUa::TypedVariant((double ) 4, QOpcUa::Double));

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
    QFETCH(QOpcUaClient*, opcuaClient);

    QSKIP("Does not reliably work with the testserver");
    if (opcuaClient->backend() == QLatin1String("freeopcua")) {
        QSKIP("Event subscriptions do not yet work with the freeopcua backend");
    }

    m_event = false;
    QScopedPointer<QOpcUaNode> triggerNode(opcuaClient->node("ns=3;s=TriggerNode"));
    QVERIFY(triggerNode != 0);

    QScopedPointer<QOpcUaSubscription> subscription(opcuaClient->createSubscription(100));
    QOpcUaMonitoredEvent *monitoredEvent = subscription->addEvent(triggerNode.data());
    QVERIFY(monitoredEvent != 0);
    QObject::connect(monitoredEvent, &QOpcUaMonitoredEvent::newEvent, this,
                     &Tst_QOpcUaClient::processEventSubscription);

    if (!monitoredEvent)
        QFAIL("can not monitor event");

    QScopedPointer<QOpcUaNode> triggerVariable(opcuaClient->node("ns=3;s=TriggerVariable"));
    QVERIFY(triggerVariable != 0);
    bool result = triggerVariable->setValue(QVariant((double) 0));
    QVERIFY(result == true);
    result = triggerVariable->setValue(QVariant((double) 1));
    QVERIFY(result == true);

    int i = 0;
    while (m_event == false && i++ < 50) {
        QTest::qWait(25);
    }
    QVERIFY(m_event == true);
    delete monitoredEvent;
}

void Tst_QOpcUaClient::eventSubscribeInvalidNode()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    QScopedPointer<QOpcUaNode> noEventNode(opcuaClient->node(readWriteNode));
    QVERIFY(noEventNode != 0);
    QScopedPointer<QOpcUaSubscription> subscription(opcuaClient->createSubscription(100));
    QOpcUaMonitoredEvent *monitoredEvent = subscription->addEvent(noEventNode.data());
    QVERIFY(monitoredEvent == 0);
}

void Tst_QOpcUaClient::readRange()
{
    QFETCH(QOpcUaClient*, opcuaClient);

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
    QFETCH(QOpcUaClient*, opcuaClient);

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
    QFETCH(QOpcUaClient*, opcuaClient);

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
    QFETCH(QOpcUaClient*, opcuaClient);

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

void Tst_QOpcUaClient::invalidNodeAccess()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    QScopedPointer<QOpcUaNode> invalidNode(opcuaClient->node("ns=0;s=IDoNotExist"));
    QVERIFY(invalidNode == 0);
}

void Tst_QOpcUaClient::multipleClients()
{
    QScopedPointer<QOpcUaClient> a(m_opcUa.createClient(m_backends[0]));
    a->connectToEndpoint(m_endpoint);
    QTRY_VERIFY2(a->state() == QOpcUaClient::Connected, "Could not connect to server");
    double value = a->node(readWriteNode)->value().toDouble();
    QVERIFY(value == 42.0);
    qDebug() << "a: " << value;
    QScopedPointer<QOpcUaClient> b(m_opcUa.createClient(m_backends[0]));
    b->connectToEndpoint(m_endpoint);
    QTRY_VERIFY2(b->state() == QOpcUaClient::Connected, "Could not connect to server");
    value = b->node(readWriteNode)->value().toDouble();
    QVERIFY(value == 42.0);
    qDebug() << "b: " << value;
    QScopedPointer<QOpcUaClient> d(m_opcUa.createClient(m_backends[0]));
    d->connectToEndpoint(m_endpoint);
    QTRY_VERIFY2(d->state() == QOpcUaClient::Connected, "Could not connect to server");
    value = d->node(readWriteNode)->value().toDouble();
    QVERIFY(value == 42.0);
    qDebug() << "d: " << value;
    d->disconnectFromEndpoint();
    QTRY_VERIFY2(d->state() == QOpcUaClient::Disconnected, "Could not disconnect from server");
    a->disconnectFromEndpoint();
    QTRY_VERIFY2(a->state() == QOpcUaClient::Disconnected, "Could not disconnect from server");
    b->disconnectFromEndpoint();
    QTRY_VERIFY2(b->state() == QOpcUaClient::Disconnected, "Could not disconnect from server");
}

void Tst_QOpcUaClient::nodeClass()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    // Root -> Types -> ReferenceTypes -> References
    QScopedPointer<QOpcUaNode> refNode(opcuaClient->node("ns=0;i=31"));
    QVERIFY(refNode != 0);
    QCOMPARE(refNode->nodeClass(), QStringLiteral("ReferenceType"));
}

void Tst_QOpcUaClient::writeArray()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    QVariantList list;

    list.append(true);
    list.append(false);
    list.append(true);
    QOpcUaNode* node = opcuaClient->node("ns=2;s=Demo.Static.Arrays.Boolean");
    QVERIFY(node != 0);
    bool success = node->setValue(list, QOpcUa::Boolean);
    QVERIFY(success == true);

    list.clear();
    list.append(11);
    list.append(12);
    list.append(13);
    node = opcuaClient->node("ns=2;s=Demo.Static.Arrays.Byte");
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::Byte);
    QVERIFY(success == true);

    list.clear();
    list.append(QDateTime::currentDateTime());
    list.append(QDateTime::currentDateTime());
    list.append(QDateTime::currentDateTime());
    node = opcuaClient->node("ns=2;s=Demo.Static.Arrays.DateTime");
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::DateTime);
    QVERIFY(success == true);

    list.clear();
    list.append(23.5);
    list.append(23.6);
    list.append(23.7);
    node = opcuaClient->node("ns=2;s=Demo.Static.Arrays.Double");
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::Double);
    QVERIFY(success == true);

    list.clear();
    list.append(23.5);
    list.append(23.6);
    list.append(23.7);
    node = opcuaClient->node("ns=2;s=Demo.Static.Arrays.Float");
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::Float);
    QVERIFY(success == true);

    list.clear();
    list.append(-11);
    list.append(-12);
    list.append(-13);
    node = opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int16");
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::Int16);
    QVERIFY(success == true);

    list.clear();
    list.append(-11);
    list.append(-12);
    list.append(-13);
    node = opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int32");
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::Int32);
    QVERIFY(success == true);

    list.clear();
    list.append(-11);
    list.append(-12);
    list.append(-13);
    node = opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int64");
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::Int64);
    QVERIFY(success == true);

    list.clear();
    list.append(-11);
    list.append(-12);
    list.append(-13);
    node = opcuaClient->node("ns=2;s=Demo.Static.Arrays.SByte");
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::SByte);
    QVERIFY(success == true);

    list.clear();
    list.append("Test1");
    list.append("Test2");
    list.append("Test3");
    node = opcuaClient->node("ns=2;s=Demo.Static.Arrays.String");
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::String);
    QVERIFY(success == true);

    list.clear();
    list.append(11);
    list.append(12);
    list.append(13);
    node = opcuaClient->node("ns=2;s=Demo.Static.Arrays.UInt16");
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::UInt16);
    QVERIFY(success == true);

    list.clear();
    list.append(11);
    list.append(12);
    list.append(13);
    node = opcuaClient->node("ns=2;s=Demo.Static.Arrays.UInt32");
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::UInt32);
    QVERIFY(success == true);

    list.clear();
    list.append(11);
    list.append(12);
    list.append(13);
    node = opcuaClient->node("ns=2;s=Demo.Static.Arrays.UInt64");
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::UInt64);
    QVERIFY(success == true);

    list.clear();
    list.append("Localized Text 1");
    list.append("Localized Text 2");
    list.append("Localized Text 3");
    node = opcuaClient->node("ns=2;s=Demo.Static.Arrays.LocalizedText");
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::LocalizedText);
    QVERIFY(success == true);

    list.clear();
    list.append("abc");
    list.append("def");
    list.append("ghi");
    node = opcuaClient->node("ns=2;s=Demo.Static.Arrays.ByteString");
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::ByteString);
    QVERIFY(success == true);

    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("NodeId parsing is not yet implemented in the freeopcua backend");

    list.clear();
    list.append("ns=0;i=0");
    list.append("ns=0;i=1");
    list.append("ns=0;i=2");
    node = opcuaClient->node("ns=2;s=Demo.Static.Arrays.NodeId");
    QVERIFY(node != 0);
    success = node->setValue(list, QOpcUa::NodeId);
    QVERIFY(success == true);

    QSKIP("XmlElement not available with freeopcua-based test server");
    // TODO add Static.Arrays.XmlElement test
}

void Tst_QOpcUaClient::readArray()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    QScopedPointer<QOpcUaNode> booleanArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Boolean"));
    QVERIFY(booleanArrayNode != 0);
    QVariant booleanArray = booleanArrayNode->value();
    QVERIFY(booleanArray.type() == QVariant::List && booleanArray.toList().length() == 3);
    QCOMPARE(booleanArray.toList()[0].type(), QVariant::Bool);
    QCOMPARE(booleanArray.toList()[0].toBool(), true);
    QCOMPARE(booleanArray.toList()[1].toBool(), false);
    QCOMPARE(booleanArray.toList()[2].toBool(), true);
    qDebug() << booleanArray.toList();

    QScopedPointer<QOpcUaNode> int32ArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int32"));
    QVERIFY(int32ArrayNode != 0);
    QVariant int32Array = int32ArrayNode->value();
    QVERIFY(int32Array.type() == QVariant::List && int32Array.toList().length() == 3);
    QCOMPARE(int32Array.toList()[0].type(), QVariant::Int);
    QCOMPARE(int32Array.toList()[0].toInt(), -11);
    QCOMPARE(int32Array.toList()[1].toInt(), -12);
    QCOMPARE(int32Array.toList()[2].toInt(), -13);
    qDebug() << int32Array.toList();


    QScopedPointer<QOpcUaNode> uint32ArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.UInt32"));
    QVERIFY(uint32ArrayNode != 0);
    QVariant uint32Array = uint32ArrayNode->value();
    QVERIFY(uint32Array.type() == QVariant::List && uint32Array.toList().length() == 3);
    QCOMPARE(uint32Array.toList()[0].type(), QVariant::UInt);
    QCOMPARE(uint32Array.toList()[0].toUInt(), (uint)11);
    QCOMPARE(uint32Array.toList()[1].toUInt(), (uint)12);
    QCOMPARE(uint32Array.toList()[2].toUInt(), (uint)13);
    qDebug() << uint32Array.toList();

    QScopedPointer<QOpcUaNode> doubleArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Double"));
    QVERIFY(doubleArrayNode != 0);
    QVariant doubleArray = doubleArrayNode->value();
    QVERIFY(doubleArray.type() == QVariant::List && doubleArray.toList().length() == 3);
    QCOMPARE(doubleArray.toList()[0].type(), QVariant::Double);
    QCOMPARE(doubleArray.toList()[0].toDouble(), (double)23.5);
    QCOMPARE(doubleArray.toList()[1].toDouble(), (double)23.6);
    QCOMPARE(doubleArray.toList()[2].toDouble(), (double)23.7);
    qDebug() << doubleArray.toList();

    QScopedPointer<QOpcUaNode> floatArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Float"));
    QVERIFY(floatArrayNode != 0);
    QVariant floatArray = floatArrayNode->value();
    QVERIFY(floatArray.type() == QVariant::List && floatArray.toList().length() == 3);
    QVERIFY(floatArray.toList()[0].userType() == QMetaType::Float);
    QCOMPARE(floatArray.toList()[0].toFloat(), (float)23.5);
    QCOMPARE(floatArray.toList()[1].toFloat(), (float)23.6);
    QCOMPARE(floatArray.toList()[2].toFloat(), (float)23.7);
    qDebug() << floatArray.toList();

    QScopedPointer<QOpcUaNode> stringArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.String"));
    QVERIFY(stringArrayNode != 0);
    QVariant stringArray = stringArrayNode->value();
    QVERIFY(stringArray.type() == QVariant::List && stringArray.toList().length() == 3);
    QCOMPARE(stringArray.toList()[0].type(), QVariant::String);
    QCOMPARE(stringArray.toList()[0].toString(), QStringLiteral("Test1"));
    QCOMPARE(stringArray.toList()[1].toString(), QStringLiteral("Test2"));
    QCOMPARE(stringArray.toList()[2].toString(), QStringLiteral("Test3"));
    qDebug() << stringArray.toList();

    QScopedPointer<QOpcUaNode> dateTimeArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.DateTime"));
    QVERIFY(dateTimeArrayNode != 0);
    QVariant dateTimeArray = dateTimeArrayNode->value();
    QVERIFY(dateTimeArray.type() == QVariant::List && dateTimeArray.toList().length() == 3);
    QCOMPARE(dateTimeArray.toList()[0].type(), QVariant::DateTime);
    qDebug() << dateTimeArray.toList();

    QScopedPointer<QOpcUaNode> ltArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.LocalizedText"));
    QVERIFY(ltArrayNode != 0);
    QVariant ltArray = ltArrayNode->value();
    QVERIFY(ltArray.type() == QVariant::List && ltArray.toList().length() == 3);
    QCOMPARE(ltArray.toList()[0].type(), QVariant::String);
    QCOMPARE(ltArray.toList()[0].toString(), QStringLiteral("Localized Text 1"));
    QCOMPARE(ltArray.toList()[1].toString(), QStringLiteral("Localized Text 2"));
    QCOMPARE(ltArray.toList()[2].toString(), QStringLiteral("Localized Text 3"));
    qDebug() << ltArray.toList();

    QScopedPointer<QOpcUaNode> uint16ArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.UInt16"));
    QVERIFY(uint16ArrayNode != 0);
    QVariant uint16Array = uint16ArrayNode->value();
    QVERIFY(uint16Array.type() == QVariant::List && uint16Array.toList().length() == 3);
    QVERIFY(uint16Array.toList()[0].userType() == QMetaType::UShort);
    QVERIFY(uint16Array.toList()[0] == (ushort)11);
    QVERIFY(uint16Array.toList()[1] == (ushort)12);
    QVERIFY(uint16Array.toList()[2] == (ushort)13);
    qDebug() << uint16Array.toList();

    QScopedPointer<QOpcUaNode> int16ArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int16"));
    QVERIFY(int16ArrayNode != 0);
    QVariant int16Array = int16ArrayNode->value();
    QVERIFY(int16Array.type() == QVariant::List && int16Array.toList().length() == 3);
    QVERIFY(int16Array.toList()[0].userType() == QMetaType::Short);
    QVERIFY(int16Array.toList()[0] == (short)-11);
    QVERIFY(int16Array.toList()[1] == (short)-12);
    QVERIFY(int16Array.toList()[2] == (short)-13);
    qDebug() << int16Array.toList();

    if (opcuaClient->backend() == QLatin1String("freeopcua")) {
        QWARN("*Int64 types are broken with the freeopcua backend");
    } else {
        QSKIP("Int64 types broken with freeopcua-based test server");
        QScopedPointer<QOpcUaNode> uint64ArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.UInt64"));
        QVERIFY(uint64ArrayNode != 0);
        QVariant uint64Array = uint64ArrayNode->value();
        QCOMPARE(uint64Array.type(), QVariant::List);
        QCOMPARE(uint64Array.toList().length(), 3);
        QCOMPARE(uint64Array.toList()[0].type(), QVariant::ULongLong);
        QVERIFY(uint64Array.toList()[0] == (unsigned long long)11);
        QVERIFY(uint64Array.toList()[1] == (unsigned long long)12);
        QVERIFY(uint64Array.toList()[2] == (unsigned long long)13);
        qDebug() << uint64Array.toList();

        QScopedPointer<QOpcUaNode> int64ArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Int64"));
        QVERIFY(int64ArrayNode != 0);
        QVariant int64Array = int64ArrayNode->value();
        QVERIFY(int64Array.type() == QVariant::List && int64Array.toList().length() == 3);
        QCOMPARE(int64Array.toList()[0].type(), QVariant::LongLong);
        QVERIFY(int64Array.toList()[0] == (long long)-11);
        QVERIFY(int64Array.toList()[1] == (long long)-12);
        QVERIFY(int64Array.toList()[2] == (long long)-13);
        qDebug() << int64Array.toList();
    }

    QScopedPointer<QOpcUaNode> byteArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.Byte"));
    QVERIFY(byteArrayNode != 0);
    QVariant byteArray = byteArrayNode->value();
    QVERIFY(byteArray.type() == QVariant::List && byteArray.toList().length() == 3);
    QVERIFY(byteArray.toList()[0].userType() == QMetaType::UChar);
    QVERIFY(byteArray.toList()[0] == (char)11);
    QVERIFY(byteArray.toList()[1] == (char)12);
    QVERIFY(byteArray.toList()[2] == (char)13);
    qDebug() << byteArray.toList();

    QScopedPointer<QOpcUaNode> byteStringArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.ByteString"));
    QVERIFY(byteStringArrayNode != 0);
    QVariant byteStringArray = byteStringArrayNode->value();
    QVERIFY(byteStringArray.type() == QVariant::List && byteStringArray.toList().length() == 3);
    QVERIFY(byteStringArray.toList()[0].userType() == QMetaType::QByteArray);
    QVERIFY(byteStringArray.toList()[0] == "abc");
    QVERIFY(byteStringArray.toList()[1] == "def");
    QVERIFY(byteStringArray.toList()[2] == "ghi");
    qDebug() << byteStringArray.toList();

    if (opcuaClient->backend() == QLatin1String("freeopcua")) {
        QWARN("SByte is broken with the freeopcua backend");
    } else {
        QScopedPointer<QOpcUaNode> sbyteArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.SByte"));
        QVERIFY(sbyteArrayNode != 0);
        QVariant sbyteArray = sbyteArrayNode->value();
        QVERIFY(sbyteArray.type() == QVariant::List && sbyteArray.toList().length() == 3);
        QVERIFY(byteArray.toList()[0].userType() == QMetaType::SChar);
        QVERIFY(byteArray.toList()[0] == (char)-11);
        QVERIFY(byteArray.toList()[1] == (char)-12);
        QVERIFY(byteArray.toList()[2] == (char)-13);
        qDebug() << sbyteArray.toList();
    }

    if (opcuaClient->backend() == QLatin1String("freeopcua")) {
        QWARN("NodeId arrays are broken with the freeopcua backend");
    } else {
        QScopedPointer<QOpcUaNode> nodeIdArrayNode(opcuaClient->node("ns=2;s=Demo.Static.Arrays.NodeId"));
        QVERIFY(byteArrayNode != 0);
        QVariant nodeIdArray = nodeIdArrayNode->value();
        QVERIFY(nodeIdArray.type() == QVariant::List && nodeIdArray.toList().length() == 3);
        QCOMPARE(nodeIdArray.toList()[0].type(), QVariant::String);
        QCOMPARE(nodeIdArray.toList()[0].toString(), QStringLiteral("ns=0;i=0"));
        QCOMPARE(nodeIdArray.toList()[1].toString(), QStringLiteral("ns=0;i=1"));
        QCOMPARE(nodeIdArray.toList()[2].toString(), QStringLiteral("ns=0;i=2"));
        qDebug() << nodeIdArray.toList();
    }

    QSKIP("XmlElement not available with freeopcua-based test server");
    // TODO add Static.Arrays.XmlElement test
}

void Tst_QOpcUaClient::writeScalar()
{
    QFETCH(QOpcUaClient*, opcuaClient);
    bool success;

    QScopedPointer<QOpcUaNode> booleanNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Boolean"));
    QVERIFY(booleanNode != 0);
    success = booleanNode->setValue(true, QOpcUa::Boolean);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> int32Node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Int32"));
    QVERIFY(int32Node != 0);
    success = int32Node->setValue(42, QOpcUa::Int32);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> uint32Node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.UInt32"));
    QVERIFY(uint32Node != 0);
    success = uint32Node->setValue(42, QOpcUa::UInt32);
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
    success = uint16Node->setValue(42, QOpcUa::UInt16);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> int16Node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Int16"));
    QVERIFY(int16Node != 0);
    success = int16Node->setValue(42, QOpcUa::Int16);
    QVERIFY(success == true);

    success = opcuaClient->node("ns=2;s=Demo.Static.Scalar.UInt64")->setValue(42, QOpcUa::UInt64);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> uint64Node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.UInt64"));
    QVERIFY(uint64Node != 0);
    success = uint64Node->setValue(42, QOpcUa::UInt64);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> int64Node(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Int64"));
    QVERIFY(int64Node != 0);
    success = int64Node->setValue(true, QOpcUa::Int64);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> byteNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.Byte"));
    QVERIFY(byteNode != 0);
    success = byteNode->setValue(42, QOpcUa::Byte);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> sbyteNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.SByte"));
    QVERIFY(sbyteNode != 0);
    success = sbyteNode->setValue(42, QOpcUa::SByte);
    QVERIFY(success == true);

    QScopedPointer<QOpcUaNode> ltNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.LocalizedText"));
    QVERIFY(ltNode != 0);
    success = ltNode->setValue(QStringLiteral("QOpcUaClient Localized Text"), QOpcUa::LocalizedText);
    QVERIFY(success == true);


    QVariant data = QByteArray("abc");

    QScopedPointer<QOpcUaNode> byteStringNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.ByteString"));
    QVERIFY(byteStringNode != 0);
    success = byteStringNode->setValue(data, QOpcUa::ByteString);
    QVERIFY(success == true);

    if (opcuaClient->backend() == QLatin1String("freeopcua")) {
        QSKIP("NodeId and XmlElement parsing is not yet implemented in the freeopcua backend");
    }

    QScopedPointer<QOpcUaNode> nodeIdNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.NodeId"));
    QVERIFY(nodeIdNode != 0);
    success = nodeIdNode->setValue("ns=42;s=Test", QOpcUa::NodeId);
    QVERIFY(success == true);

    QSKIP("XmlElement not available with freeopcua-based test server");
    QScopedPointer<QOpcUaNode> xmlElementNode(opcuaClient->node("ns=2;s=Demo.Static.Scalar.XmlElement"));
    QVERIFY(xmlElementNode != 0);
    success = xmlElementNode->setValue("<?xml version=\"42\" encoding=\"UTF-8\"?>", QOpcUa::XmlElement);
    QVERIFY(success == true);
}

void Tst_QOpcUaClient::readScalar()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    QOpcUaNode *node;

    node = opcuaClient->node("ns=2;s=Demo.Static.Scalar.Boolean");
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QStringLiteral("Boolean"));
    QVariant booleanScalar = node->value();
    QVERIFY(booleanScalar.isValid());
    QCOMPARE(booleanScalar.type(), QVariant::Bool);
    QCOMPARE(booleanScalar.toBool(), true);
    qDebug() << booleanScalar;

    node = opcuaClient->node("ns=2;s=Demo.Static.Scalar.Int32");
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QStringLiteral("Int32"));
    QVariant int32Scalar = node->value();

    QVERIFY(int32Scalar.isValid());
    QCOMPARE(int32Scalar.type(), QVariant::Int);
    QCOMPARE(int32Scalar.toInt(), 42);
    qDebug() << int32Scalar;

    node = opcuaClient->node("ns=2;s=Demo.Static.Scalar.UInt32");
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QStringLiteral("UInt32"));
    QVariant uint32Scalar = node->value();
    QVERIFY(uint32Scalar.isValid());
    QCOMPARE(uint32Scalar.type(), QVariant::UInt);
    QCOMPARE(uint32Scalar.toUInt(), (uint)42);
    qDebug() << uint32Scalar;

    node = opcuaClient->node("ns=2;s=Demo.Static.Scalar.Double");
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QStringLiteral("Double"));
    QVariant doubleScalar = node->value();
    QVERIFY(doubleScalar.isValid());
    QCOMPARE(doubleScalar.type(), QVariant::Double);
    QCOMPARE(doubleScalar.toDouble(), (double)42);
    qDebug() << doubleScalar;

    node = opcuaClient->node("ns=2;s=Demo.Static.Scalar.Float");
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QStringLiteral("Float"));
    QVariant floatScalar = node->value();
    QVERIFY(floatScalar.isValid());
    QVERIFY(floatScalar.userType() == QMetaType::Float);
    QCOMPARE(floatScalar.toFloat(), (float)42);
    qDebug() << floatScalar;

    node = opcuaClient->node("ns=2;s=Demo.Static.Scalar.String");
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QStringLiteral("String"));
    QVariant stringScalar = node->value();
    QVERIFY(stringScalar.isValid());
    QCOMPARE(stringScalar.type(), QVariant::String);
    QCOMPARE(stringScalar.toString(), QStringLiteral("QOpcUa Teststring"));
    qDebug() << stringScalar;

    node = opcuaClient->node("ns=2;s=Demo.Static.Scalar.DateTime");
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QStringLiteral("DateTime"));
    QVariant dateTimeScalar = node->value();
    QCOMPARE(dateTimeScalar.type(), QVariant::DateTime);
    QVERIFY(dateTimeScalar.isValid());
    qDebug() << dateTimeScalar;

    node = opcuaClient->node("ns=2;s=Demo.Static.Scalar.LocalizedText");
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QStringLiteral("LocalizedText"));
    QVariant ltScalar = node->value();
    QVERIFY(ltScalar.isValid());
    QCOMPARE(ltScalar.type(), QVariant::String);
    QCOMPARE(ltScalar.toString(), QStringLiteral("QOpcUaClient Localized Text"));
    qDebug() << ltScalar;

    node = opcuaClient->node("ns=2;s=Demo.Static.Scalar.UInt16");
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QStringLiteral("UInt16"));
    QVariant uint16Scalar = node->value();
    QVERIFY(uint16Scalar.isValid());
    QVERIFY(uint16Scalar.userType() == QMetaType::UShort);
    QVERIFY(uint16Scalar == (ushort)42);
    qDebug() << uint16Scalar;

    node = opcuaClient->node("ns=2;s=Demo.Static.Scalar.Int16");
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QStringLiteral("Int16"));
    QVariant int16Scalar = node->value();
    QVERIFY(int16Scalar.isValid());
    QVERIFY(int16Scalar.userType() == QMetaType::Short);
    QVERIFY(uint16Scalar == (short)42);
    qDebug() << int16Scalar;

    if (opcuaClient->backend() == QLatin1String("freeopcua")) {
        QWARN("*Int64 types are broken with the freeopcua backend");
    } else {
        QSKIP("Int64 types broken with freeopcua-based test server");
        node = opcuaClient->node("ns=2;s=Demo.Static.Scalar.UInt64");
        QVERIFY(node != 0);
        QCOMPARE(node->type(), QStringLiteral("UInt64"));
        QVariant uint64Scalar = node->value();
        QVERIFY(uint64Scalar.isValid());
        QCOMPARE(uint64Scalar.type(), QVariant::ULongLong);
        QVERIFY(uint64Scalar == (unsigned long long)42);
        qDebug() << uint64Scalar;

        node = opcuaClient->node("ns=2;s=Demo.Static.Scalar.Int64");
        QVERIFY(node != 0);
        QCOMPARE(node->type(), QStringLiteral("Int64"));
        QVariant int64Scalar = node->value();
        QVERIFY(int64Scalar.isValid());
        QCOMPARE(int64Scalar.type(), QVariant::LongLong);
        QVERIFY(int64Scalar == (long long)42);
        qDebug() << int64Scalar;
    }

    node = opcuaClient->node("ns=2;s=Demo.Static.Scalar.Byte");
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QStringLiteral("Byte"));
    QVariant byteScalar = node->value();
    QVERIFY(byteScalar.isValid());
    QVERIFY(byteScalar.userType() == QMetaType::UChar);
    QVERIFY(byteScalar == (char)42);
    qDebug() << byteScalar;

    node = opcuaClient->node("ns=2;s=Demo.Static.Scalar.SByte");
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QStringLiteral("SByte"));
    QVariant sbyteScalar = node->value();
    QVERIFY(sbyteScalar.isValid());
    QVERIFY(sbyteScalar.userType() == QMetaType::SChar);
    QVERIFY(sbyteScalar == (char)42);
    qDebug() << sbyteScalar;

    node = opcuaClient->node("ns=2;s=Demo.Static.Scalar.ByteString");
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QStringLiteral("ByteString"));
    QVariant byteStringScalar = node->value();
    QVERIFY(byteStringScalar.isValid());
    QVERIFY(byteStringScalar.userType() == QMetaType::QByteArray);
    QVERIFY(byteStringScalar == "abc");
    qDebug() << byteStringScalar;

    QVariant encodedByteStringScalar = node->encodedValue();
    QVERIFY(encodedByteStringScalar.isValid());
    QVERIFY(encodedByteStringScalar.type() == QVariant::String);
    QVERIFY(encodedByteStringScalar == QByteArray("abc").toBase64());
    qDebug() << encodedByteStringScalar;

    if (opcuaClient->backend() == QLatin1String("freeopcua"))
        QSKIP("NodeId parsing is not yet implemented in the freeopcua backend");

    node = opcuaClient->node("ns=2;s=Demo.Static.Scalar.NodeId");
    QVERIFY(node != 0);
    QCOMPARE(node->type(), QStringLiteral("NodeId"));
    QVariant nodeIdScalar = node->value();
    QVERIFY(nodeIdScalar.isValid());
    QCOMPARE(nodeIdScalar.type(), QVariant::String);
    QCOMPARE(nodeIdScalar.toString(), QStringLiteral("ns=42;s=Test"));
    qDebug() << nodeIdScalar;

    QSKIP("XmlElement not available with freeopcua-based test server");
    // TODO add Static.Scalar.XmlElement test
}

void Tst_QOpcUaClient::disconnect()
{
    QFETCH(QOpcUaClient*, opcuaClient);

    QVERIFY(m_connected == true);

    opcuaClient->disconnectFromEndpoint();

    QTRY_VERIFY_WITH_TIMEOUT(m_connected == false, 1250); // Wait 1.25 seconds for state change

    QScopedPointer<QOpcUaNode> node(opcuaClient->node(readWriteNode));
    QVERIFY(node == 0);
}

void Tst_QOpcUaClient::cleanupTestCase()
{
    if (m_serverProcess.state() == QProcess::Running) {
        m_serverProcess.kill();
        m_serverProcess.waitForFinished(2000);
    }
}

void Tst_QOpcUaClient::processConnected()
{
    qDebug("Connection state changed");
    m_connected = true;
}

void Tst_QOpcUaClient::processDisconnected()
{
    m_connected = false;
}

void Tst_QOpcUaClient::processSubscription(QVariant val)
{
    QCOMPARE(QThread::currentThread(), QCoreApplication::instance()->thread());
    m_value = val.toDouble();
}

void Tst_QOpcUaClient::processEventSubscription(QVector<QVariant> val)
{
    qDebug("Event");
    QCOMPARE(QThread::currentThread(), QCoreApplication::instance()->thread());
    Q_UNUSED(val)
    m_event = true;
    QCOMPARE(val.size(), 3);
    QCOMPARE(val.at(0).type(), QVariant::String);
    QCOMPARE(val.at(1).type(), QVariant::String);
    QCOMPARE(val.at(2).type(), QVariant::Int);
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QTEST_SET_MAIN_SOURCE_PATH

    int result = 0;

    // run tests for all available backends
    QStringList availableBackends = QOpcUaProvider::availableBackends();
    if (availableBackends.empty()) {
        qDebug("No OPCUA backends found, skipping tests.");
        return EXIT_SUCCESS;
    }

    Tst_QOpcUaClient tc;
    result = QTest::qExec(&tc, argc, argv);
    return result;
}

#include "tst_client.moc"


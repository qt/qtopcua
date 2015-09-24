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

#include <QtOpcUa/QOpcUaProvider>
#include <QtOpcUa/QOpcUaClient>

#include <QtTest/QtTest>

#include <QtCore/QProcess>
#include <QtCore/QScopedPointer>
#include <QtCore/QThread>
#include <QtCore/QCoreApplication>

const QString readWriteNode = QStringLiteral("ns=3;s=TestNode.ReadWrite");
const int numberOfOperations = 1000;

class TestQOpcUaClient: public QObject
{
    Q_OBJECT

public:
    TestQOpcUaClient(QString backend, QObject* parent = 0)
        : QObject(parent)
        , m_opcUa(backend)
        , m_client(m_opcUa.createClient())
    {
    }

private:
    QString envOrDefault(const char *env, QString def)
    {
        return qEnvironmentVariableIsSet(env) ? qgetenv(env).constData() : def;
    }

    QString m_endpoint;
    double m_value;
    bool m_event;
    QOpcUaProvider m_opcUa;
    QOpcUaClient *m_client;
    QProcess m_serverProcess;

private slots:
    void initTestCase()
    {
        const QString testServerPath(QDir::currentPath() + "/../freeopcua-testserver/freeopcua-testserver");
        if (!QFile::exists(testServerPath))
            QSKIP("all auto tests rely on a freeopcua based test-server");

        m_serverProcess.start(testServerPath);
        QVERIFY2(m_serverProcess.waitForStarted(), qPrintable(m_serverProcess.errorString()));
        // Let the server come up
        QTest::qSleep(2000);
        QString host = envOrDefault("OPCUA_HOST", "localhost");
        QString port = envOrDefault("OPCUA_PORT", "43344");
        m_endpoint = QString("opc.tcp://%1:%2").arg(host).arg(port);
        qDebug() << "Using endpoint:" << m_endpoint;
        qDebug() << "Using SDK plugin:" << m_opcUa.backend();

        QVERIFY(m_client != 0);
    }

    void connectToInvalid()
    {
        bool isConnected = m_client->connectToEndpoint("opc.tcp:127.0.0.1:1234");
        QVERIFY(isConnected == false);
    }

    void connect()
    {
        QVERIFY(m_client != 0);
        bool isConnected = m_client->connectToEndpoint(m_endpoint);
        QVERIFY(isConnected == true);
    }

    void getRootNode()
    {
        QScopedPointer<QOpcUaNode> root(m_client->node("ns=0;i=84"));
        QVERIFY(root != 0);
        QVERIFY(root->name() == QLatin1String("Root"));
    }

    void getChildren()
    {
        QScopedPointer<QOpcUaNode> node(m_client->node("ns=1;s=Large.Folder"));
        QVERIFY(node != 0);
        QVERIFY(node->name() == QLatin1String("Large_Folder"));
        QVERIFY(node->childIds().size() == 1001);
    }

    void read()
    {
        for (int i = 0; i < numberOfOperations; i++)
            QVERIFY(m_client->read(readWriteNode).type() == QVariant::Double);
    }

    void readWrite()
    {
        for (int i = 0; i < numberOfOperations; i++) {
            m_client->write(readWriteNode, QVariant((double) i));
            QVERIFY(m_client->read(readWriteNode).toInt() ==  i);
        }
    }

    void dataChangeSubscription()
    {
        m_value = 0;
        QOpcUaMonitoredItem *sub;
        m_client->write(readWriteNode, QVariant((double ) 0));
        sub = m_client->dataMonitor(100, readWriteNode);
        QObject::connect(sub, &QOpcUaMonitoredItem::valueChanged, this, &TestQOpcUaClient::processSubscription);
        m_client->write(readWriteNode, QVariant((double ) 42));
        while (m_value == 0) {
            QTest::qWait(25); // wait n periods for data change
        }
        QObject::disconnect(sub, &QOpcUaMonitoredItem::valueChanged, this, &TestQOpcUaClient::processSubscription);
        qDebug() << "Value from subscription: " << m_value;
        QVERIFY(m_value == (double) 42);
        delete sub;
    }

    void methodCall()
    {
        QSKIP("Method calls are not implemented in the testserver");
        QVector<QOpcUaTypedVariant> args;
        QVector<QVariant> ret;
        for (int i = 0; i < 2; i++)
            args.push_back(QOpcUaTypedVariant((double ) 4, QOpcUa::Double));

        bool success = m_client->call("ns=3;s=TestFolder", "ns=3;s=Test.Method.Multiply", &args, &ret);
        QVERIFY(success == true);
        QVERIFY(ret.size() == 1);
        QVERIFY(ret[0].type() == QVariant::Double && ret[0].value<double>() == 16);
    }

    void eventSubscription()
    {
        QSKIP("Does not reliably work with the testserver");
        if (m_opcUa.backend() == QLatin1String("freeopcua"))
            QSKIP("Event subscriptions do not yet work with the freeopcua backend");

        qDebug("Event subscription");
        m_event = false;
        QOpcUaMonitoredItem *sub = m_client->eventMonitor("ns=3;s=TriggerNode");
        Q_ASSERT(sub != 0);
        QObject::connect(sub, &QOpcUaMonitoredItem::newEvent, this,
                         &TestQOpcUaClient::processEventSubscription);
        m_client->write("ns=3;s=TriggerVariable", QVariant((double) 0));
        m_client->write("ns=3;s=TriggerVariable", QVariant((double) 1));
        while (m_event == false) {
            QTest::qWait(25);
        }
        QObject::disconnect(sub, &QOpcUaMonitoredItem::newEvent, this,
                            &TestQOpcUaClient::processEventSubscription);
        delete sub;
    }

    void readRange()
    {
        QSKIP("No ranges supported in our testserver");
        if (m_opcUa.backend() == QLatin1String("freeopcua"))
            QSKIP("Ranges are not yet implemented in the freeopcua backend");

        QPair<double, double> range = m_client->readEuRange("ns=3;s=ACControl.CurrentTemp.EURange");
        QVERIFY(range.first == 0 && range.second == 100);
    }

    void invalidNodeAccess()
    {
        if (m_opcUa.backend() == QLatin1String("freeopcua"))
            QSKIP("The freeopcua backend does not check if remote nodes exist on instantiation.");

        QOpcUaNode *invalidNode = m_client->node("ns=0;s=IDoNotExist");
        QVERIFY(invalidNode == 0);
    }

    void multipleClients()
    {
        QScopedPointer<QOpcUaClient> a(m_opcUa.createClient());
        qDebug() << a->connectToEndpoint(m_endpoint);
        qDebug() << "a: " << a->read(readWriteNode).toDouble();
        QScopedPointer<QOpcUaClient> b(m_opcUa.createClient());
        qDebug() << b->connectToEndpoint(m_endpoint);
        qDebug() << "b: " << b->read(readWriteNode).toDouble();
        QScopedPointer<QOpcUaClient> d(m_opcUa.createClient());
        qDebug() << d->connectToEndpoint(m_endpoint);
        qDebug() << "d: " << d->read(readWriteNode).toDouble();
        d->disconnectFromEndpoint();
        a->disconnectFromEndpoint();
        b->disconnectFromEndpoint();
    }

    void nodeClass()
    {
        // Root -> Types -> ReferenceTypes -> References
        QScopedPointer<QOpcUaNode> refNode(m_client->node("ns=0;i=31"));
        QVERIFY(refNode != 0);
        QCOMPARE(refNode->nodeClass(), QStringLiteral("ReferenceType"));
    }

    void writeArray()
    {
        QVariantList list;

        list.append(true);
        list.append(false);
        list.append(true);
        bool success = m_client->write("ns=2;s=Demo.Static.Arrays.Boolean", list, QOpcUa::Boolean);
        QVERIFY(success == true);

        list.clear();
        list.append(11);
        list.append(12);
        list.append(13);
        success = m_client->write("ns=2;s=Demo.Static.Arrays.Byte", list, QOpcUa::Byte);
        QVERIFY(success == true);

        list.clear();
        list.append(QDateTime::currentDateTime());
        list.append(QDateTime::currentDateTime());
        list.append(QDateTime::currentDateTime());
        success = m_client->write("ns=2;s=Demo.Static.Arrays.DateTime", list, QOpcUa::DateTime);
        QVERIFY(success == true);

        list.clear();
        list.append(23.5);
        list.append(23.6);
        list.append(23.7);
        success = m_client->write("ns=2;s=Demo.Static.Arrays.Double", list, QOpcUa::Double);
        QVERIFY(success == true);

        list.clear();
        list.append(23.5);
        list.append(23.6);
        list.append(23.7);
        success = m_client->write("ns=2;s=Demo.Static.Arrays.Float", list, QOpcUa::Float);
        QVERIFY(success == true);

        list.clear();
        list.append(-11);
        list.append(-12);
        list.append(-13);
        success = m_client->write("ns=2;s=Demo.Static.Arrays.Int16", list, QOpcUa::Int16);
        QVERIFY(success == true);

        list.clear();
        list.append(-11);
        list.append(-12);
        list.append(-13);
        success = m_client->write("ns=2;s=Demo.Static.Arrays.Int32", list, QOpcUa::Int32);
        QVERIFY(success == true);

        list.clear();
        list.append(-11);
        list.append(-12);
        list.append(-13);
        success = m_client->write("ns=2;s=Demo.Static.Arrays.Int64", list, QOpcUa::Int64);
        QVERIFY(success == true);

        list.clear();
        list.append(-11);
        list.append(-12);
        list.append(-13);
        success = m_client->write("ns=2;s=Demo.Static.Arrays.SByte", list, QOpcUa::SByte);
        QVERIFY(success == true);

        list.clear();
        list.append("Test1");
        list.append("Test2");
        list.append("Test3");
        success = m_client->write("ns=2;s=Demo.Static.Arrays.String", list, QOpcUa::String);
        QVERIFY(success == true);

        list.clear();
        list.append(11);
        list.append(12);
        list.append(13);
        success = m_client->write("ns=2;s=Demo.Static.Arrays.UInt16", list, QOpcUa::UInt16);
        QVERIFY(success == true);

        list.clear();
        list.append(11);
        list.append(12);
        list.append(13);
        success = m_client->write("ns=2;s=Demo.Static.Arrays.UInt32", list, QOpcUa::UInt32);
        QVERIFY(success == true);

        list.clear();
        list.append(11);
        list.append(12);
        list.append(13);
        success = m_client->write("ns=2;s=Demo.Static.Arrays.UInt64", list, QOpcUa::UInt64);
        QVERIFY(success == true);

        list.clear();
        list.append("Localized Text 1");
        list.append("Localized Text 2");
        list.append("Localized Text 3");
        success = m_client->write("ns=2;s=Demo.Static.Arrays.LocalizedText", list, QOpcUa::LocalizedText);
        QVERIFY(success == true);

        list.clear();
        list.append("abc");
        list.append("def");
        list.append("ghi");
        success = m_client->write("ns=2;s=Demo.Static.Arrays.ByteString", list, QOpcUa::ByteString);
        QVERIFY(success == true);

        if (m_opcUa.backend() == QLatin1String("freeopcua"))
            QSKIP("NodeId parsing is not yet implemented in the freeopcua backend");

        list.clear();
        list.append("ns=0;i=0");
        list.append("ns=0;i=1");
        list.append("ns=0;i=2");
        success = m_client->write("ns=2;s=Demo.Static.Arrays.NodeId", list, QOpcUa::NodeId);
        QVERIFY(success == true);
    }

    void readArray()
    {
        QVariant booleanArray = m_client->read("ns=2;s=Demo.Static.Arrays.Boolean");
        QVERIFY(booleanArray.type() == QVariant::List && booleanArray.toList().length() == 3);

        QVariant int32Array = m_client->read("ns=2;s=Demo.Static.Arrays.Int32");
        QVERIFY(int32Array.type() == QVariant::List && int32Array.toList().length() == 3);

        QVariant uint32Array = m_client->read("ns=2;s=Demo.Static.Arrays.UInt32");
        QVERIFY(uint32Array.type() == QVariant::List && uint32Array.toList().length() == 3);

        QVariant doubleArray = m_client->read("ns=2;s=Demo.Static.Arrays.Double");
        QVERIFY(doubleArray.type() == QVariant::List && doubleArray.toList().length() == 3);

        QVariant floatArray = m_client->read("ns=2;s=Demo.Static.Arrays.Float");
        QVERIFY(floatArray.type() == QVariant::List && floatArray.toList().length() == 3);

        QVariant stringArray = m_client->read("ns=2;s=Demo.Static.Arrays.String");
        QVERIFY(stringArray.type() == QVariant::List && stringArray.toList().length() == 3);

        QVariant dateTimeArray = m_client->read("ns=2;s=Demo.Static.Arrays.DateTime");
        QVERIFY(dateTimeArray.type() == QVariant::List && dateTimeArray.toList().length() == 3);

        QVariant ltArray = m_client->read("ns=2;s=Demo.Static.Arrays.LocalizedText");
        QVERIFY(ltArray.type() == QVariant::List && ltArray.toList().length() == 3);

        QVariant uint16Array = m_client->read("ns=2;s=Demo.Static.Arrays.UInt16");
        QVERIFY(uint16Array.type() == QVariant::List && uint16Array.toList().length() == 3);

        QVariant int16Array = m_client->read("ns=2;s=Demo.Static.Arrays.Int16");
        QVERIFY(int16Array.type() == QVariant::List && int16Array.toList().length() == 3);
        if (m_opcUa.backend() == QLatin1String("freeopcua")) {
            QWARN("*Int64 types are broken with the freeopcua backend");
        } else {
            QVariant uint64Array = m_client->read("ns=2;s=Demo.Static.Arrays.UInt64");
            QCOMPARE(uint64Array.type(), QVariant::List);
            QCOMPARE(uint64Array.toList().length(), 3);

            QVariant int64Array = m_client->read("ns=2;s=Demo.Static.Arrays.Int64");
            QVERIFY(int64Array.type() == QVariant::List && int64Array.toList().length() == 3);
        }

        QVariant byteArray = m_client->read("ns=2;s=Demo.Static.Arrays.Byte");
        QVERIFY(byteArray.type() == QVariant::List && byteArray.toList().length() == 3);

        if (m_opcUa.backend() == QLatin1String("freeopcua")) {
            QWARN("SByte is broken with the freeopcua backend");
        } else {
            QVariant sbyteArray = m_client->read("ns=2;s=Demo.Static.Arrays.SByte");
            QVERIFY(sbyteArray.type() == QVariant::List && sbyteArray.toList().length() == 3);
        }

        if (m_opcUa.backend() == QLatin1String("freeopcua")) {
            QWARN("NodeId arrays are broken with the freeopcua backend");
        } else {
            QVariant nodeIdArray = m_client->read("ns=2;s=Demo.Static.Arrays.NodeId");
            QVERIFY(nodeIdArray.type() == QVariant::List && nodeIdArray.toList().length() == 3);
        }
    }

    void writeScalar()
    {
        bool success;
        success = m_client->write("ns=2;s=Demo.Static.Scalar.Boolean", true, QOpcUa::Boolean);
        QVERIFY(success == true);

        success = m_client->write("ns=2;s=Demo.Static.Scalar.Int32", 42, QOpcUa::Int32);
        QVERIFY(success == true);

        success = m_client->write("ns=2;s=Demo.Static.Scalar.UInt32", 42, QOpcUa::UInt32);
        QVERIFY(success == true);

        success = m_client->write("ns=2;s=Demo.Static.Scalar.Double", 42, QOpcUa::Double);
        QVERIFY(success == true);

        success = m_client->write("ns=2;s=Demo.Static.Scalar.Float", 42, QOpcUa::Float);
        QVERIFY(success == true);

        success = m_client->write("ns=2;s=Demo.Static.Scalar.String", "QOpcUa Teststring", QOpcUa::String);
        QVERIFY(success == true);

        success = m_client->write("ns=2;s=Demo.Static.Scalar.DateTime", QDateTime::currentDateTime(),
                                  QOpcUa::DateTime);
        QVERIFY(success == true);

        success = m_client->write("ns=2;s=Demo.Static.Scalar.UInt16", 42, QOpcUa::UInt16);
        QVERIFY(success == true);

        success = m_client->write("ns=2;s=Demo.Static.Scalar.Int16", 42, QOpcUa::Int16);
        QVERIFY(success == true);

        success = m_client->write("ns=2;s=Demo.Static.Scalar.UInt64", 42, QOpcUa::UInt64);
        QVERIFY(success == true);

        success = m_client->write("ns=2;s=Demo.Static.Scalar.Int64", 42, QOpcUa::Int64);
        QVERIFY(success == true);

        success = m_client->write("ns=2;s=Demo.Static.Scalar.Byte", 42, QOpcUa::Byte);
        QVERIFY(success == true);

        success = m_client->write("ns=2;s=Demo.Static.Scalar.SByte", 42, QOpcUa::SByte);
        QVERIFY(success == true);

        success = m_client->write("ns=2;s=Demo.Static.Scalar.LocalizedText", "QOpcUaClient Localized Text",
                                  QOpcUa::LocalizedText);
        QVERIFY(success == true);


        if (m_opcUa.backend() == QLatin1String("freeopcua")) {
            QSKIP("NodeId and XmlElement parsing is not yet implemented in the freeopcua backend");
        }

        success = m_client->write("ns=2;s=Demo.Static.Scalar.NodeId", "ns=42;s=Test", QOpcUa::NodeId);
        QVERIFY(success == true);

        success = m_client->write("ns=2;s=Demo.Static.Scalar.XmlElement",
                                  "<?xml version=\"42\" encoding=\"UTF-8\"?>", QOpcUa::XmlElement);
        QVERIFY(success == true);

    }

    void readScalar() {
        QVariant booleanScalar = m_client->read("ns=2;s=Demo.Static.Scalar.Boolean");
        QVERIFY(booleanScalar.isValid());
        qDebug() << booleanScalar;

        QVariant int32Scalar = m_client->read("ns=2;s=Demo.Static.Scalar.Int32");
        QVERIFY(int32Scalar.isValid());
        qDebug() << int32Scalar;

        QVariant uint32Scalar = m_client->read("ns=2;s=Demo.Static.Scalar.UInt32");
        QVERIFY(uint32Scalar.isValid());
        qDebug() << uint32Scalar;

        QVariant doubleScalar = m_client->read("ns=2;s=Demo.Static.Scalar.Double");
        QVERIFY(doubleScalar.isValid());
        qDebug() << doubleScalar;

        QVariant floatScalar = m_client->read("ns=2;s=Demo.Static.Scalar.Float");
        QVERIFY(floatScalar.isValid());
        qDebug() << floatScalar;

        QVariant stringScalar = m_client->read("ns=2;s=Demo.Static.Scalar.String");
        QVERIFY(stringScalar.isValid());
        qDebug() << stringScalar;

        QVariant dateTimeScalar = m_client->read("ns=2;s=Demo.Static.Scalar.DateTime");
        QVERIFY(dateTimeScalar.isValid());
        qDebug() << dateTimeScalar;

        QVariant ltScalar = m_client->read("ns=2;s=Demo.Static.Scalar.LocalizedText");
        QVERIFY(ltScalar.isValid());
        qDebug() << ltScalar;

        QVariant uint16Scalar = m_client->read("ns=2;s=Demo.Static.Scalar.UInt16");
        QVERIFY(uint16Scalar.isValid());
        qDebug() << uint16Scalar;

        QVariant int16Scalar = m_client->read("ns=2;s=Demo.Static.Scalar.Int16");
        QVERIFY(int16Scalar.isValid());
        qDebug() << int16Scalar;

        if (m_opcUa.backend() == QLatin1String("freeopcua")) {
            QWARN("*Int64 types are broken with the freeopcua backend");
        } else {
            QVariant uint64Scalar = m_client->read("ns=2;s=Demo.Static.Scalar.UInt64");
            QVERIFY(uint64Scalar.isValid());
            qDebug() << uint64Scalar;

            QVariant int64Scalar = m_client->read("ns=2;s=Demo.Static.Scalar.Int64");
            QVERIFY(int64Scalar.isValid());
            qDebug() << int64Scalar;
        }

        QVariant byteScalar = m_client->read("ns=2;s=Demo.Static.Scalar.Byte");
        QVERIFY(byteScalar.isValid());
        qDebug() << byteScalar;

        QVariant sbyteScalar = m_client->read("ns=2;s=Demo.Static.Scalar.SByte");
        QVERIFY(sbyteScalar.isValid());
        qDebug() << sbyteScalar;

        QVariant nodeIdScalar = m_client->read("ns=2;s=Demo.Static.Scalar.NodeId");
        QVERIFY(nodeIdScalar.isValid());
        qDebug() << nodeIdScalar;
    }

    void disconnect()
    {
        bool disconnected = m_client->disconnectFromEndpoint();
        QVERIFY(disconnected == true);
    }

    void cleanupTestCase()
    {
        m_serverProcess.kill();
        m_serverProcess.waitForFinished(2000);
    }


public slots:
    void processSubscription(QVariant val)
    {
        m_value = val.toDouble();
    }

    void processEventSubscription(QList<QVariant> val)
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
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);

    QTEST_SET_MAIN_SOURCE_PATH

    int result = 0;

    // run tests for all available backends
    if (QOpcUaProvider::availableBackends().size() == 0) {
        qDebug("No OPCUA backends found, skipping tests.");
        return 0;
    }

    foreach (const QString &backend, QOpcUaProvider::availableBackends()) {
        qDebug() << "Testing backend:" << backend;

        TestQOpcUaClient tc(backend);
        result = QTest::qExec(&tc, argc, argv) || result;
    }

    return result;
}

#include "tst_client.moc"

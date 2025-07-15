// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtQuickTest/quicktest.h>
#include <QObject>
#include <QProcess>
#include <QQmlContext>
#include <QQmlEngine>
#include <QTcpServer>
#include <QTcpSocket>
#include <QOpcUaClient>
#include <QOpcUaProvider>
#include <QSignalSpy>

using namespace Qt::Literals::StringLiterals;

const int signalSpyTimeout = 10000;
const quint16 defaultPort = 43344;
const QHostAddress defaultHost(QHostAddress::LocalHost);

static QString envOrDefault(const char *env, QString def)
{
    return qEnvironmentVariableIsSet(env) ? QString::fromUtf8(qgetenv(env).constData()) : def;
}

class MyClass : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QOpcUaClient* connection READ connection NOTIFY connectionChanged)

public:
    MyClass (QObject* parent = nullptr) : QObject(parent) {
    }

    ~MyClass() {
        if (m_client && m_client->parent() == this)
            delete m_client;
    }

    QOpcUaClient *connection() const {
        return m_client;
    }

signals:
    void connectionChanged(QOpcUaClient *);

public slots:
    void startConnection() {
        QOpcUaProvider p;
        QOpcUaClient *client = p.createClient(u"open62541"_s);

        if (!client)
            qFatal("Failed to instantiate backend");

        m_client = client;

        QString host = envOrDefault("OPCUA_HOST", defaultHost.toString());
        QString port = envOrDefault("OPCUA_PORT", QString::number(defaultPort));
        const auto discoveryEndpoint = u"opc.tcp://%1:%2"_s.arg(host, port);

        QSignalSpy endpointSpy(client, &QOpcUaClient::endpointsRequestFinished);
        client->requestEndpoints(discoveryEndpoint);
        endpointSpy.wait(signalSpyTimeout);
        QCOMPARE(endpointSpy.size(), 1);

        const auto desc = endpointSpy.at(0).at(0).value<QList<QOpcUaEndpointDescription>>();
        QVERIFY(desc.size() > 0);
        QCOMPARE(endpointSpy.at(0).at(2).value<QUrl>(), discoveryEndpoint);

        client->connectToEndpoint(desc.first());
        QTRY_VERIFY_WITH_TIMEOUT(client->state() == QOpcUaClient::Connected, signalSpyTimeout);
        qDebug() << "DONE";
        emit connectionChanged(m_client);
    }

private:
    QPointer<QOpcUaClient> m_client;
};

class SetupClass : public QObject
{
    Q_OBJECT
public:
    SetupClass() {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    };
    ~SetupClass() {
    }

public slots:
    void applicationAvailable() {

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
            qDebug() << "Server Path:" << m_testServerPath;
            if (!QFile::exists(m_testServerPath)) {
                qFatal("all auto tests rely on an open62541-based test-server");
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

            qDebug() << "Starting test server";
            //m_serverProcess.setProcessChannelMode(QProcess::ForwardedChannels);
            m_serverProcess.start(m_testServerPath);
            QVERIFY2(m_serverProcess.waitForStarted(), qPrintable(m_serverProcess.errorString()));

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
        const QString host = envOrDefault("OPCUA_HOST", defaultHost.toString());
        const QString port = envOrDefault("OPCUA_PORT", QString::number(defaultPort));
        m_opcuaDiscoveryUrl = u"opc.tcp://%1:%2"_s.arg(host, port);
    }
    void qmlEngineAvailable(QQmlEngine *engine) {
        bool value = false;
#ifdef SERVER_SUPPORTS_SECURITY
        value = true;
#endif
        engine->rootContext()->setContextProperty(u"SERVER_SUPPORTS_SECURITY"_s, value);
        engine->rootContext()->setContextProperty(u"OPCUA_DISCOVERY_URL"_s, m_opcuaDiscoveryUrl);
        qmlRegisterType<MyClass>("App", 1, 0, "MyClass");
    }
    void cleanupTestCase() {
        if (m_serverProcess.state() == QProcess::Running) {
            m_serverProcess.kill();
            m_serverProcess.waitForFinished(2000);
        }
    }
private:
    QProcess m_serverProcess;
    QString m_testServerPath;
    QString m_opcuaDiscoveryUrl;
};

QUICK_TEST_MAIN_WITH_SETUP(opcua, SetupClass)

#include "tst_clientSetupInCpp.moc"

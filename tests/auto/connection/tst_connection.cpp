// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtOpcUa/QOpcUaAuthenticationInformation>
#include <QtOpcUa/QOpcUaClient>
#include <QtOpcUa/QOpcUaProvider>

#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtCore/QScopedPointer>

#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>
#include <QTcpSocket>
#include <QTcpServer>

using namespace Qt::Literals::StringLiterals;

#define defineDataMethod(name) void name()\
{\
    QTest::addColumn<QOpcUaClient *>("opcuaClient");\
    for (auto *client : std::as_const(m_clients))\
        QTest::newRow(client->backend().toLatin1().constData()) << client;\
}

const int signalSpyTimeout = 10000;

class Tst_Connection: public QObject
{
    Q_OBJECT

public:
    Tst_Connection();
    ~Tst_Connection();

private slots:
    void initTestCase();
    void cleanupTestCase();

    // connect & disconnect
    defineDataMethod(connectMultipleTimes_data)
    void connectMultipleTimes();

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

Tst_Connection::Tst_Connection()
{
    m_backends = QOpcUaProvider::availableBackends();
}

Tst_Connection::~Tst_Connection()
{
    qDeleteAll(m_clients);
}

void Tst_Connection::initTestCase()
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

void Tst_Connection::connectMultipleTimes()
{
    QFETCH(QOpcUaClient *, opcuaClient);

    QVERIFY(opcuaClient != nullptr);
    QSignalSpy connectedSpy(opcuaClient, &QOpcUaClient::connected);
    QSignalSpy disconnectedSpy(opcuaClient, &QOpcUaClient::disconnected);
    QSignalSpy stateSpy(opcuaClient, &QOpcUaClient::stateChanged);

    QTest::qWait(500);

    opcuaClient->connectToEndpoint(m_endpoint);
    QTRY_VERIFY2(opcuaClient->state() == QOpcUaClient::Connected, "Could not connect to server");

    QCOMPARE(connectedSpy.size(), 1);
    QCOMPARE(disconnectedSpy.size(), 0);
    QCOMPARE(stateSpy.size(), 2);

    QCOMPARE(stateSpy.at(0).at(0).value<QOpcUaClient::ClientState>(),
             QOpcUaClient::ClientState::Connecting);
    QCOMPARE(stateSpy.at(1).at(0).value<QOpcUaClient::ClientState>(),
             QOpcUaClient::ClientState::Connected);

    stateSpy.clear();
    connectedSpy.clear();
    disconnectedSpy.clear();

    QVERIFY(opcuaClient->endpoint() == m_endpoint);

    // Connect again
    opcuaClient->connectToEndpoint(m_endpoint);
    stateSpy.wait(signalSpyTimeout);
    QTRY_VERIFY2(opcuaClient->state() == QOpcUaClient::Connected, "Could not connect to server");

    QCOMPARE(connectedSpy.size(), 1);
    QCOMPARE(disconnectedSpy.size(), 1);
    QCOMPARE(stateSpy.size(), 3);

    QCOMPARE(stateSpy.at(0).at(0).value<QOpcUaClient::ClientState>(),
             QOpcUaClient::ClientState::Disconnected);
    QCOMPARE(stateSpy.at(1).at(0).value<QOpcUaClient::ClientState>(),
             QOpcUaClient::ClientState::Connecting);
    QCOMPARE(stateSpy.at(2).at(0).value<QOpcUaClient::ClientState>(),
             QOpcUaClient::ClientState::Connected);

    opcuaClient->disconnectFromEndpoint();
}

void Tst_Connection::cleanupTestCase()
{
    if (m_serverProcess.state() == QProcess::Running) {
        m_serverProcess.kill();
        m_serverProcess.waitForFinished(2000);
    }

    qDeleteAll(m_clients);
    m_clients.clear();
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

    Tst_Connection tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "tst_connection.moc"


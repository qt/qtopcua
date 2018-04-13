/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module of the Qt Toolkit.
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

#include <QtOpcUa/QOpcUaAuthenticationInformation>
#include <QtOpcUa/QOpcUaClient>
#include <QtOpcUa/QOpcUaEndpointDescription>
#include <QtOpcUa/QOpcUaProvider>

#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtCore/QScopedPointer>

#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>
#include <QTcpSocket>
#include <QTcpServer>

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

        QTest::qWait(500);

        opcuaClient->connectToEndpoint(endPoint);
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

static QString messageSecurityModeToString(QOpcUaEndpointDescription::MessageSecurityMode msm)
{
    if (msm == QOpcUaEndpointDescription::None)
        return "None";
    else if (msm ==  QOpcUaEndpointDescription::Sign)
        return "Sign";
    else if (msm == QOpcUaEndpointDescription::SignAndEncrypt)
        return "SignAndEncrypt";
    return "Invalid";
}

#define defineDataMethod(name) void name()\
{\
    QTest::addColumn<QString>("backend");\
    QTest::addColumn<QOpcUaEndpointDescription>("endpoint");\
    for (auto backend : m_backends)\
        for (auto endpoint : m_endpoints) { \
            const QString rowName = QString("%1 using %2 %3").arg(backend).arg(endpoint.securityPolicy()).arg(messageSecurityModeToString(endpoint.securityMode())); \
            QTest::newRow(rowName.toLatin1().constData()) << backend << endpoint; \
        } \
}

class Tst_QOpcUaSecurity: public QObject
{
    Q_OBJECT

public:
    Tst_QOpcUaSecurity();

private slots:
    void initTestCase();
    void cleanupTestCase();

    defineDataMethod(connectAndDisconnectSecureUnencryptedKey_data)
    void connectAndDisconnectSecureUnencryptedKey();

    defineDataMethod(connectAndDisconnectSecureEncryptedKey_data)
    void connectAndDisconnectSecureEncryptedKey();

private:
    QString envOrDefault(const char *env, QString def)
    {
        return qEnvironmentVariableIsSet(env) ? qgetenv(env).constData() : def;
    }

    QString m_testServerPath;
    QStringList m_backends;
    QProcess m_serverProcess;
    QVector<QOpcUaEndpointDescription> m_endpoints;
    QString m_discoveryEndpoint;
    QOpcUaProvider m_opcUa;
    QSharedPointer<QTemporaryDir> m_pkiData;
};

Tst_QOpcUaSecurity::Tst_QOpcUaSecurity()
{
    m_backends = QOpcUaProvider::availableBackends();
}

void Tst_QOpcUaSecurity::initTestCase()
{
    const quint16 defaultPort = 43344;
    const QHostAddress defaultHost(QHostAddress::LocalHost);

    m_pkiData = QTest::qExtractTestData("pki");
    qDebug() << "PKI data available at" << m_pkiData->path();

    if (qEnvironmentVariableIsEmpty("OPCUA_HOST") && qEnvironmentVariableIsEmpty("OPCUA_PORT")) {
        m_testServerPath = qApp->applicationDirPath()

#if defined(Q_OS_MACOS)
                                     + QLatin1String("/../../open62541-testserver/open62541-testserver.app/Contents/MacOS/open62541-testserver")
#else

#ifdef Q_OS_WIN
                                     + QLatin1String("/..")
#endif
                                     + QLatin1String("/../../open62541-testserver/open62541-testserver")
#ifdef Q_OS_WIN
                                     + QLatin1String(".exe")
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
        // Let the server come up
        QTest::qSleep(2000);
    }
    QString host = envOrDefault("OPCUA_HOST", defaultHost.toString());
    QString port = envOrDefault("OPCUA_PORT", QString::number(defaultPort));
    m_discoveryEndpoint = QString("opc.tcp://%1:%2").arg(host).arg(port);
    qDebug() << "Using endpoint:" << m_discoveryEndpoint;

    QScopedPointer<QOpcUaClient> client(m_opcUa.createClient(m_backends.first()));
    QVERIFY2(client, "Loading backend failed");

    if (client) {
        QSignalSpy endpointSpy(client.data(), &QOpcUaClient::endpointsRequestFinished);

        client->requestEndpoints(m_discoveryEndpoint);
        endpointSpy.wait(signalSpyTimeout);
        QCOMPARE(endpointSpy.size(), 1);
        QCOMPARE(endpointSpy.at(0).at(2).value<QUrl>(), m_discoveryEndpoint);

        const QVector<QOpcUaEndpointDescription> desc = endpointSpy.at(0).at(0).value<QVector<QOpcUaEndpointDescription>>();
        QVERIFY(desc.size() > 0);

        m_endpoints.clear();

        // Select first non-None security policy
        for (const auto &endpoint : qAsConst(desc)) {
            if (QOpcUa::isSecurePolicy(endpoint.securityPolicy())) {
                m_endpoints.append(endpoint);
                qDebug() << endpoint.securityPolicy();
            }
        }

        QVERIFY(m_endpoints.size() > 0);
    }
}

void Tst_QOpcUaSecurity::connectAndDisconnectSecureUnencryptedKey()
{
    QFETCH(QString, backend);
    QFETCH(QOpcUaEndpointDescription, endpoint);

    QScopedPointer<QOpcUaClient> client(m_opcUa.createClient(backend));
    QVERIFY2(client, QString("Loading backend failed: %1").arg(backend).toLatin1().data());

    if (!client->supportedSecurityPolicies().contains(endpoint.securityPolicy()))
           QSKIP(QString("This test is skipped because backend %1 does not support security policy %2").arg(
                     client->backend()).arg(endpoint.securityPolicy()).toLatin1().constData());

    const QString pkidir = m_pkiData->path();
    QOpcUaPkiConfiguration pkiConfig;
    pkiConfig.setClientCertificateFile(pkidir + "/own/certs/tst_security.der");
    pkiConfig.setPrivateKeyFile(pkidir + "/own/private/privateKeyWithoutPassword.pem");
    pkiConfig.setTrustListDirectory(pkidir + "/trusted/certs");
    pkiConfig.setRevocationListDirectory(pkidir + "/trusted/crl");
    pkiConfig.setIssuerListDirectory(pkidir + "/issuers/certs");
    pkiConfig.setIssuerRevocationListDirectory(pkidir + "/issuers/crl");

    const auto identity = pkiConfig.applicationIdentity();
    QOpcUaAuthenticationInformation authInfo;
    authInfo.setUsernameAuthentication("user1", "password");

    client->setAuthenticationInformation(authInfo);
    client->setApplicationIdentity(identity);
    client->setPkiConfiguration(pkiConfig);

    qDebug() << "Testing security policy" << endpoint.securityPolicy();
    QSignalSpy connectSpy(client.data(), &QOpcUaClient::stateChanged);
    int passwordRequestSpy = 0;
    connect(client.data(), &QOpcUaClient::passwordForPrivateKeyRequired, [&passwordRequestSpy](const QString &privateKeyFilePath, QString *password, bool previousTryFailed) {
        Q_UNUSED(privateKeyFilePath);
        Q_UNUSED(previousTryFailed);
        Q_UNUSED(password);
        ++passwordRequestSpy;
    });

    client->connectToEndpoint(endpoint);
    connectSpy.wait(signalSpyTimeout);
    if (client->state() == QOpcUaClient::Connecting)
        connectSpy.wait(signalSpyTimeout);

    QCOMPARE(connectSpy.count(), 2);
    QCOMPARE(connectSpy.at(0).at(0), QOpcUaClient::Connecting);
    connectSpy.wait(signalSpyTimeout);
    QCOMPARE(connectSpy.at(1).at(0), QOpcUaClient::Connected);

    QCOMPARE(passwordRequestSpy, 0);

    QCOMPARE(client->endpoint(), endpoint);
    QCOMPARE(client->error(), QOpcUaClient::NoError);
    qDebug() << "connected";

    connectSpy.clear();
    client->disconnectFromEndpoint();
    connectSpy.wait(signalSpyTimeout);
    QCOMPARE(connectSpy.count(), 2);
    QCOMPARE(connectSpy.at(0).at(0), QOpcUaClient::Closing);
    QCOMPARE(connectSpy.at(1).at(0), QOpcUaClient::Disconnected);
}

void Tst_QOpcUaSecurity::connectAndDisconnectSecureEncryptedKey()
{
    QFETCH(QString, backend);
    QFETCH(QOpcUaEndpointDescription, endpoint);

    QScopedPointer<QOpcUaClient> client(m_opcUa.createClient(backend));
    QVERIFY2(client, QString("Loading backend failed: %1").arg(backend).toLatin1().data());

    if (client->backend() == QLatin1String("open62541"))
        QSKIP(QString("This test is skipped because backend %1 does not support encrypted keys").arg(client->backend()).toLatin1().constData());

    const QString pkidir = m_pkiData->path();
    QOpcUaPkiConfiguration pkiConfig;
    pkiConfig.setClientCertificateFile(pkidir + "/own/certs/tst_security.der");
    pkiConfig.setPrivateKeyFile(pkidir + "/own/private/privateKeyWithPassword_secret.pem");
    pkiConfig.setTrustListDirectory(pkidir + "/trusted/certs");
    pkiConfig.setRevocationListDirectory(pkidir + "/trusted/crl");
    pkiConfig.setIssuerListDirectory(pkidir + "/issuers/certs");
    pkiConfig.setIssuerRevocationListDirectory(pkidir + "/issuers/crl");

    const auto identity = pkiConfig.applicationIdentity();
    QOpcUaAuthenticationInformation authInfo;
    authInfo.setUsernameAuthentication("user1", "password");

    client->setAuthenticationInformation(authInfo);
    client->setApplicationIdentity(identity);
    client->setPkiConfiguration(pkiConfig);

    qDebug() << "Testing security policy" << endpoint.securityPolicy();
    QSignalSpy connectSpy(client.data(), &QOpcUaClient::stateChanged);
    int passwordRequestSpy = 0;
    connect(client.data(), &QOpcUaClient::passwordForPrivateKeyRequired, [&passwordRequestSpy, &pkiConfig](const QString &privateKeyFilePath, QString *password, bool previousTryFailed) {
        qDebug() << "Password requested";
        if (passwordRequestSpy == 0) {
            QVERIFY(password->isEmpty());
            QVERIFY(previousTryFailed == false);
        } else {
            QVERIFY(*password == QLatin1String("wrong password"));
            QVERIFY(previousTryFailed == true);
        }

        QCOMPARE(privateKeyFilePath, pkiConfig.privateKeyFile());

        if (passwordRequestSpy < 1)
            *password = "wrong password";
        else
            *password = "secret";
        ++passwordRequestSpy;
    });

    client->connectToEndpoint(endpoint);
    connectSpy.wait(signalSpyTimeout);
    if (client->state() == QOpcUaClient::Connecting)
        connectSpy.wait(signalSpyTimeout);

    QCOMPARE(connectSpy.count(), 2);
    QCOMPARE(connectSpy.at(0).at(0), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0), QOpcUaClient::Connected);

    QCOMPARE(passwordRequestSpy, 2);

    QCOMPARE(client->endpoint(), endpoint);
    QCOMPARE(client->error(), QOpcUaClient::NoError);
    qDebug() << "connected";

    connectSpy.clear();
    client->disconnectFromEndpoint();
    connectSpy.wait(signalSpyTimeout);
    QCOMPARE(connectSpy.count(), 2);
    QCOMPARE(connectSpy.at(0).at(0), QOpcUaClient::Closing);
    QCOMPARE(connectSpy.at(1).at(0), QOpcUaClient::Disconnected);
}

void Tst_QOpcUaSecurity::cleanupTestCase()
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

    Tst_QOpcUaSecurity tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "tst_security.moc"


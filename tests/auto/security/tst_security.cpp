// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtOpcUa/QOpcUaAuthenticationInformation>
#include <QtOpcUa/QOpcUaClient>
#include <QtOpcUa/QOpcUaEndpointDescription>
#include <QtOpcUa/QOpcUaProvider>
#include <QtOpcUa/QOpcUaErrorState>

#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtCore/QScopedPointer>

#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>
#include <QTcpSocket>
#include <QTcpServer>

using namespace Qt::Literals::StringLiterals;

const int signalSpyTimeout = 10000;

static QString messageSecurityModeToString(QOpcUaEndpointDescription::MessageSecurityMode msm)
{
    if (msm == QOpcUaEndpointDescription::None)
        return u"None"_s;
    else if (msm ==  QOpcUaEndpointDescription::Sign)
        return u"Sign"_s;
    else if (msm == QOpcUaEndpointDescription::SignAndEncrypt)
        return u"SignAndEncrypt"_s;
    return u"Invalid"_s;
}

#define defineDataMethod(name) void name()\
{\
    QTest::addColumn<QString>("backend");\
    QTest::addColumn<QOpcUaEndpointDescription>("endpoint");\
    for (const auto &backend : std::as_const(m_backends))\
        for (const auto &endpoint : std::as_const(m_endpoints)) { \
            const QString rowName = u"%1 using %2 %3"_s \
                    .arg(backend, endpoint.securityPolicy(), \
                         messageSecurityModeToString(endpoint.securityMode())); \
            QTest::newRow(rowName.toLatin1().constData()) << backend << endpoint; \
        } \
}

#define defineDataMethodNonePolicy(name) void name() \
{\
    QTest::addColumn<QString>("backend"); \
    QTest::addColumn<QOpcUaEndpointDescription>("endpoint"); \
    for (const auto &backend : std::as_const(m_backends)) { \
        const QString rowName = u"%1 using %2 %3"_s \
              .arg(backend, m_noneEndpoint.securityPolicy(), \
                   messageSecurityModeToString(m_noneEndpoint.securityMode())); \
        QTest::newRow(rowName.toLatin1().constData()) << backend << m_noneEndpoint; \
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

    defineDataMethodNonePolicy(connectAndDisconnectNonePolicyAnonymousUnencryptedKey_data)
    void connectAndDisconnectNonePolicyAnonymousUnencryptedKey();

    defineDataMethodNonePolicy(connectAndDisconnectNonePolicyUsernameUnencryptedKey_data)
    void connectAndDisconnectNonePolicyUsernameUnencryptedKey();

    defineDataMethod(connectAndDisconnectSecureUnencryptedKey_data)
    void connectAndDisconnectSecureUnencryptedKey();

    defineDataMethod(connectAndDisconnectSecureEncryptedKey_data)
    void connectAndDisconnectSecureEncryptedKey();

    defineDataMethod(connectAndDisconnectSecureIgnoreUntrusted_data)
    void connectAndDisconnectSecureIgnoreUntrusted();

    defineDataMethod(connectAndDisconnectSecureWithCertAuth_data)
    void connectAndDisconnectSecureWithCertAuth();

    defineDataMethod(connectAndDisconnectSecureWithCertAuthOtherCert_data)
    void connectAndDisconnectSecureWithCertAuthOtherCert();

    // Error cases

    defineDataMethod(attemptConnectToUntrustedServer_data)
    void attemptConnectToUntrustedServer();

    defineDataMethod(initializeWithBrokenCert_data)
    void initializeWithBrokenCert();

    defineDataMethod(initializeWithInvalidKeyPath_data)
    void initializeWithInvalidKeyPath();

    defineDataMethod(connectWithUnsupportedTokenPolicy_data)
    void connectWithUnsupportedTokenPolicy();

    defineDataMethod(connectCertificateAuthWithInvalidPath_data)
    void connectCertificateAuthWithInvalidPath();

private:
    QString envOrDefault(const char *env, QString def)
    {
        return qEnvironmentVariableIsSet(env) ? QString::fromUtf8(qgetenv(env).constData()) : def;
    }

    QString m_testServerPath;
    QStringList m_backends;
    QProcess m_serverProcess;
    QList<QOpcUaEndpointDescription> m_endpoints;
    QOpcUaEndpointDescription m_noneEndpoint;
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

    m_pkiData = QTest::qExtractTestData(u"pki"_s);
    qDebug() << "PKI data available at" << m_pkiData->path();

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

        m_serverProcess.start(m_testServerPath, { u"--noNonePolicyPassword"_s });
        QVERIFY2(m_serverProcess.waitForStarted(), qPrintable(m_serverProcess.errorString()));
        // Let the server come up

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

    QScopedPointer<QOpcUaClient> client(m_opcUa.createClient(m_backends.first()));
    QVERIFY2(client, "Loading backend failed");

    if (client) {
        QSignalSpy endpointSpy(client.data(), &QOpcUaClient::endpointsRequestFinished);

        client->requestEndpoints(m_discoveryEndpoint);
        endpointSpy.wait(signalSpyTimeout);
        QCOMPARE(endpointSpy.size(), 1);
        QCOMPARE(endpointSpy.at(0).at(2).value<QUrl>(), m_discoveryEndpoint);

        const auto desc = endpointSpy.at(0).at(0).value<QList<QOpcUaEndpointDescription>>();
        QVERIFY(desc.size() > 0);

        m_endpoints.clear();

        // Select first non-None security policy
        for (const auto &endpoint : std::as_const(desc)) {
            if (QOpcUa::isSecurePolicy(endpoint.securityPolicy())) {
                m_endpoints.append(endpoint);
                qDebug() << endpoint.securityPolicy();
            } else if (endpoint.securityPolicy().contains(u"None"_s)) {
                m_noneEndpoint = endpoint;
            }
        }
    }
}

void Tst_QOpcUaSecurity::connectAndDisconnectNonePolicyAnonymousUnencryptedKey()
{
    QFETCH(QString, backend);
    QFETCH(QOpcUaEndpointDescription, endpoint);

    QScopedPointer<QOpcUaClient> client(m_opcUa.createClient(backend));
    QVERIFY2(client, u"Loading backend failed: %1"_s.arg(backend).toLatin1().data());

    const QString pkidir = m_pkiData->path();
    QOpcUaPkiConfiguration pkiConfig;
    pkiConfig.setClientCertificateFile(pkidir + u"/own/certs/tst_security.der"_s);
    pkiConfig.setPrivateKeyFile(pkidir + u"/own/private/privateKeyWithoutPassword.pem"_s);
    pkiConfig.setTrustListDirectory(pkidir + u"/trusted/certs"_s);
    pkiConfig.setRevocationListDirectory(pkidir + u"/trusted/crl"_s);
    pkiConfig.setIssuerListDirectory(pkidir + u"/issuers/certs"_s);
    pkiConfig.setIssuerRevocationListDirectory(pkidir + u"/issuers/crl"_s);

    const auto identity = pkiConfig.applicationIdentity();
    QOpcUaAuthenticationInformation authInfo;
    // authInfo.setUsernameAuthentication("user1", "password");
    authInfo.setAnonymousAuthentication();

    client->setAuthenticationInformation(authInfo);
    client->setApplicationIdentity(identity);
    client->setPkiConfiguration(pkiConfig);

    qDebug() << "Testing security policy" << endpoint.securityPolicy();
    QSignalSpy connectSpy(client.data(), &QOpcUaClient::stateChanged);

    client->connectToEndpoint(endpoint);
    connectSpy.wait(signalSpyTimeout);
    if (connectSpy.size() != 2)
        connectSpy.wait(signalSpyTimeout);

    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connected);

    QCOMPARE(client->endpoint(), endpoint);
    QCOMPARE(client->error(), QOpcUaClient::NoError);

    connectSpy.clear();
    client->disconnectFromEndpoint();
    connectSpy.wait(signalSpyTimeout);
    if (connectSpy.size() != 2)
        connectSpy.wait(signalSpyTimeout);
    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Closing);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);
}

void Tst_QOpcUaSecurity::connectAndDisconnectNonePolicyUsernameUnencryptedKey()
{
    QFETCH(QString, backend);
    QFETCH(QOpcUaEndpointDescription, endpoint);

    QScopedPointer<QOpcUaClient> client(m_opcUa.createClient(backend));
    QVERIFY2(client, u"Loading backend failed: %1"_s.arg(backend).toLatin1().data());

    const QString pkidir = m_pkiData->path();
    QOpcUaPkiConfiguration pkiConfig;
    pkiConfig.setClientCertificateFile(pkidir + u"/own/certs/tst_security.der"_s);
    pkiConfig.setPrivateKeyFile(pkidir + u"/own/private/privateKeyWithoutPassword.pem"_s);
    pkiConfig.setTrustListDirectory(pkidir + u"/trusted/certs"_s);
    pkiConfig.setRevocationListDirectory(pkidir + u"/trusted/crl"_s);
    pkiConfig.setIssuerListDirectory(pkidir + u"/issuers/certs"_s);
    pkiConfig.setIssuerRevocationListDirectory(pkidir + u"/issuers/crl"_s);

    const auto identity = pkiConfig.applicationIdentity();
    QOpcUaAuthenticationInformation authInfo;
    authInfo.setUsernameAuthentication(u"user1"_s, u"password"_s);

    client->setAuthenticationInformation(authInfo);
    client->setApplicationIdentity(identity);
    client->setPkiConfiguration(pkiConfig);

    QSignalSpy connectSpy(client.data(), &QOpcUaClient::stateChanged);

    client->connectToEndpoint(endpoint);
    connectSpy.wait(signalSpyTimeout);
    if (connectSpy.size() != 2)
        connectSpy.wait(signalSpyTimeout);

    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connected);

    QCOMPARE(client->endpoint(), endpoint);
    QCOMPARE(client->error(), QOpcUaClient::NoError);

    connectSpy.clear();
    client->disconnectFromEndpoint();
    connectSpy.wait(signalSpyTimeout);
    if (connectSpy.size() != 2)
        connectSpy.wait(signalSpyTimeout);
    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Closing);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);
}

void Tst_QOpcUaSecurity::connectAndDisconnectSecureUnencryptedKey()
{
    if (m_endpoints.size() == 0)
        QSKIP("No secure endpoints available");

    QFETCH(QString, backend);
    QFETCH(QOpcUaEndpointDescription, endpoint);

    QScopedPointer<QOpcUaClient> client(m_opcUa.createClient(backend));
    QVERIFY2(client, u"Loading backend failed: %1"_s.arg(backend).toLatin1().data());

    if (!client->supportedSecurityPolicies().contains(endpoint.securityPolicy())) {
        QSKIP(u"This test is skipped because backend %1 "
              "does not support security policy %2"_s
              .arg(client->backend(), endpoint.securityPolicy()).toLatin1().constData());
    }

    const QString pkidir = m_pkiData->path();
    QOpcUaPkiConfiguration pkiConfig;
    pkiConfig.setClientCertificateFile(pkidir + u"/own/certs/tst_security.der"_s);
    pkiConfig.setPrivateKeyFile(pkidir + u"/own/private/privateKeyWithoutPassword.pem"_s);
    pkiConfig.setTrustListDirectory(pkidir + u"/trusted/certs"_s);
    pkiConfig.setRevocationListDirectory(pkidir + u"/trusted/crl"_s);
    pkiConfig.setIssuerListDirectory(pkidir + u"/issuers/certs"_s);
    pkiConfig.setIssuerRevocationListDirectory(pkidir + u"/issuers/crl"_s);

    const auto identity = pkiConfig.applicationIdentity();
    QOpcUaAuthenticationInformation authInfo;
    authInfo.setUsernameAuthentication(u"user1"_s, u"password"_s);

    client->setAuthenticationInformation(authInfo);
    client->setApplicationIdentity(identity);
    client->setPkiConfiguration(pkiConfig);

    qDebug() << "Testing security policy" << endpoint.securityPolicy();
    QSignalSpy connectSpy(client.data(), &QOpcUaClient::stateChanged);
    int passwordRequestSpy = 0;
    connect(client.data(), &QOpcUaClient::passwordForPrivateKeyRequired,
            this, [&passwordRequestSpy](const QString &privateKeyFilePath, QString *password,
                                        bool previousTryFailed) {
        Q_UNUSED(privateKeyFilePath);
        Q_UNUSED(previousTryFailed);
        Q_UNUSED(password);
        ++passwordRequestSpy;
    });

    client->connectToEndpoint(endpoint);
    connectSpy.wait(signalSpyTimeout);
    if (connectSpy.size() != 2)
        connectSpy.wait(signalSpyTimeout);

    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connected);

    QCOMPARE(passwordRequestSpy, 0);

    QCOMPARE(client->endpoint(), endpoint);
    QCOMPARE(client->error(), QOpcUaClient::NoError);
    qDebug() << "connected";

    connectSpy.clear();
    client->disconnectFromEndpoint();
    connectSpy.wait(signalSpyTimeout);
    if (connectSpy.size() != 2)
        connectSpy.wait(signalSpyTimeout);
    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Closing);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);
}

void Tst_QOpcUaSecurity::connectAndDisconnectSecureEncryptedKey()
{
    if (m_endpoints.size() == 0)
        QSKIP("No secure endpoints available");

    QFETCH(QString, backend);
    QFETCH(QOpcUaEndpointDescription, endpoint);

    QScopedPointer<QOpcUaClient> client(m_opcUa.createClient(backend));
    QVERIFY2(client, u"Loading backend failed: %1"_s.arg(backend).toLatin1().data());

    if (!client->supportedSecurityPolicies().contains(endpoint.securityPolicy())) {
        QSKIP(u"This test is skipped because backend %1 "
              "does not support security policy %2"_s
              .arg(client->backend(), endpoint.securityPolicy()).toLatin1().constData());
    }

    const QString pkidir = m_pkiData->path();
    QOpcUaPkiConfiguration pkiConfig;
    pkiConfig.setClientCertificateFile(pkidir + u"/own/certs/tst_security.der"_s);
    pkiConfig.setPrivateKeyFile(pkidir + u"/own/private/privateKeyWithPassword_secret.pem"_s);
    pkiConfig.setTrustListDirectory(pkidir + u"/trusted/certs"_s);
    pkiConfig.setRevocationListDirectory(pkidir + u"/trusted/crl"_s);
    pkiConfig.setIssuerListDirectory(pkidir + u"/issuers/certs"_s);
    pkiConfig.setIssuerRevocationListDirectory(pkidir + u"/issuers/crl"_s);

    const auto identity = pkiConfig.applicationIdentity();
    QOpcUaAuthenticationInformation authInfo;
    authInfo.setUsernameAuthentication(u"user1"_s, u"password"_s);

    client->setAuthenticationInformation(authInfo);
    client->setApplicationIdentity(identity);
    client->setPkiConfiguration(pkiConfig);

    qDebug() << "Testing security policy" << endpoint.securityPolicy();
    QSignalSpy connectSpy(client.data(), &QOpcUaClient::stateChanged);
    int passwordRequestSpy = 0;
    connect(client.data(), &QOpcUaClient::passwordForPrivateKeyRequired,
            this, [&passwordRequestSpy, &pkiConfig](const QString &privateKeyFilePath,
                                                    QString *password, bool previousTryFailed) {
        qDebug() << "Password requested";
        if (passwordRequestSpy == 0) {
            QVERIFY(password->isEmpty());
            QVERIFY(previousTryFailed == false);
        } else {
            QVERIFY(*password == "wrong password"_L1);
            QVERIFY(previousTryFailed == true);
        }

        QCOMPARE(privateKeyFilePath, pkiConfig.privateKeyFile());

        if (passwordRequestSpy < 1)
            *password = u"wrong password"_s;
        else
            *password = u"secret"_s;
        ++passwordRequestSpy;
    });

    client->connectToEndpoint(endpoint);
    connectSpy.wait(signalSpyTimeout);
    if (connectSpy.size() != 2)
        connectSpy.wait(signalSpyTimeout);

    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0), QOpcUaClient::Connected);

    QCOMPARE(passwordRequestSpy, 2);

    QCOMPARE(client->endpoint(), endpoint);
    QCOMPARE(client->error(), QOpcUaClient::NoError);
    qDebug() << "connected";

    connectSpy.clear();
    client->disconnectFromEndpoint();
    connectSpy.wait(signalSpyTimeout);
    // Sometimes, the first wait() only gets the first signal.
    // Make the wait conditional because an unconditional second
    // wait() would block for the full time if there is no signal.
    if (connectSpy.size() != 2)
        connectSpy.wait(signalSpyTimeout);
    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0), QOpcUaClient::Closing);
    QCOMPARE(connectSpy.at(1).at(0), QOpcUaClient::Disconnected);
}

void Tst_QOpcUaSecurity::connectAndDisconnectSecureIgnoreUntrusted()
{
    if (m_endpoints.size() == 0)
        QSKIP("No secure endpoints available");

    QFETCH(QString, backend);
    QFETCH(QOpcUaEndpointDescription, endpoint);

    QScopedPointer<QOpcUaClient> client(m_opcUa.createClient(backend));
    QVERIFY2(client, u"Loading backend failed: %1"_s.arg(backend).toLatin1().data());

    if (!client->supportedSecurityPolicies().contains(endpoint.securityPolicy())) {
        QSKIP(u"This test is skipped because backend %1 "
              "does not support security policy %2"_s
                  .arg(client->backend(), endpoint.securityPolicy()).toLatin1().constData());
    }

    // Initialize a PKI with only certificate and private key paths
    // This will make the connection fail with BadCertificateUntrusted
    const QString pkidir = m_pkiData->path();
    QOpcUaPkiConfiguration pkiConfig;
    pkiConfig.setClientCertificateFile(pkidir + u"/own/certs/tst_security.der"_s);
    pkiConfig.setPrivateKeyFile(pkidir + u"/own/private/privateKeyWithoutPassword.pem"_s);

    const auto identity = pkiConfig.applicationIdentity();
    QOpcUaAuthenticationInformation authInfo;
    authInfo.setUsernameAuthentication(u"user1"_s, u"password"_s);

    client->setAuthenticationInformation(authInfo);
    client->setApplicationIdentity(identity);
    client->setPkiConfiguration(pkiConfig);

    qDebug() << "Testing security policy" << endpoint.securityPolicy();
    QSignalSpy connectSpy(client.data(), &QOpcUaClient::stateChanged);
    QSignalSpy errorSpy(client.data(), &QOpcUaClient::errorChanged);

    bool errorStateReported = false;

    connect(client.data(), &QOpcUaClient::connectError,
            this, [&](QOpcUaErrorState *errorState) {
                if (errorState->connectionStep() == QOpcUaErrorState::ConnectionStep::CertificateValidation) {
                    errorState->setIgnoreError(true);
                    errorStateReported = true;
                }
            });

    client->connectToEndpoint(endpoint);
    connectSpy.wait(signalSpyTimeout);
    if (client->state() == QOpcUaClient::Connecting)
        connectSpy.wait(signalSpyTimeout);

    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connected);

    QVERIFY(errorStateReported);

    errorSpy.wait(std::chrono::milliseconds(100));
    QVERIFY(errorSpy.empty());

    connectSpy.clear();
    client->disconnectFromEndpoint();
    connectSpy.wait(signalSpyTimeout);
    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Closing);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);
}

void Tst_QOpcUaSecurity::connectAndDisconnectSecureWithCertAuth()
{
    if (m_endpoints.size() == 0)
        QSKIP("No secure endpoints available");

    QFETCH(QString, backend);
    QFETCH(QOpcUaEndpointDescription, endpoint);

    QScopedPointer<QOpcUaClient> client(m_opcUa.createClient(backend));
    QVERIFY2(client, u"Loading backend failed: %1"_s.arg(backend).toLatin1().data());

    if (!client->supportedSecurityPolicies().contains(endpoint.securityPolicy())) {
        QSKIP(u"This test is skipped because backend %1 "
              "does not support security policy %2"_s
                  .arg(client->backend(), endpoint.securityPolicy()).toLatin1().constData());
    }

    const QString pkidir = m_pkiData->path();
    QOpcUaPkiConfiguration pkiConfig;
    pkiConfig.setClientCertificateFile(pkidir + u"/own/certs/tst_security.der"_s);
    pkiConfig.setPrivateKeyFile(pkidir + u"/own/private/privateKeyWithoutPassword.pem"_s);
    pkiConfig.setTrustListDirectory(pkidir + u"/trusted/certs"_s);
    pkiConfig.setRevocationListDirectory(pkidir + u"/trusted/crl"_s);
    pkiConfig.setIssuerListDirectory(pkidir + u"/issuers/certs"_s);
    pkiConfig.setIssuerRevocationListDirectory(pkidir + u"/issuers/crl"_s);

    const auto identity = pkiConfig.applicationIdentity();
    QOpcUaAuthenticationInformation authInfo;
    authInfo.setCertificateAuthentication();

    client->setAuthenticationInformation(authInfo);
    client->setApplicationIdentity(identity);
    client->setPkiConfiguration(pkiConfig);

    qDebug() << "Testing security policy" << endpoint.securityPolicy();
    QSignalSpy connectSpy(client.data(), &QOpcUaClient::stateChanged);

    client->connectToEndpoint(endpoint);
    connectSpy.wait(signalSpyTimeout);
    if (client->state() == QOpcUaClient::Connecting)
        connectSpy.wait(signalSpyTimeout);

    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connected);

    connectSpy.clear();
    client->disconnectFromEndpoint();
    connectSpy.wait(signalSpyTimeout);
    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Closing);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);
}

void Tst_QOpcUaSecurity::connectAndDisconnectSecureWithCertAuthOtherCert()
{
    if (m_endpoints.size() == 0)
        QSKIP("No secure endpoints available");

    QFETCH(QString, backend);
    QFETCH(QOpcUaEndpointDescription, endpoint);

    if (!endpoint.securityPolicy().contains("Basic256Sha256"_L1))
        return;

    QScopedPointer<QOpcUaClient> client(m_opcUa.createClient(backend));
    QVERIFY2(client, u"Loading backend failed: %1"_s.arg(backend).toLatin1().data());

    if (!client->supportedSecurityPolicies().contains(endpoint.securityPolicy())) {
        QSKIP(u"This test is skipped because backend %1 "
               "does not support security policy %2"_s
               .arg(client->backend(), endpoint.securityPolicy()).toLatin1().constData());
    }

    const QString pkidir = m_pkiData->path();
    QOpcUaPkiConfiguration pkiConfig;
    pkiConfig.setClientCertificateFile(pkidir + u"/own/certs/tst_security.der"_s);
    pkiConfig.setPrivateKeyFile(pkidir + u"/own/private/privateKeyWithoutPassword.pem"_s);
    pkiConfig.setTrustListDirectory(pkidir + u"/trusted/certs"_s);
    pkiConfig.setRevocationListDirectory(pkidir + u"/trusted/crl"_s);
    pkiConfig.setIssuerListDirectory(pkidir + u"/issuers/certs"_s);
    pkiConfig.setIssuerRevocationListDirectory(pkidir + u"/issuers/crl"_s);

    const auto identity = pkiConfig.applicationIdentity();
    QOpcUaAuthenticationInformation authInfo;
    authInfo.setCertificateAuthentication(pkidir + u"/own/certs/tst_security.der"_s,
                                          pkidir + u"/own/private/privateKeyWithoutPassword.pem"_s);

    client->setAuthenticationInformation(authInfo);
    client->setApplicationIdentity(identity);
    client->setPkiConfiguration(pkiConfig);

    qDebug() << "Testing security policy" << endpoint.securityPolicy();
    QSignalSpy connectSpy(client.data(), &QOpcUaClient::stateChanged);

    client->connectToEndpoint(endpoint);
    connectSpy.wait(signalSpyTimeout);
    if (client->state() == QOpcUaClient::Connecting)
        connectSpy.wait(signalSpyTimeout);

    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connected);

    connectSpy.clear();
    client->disconnectFromEndpoint();
    connectSpy.wait(signalSpyTimeout);
    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Closing);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);
}

void Tst_QOpcUaSecurity::attemptConnectToUntrustedServer()
{
    if (m_endpoints.size() == 0)
        QSKIP("No secure endpoints available");

    QFETCH(QString, backend);
    QFETCH(QOpcUaEndpointDescription, endpoint);

    QScopedPointer<QOpcUaClient> client(m_opcUa.createClient(backend));
    QVERIFY2(client, "Loading backend failed: %1"_L1.arg(backend).toLatin1().data());

    if (!client->supportedSecurityPolicies().contains(endpoint.securityPolicy())) {
        QSKIP("This test is skipped because backend %1 "
              "does not support security policy %2"_L1
                  .arg(client->backend(), endpoint.securityPolicy()).toLatin1().constData());
    }

    // Initialize a PKI with only certificate and private key paths
    // This will make the connection fail with BadCertificateUntrusted
    const QString pkidir = m_pkiData->path();
    QOpcUaPkiConfiguration pkiConfig;
    pkiConfig.setClientCertificateFile(pkidir + "/own/certs/tst_security.der"_L1);
    pkiConfig.setPrivateKeyFile(pkidir + "/own/private/privateKeyWithoutPassword.pem"_L1);

    const auto identity = pkiConfig.applicationIdentity();
    QOpcUaAuthenticationInformation authInfo;
    authInfo.setUsernameAuthentication("user1"_L1, "password"_L1);

    client->setAuthenticationInformation(authInfo);
    client->setApplicationIdentity(identity);
    client->setPkiConfiguration(pkiConfig);

    qDebug() << "Testing security policy" << endpoint.securityPolicy();
    QSignalSpy connectSpy(client.data(), &QOpcUaClient::stateChanged);
    QSignalSpy errorSpy(client.data(), &QOpcUaClient::errorChanged);

    bool errorStateReported = false;

    // Register the state callback, but don't ignore the error
    connect(client.data(), &QOpcUaClient::connectError,
            this, [&](QOpcUaErrorState *errorState) {
                if (errorState->connectionStep() == QOpcUaErrorState::ConnectionStep::CertificateValidation) {
                    errorState->setIgnoreError(false);
                    errorStateReported = true;
                }
            });

    client->connectToEndpoint(endpoint);
    connectSpy.wait(signalSpyTimeout);
    if (connectSpy.size() != 2)
        QVERIFY(connectSpy.wait());

    if (errorSpy.isEmpty())
        QVERIFY(errorSpy.wait());

    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);

    QVERIFY(errorStateReported);
    QCOMPARE(errorSpy.at(0).at(0), QOpcUaClient::ClientError::CertificateUntrusted);
    QCOMPARE(client->error(), QOpcUaClient::ClientError::CertificateUntrusted);
}

void Tst_QOpcUaSecurity::initializeWithBrokenCert()
{
    if (m_endpoints.size() == 0)
        QSKIP("No secure endpoints available");

    QFETCH(QString, backend);
    QFETCH(QOpcUaEndpointDescription, endpoint);

    QScopedPointer<QOpcUaClient> client(m_opcUa.createClient(backend));
    QVERIFY2(client, u"Loading backend failed: %1"_s.arg(backend).toUtf8().data());

    if (!client->supportedSecurityPolicies().contains(endpoint.securityPolicy())) {
        QSKIP(u"This test is skipped because backend %1 "
              "does not support security policy %2"_s
                  .arg(client->backend(), endpoint.securityPolicy()).toUtf8().constData());
    }

    const QString pkidir = m_pkiData->path();
    QOpcUaPkiConfiguration pkiConfig;
    pkiConfig.setClientCertificateFile(pkidir + "/own/certs/invalid_cert.der"_L1);
    pkiConfig.setPrivateKeyFile(pkidir + "/own/private/privateKeyWithoutPassword.pem"_L1);
    pkiConfig.setTrustListDirectory(pkidir + "/trusted/certs"_L1);
    pkiConfig.setRevocationListDirectory(pkidir + "/trusted/crl"_L1);
    pkiConfig.setIssuerListDirectory(pkidir + "/issuers/certs"_L1);
    pkiConfig.setIssuerRevocationListDirectory(pkidir + "/issuers/crl"_L1);

    client->setPkiConfiguration(pkiConfig);

    qDebug() << "Testing invalid certificate load with security policy" << endpoint.securityPolicy();
    QSignalSpy connectSpy(client.data(), &QOpcUaClient::stateChanged);
    QSignalSpy errorSpy(client.data(), &QOpcUaClient::errorChanged);

    client->connectToEndpoint(endpoint);
    connectSpy.wait(signalSpyTimeout);
    if (client->state() == QOpcUaClient::Connecting)
        connectSpy.wait(signalSpyTimeout);

    if (errorSpy.isEmpty())
        QVERIFY(errorSpy.wait());

    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);

    QCOMPARE(errorSpy.at(0).at(0), QOpcUaClient::ClientError::InvalidPki);
    QCOMPARE(client->error(), QOpcUaClient::ClientError::InvalidPki);
}

void Tst_QOpcUaSecurity::initializeWithInvalidKeyPath()
{
    if (m_endpoints.size() == 0)
        QSKIP("No secure endpoints available");

    QFETCH(QString, backend);
    QFETCH(QOpcUaEndpointDescription, endpoint);

    QScopedPointer<QOpcUaClient> client(m_opcUa.createClient(backend));
    QVERIFY2(client, u"Loading backend failed: %1"_s.arg(backend).toLatin1().data());

    if (!client->supportedSecurityPolicies().contains(endpoint.securityPolicy())) {
        QSKIP(u"This test is skipped because backend %1 "
              "does not support security policy %2"_s
                  .arg(client->backend(), endpoint.securityPolicy()).toLatin1().constData());
    }

    const QString pkidir = m_pkiData->path();
    QOpcUaPkiConfiguration pkiConfig;
    pkiConfig.setClientCertificateFile(pkidir + "/own/certs/tst_security.der"_L1);
    pkiConfig.setPrivateKeyFile(pkidir + "/own/private/idonotexist.pem"_L1);

    const auto identity = pkiConfig.applicationIdentity();
    client->setApplicationIdentity(identity);
    client->setPkiConfiguration(pkiConfig);

    qDebug() << "Testing invalid private key path with security policy" << endpoint.securityPolicy();
    QSignalSpy connectSpy(client.data(), &QOpcUaClient::stateChanged);
    QSignalSpy errorSpy(client.data(), &QOpcUaClient::errorChanged);

    client->connectToEndpoint(endpoint);
    connectSpy.wait(signalSpyTimeout);
    if (connectSpy.size() != 2)
        QVERIFY(connectSpy.wait(signalSpyTimeout));

    if (errorSpy.isEmpty())
        QVERIFY(errorSpy.wait());

    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);

    QCOMPARE(errorSpy.at(0).at(0), QOpcUaClient::ClientError::InvalidPki);
    QCOMPARE(client->error(), QOpcUaClient::ClientError::InvalidPki);
}

void Tst_QOpcUaSecurity::connectWithUnsupportedTokenPolicy()
{
    if (m_endpoints.size() == 0)
        QSKIP("No secure endpoints available");

    QFETCH(QString, backend);
    QFETCH(QOpcUaEndpointDescription, endpoint);

    QScopedPointer<QOpcUaClient> client(m_opcUa.createClient(backend));
    QVERIFY2(client, u"Loading backend failed: %1"_s.arg(backend).toLatin1().data());

    if (!client->supportedSecurityPolicies().contains(endpoint.securityPolicy())) {
        QSKIP(u"This test is skipped because backend %1 "
              "does not support security policy %2"_s
                  .arg(client->backend(), endpoint.securityPolicy()).toLatin1().constData());
    }

    QOpcUaUserTokenPolicy pol;
    pol.setTokenType(QOpcUaUserTokenPolicy::Username);
    pol.setSecurityPolicy("http://idonotexist"_L1);
    endpoint.setUserIdentityTokens({ pol });

    const QString pkidir = m_pkiData->path();
    QOpcUaPkiConfiguration pkiConfig;
    pkiConfig.setClientCertificateFile(pkidir + "/own/certs/tst_security.der"_L1);
    pkiConfig.setPrivateKeyFile(pkidir + "/own/private/privateKeyWithoutPassword.pem"_L1);

    QOpcUaAuthenticationInformation authInfo;
    authInfo.setUsernameAuthentication("user1"_L1, "password"_L1);
    client->setAuthenticationInformation(authInfo);

    const auto identity = pkiConfig.applicationIdentity();
    client->setApplicationIdentity(identity);
    client->setPkiConfiguration(pkiConfig);

    qDebug() << "Testing invalid user token security policy with secure channel security policy" << endpoint.securityPolicy();
    QSignalSpy connectSpy(client.data(), &QOpcUaClient::stateChanged);
    QSignalSpy errorSpy(client.data(), &QOpcUaClient::errorChanged);

    client->connectToEndpoint(endpoint);
    connectSpy.wait(signalSpyTimeout);
    if (connectSpy.size() != 2)
        QVERIFY(connectSpy.wait(signalSpyTimeout));

    if (errorSpy.isEmpty())
        QVERIFY(errorSpy.wait());

    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);

    QCOMPARE(errorSpy.at(0).at(0), QOpcUaClient::ClientError::NoMatchingUserIdentityTokenFound);
    QCOMPARE(client->error(), QOpcUaClient::ClientError::NoMatchingUserIdentityTokenFound);
}

void Tst_QOpcUaSecurity::connectCertificateAuthWithInvalidPath()
{
    if (m_endpoints.size() == 0)
        QSKIP("No secure endpoints available");

    QFETCH(QString, backend);
    QFETCH(QOpcUaEndpointDescription, endpoint);

    QScopedPointer<QOpcUaClient> client(m_opcUa.createClient(backend));
    QVERIFY2(client, u"Loading backend failed: %1"_s.arg(backend).toLatin1().data());

    if (!client->supportedSecurityPolicies().contains(endpoint.securityPolicy())) {
        QSKIP(u"This test is skipped because backend %1 "
              "does not support security policy %2"_s
                  .arg(client->backend(), endpoint.securityPolicy()).toLatin1().constData());
    }

    const QString pkidir = m_pkiData->path();
    QOpcUaPkiConfiguration pkiConfig;
    pkiConfig.setClientCertificateFile(pkidir + "/own/certs/tst_security.der"_L1);
    pkiConfig.setPrivateKeyFile(pkidir + "/own/private/privateKeyWithoutPassword.pem"_L1);
    pkiConfig.setTrustListDirectory(pkidir + "/trusted/certs"_L1);
    pkiConfig.setRevocationListDirectory(pkidir + "/trusted/crl"_L1);
    pkiConfig.setIssuerListDirectory(pkidir + "/issuers/certs"_L1);
    pkiConfig.setIssuerRevocationListDirectory(pkidir + "/issuers/crl"_L1);

    const auto identity = pkiConfig.applicationIdentity();
    QOpcUaAuthenticationInformation authInfo;
    authInfo.setCertificateAuthentication(pkidir + "/own/certs/idontexist.der"_L1,
                                          pkidir + "/own/private/idonotexist.pem"_L1);

    client->setAuthenticationInformation(authInfo);
    client->setApplicationIdentity(identity);
    client->setPkiConfiguration(pkiConfig);

    qDebug() << "Testing security policy" << endpoint.securityPolicy();
    QSignalSpy connectSpy(client.data(), &QOpcUaClient::stateChanged);
    QSignalSpy errorSpy(client.data(), &QOpcUaClient::errorChanged);

    client->connectToEndpoint(endpoint);
    connectSpy.wait(signalSpyTimeout);
    if (connectSpy.size() != 2)
        QVERIFY(connectSpy.wait(signalSpyTimeout));

    if (errorSpy.empty())
        QVERIFY(errorSpy.wait());

    QCOMPARE(connectSpy.size(), 2);
    QCOMPARE(connectSpy.at(0).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Connecting);
    QCOMPARE(connectSpy.at(1).at(0).value<QOpcUaClient::ClientState>(), QOpcUaClient::Disconnected);

    QCOMPARE(errorSpy.at(0).at(0), QOpcUaClient::ClientError::InvalidAuthenticationInformation);
    QCOMPARE(client->error(), QOpcUaClient::ClientError::InvalidAuthenticationInformation);
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


// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtOpcUa/QOpcUaProvider>
#include <QtOpcUa/QOpcUaGdsClient>
#include <QtOpcUa/QOpcUaApplicationIdentity>
#include <QtOpcUa/QOpcUaPkiConfiguration>
#include <QtOpcUa/QOpcUaAuthenticationInformation>
#include <QtOpcUa/QOpcUaEndpointDescription>
#include <QtOpcUa/QOpcUaApplicationRecordDataType>
#include <QtOpcUa/QOpcUaClient>
#include <QtOpcUa/QOpcUaX509DistinguishedName>

#include <QtCore/QCoreApplication>
#include <QtCore/QScopedPointer>
#include <QHostInfo>

#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>

#define defineDataMethod(name) void name()\
{\
    QTest::addColumn<QString>("backend");\
    for (const QString &backend : m_backends) {\
        QTest::newRow(backend.toLatin1().constData()) << backend ; \
        QVERIFY(!backend.isEmpty()); \
    }\
}

static QOpcUaPkiConfiguration getPkiConfig()
{
    QOpcUaPkiConfiguration pkiConfig;
    const QString pkiDir = QCoreApplication::applicationDirPath() + "/pki";

    pkiConfig.setClientCertificateFile(pkiDir + "/own/certs/application.der");
    pkiConfig.setPrivateKeyFile(pkiDir + "/own/private/application.pem");
    pkiConfig.setTrustListDirectory(pkiDir + "/trusted/certs");
    pkiConfig.setRevocationListDirectory(pkiDir + "/trusted/crl");
    pkiConfig.setIssuerListDirectory(pkiDir + "/issuers/certs");
    pkiConfig.setIssuerRevocationListDirectory(pkiDir + "/issuers/crl");
    return pkiConfig;
}

static QOpcUaApplicationIdentity getAppIdentity()
{
    QOpcUaApplicationIdentity identity;

    const QString applicationUri = QStringLiteral("urn:%1:%2:%3")
            .arg(QHostInfo::localHostName())
            .arg(QCoreApplication::organizationName())
            .arg(QCoreApplication::applicationName());
    const QString productUri = QStringLiteral("urn:%1:%2")
            .arg(QCoreApplication::organizationName())
            .arg(QCoreApplication::applicationName());

    identity.setProductUri(productUri);
    identity.setApplicationUri(applicationUri);
    identity.setApplicationName(QCoreApplication::applicationName());
    identity.setApplicationType(QOpcUaApplicationDescription::Client);
    return identity;
}

static bool removeSettingsFile()
{
    QSettings settings;
    QFile file(settings.fileName());
    if (file.exists())
        return file.remove();
    return true;
}

static void provideCredentials(QOpcUaAuthenticationInformation &authInfo)
{
    qDebug() << "Authentication credentials provided";
    authInfo.setUsernameAuthentication("root", "secret");
}

static void commonGdsClientSetup(QOpcUaGdsClient &gc, const QString &backend, const QOpcUaEndpointDescription endpoint)
{
    QObject::connect(&gc, &QOpcUaGdsClient::authenticationRequired, &gc, provideCredentials);

    gc.setBackend(backend);
    gc.setEndpoint(endpoint);
    gc.setApplicationIdentity(getAppIdentity());
    gc.setPkiConfiguration(getPkiConfig());

    QOpcUaApplicationRecordDataType ar = gc.applicationRecord();
    ar.setApplicationNames(QList<QOpcUaLocalizedText>{QOpcUaLocalizedText("en",  gc.applicationIdentity().applicationName())});
    ar.setApplicationType(gc.applicationIdentity().applicationType());
    ar.setApplicationUri(gc.applicationIdentity().applicationUri());
    ar.setProductUri(gc.applicationIdentity().productUri());
    ar.setDiscoveryUrls(QList<QString>{QLatin1String("opc.tcp://localhost")});
    gc.setApplicationRecord(ar);

    QOpcUaX509DistinguishedName dn;
    dn.setEntry(QOpcUaX509DistinguishedName::Type::CountryName, QLatin1String("DE"));
    dn.setEntry(QOpcUaX509DistinguishedName::Type::LocalityName, QLatin1String("Berlin"));
    dn.setEntry(QOpcUaX509DistinguishedName::Type::StateOrProvinceName, QLatin1String("Berlin"));
    dn.setEntry(QOpcUaX509DistinguishedName::Type::OrganizationName, QLatin1String("The Qt Company"));
    gc.setCertificateSigningRequestPresets(dn, QLatin1String("foo.com"));
}

// The tests have are depending on each other and have to be executed in order.

class Tst_QOpcUaGds: public QObject
{
    Q_OBJECT

public:
    Tst_QOpcUaGds();

private slots:
    void initTestCase();
    void cleanupTestCase();

    defineDataMethod(registerApplication_data)
    void registerApplication();
    defineDataMethod(reuseApplicationId_data)
    void reuseApplicationId();
    defineDataMethod(reuseRegisteredUri_data)
    void reuseRegisteredUri();
    defineDataMethod(serverForgotRegistration_data)
    void serverForgotRegistration();
private:
    QStringList m_backends;
    QOpcUaEndpointDescription m_endpoint;
};

Tst_QOpcUaGds::Tst_QOpcUaGds()
{
    m_backends = QOpcUaProvider::availableBackends();
}

void Tst_QOpcUaGds::initTestCase()
{
    QOpcUaEndpointDescription endpoint;
    QOpcUaProvider provider;

    QScopedPointer<QOpcUaClient> client(provider.createClient("open62541"));
    QVERIFY(!client.isNull());
    QSignalSpy endpointSpy(client.data(), &QOpcUaClient::endpointsRequestFinished);
    client->requestEndpoints(QUrl("opc.tcp://172.17.0.1:48060"));
    endpointSpy.wait(2000);
    QCOMPARE(endpointSpy.size(), 1);

    const auto desc = endpointSpy.at(0).at(0).value<QList<QOpcUaEndpointDescription>>();
    QVERIFY(desc.size() > 0);

    for (const auto &i : std::as_const(desc)) {
        if (i.securityPolicy().endsWith("#Basic256Sha256")
            && i.securityMode() == QOpcUaEndpointDescription::MessageSecurityMode::SignAndEncrypt)
            m_endpoint = i;
    }

    QVERIFY(m_endpoint.securityPolicy().endsWith("#Basic256Sha256"));
}

void Tst_QOpcUaGds::registerApplication()
{
    QFETCH(QString, backend);
    if (backend == "open62541")
        QSKIP("Skipping open62541");

    QVERIFY(removeSettingsFile());

    QOpcUaGdsClient gc;
    commonGdsClientSetup(gc, backend, m_endpoint);

    QSignalSpy registeredSpy(&gc, &QOpcUaGdsClient::applicationRegistered);
    QSignalSpy certificateGroupsSpy(&gc, &QOpcUaGdsClient::certificateGroupsReceived);
    QSignalSpy certificateStatusSpy(&gc, &QOpcUaGdsClient::certificateUpdateRequired);
    QSignalSpy certificateUpdatedSpy(&gc, &QOpcUaGdsClient::certificateUpdated);
    QSignalSpy trustListUpdatedSpy(&gc, &QOpcUaGdsClient::trustListUpdated);

    gc.start();

    QVERIFY(certificateGroupsSpy.wait());
    QVERIFY(!certificateGroupsSpy.at(0).at(0).value<QStringList>().isEmpty());
    QVERIFY(registeredSpy.wait());

    if (certificateStatusSpy.isEmpty())
        certificateStatusSpy.wait();

    if (certificateUpdatedSpy.isEmpty())
        certificateUpdatedSpy.wait();

    if (trustListUpdatedSpy.isEmpty())
        trustListUpdatedSpy.wait();

    QVERIFY(!certificateStatusSpy.isEmpty());
    QVERIFY(!certificateUpdatedSpy.isEmpty());
    QVERIFY(!trustListUpdatedSpy.isEmpty());
}

void Tst_QOpcUaGds::reuseApplicationId()
{
    QFETCH(QString, backend);
    if (backend == "open62541")
        QSKIP("Skipping open62541");

    // Keep settings file in order to reuse the existing registration

    QOpcUaGdsClient gc;
    commonGdsClientSetup(gc, backend, m_endpoint);

    QSignalSpy registeredSpy(&gc, &QOpcUaGdsClient::applicationRegistered);
    QSignalSpy certificateGroupsSpy(&gc, &QOpcUaGdsClient::certificateGroupsReceived);

    gc.start();

    QVERIFY(certificateGroupsSpy.wait());
    QVERIFY(!certificateGroupsSpy.at(0).at(0).value<QStringList>().isEmpty());
    QVERIFY(registeredSpy.wait());
}

void Tst_QOpcUaGds::reuseRegisteredUri()
{
    // This test assumes that a registratin at the server is still present.
    // It will remove the information locally and has to reuse the
    // registration from the server.

    QFETCH(QString, backend);
    if (backend == "open62541")
        QSKIP("Skipping open62541");

    // Remove the registration data locally; the server will keep it.
    QSettings settings("Unknown Organization", "tst_gds");

    QString applicationId = settings.value(QLatin1String("gds/applicationId")).toString();
    settings.remove(QLatin1String("gds/applicationId"));
    settings.sync();

    QVERIFY(removeSettingsFile());

    QOpcUaGdsClient gc;
    commonGdsClientSetup(gc, backend, m_endpoint);

    QSignalSpy registeredSpy(&gc, &QOpcUaGdsClient::applicationRegistered);
    QSignalSpy certificateGroupsSpy(&gc, &QOpcUaGdsClient::certificateGroupsReceived);

    gc.start();

    QVERIFY(certificateGroupsSpy.wait());
    QVERIFY(!certificateGroupsSpy.at(0).at(0).value<QStringList>().isEmpty());
    QVERIFY(registeredSpy.wait());
    QVERIFY(gc.applicationId().size() > 10);
    if (!applicationId.isEmpty())
        QVERIFY(gc.applicationId() == applicationId);
}

void Tst_QOpcUaGds::serverForgotRegistration()
{
    QFETCH(QString, backend);
    if (backend == "open62541")
        QSKIP("Skipping open62541");

    // Setting an invalid application ID simulates a previous registration which has been
    // forgotten by the server
    QSettings settings("Unknown Organization", "tst_gds");
    settings.setValue(QLatin1String("gds/applicationId"), "ns=42;i=1111111");
    settings.sync();

    QOpcUaGdsClient gc;
    commonGdsClientSetup(gc, backend, m_endpoint);

    QSignalSpy registeredSpy(&gc, &QOpcUaGdsClient::applicationRegistered);
    QSignalSpy certificateGroupsSpy(&gc, &QOpcUaGdsClient::certificateGroupsReceived);

    gc.start();

    QVERIFY(certificateGroupsSpy.wait());
    QVERIFY(!certificateGroupsSpy.at(0).at(0).value<QStringList>().isEmpty());
    QVERIFY(registeredSpy.wait());
}

void Tst_QOpcUaGds::cleanupTestCase()
{
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QTEST_SET_MAIN_SOURCE_PATH

    if (QOpcUaProvider::availableBackends().empty()) {
        qDebug("No OPCUA backends found, skipping tests.");
        return EXIT_SUCCESS;
    }

    Tst_QOpcUaGds tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "tst_gds.moc"

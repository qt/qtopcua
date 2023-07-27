// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtOpcUa/QOpcUaProvider>
#include <QtOpcUa/QOpcUaKeyPair>

#include <QtCore/QCoreApplication>
#include <QtCore/QScopedPointer>
#include <QOpcUaX509CertificateSigningRequest>
#include <QOpcUaX509ExtensionSubjectAlternativeName>
#include <QOpcUaX509ExtensionBasicConstraints>
#include <QOpcUaX509ExtensionKeyUsage>
#include <QOpcUaX509ExtensionExtendedKeyUsage>

#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>

#define defineDataMethod(name) void name()\
{\
    QTest::addColumn<QString>("backend");\
    for (auto backend : m_backends) {\
        const QString rowName = QStringLiteral("%1").arg(backend); \
        QTest::newRow(rowName.toLatin1().constData()) << backend ; \
    }\
}

class Tst_QOpcUaSecurity: public QObject
{
    Q_OBJECT

public:
    Tst_QOpcUaSecurity();

private slots:
    void initTestCase();
    void cleanupTestCase();

    defineDataMethod(keyPairs_data)
    void keyPairs();

    defineDataMethod(certificateSigningRequest_data)
    void certificateSigningRequest();

private:
    QStringList m_backends;
    QOpcUaProvider m_opcUa;
};

QByteArray textifyCertificateRequest(const QByteArray &data)
{
    QProcess p;
    p.start("openssl", QStringList {"req", "-text", "-noout"});
    p.waitForStarted();
    p.write(data);
    p.closeWriteChannel();
    p.waitForFinished();
    return p.readAllStandardOutput();
}

QByteArray textifyCertificate(const QByteArray &data)
{
    QProcess p;
    p.start("openssl", QStringList {"x509", "-text", "-noout"});
    p.waitForStarted();
    p.write(data);
    p.closeWriteChannel();
    p.waitForFinished();
    return p.readAllStandardOutput();
}

QByteArray asn1dump(const QByteArray &data)
{
    QProcess p;
    p.start("openssl", QStringList {"asn1parse", "-inform","PEM"});
    p.waitForStarted();
    p.write(data);
    p.closeWriteChannel();
    p.waitForFinished();
    return p.readAllStandardOutput();
}

Tst_QOpcUaSecurity::Tst_QOpcUaSecurity()
{
    m_backends = QOpcUaProvider::availableBackends();
}

void Tst_QOpcUaSecurity::initTestCase()
{
}

void Tst_QOpcUaSecurity::keyPairs()
{
    QFETCH(QString, backend);

    QOpcUaKeyPair key;
    QOpcUaKeyPair loadedKey;
    QByteArray byteArray;

    QVERIFY(key.hasPrivateKey() == false);

    // Generate key
    key.generateRsaKey(QOpcUaKeyPair::RsaKeyStrength::Bits1024);
    QVERIFY(key.hasPrivateKey() == true);

    // Export public key
    byteArray = key.publicKeyToByteArray();
    QVERIFY(byteArray.startsWith("-----BEGIN PUBLIC KEY-----\n"));
    QVERIFY(byteArray.endsWith("-----END PUBLIC KEY-----\n"));

    // Load public key
    QVERIFY(loadedKey.loadFromPemData(byteArray));
    QVERIFY(loadedKey.hasPrivateKey() == false);

    // Check unencrypted PEM export
    byteArray = key.privateKeyToByteArray(QOpcUaKeyPair::Cipher::Unencrypted, QString());
    QVERIFY(byteArray.startsWith("-----BEGIN PRIVATE KEY-----\n"));
    QVERIFY(byteArray.endsWith("-----END PRIVATE KEY-----\n"));

    // Load private key from PEM data
    QSignalSpy passwordSpy(&loadedKey, SIGNAL(passphraseNeeded(QString&,int,bool)));

    QVERIFY(loadedKey.loadFromPemData(byteArray));
    QVERIFY(loadedKey.hasPrivateKey() == true);
    QCOMPARE(passwordSpy.size(), 0);
    QCOMPARE(loadedKey.privateKeyToByteArray(QOpcUaKeyPair::Cipher::Unencrypted, QString()), byteArray);

    // Check encrypted PEM export
    byteArray = key.privateKeyToByteArray(QOpcUaKeyPair::Cipher::Aes128Cbc,
                                          QStringLiteral("password"));
    QVERIFY(byteArray.startsWith("-----BEGIN ENCRYPTED PRIVATE KEY-----\n"));
    QVERIFY(byteArray.endsWith("-----END ENCRYPTED PRIVATE KEY-----\n"));
    QCOMPARE(passwordSpy.size(), 0);

    // Setup password callback
    QString passphraseToReturn;
    connect(&loadedKey, &QOpcUaKeyPair::passphraseNeeded, this, [&passphraseToReturn](QString &passphrase, int maximumLength, bool writeOperation){
        Q_UNUSED(maximumLength);
        qDebug() << "Requested a passphrase for" << (writeOperation ? "write":"read") << "operation";
        passphrase = passphraseToReturn;
    });

    // Load key with wrong password
    qDebug() << "Trying to decrypt with wrong password; will cause an error";
    passphraseToReturn = "WrongPassword";
    QVERIFY(!loadedKey.loadFromPemData(byteArray));
    QCOMPARE(passwordSpy.size(), 1);
    QVERIFY(loadedKey.hasPrivateKey() == false);

    // Load key with right password
    qDebug() << "Trying to decrypt with right password; will cause no error";
    passphraseToReturn = "password";
    QVERIFY(loadedKey.loadFromPemData(byteArray));
    QCOMPARE(passwordSpy.size(), 2);
    QCOMPARE(loadedKey.privateKeyToByteArray(QOpcUaKeyPair::Cipher::Unencrypted, QString()),
             key.privateKeyToByteArray(QOpcUaKeyPair::Cipher::Unencrypted, QString()));
    QVERIFY(loadedKey.hasPrivateKey() == true);
}

void Tst_QOpcUaSecurity::certificateSigningRequest()
{
    QFETCH(QString, backend);

    QOpcUaKeyPair key;

    // Generate key
    key.generateRsaKey(QOpcUaKeyPair::RsaKeyStrength::Bits1024);
    QVERIFY(key.hasPrivateKey() == true);

    QOpcUaX509CertificateSigningRequest csr;

    QOpcUaX509DistinguishedName dn;
    dn.setEntry(QOpcUaX509DistinguishedName::Type::CommonName, "QtOpcUaViewer");
    dn.setEntry(QOpcUaX509DistinguishedName::Type::CountryName, "DE");
    dn.setEntry(QOpcUaX509DistinguishedName::Type::LocalityName, "Berlin");
    dn.setEntry(QOpcUaX509DistinguishedName::Type::StateOrProvinceName, "Berlin");
    dn.setEntry(QOpcUaX509DistinguishedName::Type::OrganizationName, "The Qt Company");
    csr.setSubject(dn);

    QOpcUaX509ExtensionSubjectAlternativeName *san = new QOpcUaX509ExtensionSubjectAlternativeName;
    san->addEntry(QOpcUaX509ExtensionSubjectAlternativeName::Type::DNS, "foo.com");
    san->addEntry(QOpcUaX509ExtensionSubjectAlternativeName::Type::DNS, "bla.com");
    san->addEntry(QOpcUaX509ExtensionSubjectAlternativeName::Type::URI, "urn:foo.com:The%20Qt%20Company:QtOpcUaViewer");
    san->setCritical(true);
    csr.addExtension(san);

    QOpcUaX509ExtensionBasicConstraints *bc = new QOpcUaX509ExtensionBasicConstraints;
    bc->setCa(false);
    bc->setCritical(true);
    csr.addExtension(bc);

    QOpcUaX509ExtensionKeyUsage *ku = new QOpcUaX509ExtensionKeyUsage;
    ku->setCritical(true);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::DigitalSignature);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::NonRepudiation);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::KeyEncipherment);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::DataEncipherment);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::KeyAgreement);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::CertificateSigning);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::CrlSigning);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::EnciptherOnly);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::DecipherOnly);
    csr.addExtension(ku);

    QOpcUaX509ExtensionExtendedKeyUsage *eku = new QOpcUaX509ExtensionExtendedKeyUsage;
    eku->setCritical(true);
    eku->setKeyUsage(QOpcUaX509ExtensionExtendedKeyUsage::KeyUsage::TlsWebClientAuthentication);
    eku->setKeyUsage(QOpcUaX509ExtensionExtendedKeyUsage::KeyUsage::TlsWebServerAuthentication);
    eku->setKeyUsage(QOpcUaX509ExtensionExtendedKeyUsage::KeyUsage::EmailProtection);
    eku->setKeyUsage(QOpcUaX509ExtensionExtendedKeyUsage::KeyUsage::SignExecutableCode);
    csr.addExtension(eku);

    QByteArray csrData = csr.createRequest(key);
    qDebug() << csrData;
    QVERIFY(csrData.startsWith("-----BEGIN CERTIFICATE REQUEST-----\n"));
    QVERIFY(csrData.endsWith("\n-----END CERTIFICATE REQUEST-----\n"));
    qDebug().noquote() << textifyCertificateRequest(csrData);
    qDebug().noquote() << asn1dump(csrData);

    QByteArray certData = csr.createSelfSignedCertificate(key);
    qDebug() << certData;
    QVERIFY(certData.startsWith("-----BEGIN CERTIFICATE-----\n"));
    QVERIFY(certData.endsWith("\n-----END CERTIFICATE-----\n"));
    const auto textCert = QString::fromUtf8(textifyCertificate(certData));
    qDebug().noquote() << textCert;
    qDebug().noquote() << asn1dump(certData);
    QVERIFY(textCert.contains(QStringLiteral("Digital Signature, Non Repudiation, Key Encipherment, Data Encipherment, Key Agreement, Certificate Sign, CRL Sign, Encipher Only, Decipher Only")));
    QVERIFY(textCert.contains(QStringLiteral("TLS Web Server Authentication, TLS Web Client Authentication, Code Signing, E-mail Protection")));
}

void Tst_QOpcUaSecurity::cleanupTestCase()
{
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

#include "tst_x509.moc"


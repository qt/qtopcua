// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QOpcUaProvider>
#include <QOpcUaKeyPair>
#include <QOpcUaX509CertificateSigningRequest>
#include <QOpcUaX509ExtensionSubjectAlternativeName>
#include <QOpcUaX509ExtensionBasicConstraints>
#include <QOpcUaX509ExtensionKeyUsage>
#include <QOpcUaX509ExtensionExtendedKeyUsage>
#include <QFile>

int main(int argc, char **argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    // Generate RSA Key
    QOpcUaKeyPair key;
    key.generateRsaKey(QOpcUaKeyPair::RsaKeyStrength::Bits2048);

    // Save private key to file
    QByteArray keyData = key.privateKeyToByteArray(QOpcUaKeyPair::Cipher::Unencrypted, QString());

    // In order to create a private key file with password for the Unified Automation plugin,
    // the following invocation can be used:
    // QByteArray keyData = key.privateKeyToByteArray(QOpcUaKeyPair::Cipher::Aes128Cbc, "password");

    QFile keyFile("privateKey.pem");
    keyFile.open(QFile::WriteOnly);
    keyFile.write(keyData);
    keyFile.close();

    // Create a certificate signing request
    QOpcUaX509CertificateSigningRequest csr;

    // Set the subject of the certificate
    QOpcUaX509DistinguishedName dn;
    dn.setEntry(QOpcUaX509DistinguishedName::Type::CommonName, "QtOpcUaViewer");
    dn.setEntry(QOpcUaX509DistinguishedName::Type::CountryName, "DE");
    dn.setEntry(QOpcUaX509DistinguishedName::Type::LocalityName, "Berlin");
    dn.setEntry(QOpcUaX509DistinguishedName::Type::StateOrProvinceName, "Berlin");
    dn.setEntry(QOpcUaX509DistinguishedName::Type::OrganizationName, "The Qt Company");
    csr.setSubject(dn);

    // The subject alternative name extension is needed for OPC UA
    QOpcUaX509ExtensionSubjectAlternativeName *san = new QOpcUaX509ExtensionSubjectAlternativeName;
    san->addEntry(QOpcUaX509ExtensionSubjectAlternativeName::Type::DNS, "foo.com");
    san->addEntry(QOpcUaX509ExtensionSubjectAlternativeName::Type::URI, "urn:foo.com:The%20Qt%20Company:QtOpcUaViewer");
    san->setCritical(true);
    csr.addExtension(san);

    // Set the certificate basic constraints
    QOpcUaX509ExtensionBasicConstraints *bc = new QOpcUaX509ExtensionBasicConstraints;
    bc->setCa(false);
    bc->setCritical(true);
    csr.addExtension(bc);

    // The required values for key usage and extended key usage are defined in OPC UA Part 6, 6.2.2, Table 43

    // Set the key usage constraints
    QOpcUaX509ExtensionKeyUsage *ku = new QOpcUaX509ExtensionKeyUsage;
    ku->setCritical(true);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::DigitalSignature);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::NonRepudiation);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::KeyEncipherment);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::DataEncipherment);
    csr.addExtension(ku);

    // Set the extended key usage constraints
    QOpcUaX509ExtensionExtendedKeyUsage *eku = new QOpcUaX509ExtensionExtendedKeyUsage;
    eku->setCritical(true);
    eku->setKeyUsage(QOpcUaX509ExtensionExtendedKeyUsage::KeyUsage::TlsWebClientAuthentication);
    csr.addExtension(eku);

    // Now there are two options:
    // 1. When you need to get your certificate signing request signed by a certificate authority
    //    you have to use the request data.
    // 2. When there is no certificate authority you have to self-sign the request.

    // Option 1
    QByteArray certificateSigingRequestData = csr.createRequest(key);

    // Option 2
    QByteArray selfSignedCertificateData = csr.createSelfSignedCertificate(key);

    return 0;
}

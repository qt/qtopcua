// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QOpcUaKeyPair>
#include <QOpcUaProvider>
#include <QOpcUaX509CertificateSigningRequest>
#include <QOpcUaX509ExtensionBasicConstraints>
#include <QOpcUaX509ExtensionExtendedKeyUsage>
#include <QOpcUaX509ExtensionKeyUsage>
#include <QOpcUaX509ExtensionSubjectAlternativeName>
#include <QFile>

using namespace Qt::StringLiterals;

int main(int argc, char **argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    // Generate RSA Key
    //! [0]
    QOpcUaKeyPair key;
    key.generateRsaKey(QOpcUaKeyPair::RsaKeyStrength::Bits2048);
    //! [0]

    // Save private key to file
    //! [1]
    QByteArray keyData = key.privateKeyToByteArray(QOpcUaKeyPair::Cipher::Unencrypted, QString());

    QFile keyFile(u"privateKey.pem"_s);
    if (!keyFile.open(QFile::WriteOnly))
        return EXIT_FAILURE;

    keyFile.write(keyData);
    keyFile.close();
    //! [1]

    // Create a certificate signing request
    //! [2]
    QOpcUaX509CertificateSigningRequest csr;

    // Set the subject of the certificate
    QOpcUaX509DistinguishedName dn;
    dn.setEntry(QOpcUaX509DistinguishedName::Type::CommonName, u"QtOpcUaViewer"_s);
    dn.setEntry(QOpcUaX509DistinguishedName::Type::CountryName, u"DE"_s);
    dn.setEntry(QOpcUaX509DistinguishedName::Type::LocalityName, u"Berlin"_s);
    dn.setEntry(QOpcUaX509DistinguishedName::Type::StateOrProvinceName, u"Berlin"_s);
    dn.setEntry(QOpcUaX509DistinguishedName::Type::OrganizationName, u"The Qt Company"_s);
    csr.setSubject(dn);
    //! [2]

    // The subject alternative name extension is needed for OPC UA
    QOpcUaX509ExtensionSubjectAlternativeName *san = new QOpcUaX509ExtensionSubjectAlternativeName;
    san->addEntry(QOpcUaX509ExtensionSubjectAlternativeName::Type::DNS, u"foo.com"_s);
    san->addEntry(QOpcUaX509ExtensionSubjectAlternativeName::Type::URI, u"urn:foo.com:The%20Qt%20Company:QtOpcUaViewer"_s);
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
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::CertificateSigning);
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
    //! [3]
    QByteArray certificateSigningRequestData = csr.createRequest(key);
    //! [3]

    // Option 2
    //! [4]
    QByteArray selfSignedCertificateData = csr.createSelfSignedCertificate(key);
    //! [4]

    Q_UNUSED(certificateSigningRequestData)
    Q_UNUSED(selfSignedCertificateData)

    return 0;
}

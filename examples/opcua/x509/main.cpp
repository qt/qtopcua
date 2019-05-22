/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QOpcUaProvider>
#include <QOpcUaKeyPair>
#include <QOpcUaX509CertificateSigningRequest>
#include <QOpcUaX509ExtensionSubjectAlternativeName>
#include <QOpcUaX509ExtensionBasicConstraints>
#include <QOpcUaX509ExtensionKeyUsage>
#include <QFile>

int main(int argc, char **argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    // Generate RSA Key
    QOpcUaKeyPair key;
    key.generateRsaKey(QOpcUaKeyPair::RsaKeyStrength::Bits1024);

    // Save private key to file
    QByteArray keyData = key.privateKeyToByteArray(QOpcUaKeyPair::Cipher::Aes128Cbc, "password");

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

    // Set the key usage constraints
    QOpcUaX509ExtensionKeyUsage *ku = new QOpcUaX509ExtensionKeyUsage;
    ku->setCritical(true);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::DigitalSignature);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::NonRepudiation);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::KeyEncipherment);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::DataEncipherment);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::CertificateSigning);
    csr.addExtension(ku);

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

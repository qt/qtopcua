/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#include "qopcuax509certificatesigningrequest.h"
#include "openssl_symbols_p.h"

#include "qopcuakeypair_p.h"
#include "qopcuax509certificatesigningrequest_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaX509CertificateSigningRequest
    \inmodule QtOpcUa
    \since 5.14

    \brief QOpcUaX509CertificateSigningRequest create a certificate signing request

    This class is currently available as a Technology Preview, and therefore the API
    and functionality provided by the class may be subject to change at any time without
    prior notice.

    Before actually creating the singing request data, any extension needed for that
    specific request has to be added. Current supported extensions are SubjectAlternativeName,
    BasicConstrains, KeyUsage and ExtendedKeyUsage.

    \code
        // Generate key
        QOpcUaKeyPair key;
        key.generateRsaKey(QOpcUaKeyPair::RsaKeyStrength::Bits1024);

        QOpcUaX509CertificateSigningRequest csr;

        QOpcUaX509DistinguishedName dn;
        dn.setEntry(QOpcUaX509DistinguishedName::Type::CommonName, "QtOpcUaViewer");
        dn.setEntry(QOpcUaX509DistinguishedName::Type::CountryName, "DE");
        dn.setEntry(QOpcUaX509DistinguishedName::Type::LocalityName, "Berlin");
        dn.setEntry(QOpcUaX509DistinguishedName::Type::StateOrProvinceName, "Berlin");
        dn.setEntry(QOpcUaX509DistinguishedName::Type::OrganizationName, "The Qt Company");
        csr.setSubject(dn);

        QOpcUaX509ExtensionSubjectAlternativeName *san = new QOpcUaX509ExtensionSubjectAlternativeName;
        san->addData(QOpcUaX509ExtensionSubjectAlternativeName::Type::DNS, "foo.com");
        san->addData(QOpcUaX509ExtensionSubjectAlternativeName::Type::DNS, "foo.com");
        san->addData(QOpcUaX509ExtensionSubjectAlternativeName::Type::URI, "urn:foo.com:The%20Qt%20Company:QtOpcUaViewer");
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
        ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::CertificateSigning);
        csr.addExtension(ku);

        QOpcUaX509ExtensionExtendedKeyUsage *eku = new QOpcUaX509ExtensionExtendedKeyUsage;
        eku->setCritical(true);
        eku->setKeyUsage(QOpcUaX509ExtensionExtendedKeyUsage::KeyUsage::EmailProtection);
        csr.addExtension(eku);

        QByteArray csrData = csr.createRequest(key);
    \endcode

    \sa QOpcUaX509ExtensionSubjectAlternativeName, QOpcUaX509ExtensionBasicConstraints, QOpcUaX509ExtensionKeyUsage, QOpcUaX509ExtensionKeyUsage
*/

/*!
    \enum QOpcUaX509CertificateSigningRequest::MessageDigest

    This enum type specifies the message digest to be used.

    \value SHA256
           Using the SHA256 message digest
*/

/*!
    \enum QOpcUaX509CertificateSigningRequest::Encoding

    This enum type specifies the encoding of the generated certificate siging request.

    \value PEM
           Using PEM encoding
    \value DER
           Using DER encoding
*/

/*!
    Creates an empty certificate signing request.
*/
QOpcUaX509CertificateSigningRequest::QOpcUaX509CertificateSigningRequest()
    : d_ptr(new QOpcUaX509CertificateSigningRequestPrivate)
{
    setEncoding(Encoding::PEM);
}

/*!
    Destroys the request and frees all extensions.
*/
QOpcUaX509CertificateSigningRequest::~QOpcUaX509CertificateSigningRequest()
{
}

/*!
    Sets the used message digest to \a digest.
    The default message digest is SHA256.
*/
void QOpcUaX509CertificateSigningRequest::setMessageDigest(QOpcUaX509CertificateSigningRequest::MessageDigest digest)
{
    Q_D(QOpcUaX509CertificateSigningRequest);
    d->setMessageDigest(digest);
}

/*!
    Returns the used message digest.
*/
QOpcUaX509CertificateSigningRequest::MessageDigest QOpcUaX509CertificateSigningRequest::messageDigest() const
{
    Q_D(const QOpcUaX509CertificateSigningRequest);
    return d->messageDigest();
}

/*!
    Returns the used request encoding.
*/
QOpcUaX509CertificateSigningRequest::Encoding QOpcUaX509CertificateSigningRequest::encoding() const
{
    Q_D(const QOpcUaX509CertificateSigningRequest);
    return d->encoding();
}

/*!
    Sets the used request encoding to \a encoding.
    The default request encoding is PEM.
*/
void QOpcUaX509CertificateSigningRequest::setEncoding(QOpcUaX509CertificateSigningRequest::Encoding encoding)
{
    Q_D(QOpcUaX509CertificateSigningRequest);
    d->setEncoding(encoding);
}

/*!
    Adds a certificate extension to the request.

    The ownership of the extension object will be transferred to this class.

    \sa QOpcUaX509ExtensionSubjectAlternativeName, QOpcUaX509ExtensionBasicConstraints, QOpcUaX509ExtensionKeyUsage, QOpcUaX509ExtensionKeyUsage
*/
void QOpcUaX509CertificateSigningRequest::addExtension(QOpcUaX509Extension *extension)
{
    Q_D(QOpcUaX509CertificateSigningRequest);
    d->addExtension(extension);
}

/*!
    Sets the subject for this request.
    Without a subject it is not possible to generate the request.
*/
void QOpcUaX509CertificateSigningRequest::setSubject(const QOpcUaX509DistinguishedName &subject)
{
    Q_D(QOpcUaX509CertificateSigningRequest);
    d->setSubject(subject);
}

/*!
    Returns the subject of this request.
*/
const QOpcUaX509DistinguishedName &QOpcUaX509CertificateSigningRequest::subject() const
{
    Q_D(const QOpcUaX509CertificateSigningRequest);
    return d->subject();
}

/*!
    Creates a certificate signing request to be the to a CA for signing.
    The private key in \a privateKey is used to sign the request.
    The request data is returned as a byte array in the encoding set by setEncoding().
*/
QByteArray QOpcUaX509CertificateSigningRequest::createRequest(const QOpcUaKeyPair &privateKey)
{
    Q_D(QOpcUaX509CertificateSigningRequest);
    return d->createRequest(privateKey);
}

/*!
    Creates a self-signed certificate from this request for immediate use.
    The private key in \a privateKey is used to sign the request.
    A validity in days can be specified in \a validityInDays.
    The request data is returned as a byte array in the encoding set by setEncoding().
*/
QByteArray QOpcUaX509CertificateSigningRequest::createSelfSignedCertificate(const QOpcUaKeyPair &privateKey, int validityInDays)
{
    Q_D(QOpcUaX509CertificateSigningRequest);
    return d->createSelfSignedCertificate(privateKey, validityInDays);
}

QT_END_NAMESPACE

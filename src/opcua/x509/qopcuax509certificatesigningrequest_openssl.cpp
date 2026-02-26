// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:critical reason:cryptography

#include "qopcuax509certificatesigningrequest.h"
#include "openssl_symbols_p.h"
#include "qopcuakeypair_p.h"
#include "qopcuax509utils_p.h"
#include "qopcuax509distinguishedname.h"
#include "qopcuax509extensionsubjectalternativename.h"
#include "qopcuax509certificatesigningrequest_p.h"
#include <QOpcUaX509ExtensionBasicConstraints>
#include <QOpcUaX509ExtensionKeyUsage>
#include <QOpcUaX509ExtensionExtendedKeyUsage>


QT_BEGIN_NAMESPACE

using namespace Qt::Literals::StringLiterals;

QOpcUaX509CertificateSigningRequestPrivate::QOpcUaX509CertificateSigningRequestPrivate()
{

}

QOpcUaX509CertificateSigningRequestPrivate::~QOpcUaX509CertificateSigningRequestPrivate()
{
    qDeleteAll(m_extensions);
}

void QOpcUaX509CertificateSigningRequestPrivate::setMessageDigest(QOpcUaX509CertificateSigningRequest::MessageDigest digest)
{
    m_messageDigest = digest;
}

QOpcUaX509CertificateSigningRequest::MessageDigest QOpcUaX509CertificateSigningRequestPrivate::messageDigest() const
{
    return m_messageDigest;
}

void QOpcUaX509CertificateSigningRequestPrivate::addExtension(QOpcUaX509Extension *extension)
{
    m_extensions.append(extension);
}

void QOpcUaX509CertificateSigningRequestPrivate::setSubject(const QOpcUaX509DistinguishedName &subject)
{
    m_subject = subject;
}

QOpcUaX509CertificateSigningRequest::Encoding QOpcUaX509CertificateSigningRequestPrivate::encoding() const
{
    return m_encoding;
}

void QOpcUaX509CertificateSigningRequestPrivate::setEncoding(QOpcUaX509CertificateSigningRequest::Encoding encoding)
{
    m_encoding = encoding;
}

const QOpcUaX509DistinguishedName &QOpcUaX509CertificateSigningRequestPrivate::subject() const
{
    return m_subject;
}

static X509_EXTENSION *createExtension(QOpcUaX509Extension *extension)
{
    X509_EXTENSION *ex = nullptr;

    if (const auto *san = dynamic_cast<const QOpcUaX509ExtensionSubjectAlternativeName *>(extension)) {
        QStringList data;

        for (const auto &pair : std::as_const(san->entries())) {
            QString prefix;
            if (pair.first == QOpcUaX509ExtensionSubjectAlternativeName::Type::DNS)
                prefix = u"DNS:"_s;
            else if (pair.first == QOpcUaX509ExtensionSubjectAlternativeName::Type::Email)
                    prefix = u"EMAIL:"_s;
            else if (pair.first == QOpcUaX509ExtensionSubjectAlternativeName::Type::IP)
                    prefix = u"IP:"_s;
            else if (pair.first == QOpcUaX509ExtensionSubjectAlternativeName::Type::URI)
                    prefix = u"URI:"_s;
            else {
                qCWarning(lcOpcUaSsl) << "Invalid SubjectAlternativeName type";
                return nullptr;
            }

            if (pair.second.isEmpty() || pair.second.contains(QChar::fromLatin1(','))) {
                qCWarning(lcOpcUaSsl) << "Invalid SubjectAlternativeName value";
                return nullptr;
            }

            data.append(prefix + pair.second);
        }

        ex = QtOpcUaSsl::q_X509V3_EXT_conf_nid(NULL, NULL, NID_subject_alt_name, data.join(QLatin1Char(',')).toUtf8().data());
        if (!ex) {
            qCWarning(lcOpcUaSsl) << "Failed to create X509 extension" << data;
            return nullptr;
        }
        QtOpcUaSsl::q_X509_EXTENSION_set_critical(ex, san->critical() ? 1 : 0);
    } else if (const auto *bc = dynamic_cast<const QOpcUaX509ExtensionBasicConstraints *>(extension)) {
        QString data = u"CA:"_s + (bc->ca() ? u"true"_s : u"false"_s);
        if (bc->ca() && bc->pathLength() >= 0)
            data.append(u",pathlen:"_s + QString::number(bc->pathLength()));

        ex = QtOpcUaSsl::q_X509V3_EXT_conf_nid(NULL, NULL, NID_basic_constraints, data.toUtf8().data());
        if (!ex) {
            qCWarning(lcOpcUaSsl) << "Failed to create X509 extension" << data;
            return nullptr;
        }
        QtOpcUaSsl::q_X509_EXTENSION_set_critical(ex, bc->critical() ? 1 : 0);
    } else if (const auto *ku = dynamic_cast<const QOpcUaX509ExtensionKeyUsage *>(extension)) {
        QStringList data;

        if (ku->keyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::DigitalSignature))
            data.append(u"digitalSignature"_s);
        if (ku->keyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::NonRepudiation))
            data.append(u"nonRepudiation"_s);
        if (ku->keyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::KeyEncipherment))
            data.append(u"keyEncipherment"_s);
        if (ku->keyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::DataEncipherment))
            data.append(u"dataEncipherment"_s);
        if (ku->keyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::KeyAgreement))
            data.append(u"keyAgreement"_s);
        if (ku->keyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::CertificateSigning))
            data.append(u"keyCertSign"_s);
        if (ku->keyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::CrlSigning))
            data.append(u"cRLSign"_s);
        if (ku->keyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::EnciptherOnly))
            data.append(u"encipherOnly"_s);
        if (ku->keyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::DecipherOnly))
            data.append(u"decipherOnly"_s);

        ex = QtOpcUaSsl::q_X509V3_EXT_conf_nid(NULL, NULL, NID_key_usage, data.join(QLatin1Char(',')).toUtf8().data());
        if (!ex) {
            qCWarning(lcOpcUaSsl) << "Failed to create X509 extension" << data;
            return nullptr;
        }
        QtOpcUaSsl::q_X509_EXTENSION_set_critical(ex, ku->critical() ? 1 : 0);
    } else if (const auto *eku = dynamic_cast<const QOpcUaX509ExtensionExtendedKeyUsage *>(extension)) {
        QStringList data;

        if (eku->keyUsage(QOpcUaX509ExtensionExtendedKeyUsage::KeyUsage::TlsWebServerAuthentication))
            data.append(u"serverAuth"_s);
        if (eku->keyUsage(QOpcUaX509ExtensionExtendedKeyUsage::KeyUsage::TlsWebClientAuthentication))
            data.append(u"clientAuth"_s);
        if (eku->keyUsage(QOpcUaX509ExtensionExtendedKeyUsage::KeyUsage::SignExecutableCode))
            data.append(u"codeSigning"_s);
        if (eku->keyUsage(QOpcUaX509ExtensionExtendedKeyUsage::KeyUsage::EmailProtection))
            data.append(u"emailProtection"_s);

       // NID_ext_key_usage
        ex = QtOpcUaSsl::q_X509V3_EXT_conf_nid(NULL, NULL, NID_ext_key_usage, data.join(QLatin1Char(',')).toUtf8().data());
        if (!ex) {
            qCWarning(lcOpcUaSsl) << "Failed to create X509 extension" << data;
            return nullptr;
        }
        QtOpcUaSsl::q_X509_EXTENSION_set_critical(ex, eku->critical() ? 1 : 0);
    } else {
        qCWarning(lcOpcUaSsl) << "Unknown X509 extension";
        return nullptr;
    }
    return ex;
}

static bool setSubjectName(X509_NAME *subject, const QOpcUaX509DistinguishedName &dn) {
    QList<QOpcUaX509DistinguishedName::Type> entries = {
        QOpcUaX509DistinguishedName::Type::CommonName,
        QOpcUaX509DistinguishedName::Type::CountryName,
        QOpcUaX509DistinguishedName::Type::LocalityName,
        QOpcUaX509DistinguishedName::Type::StateOrProvinceName,
        QOpcUaX509DistinguishedName::Type::OrganizationName,
    };

    for (const auto &type : entries) {
        const auto value = dn.entry(type);
        if (value.isEmpty())
            continue;

        ASN1_OBJECT *obj = QtOpcUaSsl::q_OBJ_txt2obj(QOpcUaX509DistinguishedName::typeToOid(type).toLatin1().constData(), 1 /* no names allowed */);
        if (!obj) {
            qCWarning(lcOpcUaSsl) << "Invalid distinguishedName type";
            return false;
        }

        if (!QtOpcUaSsl::q_X509_NAME_add_entry_by_OBJ(subject, obj, MBSTRING_UTF8, (const unsigned char*)(value.toUtf8().constData()), -1, -1, 0)) {
            qCWarning(lcOpcUaSsl) << "Failed to set CSR entry:" << getOpenSslError();
            return false;
        }
    }
    return true;
}

// Creates the request and returns a PEM encoded byte array
QByteArray QOpcUaX509CertificateSigningRequestPrivate::createRequest(const QOpcUaKeyPair &privateKey)
{
    if (!privateKey.hasPrivateKey()) {
        qCWarning(lcOpcUaSsl) << "Key has no private key";
        return QByteArray();
    }

    auto keyData = privateKey.d_func()->m_keyData;

    X509_REQ *req = QtOpcUaSsl::q_X509_REQ_new();
    if (!req) {
        qCWarning(lcOpcUaSsl) << "Failed to create CSR:" << getOpenSslError();
        return QByteArray();
    }
    Deleter<X509_REQ> reqDeleter(req, QtOpcUaSsl::q_X509_REQ_free);

    if (!QtOpcUaSsl::q_X509_REQ_set_version(req, 0 /* version */)) {
        qCWarning(lcOpcUaSsl) << "Failed to set CSR version:" << getOpenSslError();
        return QByteArray();
    }

    X509_NAME *subj = QtOpcUaSsl::q_X509_REQ_get_subject_name(req);
    if (!subj) {
        qCWarning(lcOpcUaSsl) << "Invalid subject pointer";
        return QByteArray();
    }

    if (!setSubjectName(subj, m_subject)) {
        qCWarning(lcOpcUaSsl) << "Failed to set subject";
        return QByteArray();
    }

    if (m_extensions.size() > 0) {
        auto exts = QtOpcUaSsl::q_sk_X509_EXTENSION_new_null();

        for (auto extension : std::as_const(m_extensions)) {
            auto ex = createExtension(extension);
            if (ex)
                QtOpcUaSsl::q_sk_X509_EXTENSION_push(exts, ex); // returns void
        }
        if (QtOpcUaSsl::q_X509_REQ_add_extensions(req, (STACK_OF(X509_EXTENSION) *)exts) == 0) {
            qCWarning(lcOpcUaSsl) << "Failed to add X509 extensions";
            return QByteArray();
        }
        QtOpcUaSsl::q_sk_X509_EXTENSION_pop_free(exts, (void(*)(void*))QtOpcUaSsl::q_X509_EXTENSION_free); // frees the whole stack, returns void
    } // end of for loop

    if (!QtOpcUaSsl::q_X509_REQ_set_pubkey(req, keyData)) {
        qCWarning(lcOpcUaSsl) << "Failed to set public key:" << getOpenSslError();
        return QByteArray();
    }

    const EVP_MD *digest = nullptr;
    if (m_messageDigest == QOpcUaX509CertificateSigningRequest::MessageDigest::SHA256)
        digest = QtOpcUaSsl::q_EVP_sha256();

    if (!digest) {
        qCWarning(lcOpcUaSsl) << "Invalid message digest";
        return QByteArray();
    }

    if (QtOpcUaSsl::q_X509_REQ_sign(req, keyData, digest) <= 0) {
        qCWarning(lcOpcUaSsl) << "Failed to sign CSR:" << getOpenSslError();
        return QByteArray();
    }

    BIO *bio = QtOpcUaSsl::q_BIO_new(QtOpcUaSsl::q_BIO_s_mem());
    if (!bio) {
        qCWarning(lcOpcUaSsl) << "Failed to allocate a buffer:" << getOpenSslError();
        return QByteArray();
    }
    Deleter<BIO> bioDeleter(bio, QtOpcUaSsl::q_BIO_free_all);

    int result = 0;

    if (m_encoding == QOpcUaX509CertificateSigningRequest::Encoding::PEM) {
        // Some CAs require to use QtOpcUaSsl::q_PEM_write_bio_X509_REW_NEW
        result = QtOpcUaSsl::q_PEM_write_bio_X509_REQ(bio, req);
    } else if (m_encoding == QOpcUaX509CertificateSigningRequest::Encoding::DER) {
        result = QtOpcUaSsl::q_i2d_X509_REQ_bio(bio, req);
    }
    if (result != 1) {
        qCWarning(lcOpcUaSsl) << "Failed to export certificate request";
        return QByteArray();
    }

    char *buf;
    int length = QtOpcUaSsl::q_BIO_get_mem_data(bio, &buf);
    QByteArray data(buf, length);
    return data;
}

QByteArray QOpcUaX509CertificateSigningRequestPrivate::createSelfSignedCertificate(const QOpcUaKeyPair &privateKey, int validityInDays)
{
    if (!privateKey.hasPrivateKey()) {
        qCWarning(lcOpcUaSsl) << "Key has no private key";
        return QByteArray();
    }

    auto keyData = privateKey.d_func()->m_keyData;

    X509 *x509 = QtOpcUaSsl::q_X509_new();
    if (!x509)
        return QByteArray();

    Deleter<X509> x509Deleter(x509, QtOpcUaSsl::q_X509_free);

    if (!QtOpcUaSsl::q_X509_set_version(x509, 2 /* version */)) {
        qCWarning(lcOpcUaSsl) << "Failed to set version";
        return QByteArray();
    }
    QtOpcUaSsl::q_X509_gmtime_adj(QtOpcUaSsl::q_X509_getm_notBefore(x509), 0); // current time
    QtOpcUaSsl::q_X509_gmtime_adj(QtOpcUaSsl::q_X509_getm_notAfter(x509), (long)60 * 60 * 24 * validityInDays);

    if (!QtOpcUaSsl::q_X509_set_pubkey(x509, keyData)) {
        qCWarning(lcOpcUaSsl) << "Failed to set public key:" << getOpenSslError();
        return QByteArray();
    }

    X509_NAME *subj = QtOpcUaSsl::q_X509_get_subject_name(x509);
    if (!subj) {
        qCWarning(lcOpcUaSsl) << "Invalid subject pointer";
        return QByteArray();
    }

    if (!setSubjectName(subj, m_subject)) {
        qCWarning(lcOpcUaSsl) << "Failed to set subject";
        return QByteArray();
    }

    X509_NAME *issuer = QtOpcUaSsl::q_X509_get_issuer_name(x509);
    if (!issuer) {
        qCWarning(lcOpcUaSsl) << "Invalid issuer pointer";
        return QByteArray();
    }

    if (!setSubjectName(issuer, m_subject)) {
        qCWarning(lcOpcUaSsl) << "Failed to set issuer";
        return QByteArray();
    }

    for (auto extension : std::as_const(m_extensions)) {
        auto ex = createExtension(extension);
        if (ex) {
            if (!QtOpcUaSsl::q_X509_add_ext(x509, ex, -1)) {
                qCWarning(lcOpcUaSsl) << "Failed to add extension";
                return QByteArray();
            }
            QtOpcUaSsl::q_X509_EXTENSION_free(ex);
        } else {
            qCWarning(lcOpcUaSsl) << "Invalid extension";
            return QByteArray();
        }
    }

    // Hash of public key
    unsigned char publicKeyHash[SHA_DIGEST_LENGTH];
    unsigned int len;
    if (!QtOpcUaSsl::q_X509_pubkey_digest(x509, QtOpcUaSsl::q_EVP_sha1(), publicKeyHash, &len)) {
        qCWarning(lcOpcUaSsl) << "Failed to hash public key";
        return QByteArray();
    }

    // Set subject key identifier
    ASN1_OCTET_STRING *subjectKeyIdentifier = QtOpcUaSsl::q_ASN1_OCTET_STRING_new();
    if (!subjectKeyIdentifier) {
        qCWarning(lcOpcUaSsl) << "Failed to allocate ASN1 string";
        return QByteArray();
    }
    Deleter<ASN1_OCTET_STRING> subjectKeyIdentifierDeleter(subjectKeyIdentifier, QtOpcUaSsl::q_ASN1_OCTET_STRING_free);

    if (!QtOpcUaSsl::q_ASN1_OCTET_STRING_set(subjectKeyIdentifier, publicKeyHash, SHA_DIGEST_LENGTH)) {
        qCWarning(lcOpcUaSsl) << "Failed set ASN1 string";
        return QByteArray();
    }

    if (!QtOpcUaSsl::q_X509_add1_ext_i2d(x509, NID_subject_key_identifier, subjectKeyIdentifier, 0, X509V3_ADD_DEFAULT)) {
        qCWarning(lcOpcUaSsl) << "Failed to add subject key identifier extension";
        return QByteArray();
    }

    // Set serial number
    unsigned char subjHash[SHA_DIGEST_LENGTH];
    unsigned char finalHash[SHA_DIGEST_LENGTH];

    if (!QtOpcUaSsl::q_X509_NAME_digest(subj, QtOpcUaSsl::q_EVP_sha1(), subjHash, &len)) {
        qCWarning(lcOpcUaSsl) << "failed";
        return QByteArray();
    }
    for (unsigned int i = 0; i < len; i++)
        finalHash[i] = subjHash[i] ^ publicKeyHash[i];

    ASN1_INTEGER *serial_num = QtOpcUaSsl::q_ASN1_INTEGER_new();
    if (!serial_num) {
        qCWarning(lcOpcUaSsl) << "Failed to allocate ASN1 integer";
        return QByteArray();
    }
    Deleter<ASN1_OCTET_STRING> serial_numDeleter(serial_num, QtOpcUaSsl::q_ASN1_INTEGER_free);

    if (!QtOpcUaSsl::q_ASN1_OCTET_STRING_set(serial_num, finalHash, len)) {
        qCWarning(lcOpcUaSsl) << "Failed to set ASN1 integer";
        return QByteArray();
    }
    if (!QtOpcUaSsl::q_X509_set_serialNumber(x509, serial_num)) {
        qCWarning(lcOpcUaSsl) << "Failed to set serial number";
        return QByteArray();
    }

    // Set authority key identifier
    AUTHORITY_KEYID *akid = QtOpcUaSsl::q_AUTHORITY_KEYID_new();
    if (!akid) {
        qCWarning(lcOpcUaSsl) << "Failed to allocate authority key id";
        return QByteArray();
    }
    Deleter<AUTHORITY_KEYID> akidDeleter(akid, QtOpcUaSsl::q_AUTHORITY_KEYID_free);

    akid->issuer = QtOpcUaSsl::q_GENERAL_NAMES_new();
    if (!akid->issuer) {
        qCWarning(lcOpcUaSsl) << "Failed to set authority key id";
        return QByteArray();
    }

    GENERAL_NAME *generalName = QtOpcUaSsl::q_GENERAL_NAME_new();
    if (!generalName) {
        qCWarning(lcOpcUaSsl) << "Failed to set authority key id";
        return QByteArray();
    }
    generalName->type = GEN_DIRNAME;
    generalName->d.directoryName = QtOpcUaSsl::q_X509_NAME_dup(QtOpcUaSsl::q_X509_get_subject_name(x509));
    QtOpcUaSsl::q_sk_GENERAL_NAME_push((OPENSSL_STACK*)akid->issuer, generalName);
    akid->keyid = (ASN1_OCTET_STRING*)QtOpcUaSsl::q_X509_get_ext_d2i(x509, NID_subject_key_identifier, NULL, NULL);
    akid->serial = QtOpcUaSsl::q_ASN1_INTEGER_dup(QtOpcUaSsl::q_X509_get_serialNumber(x509));

    if (!QtOpcUaSsl::q_X509_add1_ext_i2d(x509, NID_authority_key_identifier, akid, 0, X509V3_ADD_DEFAULT)) {
        qCWarning(lcOpcUaSsl) << "Failed to add authority key id extension";
        return QByteArray();
    }

    const EVP_MD *digest = nullptr;
    if (m_messageDigest == QOpcUaX509CertificateSigningRequest::MessageDigest::SHA256)
        digest = QtOpcUaSsl::q_EVP_sha256();

    if (!digest) {
        qCWarning(lcOpcUaSsl) << "Invalid message digest";
        return QByteArray();
    }

    if (QtOpcUaSsl::q_X509_sign(x509, keyData, digest) <= 0) {
        qCWarning(lcOpcUaSsl) << "Failed to sign certificate:" << getOpenSslError();
        return QByteArray();
    }

    BIO *bio = QtOpcUaSsl::q_BIO_new(QtOpcUaSsl::q_BIO_s_mem());
    if (!bio) {
        qCWarning(lcOpcUaSsl) << "Failed to allocate a buffer:" << getOpenSslError();
        return QByteArray();
    }
    Deleter<BIO> bioDeleter(bio, QtOpcUaSsl::q_BIO_free_all);

    int result = 0;

    if (m_encoding == QOpcUaX509CertificateSigningRequest::Encoding::PEM) {
        result = QtOpcUaSsl::q_PEM_write_bio_X509(bio, x509);
    } else if (m_encoding == QOpcUaX509CertificateSigningRequest::Encoding::DER) {
        result = QtOpcUaSsl::q_i2d_X509_bio(bio, x509);
    }
    if (result != 1) {
        qCWarning(lcOpcUaSsl) << "Failed to export certificate";
        return QByteArray();
    }

    char *buf;
    int length = QtOpcUaSsl::q_BIO_get_mem_data(bio, &buf);
    QByteArray data(buf, length);
    return data;
}

QT_END_NAMESPACE


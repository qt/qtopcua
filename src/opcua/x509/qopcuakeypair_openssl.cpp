// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:critical reason:cryptography

#include "qopcuakeypair_p.h"
#include "openssl_symbols_p.h"
#include "qopcuax509utils_p.h"

QT_BEGIN_NAMESPACE

QOpcUaKeyPairPrivate::QOpcUaKeyPairPrivate()
    : QObjectPrivate()
{
    if (!QtOpcUaSsl::q_resolveOpenSslSymbols())
        qFatal("Failed to resolve symbols");

    QtOpcUaSsl::q_ERR_load_crypto_strings();
}

QOpcUaKeyPairPrivate::~QOpcUaKeyPairPrivate()
{
    if (m_keyData) {
        QtOpcUaSsl::q_EVP_PKEY_free(m_keyData);
        m_keyData = nullptr;
    }
}

static int passwordCallback(char *passphraseBuffer, int maximumPassphraseSize, int writeOperation, void *userData) {
    QOpcUaKeyPair *source = reinterpret_cast<QOpcUaKeyPair*>(userData);
    QString passphrase;
    emit source->passphraseNeeded(passphrase, maximumPassphraseSize, writeOperation == 1);

    if (passphrase.isEmpty())
        return -1;

    memcpy(passphraseBuffer, passphrase.toUtf8().constData(), qMin(maximumPassphraseSize, passphrase.size()));
    return passphrase.size();
}

bool QOpcUaKeyPairPrivate::loadFromPemData(const QByteArray &data) {
    Q_Q(QOpcUaKeyPair);

    if (m_keyData) {
        QtOpcUaSsl::q_EVP_PKEY_free(m_keyData);
        m_keyData = nullptr;
    }
    m_hasPrivateKey = false;

    BIO *bio = QtOpcUaSsl::q_BIO_new_mem_buf((void *)data.constData(), data.size());
    if (!bio) {
        qCWarning(lcOpcUaSsl) << "Failed to allocate a buffer:" << getOpenSslError();
        return false;
    }
    Deleter<BIO> bioDeleter(bio, QtOpcUaSsl::q_BIO_free_all);

    if (data.startsWith("-----BEGIN PRIVATE KEY-----") || data.startsWith("-----BEGIN ENCRYPTED PRIVATE KEY-----")) {
        if (!QtOpcUaSsl::q_PEM_read_bio_PrivateKey(bio, &m_keyData, &passwordCallback, q /* userData */)) {
            qCWarning(lcOpcUaSsl) << "Failed to load private key:" << getOpenSslError();
            return false;
        }
        m_hasPrivateKey = true;
    } else {
        if (!QtOpcUaSsl::q_PEM_read_bio_PUBKEY(bio, &m_keyData, NULL, NULL)) {
            qCWarning(lcOpcUaSsl) << "Failed to load public key:" << getOpenSslError();
            return false;
        }
    }

    return true;
}

QByteArray QOpcUaKeyPairPrivate::publicKeyToByteArray() const
{
    if (!m_keyData) {
        qCWarning(lcOpcUaSsl) << "No public key to write";
        return QByteArray();
    }

    BIO *bio = QtOpcUaSsl::q_BIO_new(QtOpcUaSsl::q_BIO_s_mem());
    if (!bio) {
        qCWarning(lcOpcUaSsl) << "Failed to allocate a buffer:" << getOpenSslError();
        return QByteArray();
    }
    Deleter<BIO> bioDeleter(bio, QtOpcUaSsl::q_BIO_free_all);

    if (0 == QtOpcUaSsl::q_PEM_write_bio_PUBKEY(bio, m_keyData)) {
        qCWarning(lcOpcUaSsl) << "Failed to write public key:" << getOpenSslError();
        return QByteArray();
    }

    char *buf;
    int length = QtOpcUaSsl::q_BIO_get_mem_data(bio, &buf);
    QByteArray data(buf, length);
    return data;
}

bool QOpcUaKeyPairPrivate::generateRsaKey(QOpcUaKeyPair::RsaKeyStrength strength)
{
    if (m_keyData) {
        QtOpcUaSsl::q_EVP_PKEY_free(m_keyData);
        m_keyData = nullptr;
    }

    EVP_PKEY_CTX *ctx = QtOpcUaSsl::q_EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (!ctx) {
        qCWarning(lcOpcUaSsl) << "Failed to allocate context:" << getOpenSslError();
        return false;
    }
    Deleter<EVP_PKEY_CTX> ctxDeleter(ctx, QtOpcUaSsl::q_EVP_PKEY_CTX_free);

    if (QtOpcUaSsl::q_EVP_PKEY_keygen_init(ctx) <= 0) {
        qCWarning(lcOpcUaSsl) << "Failed to initialize context:" << getOpenSslError();
        return false;
    }

    if (QtOpcUaSsl::q_EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, static_cast<int>(strength)) <= 0) {
        qCWarning(lcOpcUaSsl) << "Failed to set context property:" << getOpenSslError();
        return false;
    }

    if (QtOpcUaSsl::q_EVP_PKEY_keygen(ctx, &m_keyData) <= 0) {
        qCWarning(lcOpcUaSsl) << "Failed to generate key:" << getOpenSslError();
        return false;
    }

    m_hasPrivateKey = true;
    return true;
}

QOpcUaKeyPair::KeyType QOpcUaKeyPairPrivate::keyType() const
{
    if (!m_keyData)
        return QOpcUaKeyPair::KeyType::Empty;
    switch (QtOpcUaSsl::q_EVP_PKEY_base_id(m_keyData)) {
    case EVP_PKEY_RSA:
        return QOpcUaKeyPair::KeyType::Rsa;
    default:
        return QOpcUaKeyPair::KeyType::Unknown;
    }
}

QByteArray QOpcUaKeyPairPrivate::privateKeyToByteArray(QOpcUaKeyPair::Cipher cipher, const QString &password) const
{
    if (!m_keyData) {
        qCWarning(lcOpcUaSsl) << "No private key to write";
        return QByteArray();
    }

    BIO *bio = QtOpcUaSsl::q_BIO_new(QtOpcUaSsl::q_BIO_s_mem());
    if (!bio) {
        qCWarning(lcOpcUaSsl) << "Failed to allocate a buffer:" << getOpenSslError();
        return QByteArray();
    }
    Deleter<BIO> bioDeleter(bio, QtOpcUaSsl::q_BIO_free_all);

    const EVP_CIPHER *enc = NULL;
    if (cipher == QOpcUaKeyPair::Cipher::Unencrypted)
        enc = NULL;
    else if (cipher == QOpcUaKeyPair::Cipher::Aes128Cbc)
        enc = QtOpcUaSsl::q_EVP_aes_128_cbc();
    else {
        qCWarning(lcOpcUaSsl) << "Unknown cipher given";
        return QByteArray();
    }

    if (0 == QtOpcUaSsl::q_PEM_write_bio_PKCS8PrivateKey(bio, m_keyData, enc,
                                         enc ? password.toUtf8().data() : NULL,
                                         enc ? password.size() : 0,
                                         NULL /* callback */, NULL /* userdata */)) {
        qCWarning(lcOpcUaSsl) << "Failed to write private key:" << getOpenSslError();
        return QByteArray();
    }

    char *buf;
    int length = QtOpcUaSsl::q_BIO_get_mem_data(bio, &buf);
    QByteArray data(buf, length);
    return data;
}

bool QOpcUaKeyPairPrivate::hasPrivateKey() const
{
    if (!m_keyData)
        return false;

    return m_hasPrivateKey;
}

QT_END_NAMESPACE

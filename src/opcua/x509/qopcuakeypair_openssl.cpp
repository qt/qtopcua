// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuakeypair_p.h"
#include "openssl_symbols_p.h"
#include "qopcuax509utils_p.h"

QT_BEGIN_NAMESPACE

QOpcUaKeyPairPrivate::QOpcUaKeyPairPrivate()
    : QObjectPrivate()
{
    if (!q_resolveOpenSslSymbols())
        qFatal("Failed to resolve symbols");

    q_ERR_load_crypto_strings();
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    q_OPENSSL_add_all_algorithms_noconf();
#endif
}

QOpcUaKeyPairPrivate::~QOpcUaKeyPairPrivate()
{
    if (m_keyData) {
        q_EVP_PKEY_free(m_keyData);
        m_keyData = nullptr;
    }
}

static int passwordCallback(char *passphraseBuffer, int maximumPassphraseSize, int writeOperation, void *userData) {
    QOpcUaKeyPair *source = reinterpret_cast<QOpcUaKeyPair*>(userData);
    QString passphrase;
    source->passphraseNeeded(passphrase, maximumPassphraseSize, writeOperation == 1);

    if (passphrase.isEmpty())
        return -1;

    memcpy(passphraseBuffer, passphrase.toUtf8().constData(), qMin(maximumPassphraseSize, passphrase.size()));
    return passphrase.size();
}

bool QOpcUaKeyPairPrivate::loadFromPemData(const QByteArray &data) {
    Q_Q(QOpcUaKeyPair);

    if (m_keyData) {
        q_EVP_PKEY_free(m_keyData);
        m_keyData = nullptr;
    }
    m_hasPrivateKey = false;

    BIO *bio = q_BIO_new_mem_buf((void *)data.constData(), data.size());
    if (!bio) {
        qCWarning(lcSsl) << "Failed to allocate a buffer:" << getOpenSslError();
        return false;
    }
    Deleter<BIO> bioDeleter(bio, q_BIO_free_all);

    if (data.startsWith("-----BEGIN PRIVATE KEY-----") || data.startsWith("-----BEGIN ENCRYPTED PRIVATE KEY-----")) {
        if (!q_PEM_read_bio_PrivateKey(bio, &m_keyData, &passwordCallback, q /* userData */)) {
            qCWarning(lcSsl) << "Failed to load private key:" << getOpenSslError();
            return false;
        }
        m_hasPrivateKey = true;
    } else {
        if (!q_PEM_read_bio_PUBKEY(bio, &m_keyData, NULL, NULL)) {
            qCWarning(lcSsl) << "Failed to load public key:" << getOpenSslError();
            return false;
        }
    }

    return true;
}

QByteArray QOpcUaKeyPairPrivate::publicKeyToByteArray() const
{
    if (!m_keyData) {
        qCWarning(lcSsl) << "No public key to write";
        return QByteArray();
    }

    BIO *bio = q_BIO_new(q_BIO_s_mem());
    if (!bio) {
        qCWarning(lcSsl) << "Failed to allocate a buffer:" << getOpenSslError();
        return QByteArray();
    }
    Deleter<BIO> bioDeleter(bio, q_BIO_free_all);

    if (0 == q_PEM_write_bio_PUBKEY(bio, m_keyData)) {
        qCWarning(lcSsl) << "Failed to write public key:" << getOpenSslError();
        return QByteArray();
    }

    char *buf;
    int length = q_BIO_get_mem_data(bio, &buf);
    QByteArray data(buf, length);
    return data;
}

bool QOpcUaKeyPairPrivate::generateRsaKey(QOpcUaKeyPair::RsaKeyStrength strength)
{
    if (m_keyData) {
        q_EVP_PKEY_free(m_keyData);
        m_keyData = nullptr;
    }

#if OPENSSL_VERSION_NUMBER >= 0x10100000L
    EVP_PKEY_CTX *ctx = q_EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (!ctx) {
        qCWarning(lcSsl) << "Failed to allocate context:" << getOpenSslError();
        return false;
    }
    Deleter<EVP_PKEY_CTX> ctxDeleter(ctx, q_EVP_PKEY_CTX_free);

    if (q_EVP_PKEY_keygen_init(ctx) <= 0) {
        qCWarning(lcSsl) << "Failed to initialize context:" << getOpenSslError();
        return false;
    }

    if (q_EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, static_cast<int>(strength)) <= 0) {
        qCWarning(lcSsl) << "Failed to set context property:" << getOpenSslError();
        return false;
    }

    if (q_EVP_PKEY_keygen(ctx, &m_keyData) <= 0) {
        qCWarning(lcSsl) << "Failed to generate key:" << getOpenSslError();
        return false;
    }

#else
    RSA *rsa;
    BIGNUM *publicExponent;

    publicExponent = q_BN_new();
    if (publicExponent == NULL) {
        qCWarning(lcSsl) << "Failed to allocate public exponent:" << getOpenSslError();
        return false;
    }
    Deleter<BIGNUM> publicExponentDeleter(publicExponent, q_BN_free);

    if (q_BN_set_word(publicExponent, RSA_F4) == 0) {
        qCWarning(lcSsl) << "Failed to set public exponent:" << getOpenSslError();
        return false;
    }

    rsa = q_RSA_new();
    if (rsa == NULL) {
        qCWarning(lcSsl) << "Failed to allocate RSA:" << getOpenSslError();
        return false;
    }
    Deleter<RSA> rsaDeleter(rsa, q_RSA_free);

    int result = q_RSA_generate_key_ex(rsa, static_cast<int>(strength), publicExponent, nullptr /* progress callback */);
    if (result == 0) {
        qCWarning(lcSsl) << "Failed to generate key:" << getOpenSslError();
        return false;
    }

    m_keyData = q_EVP_PKEY_new();
    if (!m_keyData) {
        qCWarning(lcSsl) << "Failed to allocate key data:" << getOpenSslError();
        return false;
    }

    if (!q_EVP_PKEY_set1_RSA(m_keyData, rsa)) {
        qCWarning(lcSsl) << "Failed to transfer key data:" << getOpenSslError();
        return false;
    }
#endif
    m_hasPrivateKey = true;
    return true;
}

QOpcUaKeyPair::KeyType QOpcUaKeyPairPrivate::keyType() const
{
    if (!m_keyData)
        return QOpcUaKeyPair::KeyType::Empty;
    switch (q_EVP_PKEY_base_id(m_keyData)) {
    case EVP_PKEY_RSA:
        return QOpcUaKeyPair::KeyType::Rsa;
    default:
        return QOpcUaKeyPair::KeyType::Unknown;
    }
}

QByteArray QOpcUaKeyPairPrivate::privateKeyToByteArray(QOpcUaKeyPair::Cipher cipher, const QString &password) const
{
    if (!m_keyData) {
        qCWarning(lcSsl) << "No private key to write";
        return QByteArray();
    }

    BIO *bio = q_BIO_new(q_BIO_s_mem());
    if (!bio) {
        qCWarning(lcSsl) << "Failed to allocate a buffer:" << getOpenSslError();
        return QByteArray();
    }
    Deleter<BIO> bioDeleter(bio, q_BIO_free_all);

    const EVP_CIPHER *enc = NULL;
    if (cipher == QOpcUaKeyPair::Cipher::Unencrypted)
        enc = NULL;
    else if (cipher == QOpcUaKeyPair::Cipher::Aes128Cbc)
        enc = q_EVP_aes_128_cbc();
    else {
        qCWarning(lcSsl) << "Unknown cipher given";
        return QByteArray();
    }

    if (0 == q_PEM_write_bio_PKCS8PrivateKey(bio, m_keyData, enc,
                                         enc ? password.toUtf8().data() : NULL,
                                         enc ? password.size() : 0,
                                         NULL /* callback */, NULL /* userdata */)) {
        qCWarning(lcSsl) << "Failed to write private key:" << getOpenSslError();
        return QByteArray();
    }

    char *buf;
    int length = q_BIO_get_mem_data(bio, &buf);
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

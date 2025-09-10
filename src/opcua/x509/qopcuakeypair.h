// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAKEYPAIR_H
#define QOPCUAKEYPAIR_H

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qsharedpointer.h>
#include <QtOpcUa/qopcuaglobal.h>

QT_BEGIN_NAMESPACE

class QOpcUaKeyPairPrivate;

class Q_OPCUA_EXPORT QOpcUaKeyPair : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QOpcUaKeyPair)

public:
    enum class RsaKeyStrength {
        Bits1024 = 1024,
        Bits2048 = 2048,
        Bits4096 = 4096,
    };

    enum class KeyType {
        Rsa,
        Empty,
        Unknown,
    };

    enum class Cipher {
        Aes128Cbc,
        Unencrypted,
    };

    QOpcUaKeyPair(QObject *parent = nullptr);

    virtual ~QOpcUaKeyPair();
    bool loadFromPemData(const QByteArray &data);
    QByteArray publicKeyToByteArray() const;
    QByteArray privateKeyToByteArray(Cipher cipher, const QString &password) const;
    KeyType type() const;
    bool hasPrivateKey() const;
    void generateRsaKey(QOpcUaKeyPair::RsaKeyStrength strength);

Q_SIGNALS:
    void passphraseNeeded(QString &passphrase, int maximumLength, bool writeOperation);

    friend class QOpcUaX509CertificateSigningRequestPrivate;
};

QT_END_NAMESPACE

#endif // QOPCUAKEYPAIR_H

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

#ifndef QOPCUAKEYPAIR_H
#define QOPCUAKEYPAIR_H

#include <QObject>
#include <QString>
#include <QSharedPointer>
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
        Bits4096 = 4096
    };

    enum class KeyType {
        Rsa,
        Empty,
        Unknown
    };

    enum class Cipher {
        Aes128Cbc,
        Unencrypted
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

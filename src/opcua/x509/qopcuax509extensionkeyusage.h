// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAX509EXTENSIONKEYUSAGE_H
#define QOPCUAX509EXTENSIONKEYUSAGE_H

#include "QtOpcUa/qopcuax509extension.h"
#include <QtOpcUa/qopcuaglobal.h>

QT_BEGIN_NAMESPACE

class Q_OPCUA_EXPORT QOpcUaX509ExtensionKeyUsage : public QOpcUaX509Extension
{
public:
    enum class KeyUsage : uint {
        DigitalSignature,
        NonRepudiation,
        KeyEncipherment,
        DataEncipherment,
        KeyAgreement,
        CertificateSigning,
        CrlSigning,
        EnciptherOnly,
        DecipherOnly
    };

    QOpcUaX509ExtensionKeyUsage();
    QOpcUaX509ExtensionKeyUsage(const QOpcUaX509ExtensionKeyUsage &);
    QOpcUaX509ExtensionKeyUsage &operator=(const QOpcUaX509ExtensionKeyUsage &);
    bool operator==(const QOpcUaX509ExtensionKeyUsage &rhs) const;
    ~QOpcUaX509ExtensionKeyUsage();

    void setKeyUsage(KeyUsage keyUsage, bool enable = true);
    bool keyUsage(KeyUsage) const;
};

#if QT_VERSION >= 0x060000
inline size_t qHash(const QOpcUaX509ExtensionKeyUsage::KeyUsage &key)
#else
inline uint qHash(const QOpcUaX509ExtensionKeyUsage::KeyUsage &key)
#endif
{
    return ::qHash(static_cast<uint>(key));
}

QT_END_NAMESPACE

#endif // QOPCUAX509EXTENSIONKEYUSAGE_H

// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAX509EXTENSIONEXTENDEDKEYUSAGE_H
#define QOPCUAX509EXTENSIONEXTENDEDKEYUSAGE_H

#include "QtOpcUa/qopcuax509extension.h"
#include <QtOpcUa/qopcuaglobal.h>

QT_BEGIN_NAMESPACE

class Q_OPCUA_EXPORT QOpcUaX509ExtensionExtendedKeyUsage : public QOpcUaX509Extension
{
public:
    enum class KeyUsage : uint {
        TlsWebServerAuthentication,
        TlsWebClientAuthentication,
        SignExecutableCode,
        EmailProtection,
    };

    QOpcUaX509ExtensionExtendedKeyUsage();
    QOpcUaX509ExtensionExtendedKeyUsage(const QOpcUaX509ExtensionExtendedKeyUsage &);
    QOpcUaX509ExtensionExtendedKeyUsage &operator=(const QOpcUaX509ExtensionExtendedKeyUsage &);
    bool operator==(const QOpcUaX509ExtensionExtendedKeyUsage &rhs) const;
    ~QOpcUaX509ExtensionExtendedKeyUsage();

    void setKeyUsage(KeyUsage keyUsage, bool enable = true);
    bool keyUsage(KeyUsage) const;
};

#if QT_VERSION >= 0x060000
inline size_t qHash(const QOpcUaX509ExtensionExtendedKeyUsage::KeyUsage &key)
#else
inline uint qHash(const QOpcUaX509ExtensionExtendedKeyUsage::KeyUsage &key)
#endif
{
    return ::qHash(static_cast<uint>(key));
}

QT_END_NAMESPACE

#endif // QOPCUAX509EXTENSIONEXTENDEDKEYUSAGE_H

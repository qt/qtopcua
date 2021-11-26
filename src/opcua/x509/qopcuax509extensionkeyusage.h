/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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

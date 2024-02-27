// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAX509CERTIFICATESIGNINGREQUEST_H
#define QOPCUAX509CERTIFICATESIGNINGREQUEST_H

#include "QtOpcUa/qopcuakeypair.h"
#include "QtOpcUa/qopcuax509extension.h"
#include "QtOpcUa/qopcuax509distinguishedname.h"
#include <QtOpcUa/qopcuaglobal.h>
#include <QtOpcUa/qopcuax509certificatesigningrequest.h>

QT_BEGIN_NAMESPACE

class QOpcUaX509CertificateSigningRequestPrivate;

class Q_OPCUA_EXPORT QOpcUaX509CertificateSigningRequest
{
    Q_DECLARE_PRIVATE(QOpcUaX509CertificateSigningRequest)

public:
    enum class MessageDigest {
        SHA256,
    };
    enum class Encoding {
        PEM,
        DER,
    };

    QOpcUaX509CertificateSigningRequest();
    ~QOpcUaX509CertificateSigningRequest();

    void setMessageDigest(MessageDigest digest);
    MessageDigest messageDigest() const;

    void setEncoding(Encoding encoding);
    Encoding encoding() const;

    void setSubject(const QOpcUaX509DistinguishedName &subject);
    const QOpcUaX509DistinguishedName &subject() const;

    void addExtension(QOpcUaX509Extension *extension);
    QByteArray createRequest(const QOpcUaKeyPair &privateKey);
    QByteArray createSelfSignedCertificate(const QOpcUaKeyPair &privateKey, int validityInDays = 365);

private:
    QOpcUaX509CertificateSigningRequestPrivate *d_ptr = nullptr;
};

QT_END_NAMESPACE

#endif // QOPCUAX509CERTIFICATESIGNINGREQUEST_H

// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QOPCUAX509CERTIFICATESIGNINGREQUESTPRIVATE_H
#define QOPCUAX509CERTIFICATESIGNINGREQUESTPRIVATE_H

#include "QtOpcUa/qopcuax509extension.h"
#include "QtOpcUa/qopcuax509distinguishedname.h"
#include "QtOpcUa/qopcuakeypair.h"
#include <QList>
#include <QtOpcUa/qopcuaglobal.h>
#include "qopcuax509certificatesigningrequest.h"
#include "private/qglobal_p.h"

QT_BEGIN_NAMESPACE

class QOpcUaX509CertificateSigningRequestPrivate
{
public:
    QOpcUaX509CertificateSigningRequestPrivate();
    ~QOpcUaX509CertificateSigningRequestPrivate();

    void setMessageDigest(QOpcUaX509CertificateSigningRequest::MessageDigest);
    QOpcUaX509CertificateSigningRequest::MessageDigest messageDigest() const;

    QOpcUaX509CertificateSigningRequest::Encoding encoding() const;
    void setEncoding(QOpcUaX509CertificateSigningRequest::Encoding encoding);

    const QOpcUaX509DistinguishedName& subject() const;
    void setSubject(const QOpcUaX509DistinguishedName &subject);

    void addExtension(QOpcUaX509Extension *extension);
    QByteArray createRequest(const QOpcUaKeyPair &privateKey);
    QByteArray createSelfSignedCertificate(const QOpcUaKeyPair &privateKey, int validityInDays);

private:
    QList<QOpcUaX509Extension *> m_extensions;
    QOpcUaX509CertificateSigningRequest::MessageDigest m_messageDigest = QOpcUaX509CertificateSigningRequest::MessageDigest::SHA256;
    QOpcUaX509DistinguishedName m_subject;
    QOpcUaX509CertificateSigningRequest::Encoding m_encoding = QOpcUaX509CertificateSigningRequest::Encoding::PEM;

    Q_DECLARE_PUBLIC(QOpcUaX509CertificateSigningRequest)
    QOpcUaX509CertificateSigningRequest *q_ptr = nullptr;
};

QT_END_NAMESPACE

#endif // QOPCUAX509CERTIFICATESIGNINGREQUESTPRIVATE_H

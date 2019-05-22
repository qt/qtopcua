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
#include <QVector>
#include <QtOpcUa/qopcuaglobal.h>
#include "qopcuax509certificatesigningrequest.h"

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
    QVector<QOpcUaX509Extension *> m_extensions;
    QOpcUaX509CertificateSigningRequest::MessageDigest m_messageDigest = QOpcUaX509CertificateSigningRequest::MessageDigest::SHA256;
    QOpcUaX509DistinguishedName m_subject;
    QOpcUaX509CertificateSigningRequest::Encoding m_encoding = QOpcUaX509CertificateSigningRequest::Encoding::PEM;

    Q_DECLARE_PUBLIC(QOpcUaX509CertificateSigningRequest)
    QOpcUaX509CertificateSigningRequest *q_ptr = nullptr;
};

QT_END_NAMESPACE

#endif // QOPCUAX509CERTIFICATESIGNINGREQUESTPRIVATE_H

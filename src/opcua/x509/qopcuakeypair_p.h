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

#ifndef QOPCUAPUBLICKEYPRIVATE_H
#define QOPCUAPUBLICKEYPRIVATE_H

#include <private/qobject_p.h>
#include "qopcuakeypair.h"
#include <QtCore/QLoggingCategory>
#include <openssl/rsa.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(lcSsl)

class QOpcUaKeyPairPrivate : public QObjectPrivate
{
public:
    QOpcUaKeyPairPrivate();
    ~QOpcUaKeyPairPrivate();

    bool generateRsaKey(QOpcUaKeyPair::RsaKeyStrength strength);
    bool loadFromPemData(const QByteArray &data);
    QByteArray publicKeyToByteArray() const;
    QByteArray privateKeyToByteArray(QOpcUaKeyPair::Cipher cipher, const QString &password) const;
    QOpcUaKeyPair::KeyType keyType() const;
    bool hasPrivateKey() const;

protected:
    EVP_PKEY *m_keyData = nullptr;
    bool m_hasPrivateKey = false;

private:
    Q_DECLARE_PUBLIC(QOpcUaKeyPair)

    friend class QOpcUaX509CertificateSigningRequestPrivate;
};
QT_END_NAMESPACE

#endif // QOPCUAPUBLICKEYPRIVATE_H

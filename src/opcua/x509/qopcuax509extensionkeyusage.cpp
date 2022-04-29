/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#include "qopcuax509extensionkeyusage.h"
#include "qopcuax509extension_p.h"
#include <QSet>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaX509ExtensionKeyUsage
    \inmodule QtOpcUa
    \since 5.14

    \brief Class for X509 extended key usage.

    This class is currently available as a Technology Preview, and therefore the API
    and functionality provided by the class may be subject to change at any time without
    prior notice.
*/

/*!
    \enum QOpcUaX509ExtensionKeyUsage::KeyUsage

    Enum with entry types for X509ExtensionKeyUsage.

    \value DigitalSignature
        Permits digital signatures
    \value NonRepudiation
        Permits non repudiation
    \value KeyEncipherment
        Permits key encipherment
    \value DataEncipherment
        Permits data encipherment
    \value KeyAgreement
        Permits key agreement
    \value CertificateSigning
        Permits certificate signing
    \value CrlSigning
        Permits CRL signing
    \value EnciptherOnly
        Restricts to encipherment only
    \value DecipherOnly
        Restricts to decipher only
*/

class QOpcUaX509ExtensionKeyUsageData : public QOpcUaX509ExtensionData
{
public:
    ~QOpcUaX509ExtensionKeyUsageData() override = default;
    QSet<QOpcUaX509ExtensionKeyUsage::KeyUsage> keyUsage;
};

/*!
    Constructs a X509ExtensionKeyUsage.
*/
QOpcUaX509ExtensionKeyUsage::QOpcUaX509ExtensionKeyUsage()
    : QOpcUaX509Extension(new QOpcUaX509ExtensionKeyUsageData)
{
}

/*!
    Constructs a X509ExtensionKeyUsage from \a rhs.
*/
QOpcUaX509ExtensionKeyUsage::QOpcUaX509ExtensionKeyUsage(const QOpcUaX509ExtensionKeyUsage &rhs)
    : QOpcUaX509Extension(rhs.data)
{
}

/*!
    Returns \c true if this X509ExtensionKeyUsage has the same value as \a rhs.
*/
bool QOpcUaX509ExtensionKeyUsage::operator==(const QOpcUaX509ExtensionKeyUsage &rhs) const
{
    return data->critical == rhs.data->critical;
}

/*!
    Destructs a X509ExtensionKeyUsage.
*/
QOpcUaX509ExtensionKeyUsage::~QOpcUaX509ExtensionKeyUsage()
{
}

/*!
    Sets the values from \a rhs in this X509ExtensionKeyUsage.
*/
QOpcUaX509ExtensionKeyUsage &QOpcUaX509ExtensionKeyUsage::operator=(const QOpcUaX509ExtensionKeyUsage &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Sets the key usage flag in \a keyUsage to \a enable.
*/
void QOpcUaX509ExtensionKeyUsage::setKeyUsage(KeyUsage keyUsage, bool enable)
{
    QOpcUaX509ExtensionKeyUsageData *d = static_cast<QOpcUaX509ExtensionKeyUsageData*>(data.data());

    if (enable)
        d->keyUsage.insert(keyUsage);
    else
        d->keyUsage.remove(keyUsage);
}

/*!
    Returns the key usage flag for \a keyUsage.
*/
bool QOpcUaX509ExtensionKeyUsage::keyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage keyUsage) const
{
    const QOpcUaX509ExtensionKeyUsageData *d = static_cast<const QOpcUaX509ExtensionKeyUsageData*>(data.data());
    return d->keyUsage.contains(keyUsage);
}

QT_END_NAMESPACE

// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuax509extensionextendedkeyusage.h"
#include "qopcuax509extension_p.h"
#include <QSet>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaX509ExtensionExtendedKeyUsage
    \inmodule QtOpcUa
    \since 5.14

    \brief Class for X509 extended key usage.

    This class is currently available as a Technology Preview, and therefore the API
    and functionality provided by the class may be subject to change at any time without
    prior notice.
*/

/*!
    \enum QOpcUaX509ExtensionExtendedKeyUsage::KeyUsage

    Enum with entry types for X509ExtensionExtendedKeyUsage.

    \value TlsWebServerAuthentication
        Permits TLS webserver Authentication
    \value TlsWebClientAuthentication
        Permits TLS client authentication
    \value SignExecutableCode
        Permits signature of executable code
    \value EmailProtection
        Permits signing emails
*/

class QOpcUaX509ExtensionExtendedKeyUsageData : public QOpcUaX509ExtensionData
{
public:
    ~QOpcUaX509ExtensionExtendedKeyUsageData() override = default;
    QSet<QOpcUaX509ExtensionExtendedKeyUsage::KeyUsage> keyUsage;
};

/*!
    Constructs a X509ExtensionExtendedKeyUsage.
*/
QOpcUaX509ExtensionExtendedKeyUsage::QOpcUaX509ExtensionExtendedKeyUsage()
    : QOpcUaX509Extension(new QOpcUaX509ExtensionExtendedKeyUsageData)
{

}

/*!
    Constructs a X509ExtensionExtendedKeyUsage from \a rhs.
*/
QOpcUaX509ExtensionExtendedKeyUsage::QOpcUaX509ExtensionExtendedKeyUsage(const QOpcUaX509ExtensionExtendedKeyUsage &rhs)
    : QOpcUaX509Extension(rhs.data)
{
}

/*!
    Returns \c true if this X509ExtensionExtendedKeyUsage has the same value as \a rhs.
*/
bool QOpcUaX509ExtensionExtendedKeyUsage::operator==(const QOpcUaX509ExtensionExtendedKeyUsage &rhs) const
{
    return data->critical == rhs.data->critical;
}

/*!
    Destructs a X509ExtensionExtendedKeyUsage.
*/
QOpcUaX509ExtensionExtendedKeyUsage::~QOpcUaX509ExtensionExtendedKeyUsage()
{
}

/*!
    Sets the values from \a rhs in this X509ExtensionExtendedKeyUsage.
*/
QOpcUaX509ExtensionExtendedKeyUsage &QOpcUaX509ExtensionExtendedKeyUsage::operator=(const QOpcUaX509ExtensionExtendedKeyUsage &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Sets the key usage flag in \a keyUsage to \a enable.
*/
void QOpcUaX509ExtensionExtendedKeyUsage::setKeyUsage(KeyUsage keyUsage, bool enable)
{
    QOpcUaX509ExtensionExtendedKeyUsageData *d = static_cast<QOpcUaX509ExtensionExtendedKeyUsageData*>(data.data());

    if (enable)
        d->keyUsage.insert(keyUsage);
    else
        d->keyUsage.remove(keyUsage);
}

/*!
    Returns the key usage flag for \a keyUsage.
*/
bool QOpcUaX509ExtensionExtendedKeyUsage::keyUsage(QOpcUaX509ExtensionExtendedKeyUsage::KeyUsage keyUsage) const
{
    const QOpcUaX509ExtensionExtendedKeyUsageData *d = static_cast<const QOpcUaX509ExtensionExtendedKeyUsageData*>(data.data());
    return d->keyUsage.contains(keyUsage);
}

QT_END_NAMESPACE

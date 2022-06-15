// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuax509extension.h"
#include "qopcuax509extension_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaX509Extension
    \inmodule QtOpcUa
    \since 5.14

    \brief Base class for all X509 extensions.

    This class is currently available as a Technology Preview, and therefore the API
    and functionality provided by the class may be subject to change at any time without
    prior notice.

    \sa QOpcUaX509ExtensionSubjectAlternativeName, QOpcUaX509ExtensionBasicConstraints,
        QOpcUaX509ExtensionKeyUsage, QOpcUaX509ExtensionKeyUsage
*/

/*!
    Constructs a default X509Extension.
*/
QOpcUaX509Extension::QOpcUaX509Extension()
    : data(new QOpcUaX509ExtensionData)
{
}

/*!
    Constructs a X509Extension from \a rhs.
*/
QOpcUaX509Extension::QOpcUaX509Extension(const QOpcUaX509Extension &rhs)
    : data(rhs.data)
{
}

/*!
    Constructs a X509Extension from \a rhs.
*/
QOpcUaX509Extension::QOpcUaX509Extension(QSharedDataPointer<QOpcUaX509ExtensionData> rhs)
    : data(rhs)
{
}

/*!
    Returns \c true if this X509Extension has the same value as \a rhs.
*/
bool QOpcUaX509Extension::operator==(const QOpcUaX509Extension &rhs) const
{
    return data->critical == rhs.data->critical;
}

/*!
    Sets the values from \a rhs in this X509Extension.
*/
QOpcUaX509Extension &QOpcUaX509Extension::operator=(const QOpcUaX509Extension &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Destructs a X509Extension.
*/
QOpcUaX509Extension::~QOpcUaX509Extension()
{
}

/*!
    Sets the critical flag to \a critical.
*/
void QOpcUaX509Extension::setCritical(bool critical)
{
    data->critical = critical;
}

/*!
    Return the state of the critical flag.
*/
bool QOpcUaX509Extension::critical() const
{
    return data->critical;
}

QOpcUaX509Extension::QOpcUaX509Extension(QOpcUaX509ExtensionData *other)
{
    data = other;
}

QT_END_NAMESPACE

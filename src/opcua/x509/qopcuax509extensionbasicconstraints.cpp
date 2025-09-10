// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuax509extensionbasicconstraints.h"
#include "qopcuax509extension_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaX509ExtensionBasicConstraints
    \inmodule QtOpcUa
    \since 5.14

    \brief Class for X509 basic constraints.

    This class is currently available as a Technology Preview, and therefore the API
    and functionality provided by the class may be subject to change at any time without
    prior notice.
*/

class QOpcUaX509ExtensionBasicConstraintsData : public QOpcUaX509ExtensionData
{
public:
    ~QOpcUaX509ExtensionBasicConstraintsData() override = default;
    bool ca = false;
    int pathLength = -1;
};

/*!
    Constructs a default X509ExtensionBasicConstraints.
*/
QOpcUaX509ExtensionBasicConstraints::QOpcUaX509ExtensionBasicConstraints()
    : QOpcUaX509Extension(new QOpcUaX509ExtensionBasicConstraintsData)
{
}

/*!
    Constructs a X509ExtensionBasicConstraints from \a rhs.
*/
QOpcUaX509ExtensionBasicConstraints::QOpcUaX509ExtensionBasicConstraints(const QOpcUaX509ExtensionBasicConstraints &rhs)
    : QOpcUaX509Extension(rhs.data)
{
}

/*!
    Returns \c true if this X509ExtensionBasicConstraints has the same value as \a rhs.
*/
bool QOpcUaX509ExtensionBasicConstraints::operator==(const QOpcUaX509ExtensionBasicConstraints &rhs) const
{
    return data->critical == rhs.data->critical;
}

/*!
    Destructs a X509ExtensionBasicConstraints.
*/
QOpcUaX509ExtensionBasicConstraints::~QOpcUaX509ExtensionBasicConstraints()
{
}

/*!
    Sets the values from \a rhs in this X509ExtensionBasicConstraints.
*/
QOpcUaX509ExtensionBasicConstraints &QOpcUaX509ExtensionBasicConstraints::operator=(const QOpcUaX509ExtensionBasicConstraints &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Sets the flag, if the certificate's subject is a CA to \a value.
*/
void QOpcUaX509ExtensionBasicConstraints::setCa(bool value)
{
    QOpcUaX509ExtensionBasicConstraintsData *d = static_cast<QOpcUaX509ExtensionBasicConstraintsData*>(data.data());
    d->ca = value;
}

/*!
    Returns the flag, if the certificate's subject is a CA.
*/
bool QOpcUaX509ExtensionBasicConstraints::ca() const
{
    const QOpcUaX509ExtensionBasicConstraintsData *d = static_cast<const QOpcUaX509ExtensionBasicConstraintsData*>(data.data());
    return d->ca;
}

/*!
    Sets the validation path length of the certificate to \a length.
*/
void QOpcUaX509ExtensionBasicConstraints::setPathLength(int length)
{
    QOpcUaX509ExtensionBasicConstraintsData *d = static_cast<QOpcUaX509ExtensionBasicConstraintsData*>(data.data());
    d->pathLength = length;
}

/*!
    Returns the validation path length of the certificate.
*/
int QOpcUaX509ExtensionBasicConstraints::pathLength() const
{
    const QOpcUaX509ExtensionBasicConstraintsData *d = static_cast<const QOpcUaX509ExtensionBasicConstraintsData*>(data.data());
    return d->pathLength;
}

QT_END_NAMESPACE

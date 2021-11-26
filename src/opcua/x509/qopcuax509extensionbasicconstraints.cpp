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

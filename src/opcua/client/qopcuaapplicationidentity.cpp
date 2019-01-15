/****************************************************************************
**
** Copyright (C) 2018 Unified Automation GmbH
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
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

#include "qopcuaapplicationidentity.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaApplicationIdentity
    \inmodule QtOpcUa
    \since QtOpcUa 5.13
    \brief QOpcUaApplicationIdentity defines the identity of the application.

    This class is currently available as a Technology Preview, and therefore the API
    and functionality provided by the class may be subject to change at any time without
    prior notice.

    This info must be configured using \l QOpcUaClient::setIdentity.
    The application identity can be set up manually or derived from a certificate.

    \code
    QOpcUaApplicationIdentity identity;

    const QString applicationUri = QStringLiteral("urn:%1:%2:%3")
            .arg(QHostInfo::localHostName())
            .arg(QCoreApplication::organizationName())
            .arg(QCoreApplication::applicationName());
    const QString productUri = QStringLiteral("urn:%1:%2")
            .arg(QCoreApplication::organizationName())
            .arg(QCoreApplication::applicationName());

    identity.setProductUri(QUrl::toPercentEncoding(productUri));
    identity.setApplicationUri(QUrl::toPercentEncoding(applicationUri));
    identity.setApplicationName(QCoreApplication::applicationName());
    identity.setApplicationType(QOpcUaApplicationDescription::Client);

    client->setIdentity(identity);
    \endcode

    In case your application authenticates using certificates the application identity has to match
    the used certificate. In this case all information is extracted from the certificate given in the
    PKI configuration.

    \code
    QOpcUaApplicationIdentity identity;
    identity = pkiConfig.applicationIdentity();
    \endcode
*/

class QOpcUaApplicationIdentityData : public QSharedData
{
public:
    QOpcUaApplicationIdentityData() {}

    QOpcUaApplicationDescription::ApplicationType m_applicationType
        {QOpcUaApplicationDescription::ApplicationType::Client};
    QString m_applicationUri;
    QString m_applicationName;
    QString m_productUri;
};

QOpcUaApplicationIdentity::QOpcUaApplicationIdentity()
    : data(new QOpcUaApplicationIdentityData())
{}

QOpcUaApplicationIdentity::~QOpcUaApplicationIdentity()
{}

/*!
    Constructs an application identity from \a other.
*/
QOpcUaApplicationIdentity::QOpcUaApplicationIdentity(const QOpcUaApplicationIdentity &other)
    : data(other.data)
{}

/*!
    Sets the values of \a rhs in this \l QOpcUaApplicationIdentity.
*/
QOpcUaApplicationIdentity &QOpcUaApplicationIdentity::operator=(const QOpcUaApplicationIdentity &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns the application's application URI.

    This must be unique for each installation instance of the application and must match the ApplicationURI
    in the application's certificate.
*/
QString QOpcUaApplicationIdentity::applicationUri() const
{
    return data->m_applicationUri;
}

/*!
    Sets the applicationUri to \a value.

    \sa setApplicationName()
*/
void QOpcUaApplicationIdentity::setApplicationUri(const QString &value)
{
    data->m_applicationUri = value;
}

/*!
    Returns the human readable name of the application.
    This does not need to be unique.
*/
QString QOpcUaApplicationIdentity::applicationName() const
{
    return data->m_applicationName;
}

/*!
    Sets the application name to \a value.
*/
void QOpcUaApplicationIdentity::setApplicationName(const QString &value)
{
    data->m_applicationName = value;
}

/*!
    Returns the application's productUri.

    This uniquely identifies the product.
*/
QString QOpcUaApplicationIdentity::productUri() const
{
    return data->m_productUri;
}

/*!
    Sets the productUri to \a value.
*/
void QOpcUaApplicationIdentity::setProductUri(const QString &value)
{
    data->m_productUri = value;
}

/*!
    Returns the application's type.
*/
QOpcUaApplicationDescription::ApplicationType QOpcUaApplicationIdentity::applicationType() const
{
    return data->m_applicationType;
}

/*!
    Sets the type of the application.
    Client applications should set this to \l {QOpcUaApplicationDescription::Client}{Client}.

    The default value is \l{QOpcUaApplicationDescription::Client}{Client}.
*/
void QOpcUaApplicationIdentity::setApplicationType(QOpcUaApplicationDescription::ApplicationType value)
{
    data->m_applicationType = value;
}

/*!
    Returns true if the application identity contains valid data.
*/
bool QOpcUaApplicationIdentity::isValid() const
{
    return !data->m_applicationUri.isEmpty() &&
           !data->m_applicationName.isEmpty() &&
           !data->m_productUri.isEmpty();
}

QT_END_NAMESPACE

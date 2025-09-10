// Copyright (C) 2019 The Qt Company Ltd.
// Copyright (C) 2018 Unified Automation GmbH
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuaapplicationidentity.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaApplicationIdentity
    \inmodule QtOpcUa
    \since QtOpcUa 5.13
    \brief QOpcUaApplicationIdentity defines the identity of the application.

    This info must be configured using \l QOpcUaClient::setApplicationIdentity.
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

    identity.setProductUri(productUri);
    identity.setApplicationUri(applicationUri);
    identity.setApplicationName(QCoreApplication::applicationName());
    identity.setApplicationType(QOpcUaApplicationDescription::Client);

    client->setApplicationIdentity(identity);
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

/*!
    Default constructs an application identity with no parameters set.
*/
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
    Client applications should set \a value to \l {QOpcUaApplicationDescription::Client}{Client}.

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

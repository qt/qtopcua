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

#include "qopcuaauthenticationinformation.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaAuthenticationInformation
    \inmodule QtOpcUa
    \brief The OPC UA authentication information.
    \since QtOpcUa 5.13

    This class holds the information necessary to perform a login on a server.
    Supported authentication mechanisms are

    \list
    \li Anonymous
    \li Username
    \li Certificate
    \endlist

    The anonymous method is used by default but also can be set manually.

    This is an example authentication using username and password.

    \code
    QOpcUaAuthenticationInformation authInfo;
    authInfo.setUsernameAuthentication("user", "password");

    m_client->setAuthenticationInformation(authInfo);
    m_client->connectToEndpoint(endpoint);
    \endcode

    \sa setAnonymousAuthentication() setUsernameAuthentication() setCertificateAuthentication()
*/

/*!
    \qmltype AuthenticationInformation
    \inqmlmodule QtOpcUa
    \brief The OPC UA authentication information.
    \since QtOpcUa 5.13

    This class holds the information necessary to perform a login on a server.

    \code
        var authInfo = connection.authenticationInformation;
        authInfo.setUsernameAuthentication("user1", "password");
        connection.authenticationInformation = authInfo;
    \endcode

    Current supported authentication methods are:

    \list
    \li Anonymous
    \li Usernane
    \li Certificate
    \endlist

    \sa setAnonymousAuthentication() setUsernameAuthentication()
*/

class QOpcUaAuthenticationInformationData : public QSharedData
{
public:
    QVariant authenticationData;
    QOpcUaUserTokenPolicy::TokenType authenticationType;
};

QOpcUaAuthenticationInformation::QOpcUaAuthenticationInformation()
    : data(new QOpcUaAuthenticationInformationData)
{
    setAnonymousAuthentication();
}

/*!
    Constructs an authentication information from \a rhs.
*/
QOpcUaAuthenticationInformation::QOpcUaAuthenticationInformation(const QOpcUaAuthenticationInformation &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this authentication information.
*/
QOpcUaAuthenticationInformation &QOpcUaAuthenticationInformation::operator=(const QOpcUaAuthenticationInformation &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this authentication information has the same value as \a rhs.
*/
bool QOpcUaAuthenticationInformation::operator==(const QOpcUaAuthenticationInformation &rhs) const
{
    return data->authenticationType == rhs.data->authenticationType &&
            data->authenticationData == rhs.data->authenticationData;
}

QOpcUaAuthenticationInformation::~QOpcUaAuthenticationInformation()
{
}

/*!
    \qmlmethod AuthenticationInformation::setAnonymousAuthentication()

    Sets the authentication method to anonymous.
*/

/*!
    Sets the authentication method to anonymous.
*/
void QOpcUaAuthenticationInformation::setAnonymousAuthentication()
{
    data->authenticationType = QOpcUaUserTokenPolicy::TokenType::Anonymous;
    data->authenticationData = QVariant();
}

/*!
    \qmlmethod AuthenticationInformation::setUsernameAuthentication(string username, string password)

    Sets the authentication method to username, using the given \a username and \a password.
*/

/*!
    Sets the authentication method to username, using the given \a username and \a password.
*/
void QOpcUaAuthenticationInformation::setUsernameAuthentication(const QString &username, const QString &password)
{
    data->authenticationType = QOpcUaUserTokenPolicy::TokenType::Username;
    data->authenticationData = QVariant::fromValue(qMakePair(username, password));
}

/*!
    Sets the authentication method to use certificates.

    When using this authentication type a proper configured \l QOpcUaPkiConfiguration has to be set to
    the \l QOpcUaClient.

    \sa QOpcUaPkiConfiguration QOpcUaClient::setPkiConfiguration()
*/
void QOpcUaAuthenticationInformation::setCertificateAuthentication()
{
    data->authenticationData = QVariant();
    data->authenticationType = QOpcUaUserTokenPolicy::TokenType::Certificate;
}

/*!
    The content of the \l QVariant returned by this method depends on the currently selected authentication method.
 */
const QVariant &QOpcUaAuthenticationInformation::authenticationData() const
{
    return data->authenticationData;
}

/*!
    Returns the current authentication type.

    \sa QOpcUaUserTokenPolicy::TokenType
 */
QOpcUaUserTokenPolicy::TokenType QOpcUaAuthenticationInformation::authenticationType() const
{
    return data->authenticationType;
}

QT_END_NAMESPACE

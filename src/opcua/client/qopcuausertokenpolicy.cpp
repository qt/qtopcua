/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
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

#include "qopcuausertokenpolicy.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaUserTokenPolicy
    \inmodule QtOpcUa
    \brief The OPC UA UserTokenPolicy.

    The user token policy contains information about an user token accepted by the server.
*/

/*!
    \qmltype UserTokenPolicy
    \inqmlmodule QtOpcUa
    \brief The OPC UA ApplicationDescription.
    \since QtOpcUa 5.13

    The user token policy contains information about an user token accepted by the server.
*/

/*!
    \enum QOpcUaUserTokenPolicy::TokenType

    \value Anonymous No token required.
    \value Username Username and password are required.
    \value Certificate A client certificate is required.
    \value IssuedToken Any Web Services Security (WS-Security) defined token.
*/

/*!
    \qmlproperty enumeration UserTokenPolicy::TokenType

    \value Anonymous No token required.
    \value Username Username and password are required.
    \value Certificate A client certificate is required.
    \value IssuedToken Any Web Services Security (WS-Security) defined token.
*/

class QOpcUaUserTokenPolicyData : public QSharedData
{
public:
    QString policyId;
    QOpcUaUserTokenPolicy::TokenType tokenType{QOpcUaUserTokenPolicy::TokenType::Anonymous};
    QString issuedTokenType;
    QString issuerEndpointUrl;
    QString securityPolicy;
};

QOpcUaUserTokenPolicy::QOpcUaUserTokenPolicy()
    : data(new QOpcUaUserTokenPolicyData)
{
}

/*!
    Constructs an user token policy from \a rhs.
*/
QOpcUaUserTokenPolicy::QOpcUaUserTokenPolicy(const QOpcUaUserTokenPolicy &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this user token policy.
*/
QOpcUaUserTokenPolicy &QOpcUaUserTokenPolicy::operator=(const QOpcUaUserTokenPolicy &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this user token policy has the same value as \a rhs.
*/
bool QOpcUaUserTokenPolicy::operator==(const QOpcUaUserTokenPolicy &rhs) const
{
    return rhs.policyId() == policyId() &&
            rhs.tokenType() == tokenType() &&
            rhs.issuedTokenType() == issuedTokenType() &&
            rhs.issuerEndpointUrl() == issuerEndpointUrl() &&
            rhs.securityPolicy() == securityPolicy();
}

QOpcUaUserTokenPolicy::~QOpcUaUserTokenPolicy()
{
}

/*!
    Returns the URI of the security policy required when encrypting or signing the token for ActivateSession.
*/
QString QOpcUaUserTokenPolicy::securityPolicy() const
{
    return data->securityPolicy;
}

/*!
    Sets the URI of the security policy to \a securityPolicy.
*/
void QOpcUaUserTokenPolicy::setSecurityPolicy(const QString &securityPolicy)
{
    data->securityPolicy = securityPolicy;
}

/*!
    Returns the URL of a token issuing service.
*/
QString QOpcUaUserTokenPolicy::issuerEndpointUrl() const
{
    return data->issuerEndpointUrl;
}

/*!
    Sets the URL of the token issuing service to \a issuerEndpointUrl.
*/
void QOpcUaUserTokenPolicy::setIssuerEndpointUrl(const QString &issuerEndpointUrl)
{
    data->issuerEndpointUrl = issuerEndpointUrl;
}

/*!
    Returns the URI for the token type.
*/
QString QOpcUaUserTokenPolicy::issuedTokenType() const
{
    return data->issuedTokenType;
}

/*!
    Sets the URI for the token type to \a issuedTokenType.
*/
void QOpcUaUserTokenPolicy::setIssuedTokenType(const QString &issuedTokenType)
{
    data->issuedTokenType = issuedTokenType;
}

/*!
    Returns the type of the required user identity token.
*/
QOpcUaUserTokenPolicy::TokenType QOpcUaUserTokenPolicy::tokenType() const
{
    return data->tokenType;
}

/*!
    Sets the type of the required user identity token to \a tokenType.
*/
void QOpcUaUserTokenPolicy::setTokenType(TokenType tokenType)
{
    data->tokenType = tokenType;
}

/*!
    Returns a server assigned identifier for this policy.
*/
QString QOpcUaUserTokenPolicy::policyId() const
{
    return data->policyId;
}

/*!
    Sets the identifier for this policy to \a policyId.
*/
void QOpcUaUserTokenPolicy::setPolicyId(const QString &policyId)
{
    data->policyId = policyId;
}

QT_END_NAMESPACE

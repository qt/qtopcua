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

#include "qopcuaendpointdescription.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaEndpointDescription
    \inmodule QtOpcUa
    \brief The OPC UA EndpointDescription.

    An endpoint description contains information about an endpoint and how to connect to it.
*/

/*!
    \enum QOpcUaEndpointDescription::MessageSecurityMode

    \value Invalid The default value, will be rejected by the server.
    \value None No security.
    \value Sign Messages are signed but not encrypted.
    \value SignAndEncrypt Messages are signed and encrypted.
*/

class QOpcUaEndpointDescriptionData : public QSharedData
{
public:
    QString endpointUrl;
    QOpcUaApplicationDescription server;
    QByteArray serverCertificate;
    QOpcUaEndpointDescription::MessageSecurityMode securityMode{QOpcUaEndpointDescription::MessageSecurityMode::None};
    QString securityPolicyUri;
    QVector<QOpcUaUserTokenPolicy> userIdentityTokens;
    QString transportProfileUri;
    quint8 securityLevel{0};
};

QOpcUaEndpointDescription::QOpcUaEndpointDescription()
    : data(new QOpcUaEndpointDescriptionData)
{
}

/*!
    Constructs an endpoint description from \a rhs.
*/
QOpcUaEndpointDescription::QOpcUaEndpointDescription(const QOpcUaEndpointDescription &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this endpoint description.
*/
QOpcUaEndpointDescription &QOpcUaEndpointDescription::operator=(const QOpcUaEndpointDescription &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this endpoint description has the same value as \a rhs.
 */
bool QOpcUaEndpointDescription::operator==(const QOpcUaEndpointDescription &rhs) const
{
    return rhs.server() == server() &&
            rhs.endpointUrl() == endpointUrl() &&
            rhs.securityMode() == securityMode() &&
            rhs.securityLevel() == securityLevel() &&
            rhs.securityPolicyUri() == securityPolicyUri() &&
            rhs.serverCertificate() == serverCertificate() &&
            rhs.userIdentityTokens() == userIdentityTokens() &&
            rhs.transportProfileUri() == transportProfileUri();
}

QOpcUaEndpointDescription::~QOpcUaEndpointDescription()
{
}

/*!
    Returns a relative index assigned by the server. It describes how secure this
    endpoint is compared to other endpoints of the same server. An endpoint with strong
    security measures has a higher security level than one with weaker or no security
    measures.

    Security level 0 indicates an endpoint for backward compatibility purposes which
    should only be used if the client does not support the security measures required
    by more secure endpoints.
*/
quint8 QOpcUaEndpointDescription::securityLevel() const
{
    return data->securityLevel;
}

/*!
    Sets the security level to \a securityLevel.
*/
void QOpcUaEndpointDescription::setSecurityLevel(quint8 securityLevel)
{
    data->securityLevel = securityLevel;
}

/*!
    Returns the URI of the transport profile supported by the endpoint.
*/
QString QOpcUaEndpointDescription::transportProfileUri() const
{
    return data->transportProfileUri;
}

/*!
    Sets the URI of the transport profile supported by the endpoint to \a transportProfileUri.
*/
void QOpcUaEndpointDescription::setTransportProfileUri(const QString &transportProfileUri)
{
    data->transportProfileUri = transportProfileUri;
}

/*!
    Returns a list of user identity tokens the endpoint will accept.
*/
QVector<QOpcUaUserTokenPolicy> QOpcUaEndpointDescription::userIdentityTokens() const
{
    return data->userIdentityTokens;
}

/*!
    Returns a reference to a list of user identity tokens the endpoint will accept.
*/
QVector<QOpcUaUserTokenPolicy> &QOpcUaEndpointDescription::userIdentityTokensRef()
{
    return data->userIdentityTokens;
}

/*!
    Sets the user identity tokens to \a userIdentityTokens.
*/
void QOpcUaEndpointDescription::setUserIdentityTokens(const QVector<QOpcUaUserTokenPolicy> &userIdentityTokens)
{
    data->userIdentityTokens = userIdentityTokens;
}

/*!
    Returns the URI of the security policy.
*/
QString QOpcUaEndpointDescription::securityPolicyUri() const
{
    return data->securityPolicyUri;
}

/*!
    Sets the URI of the security policy to \a securityPolicyUri.
*/
void QOpcUaEndpointDescription::setSecurityPolicyUri(const QString &securityPolicyUri)
{
    data->securityPolicyUri = securityPolicyUri;
}

/*!
    Returns the security mode supported by this endpoint.
*/
QOpcUaEndpointDescription::MessageSecurityMode QOpcUaEndpointDescription::securityMode() const
{
    return data->securityMode;
}

/*!
    Sets the security mode supported by this endpoint to \a securityMode.
*/
void QOpcUaEndpointDescription::setSecurityMode(MessageSecurityMode securityMode)
{
    data->securityMode = securityMode;
}

/*!
    Returns the application instance certificate of the server.
*/
QByteArray QOpcUaEndpointDescription::serverCertificate() const
{
    return data->serverCertificate;
}

/*!
    Sets the application instance certificate of the server to \a serverCertificate.
*/
void QOpcUaEndpointDescription::setServerCertificate(const QByteArray &serverCertificate)
{
    data->serverCertificate = serverCertificate;
}

/*!
    Returns the application description of the server.
*/
QOpcUaApplicationDescription QOpcUaEndpointDescription::server() const
{
    return data->server;
}

/*!
    Returns a reference to the application description of the server.
*/
QOpcUaApplicationDescription &QOpcUaEndpointDescription::serverRef()
{
    return data->server;
}

/*!
    Sets the application description of the server to \a server.
*/
void QOpcUaEndpointDescription::setServer(const QOpcUaApplicationDescription &server)
{
    data->server = server;
}

/*!
    Returns the URL for the endpoint.
*/
QString QOpcUaEndpointDescription::endpointUrl() const
{
    return data->endpointUrl;
}

/*!
    Sets the URL for the endpoint to \a endpointUrl.
*/
void QOpcUaEndpointDescription::setEndpointUrl(const QString &endpointUrl)
{
    data->endpointUrl = endpointUrl;
}

QT_END_NAMESPACE

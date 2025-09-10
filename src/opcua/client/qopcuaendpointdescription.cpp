// Copyright (C) 2019 The Qt Company Ltd.
// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuaendpointdescription.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaEndpointDescription
    \inmodule QtOpcUa
    \brief The OPC UA EndpointDescription.

    An endpoint description contains information about an endpoint and how to connect to it.
*/

/*!
    \qmltype EndpointDescription
    \inqmlmodule QtOpcUa
    \brief The OPC UA EndpointDescription.
    \since QtOpcUa 5.13

    An endpoint description contains information about an endpoint and how to connect to it.
*/

/*!
    \enum QOpcUaEndpointDescription::MessageSecurityMode

    This enum type holds the security mode supported by the endpoint.

    \value Invalid The default value, will be rejected by the server.
    \value None No security.
    \value Sign Messages are signed but not encrypted.
    \value SignAndEncrypt Messages are signed and encrypted.
*/

/*!
    \qmlproperty enumeration EndpointDescription::MessageSecurityMode

    The security mode supported by the endpoint.

    \value Invalid The default value, will be rejected by the server.
    \value None No security.
    \value Sign Messages are signed but not encrypted.
    \value SignAndEncrypt Messages are signed and encrypted.
*/


/*!
    \property QOpcUaEndpointDescription::endpointUrl

    The URL for the endpoint.
 */

/*!
    \qmlproperty string EndpointDescription::endpointUrl

    The URL for the endpoint.
 */

/*!
    \property QOpcUaEndpointDescription::securityMode

    Security mode supported by this endpoint.
 */

/*!
    \qmlproperty MessageSecurityMode EndpointDescription::securityMode

    Security mode supported by this endpoint.
 */

/*!
    \property QOpcUaEndpointDescription::securityPolicy

    The URI of the security policy.
 */

/*!
    \qmlproperty string EndpointDescription::securityPolicy

    The URI of the security policy.
 */


/*!
    \property QOpcUaEndpointDescription::server

    The application description of the server.
 */

/*!
    \qmlproperty ApplicationDescription EndpointDescription::server

    The application description of the server.
 */

/*!
    \property QOpcUaEndpointDescription::userIdentityTokens

    List of user identity tokens the endpoint will accept.
 */

/*!
    \qmlproperty list<UserTokenPolicy> EndpointDescription::userIdentityTokens

    List of user identity tokens the endpoint will accept.
 */

class QOpcUaEndpointDescriptionData : public QSharedData
{
public:
    QString endpointUrl;
    QOpcUaApplicationDescription server;
    QByteArray serverCertificate;
    QOpcUaEndpointDescription::MessageSecurityMode securityMode{QOpcUaEndpointDescription::MessageSecurityMode::None};
    QString securityPolicy;
    QList<QOpcUaUserTokenPolicy> userIdentityTokens;
    QString transportProfileUri;
    quint8 securityLevel{0};
};

/*!
    Default constructs an endpoint description with no parameters set.
*/
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
            rhs.securityPolicy() == securityPolicy() &&
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
QList<QOpcUaUserTokenPolicy> QOpcUaEndpointDescription::userIdentityTokens() const
{
    return data->userIdentityTokens;
}

/*!
    Returns a reference to a list of user identity tokens the endpoint will accept.
*/
QList<QOpcUaUserTokenPolicy> &QOpcUaEndpointDescription::userIdentityTokensRef()
{
    return data->userIdentityTokens;
}

/*!
    Sets the user identity tokens to \a userIdentityTokens.
*/
void QOpcUaEndpointDescription::setUserIdentityTokens(const QList<QOpcUaUserTokenPolicy> &userIdentityTokens)
{
    data->userIdentityTokens = userIdentityTokens;
}

/*!
    Returns the URI of the security policy.
*/
QString QOpcUaEndpointDescription::securityPolicy() const
{
    return data->securityPolicy;
}

/*!
    Sets the URI of the security policy to \a securityPolicy.
*/
void QOpcUaEndpointDescription::setSecurityPolicy(const QString &securityPolicy)
{
    data->securityPolicy = securityPolicy;
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

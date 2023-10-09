// Copyright (C) 2019 The Qt Company Ltd.
// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuaapplicationdescription.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaApplicationDescription
    \inmodule QtOpcUa
    \brief The OPC UA ApplicationDescription.

    The application description contains information about an OPC UA application.
*/

/*!
    \qmltype ApplicationDescription
    \inqmlmodule QtOpcUa
    \brief The OPC UA ApplicationDescription.
    \since QtOpcUa 5.13

    The application description contains information about an OPC UA application.
*/

/*!
    \enum QOpcUaApplicationDescription::ApplicationType

    This enum type holds the application type.

    \value Server This application is a server.
    \value Client This application is a client.
    \value ClientAndServer This application is a client and a server.
    \value DiscoveryServer This application is a discovery server.
*/

/*!
    \qmlproperty enumeration ApplicationDescription::ApplicationType

    The application type.

    \value Server This application is a server.
    \value Client This application is a client.
    \value ClientAndServer This application is a client and a server.
    \value DiscoveryServer This application is a discovery server.
*/

/*!
    \property QOpcUaApplicationDescription::applicationName

    Name describing the application.
 */

/*!
    \qmlproperty LocalizedText ApplicationDescription::applicationName

    Name describing the application.
 */

/*!
    \property QOpcUaApplicationDescription::applicationType

    The application's type: server, client, both, or discovery server.
 */

/*!
    \qmlproperty ApplicationType ApplicationDescription::applicationType

    The application's type: server, client, both, or discovery server.
 */

/*!
    \property QOpcUaApplicationDescription::applicationUri

    The globally unique identifier for this application instance.
 */

/*!
    \qmlproperty string ApplicationDescription::applicationUri

    The globally unique identifier for this application instance.
 */

/*!
    \property QOpcUaApplicationDescription::discoveryProfileUri

    The URI of the supported discovery profile.
 */

/*!
    \qmlproperty string ApplicationDescription::discoveryProfileUri

    The URI of the supported discovery profile.
 */

/*!
    \property QOpcUaApplicationDescription::discoveryUrls

    A list of URLs of discovery endpoints.
 */

/*!
    \qmlproperty list ApplicationDescription::discoveryUrls

    A list of URLs of discovery endpoints.
 */

/*!
    \property QOpcUaApplicationDescription::gatewayServerUri

    The URI of the gateway server.
 */

/*!
    \qmlproperty string ApplicationDescription::gatewayServerUri

    The URI of the gateway server.
 */

/*!
    \property QOpcUaApplicationDescription::productUri

    The globally unique identifier for this product.
 */

/*!
    \qmlproperty string ApplicationDescription::productUri

    The globally unique identifier for this product.
 */

class QOpcUaApplicationDescriptionData : public QSharedData
{
public:
    QString applicationUri;
    QString productUri;
    QOpcUaLocalizedText applicationName;
    QOpcUaApplicationDescription::ApplicationType applicationType{QOpcUaApplicationDescription::ApplicationType::Server};
    QString gatewayServerUri;
    QString discoveryProfileUri;
    QList<QString> discoveryUrls;
};

/*!
    Default constructs an application description with no parameters set.
*/
QOpcUaApplicationDescription::QOpcUaApplicationDescription()
    : data(new QOpcUaApplicationDescriptionData)
{
}

/*!
    Constructs an application description from \a other.
*/
QOpcUaApplicationDescription::QOpcUaApplicationDescription(const QOpcUaApplicationDescription &other)
    : data(other.data)
{
}

/*!
    Sets the values from \a other in this application description.
*/
QOpcUaApplicationDescription &QOpcUaApplicationDescription::operator=(const QOpcUaApplicationDescription &other)
{
    this->data = other.data;
    return *this;
}

/*!
    Returns \c true if this application description has the same value as \a rhs.
*/
bool QOpcUaApplicationDescription::operator==(const QOpcUaApplicationDescription &rhs) const
{
    return rhs.productUri() == productUri() &&
            rhs.discoveryUrls() == discoveryUrls() &&
            rhs.applicationUri() == applicationUri() &&
            rhs.applicationName() == applicationName() &&
            rhs.applicationType() == applicationType() &&
            rhs.gatewayServerUri() == gatewayServerUri() &&
            rhs.discoveryProfileUri() == rhs.discoveryProfileUri();
}

QOpcUaApplicationDescription::~QOpcUaApplicationDescription()
{
}

/*!
    Returns a list of URLs of discovery endpoints.
*/
QList<QString> QOpcUaApplicationDescription::discoveryUrls() const
{
    return data->discoveryUrls;
}

/*!
    Returns a reference to a list of URLs of discovery endpoints.
*/
QList<QString> &QOpcUaApplicationDescription::discoveryUrlsRef()
{
    return data->discoveryUrls;
}

/*!
    Sets the discovery URLs to \a discoveryUrls.
*/
void QOpcUaApplicationDescription::setDiscoveryUrls(const QList<QString> &discoveryUrls)
{
    data->discoveryUrls = discoveryUrls;
}

/*!
    Returns the URI of the supported discovery profile.
*/
QString QOpcUaApplicationDescription::discoveryProfileUri() const
{
    return data->discoveryProfileUri;
}

/*!
    Sets the discovery profile URI to \a discoveryProfileUri.
*/
void QOpcUaApplicationDescription::setDiscoveryProfileUri(const QString &discoveryProfileUri)
{
    data->discoveryProfileUri = discoveryProfileUri;
}

/*!
    Returns the URI of the gateway server.
*/
QString QOpcUaApplicationDescription::gatewayServerUri() const
{
    return data->gatewayServerUri;
}

/*!
    Sets the URI of the gateway server to \a gatewayServerUri.
*/
void QOpcUaApplicationDescription::setGatewayServerUri(const QString &gatewayServerUri)
{
    data->gatewayServerUri = gatewayServerUri;
}

/*!
    Returns the application's type (server, client, both, discovery server).
*/
QOpcUaApplicationDescription::ApplicationType QOpcUaApplicationDescription::applicationType() const
{
    return data->applicationType;
}

/*!
    Sets the application type to \a applicationType.
*/
void QOpcUaApplicationDescription::setApplicationType(ApplicationType applicationType)
{
    data->applicationType = applicationType;
}

/*!
    Returns a name describing the application.
*/
QOpcUaLocalizedText QOpcUaApplicationDescription::applicationName() const
{
    return data->applicationName;
}

/*!
    Sets the application name to \a applicationName.
*/
void QOpcUaApplicationDescription::setApplicationName(const QOpcUaLocalizedText &applicationName)
{
    data->applicationName = applicationName;
}

/*!
    Returns the globally unique identifier for this product.
*/
QString QOpcUaApplicationDescription::productUri() const
{
    return data->productUri;
}

/*!
    Sets the globally unique identifier for this product to \a productUri.
*/
void QOpcUaApplicationDescription::setProductUri(const QString &productUri)
{
    data->productUri = productUri;
}

/*!
    Returns the globally unique identifier for this application instance.
*/
QString QOpcUaApplicationDescription::applicationUri() const
{
    return data->applicationUri;
}

/*!
    Sets the globally unique identifier for this application instance to \a applicationUri.
*/
void QOpcUaApplicationDescription::setApplicationUri(const QString &applicationUri)
{
    data->applicationUri = applicationUri;
}

QT_END_NAMESPACE

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

#include "qopcuaapplicationdescription.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaApplicationDescription
    \inmodule QtOpcUa
    \brief The OPC UA ApplicationDescription.

    The application description contains information about an OPC UA application.
*/

/*!
    \enum QOpcUaApplicationDescription::ApplicationType

    \value Server This application is a server.
    \value Client This application is a client.
    \value ClientAndServer This application is a client and a server.
    \value DiscoveryServer This application is a discovery server.
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
    QVector<QString> discoveryUrls;
};

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
QVector<QString> QOpcUaApplicationDescription::discoveryUrls() const
{
    return data->discoveryUrls;
}

/*!
    Returns a reference to a list of URLs of discovery endpoints.
*/
QVector<QString> &QOpcUaApplicationDescription::discoveryUrlsRef()
{
    return data->discoveryUrls;
}

/*!
    Sets the discovery URLs to \a discoveryUrls.
*/
void QOpcUaApplicationDescription::setDiscoveryUrls(const QVector<QString> &discoveryUrls)
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

/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#include "qopcuaapplicationrecorddatatype.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaApplicationRecordDataType
    \inmodule QtOpcUa
    \brief The OPC UA ApplicationRecordDataType.
    \since 5.14

    This class is currently available as a Technology Preview, and therefore the API
    and functionality provided by the class may be subject to change at any time without
    prior notice.

    This is the Qt OPC UA representation for the OPC UA ApplicationRecordDataType type defined in OPC-UA version 1.04 part 12, 6.3.5.
    It is used to represent a record in the GDS.
*/

class QOpcUaApplicationRecordDataTypeData : public QSharedData
{
public:
    QString applicationId;
    QString applicationUri;
    QOpcUaApplicationDescription::ApplicationType applicationType;
    QString productUri;
    QVector<QOpcUaLocalizedText> applicationNames;
    QVector<QString> discoveryUrls;
    QVector<QString> serverCapabilityIdentifiers;
};

/*!
    Constructs a default ApplicationRecordDataType.
*/
QOpcUaApplicationRecordDataType::QOpcUaApplicationRecordDataType()
    : data(new QOpcUaApplicationRecordDataTypeData)
{
    data->applicationId = QLatin1String("ns=0;i=0"); // empty node id
}

/*!
    Constructs an ApplicationRecordDataType from \a rhs.
*/
QOpcUaApplicationRecordDataType::QOpcUaApplicationRecordDataType(const QOpcUaApplicationRecordDataType &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this ApplicationRecordDataType.
*/
QOpcUaApplicationRecordDataType &QOpcUaApplicationRecordDataType::operator=(const QOpcUaApplicationRecordDataType &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this ApplicationRecordDataType has the same values as \a rhs.
*/
bool QOpcUaApplicationRecordDataType::operator==(const QOpcUaApplicationRecordDataType &rhs) const
{
    return data->applicationId == rhs.applicationId()
           && data->applicationUri == rhs.applicationUri()
           && data->applicationType == rhs.applicationType()
           && data->productUri == rhs.productUri()
           && data->applicationNames == rhs.applicationNames()
           && data->discoveryUrls == rhs.discoveryUrls()
           && data->serverCapabilityIdentifiers == rhs.serverCapabilityIdentifiers();
}

/*!
    Converts this ApplicationRecordDataType to \l QVariant.
*/
QOpcUaApplicationRecordDataType::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
    Destructs an ApplicationRecordDataType.
*/
QOpcUaApplicationRecordDataType::~QOpcUaApplicationRecordDataType()
{
}

/*!
    Sets the application id to \a applicationId.
*/
void QOpcUaApplicationRecordDataType::setApplicationId(const QString &applicationId)
{
    data->applicationId = applicationId;
}

/*!
    Returns the application id.
*/
const QString &QOpcUaApplicationRecordDataType::applicationId() const
{
    return data->applicationId;
}

/*!
    Sets the application URI to \a applicationUri.
*/
void QOpcUaApplicationRecordDataType::setApplicationUri(const QString &applicationUri)
{
    data->applicationUri = applicationUri;
}

/*!
    Returns the application URI.
*/
const QString &QOpcUaApplicationRecordDataType::applicationUri() const
{
    return data->applicationUri;
}

/*!
    Sets the application type to \a applicationType.
*/
void QOpcUaApplicationRecordDataType::setApplicationType(QOpcUaApplicationDescription::ApplicationType applicationType)
{
    data->applicationType = applicationType;
}

/*!
    Returns the application type.
*/
QOpcUaApplicationDescription::ApplicationType QOpcUaApplicationRecordDataType::applicationType() const
{
    return data->applicationType;
}

/*!
    Sets the localized application names to \a applicationNames.
*/
void QOpcUaApplicationRecordDataType::setApplicationNames(const QVector<QOpcUaLocalizedText> &applicationNames)
{
    data->applicationNames = applicationNames;
}

/*!
    Returns the localized application names.
*/
const QVector<QOpcUaLocalizedText> &QOpcUaApplicationRecordDataType::applicationNames() const
{
    return data->applicationNames;
}

/*!
    Sets the product URI to \a productUri.
*/
void QOpcUaApplicationRecordDataType::setProductUri(const QString &productUri)
{
    data->productUri = productUri;
}

/*!
    Returns the product URI.
*/
const QString &QOpcUaApplicationRecordDataType::productUri() const
{
    return data->productUri;
}

/*!
    Sets the discovery URLs to \a discoverUrls.
*/
void QOpcUaApplicationRecordDataType::setDiscoveryUrls(const QVector<QString> &discoveryUrls)
{
    data->discoveryUrls = discoveryUrls;
}

/*!
    Returns the discovery URLs.
*/
const QVector<QString> &QOpcUaApplicationRecordDataType::discoveryUrls() const
{
    return data->discoveryUrls;
}

/*!
    Sets the server capability identifiers to \a serverCapabilityIdentifiers.
*/
void QOpcUaApplicationRecordDataType::setServerCapabilityIdentifiers(const QVector<QString> &serverCapabilityIdentifiers)
{
    data->serverCapabilityIdentifiers = serverCapabilityIdentifiers;
}

/*!
    Returns the server capability identifiers.
*/
const QVector<QString> &QOpcUaApplicationRecordDataType::serverCapabilityIdentifiers() const
{
    return data->serverCapabilityIdentifiers;
}

QT_END_NAMESPACE

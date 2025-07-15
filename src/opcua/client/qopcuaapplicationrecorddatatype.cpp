// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuaapplicationrecorddatatype.h"

QT_BEGIN_NAMESPACE

using namespace Qt::Literals::StringLiterals;

/*!
    \class QOpcUaApplicationRecordDataType
    \inmodule QtOpcUa
    \brief The OPC UA ApplicationRecordDataType.
    \since 5.14

    This class is currently available as a Technology Preview, and therefore the API
    and functionality provided by the class may be subject to change at any time without
    prior notice.

    This is the Qt OPC UA representation for the OPC UA ApplicationRecordDataType type defined in OPC UA version 1.05 part 12, 6.6.5.
    It is used to represent a record in the GDS.
*/

class QOpcUaApplicationRecordDataTypeData : public QSharedData
{
public:
    QString applicationId;
    QString applicationUri;
    QOpcUaApplicationDescription::ApplicationType applicationType;
    QString productUri;
    QList<QOpcUaLocalizedText> applicationNames;
    QList<QString> discoveryUrls;
    QList<QString> serverCapabilityIdentifiers;
};

/*!
    Constructs a default ApplicationRecordDataType.
*/
QOpcUaApplicationRecordDataType::QOpcUaApplicationRecordDataType()
    : data(new QOpcUaApplicationRecordDataTypeData)
{
    data->applicationId = u"ns=0;i=0"_s; // empty node id
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
void QOpcUaApplicationRecordDataType::setApplicationNames(const QList<QOpcUaLocalizedText> &applicationNames)
{
    data->applicationNames = applicationNames;
}

/*!
    Returns the localized application names.
*/
const QList<QOpcUaLocalizedText> &QOpcUaApplicationRecordDataType::applicationNames() const
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
    Sets the discovery URLs to \a discoveryUrls.
*/
void QOpcUaApplicationRecordDataType::setDiscoveryUrls(const QList<QString> &discoveryUrls)
{
    data->discoveryUrls = discoveryUrls;
}

/*!
    Returns the discovery URLs.
*/
const QList<QString> &QOpcUaApplicationRecordDataType::discoveryUrls() const
{
    return data->discoveryUrls;
}

/*!
    Sets the server capability identifiers to \a serverCapabilityIdentifiers.
*/
void QOpcUaApplicationRecordDataType::setServerCapabilityIdentifiers(const QList<QString> &serverCapabilityIdentifiers)
{
    data->serverCapabilityIdentifiers = serverCapabilityIdentifiers;
}

/*!
    Returns the server capability identifiers.
*/
const QList<QString> &QOpcUaApplicationRecordDataType::serverCapabilityIdentifiers() const
{
    return data->serverCapabilityIdentifiers;
}

QT_END_NAMESPACE

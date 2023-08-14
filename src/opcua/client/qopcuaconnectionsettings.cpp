// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuaconnectionsettings.h"

#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaConnectionSettings
    \inmodule QtOpcUa
    \brief The common parameters for a connection to a server.

    This class stores common connection parameters like the session timeout, the
    secure channel lifetime and the localeIds.

    The information from this class is currently only used by the open62541 backend.
*/

class QOpcUaConnectionSettingsData : public QSharedData
{
public:
    QStringList sessionLocaleIds;

    quint32 secureChannelLifeTimeMs = 10 * 60 * 1000;
    quint32 sessionTimeoutMs = 20 * 60 * 1000;
    quint32 requestTimeoutMs = 5 * 1000;
    quint32 connectTimeoutMs = 5 * 1000;
};

/*!
    Constructs a connection settings object.
*/
QOpcUaConnectionSettings::QOpcUaConnectionSettings()
    : data(new QOpcUaConnectionSettingsData)
{
}

/*!
    Constructs a connection settings object with the values of \a other.
*/
QOpcUaConnectionSettings::QOpcUaConnectionSettings(const QOpcUaConnectionSettings &other)
    : data(other.data)
{
}

/*!
    Sets the values from \a rhs in this connection settings object.
*/
QOpcUaConnectionSettings &QOpcUaConnectionSettings::operator=(const QOpcUaConnectionSettings &rhs)
{
    if (this != &rhs)
        data = rhs.data;

    return *this;
}

/*!
    Destroys this connection settings object.
*/
QOpcUaConnectionSettings::~QOpcUaConnectionSettings()
{
}

/*!
    Returns \c true if this connection settings object has the same value as \a rhs.
*/
bool QOpcUaConnectionSettings::operator==(const QOpcUaConnectionSettings &rhs) const
{
    return data->sessionLocaleIds == rhs.data->sessionLocaleIds &&
            data->secureChannelLifeTimeMs == rhs.data->secureChannelLifeTimeMs &&
            data->sessionTimeoutMs == rhs.data->sessionTimeoutMs &&
            data->requestTimeoutMs == rhs.data->requestTimeoutMs;
}

/*!
    Returns the session locale ids.
*/
QStringList QOpcUaConnectionSettings::sessionLocaleIds() const
{
    return data->sessionLocaleIds;
}

/*!
    Sets \a localeIds as the new list of locale IDs.
    This setting is currently not supported by the open62541 backend.

    For details, see \l{https://reference.opcfoundation.org/v104/Core/docs/Part4/5.6.3/#Table17}
*/
void QOpcUaConnectionSettings::setSessionLocaleIds(const QStringList &localeIds)
{
    data->sessionLocaleIds = localeIds;
}

/*!
    Returns the secure channel lifetime.
*/
quint32 QOpcUaConnectionSettings::secureChannelLifeTimeMs() const
{
    return data->secureChannelLifeTimeMs;
}

/*!
    Sets \a lifeTimeMs as new the new secure channel lifetime.

    For details, see \l{https://reference.opcfoundation.org/v104/Core/docs/Part4/5.5.2/#Table11}
*/
void QOpcUaConnectionSettings::setSecureChannelLifeTimeMs(quint32 lifeTimeMs)
{
    data->secureChannelLifeTimeMs = lifeTimeMs;
}

/*!
    Returns the requested session timeout.
*/
quint32 QOpcUaConnectionSettings::sessionTimeoutMs() const
{
    return data->sessionTimeoutMs;
}

/*!
    Sets \a timeoutMs as the new requested session timeout.

    For details, see \l{https://reference.opcfoundation.org/v104/Core/docs/Part4/5.6.2/#Table15}
*/
void QOpcUaConnectionSettings::setSessionTimeoutMs(quint32 timeoutMs)
{
    data->sessionTimeoutMs = timeoutMs;
}

/*!
    Returns the request timeout.

    This value determines how long a synchronous service call will wait for a reply.
*/
quint32 QOpcUaConnectionSettings::requestTimeoutMs() const
{
    return data->requestTimeoutMs;
}

/*!
    Sets \a timeoutMs as new request timeout.
*/
void QOpcUaConnectionSettings::setRequestTimeoutMs(quint32 timeoutMs)
{
    data->requestTimeoutMs = timeoutMs;
}

/*!
    Returns the connect timeout.

    This value determines how long the connect will wait for a reply.
*/
quint32 QOpcUaConnectionSettings::connectTimeoutMs() const
{
    return data->connectTimeoutMs;
}

/*!
    Sets \a timeoutMs as new connect timeout.
*/
void QOpcUaConnectionSettings::setConnectTimeoutMs(quint32 timeoutMs)
{
    data->connectTimeoutMs = timeoutMs;
}

QT_END_NAMESPACE

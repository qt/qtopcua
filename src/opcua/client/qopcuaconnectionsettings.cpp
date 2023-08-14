// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuaconnectionsettings.h"

#include <QtCore/qstringlist.h>

using namespace std::chrono_literals;

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

    std::chrono::milliseconds secureChannelLifeTime = 10min;
    std::chrono::milliseconds sessionTimeout = 20min;
    std::chrono::milliseconds requestTimeout = 5s;
    std::chrono::milliseconds connectTimeout = 5s;
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
            data->secureChannelLifeTime == rhs.data->secureChannelLifeTime &&
            data->sessionTimeout == rhs.data->sessionTimeout &&
            data->requestTimeout == rhs.data->requestTimeout;
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
std::chrono::milliseconds QOpcUaConnectionSettings::secureChannelLifeTime() const
{
    return data->secureChannelLifeTime;
}

/*!
    Sets \a lifeTime as the new secure channel lifetime.

    For details, see \l{https://reference.opcfoundation.org/v104/Core/docs/Part4/5.5.2/#Table11}
*/
void QOpcUaConnectionSettings::setSecureChannelLifeTime(std::chrono::milliseconds lifeTime)
{
    data->secureChannelLifeTime = lifeTime;
}

/*!
    Returns the requested session timeout.
*/
std::chrono::milliseconds QOpcUaConnectionSettings::sessionTimeout() const
{
    return data->sessionTimeout;
}

/*!
    Sets \a timeout as the new requested session timeout.

    For details, see \l{https://reference.opcfoundation.org/v104/Core/docs/Part4/5.6.2/#Table15}
*/
void QOpcUaConnectionSettings::setSessionTimeout(std::chrono::milliseconds timeout)
{
    data->sessionTimeout = timeout;
}

/*!
    Returns the request timeout.

    This value determines how long a synchronous service call will wait for a reply.
*/
std::chrono::milliseconds QOpcUaConnectionSettings::requestTimeout() const
{
    return data->requestTimeout;
}

/*!
    Sets \a timeout as the new request timeout.
*/
void QOpcUaConnectionSettings::setRequestTimeout(std::chrono::milliseconds timeout)
{
    data->requestTimeout = timeout;
}

/*!
    Returns the connect timeout.

    This value determines how long the connect will wait for a reply.
*/
std::chrono::milliseconds QOpcUaConnectionSettings::connectTimeout() const
{
    return data->connectTimeout;
}

/*!
    Sets \a timeout as the new connect timeout.
*/
void QOpcUaConnectionSettings::setConnectTimeout(std::chrono::milliseconds timeout)
{
    data->connectTimeout = timeout;
}

QT_END_NAMESPACE

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
    \since 6.6

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
QT_DEFINE_QESDP_SPECIALIZATION_DTOR(QOpcUaConnectionSettingsData)

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
    = default;

/*!
    Sets the values from \a rhs in this connection settings object.
*/
QOpcUaConnectionSettings &QOpcUaConnectionSettings::operator=(const QOpcUaConnectionSettings &rhs)
    = default;

/*!
    Destroys this connection settings object.
*/
QOpcUaConnectionSettings::~QOpcUaConnectionSettings()
    = default;

/*!
    \fn QOpcUaConnectionSettings::QOpcUaConnectionSettings(QOpcUaConnectionSettings &&other)

    Move-constructs a new connection settings object from \a other.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn QOpcUaConnectionSettings &QOpcUaConnectionSettings::operator=(QOpcUaConnectionSettings &&other)

    Move-assigns \a other to this QOpcUaConnectionSettings instance.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn void QOpcUaConnectionSettings::swap(QOpcUaConnectionSettings &other)

    Swaps connection settings object \a other with this connection settings
    object. This operation is very fast and never fails.
*/

/*!
    \fn bool QOpcUaConnectionSettings::operator==(const QOpcUaConnectionSettings &lhs,
                                                  const QOpcUaConnectionSettings &rhs)

    Returns \c true if \a lhs contains the same connection settings as \a rhs; otherwise returns \c
    false.
*/
bool operator==(const QOpcUaConnectionSettings &lhs, const QOpcUaConnectionSettings &rhs) noexcept
{
    return  lhs.data->sessionLocaleIds == rhs.data->sessionLocaleIds &&
            lhs.data->connectTimeout == rhs.data->connectTimeout &&
            lhs.data->secureChannelLifeTime == rhs.data->secureChannelLifeTime &&
            lhs.data->sessionTimeout == rhs.data->sessionTimeout &&
            lhs.data->requestTimeout == rhs.data->requestTimeout;
}

/*!
    \fn bool QOpcUaConnectionSettings::operator!=(const QOpcUaConnectionSettings &lhs,
                                                  const QOpcUaConnectionSettings &rhs)

    Returns \c true if \a lhs does not contain the same connection settings as \a rhs; otherwise
    returns \c false.
*/

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

    For details, see \l{https://reference.opcfoundation.org/v105/Core/docs/Part4/5.6.3/#Table17}
*/
void QOpcUaConnectionSettings::setSessionLocaleIds(const QStringList &localeIds)
{
    if (data->sessionLocaleIds == localeIds)
        return;
    data.detach();
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

    For details, see \l{https://reference.opcfoundation.org/v105/Core/docs/Part4/5.5.2/#Table11}
*/
void QOpcUaConnectionSettings::setSecureChannelLifeTime(std::chrono::milliseconds lifeTime)
{
    if (data->secureChannelLifeTime == lifeTime)
        return;
    data.detach();
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

    For details, see \l{https://reference.opcfoundation.org/v105/Core/docs/Part4/5.6.2/#Table15}
*/
void QOpcUaConnectionSettings::setSessionTimeout(std::chrono::milliseconds timeout)
{
    if (data->sessionTimeout == timeout)
        return;
    data.detach();
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
    if (data->requestTimeout == timeout)
        return;
    data.detach();
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
    if (data->connectTimeout == timeout)
        return;
    data.detach();
    data->connectTimeout = timeout;
}

QT_END_NAMESPACE

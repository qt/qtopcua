// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUACONNECTIONSETTINGS_H
#define QOPCUACONNECTIONSETTINGS_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qcontainerfwd.h>
#include <QtCore/qshareddata.h>

#include <chrono>

QT_BEGIN_NAMESPACE

class QOpcUaConnectionSettingsData;
class QOpcUaConnectionSettings
{
public:
    Q_OPCUA_EXPORT QOpcUaConnectionSettings();
    Q_OPCUA_EXPORT QOpcUaConnectionSettings(const QOpcUaConnectionSettings &other);
    Q_OPCUA_EXPORT QOpcUaConnectionSettings &operator=(const QOpcUaConnectionSettings &rhs);
    Q_OPCUA_EXPORT ~QOpcUaConnectionSettings();

    Q_OPCUA_EXPORT QStringList sessionLocaleIds() const;
    Q_OPCUA_EXPORT void setSessionLocaleIds(const QStringList &localeIds);

    Q_OPCUA_EXPORT std::chrono::milliseconds secureChannelLifeTime() const;
    Q_OPCUA_EXPORT void setSecureChannelLifeTime(std::chrono::milliseconds lifeTime);

    Q_OPCUA_EXPORT std::chrono::milliseconds sessionTimeout() const;
    Q_OPCUA_EXPORT void setSessionTimeout(std::chrono::milliseconds timeout);

    Q_OPCUA_EXPORT std::chrono::milliseconds requestTimeout() const;
    Q_OPCUA_EXPORT void setRequestTimeout(std::chrono::milliseconds timeout);

    Q_OPCUA_EXPORT std::chrono::milliseconds connectTimeout() const;
    Q_OPCUA_EXPORT void setConnectTimeout(std::chrono::milliseconds timeout);

private:
    friend Q_OPCUA_EXPORT bool operator==(const QOpcUaConnectionSettings &lhs,
                                          const QOpcUaConnectionSettings &rhs) noexcept;
    friend bool operator!=(const QOpcUaConnectionSettings &lhs,
                           const QOpcUaConnectionSettings &rhs) noexcept
    { return !(lhs == rhs); }

private:
    QSharedDataPointer<QOpcUaConnectionSettingsData> data;
};

QT_END_NAMESPACE

#endif // QOPCUACONNECTIONSETTINGS_H

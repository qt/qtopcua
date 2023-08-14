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
class Q_OPCUA_EXPORT QOpcUaConnectionSettings
{
public:
    QOpcUaConnectionSettings();
    QOpcUaConnectionSettings(const QOpcUaConnectionSettings &other);
    QOpcUaConnectionSettings &operator=(const QOpcUaConnectionSettings &rhs);
    ~QOpcUaConnectionSettings();

    QStringList sessionLocaleIds() const;
    void setSessionLocaleIds(const QStringList &localeIds);

    std::chrono::milliseconds secureChannelLifeTime() const;
    void setSecureChannelLifeTime(std::chrono::milliseconds lifeTime);

    std::chrono::milliseconds sessionTimeout() const;
    void setSessionTimeout(std::chrono::milliseconds timeout);

    std::chrono::milliseconds requestTimeout() const;
    void setRequestTimeout(std::chrono::milliseconds timeout);

    std::chrono::milliseconds connectTimeout() const;
    void setConnectTimeout(std::chrono::milliseconds timeout);

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

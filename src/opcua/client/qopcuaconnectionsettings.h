// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUACONNECTIONSETTINGS_H
#define QOPCUACONNECTIONSETTINGS_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qmetatype.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE

class QOpcUaConnectionSettingsData;
class Q_OPCUA_EXPORT QOpcUaConnectionSettings
{
public:
    QOpcUaConnectionSettings();
    QOpcUaConnectionSettings(const QOpcUaConnectionSettings &other);
    QOpcUaConnectionSettings &operator=(const QOpcUaConnectionSettings &rhs);
    ~QOpcUaConnectionSettings();

    bool operator==(const QOpcUaConnectionSettings &rhs) const;

    QStringList sessionLocaleIds() const;
    void setSessionLocaleIds(const QStringList &localeIds);

    quint32 secureChannelLifeTimeMs() const;
    void setSecureChannelLifeTimeMs(quint32 lifeTimeMs);

    quint32 sessionTimeoutMs() const;
    void setSessionTimeoutMs(quint32 timeoutMs);

    quint32 requestTimeoutMs() const;
    void setRequestTimeoutMs(quint32 timeoutMs);

    quint32 connectTimeoutMs() const;
    void setConnectTimeoutMs(quint32 timeoutMs);

private:
    QSharedDataPointer<QOpcUaConnectionSettingsData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaConnectionSettings)

#endif // QOPCUACONNECTIONSETTINGS_H

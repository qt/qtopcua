// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAAUTHENTICATIONINFORMATION_H
#define QOPCUAAUTHENTICATIONINFORMATION_H

#include <QtOpcUa/qopcuaglobal.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

#include <QtOpcUa/QOpcUaUserTokenPolicy>

QT_BEGIN_NAMESPACE

class QOpcUaAuthenticationInformationData;
class Q_OPCUA_EXPORT QOpcUaAuthenticationInformation
{
    Q_GADGET

public:
    QOpcUaAuthenticationInformation();
    QOpcUaAuthenticationInformation(const QOpcUaAuthenticationInformation &);
    QOpcUaAuthenticationInformation &operator=(const QOpcUaAuthenticationInformation &);
    bool operator==(const QOpcUaAuthenticationInformation &rhs) const;
    ~QOpcUaAuthenticationInformation();

    Q_INVOKABLE void setAnonymousAuthentication();
    Q_INVOKABLE void setUsernameAuthentication(const QString &username, const QString &password);
    Q_INVOKABLE void setCertificateAuthentication();

    const QVariant &authenticationData() const;
    Q_INVOKABLE QOpcUaUserTokenPolicy::TokenType authenticationType() const;

private:
    QSharedDataPointer<QOpcUaAuthenticationInformationData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaAuthenticationInformation)

#endif // QOPCUAAUTHETICATIONINFORMATION_H

// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAENDPOINTDESCRIPTION_H
#define QOPCUAENDPOINTDESCRIPTION_H

#include <QtOpcUa/qopcuaglobal.h>
#include <QtOpcUa/qopcuaapplicationdescription.h>
#include <QtOpcUa/qopcuausertokenpolicy.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaEndpointDescriptionData;
class Q_OPCUA_EXPORT QOpcUaEndpointDescription
{
    Q_GADGET
    Q_PROPERTY(QString endpointUrl READ endpointUrl)
    Q_PROPERTY(QOpcUaApplicationDescription server READ server)
    Q_PROPERTY(QOpcUaEndpointDescription::MessageSecurityMode securityMode READ securityMode)
    Q_PROPERTY(QString securityPolicy READ securityPolicy)
    Q_PROPERTY(QList<QOpcUaUserTokenPolicy> userIdentityTokens READ userIdentityTokens)

public:
    QOpcUaEndpointDescription();
    QOpcUaEndpointDescription(const QOpcUaEndpointDescription &);
    QOpcUaEndpointDescription &operator=(const QOpcUaEndpointDescription &);
    bool operator==(const QOpcUaEndpointDescription &) const;
    ~QOpcUaEndpointDescription();

    enum MessageSecurityMode {
        Invalid = 0,
        None = 1,
        Sign = 2,
        SignAndEncrypt = 3,
    };
    Q_ENUM(MessageSecurityMode)

    QString endpointUrl() const;
    void setEndpointUrl(const QString &endpointUrl);

    QOpcUaApplicationDescription server() const;
    QOpcUaApplicationDescription &serverRef();
    void setServer(const QOpcUaApplicationDescription &server);

    QByteArray serverCertificate() const;
    void setServerCertificate(const QByteArray &serverCertificate);

    QOpcUaEndpointDescription::MessageSecurityMode securityMode() const;
    void setSecurityMode(QOpcUaEndpointDescription::MessageSecurityMode securityMode);

    QString securityPolicy() const;
    void setSecurityPolicy(const QString &securityPolicy);

    QList<QOpcUaUserTokenPolicy> userIdentityTokens() const;
    QList<QOpcUaUserTokenPolicy> &userIdentityTokensRef();
    void setUserIdentityTokens(const QList<QOpcUaUserTokenPolicy> &userIdentityTokens);

    QString transportProfileUri() const;
    void setTransportProfileUri(const QString &transportProfileUri);

    quint8 securityLevel() const;
    void setSecurityLevel(quint8 securityLevel);

private:
    QSharedDataPointer<QOpcUaEndpointDescriptionData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaEndpointDescription)

#endif // QOPCUAENDPOINTDESCRIPTION_H

// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAAPPLICATIONDESCRIPTION_H
#define QOPCUAAPPLICATIONDESCRIPTION_H

#include <QtOpcUa/qopcualocalizedtext.h>

#include <QtOpcUa/qopcuaglobal.h>
#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaApplicationDescriptionData;
class Q_OPCUA_EXPORT QOpcUaApplicationDescription
{
    Q_GADGET
    Q_PROPERTY(QString applicationUri READ applicationUri)
    Q_PROPERTY(QString productUri READ productUri)
    Q_PROPERTY(QOpcUaLocalizedText applicationName READ applicationName)
    Q_PROPERTY(QOpcUaApplicationDescription::ApplicationType applicationType READ applicationType)
    Q_PROPERTY(QString gatewayServerUri READ gatewayServerUri)
    Q_PROPERTY(QString discoveryProfileUri READ discoveryProfileUri)
    Q_PROPERTY(QList<QString> discoveryUrls READ discoveryUrls)

public:
    QOpcUaApplicationDescription();
    QOpcUaApplicationDescription(const QOpcUaApplicationDescription &);
    QOpcUaApplicationDescription &operator=(const QOpcUaApplicationDescription &);
    bool operator==(const QOpcUaApplicationDescription &) const;
    ~QOpcUaApplicationDescription();

    enum ApplicationType {
        Server = 0,
        Client = 1,
        ClientAndServer = 2,
        DiscoveryServer = 3
    };
    Q_ENUM(ApplicationType)

    QString applicationUri() const;
    void setApplicationUri(const QString &applicationUri);

    QString productUri() const;
    void setProductUri(const QString &productUri);

    QOpcUaLocalizedText applicationName() const;
    void setApplicationName(const QOpcUaLocalizedText &applicationName);

    QOpcUaApplicationDescription::ApplicationType applicationType() const;
    void setApplicationType(QOpcUaApplicationDescription::ApplicationType applicationType);

    QString gatewayServerUri() const;
    void setGatewayServerUri(const QString &gatewayServerUri);

    QString discoveryProfileUri() const;
    void setDiscoveryProfileUri(const QString &discoveryProfileUri);

    QList<QString> discoveryUrls() const;
    QList<QString> &discoveryUrlsRef();
    void setDiscoveryUrls(const QList<QString> &discoveryUrls);

private:
    QSharedDataPointer<QOpcUaApplicationDescriptionData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaApplicationDescription)

#endif // QOPCUAAPPLICATIONDESCRIPTION_H

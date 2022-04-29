/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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

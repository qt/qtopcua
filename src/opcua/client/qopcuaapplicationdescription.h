/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
    Q_PROPERTY(QVector<QString> discoveryUrls READ discoveryUrls)

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

    QVector<QString> discoveryUrls() const;
    QVector<QString> &discoveryUrlsRef();
    void setDiscoveryUrls(const QVector<QString> &discoveryUrls);

private:
    QSharedDataPointer<QOpcUaApplicationDescriptionData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaApplicationDescription)

#endif // QOPCUAAPPLICATIONDESCRIPTION_H

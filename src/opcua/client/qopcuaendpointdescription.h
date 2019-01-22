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
    Q_PROPERTY(QString securityPolicyUri READ securityPolicyUri)
    Q_PROPERTY(QVector<QOpcUaUserTokenPolicy> userIdentityTokens READ userIdentityTokens)

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
        SignAndEncrypt = 3
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

    QString securityPolicyUri() const;
    void setSecurityPolicyUri(const QString &securityPolicyUri);

    QVector<QOpcUaUserTokenPolicy> userIdentityTokens() const;
    QVector<QOpcUaUserTokenPolicy> &userIdentityTokensRef();
    void setUserIdentityTokens(const QVector<QOpcUaUserTokenPolicy> &userIdentityTokens);

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

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

#ifndef QOPCUAUSERTOKENPOLICY_H
#define QOPCUAUSERTOKENPOLICY_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qmetatype.h>

QT_BEGIN_NAMESPACE

class QOpcUaUserTokenPolicyData;
class Q_OPCUA_EXPORT QOpcUaUserTokenPolicy
{
    Q_GADGET
public:
    QOpcUaUserTokenPolicy();
    QOpcUaUserTokenPolicy(const QOpcUaUserTokenPolicy &);
    QOpcUaUserTokenPolicy &operator=(const QOpcUaUserTokenPolicy &);
    bool operator==(const QOpcUaUserTokenPolicy &) const;
    ~QOpcUaUserTokenPolicy();

    enum TokenType {
        Anonymous = 0,
        Username = 1,
        Certificate = 2,
        IssuedToken = 3
    };
    Q_ENUMS(TokenType)

    QString policyId() const;
    void setPolicyId(const QString &policyId);

    QOpcUaUserTokenPolicy::TokenType tokenType() const;
    void setTokenType(QOpcUaUserTokenPolicy::TokenType tokenType);

    QString issuedTokenType() const;
    void setIssuedTokenType(const QString &issuedTokenType);

    QString issuerEndpointUrl() const;
    void setIssuerEndpointUrl(const QString &issuerEndpointUrl);

    QString securityPolicy() const;
    void setSecurityPolicy(const QString &securityPolicy);

private:
    QSharedDataPointer<QOpcUaUserTokenPolicyData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaUserTokenPolicy)

#endif // QOPCUAUSERTOKENPOLICY_H

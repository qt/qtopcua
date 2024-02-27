// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
        IssuedToken = 3,
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

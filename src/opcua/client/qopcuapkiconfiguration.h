// Copyright (C) 2018 Unified Automation GmbH
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAPKICONFIGURATION_H
#define QOPCUAPKICONFIGURATION_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qmetatype.h>
#include <QtCore/qshareddata.h>

#include <QtOpcUa/qopcuaapplicationidentity.h>

QT_BEGIN_NAMESPACE

class QOpcUaPkiConfigurationData;
class QOpcUaApplicationIdentity;

class Q_OPCUA_EXPORT QOpcUaPkiConfiguration
{
public:
    QOpcUaPkiConfiguration();
    ~QOpcUaPkiConfiguration();
    QOpcUaPkiConfiguration(const QOpcUaPkiConfiguration &other);
    QOpcUaPkiConfiguration &operator=(const QOpcUaPkiConfiguration &rhs);

    QString clientCertificateFile() const;
    void setClientCertificateFile(const QString &value);

    QString privateKeyFile() const;
    void setPrivateKeyFile(const QString &value);

    QString trustListDirectory() const;
    void setTrustListDirectory(const QString &value);

    QString revocationListDirectory() const;
    void setRevocationListDirectory(const QString &value);

    QString issuerListDirectory() const;
    void setIssuerListDirectory(const QString &value);

    QString issuerRevocationListDirectory() const;
    void setIssuerRevocationListDirectory(const QString &value);

    QOpcUaApplicationIdentity applicationIdentity() const;

    bool isPkiValid() const; // Bad name, open for better ideas
    bool isKeyAndCertificateFileSet() const;

private:
    QSharedDataPointer<QOpcUaPkiConfigurationData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaPkiConfiguration)

#endif // QOPCUAPKICONFIGURATION_H

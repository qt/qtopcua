// Copyright (C) 2018 Unified Automation GmbH
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAAPPLICATIONIDENTITY_H
#define QOPCUAAPPLICATIONIDENTITY_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qmetatype.h>
#include <QtOpcUa/qopcuaapplicationdescription.h>

QT_BEGIN_NAMESPACE

class QOpcUaApplicationIdentityData;

class Q_OPCUA_EXPORT QOpcUaApplicationIdentity
{
public:
    QOpcUaApplicationIdentity();
    ~QOpcUaApplicationIdentity();
    QOpcUaApplicationIdentity(const QOpcUaApplicationIdentity &other);
    QOpcUaApplicationIdentity &operator=(const QOpcUaApplicationIdentity &rhs);

    QString applicationUri() const;
    void setApplicationUri(const QString &value);

    QString applicationName() const;
    void setApplicationName(const QString &value);

    QString productUri() const;
    void setProductUri(const QString &value);

    QOpcUaApplicationDescription::ApplicationType applicationType() const;
    void setApplicationType(QOpcUaApplicationDescription::ApplicationType value);

    bool isValid() const;

private:
    QSharedDataPointer<QOpcUaApplicationIdentityData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaApplicationIdentity)

#endif // QOPCUAAPPLICATIONIDENTITY_H

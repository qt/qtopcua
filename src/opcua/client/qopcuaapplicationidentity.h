/****************************************************************************
**
** Copyright (C) 2018 Unified Automation GmbH
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

/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#ifndef QOPCUAERRORSTATE_H
#define QOPCUAERRORSTATE_H

#include <QtCore/qshareddata.h>
#include <QtOpcUa/qopcuatype.h>
#include <QtOpcUa/qopcuaglobal.h>

QT_BEGIN_NAMESPACE

class QOpcUaErrorStateData;

class Q_OPCUA_EXPORT QOpcUaErrorState
{
public:
    enum class ConnectionStep {
        Unknown = 0x00,
        CertificateValidation,
        OpenSecureChannel,
        CreateSession,
        ActivateSession
    };

    QOpcUaErrorState();
    QOpcUaErrorState(const QOpcUaErrorState &other);
    QOpcUaErrorState &operator =(const QOpcUaErrorState &rhs);
    ~QOpcUaErrorState();

    ConnectionStep connectionStep() const;
    void setConnectionStep(ConnectionStep step);

    QOpcUa::UaStatusCode errorCode() const;
    void setErrorCode(QOpcUa::UaStatusCode error);

    bool isClientSideError() const;
    void setClientSideError(bool clientSideError);

    void setIgnoreError(bool ignore = true);
    bool ignoreError() const;

private:
    QSharedDataPointer<QOpcUaErrorStateData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaErrorState)

#endif // QOPCUAERRORSTATE_H

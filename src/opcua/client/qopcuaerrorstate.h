// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

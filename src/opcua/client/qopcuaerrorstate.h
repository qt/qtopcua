/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

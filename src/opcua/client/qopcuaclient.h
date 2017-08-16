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

#ifndef QOPCUACLIENT_H
#define QOPCUACLIENT_H

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>

#include <QtOpcUa/qopcuaglobal.h>
#include <QtOpcUa/qopcuasubscription.h>
#include <QtOpcUa/qopcuanode.h>

QT_BEGIN_NAMESPACE

class QOpcUaClientPrivate;
class QOpcUaClientImpl;

class Q_OPCUA_EXPORT QOpcUaClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ClientState state READ state NOTIFY stateChanged)
    Q_PROPERTY(ClientError error READ error NOTIFY errorChanged)
    Q_DECLARE_PRIVATE(QOpcUaClient)

public:
    enum ClientState {
        Disconnected,
        Connecting,
        Connected,
        Closing
    };
    Q_ENUM(ClientState)

    enum ClientError {
        NoError,
        InvalidUrl,
        SecureConnectionError,
        UnknownError
    };
    Q_ENUM(ClientError)

    explicit QOpcUaClient(QOpcUaClientImpl *impl, QObject *parent = nullptr);
    ~QOpcUaClient();

    Q_INVOKABLE void connectToEndpoint(const QUrl &url);
    Q_INVOKABLE void secureConnectToEndpoint(const QUrl &url);
    Q_INVOKABLE void disconnectFromEndpoint();
    QOpcUaNode *node(const QString &nodeId);

    QOpcUaSubscription *createSubscription(quint32 interval);

    QUrl url() const;

    ClientState state() const;
    ClientError error() const;

    bool isSecureConnectionSupported() const;
    QString backend() const;

Q_SIGNALS:
    void connected();
    void disconnected();
    void stateChanged(ClientState state);
    void errorChanged(ClientError error);

private:
    Q_DISABLE_COPY(QOpcUaClient)
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaClient::ClientState)
Q_DECLARE_METATYPE(QOpcUaClient::ClientError)

#endif // QOPCUACLIENT_H

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

#ifndef QFREEOPCUACLIENT_P_H
#define QFREEOPCUACLIENT_P_H

#include <private/qopcuaclientimpl_p.h>

// freeopcua
#include <opc/ua/client/client.h>

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qfuturewatcher.h>
#include <QtCore/qthreadpool.h>

QT_BEGIN_NAMESPACE

class QFreeOpcUaClient : public QOpcUaClientImpl, public OpcUa::UaClient
{
public:
    explicit QFreeOpcUaClient();
    ~QFreeOpcUaClient() Q_DECL_OVERRIDE;

    void connectToEndpoint(const QUrl &url) Q_DECL_OVERRIDE;
    void secureConnectToEndpoint(const QUrl &url) Q_DECL_OVERRIDE;
    void disconnectFromEndpoint() Q_DECL_OVERRIDE;
    QOpcUaNode *node(const QString &nodeId) Q_DECL_OVERRIDE;

    bool isSecureConnectionSupported() const Q_DECL_OVERRIDE { return false; }
    QString backend() const Q_DECL_OVERRIDE { return QStringLiteral("freeopcua"); }

    QOpcUaSubscription *createSubscription(quint32 interval) Q_DECL_OVERRIDE;

private slots:
    void connectToEndpointFinished();
    void disconnectFromEndpointFinished();

private:
    bool asyncConnectToEndpoint(const QUrl &url);
    bool asyncDisconnectFromEndpoint();

    QThreadPool *m_clientThreadPool;
    QFutureWatcher<bool> m_connectWatcher;
    QFutureWatcher<bool> m_disconnectWatcher;
};

QT_END_NAMESPACE

#endif // QFREEOPCUACLIENT_P_H

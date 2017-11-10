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

#include <QtCore/qobject.h>
#include <QtCore/qthread.h>
#include <QtCore/qurl.h>

#include <opc/ua/client/client.h>

QT_BEGIN_NAMESPACE
class QFreeOpcUaWorker;
class QOpcUaSubscription;
class QOpcUaNode;
class QFreeOpcUaClientImpl : public QOpcUaClientImpl
{
public:
    explicit QFreeOpcUaClientImpl();
    ~QFreeOpcUaClientImpl() override;

    void connectToEndpoint(const QUrl &url) override;
    void secureConnectToEndpoint(const QUrl &url) override;
    void disconnectFromEndpoint() override;
    QOpcUaNode *node(const QString &nodeId) override;

    bool isSecureConnectionSupported() const override { return false; }
    QString backend() const override { return QStringLiteral("freeopcua"); }

    QOpcUaSubscription *createSubscription(quint32 interval) override;

public slots:
    void connectToEndpointFinished(bool isSuccess);
    void disconnectFromEndpointFinished(bool isSuccess);

private:
    QThread *m_thread{};
    QFreeOpcUaWorker *m_opcuaWorker{};
};

QT_END_NAMESPACE

#endif // QFREEOPCUACLIENT_P_H

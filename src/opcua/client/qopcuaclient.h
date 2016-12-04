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
    Q_DECLARE_PRIVATE(QOpcUaClient)

    // FIXME: is this property needed when we have a "proper qml client"?
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

public:
    QOpcUaClient(QOpcUaClientImpl *impl, QObject *parent = nullptr);
    ~QOpcUaClient();

    bool connectToEndpoint(const QUrl &url);
    bool secureConnectToEndpoint(const QUrl &url);
    bool disconnectFromEndpoint();
    QOpcUaNode *node(const QString &nodeId);

    QOpcUaSubscription *createSubscription(quint32 interval);

    QUrl url() const;
    bool isConnected() const;

    QString backend() const;

Q_SIGNALS:
    void connectedChanged(bool connected);  // FIXME: connectionStateChanged would be nicer
};

QT_END_NAMESPACE

#endif // QOPCUACLIENT_H

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

#include "qfreeopcuaworker.h"
#include "qfreeopcuaclient.h"

#include "qfreeopcuanode.h"
#include "qfreeopcuasubscription.h"

#include <qopcuasubscription.h>

// freeopcua
#include <opc/ua/node.h>

#include <QtCore/qdebug.h>
#include <QtCore/qloggingcategory.h>
#include <QtNetwork/qhostinfo.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_FREEOPCUA)

QFreeOpcUaWorker::QFreeOpcUaWorker(QFreeOpcUaClientImpl *client)
    : QObject()
    , m_client(client)
{}

void QFreeOpcUaWorker::asyncConnectToEndpoint(const QUrl &url)
{
    try {
        QString sNodeName = QHostInfo::localHostName();
        SetApplicationURI(QString("urn:%1:%2:%3").arg(
                              sNodeName).arg("qt-project").arg("QOpcUaClient").toStdString());
        SetProductURI("urn:qt-project:QOpcUaClient");
        SetSessionName(GetApplicationURI());

        Connect(url.toString().toStdString());

        // Check connection status by getting the root node
        GetRootNode();
    } catch (const std::exception &e) {
        try {
            Disconnect();
        } catch (const std::exception &e) {
            qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "Disconnect failed";
            qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << e.what();
        }
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "Client could not connect to endpoint" << url;
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << e.what();
        emit m_client->stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::NoError);
        return;
    }

    emit m_client->stateAndOrErrorChanged(QOpcUaClient::Connected, QOpcUaClient::NoError);
}

void QFreeOpcUaWorker::asyncDisconnectFromEndpoint()
{
    try {
        Disconnect();
        emit m_client->stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::NoError);
        return;
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "Could not disconnect from endpoint: " << ex.what();
    }

    emit m_client->stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::UnknownError);
}

QOpcUaSubscription *QFreeOpcUaWorker::createSubscription(quint32 interval)
{
    QFreeOpcUaSubscription *backendSubscription = new QFreeOpcUaSubscription(this, interval);
    QOpcUaSubscription *subscription = new QOpcUaSubscription(backendSubscription, interval);
    backendSubscription->m_qsubscription = subscription;
    return subscription;
}

QT_END_NAMESPACE

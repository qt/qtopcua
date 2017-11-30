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

#ifndef QFREEOPCUAWORKER_H
#define QFREEOPCUAWORKER_H

#include "qfreeopcuasubscription.h"
#include <QtOpcUa/qopcuanode.h>
#include <private/qopcuabackend_p.h>

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>

#include <opc/ua/client/client.h>

QT_BEGIN_NAMESPACE

class QFreeOpcUaClientImpl;

class QFreeOpcUaWorker : public QOpcUaBackend, public OpcUa::UaClient
{
    Q_OBJECT
public:
    QFreeOpcUaWorker(QFreeOpcUaClientImpl *client);
    ~QFreeOpcUaWorker();

public slots:
    void asyncConnectToEndpoint(const QUrl &url);
    void asyncDisconnectFromEndpoint();

    void readAttributes(uintptr_t handle, OpcUa::NodeId id, QOpcUa::NodeAttributes attr, QString indexRange);
    void writeAttribute(uintptr_t handle, OpcUa::Node node, QOpcUa::NodeAttribute attr, QVariant value, QOpcUa::Types type, QString indexRange);
    void writeAttributes(uintptr_t handle, OpcUa::Node node, QOpcUaNode::AttributeMap toWrite, QOpcUa::Types valueAttributeType);
    void browseChildren(uintptr_t handle, OpcUa::NodeId id, QOpcUa::ReferenceTypeId referenceType, QOpcUa::NodeClasses nodeClassMask);

    QFreeOpcUaSubscription *getSubscription(const QOpcUaMonitoringParameters &settings);
    bool removeSubscription(quint32 subscriptionId);

    void enableMonitoring(uintptr_t handle, OpcUa::Node node, QOpcUa::NodeAttributes attr, const QOpcUaMonitoringParameters &settings);
    void disableMonitoring(uintptr_t handle, QOpcUa::NodeAttributes attr);
    void modifyMonitoring(uintptr_t handle, QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, QVariant value);
    void callMethod(uintptr_t handle, OpcUa::NodeId objectId, OpcUa::NodeId methodId, QVector<QOpcUa::TypedVariant> args);

private:
    QFreeOpcUaSubscription *getSubscriptionForItem(uintptr_t handle, QOpcUa::NodeAttribute attr);

    QFreeOpcUaClientImpl *m_client;

    QHash<quint32, QFreeOpcUaSubscription *> m_subscriptions;
    QHash<uintptr_t, QHash<QOpcUa::NodeAttribute, QFreeOpcUaSubscription *>> m_attributeMapping; // Handle -> Attribute -> Subscription
};

QT_END_NAMESPACE

#endif // QFREEOPCUAWORKER_H

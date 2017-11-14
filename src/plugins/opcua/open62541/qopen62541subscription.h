/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
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

#ifndef QOPEN62541SUBSCRIPTION_H
#define QOPEN62541SUBSCRIPTION_H

#include "qopen62541.h"
#include <QtOpcUa/qopcuamonitoredevent.h>
#include <QtOpcUa/qopcuamonitoredvalue.h>
#include <private/qopcuasubscriptionimpl_p.h>

QT_BEGIN_NAMESPACE

class QOpen62541Client;
class Open62541AsyncBackend;

class QOpen62541Subscription : public QOpcUaSubscriptionImpl
{
public:
    explicit QOpen62541Subscription(Open62541AsyncBackend *backend, quint32 interval);
    ~QOpen62541Subscription() override;

    QOpcUaMonitoredEvent *addEvent(QOpcUaNode *node) override;
    void removeEvent(QOpcUaMonitoredEvent *event) override;

    QOpcUaMonitoredValue *addValue(QOpcUaNode *node) override;
    void removeValue(QOpcUaMonitoredValue *v) override;

    void monitoredValueUpdated(UA_UInt32 monId, UA_DataValue *value);

    QOpcUaSubscription *m_qsubscription;

private:
    bool ensureNativeSubscription();
    void removeNativeSubscription();
    Open62541AsyncBackend *m_backend;
    quint32 m_interval;
    UA_UInt32 m_subscriptionId;
    QMap<UA_UInt32, QOpcUaMonitoredValue *> m_dataChangeHandles;
    QMap<UA_UInt32, QOpcUaMonitoredEvent *> m_eventHandles;
};

QT_END_NAMESPACE

#endif // QOPEN62541SUBSCRIPTION_H

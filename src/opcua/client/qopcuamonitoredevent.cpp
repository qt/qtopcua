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

#include "qopcuamonitoredevent.h"
#include <QtOpcUa/qopcuasubscription.h>
#include <private/qopcuamonitoredevent_p.h>

QT_BEGIN_NAMESPACE

/*!
 * \internal
 */
QOpcUaMonitoredEvent::QOpcUaMonitoredEvent(QOpcUaNode *node, QOpcUaSubscription *subscription, QObject *parent)
    : QObject(*new QOpcUaMonitoredEventPrivate(node, subscription), parent)
{}

/*!
    Destroys this event monitor instance. This will automatically
    remove it from its subscription.
 */
QOpcUaMonitoredEvent::~QOpcUaMonitoredEvent()
{
    d_func()->m_subscription->removeEvent(this);
}

/*!
    Returns the node which belongs to this event monitor instance.
*/
QOpcUaNode &QOpcUaMonitoredEvent::node()
{
    return *d_func()->m_node;
}

/*!
    \class QOpcUaMonitoredEvent
    \inmodule QtOpcUa

    \brief Represents a source for events corresponding to a node on a OPC UA server.

    \chapter QOpcUaMonitoredEvent
    Every QOpcUaMonitoredEvent emits signals when the corresponding event on
    the server is triggered. They are backed by QOpcUaSubscriptions.

    The objects are owned by the user and must be deleted when they are no longer needed.
    Deletion causes the monitored items to be unsubscribed from the server.
*/

/*!
    \fn void QOpcUaMonitoredEvent::newEvent(QVector<QVariant> val) const
    This signal is emitted when a new event is received by a data change
    subscription.
    The QVector \a val currently contains three QVariants holding the values of the
    first three event fields.
    \warning When using the FreeOPCUA backend, no signal is emitted as the
    internal implementation of events does not call the callback method.
*/

QT_END_NAMESPACE

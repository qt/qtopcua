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

#include <QtOpcUa/qopcuasubscription.h>

#include <private/qopcuasubscription_p.h>
#include <private/qopcuasubscriptionimpl_p.h>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaSubscription
    \inmodule QtOpcUa

    \brief Enables the user to utilize the
    subscription mechanism in OPC UA for event subscriptions.
*/

/*!
    \internal
 */
QOpcUaSubscription::QOpcUaSubscription(QOpcUaSubscriptionImpl *impl, quint32 interval, QObject *parent)
    : QObject(*new QOpcUaSubscriptionPrivate(impl, interval), parent)
{
}

/*!
   \fn QOpcUaSubscription::~QOpcUaSubscription()

   Destroys this subscription instance. It will also destroy the subscrption on
   the server.
 */
QOpcUaSubscription::~QOpcUaSubscription()
{
}

/*!
    \fn QOpcUaMonitoredEvent *QOpcUaSubscription::addEvent(QOpcUaNode *node)

    Create an event monitor for \a node by adding it to this subscription object.

    Returns a QOpcUaMonitoredEvent which can be used to receive a signal when an
    event occcurs.
*/
QOpcUaMonitoredEvent *QOpcUaSubscription::addEvent(QOpcUaNode *node)
{
    return d_func()->m_impl->addEvent(node);
}

/*!
   \fn QOpcUaMonitoredValue *QOpcUaSubscription::addValue(QOpcUaNode *node)

   Create a value monitor for \a node by adding it to this subscription object.

   Return a QOpcUaMonitoredEvent which can be used to receive a signal when
   the value changes.
 */
QOpcUaMonitoredValue *QOpcUaSubscription::addValue(QOpcUaNode *node)
{
    return d_func()->m_impl->addValue(node);
}

/*!
   \fn void QOpcUaSubscription::removeEvent(QOpcUaMonitoredEvent *event)

   Remove the monitored event represented by \a event from this subscription.
 */
void QOpcUaSubscription::removeEvent(QOpcUaMonitoredEvent *e)
{
    d_func()->m_impl->removeEvent(e);
}

/*!
   void QOpcUaSubscription::removeValue(QOpcUaMonitoredValue *value)

   Remove the monitored value represented by \a value from this subscription.
 */
void QOpcUaSubscription::removeValue(QOpcUaMonitoredValue *value)
{
    d_func()->m_impl->removeValue(value);
}

QT_END_NAMESPACE

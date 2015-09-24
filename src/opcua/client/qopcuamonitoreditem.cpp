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

#include "qopcuamonitoreditem.h"

#include "qopcuasubscription.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaMonitoredItem
    \inmodule QtOpcUa

    \brief A QOpcUaMonitoredItem represents a source for events corresponding
    to a node on a OPC UA server.

    \chapter QOpcUaMonitoredItem
    Every QOpcUaMonitoredItem emits signals when the corresponding nodes on the server
    change. They are backed by QOpcUaSubscriptions.
*/

/*!
    \fn void QOpcUaMonitoredItem::valueChanged(QVariant val) const

    This signal is emitted when a new update for a data change subscription
    arrives. \a val contains the new value.
*/

/*!
    \fn void QOpcUaMonitoredItem::newEvent(QList<QVariant> val) const
    This signal is emitted when a new event is received by a data change
    subscription.
    The QList \a val currently contains three QVariants holding the values of the
    first three event fields.
    \warning When using the FreeOPCUA backend, no signal is emitted as the
    internal implementation of events does not call the callback method.
*/

QOpcUaMonitoredItem::QOpcUaMonitoredItem(QOpcUaSubscription *parent)
    : QObject(parent)
    , m_pSubscription(parent)
{
}

QOpcUaSubscription *QOpcUaMonitoredItem::subscription() const
{
    return m_pSubscription;
}


QOpcUaMonitoredItem::~QOpcUaMonitoredItem()
{
    // it's only safe to unsubscribe automatically if our parent is not already being destroyed
    if (qobject_cast<QOpcUaSubscription*>(m_pSubscription))
        m_pSubscription->unsubscribe(this);
}


void QOpcUaMonitoredItem::triggerValueChanged(const QVariant &val)
{
    // explicitly use invoke to force the signal to be emitted on the main thread
    // even if the plugin triggered this from a worker thread
    QMetaObject::invokeMethod(this, "valueChanged", Qt::AutoConnection, Q_ARG(QVariant, val));
}


void QOpcUaMonitoredItem::triggerNewEvent(const QList<QVariant> &val)
{
    // explicitly use invoke to force the signal to be emitted on the main thread
    // even if the plugin triggered this from a worker thread
    QMetaObject::invokeMethod(this, "newEvent", Qt::AutoConnection, Q_ARG(QList<QVariant>, val));
}

QT_END_NAMESPACE

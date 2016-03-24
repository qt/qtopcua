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

#include "qopcuamonitoredvalue.h"
#include <private/qopcuamonitoredvalue_p.h>

#include "qopcuasubscription.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaMonitoredValue
    \inmodule QtOpcUa

    \brief Represents a source for values corresponding to a node on a OPC UA server.

    \chapter QOpcUaMonitoredValue
    Every QOpcUaMonitoredValue emits signals when the corresponding value in
    nodes on the server change. They are backed by QOpcUaSubscriptions.

    The objects are owned by the user and must be deleted when they are no longer needed.
    Deletion causes the monitored items to be unsubscribed from the server.
*/

/*!
    \fn void QOpcUaMonitoredValue::valueChanged(QVariant val) const

    This signal is emitted when a new update for a data change subscription
    arrives. \a val contains the new value.
 */

QOpcUaMonitoredValue::QOpcUaMonitoredValue(QOpcUaNode *node, QOpcUaSubscription *subscription, QObject *parent)
    : QObject(*new QOpcUaMonitoredValuePrivate(node, subscription), parent)
{
}

QOpcUaMonitoredValue::~QOpcUaMonitoredValue()
{
    d_func()->m_subscription->removeValue(this);
}

QOpcUaNode &QOpcUaMonitoredValue::node()
{
    return *d_func()->m_node;
}

QT_END_NAMESPACE

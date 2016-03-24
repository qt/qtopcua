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

#include "qopcuavaluesubscription.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaValueSubscription
    \inmodule QtOpcUa

    \brief Enables the user to utilize the
    subscription mechanism in OPC UA for data change subscriptions.

    \note This class is not exposed to application developers.
*/

/*!
    Creates an empty QOpcUaValueSubscription object with interval \a interval
 */
QOpcUaValueSubscription::QOpcUaValueSubscription(uint interval)
    : QOpcUaSubscription()
    , d_ptr(0)  // new QOpcUaValueSubscriptionPrivate(this))
{
    m_interval = interval;
}

/*!
    Return the interval of the subscription.
 */
uint QOpcUaValueSubscription::interval() const
{
   return m_interval;
}

/*!
    \fn QOpcUaMonitoredValue QOpcUaValueSubscription::addValue(const QString &nodeId)

    Adds the node with \a nodeId to this subscription.

    Must be reimplemented in the concrete plugin implementation.
*/

QT_END_NAMESPACE

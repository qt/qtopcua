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

#include "qopcuasubscription.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaSubscription
    \inmodule QtOpcUa

    \brief The QOpcSubscription class enables the user to utilize the
    subscription mechanism in OPC UA for event and data change subscriptions.

    Subscriptions are a concept in OPC UA to enable updating values by data
    changes or events instead of polling.
    The subscription has an interval in which it checks for value changes and
    sends updates to the client. For an event monitored item, the interval is
    set to 0.

    Monitored Items are used to assign the node and the node's attribute or the
    event we want to keep up with to the subscription.

    \image subscriptions.png

    After getting a subscription from QOpcUaClient via getSubscription or
    getEventSubscription, a pointer to an object of this type is returned.
    The object has a signal which is emitted on a data change or event,
    depending on which type of subscription has been requested from the server.

    Objects of this type are owned by QOpcUaClient and will be destroyed when the
    QOpcUaClient object is destroyed.
*/

/*!
    Creates an empty QOpcUaSubscription object. The value of \a parent is passed
    to the QObject constructor.
 */
QOpcUaSubscription::QOpcUaSubscription(QObject *parent)
    : QObject(parent)
{
}

/*!
    Unsubscribes the subscription from the server and deletes the wrapper
    object.
*/
QOpcUaSubscription::~QOpcUaSubscription()
{
    unsubscribe();
}

/*!
    \internal
    Returns the success status of the subscription.
 */
bool QOpcUaSubscription::success() const
{
    return true;
}

/*!
    \fn void QOpcUaSubscription::unsubscribe()

    Removes the subscription from the server.
    \warning When using the FreeOPCUA plugin, unsubscribing can result in
    unwanted behavior.
*/

/*!
    \fn int QOpcUaSubscription::addItem(const QString &xmlNodeId)

    Adds the node with \a xmlNodeId to this subscription.
*/

QT_END_NAMESPACE

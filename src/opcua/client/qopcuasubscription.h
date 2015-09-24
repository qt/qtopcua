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

#ifndef QOPCUASUBSCRIPTION_H
#define QOPCUASUBSCRIPTION_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QOpcUaMonitoredItem;

class Q_OPCUA_EXPORT QOpcUaSubscription : public QObject
{
    Q_OBJECT
public:
    virtual ~QOpcUaSubscription();

    virtual void unsubscribe() {}
    virtual bool success() const;

    virtual QOpcUaMonitoredItem *addItem(const QString &xmlNodeId) = 0;
    virtual QOpcUaMonitoredItem *addEventItem(const QString &xmlNodeId) = 0;

    virtual bool unsubscribe(QOpcUaMonitoredItem *) = 0;

protected:
    explicit QOpcUaSubscription(QObject *parent = 0);
};

QT_END_NAMESPACE

#endif // QOPCUASUBSCRIPTION_H

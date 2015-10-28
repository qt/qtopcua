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

#ifndef QOPCUANODE_H
#define QOPCUANODE_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class Q_OPCUA_EXPORT QOpcUaNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString type READ type)
    Q_PROPERTY(int id READ nodeId)
    Q_PROPERTY(QString xmlNodeId READ xmlNodeId)
    Q_PROPERTY(int childCount READ childCount)
    Q_PROPERTY(QStringList childIds READ childIds)
    Q_PROPERTY(QVariant value READ value)
    Q_PROPERTY(QString nodeClass READ nodeClass)

public:

    Q_INVOKABLE QVariant encodedValue() const ;

    virtual QString name() const = 0;
    virtual QString type() const = 0;
    virtual QVariant value() const = 0;
    virtual int nodeId() const = 0;
    virtual int childCount() const = 0;
    virtual QList<QOpcUaNode *> children() = 0;
    virtual QStringList childIds() const = 0;
    virtual QString xmlNodeId() const = 0;
    virtual QString nodeClass() const = 0;

protected:
    explicit QOpcUaNode(QObject *parent = 0);
};

QT_END_NAMESPACE

#endif // QOPCUANODE_H

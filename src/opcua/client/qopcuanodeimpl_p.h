/****************************************************************************
**
** Copyright (C) 2016 basysKom GmbH, opensource@basyskom.com
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

#ifndef QOPCUANODEIMPL_P_H
#define QOPCUANODEIMPL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtOpcUa/qopcuaglobal.h>
#include <QtOpcUa/qopcuatype.h>
#include <QtOpcUa/qopcuanode.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QOpcUaMonitoredEvent;
class QOpcUaMonitoredValue;

class Q_OPCUA_EXPORT QOpcUaNodeImpl
{
public:
    QOpcUaNodeImpl();
    virtual ~QOpcUaNodeImpl();

    virtual QString displayName() const = 0;
    virtual QOpcUa::Types type() const = 0;
    virtual QVariant value() const = 0;
    virtual QStringList childIds() const = 0;
    virtual QString nodeId() const = 0;
    virtual QOpcUaNode::NodeClass nodeClass() const = 0;

    virtual bool setValue(const QVariant &value,
            QOpcUa::Types type = QOpcUa::Undefined)  = 0;
    virtual QPair<double, double> readEuRange() const = 0;
    virtual QPair<QString, QString> readEui() const = 0;

    virtual QVector<QPair<QVariant, QDateTime> > readHistorical(
            uint maxCount, const QDateTime &begin, const QDateTime &end) const = 0;
    virtual bool writeHistorical(QOpcUa::Types type,
            const QVector<QPair<QVariant, QDateTime> > data) = 0;

    virtual bool call(const QString &methodNodeId,
            QVector<QOpcUa::TypedVariant> *args = 0, QVector<QVariant> *ret = 0) = 0;
};

QT_END_NAMESPACE

#endif // QOPCUANODEIMPL_P_H

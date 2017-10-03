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
#include <QtOpcUa/qopcuatype.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qvariant.h>

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

class QOpcUaNodePrivate;
class QOpcUaNodeImpl;
class QOpcUaClient;
class QOpcUaMonitoredEvent;
class QOpcUaMonitoredValue;

class Q_OPCUA_EXPORT QOpcUaNode : public QObject
{
public:
    Q_DECLARE_PRIVATE(QOpcUaNode)

    enum class NodeClass {
        Undefined = 0,
        Object = 1,
        Variable = 2,
        Method = 4,
        ObjectType = 8,
        VariableType = 16,
        ReferenceType = 32,
        DataType = 64,
        View = 128,
    };
    Q_ENUM(NodeClass)

    QOpcUaNode(QOpcUaNodeImpl *impl, QOpcUaClient *client, QObject *parent = nullptr);
    virtual ~QOpcUaNode();

    QString displayName() const;
    QOpcUa::Types type() const;
    QVariant value() const;
    QStringList childIds() const;
    QString nodeId() const;
    NodeClass nodeClass() const;

    bool setValue(const QVariant &value, QOpcUa::Types type = QOpcUa::Undefined);
    QPair<double, double> readEuRange() const;
    QPair<QString, QString> readEui() const;

    QVector<QPair<QVariant, QDateTime> > readHistorical(
            uint maxCount, const QDateTime &begin, const QDateTime &end) const;
    bool writeHistorical(QOpcUa::Types type, const QVector<QPair<QVariant, QDateTime> > data);

    bool call(const QString &methodNodeId, QVector<QOpcUa::TypedVariant> *args = nullptr,
              QVector<QVariant> *ret = nullptr);
private:
    Q_DISABLE_COPY(QOpcUaNode)
};

Q_OPCUA_EXPORT QDebug operator<<(QDebug dbg, const QOpcUaNode &node);

QT_END_NAMESPACE

#endif // QOPCUANODE_H

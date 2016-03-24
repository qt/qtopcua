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

#ifndef QFREEOPCUANODE_H
#define QFREEOPCUANODE_H

#include <private/qopcuanodeimpl_p.h>

// freeopcua
#include <opc/ua/node.h>

#include <QtCore/QPointer>

QT_BEGIN_NAMESPACE

class QFreeOpcUaClient;

class QFreeOpcUaNode : public QOpcUaNodeImpl
{
public:
    explicit QFreeOpcUaNode(OpcUa::Node node, QFreeOpcUaClient *client);
    ~QFreeOpcUaNode() Q_DECL_OVERRIDE;

    QString name() const Q_DECL_OVERRIDE;
    QString type() const Q_DECL_OVERRIDE;
    QStringList childIds() const Q_DECL_OVERRIDE;
    QString nodeId() const Q_DECL_OVERRIDE;
    QString nodeClass() const Q_DECL_OVERRIDE;

    QVariant value() const Q_DECL_OVERRIDE;
    bool setValue(const QVariant &value, QOpcUa::Types type) Q_DECL_OVERRIDE;
    bool call(const QString &methodNodeId,
              QVector<QOpcUa::TypedVariant> *args = 0, QVector<QVariant> *ret = 0) Q_DECL_OVERRIDE;
    QPair<QString, QString> readEui() const Q_DECL_OVERRIDE;
    QPair<double, double> readEuRange() const Q_DECL_OVERRIDE;

    QVector<QPair<QVariant, QDateTime> > readHistorical(uint maxCount,
            const QDateTime &begin, const QDateTime &end) const Q_DECL_OVERRIDE;
    Q_INVOKABLE bool writeHistorical(QOpcUa::Types type,
            const QVector<QPair<QVariant, QDateTime> > data) Q_DECL_OVERRIDE;

    OpcUa::Node opcuaNode() const;

    OpcUa::Node m_node;
    QFreeOpcUaClient* m_client;
};

QT_END_NAMESPACE

#endif // QFREEOPCUANODE_H

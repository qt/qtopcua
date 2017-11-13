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

#include <QtCore/qpointer.h>

#include <opc/ua/node.h>

namespace OpcUa
{
    class UaClient;
}

QT_BEGIN_NAMESPACE

class QFreeOpcUaClientImpl;

class QFreeOpcUaNode : public QOpcUaNodeImpl
{
public:
    explicit QFreeOpcUaNode(OpcUa::Node node, QFreeOpcUaClientImpl *client);
    ~QFreeOpcUaNode() override;

    bool readAttributes(QOpcUaNode::NodeAttributes attr) override;
    bool enableMonitoring(QOpcUaNode::NodeAttributes attr, const QOpcUaMonitoringParameters &settings);
    bool disableMonitoring(QOpcUaNode::NodeAttributes attr);
    bool modifyMonitoring(QOpcUaNode::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, const QVariant &value);
    QStringList childrenIds() const override;
    QString nodeId() const override;

    bool writeAttribute(QOpcUaNode::NodeAttribute attribute, const QVariant &value, QOpcUa::Types type) override;
    bool writeAttributes(const QOpcUaNode::AttributeMap &toWrite, QOpcUa::Types valueAttributeType) override;
    bool call(const QString &methodNodeId,
              QVector<QOpcUa::TypedVariant> *args = nullptr, QVector<QVariant> *ret = nullptr) override;
    QPair<QString, QString> readEui() const override;
    QPair<double, double> readEuRange() const override;

    OpcUa::Node m_node;
    QPointer<QFreeOpcUaClientImpl> m_client;
};

QT_END_NAMESPACE

#endif // QFREEOPCUANODE_H

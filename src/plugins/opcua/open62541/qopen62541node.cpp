/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
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

#include "qopen62541.h"
#include "qopen62541backend.h"
#include "qopen62541node.h"
#include "qopen62541utils.h"
#include "qopen62541valueconverter.h"

#include <QtCore/qdatetime.h>
#include <QtCore/qstring.h>
#include <QtCore/qvector.h>

QT_BEGIN_NAMESPACE

QOpen62541Node::QOpen62541Node(const UA_NodeId nodeId, QOpen62541Client *client, const QString nodeIdString)
    : m_client(client)
    , m_nodeIdString(nodeIdString)
    , m_nodeId(nodeId)
{
    m_client->registerNode(this);
}

QOpen62541Node::~QOpen62541Node()
{
    if (m_client)
        m_client->unregisterNode(this);

    UA_NodeId_deleteMembers(&m_nodeId);
}

bool QOpen62541Node::readAttributes(QOpcUaNode::NodeAttributes attr, const QString &indexRange)
{
    UA_NodeId tempId;
    UA_NodeId_copy(&m_nodeId, &tempId);
    return QMetaObject::invokeMethod(m_client->m_backend, "readAttributes",
                                     Qt::QueuedConnection,
                                     Q_ARG(uintptr_t, reinterpret_cast<uintptr_t>(this)),
                                     Q_ARG(UA_NodeId, tempId),
                                     Q_ARG(QOpcUaNode::NodeAttributes, attr),
                                     Q_ARG(QString, indexRange));
}

bool QOpen62541Node::enableMonitoring(QOpcUaNode::NodeAttributes attr, const QOpcUaMonitoringParameters &settings)
{
    return QMetaObject::invokeMethod(m_client->m_backend, "enableMonitoring",
                                     Qt::QueuedConnection,
                                     Q_ARG(uintptr_t, reinterpret_cast<uintptr_t>(this)),
                                     Q_ARG(UA_NodeId, m_nodeId),
                                     Q_ARG(QOpcUaNode::NodeAttributes, attr),
                                     Q_ARG(QOpcUaMonitoringParameters, settings));
}

bool QOpen62541Node::disableMonitoring(QOpcUaNode::NodeAttributes attr)
{
    return QMetaObject::invokeMethod(m_client->m_backend, "disableMonitoring",
                                     Qt::QueuedConnection,
                                     Q_ARG(uintptr_t, reinterpret_cast<uintptr_t>(this)),
                                     Q_ARG(QOpcUaNode::NodeAttributes, attr));
}

bool QOpen62541Node::modifyMonitoring(QOpcUaNode::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, const QVariant &value)
{
    return QMetaObject::invokeMethod(m_client->m_backend, "modifyMonitoring",
                                     Qt::QueuedConnection,
                                     Q_ARG(uintptr_t, reinterpret_cast<uintptr_t>(this)),
                                     Q_ARG(QOpcUaNode::NodeAttribute, attr),
                                     Q_ARG(QOpcUaMonitoringParameters::Parameter, item),
                                     Q_ARG(QVariant, value));
}

QStringList QOpen62541Node::childrenIds() const
{
    QStringList result = m_client->m_backend->childrenIds(&m_nodeId);
    return result;
}

QString QOpen62541Node::nodeId() const
{
    return m_nodeIdString;
}

bool QOpen62541Node::writeAttribute(QOpcUaNode::NodeAttribute attribute, const QVariant &value, QOpcUa::Types type, const QString &indexRange)
{
    UA_NodeId tempId;
    UA_NodeId_copy(&m_nodeId, &tempId);
    return QMetaObject::invokeMethod(m_client->m_backend, "writeAttribute",
                                     Qt::QueuedConnection,
                                     Q_ARG(uintptr_t, reinterpret_cast<uintptr_t>(this)),
                                     Q_ARG(UA_NodeId, tempId),
                                     Q_ARG(QOpcUaNode::NodeAttribute, attribute),
                                     Q_ARG(QVariant, value),
                                     Q_ARG(QOpcUa::Types, type),
                                     Q_ARG(QString, indexRange));
}

bool QOpen62541Node::writeAttributes(const QOpcUaNode::AttributeMap &toWrite, QOpcUa::Types valueAttributeType)
{
    UA_NodeId tempId;
    UA_NodeId_copy(&m_nodeId, &tempId);
    return QMetaObject::invokeMethod(m_client->m_backend, "writeAttributes",
                                     Qt::QueuedConnection,
                                     Q_ARG(uintptr_t, reinterpret_cast<uintptr_t>(this)),
                                     Q_ARG(UA_NodeId, tempId),
                                     Q_ARG(QOpcUaNode::AttributeMap, toWrite),
                                     Q_ARG(QOpcUa::Types, valueAttributeType));
}

bool QOpen62541Node::callMethod(const QString &methodNodeId, const QVector<QOpcUa::TypedVariant> &args)
{
    UA_NodeId obj;
    UA_NodeId_copy(&m_nodeId, &obj);
    return QMetaObject::invokeMethod(m_client->m_backend, "callMethod",
                                     Qt::QueuedConnection,
                                     Q_ARG(uintptr_t, reinterpret_cast<uintptr_t>(this)),
                                     Q_ARG(UA_NodeId, obj),
                                     Q_ARG(UA_NodeId, Open62541Utils::nodeIdFromQString(methodNodeId)),
                                     Q_ARG(QVector<QOpcUa::TypedVariant>, args));
}

QPair<QString, QString> QOpen62541Node::readEui() const
{
    return QPair<QString, QString>();
}

QPair<double, double> QOpen62541Node::readEuRange() const
{
    return QPair<double, double>();
}

QT_END_NAMESPACE

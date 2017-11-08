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

bool QOpen62541Node::readAttributes(QOpcUaNode::NodeAttributes attr)
{
    UA_NodeId tempId;
    UA_NodeId_copy(&m_nodeId, &tempId);
    return QMetaObject::invokeMethod(m_client->m_backend, "readAttributes",
                                     Qt::QueuedConnection,
                                     Q_ARG(uintptr_t, reinterpret_cast<uintptr_t>(this)),
                                     Q_ARG(UA_NodeId, tempId),
                                     Q_ARG(QOpcUaNode::NodeAttributes, attr));
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

bool QOpen62541Node::setValue(const QVariant &value, QOpcUa::Types type)
{
    bool result;
    QMetaObject::invokeMethod(m_client->m_backend, "writeNodeValueAttribute",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(bool, result),
                              Q_ARG(UA_NodeId, m_nodeId),
                              Q_ARG(const QVariant&, value),
                              Q_ARG(QOpcUa::Types, type));
    return result;
}

bool QOpen62541Node::call(const QString &methodNodeId, QVector<QOpcUa::TypedVariant> *args, QVector<QVariant> *ret)
{
    Q_UNUSED(methodNodeId);
    Q_UNUSED(args);
    Q_UNUSED(ret);

    return false;
}

QPair<QString, QString> QOpen62541Node::readEui() const
{
    return QPair<QString, QString>();
}

QPair<double, double> QOpen62541Node::readEuRange() const
{
    return QPair<double, double>();
}

UA_NodeId QOpen62541Node::nativeNodeId() const
{
    return m_nodeId;
}

QT_END_NAMESPACE

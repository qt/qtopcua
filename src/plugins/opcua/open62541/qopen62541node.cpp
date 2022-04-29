/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#include "qopen62541.h"
#include "qopen62541backend.h"
#include "qopen62541node.h"
#include "qopen62541utils.h"
#include "qopen62541valueconverter.h"

#include <QtCore/qdatetime.h>
#include <QtCore/qstring.h>
#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

QOpen62541Node::QOpen62541Node(const UA_NodeId nodeId, QOpen62541Client *client, const QString nodeIdString)
    : m_client(client)
    , m_nodeIdString(nodeIdString)
    , m_nodeId(nodeId)
{
    bool success = m_client->registerNode(this);
    setRegistered(success);
}

QOpen62541Node::~QOpen62541Node()
{
    if (m_client)
        m_client->unregisterNode(this);

    UA_NodeId_deleteMembers(&m_nodeId);
}

bool QOpen62541Node::readAttributes(QOpcUa::NodeAttributes attr, const QString &indexRange)
{
    if (!m_client)
        return false;

    UA_NodeId tempId;
    UA_NodeId_copy(&m_nodeId, &tempId);
    return QMetaObject::invokeMethod(m_client->m_backend, "readAttributes",
                                     Qt::QueuedConnection,
                                     Q_ARG(quint64, handle()),
                                     Q_ARG(UA_NodeId, tempId),
                                     Q_ARG(QOpcUa::NodeAttributes, attr),
                                     Q_ARG(QString, indexRange));
}

bool QOpen62541Node::enableMonitoring(QOpcUa::NodeAttributes attr, const QOpcUaMonitoringParameters &settings)
{
    if (!m_client)
        return false;

    UA_NodeId tempId;
    UA_NodeId_copy(&m_nodeId, &tempId);
    return QMetaObject::invokeMethod(m_client->m_backend, "enableMonitoring",
                                     Qt::QueuedConnection,
                                     Q_ARG(quint64, handle()),
                                     Q_ARG(UA_NodeId, tempId),
                                     Q_ARG(QOpcUa::NodeAttributes, attr),
                                     Q_ARG(QOpcUaMonitoringParameters, settings));
}

bool QOpen62541Node::disableMonitoring(QOpcUa::NodeAttributes attr)
{
    if (!m_client)
        return false;

    return QMetaObject::invokeMethod(m_client->m_backend, "disableMonitoring",
                                     Qt::QueuedConnection,
                                     Q_ARG(quint64, handle()),
                                     Q_ARG(QOpcUa::NodeAttributes, attr));
}

bool QOpen62541Node::modifyMonitoring(QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, const QVariant &value)
{
    if (!m_client)
        return false;

    return QMetaObject::invokeMethod(m_client->m_backend, "modifyMonitoring",
                                     Qt::QueuedConnection,
                                     Q_ARG(quint64, handle()),
                                     Q_ARG(QOpcUa::NodeAttribute, attr),
                                     Q_ARG(QOpcUaMonitoringParameters::Parameter, item),
                                     Q_ARG(QVariant, value));
}

QString QOpen62541Node::nodeId() const
{
    return m_nodeIdString;
}

bool QOpen62541Node::browse(const QOpcUaBrowseRequest &request)
{
    if (!m_client)
        return false;

    UA_NodeId tempId;
    UA_NodeId_copy(&m_nodeId, &tempId);
    return QMetaObject::invokeMethod(m_client->m_backend, "browse",
                                     Qt::QueuedConnection,
                                     Q_ARG(quint64, handle()),
                                     Q_ARG(UA_NodeId, tempId),
                                     Q_ARG(QOpcUaBrowseRequest, request));
}

bool QOpen62541Node::writeAttribute(QOpcUa::NodeAttribute attribute, const QVariant &value, QOpcUa::Types type, const QString &indexRange)
{
    if (!m_client)
        return false;

    UA_NodeId tempId;
    UA_NodeId_copy(&m_nodeId, &tempId);
    return QMetaObject::invokeMethod(m_client->m_backend, "writeAttribute",
                                     Qt::QueuedConnection,
                                     Q_ARG(quint64, handle()),
                                     Q_ARG(UA_NodeId, tempId),
                                     Q_ARG(QOpcUa::NodeAttribute, attribute),
                                     Q_ARG(QVariant, value),
                                     Q_ARG(QOpcUa::Types, type),
                                     Q_ARG(QString, indexRange));
}

bool QOpen62541Node::writeAttributes(const QOpcUaNode::AttributeMap &toWrite, QOpcUa::Types valueAttributeType)
{
    if (!m_client)
        return false;

    UA_NodeId tempId;
    UA_NodeId_copy(&m_nodeId, &tempId);
    return QMetaObject::invokeMethod(m_client->m_backend, "writeAttributes",
                                     Qt::QueuedConnection,
                                     Q_ARG(quint64, handle()),
                                     Q_ARG(UA_NodeId, tempId),
                                     Q_ARG(QOpcUaNode::AttributeMap, toWrite),
                                     Q_ARG(QOpcUa::Types, valueAttributeType));
}

bool QOpen62541Node::callMethod(const QString &methodNodeId, const QList<QOpcUa::TypedVariant> &args)
{
    if (!m_client)
        return false;

    UA_NodeId obj;
    UA_NodeId_copy(&m_nodeId, &obj);
    return QMetaObject::invokeMethod(m_client->m_backend, "callMethod",
                                     Qt::QueuedConnection,
                                     Q_ARG(quint64, handle()),
                                     Q_ARG(UA_NodeId, obj),
                                     Q_ARG(UA_NodeId, Open62541Utils::nodeIdFromQString(methodNodeId)),
                                     Q_ARG(QList<QOpcUa::TypedVariant>, args));
}

bool QOpen62541Node::resolveBrowsePath(const QList<QOpcUaRelativePathElement> &path)
{
    if (!m_client)
        return false;

    UA_NodeId start;
    UA_NodeId_copy(&m_nodeId, &start);

    return QMetaObject::invokeMethod(m_client->m_backend, "resolveBrowsePath", Qt::QueuedConnection,
                                             Q_ARG(quint64, handle()),
                                             Q_ARG(UA_NodeId, start),
                                             Q_ARG(QList<QOpcUaRelativePathElement>, path));
}

QT_END_NAMESPACE

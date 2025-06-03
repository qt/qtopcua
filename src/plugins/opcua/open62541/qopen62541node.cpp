// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopen62541backend.h"
#include "qopen62541node.h"
#include "qopen62541utils.h"
#include "qopen62541valueconverter.h"

#include <private/qopcuahistoryreadresponse_p.h>

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

    UA_NodeId_clear(&m_nodeId);
}

bool QOpen62541Node::readAttributes(QOpcUa::NodeAttributes attr, const QString &indexRange)
{
    if (!m_client)
        return false;

    UA_NodeId tempId;
    UA_NodeId_copy(&m_nodeId, &tempId);
    return QMetaObject::invokeMethod(m_client->m_backend, &Open62541AsyncBackend::readAttributes,
                                     Qt::QueuedConnection, handle(), tempId, attr, indexRange);
}

bool QOpen62541Node::enableMonitoring(QOpcUa::NodeAttributes attr, const QOpcUaMonitoringParameters &settings)
{
    if (!m_client)
        return false;

    UA_NodeId tempId;
    UA_NodeId_copy(&m_nodeId, &tempId);
    return QMetaObject::invokeMethod(m_client->m_backend, &Open62541AsyncBackend::enableMonitoring,
                                     Qt::QueuedConnection, handle(), tempId, attr, settings);
}

bool QOpen62541Node::disableMonitoring(QOpcUa::NodeAttributes attr)
{
    if (!m_client)
        return false;

    return QMetaObject::invokeMethod(m_client->m_backend, &Open62541AsyncBackend::disableMonitoring,
                                     Qt::QueuedConnection, handle(), attr);
}

bool QOpen62541Node::modifyMonitoring(QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, const QVariant &value)
{
    if (!m_client)
        return false;

    return QMetaObject::invokeMethod(m_client->m_backend, &Open62541AsyncBackend::modifyMonitoring,
                                     Qt::QueuedConnection, handle(), attr, item, value);
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
    return QMetaObject::invokeMethod(m_client->m_backend, &Open62541AsyncBackend::browse,
                                     Qt::QueuedConnection, handle(), tempId, request);
}

bool QOpen62541Node::writeAttribute(QOpcUa::NodeAttribute attribute, const QVariant &value, QOpcUa::Types type, const QString &indexRange)
{
    if (!m_client)
        return false;

    UA_NodeId tempId;
    UA_NodeId_copy(&m_nodeId, &tempId);
    return QMetaObject::invokeMethod(m_client->m_backend, &Open62541AsyncBackend::writeAttribute,
                                     Qt::QueuedConnection, handle(), tempId, attribute, value, type, indexRange);
}

bool QOpen62541Node::writeAttributes(const QOpcUaNode::AttributeMap &toWrite, QOpcUa::Types valueAttributeType)
{
    if (!m_client)
        return false;

    UA_NodeId tempId;
    UA_NodeId_copy(&m_nodeId, &tempId);
    return QMetaObject::invokeMethod(m_client->m_backend, &Open62541AsyncBackend::writeAttributes,
                                     Qt::QueuedConnection, handle(), tempId, toWrite, valueAttributeType);
}


bool QOpen62541Node::callMethod(const QString &methodNodeId, const QList<QOpcUa::TypedVariant> &args)

{
    if (!m_client)
        return false;

    UA_NodeId obj;
    UA_NodeId_copy(&m_nodeId, &obj);
    return QMetaObject::invokeMethod(m_client->m_backend, &Open62541AsyncBackend::callMethod,
                                     Qt::QueuedConnection, handle(), obj,
                                     Open62541Utils::nodeIdFromQString(methodNodeId), args);
}

QOpcUaHistoryReadResponse *QOpen62541Node::readHistoryRaw(const QDateTime &startTime, const QDateTime &endTime,
                                                          quint32 numValues, bool returnBounds,
                                                          QOpcUa::TimestampsToReturn timestampsToReturn)
{
    if (!m_client)
        return nullptr;

    QOpcUaHistoryReadRawRequest request({ QOpcUaReadItem(m_nodeIdString) }, startTime, endTime, timestampsToReturn);
    request.setNumValuesPerNode(numValues);
    request.setReturnBounds(returnBounds);
    return m_client->readHistoryData(request);
}

QOpcUaHistoryReadResponse *QOpen62541Node::readHistoryEvents(const QDateTime &startTime, const QDateTime &endTime,
                                                             const QOpcUaMonitoringParameters::EventFilter &filter, quint32 numValues)
{
    if (!m_client)
        return nullptr;

    QOpcUaHistoryReadEventRequest request({QOpcUaReadItem(m_nodeIdString)},
                                          startTime, endTime, filter);
    request.setNumValuesPerNode(numValues);
    return m_client->readHistoryEvents(request);
}

bool QOpen62541Node::resolveBrowsePath(const QList<QOpcUaRelativePathElement> &path)
{
    if (!m_client)
        return false;

    UA_NodeId start;
    UA_NodeId_copy(&m_nodeId, &start);

    return QMetaObject::invokeMethod(m_client->m_backend, &Open62541AsyncBackend::resolveBrowsePath,
                                     Qt::QueuedConnection, handle(), start, path);
}

QT_END_NAMESPACE

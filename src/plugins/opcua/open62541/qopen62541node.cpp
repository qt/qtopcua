// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopen62541.h"
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

QOpcUaHistoryReadResponse *QOpen62541Node::readHistoryRaw(const QDateTime &startTime, const QDateTime &endTime,
                                                          quint32 numValues, bool returnBounds)
{
    if (!m_client)
        return nullptr;

    return m_client->readHistoryData(QOpcUaHistoryReadRawRequest{{QOpcUaReadItem(m_nodeIdString)},
                                                                 startTime, endTime, numValues, returnBounds});
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

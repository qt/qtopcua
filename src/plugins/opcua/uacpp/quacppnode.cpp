/******************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
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
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#include "quacppbackend.h"
#include "quacppnode.h"
#include "quacpputils.h"

#include <QtCore/QDateTime>
#include <QtCore/QLoggingCategory>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_UACPP)

QUACppNode::QUACppNode(const UaNodeId nodeId, QUACppClient *client, const QString nodeIdString)
    : m_client(client)
    , m_nodeIdString(nodeIdString)
    , m_nodeId(nodeId)
    , m_handle(0)
{
    bool success = m_client->registerNode(this);
    setRegistered(success);
}

QUACppNode::~QUACppNode()
{
    if (m_client)
        m_client->unregisterNode(this);
}

bool QUACppNode::readAttributes(QOpcUa::NodeAttributes attr, const QString &indexRange)
{
    return QMetaObject::invokeMethod(m_client->m_backend, "readAttributes",
                                     Qt::QueuedConnection,
                                     Q_ARG(quint64, handle()),
                                     Q_ARG(UaNodeId, m_nodeId),
                                     Q_ARG(QOpcUa::NodeAttributes, attr),
                                     Q_ARG(QString, indexRange));
}

bool QUACppNode::enableMonitoring(QOpcUa::NodeAttributes attr, const QOpcUaMonitoringParameters &settings)
{
    return QMetaObject::invokeMethod(m_client->m_backend, "enableMonitoring",
                                     Qt::QueuedConnection,
                                     Q_ARG(quint64, handle()),
                                     Q_ARG(UaNodeId, m_nodeId),
                                     Q_ARG(QOpcUa::NodeAttributes, attr),
                                     Q_ARG(QOpcUaMonitoringParameters, settings));
}

bool QUACppNode::disableMonitoring(QOpcUa::NodeAttributes attr)
{
    return QMetaObject::invokeMethod(m_client->m_backend, "disableMonitoring",
                                     Qt::QueuedConnection,
                                     Q_ARG(quint64, handle()),
                                     Q_ARG(QOpcUa::NodeAttributes, attr));
}

bool QUACppNode::modifyMonitoring(QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, const QVariant &value)
{
    return QMetaObject::invokeMethod(m_client->m_backend, "modifyMonitoring",
                                     Qt::QueuedConnection,
                                     Q_ARG(quint64, handle()),
                                     Q_ARG(QOpcUa::NodeAttribute, attr),
                                     Q_ARG(QOpcUaMonitoringParameters::Parameter, item),
                                     Q_ARG(QVariant, value));
}

bool QUACppNode::browseChildren(QOpcUa::ReferenceTypeId referenceType, QOpcUa::NodeClasses nodeClassMask)
{
    return QMetaObject::invokeMethod(m_client->m_backend, "browseChildren",
                                     Qt::QueuedConnection,
                                     Q_ARG(quint64, handle()),
                                     Q_ARG(UaNodeId, m_nodeId),
                                     Q_ARG(QOpcUa::ReferenceTypeId, referenceType),
                                     Q_ARG(QOpcUa::NodeClasses, nodeClassMask));

}

QString QUACppNode::nodeId() const
{
    return m_nodeIdString;
}

bool QUACppNode::writeAttribute(QOpcUa::NodeAttribute attribute, const QVariant &value, QOpcUa::Types type, const QString &indexRange)
{
    return QMetaObject::invokeMethod(m_client->m_backend, "writeAttribute",
                                     Qt::QueuedConnection,
                                     Q_ARG(quint64, handle()),
                                     Q_ARG(UaNodeId, m_nodeId),
                                     Q_ARG(QOpcUa::NodeAttribute, attribute),
                                     Q_ARG(QVariant, value),
                                     Q_ARG(QOpcUa::Types, type),
                                     Q_ARG(QString, indexRange));
}

bool QUACppNode::writeAttributes(const QOpcUaNode::AttributeMap &toWrite, QOpcUa::Types valueAttributeType)
{
    return QMetaObject::invokeMethod(m_client->m_backend, "writeAttributes",
                                     Qt::QueuedConnection,
                                     Q_ARG(quint64, handle()),
                                     Q_ARG(UaNodeId, m_nodeId),
                                     Q_ARG(QOpcUaNode::AttributeMap, toWrite),
                                     Q_ARG(QOpcUa::Types, valueAttributeType));

}

bool QUACppNode::callMethod(const QString &methodNodeId, const QVector<QOpcUa::TypedVariant> &args)
{
    const UaNodeId methodId = UACppUtils::nodeIdFromQString(methodNodeId);

    return QMetaObject::invokeMethod(m_client->m_backend, "callMethod",
                                     Qt::QueuedConnection,
                                     Q_ARG(quint64, handle()),
                                     Q_ARG(UaNodeId, m_nodeId),
                                     Q_ARG(UaNodeId, methodId),
                                     Q_ARG(QVector<QOpcUa::TypedVariant>, args));
}

bool QUACppNode::resolveBrowsePath(const QVector<QOpcUa::QRelativePathElement> &path)
{
    return QMetaObject::invokeMethod(m_client->m_backend, "resolveBrowsePath",
                                     Qt::QueuedConnection,
                                     Q_ARG(quint64, handle()),
                                     Q_ARG(UaNodeId, m_nodeId),
                                     Q_ARG(QVector<QOpcUa::QRelativePathElement>, path));
}

QT_END_NAMESPACE

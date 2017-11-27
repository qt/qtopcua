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

#include "qfreeopcuaclient.h"
#include "qfreeopcuanode.h"
#include "qfreeopcuasubscription.h"
#include "qfreeopcuavalueconverter.h"
#include "qfreeopcuaworker.h"

#include <QtCore/qdatetime.h>
#include <QtCore/qloggingcategory.h>

#include <opc/ua/client/client.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_FREEOPCUA)

QFreeOpcUaNode::QFreeOpcUaNode(OpcUa::Node node, QFreeOpcUaClientImpl *client)
    : m_node(node)
    , m_client(client)
{
    m_client->registerNode(this);
}

QFreeOpcUaNode::~QFreeOpcUaNode()
{
    if (m_client)
        m_client->unregisterNode(this);
}

bool QFreeOpcUaNode::readAttributes(QOpcUaNode::NodeAttributes attr)
{
    return QMetaObject::invokeMethod(m_client->m_opcuaWorker, "readAttributes",
                                     Qt::QueuedConnection,
                                     Q_ARG(uintptr_t, reinterpret_cast<uintptr_t>(this)),
                                     Q_ARG(OpcUa::NodeId, m_node.GetId()),
                                     Q_ARG(QOpcUaNode::NodeAttributes, attr));
}

bool QFreeOpcUaNode::enableMonitoring(QOpcUaNode::NodeAttributes attr, const QOpcUaMonitoringParameters &settings)
{
    return QMetaObject::invokeMethod(m_client->m_opcuaWorker, "enableMonitoring",
                                     Qt::QueuedConnection,
                                     Q_ARG(uintptr_t, reinterpret_cast<uintptr_t>(this)),
                                     Q_ARG(OpcUa::Node, m_node),
                                     Q_ARG(QOpcUaNode::NodeAttributes, attr),
                                     Q_ARG(QOpcUaMonitoringParameters, settings));
}

bool QFreeOpcUaNode::disableMonitoring(QOpcUaNode::NodeAttributes attr)
{
    return QMetaObject::invokeMethod(m_client->m_opcuaWorker, "disableMonitoring",
                                     Qt::QueuedConnection,
                                     Q_ARG(uintptr_t, reinterpret_cast<uintptr_t>(this)),
                                     Q_ARG(QOpcUaNode::NodeAttributes, attr));
}

bool QFreeOpcUaNode::modifyMonitoring(QOpcUaNode::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, const QVariant &value)
{
    return QMetaObject::invokeMethod(m_client->m_opcuaWorker, "modifyMonitoring",
                                     Qt::QueuedConnection,
                                     Q_ARG(uintptr_t, reinterpret_cast<uintptr_t>(this)),
                                     Q_ARG(QOpcUaNode::NodeAttribute, attr),
                                     Q_ARG(QOpcUaMonitoringParameters::Parameter, item),
                                     Q_ARG(QVariant, value));
}

QStringList QFreeOpcUaNode::childrenIds() const
{
    QStringList result;
    try {
        std::vector<OpcUa::Node> tmp = m_node.GetChildren();
        result.reserve(tmp.size());
        for (std::vector<OpcUa::Node>::const_iterator it = tmp.cbegin(); it != tmp.end(); ++it)
            result.append(QFreeOpcUaValueConverter::nodeIdToString(it->GetId()));
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "Failed to get child ids for node:" << ex.what();
    }

    return result;
}

QString QFreeOpcUaNode::nodeId() const
{
    try {
        return QFreeOpcUaValueConverter::nodeIdToString(m_node.GetId());
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "Failed to get id for node:" << ex.what();
        return QString();
    }
}

bool QFreeOpcUaNode::writeAttribute(QOpcUaNode::NodeAttribute attribute, const QVariant &value, QOpcUa::Types type)
{
    return QMetaObject::invokeMethod(m_client->m_opcuaWorker, "writeAttribute",
                                     Qt::QueuedConnection,
                                     Q_ARG(uintptr_t, reinterpret_cast<uintptr_t>(this)),
                                     Q_ARG(OpcUa::Node, m_node),
                                     Q_ARG(QOpcUaNode::NodeAttribute, attribute),
                                     Q_ARG(QVariant, value),
                                     Q_ARG(QOpcUa::Types, type));
}

bool QFreeOpcUaNode::writeAttributes(const QOpcUaNode::AttributeMap &toWrite, QOpcUa::Types valueAttributeType)
{
    return QMetaObject::invokeMethod(m_client->m_opcuaWorker, "writeAttributes",
                                     Qt::QueuedConnection,
                                     Q_ARG(uintptr_t, reinterpret_cast<uintptr_t>(this)),
                                     Q_ARG(OpcUa::Node, m_node),
                                     Q_ARG(QOpcUaNode::AttributeMap, toWrite),
                                     Q_ARG(QOpcUa::Types, valueAttributeType));
}

bool QFreeOpcUaNode::callMethod(const QString &methodNodeId, const QVector<QOpcUa::TypedVariant> &args)
{
    return QMetaObject::invokeMethod(m_client->m_opcuaWorker, "callMethod",
                                     Qt::QueuedConnection,
                                     Q_ARG(uintptr_t, reinterpret_cast<uintptr_t>(this)),
                                     Q_ARG(OpcUa::NodeId, m_node.GetId()),
                                     Q_ARG(OpcUa::NodeId, QFreeOpcUaValueConverter::scalarFromQVariant<OpcUa::NodeId>(methodNodeId)),
                                     Q_ARG(QVector<QOpcUa::TypedVariant>, args));
}

// Support for structures in freeopcua seems to be not implemented yet
QPair<QString, QString> QFreeOpcUaNode::readEui() const
{
    // Return empty QVariant
    return QPair<QString, QString>();
}

// Support for structures in freeopcua seems to be not implemented yet
QPair<double, double> QFreeOpcUaNode::readEuRange() const
{
    return QPair<double, double>(qQNaN(), qQNaN());
}

QT_END_NAMESPACE

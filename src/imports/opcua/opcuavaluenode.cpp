/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#include "opcuavaluenode.h"
#include "opcuaconnection.h"
#include "opcuanodeid.h"
#include "opcuaattributevalue.h"
#include <QLoggingCategory>
#include <QMetaEnum>

QT_BEGIN_NAMESPACE

/*!
    \qmltype ValueNode
    \inqmlmodule QtOpcUa
    \brief Represents a value node from a server.
    \inherits Node
    \since QtOpcUa 5.12

    \code
    import QtOpcUa 5.13 as QtOpcUa

    QtOpcUa.ValueNode {
        nodeId: QtOpcUa.NodeId {
            ns: "Test Namespace"
            identifier: "s=TestName"
        }
        connection: myConnection
    }
    \endcode

    A subscription will be created on the server in order to track value changes on the server.

    \sa NodeId, Connection, Node
*/

/*!
    \qmlproperty variant ValueNode::value

    Value of this node.
    Reading and writing this property will access the node on the server.
*/

/*!
    \qmlproperty Date ValueNode::sourceTimestamp
    \readonly

    Source timestamp of the value attribute.
*/

/*!
    \qmlproperty Date ValueNode::serverTimestamp
    \readonly

    Server timestamp of the value attribute.
*/

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_QML)

OpcUaValueNode::OpcUaValueNode(QObject *parent):
    OpcUaNode(parent)
{
    connect(m_attributeCache.attribute(QOpcUa::NodeAttribute::Value), &OpcUaAttributeValue::changed, this, &OpcUaValueNode::valueChanged);
}

OpcUaValueNode::~OpcUaValueNode()
{
}

void OpcUaValueNode::setValue(const QVariant &value)
{
    if (!m_connection || !m_node)
        return;
    m_node->writeAttribute(QOpcUa::NodeAttribute::Value, value, QOpcUa::Types::Undefined);
}

template<typename T> QString enumToString(T value) {
    const auto metaEnum = QMetaEnum::fromType<T>();
    const auto key = metaEnum.valueToKey(static_cast<int>(value));
    if (key)
        return QString::fromLatin1(key);
    else
        return QString();
}

void OpcUaValueNode::setupNode(const QString &absolutePath)
{
    // Additionally read the value attribute
    setAttributesToRead(attributesToRead()
                        | QOpcUa::NodeAttribute::Value
                        | QOpcUa::NodeAttribute::DataType);

    OpcUaNode::setupNode(absolutePath);
    if (!m_node)
        return;

    connect(m_node, &QOpcUaNode::attributeWritten, this, [this](QOpcUa::NodeAttribute attribute, QOpcUa::UaStatusCode statusCode) {
        if (statusCode != QOpcUa::Good) {
            QString msg = "Failed to write attribute "
                    + enumToString(attribute)
                    + ": "
                    + enumToString(statusCode);
            setStatus(Status::FailedToWriteAttribute, msg);
            qCWarning(QT_OPCUA_PLUGINS_QML) << msg;
        }
    });

    connect(m_node, &QOpcUaNode::enableMonitoringFinished, this, [this](QOpcUa::NodeAttribute attr, QOpcUa::UaStatusCode statusCode){
        if (attr != QOpcUa::NodeAttribute::Value)
            return;
        if (statusCode == QOpcUa::Good) {
            m_monitored = true;
            emit monitoredChanged(m_monitored);
        } else {
            qCWarning(QT_OPCUA_PLUGINS_QML) << "Failed to enable monitoring";
            setStatus(Status::FailedToSetupMonitoring);
        }
    });
    connect(m_node, &QOpcUaNode::disableMonitoringFinished, this, [this](QOpcUa::NodeAttribute attr, QOpcUa::UaStatusCode statusCode){
        if (attr != QOpcUa::NodeAttribute::Value)
            return;
        if (statusCode == QOpcUa::Good) {
            m_monitored = false;
            emit monitoredChanged(m_monitored);
        } else {
            setStatus(Status::FailedToDisableMonitoring);
            qCWarning(QT_OPCUA_PLUGINS_QML) << "Failed to disable monitoring";
        }
    });
    connect(m_node, &QOpcUaNode::monitoringStatusChanged, this, [this](QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameters items,
                                                                   QOpcUa::UaStatusCode statusCode) {
       if (attr != QOpcUa::NodeAttribute::Value)
           return;
       if (statusCode != QOpcUa::Good) {
           setStatus(Status::FailedToModifyMonitoring);
           qCWarning(QT_OPCUA_PLUGINS_QML) << "Failed to modify monitoring";
       } else {
           if (items & QOpcUaMonitoringParameters::Parameter::PublishingInterval) {
               m_publishingInterval = m_node->monitoringStatus(QOpcUa::NodeAttribute::Value).publishingInterval();
               emit publishingIntervalChanged(m_publishingInterval);
           }
       }
    });

    setMonitored(true);
}

bool OpcUaValueNode::checkValidity()
{
    if (!m_connection || !m_node)
        return false;

    if (m_node->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>() != QOpcUa::NodeClass::Variable) {
        setStatus(Status::InvalidNodeType);
        return false;
    } else {
        return true;
    }

}

QVariant OpcUaValueNode::value() const
{
    if (!m_connection || !m_node)
        return QVariant();
    return m_node->attribute(QOpcUa::NodeAttribute::Value);
}

QDateTime OpcUaValueNode::serverTimestamp() const
{
    return getServerTimestamp(QOpcUa::NodeAttribute::Value);
}

QDateTime OpcUaValueNode::sourceTimestamp() const
{
    return getSourceTimestamp(QOpcUa::NodeAttribute::Value);
}

bool OpcUaValueNode::monitored() const
{
    if (!m_connection || !m_node)
        return false;

    return m_monitored;
}

void OpcUaValueNode::setMonitored(bool monitored)
{
    if (!m_connection || !m_node)
        return;
    if (m_monitored == monitored)
        return;

    if (monitored) {
        m_node->enableMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters(m_publishingInterval));
    } else {
        m_node->disableMonitoring(QOpcUa::NodeAttribute::Value);
    }
}

double OpcUaValueNode::publishingInterval() const
{
    if (!m_connection || !m_node)
        return 0.0;

    auto monitoringStatus = node()->monitoringStatus(QOpcUa::NodeAttribute::Value);
    if (monitoringStatus.statusCode() == QOpcUa::BadNoEntryExists)
        return 0.0;
    return monitoringStatus.publishingInterval();
}

void OpcUaValueNode::setPublishingInterval(double publishingInterval)
{
    if (!m_connection || !m_node)
        return;
    if (qFuzzyCompare(m_publishingInterval, publishingInterval))
        return;

     m_node->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::PublishingInterval, publishingInterval);
}

QT_END_NAMESPACE

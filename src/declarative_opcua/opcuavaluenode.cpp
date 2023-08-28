// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/opcuavaluenode_p.h>
#include <private/opcuaconnection_p.h>
#include <private/opcuanodeid_p.h>
#include <private/opcuaattributevalue_p.h>

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
    import QtOpcUa as QtOpcUa

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
    \qmlproperty variant ValueNode::valueType

    Type type of this node.
    The initial value will be \l {QOpcUa::Undefined}{QtOpcUa.Constants.Undefined} and be fetched from the server when the first connection is established.
    Any value will be written to the server as the specified type.
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
    connect(this, &OpcUaValueNode::filterChanged, this, &OpcUaValueNode::updateFilters);
}

OpcUaValueNode::~OpcUaValueNode()
{
}

void OpcUaValueNode::setValue(const QVariant &value)
{
    if (!m_connection || !m_node)
        return;
    m_node->writeAttribute(QOpcUa::NodeAttribute::Value, value, m_valueType);
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
            QString msg = QStringLiteral("Failed to write attribute ")
                    + enumToString(attribute)
                    + QStringLiteral(": ")
                    + enumToString(statusCode);
            setStatus(Status::FailedToWriteAttribute, msg);
            qCWarning(QT_OPCUA_PLUGINS_QML) << msg;
            }
      });


    connect(m_node, &QOpcUaNode::attributeUpdated, this, [this](QOpcUa::NodeAttribute attr, QVariant value) {
        if (attr == QOpcUa::NodeAttribute::DataType && m_valueType == QOpcUa::Types::Undefined) {
            const auto valueType = QOpcUa::opcUaDataTypeToQOpcUaType(value.toString());
            m_valueType = valueType;
        }
    });

    connect(m_node, &QOpcUaNode::enableMonitoringFinished, this, [this](QOpcUa::NodeAttribute attr, QOpcUa::UaStatusCode statusCode){
        if (attr != QOpcUa::NodeAttribute::Value)
            return;
        if (statusCode == QOpcUa::Good) {
            m_monitoredState = true;
            emit monitoredChanged(m_monitoredState);
            qCDebug(QT_OPCUA_PLUGINS_QML) << "Monitoring was enabled for node" << resolvedNode().fullNodeId();
            updateFilters();
        } else {
            qCWarning(QT_OPCUA_PLUGINS_QML) << "Failed to enable monitoring for node" << resolvedNode().fullNodeId();
            setStatus(Status::FailedToSetupMonitoring);
        }
    });
    connect(m_node, &QOpcUaNode::disableMonitoringFinished, this, [this](QOpcUa::NodeAttribute attr, QOpcUa::UaStatusCode statusCode){
        if (attr != QOpcUa::NodeAttribute::Value)
            return;
        // The monitoring is gone in this case, regardless of the status code
        if (statusCode == QOpcUa::Good ||
                node()->monitoringStatus(QOpcUa::NodeAttribute::Value).statusCode()== QOpcUa::UaStatusCode::BadNoEntryExists) {
            m_monitoredState = false;
            emit monitoredChanged(m_monitoredState);
            qCDebug(QT_OPCUA_PLUGINS_QML) << "Monitoring was disabled for node "<< resolvedNode().fullNodeId();
        } else {
            qCWarning(QT_OPCUA_PLUGINS_QML) << "Failed to disable monitoring for node "<< resolvedNode().fullNodeId();
            setStatus(Status::FailedToDisableMonitoring);
        }
    });
    connect(m_node, &QOpcUaNode::monitoringStatusChanged, this, [this](QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameters items,
                                                                   QOpcUa::UaStatusCode statusCode) {
       if (attr != QOpcUa::NodeAttribute::Value && attr != QOpcUa::NodeAttribute::EventNotifier)
           return;
       if (statusCode != QOpcUa::Good) {
           setStatus(Status::FailedToModifyMonitoring);
           qCWarning(QT_OPCUA_PLUGINS_QML) << "Failed to modify monitoring";
       } else {
           if (items & QOpcUaMonitoringParameters::Parameter::PublishingInterval) {
               if (m_publishingInterval != m_node->monitoringStatus(QOpcUa::NodeAttribute::Value).publishingInterval()) {
                   m_publishingInterval = m_node->monitoringStatus(QOpcUa::NodeAttribute::Value).publishingInterval();
                   emit publishingIntervalChanged(m_publishingInterval);
               }
           }
       }
    });

    updateSubscription();
}

void OpcUaValueNode::updateFilters() const
{
    if (!m_connection || !m_node || !m_filter || !m_monitoredState)
        return;

    m_node->modifyDataChangeFilter(QOpcUa::NodeAttribute::Value, m_filter->filter());
}

bool OpcUaValueNode::checkValidity()
{
    if (!m_connection || !m_node)
        return false;

    if (m_node->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>() != QOpcUa::NodeClass::Variable) {
        setStatus(Status::InvalidNodeType);
        return false;
    }

    setStatus(Status::Valid);
    return true;
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
        return m_monitored;

    return m_monitoredState;
}

void OpcUaValueNode::updateSubscription()
{
    if (!m_connection || !m_node)
        return;

    QOpcUaMonitoringParameters parameters;
    parameters.setPublishingInterval(m_publishingInterval);

    if (m_filter)
        parameters.setFilter(m_filter->filter());

    if (m_monitoredState != m_monitored) {
        if (m_monitored) {
            m_node->enableMonitoring(QOpcUa::NodeAttribute::Value, parameters);
        } else {
            m_node->disableMonitoring(QOpcUa::NodeAttribute::Value);
        }
    }
}
void OpcUaValueNode::setMonitored(bool monitored)
{
    m_monitored = monitored;
    updateSubscription();
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

OpcUaDataChangeFilter *OpcUaValueNode::filter() const
{
    return m_filter;
}

void OpcUaValueNode::setFilter(OpcUaDataChangeFilter *filter)
{
    bool changed = false;

    if (m_filter) {
        disconnect(m_filter, &OpcUaDataChangeFilter::filterChanged, this, &OpcUaValueNode::updateFilters);
        changed = !(*m_filter == *filter);
    } else {
        changed = true;
    }

    m_filter = filter;
    connect(m_filter, &OpcUaDataChangeFilter::filterChanged, this, &OpcUaValueNode::updateFilters);

    if (changed)
        emit filterChanged();
}

void OpcUaValueNode::setPublishingInterval(double publishingInterval)
{
    if (!m_connection || !m_node)
        return;
    if (qFuzzyCompare(m_publishingInterval, publishingInterval))
        return;

     m_node->modifyMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters::Parameter::PublishingInterval, publishingInterval);
}

QOpcUa::Types OpcUaValueNode::valueType() const
{
    return m_valueType;
}

void OpcUaValueNode::setValueType(QOpcUa::Types valueType)
{
    m_valueType = valueType;
}

QT_END_NAMESPACE

// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/opcuanode_p.h>
#include <private/opcuarelativenodepath_p.h>
#include <private/opcuarelativenodeid_p.h>
#include <private/opcuapathresolver_p.h>
#include <private/opcuaattributevalue_p.h>

#include <qopcuatype.h>
#include <QOpcUaNode>
#include <QOpcUaClient>
#include <QLoggingCategory>

QT_BEGIN_NAMESPACE

/*!
    \qmltype Node
    \inqmlmodule QtOpcUa
    \brief Represents a node on a server.
    \since QtOpcUa 5.12

    \code
    import QtOpcUa as QtOpcUa

    QtOpcUa.Node {
        nodeId : QtOpcUa.NodeId {
            identifier: "s=Example.Method"
            ns: "Example Namespace"
        }
        connection: myConnection
    }
    \endcode
*/

/*!
    \qmlproperty NodeId Node::nodeId

    ID of the node on the server to be used.
    This can be an absolute node ID or a relative node ID.

    \sa NodeId, RelativeNodeId
*/

/*!
    \qmlproperty Connection Node::connection

    The connection to be used for node instances.
    The node will automatically be accessible when the associated connection
    has established a connection to a server.

    If this property is not set, the default connection will be used, if any.

    \sa Connection, Connection::defaultConnection
*/

/*!
    \qmlproperty bool Node::readyToUse
    \readonly

    This property returns whether the node is ready to use.
    This happens once after a successful connection to a server was established
    and the node was successfully set up.
*/

/*!
    \qmlsignal Connection::nodeChanged()

    Emitted when the underlying node has changed.
    This happens when the namespace or identifier of the \l NodeId changed.
*/

/*!
    \qmlproperty  QOpcUa::NodeClass Node::nodeClass
    \readonly

    The node class of the node. In case the information is not available
    \c QtOpcUa.Constants.NodeClass.Undefined is returned.
*/

/*!
    \qmlproperty string Node::browseName

    The browse name of the node. In case the information is not available
    an empty string is returned.
*/

/*!
    \qmlproperty LocalizedText Node::displayName

    The localized text of the node. In case the information is not available
    a default constructed \l LocalizedText is returned.
*/

/*!
    \qmlproperty LocalizedText Node::description

    The description of the node. In case the information is not available
    a default constructed \l LocalizedText is returned.
*/

/*!
    \qmlproperty Status Node::status
    \readonly

    Current status of the node. The node is only usable when the status is \c Valid.
    In any other case it indicates an error.

    \sa errorMessage, Status
*/

/*!
    \qmlproperty string Node::errorMessage
    \readonly

    A string representation of the current status code.

    \sa status, Status
*/

/*!
    \qmlproperty enumeration Node::Status

    Status of a node.

    \value Node.Status.Valid Node is ready for use
    \value Node.Status.InvalidNodeId Node id is invalid
    \value Node.Status.NoConnection Not connected to a server
    \value Node.Status.InvalidNodeType Node type on the server does not match the QML type
    \value Node.Status.InvalidClient Invalid connection client
    \value Node.Status.FailedToResolveNode Failed to resolve node
    \value Node.Status.InvalidObjectNode The object node is invalid or its type does not match the expected type
    \value Node.Status.FailedToReadAttributes Failed to read attributes
    \value Node.Status.FailedToSetupMonitoring Failed to setup monitoring

    \sa status, errorMessage
*/

/*!
    \qmlproperty EventFilter Node::eventFilter
    \since 5.13

    An event filter allows monitoring events on the server for certain conditions.

    \sa EventFilter
*/

Q_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_QML, "qt.opcua.plugins.qml")

OpcUaNode::OpcUaNode(QObject *parent):
    QObject(parent),
    m_nodeId(new OpcUaNodeIdType(this)),
    m_attributesToRead(QOpcUaNode::mandatoryBaseAttributes()),
    m_status(Status::InvalidNodeId)
{
    m_attributesToRead |= QOpcUa::NodeAttribute::Description;
    connect(&m_resolvedNode, &UniversalNode::nodeChanged, this, &OpcUaNode::nodeChanged);
    connect(m_attributeCache.attribute(QOpcUa::NodeAttribute::BrowseName), &OpcUaAttributeValue::changed, this, &OpcUaNode::browseNameChanged);
    connect(m_attributeCache.attribute(QOpcUa::NodeAttribute::NodeClass), &OpcUaAttributeValue::changed, this, &OpcUaNode::nodeClassChanged);
    connect(m_attributeCache.attribute(QOpcUa::NodeAttribute::DisplayName), &OpcUaAttributeValue::changed, this, &OpcUaNode::displayNameChanged);
    connect(m_attributeCache.attribute(QOpcUa::NodeAttribute::Description), &OpcUaAttributeValue::changed, this, &OpcUaNode::descriptionChanged);
}

OpcUaNode::~OpcUaNode()
{
    delete m_node;
}

OpcUaNodeIdType *OpcUaNode::nodeId() const
{
    return m_nodeId;
}

OpcUaConnection *OpcUaNode::connection()
{
    if (!m_connection)
        setConnection(OpcUaConnection::defaultConnection());

    return m_connection;
}

bool OpcUaNode::readyToUse() const
{
    return m_readyToUse;
}

void OpcUaNode::setBrowseName(const QString &value)
{
    if (!m_connection || !m_node)
        return;
    if (!m_resolvedNode.isNamespaceIndexValid())
        return;

    m_node->writeAttribute(QOpcUa::NodeAttribute::BrowseName, QOpcUaQualifiedName(m_resolvedNode.namespaceIndex(), value));
}

QString OpcUaNode::browseName()
{
    return m_attributeCache.attributeValue(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>().name();
}

QOpcUa::NodeClass OpcUaNode::nodeClass()
{
    return m_attributeCache.attributeValue(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>();
}

void OpcUaNode::setDisplayName(const QOpcUaLocalizedText &value)
{
    if (!m_connection || !m_node)
        return;
    m_node->writeAttribute(QOpcUa::NodeAttribute::DisplayName, value);
}

QOpcUaLocalizedText OpcUaNode::displayName()
{
    return m_attributeCache.attributeValue(QOpcUa::NodeAttribute::DisplayName).value<QOpcUaLocalizedText>();
}

void OpcUaNode::setDescription(const QOpcUaLocalizedText &value)
{
    if (!m_connection || !m_node)
        return;
    m_node->writeAttribute(QOpcUa::NodeAttribute::Description, value);
}

QOpcUaLocalizedText OpcUaNode::description()
{
    return m_attributeCache.attributeValue(QOpcUa::NodeAttribute::Description).value<QOpcUaLocalizedText>();
}

OpcUaNode::Status OpcUaNode::status() const
{
    return m_status;
}

const QString &OpcUaNode::errorMessage() const
{
    return m_errorMessage;
}

/*!
    \qmlmethod Date Node::getSourceTimestamp(QOpcUa::NodeAttribute attribute)

    Returns the source timestamp of the given \a attribute.
*/
QDateTime OpcUaNode::getSourceTimestamp(QOpcUa::NodeAttribute attribute) const
{
    if (!m_connection || !m_node)
        return QDateTime();
    return m_node->sourceTimestamp(attribute);
}

/*!
    \qmlmethod Date Node::getServerTimestamp(Constants::NodeAttribute attribute)

    Returns the server timestamp of the given \a attribute.
*/
QDateTime OpcUaNode::getServerTimestamp(QOpcUa::NodeAttribute attribute) const
{
    if (!m_connection || !m_node)
        return QDateTime();
    return m_node->serverTimestamp(attribute);
}

void OpcUaNode::setNodeId(OpcUaNodeIdType *nodeId)
{
    if (m_nodeId == nodeId)
        return;

    // This deletes the initial dummy object that was created in the
    // constructor in case a "real" nodeId is set.
    if (m_nodeId->parent() == this)
        m_nodeId->deleteLater();

    // Disconnect signals from old node
    m_nodeId->disconnect(this);
    m_nodeId = nodeId;
    connect(m_nodeId, &OpcUaNodeIdType::nodeChanged, this, &OpcUaNode::updateNode);
    connect(m_nodeId, &OpcUaNodeIdType::destroyed, this, [&]() { m_nodeId = nullptr; });

    updateNode();
}

void OpcUaNode::setConnection(OpcUaConnection *connection)
{
    if (connection == m_connection)
        return;

    m_connection = connection;
    connect(connection, SIGNAL(connectedChanged()), this, SLOT(updateNode()));

    updateNode();
    emit connectionChanged(connection);
}

void OpcUaNode::setupNode(const QString &absoluteNodePath)
{
    m_attributeCache.invalidate();
    m_absoluteNodePath = absoluteNodePath;

    if (m_node) {
        m_node->deleteLater();

        // Prevents a race condition where an update from the old node appears after the cache has been invalidate
        QObject::disconnect(m_attributeUpdatedConnection);
        QObject::disconnect(m_attributeReadConnection);
        QObject::disconnect(m_enableMonitoringFinishedConnection);
        QObject::disconnect(m_disableMonitoringFinishedConnection);
        QObject::disconnect(m_monitoringStatusChangedConnection);
        QObject::disconnect(m_eventOccurredConnection);

        m_node = nullptr;
    }

    if (m_absoluteNodePath.isEmpty())
        return;

    auto conn = connection();
    if (!conn || !m_nodeId || !conn->m_client)
        return;

    if (!conn->connected())
        return;

    m_node = conn->m_client->node(m_absoluteNodePath);
    if (!m_node) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "Invalid node:" << m_absoluteNodePath;
        return;
    }

    m_attributeUpdatedConnection = connect(m_node, &QOpcUaNode::attributeUpdated,
                                           &m_attributeCache, &OpcUaAttributeCache::setAttributeValue);

    m_attributeReadConnection = connect(m_node, &QOpcUaNode::attributeRead, this, [this](){
        setReadyToUse(true);
    });

    m_enableMonitoringFinishedConnection = connect(m_node, &QOpcUaNode::enableMonitoringFinished, this,
                                                   [this](QOpcUa::NodeAttribute attr, QOpcUa::UaStatusCode statusCode){
        if (attr != QOpcUa::NodeAttribute::EventNotifier)
            return;
        if (statusCode == QOpcUa::Good) {
            m_eventFilterActive = true;
            qCDebug(QT_OPCUA_PLUGINS_QML) << "Event filter was enabled for node" << resolvedNode().fullNodeId();
            updateEventFilter();
        } else {
            qCWarning(QT_OPCUA_PLUGINS_QML) << "Failed to enable event filter for node" << resolvedNode().fullNodeId();
            setStatus(Status::FailedToSetupMonitoring);
        }
    });

    m_disableMonitoringFinishedConnection = connect(m_node, &QOpcUaNode::disableMonitoringFinished, this,
                                                    [this](QOpcUa::NodeAttribute attr, QOpcUa::UaStatusCode statusCode){
        if (attr != QOpcUa::NodeAttribute::EventNotifier)
            return;
        if (statusCode == QOpcUa::Good) {
            m_eventFilterActive = false;
            qCDebug(QT_OPCUA_PLUGINS_QML) << "Event filter was disabled for node "<< resolvedNode().fullNodeId();
        } else {
            qCWarning(QT_OPCUA_PLUGINS_QML) << "Failed to disable event filter for node "<< resolvedNode().fullNodeId();
            setStatus(Status::FailedToDisableMonitoring);
        }
    });

    m_monitoringStatusChangedConnection = connect(m_node, &QOpcUaNode::monitoringStatusChanged, this,
                                                  [this](QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameters items,
                                                  QOpcUa::UaStatusCode statusCode) {
        Q_UNUSED(items);
        if (attr != QOpcUa::NodeAttribute::EventNotifier)
            return;
        if (statusCode != QOpcUa::Good) {
            setStatus(Status::FailedToModifyMonitoring);
            qCWarning(QT_OPCUA_PLUGINS_QML) << "Failed to modify event filter for" << m_node->nodeId();
        }
    });

    m_eventOccurredConnection = connect (m_node, &QOpcUaNode::eventOccurred, this, &OpcUaNode::eventOccurred);


    // Read mandatory attributes
    if (!m_node->readAttributes(m_attributesToRead)) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "Reading attributes" << m_node->nodeId() << "failed";
        setStatus(Status::FailedToReadAttributes);
    }

    updateEventFilter();
}

void OpcUaNode::updateNode()
{
    retrieveAbsoluteNodePath(m_nodeId, [this](const QString &absoluteNodePath) {setupNode(absoluteNodePath);});
}

OpcUaEventFilter *OpcUaNode::eventFilter() const
{
    return m_eventFilter;
}

void OpcUaNode::setEventFilter(OpcUaEventFilter *eventFilter)
{
    bool changed = false;

    if (m_eventFilter) {
        disconnect(m_eventFilter, &OpcUaEventFilter::dataChanged, this, &OpcUaNode::updateEventFilter);
        changed = !(*m_eventFilter == *eventFilter);
    } else {
        changed = true;
    }

    m_eventFilter = eventFilter;
    connect(m_eventFilter, &OpcUaEventFilter::dataChanged, this, &OpcUaNode::updateEventFilter);

    if (changed)
        emit eventFilterChanged();
}


void OpcUaNode::updateEventFilter()
{
    if (!m_connection || !m_node || !m_eventFilter)
        return;

    if (m_eventFilterActive) {
        m_node->modifyEventFilter(m_eventFilter->filter(m_connection->m_client));
    } else {
        QOpcUaMonitoringParameters parameters;
        parameters.setFilter(m_eventFilter->filter(m_connection->m_client));
        m_node->enableMonitoring(QOpcUa::NodeAttribute::EventNotifier, parameters);
        m_eventFilterActive = true;
    }
}

void OpcUaNode::setStatus(OpcUaNode::Status status, const QString &message)
{
    QString errorMessage(message);
    bool emitStatusChanged = false;
    bool emitErrorMessageChanged = false;

    if (m_status != status) {
        m_status = status;
        emitStatusChanged = true;
    }

    // if error message is not given, use default error message
    if (errorMessage.isEmpty()) {
        switch (m_status) {
        case Status::Valid:
            errorMessage = tr("Node is valid");
            break;
        case Status::InvalidNodeId:
            errorMessage = tr("Node Id is invalid");
            break;
        case Status::NoConnection:
            errorMessage = tr("Not connected to server");
            break;
        case Status::InvalidNodeType:
            errorMessage = tr("QML element does not match node type on the server");
            break;
        case Status::InvalidClient:
            errorMessage = tr("Connecting client is invalid");
            break;
        case Status::FailedToResolveNode:
            errorMessage = tr("Failed to resolve node");
            break;
        case Status::InvalidObjectNode:
            errorMessage = tr("Invalid object node");
            break;
        case Status::FailedToReadAttributes:
            errorMessage = tr("Failed to read attributes");
            break;
        case Status::FailedToSetupMonitoring:
            errorMessage = tr("Failed to setup monitoring");
            break;
        case Status::FailedToWriteAttribute:
            errorMessage = tr("Failed to write attribute");
            break;
        case Status::FailedToModifyMonitoring:
            errorMessage = tr("Failed to modify monitoring");
            break;
        case Status::FailedToDisableMonitoring:
            errorMessage = tr("Failed to disable monitoring");
            break;
        }
    }

    if (errorMessage != m_errorMessage) {
        m_errorMessage = errorMessage;
        emitErrorMessageChanged = true;
    }

    if (emitStatusChanged)
        emit statusChanged();
    if (emitErrorMessageChanged)
        emit errorMessageChanged();
}

const UniversalNode &OpcUaNode::resolvedNode() const
{
    return m_resolvedNode;
}

QOpcUaNode *OpcUaNode::node() const
{
    return m_node;
}

void OpcUaNode::setAttributesToRead(QOpcUa::NodeAttributes attributes)
{
    m_attributesToRead = attributes;
}

QOpcUa::NodeAttributes OpcUaNode::attributesToRead() const
{
    return m_attributesToRead;
}

void OpcUaNode::retrieveAbsoluteNodePath(OpcUaNodeIdType *node, std::function<void (const QString &)> functor)
{
    auto conn = connection();
    if (!conn) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "No connection to server";
        setStatus(Status::NoConnection);
        return;
    }
    if (!m_nodeId) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "Invalid node ID";
        setStatus(Status::InvalidNodeId);
        return;
    }
    if (!conn->m_client) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "Client instance is invalid";
        setStatus(Status::InvalidClient);
        return;
    }

    if (!conn->connected()) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "not connected";
        return;
    }

    if (qobject_cast<const OpcUaNodeId *>(node)) {
        UniversalNode tmp(node);
        tmp.resolveNamespace(conn->m_client);
        m_resolvedNode.from(tmp);
        functor(m_resolvedNode.fullNodeId());
        emit nodeIdChanged(m_nodeId);
        emit nodeChanged();
    } else if (qobject_cast<OpcUaRelativeNodeId *>(node)) {
        auto nodeId = qobject_cast<OpcUaRelativeNodeId *>(node);
        OpcUaPathResolver *resolver = new OpcUaPathResolver(nodeId, conn->m_client, this);
        connect(resolver, &OpcUaPathResolver::resolvedNode, this, [this, functor, resolver](UniversalNode nodeToUse, const QString &errorMessage) {
            resolver->deleteLater();

            if (!errorMessage.isEmpty()) {
                qCWarning(QT_OPCUA_PLUGINS_QML) << "Failed to resolve node:" << errorMessage;
                setStatus(Status::FailedToResolveNode, errorMessage);
                functor(QString());
                return;
            }

            m_resolvedNode.from(nodeToUse);
            functor(m_resolvedNode.fullNodeId());
            emit nodeIdChanged(m_nodeId);
            emit nodeChanged();
        });
        resolver->startResolving();
    } else {
        functor(QString());
    }
}

void OpcUaNode::setReadyToUse(bool value)
{
    if (value && !checkValidity())
        value = false;

    bool old = m_readyToUse;
    m_readyToUse = value;

    if (value)
        setStatus(Status::Valid);

    if (!old && value)
        emit readyToUseChanged();
}

bool OpcUaNode::checkValidity()
{
    return true;
}

QT_END_NAMESPACE

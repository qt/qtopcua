// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "opcuamachinebackend.h"

#include <QOpcUaProvider>
#include <QQmlEngine>

using namespace Qt::Literals::StringLiterals;

OpcUaMachineBackend::OpcUaMachineBackend(QObject *parent)
    : QObject(parent)
    , m_percentFilledTank1(0)
    , m_percentFilledTank2(0)
    , m_tank2TargetPercent(0)
    , m_tank2ValveState(false)
    , m_machineState(MachineState::Idle)
    , m_connected(false)
    , m_message(u"Ready to connect"_s)
    , m_successfullyCreated(false)
{
    qRegisterMetaType<OpcUaMachineBackend::MachineState>();
    qmlRegisterType<OpcUaMachineBackend>("OpcUaMachineBackend", 1, 0, "OpcUaMachineBackend");

    QOpcUaProvider provider;
    setBackends(provider.availableBackends());
}

OpcUaMachineBackend::~OpcUaMachineBackend()
{
    if (m_client && m_client->state() == QOpcUaClient::Connected)
        m_client->disconnectFromEndpoint();
}

void OpcUaMachineBackend::clientStateHandler(QOpcUaClient::ClientState state)
{
    m_connected = (state == QOpcUaClient::ClientState::Connected);

    emit connectedChanged(m_connected);

    if (state == QOpcUaClient::ClientState::Connected) {
        setMessage(u"Connected"_s);
        // Create node objects for reading, writing and subscriptions
        m_machineNode.reset(m_client->node(u"ns=2;s=Machine"_s));
        m_machineStateNode.reset(m_client->node(u"ns=2;s=Machine.State"_s));
        m_percentFilledTank1Node.reset(m_client->node(u"ns=2;s=Machine.Tank1.PercentFilled"_s));
        m_percentFilledTank2Node.reset(m_client->node(u"ns=2;s=Machine.Tank2.PercentFilled"_s));
        m_tank2TargetPercentNode.reset(m_client->node(u"ns=2;s=Machine.Tank2.TargetPercent"_s));
        m_tank2ValveStateNode.reset(m_client->node(u"ns=2;s=Machine.Tank2.ValveState"_s));
        m_machineDesignationNode.reset(m_client->node(u"ns=2;s=Machine.Designation"_s));

        // Connect signal handlers for subscribed values
        QObject::connect(m_machineStateNode.data(),
                         &QOpcUaNode::dataChangeOccurred,
                         this,
                         &OpcUaMachineBackend::machineStateUpdated);
        QObject::connect(m_percentFilledTank1Node.data(),
                         &QOpcUaNode::dataChangeOccurred,
                         this,
                         &OpcUaMachineBackend::percentFilledTank1Updated);
        QObject::connect(m_percentFilledTank2Node.data(),
                         &QOpcUaNode::dataChangeOccurred,
                         this,
                         &OpcUaMachineBackend::percentFilledTank2Updated);
        QObject::connect(m_tank2TargetPercentNode.data(),
                         &QOpcUaNode::dataChangeOccurred,
                         this,
                         &OpcUaMachineBackend::tank2TargetPercentUpdated);
        QObject::connect(m_tank2ValveStateNode.data(),
                         &QOpcUaNode::dataChangeOccurred,
                         this,
                         &OpcUaMachineBackend::tank2ValveStateUpdated);

        // Subscribe to data changes
        m_machineStateNode->enableMonitoring(
                QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters(100));
        m_percentFilledTank1Node->enableMonitoring(
                QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters(100));
        m_percentFilledTank2Node->enableMonitoring(
                QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters(100));
        m_tank2TargetPercentNode->enableMonitoring(
                QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters(100));
        m_tank2ValveStateNode->enableMonitoring(
                QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters(100));

        // Connect the handler for async reading
        QObject::connect(m_machineDesignationNode.data(),
                         &QOpcUaNode::attributeRead,
                         this,
                         &OpcUaMachineBackend::machineDesignationRead);

        // Request the value attribute of the machine designation node
        m_machineDesignationNode->readAttributes(QOpcUa::NodeAttribute::Value);

        // Add handlers for write and call results
        QObject::connect(m_tank2TargetPercentNode.data(),
                         &QOpcUaNode::attributeWritten,
                         this,
                         &OpcUaMachineBackend::setpointWritten);
        QObject::connect(m_machineNode.data(),
                         &QOpcUaNode::methodCallFinished,
                         this,
                         &OpcUaMachineBackend::handleMethodResult);

        // Add handlers for enableMonitoring results
        QObject::connect(m_machineStateNode.data(),
                         &QOpcUaNode::enableMonitoringFinished,
                         this,
                         &OpcUaMachineBackend::enableMonitoringFinished);
        QObject::connect(m_percentFilledTank1Node.data(),
                         &QOpcUaNode::enableMonitoringFinished,
                         this,
                         &OpcUaMachineBackend::enableMonitoringFinished);
        QObject::connect(m_percentFilledTank2Node.data(),
                         &QOpcUaNode::enableMonitoringFinished,
                         this,
                         &OpcUaMachineBackend::enableMonitoringFinished);
        QObject::connect(m_tank2TargetPercentNode.data(),
                         &QOpcUaNode::enableMonitoringFinished,
                         this,
                         &OpcUaMachineBackend::enableMonitoringFinished);
        QObject::connect(m_tank2ValveStateNode.data(),
                         &QOpcUaNode::enableMonitoringFinished,
                         this,
                         &OpcUaMachineBackend::enableMonitoringFinished);
    }

    if (state == QOpcUaClient::ClientState::Connecting)
        setMessage(u"Connecting"_s);

    if (state == QOpcUaClient::ClientState::Disconnected) {
        setMessage(u"Disconnected: %1"_s
                   .arg(QMetaEnum::fromType<QOpcUaClient::ClientError>().valueToKey(
                            static_cast<int>(m_client->error()))));
    }
}

void OpcUaMachineBackend::machineStateUpdated(QOpcUa::NodeAttribute attr, const QVariant &value)
{
    Q_UNUSED(attr);
    MachineState newState = static_cast<MachineState>(value.toUInt());
    if (newState != m_machineState) {
        m_machineState = newState;
        emit machineStateChanged(m_machineState);
    }
}

void OpcUaMachineBackend::percentFilledTank1Updated(QOpcUa::NodeAttribute attr, const QVariant &value)
{
    Q_UNUSED(attr);
    m_percentFilledTank1 = value.toDouble();
    emit percentFilledTank1Changed(m_percentFilledTank1);
}

void OpcUaMachineBackend::percentFilledTank2Updated(QOpcUa::NodeAttribute attr, const QVariant &value)
{
    Q_UNUSED(attr);
    m_percentFilledTank2 = value.toDouble();
    emit percentFilledTank2Changed(m_percentFilledTank2);
}

void OpcUaMachineBackend::tank2TargetPercentUpdated(QOpcUa::NodeAttribute attr, const QVariant &value)
{
    Q_UNUSED(attr);
    m_tank2TargetPercent = value.toDouble();
    emit tank2TargetPercentChanged(m_tank2TargetPercent);
}

void OpcUaMachineBackend::tank2ValveStateUpdated(QOpcUa::NodeAttribute attr, const QVariant &value)
{
    Q_UNUSED(attr);
    m_tank2ValveState = value.toBool();
    emit tank2ValveStateChanged(m_tank2ValveState);
}

void OpcUaMachineBackend::machineDesignationRead(QOpcUa::NodeAttributes attr)
{
    if (attr & QOpcUa::NodeAttribute::Value) { // Make sure the value attribute has been read
        if (m_machineDesignationNode->attributeError(QOpcUa::NodeAttribute::Value) == QOpcUa::UaStatusCode::Good) { // Make sure there was no error
            m_machineDesignation = m_machineDesignationNode->attribute(QOpcUa::NodeAttribute::Value).toString(); // Get the attribute from the cache
            emit machineDesignationChanged(m_machineDesignation);
        }
    }
}

void OpcUaMachineBackend::setpointWritten(QOpcUa::NodeAttribute attr, QOpcUa::UaStatusCode status)
{
    if (attr == QOpcUa::NodeAttribute::Value && status == QOpcUa::UaStatusCode::Good)
        setMessage(u"Setpoint successfully set"_s);
    else if (attr == QOpcUa::NodeAttribute::Value && status != QOpcUa::UaStatusCode::Good)
        setMessage(u"Failed to set setpoint"_s);
}

void OpcUaMachineBackend::handleMethodResult(QString methodNodeId,
                                             const QVariant &result,
                                             QOpcUa::UaStatusCode statusCode)
{
    Q_UNUSED(result);

    if (methodNodeId == u"ns=2;s=Machine.Start"_s) {
        if (statusCode == QOpcUa::UaStatusCode::Good)
            setMessage(u"Pump successfully started"_s);
        else
            setMessage(u"Unable to start pump"_s);
    } else if (methodNodeId == u"ns=2;s=Machine.Stop"_s) {
        if (statusCode == QOpcUa::UaStatusCode::Good)
            setMessage(u"Pump successfully stopped"_s);
        else
            setMessage(u"Unable to stop pump"_s);
    } else if (methodNodeId == u"ns=2;s=Machine.FlushTank2"_s) {
        if (statusCode == QOpcUa::UaStatusCode::Good)
            setMessage(u"Flushing tank 2 successfully started"_s);
        else
            setMessage(u"Unable to flush tank 2"_s);
    } else if (methodNodeId == u"ns=2;s=Machine.Reset"_s) {
        if (statusCode == QOpcUa::UaStatusCode::Good)
            setMessage(u"Simulation successfully reset"_s);
        else
            setMessage(u"Unable to reset simulation"_s);
    }
}

void OpcUaMachineBackend::enableMonitoringFinished(QOpcUa::NodeAttribute attr,
                                                   QOpcUa::UaStatusCode status)
{
    Q_UNUSED(attr);
    if (!sender())
        return;
    if (status == QOpcUa::UaStatusCode::Good) {
        qDebug() << "Monitoring successfully enabled for"
                 << qobject_cast<QOpcUaNode *>(sender())->nodeId();
    } else {
        qDebug() << "Failed to enable monitoring for"
                 << qobject_cast<QOpcUaNode *>(sender())->nodeId() << ":" << status;
        setMessage(u"Failed to enable monitoring"_s);
    }
}

void OpcUaMachineBackend::setBackends(const QStringList &backends)
{
    if (m_backends != backends) {
        m_backends = backends;
        emit backendsChanged(m_backends);
    }
}

QStringList OpcUaMachineBackend::backends() const
{
    return m_backends;
}

double OpcUaMachineBackend::percentFilledTank1() const
{
    return m_percentFilledTank1;
}

double OpcUaMachineBackend::percentFilledTank2() const
{
    return m_percentFilledTank2;
}

OpcUaMachineBackend::MachineState OpcUaMachineBackend::machineState() const
{
    return m_machineState;
}

void OpcUaMachineBackend::machineWriteTank2TargetPercent(double value)
{
    if (m_tank2TargetPercentNode)
        m_tank2TargetPercentNode->writeAttribute(QOpcUa::NodeAttribute::Value, value);
}

void OpcUaMachineBackend::startPump()
{
    m_machineNode->callMethod(u"ns=2;s=Machine.Start"_s);
}

void OpcUaMachineBackend::stopPump()
{
    if (m_machineNode)
        m_machineNode->callMethod(u"ns=2;s=Machine.Stop"_s);
}

void OpcUaMachineBackend::flushTank2()
{
    if (m_machineNode)
        m_machineNode->callMethod(u"ns=2;s=Machine.FlushTank2"_s);
}

void OpcUaMachineBackend::resetSimulation()
{
    if (m_machineNode)
        m_machineNode->callMethod(u"ns=2;s=Machine.Reset"_s);
}

void OpcUaMachineBackend::requestEndpointsFinished(const QList<QOpcUaEndpointDescription> &endpoints)
{
    if (endpoints.isEmpty()) {
       qWarning() << "The server did not return any endpoints";
       clientStateHandler(QOpcUaClient::ClientState::Disconnected);
       return;
    }
    m_client->connectToEndpoint(endpoints.at(0));
}

void OpcUaMachineBackend::setMessage(const QString &message)
{
    if (message != m_message) {
        m_message = message;
        emit messageChanged(m_message);
    }
}

bool OpcUaMachineBackend::successfullyCreated() const
{
    return m_successfullyCreated;
}

QString OpcUaMachineBackend::message() const
{
    return m_message;
}

QString OpcUaMachineBackend::machineDesignation() const
{
    return m_machineDesignation;
}

bool OpcUaMachineBackend::connected() const
{
    return m_connected;
}

void OpcUaMachineBackend::connectToEndpoint(const QString &url, qint32 index)
{
    if (m_connected)
        return;

    QOpcUaProvider provider;

    if (index < 0 || index >= m_backends.size())
        return; // Invalid index

    if (!m_client || (m_client && m_client->backend() != m_backends.at(index))) {
        m_client.reset(provider.createClient(m_backends.at(index)));
        if (m_client) {
            QObject::connect(m_client.data(),
                             &QOpcUaClient::endpointsRequestFinished,
                             this,
                             &OpcUaMachineBackend::requestEndpointsFinished);
            QObject::connect(m_client.data(),
                             &QOpcUaClient::stateChanged,
                             this,
                             &OpcUaMachineBackend::clientStateHandler);
        }
    }

    if (!m_client) {
        qWarning() << "Could not create client";
        m_successfullyCreated = false;
        return;
    }

    m_successfullyCreated = true;
    m_client->requestEndpoints(url);
}

void OpcUaMachineBackend::disconnectFromEndpoint()
{
    if (m_connected)
        m_client->disconnectFromEndpoint();
}

bool OpcUaMachineBackend::tank2ValveState() const
{
    return m_tank2ValveState;
}

double OpcUaMachineBackend::tank2TargetPercent() const
{
    return m_tank2TargetPercent;
}

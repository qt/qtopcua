// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "simulationserver.h"
#include <qopen62541utils.h>
#include <qopen62541valueconverter.h>
#include <QtOpcUa/qopcuatype.h>

#include <QtCore/QDebug>
#include <QtCore/QLoggingCategory>
#include <QtCore/QUuid>

#include <cstring>

QT_BEGIN_NAMESPACE

// Node ID conversion is included from the open62541 plugin but warnings from there should be logged
// using qt.opcua.testserver instead of qt.opcua.plugins.open62541 for usage in the test server
Q_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_OPEN62541, "qt.opcua.demoserver")

DemoServer::DemoServer(QObject *parent)
    : QObject(parent)
    , m_state(DemoServer::MachineState::Idle)
    , m_percentFilledTank1(100)
    , m_percentFilledTank2(0)
{
    m_timer.setInterval(0);
    m_timer.setSingleShot(false);
    m_machineTimer.setInterval(200);
    connect(&m_timer, &QTimer::timeout, this, &DemoServer::processServerEvents);
}

DemoServer::~DemoServer()
{
    shutdown();
    UA_Server_delete(m_server);
    UA_NodeId_clear(&m_percentFilledTank1Node);
    UA_NodeId_clear(&m_percentFilledTank2Node);
    UA_NodeId_clear(&m_tank2TargetPercentNode);
    UA_NodeId_clear(&m_tank2ValveStateNode);
    UA_NodeId_clear(&m_machineStateNode);
}

bool DemoServer::init()
{
    m_server = UA_Server_new();

    if (!m_server)
        return false;

    UA_StatusCode result = UA_ServerConfig_setMinimal(UA_Server_getConfig(m_server), 43344, nullptr);

    if (result != UA_STATUSCODE_GOOD)
        return false;

    return true;
}

void DemoServer::processServerEvents()
{
    if (m_running)
        UA_Server_run_iterate(m_server, true);
}

void DemoServer::shutdown()
{
    if (m_running) {
        UA_Server_run_shutdown(m_server);
        m_running = false;
    }
}

UA_NodeId DemoServer::addObject(const QString &parent, const QString &nodeString, const QString &browseName,
                                const QString &displayName, const QString &description, quint32 referenceType)
{
    UA_NodeId resultNode;
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;

    oAttr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", displayName.toUtf8().constData());
    if (description.size())
        oAttr.description = UA_LOCALIZEDTEXT_ALLOC("en-US", description.toUtf8().constData());

    UA_StatusCode result;
    UA_NodeId requestedNodeId = Open62541Utils::nodeIdFromQString(nodeString);
    UA_NodeId parentNodeId = Open62541Utils::nodeIdFromQString(parent);

    UA_QualifiedName nodeBrowseName = UA_QUALIFIEDNAME_ALLOC(requestedNodeId.namespaceIndex, browseName.toUtf8().constData());

    result = UA_Server_addObjectNode(m_server,
                                     requestedNodeId,
                                     parentNodeId,
                                     UA_NODEID_NUMERIC(0, referenceType),
                                     nodeBrowseName,
                                     UA_NODEID_NULL,
                                     oAttr,
                                     nullptr,
                                     &resultNode);

    UA_QualifiedName_clear(&nodeBrowseName);
    UA_NodeId_clear(&requestedNodeId);
    UA_NodeId_clear(&parentNodeId);
    UA_ObjectAttributes_clear(&oAttr);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add folder:" << nodeString << " :" << result;
        return UA_NODEID_NULL;
    }
    return resultNode;
}

UA_NodeId DemoServer::addVariable(const UA_NodeId &folder, const QString &variableNode, const QString &browseName,
                                  const QString &displayName, const QVariant &value, QOpcUa::Types type,
                                  quint32 referenceType)
{
    UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.value = QOpen62541ValueConverter::toOpen62541Variant(value, type);
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", displayName.toUtf8().constData());
    attr.dataType = attr.value.type ? attr.value.type->typeId : UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_QualifiedName variableName = UA_QUALIFIEDNAME_ALLOC(variableNodeId.namespaceIndex, browseName.toUtf8().constData());

    UA_NodeId resultId;
    UA_StatusCode result = UA_Server_addVariableNode(m_server,
                                                     variableNodeId,
                                                     folder,
                                                     UA_NODEID_NUMERIC(0, referenceType),
                                                     variableName,
                                                     UA_NODEID_NULL,
                                                     attr,
                                                     nullptr,
                                                     &resultId);

    UA_NodeId_clear(&variableNodeId);
    UA_VariableAttributes_clear(&attr);
    UA_QualifiedName_clear(&variableName);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add variable:" << result;
        return UA_NODEID_NULL;
    }

    return resultId;
}

UA_StatusCode DemoServer::startPumpMethod(UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle, const UA_NodeId *methodId, void *methodContext, const UA_NodeId *objectId, void *objectContext, size_t inputSize, const UA_Variant *input, size_t outputSize, UA_Variant *output)
{
    Q_UNUSED(server);
    Q_UNUSED(sessionId);
    Q_UNUSED(sessionHandle);
    Q_UNUSED(methodId);
    Q_UNUSED(objectId);
    Q_UNUSED(objectContext);
    Q_UNUSED(inputSize);
    Q_UNUSED(input);
    Q_UNUSED(outputSize);
    Q_UNUSED(output);

    DemoServer *data = static_cast<DemoServer *>(methodContext);

    double targetValue = data->readTank2TargetValue();

    if (data->m_state == MachineState::Idle && data->m_percentFilledTank1 > 0 && data->m_percentFilledTank2 < targetValue) {
        qDebug() << "Start pumping";
        data->setState(MachineState::Pumping);
        data->m_machineTimer.start();
        return UA_STATUSCODE_GOOD;
    }
    else {
        qDebug() << "Machine already running";
        return UA_STATUSCODE_BADUSERACCESSDENIED;
    }
}

UA_StatusCode DemoServer::stopPumpMethod(UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle, const UA_NodeId *methodId, void *methodContext, const UA_NodeId *objectId, void *objectContext, size_t inputSize, const UA_Variant *input, size_t outputSize, UA_Variant *output)
{
    Q_UNUSED(server);
    Q_UNUSED(sessionId);
    Q_UNUSED(sessionHandle);
    Q_UNUSED(methodId);
    Q_UNUSED(objectId);
    Q_UNUSED(objectContext);
    Q_UNUSED(inputSize);
    Q_UNUSED(input);
    Q_UNUSED(outputSize);
    Q_UNUSED(output);

    DemoServer *data = static_cast<DemoServer *>(methodContext);

    if (data->m_state == MachineState::Pumping) {
        qDebug() << "Stopping";
        data->m_machineTimer.stop();
        data->setState(MachineState::Idle);
        return UA_STATUSCODE_GOOD;
    } else {
        qDebug() << "Nothing to stop";
        return UA_STATUSCODE_BADUSERACCESSDENIED;
    }
}

UA_StatusCode DemoServer::flushTank2Method(UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle, const UA_NodeId *methodId, void *methodContext, const UA_NodeId *objectId, void *objectContext, size_t inputSize, const UA_Variant *input, size_t outputSize, UA_Variant *output)
{
    Q_UNUSED(server);
    Q_UNUSED(sessionId);
    Q_UNUSED(sessionHandle);
    Q_UNUSED(methodId);
    Q_UNUSED(objectId);
    Q_UNUSED(objectContext);
    Q_UNUSED(inputSize);
    Q_UNUSED(input);
    Q_UNUSED(outputSize);
    Q_UNUSED(output);

    DemoServer *data = static_cast<DemoServer *>(methodContext);

    double targetValue = data->readTank2TargetValue();

    if (data->m_state == MachineState::Idle && data->m_percentFilledTank2 > targetValue) {
        data->setState(MachineState::Flushing);
        qDebug() << "Flushing tank 2";
        data->setTank2ValveState(true);
        data->m_machineTimer.start();
        return UA_STATUSCODE_GOOD;
    }
    else {
        qDebug() << "Unable to comply";
        return UA_STATUSCODE_BADUSERACCESSDENIED;
    }
}

UA_StatusCode DemoServer::resetMethod(UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle, const UA_NodeId *methodId, void *methodContext, const UA_NodeId *objectId, void *objectContext, size_t inputSize, const UA_Variant *input, size_t outputSize, UA_Variant *output)
{
    Q_UNUSED(server);
    Q_UNUSED(sessionId);
    Q_UNUSED(sessionHandle);
    Q_UNUSED(methodId);
    Q_UNUSED(objectId);
    Q_UNUSED(objectContext);
    Q_UNUSED(inputSize);
    Q_UNUSED(input);
    Q_UNUSED(outputSize);
    Q_UNUSED(output);

    DemoServer *data = static_cast<DemoServer *>(methodContext);

        qDebug() << "Reset simulation";
        data->setState(MachineState::Idle);
        data->m_machineTimer.stop();
        data->setTank2ValveState(false);
        data->setPercentFillTank1(100);
        data->setPercentFillTank2(0);
        return UA_STATUSCODE_GOOD;
}

void DemoServer::setState(DemoServer::MachineState state)
{
    UA_Variant val;
    m_state = state;
    UA_Variant_setScalarCopy(&val, &state, &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_writeValue(m_server, m_machineStateNode, val);
}

void DemoServer::setPercentFillTank1(double fill)
{
    UA_Variant val;
    m_percentFilledTank1 = fill;
    UA_Variant_setScalarCopy(&val, &fill, &UA_TYPES[UA_TYPES_DOUBLE]);
    UA_Server_writeValue(this->m_server, this->m_percentFilledTank1Node, val);
}

void DemoServer::setPercentFillTank2(double fill)
{
    UA_Variant val;
    m_percentFilledTank2 = fill;
    UA_Variant_setScalarCopy(&val, &fill, &UA_TYPES[UA_TYPES_DOUBLE]);
    UA_Server_writeValue(this->m_server, this->m_percentFilledTank2Node, val);
}

void DemoServer::setTank2ValveState(bool state)
{
    UA_Variant val;
    UA_Variant_setScalarCopy(&val, &state, &UA_TYPES[UA_TYPES_BOOLEAN]);
    UA_Server_writeValue(this->m_server, this->m_tank2ValveStateNode, val);
}

double DemoServer::readTank2TargetValue()
{
    UA_Variant var;
    UA_Server_readValue(m_server, m_tank2TargetPercentNode, &var);
    return static_cast<double *>(var.data)[0];
}

UA_NodeId DemoServer::addMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description,
                                const QString &browseName, const QString &displayName, UA_MethodCallback cb,
                                quint32 referenceType)
{
    UA_NodeId methodNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    UA_MethodAttributes attr = UA_MethodAttributes_default;

    attr.description = UA_LOCALIZEDTEXT_ALLOC("en-US", description.toUtf8().constData());
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", displayName.toUtf8().constData());
    attr.executable = true;
    UA_QualifiedName methodBrowseName = UA_QUALIFIEDNAME_ALLOC(methodNodeId.namespaceIndex, browseName.toUtf8().constData());

    UA_NodeId resultId;
    UA_StatusCode result = UA_Server_addMethodNode(m_server, methodNodeId, folder,
                                                     UA_NODEID_NUMERIC(0, referenceType),
                                                     methodBrowseName,
                                                     attr, cb,
                                                     0, nullptr,
                                                     0, nullptr,
                                                     this, &resultId);

    UA_NodeId_clear(&methodNodeId);
    UA_MethodAttributes_clear(&attr);
    UA_QualifiedName_clear(&methodBrowseName);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add Method:" << result;
        return UA_NODEID_NULL;
    }
    return resultId;
}

void DemoServer::launch()
{
    UA_StatusCode s = UA_Server_run_startup(m_server);
    if (s != UA_STATUSCODE_GOOD)
         qFatal("Could not launch server");
     m_running = true;
     m_timer.start();

     int ns1 = UA_Server_addNamespace(m_server, "Demo Namespace");
     if (ns1 != 2) {
         qFatal("Unexpected namespace index for Demo namespace");
     }

     UA_NodeId machineObject = addObject(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::ObjectsFolder), "ns=2;s=Machine",
                                         "Machine", "Machine", "The machine simulator", UA_NS0ID_ORGANIZES);
     UA_NodeId tank1Object = addObject("ns=2;s=Machine", "ns=2;s=Machine.Tank1", "Tank1", "Tank 1");
     UA_NodeId tank2Object = addObject("ns=2;s=Machine", "ns=2;s=Machine.Tank2", "Tank2", "Tank 2");

     m_percentFilledTank1Node = addVariable(tank1Object, "ns=2;s=Machine.Tank1.PercentFilled", "PercentFilled", "Tank 1 Fill Level", 100.0, QOpcUa::Types::Double);
     m_percentFilledTank2Node = addVariable(tank2Object, "ns=2;s=Machine.Tank2.PercentFilled", "PercentFilled", "Tank 2 Fill Level", 0.0, QOpcUa::Types::Double);
     m_tank2TargetPercentNode = addVariable(tank2Object, "ns=2;s=Machine.Tank2.TargetPercent", "TargetPercent", "Tank 2 Target Level", 0.0, QOpcUa::Types::Double);
     m_tank2ValveStateNode = addVariable(tank2Object, "ns=2;s=Machine.Tank2.ValveState", "ValveState", "Tank 2 Valve State", false, QOpcUa::Types::Boolean);
     m_machineStateNode = addVariable(machineObject, "ns=2;s=Machine.State", "State", "Machine State", static_cast<quint32>(MachineState::Idle), QOpcUa::Types::UInt32);
     UA_NodeId tempId;
     tempId = addVariable(machineObject, "ns=2;s=Machine.Designation", "Designation", "Machine Designation", "TankExample", QOpcUa::Types::String);
     UA_NodeId_clear(&tempId);

     tempId = addMethod(machineObject, "ns=2;s=Machine.Start", "Starts the pump", "Start", "Start Pump", &startPumpMethod);
     UA_NodeId_clear(&tempId);
     tempId = addMethod(machineObject, "ns=2;s=Machine.Stop", "Stops the pump", "Stop", "Stop Pump", &stopPumpMethod);
     UA_NodeId_clear(&tempId);
     tempId = addMethod(machineObject, "ns=2;s=Machine.FlushTank2", "Flushes tank 2", "FlushTank2", "Flush Tank 2", &flushTank2Method);
     UA_NodeId_clear(&tempId);
     tempId = addMethod(machineObject, "ns=2;s=Machine.Reset", "Resets the simulation", "Reset", "Reset Simulation", &resetMethod);
     UA_NodeId_clear(&tempId);

     UA_NodeId_clear(&machineObject);
     UA_NodeId_clear(&tank1Object);
     UA_NodeId_clear(&tank2Object);

     QObject::connect(&m_machineTimer, &QTimer::timeout, [this]() {

         double targetValue = readTank2TargetValue();
         if (m_state == MachineState::Pumping && m_percentFilledTank1 > 0 && m_percentFilledTank2 < targetValue) {
            setPercentFillTank1(m_percentFilledTank1 - 1);
            setPercentFillTank2(m_percentFilledTank2 + 1);
            if (qFuzzyIsNull(m_percentFilledTank1) || m_percentFilledTank2 >= targetValue) {
                setState(MachineState::Idle);
                m_machineTimer.stop();
            }
         } else if (m_state == MachineState::Flushing && m_percentFilledTank2 > targetValue) {
             setPercentFillTank2(m_percentFilledTank2 - 1);
             if (m_percentFilledTank2 <= targetValue) {
                 setTank2ValveState(false);
                 setState(MachineState::Idle);
                 m_machineTimer.stop();
             }
         }
     });
}

QT_END_NAMESPACE

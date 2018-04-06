/****************************************************************************
**
** Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the QtOpcUa module.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
    UA_ServerConfig_delete(m_config);
}

bool DemoServer::init()
{
    m_config = UA_ServerConfig_new_minimal(43344, NULL);
    if (!m_config)
        return false;

    m_server = UA_Server_new(m_config);

    if (!m_server)
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

UA_NodeId DemoServer::addFolder(const QString &parent, const QString &nodeString, const QString &displayName, const QString &description)
{
    UA_NodeId resultNode;
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;

    oAttr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", displayName.toUtf8().constData());
    if (description.size())
        oAttr.description = UA_LOCALIZEDTEXT_ALLOC("en_US", description.toUtf8().constData());

    UA_StatusCode result;
    UA_NodeId requestedNodeId = Open62541Utils::nodeIdFromQString(nodeString);

    result = UA_Server_addObjectNode(m_server,
                                     requestedNodeId,
                                     Open62541Utils::nodeIdFromQString(parent),
                                     UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                     UA_QUALIFIEDNAME_ALLOC(requestedNodeId.namespaceIndex, nodeString.toUtf8().constData()),
                                     UA_NODEID_NULL,
                                     oAttr,
                                     NULL,
                                     &resultNode);
    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add folder:" << nodeString << " :" << result;
        return UA_NODEID_NULL;
    }
    return resultNode;
}

UA_NodeId DemoServer::addVariable(const UA_NodeId &folder, const QString &variableNode, const QString &name, const QVariant &value, QOpcUa::Types type)
{
    UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.value = QOpen62541ValueConverter::toOpen62541Variant(value, type);
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", name.toUtf8().constData());
    attr.dataType = attr.value.type ? attr.value.type->typeId : UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_QualifiedName variableName;
    variableName.namespaceIndex = variableNodeId.namespaceIndex;
    variableName.name = attr.displayName.text;

    UA_NodeId resultId;
    UA_StatusCode result = UA_Server_addVariableNode(m_server,
                                                     variableNodeId,
                                                     folder,
                                                     UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                                     variableName,
                                                     UA_NODEID_NULL,
                                                     attr,
                                                     NULL,
                                                     &resultId);

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

UA_NodeId DemoServer::addMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description, const QString &name, UA_MethodCallback cb)
{
    UA_NodeId methodNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    UA_MethodAttributes attr = UA_MethodAttributes_default;

    attr.description = UA_LOCALIZEDTEXT_ALLOC("en_US", description.toUtf8().constData());
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", name.toUtf8().constData());
    attr.executable = true;
    UA_QualifiedName methodBrowseName = UA_QUALIFIEDNAME_ALLOC(methodNodeId.namespaceIndex, name.toUtf8().constData());

    UA_NodeId resultId;
    UA_StatusCode result = UA_Server_addMethodNode(m_server, methodNodeId, folder,
                                                     UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                                     methodBrowseName,
                                                     attr, cb,
                                                     0, nullptr,
                                                     0, nullptr,
                                                     this, &resultId);
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
         qWarning() << "Unexpected namespace index for Demo namespace";
     }

     const UA_NodeId machineFolder = addFolder("ns=0;i=85", "ns=2;s=Machine", "Machine");
     const UA_NodeId tank1Folder = addFolder("ns=2;s=Machine", "ns=2;s=Machine.Tank1", "Machine.Tank1");
     const UA_NodeId tank2Folder = addFolder("ns=2;s=Machine", "ns=2;s=Machine.Tank2", "Machine.Tank2");

     m_percentFilledTank1Node = addVariable(tank1Folder, "ns=2;s=Machine.Tank1.PercentFilled", "Machine.Tank1.PercentFilled", 100.0, QOpcUa::Types::Double);
     m_percentFilledTank2Node = addVariable(tank2Folder, "ns=2;s=Machine.Tank2.PercentFilled", "Machine.Tank2.PercentFilled", 0.0, QOpcUa::Types::Double);
     m_tank2TargetPercentNode = addVariable(tank2Folder, "ns=2;s=Machine.Tank2.TargetPercent", "Machine.Tank2.TargetPercent", 0.0, QOpcUa::Types::Double);
     m_tank2ValveStateNode = addVariable(tank2Folder, "ns=2;s=Machine.Tank2.ValveState", "Machine.Tank2.ValveState", false, QOpcUa::Types::Boolean);
     m_machineStateNode = addVariable(machineFolder, "ns=2;s=Machine.State", "Machine.State", static_cast<quint32>(MachineState::Idle), QOpcUa::Types::UInt32);
     addVariable(machineFolder, "ns=2;s=Machine.Designation", "Machine.Designation", "TankExample", QOpcUa::Types::String);

     addMethod(machineFolder, "ns=2;s=Machine.Start", "Starts the pump", "Machine.Start", &startPumpMethod);
     addMethod(machineFolder, "ns=2;s=Machine.Stop", "Stops the pump", "Machine.Stop", &stopPumpMethod);
     addMethod(machineFolder, "ns=2;s=Machine.FlushTank2", "Flushes tank 2", "Machine.FlushTank2", &flushTank2Method);
     addMethod(machineFolder, "ns=2;s=Machine.Reset", "Resets the simulation", "Machine.Reset", &resetMethod);

     QObject::connect(&m_machineTimer, &QTimer::timeout, [this]() {

         double targetValue = readTank2TargetValue();
         if (m_state == MachineState::Pumping && m_percentFilledTank1 > 0 && m_percentFilledTank2 < targetValue) {
            setPercentFillTank1(m_percentFilledTank1 - 1);
            setPercentFillTank2(m_percentFilledTank2 + 1);
            if (m_percentFilledTank1 == 0 || m_percentFilledTank2 >= targetValue) {
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

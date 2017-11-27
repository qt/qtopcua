/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
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

#include "testserver.h"
#include "qopen62541utils.h"
#include <QtOpcUa/qopcuatype.h>

#include <QtCore/QDebug>
#include <QtCore/QLoggingCategory>
#include <QtCore/QUuid>

#include <cstring>

// Node ID conversion is included from the open62541 plugin but warnings from there should be logged
// using qt.opcua.testserver instead of qt.opcua.plugins.open62541 for usage in the test server
Q_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_OPEN62541, "qt.opcua.testserver")

TestServer::TestServer(QObject *parent) : QObject(parent)
{
    m_timer.setInterval(0);
    m_timer.setSingleShot(false);
    connect(&m_timer, &QTimer::timeout, this, &TestServer::processServerEvents);
}

TestServer::~TestServer()
{
    shutdown();
    UA_Server_delete(m_server);
    UA_ServerConfig_delete(m_config);
}

bool TestServer::init()
{
    m_config = UA_ServerConfig_new_minimal(43344, NULL);
    if (!m_config)
        return false;

    m_server = UA_Server_new(m_config);

    if (!m_server)
        return false;

    return true;
}

void TestServer::launch()
{
    UA_StatusCode s = UA_Server_run_startup(m_server);
    if (s != UA_STATUSCODE_GOOD)
         qFatal("Could not launch server");
     m_running = true;
     m_timer.start();
}

void TestServer::processServerEvents()
{
    if (m_running)
        UA_Server_run_iterate(m_server, true);
}

void TestServer::shutdown()
{
    if (m_running) {
        UA_Server_run_shutdown(m_server);
        m_running = false;
    }
}

int TestServer::registerNamespace(const QString &ns)
{
    return UA_Server_addNamespace(m_server, ns.toUtf8().constData());
}

UA_NodeId TestServer::addFolder(const QString &nodeString, const QString &displayName, const QString &description)
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
                                     UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
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

UA_NodeId TestServer::addObject(const UA_NodeId &parentFolder, int namespaceIndex, const QString &objectName)
{
    UA_NodeId resultNode;
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;

    if (objectName.size())
        oAttr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", objectName.toUtf8().constData());

    UA_StatusCode result;
    result = UA_Server_addObjectNode(m_server, UA_NODEID_NULL,
                                     parentFolder,
                                     UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                     UA_QUALIFIEDNAME_ALLOC(namespaceIndex, objectName.toUtf8().constData()),
                                     UA_NODEID_NULL,
                                     oAttr,
                                     NULL,
                                     &resultNode);
    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add object to folder:" << result;
        return UA_NODEID_NULL;
    }
    return resultNode;
}

template <typename UA_TYPE_VALUE, typename QTYPE, int UA_TYPE_IDENTIFIER>
UA_NodeId TestServer::addVariable(const UA_NodeId &folder, const QString &variableNode,
                                  const QString &description, QTYPE value)
{
    UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    UA_VariableAttributes attr = UA_VariableAttributes_default;
    UA_TYPE_VALUE uaValue = static_cast<UA_TYPE_VALUE>(value);
    UA_Variant_setScalarCopy(&attr.value, &uaValue, &UA_TYPES[UA_TYPE_IDENTIFIER]);
    attr.description = UA_LOCALIZEDTEXT_ALLOC("en_US", description.toUtf8().constData());
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", variableNode.toUtf8().constData());
    attr.dataType = UA_TYPES[UA_TYPE_IDENTIFIER].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_QualifiedName variableName;
    variableName.namespaceIndex = variableNodeId.namespaceIndex;
    UA_String_copy(&variableNodeId.identifier.string, &variableName.name);

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

template <>
UA_NodeId TestServer::addVariable<UA_String, QString, UA_TYPES_STRING>(const UA_NodeId &folder, const QString &variableNode,
                                  const QString &description, QString value)
{
    UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    UA_VariableAttributes attr = UA_VariableAttributes_default;
    UA_String uaValue = UA_String_fromChars(value.toUtf8().constData());
    UA_Variant_setScalar(&attr.value, &uaValue, &UA_TYPES[UA_TYPES_STRING]);
    attr.description = UA_LOCALIZEDTEXT_ALLOC("en_US", description.toUtf8().constData());
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", variableNode.toUtf8().constData());
    attr.dataType = UA_TYPES[UA_TYPES_STRING].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_QualifiedName variableName = UA_QUALIFIEDNAME_ALLOC(variableNodeId.namespaceIndex, variableNode.toUtf8().constData());

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

template <>
UA_NodeId TestServer::addVariable<UA_LocalizedText, QString, UA_TYPES_LOCALIZEDTEXT>(const UA_NodeId &folder, const QString &variableNode,
                                  const QString &description, QString value)
{
    UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    UA_VariableAttributes attr = UA_VariableAttributes_default;
    UA_LocalizedText uaValue;
    UA_String_init(&uaValue.locale);
    uaValue.text = UA_String_fromChars(value.toUtf8().constData());
    UA_Variant_setScalar(&attr.value, &uaValue, &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);
    attr.description = UA_LOCALIZEDTEXT_ALLOC("en_US", description.toUtf8().constData());
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", variableNode.toUtf8().constData());
    attr.dataType = UA_TYPES[UA_TYPES_LOCALIZEDTEXT].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_QualifiedName variableName = UA_QUALIFIEDNAME_ALLOC(variableNodeId.namespaceIndex, variableNode.toUtf8().constData());

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

template <>
UA_NodeId TestServer::addVariable<UA_ByteString, QByteArray, UA_TYPES_BYTESTRING>(const UA_NodeId &folder, const QString &variableNode,
                                  const QString &description, QByteArray value)
{
    UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    UA_VariableAttributes attr = UA_VariableAttributes_default;
    UA_ByteString uaValue = UA_BYTESTRING(value.data());
    UA_Variant_setScalar(&attr.value, &uaValue, &UA_TYPES[UA_TYPES_BYTESTRING]);
    attr.description = UA_LOCALIZEDTEXT_ALLOC("en_US", description.toUtf8().constData());
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", variableNode.toUtf8().constData());
    attr.dataType = UA_TYPES[UA_TYPES_BYTESTRING].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_QualifiedName variableName = UA_QUALIFIEDNAME_ALLOC(variableNodeId.namespaceIndex, variableNode.toUtf8().constData());

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

template <>
UA_NodeId TestServer::addVariable<UA_DateTime, QDateTime, UA_TYPES_DATETIME>(const UA_NodeId &folder, const QString &variableNode,
                                  const QString &description, QDateTime value)
{
    UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    UA_VariableAttributes attr = UA_VariableAttributes_default;
    UA_DateTime uaValue = UA_MSEC_TO_DATETIME * value.toMSecsSinceEpoch();
    UA_Variant_setScalar(&attr.value, &uaValue, &UA_TYPES[UA_TYPES_DATETIME]);
    attr.description = UA_LOCALIZEDTEXT_ALLOC("en_US", description.toUtf8().constData());
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", variableNode.toUtf8().constData());
    attr.dataType = UA_TYPES[UA_TYPES_DATETIME].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_QualifiedName variableName = UA_QUALIFIEDNAME_ALLOC(variableNodeId.namespaceIndex, variableNode.toUtf8().constData());

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

template <>
UA_NodeId TestServer::addVariable<UA_NodeId, QString, UA_TYPES_NODEID>(const UA_NodeId &folder, const QString &variableNode,
                                  const QString &description, QString value)
{
    UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    UA_VariableAttributes attr = UA_VariableAttributes_default;
    UA_NodeId uaValue = value.size() ? Open62541Utils::nodeIdFromQString(value) : UA_NODEID_NULL;
    UA_Variant_setScalar(&attr.value, &uaValue, &UA_TYPES[UA_TYPES_NODEID]);
    attr.description = UA_LOCALIZEDTEXT_ALLOC("en_US", description.toUtf8().constData());
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", variableNode.toUtf8().constData());
    attr.dataType = UA_TYPES[UA_TYPES_NODEID].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_QualifiedName variableName = UA_QUALIFIEDNAME_ALLOC(variableNodeId.namespaceIndex, variableNode.toUtf8().constData());

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

template <>
UA_NodeId TestServer::addVariable<UA_Guid, QUuid, UA_TYPES_GUID>(const UA_NodeId &folder, const QString &variableNode,
                                  const QString &description, QUuid value)
{
    UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    UA_VariableAttributes attr = UA_VariableAttributes_default;
    UA_Guid uaValue;
    uaValue.data1 = value.data1;
    uaValue.data2 = value.data2;
    uaValue.data3 = value.data3;
    std::memcpy(uaValue.data4, value.data4, sizeof(value.data4));
    UA_Variant_setScalar(&attr.value, &uaValue, &UA_TYPES[UA_TYPES_GUID]);
    attr.description = UA_LOCALIZEDTEXT_ALLOC("en_US", description.toLocal8Bit().constData());
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", variableNode.toLocal8Bit().constData());
    attr.dataType = UA_TYPES[UA_TYPES_GUID].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_QualifiedName variableName = UA_QUALIFIEDNAME_ALLOC(variableNodeId.namespaceIndex, variableNode.toLocal8Bit().constData());

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

template <>
UA_NodeId TestServer::addVariable<UA_String, QString, UA_TYPES_XMLELEMENT>(const UA_NodeId &folder, const QString &variableNode,
                                  const QString &description, QString value)
{
    UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    UA_VariableAttributes attr = UA_VariableAttributes_default;
    UA_String uaValue = UA_String_fromChars(value.toUtf8().constData());
    UA_Variant_setScalar(&attr.value, &uaValue, &UA_TYPES[UA_TYPES_XMLELEMENT]);
    attr.description = UA_LOCALIZEDTEXT_ALLOC("en_US", description.toUtf8().constData());
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", variableNode.toUtf8().constData());
    attr.dataType = UA_TYPES[UA_TYPES_XMLELEMENT].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_QualifiedName variableName = UA_QUALIFIEDNAME_ALLOC(variableNodeId.namespaceIndex, variableNode.toUtf8().constData());

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

template <>
UA_NodeId TestServer::addVariable<UA_QualifiedName, QPair<quint16, QString>, UA_TYPES_QUALIFIEDNAME>(const UA_NodeId &folder, const QString &variableNode,
                                const QString &description, QPair<quint16, QString> value)
{
  UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(variableNode);

  UA_VariableAttributes attr = UA_VariableAttributes_default;
  UA_QualifiedName uaValue;
  uaValue.name = UA_String_fromChars(value.second.toUtf8().constData());
  UA_Variant_setScalar(&attr.value, &uaValue, &UA_TYPES[UA_TYPES_QUALIFIEDNAME]);
  attr.description = UA_LOCALIZEDTEXT_ALLOC("en_US", description.toUtf8().constData());
  attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", variableNode.toUtf8().constData());
  attr.dataType = UA_TYPES[UA_TYPES_QUALIFIEDNAME].typeId;
  attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

  UA_QualifiedName variableName = UA_QUALIFIEDNAME_ALLOC(variableNodeId.namespaceIndex, variableNode.toUtf8().constData());

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

template <>
UA_NodeId TestServer::addVariable<UA_StatusCode, UA_StatusCode, UA_TYPES_STATUSCODE>(const UA_NodeId &folder, const QString &variableNode,
                                                                                     const QString &description, uint32_t value)
{
    UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    UA_VariableAttributes attr = UA_VariableAttributes_default;
    UA_StatusCode uaValue = value;
    UA_Variant_setScalar(&attr.value, &uaValue, &UA_TYPES[UA_TYPES_STATUSCODE]);
    attr.description = UA_LOCALIZEDTEXT_ALLOC("en_US", description.toLocal8Bit().constData());
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", variableNode.toLocal8Bit().constData());
    attr.dataType = UA_TYPES[UA_TYPES_STATUSCODE].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_QualifiedName variableName = UA_QUALIFIEDNAME_ALLOC(variableNodeId.namespaceIndex, variableNode.toLocal8Bit().constData());

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

UA_StatusCode TestServer::multiplyMethod(UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle, const UA_NodeId *methodId, void *methodContext, const UA_NodeId *objectId, void *objectContext, size_t inputSize, const UA_Variant *input, size_t outputSize, UA_Variant *output)
{
    Q_UNUSED(server);
    Q_UNUSED(sessionId);
    Q_UNUSED(sessionHandle);
    Q_UNUSED(methodId);
    Q_UNUSED(methodContext);
    Q_UNUSED(objectId);
    Q_UNUSED(objectContext);

    if (inputSize < 2)
        return QOpcUa::UaStatusCode::BadArgumentsMissing;
    if (inputSize > 2)
        return QOpcUa::UaStatusCode::BadTooManyArguments;
    if (outputSize != 1)
        return QOpcUa::UaStatusCode::BadInvalidArgument;

    double arg1 = *static_cast<double *>(input[0].data);
    double arg2 = *static_cast<double *>(input[1].data);

    double temp = arg1 * arg2;
    UA_Variant_setScalarCopy(output, &temp, &UA_TYPES[UA_TYPES_DOUBLE]);

    return UA_STATUSCODE_GOOD;
}

UA_NodeId TestServer::addMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description)
{
    UA_NodeId methodNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    UA_Argument inputArguments[2];
    UA_Argument_init(&inputArguments[0]);
    UA_Argument_init(&inputArguments[1]);

    inputArguments[0].description = UA_LOCALIZEDTEXT_ALLOC("en", "First value");
    inputArguments[0].name = UA_STRING_ALLOC("The first double");
    inputArguments[0].dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;
    inputArguments[0].valueRank = -1;

    inputArguments[1].description = UA_LOCALIZEDTEXT_ALLOC("en", "Second value");
    inputArguments[1].name = UA_STRING_ALLOC("The second double");
    inputArguments[1].dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;
    inputArguments[1].valueRank = -1;

    UA_Argument outputArgument;
    UA_Argument_init(&outputArgument);

    outputArgument.description = UA_LOCALIZEDTEXT_ALLOC("en", "The product of the two arguments");
    outputArgument.name = UA_STRING_ALLOC("The product of the two arguments");
    outputArgument.dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;
    outputArgument.valueRank = -1;

    UA_MethodAttributes attr = UA_MethodAttributes_default;

    attr.description = UA_LOCALIZEDTEXT_ALLOC("en_US", description.toUtf8().constData());
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", variableNode.toUtf8().constData());
    attr.executable = true;

    UA_NodeId resultId;
    UA_StatusCode result = UA_Server_addMethodNode(m_server, methodNodeId, folder,
                                                     UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                                     UA_QUALIFIEDNAME_ALLOC(methodNodeId.namespaceIndex, "multiplyArguments"),
                                                     attr, &multiplyMethod,
                                                     2, inputArguments,
                                                     1, &outputArgument,
                                                     NULL, &resultId);
    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add variable:" << result;
        return UA_NODEID_NULL;
    }
    return resultId;
}

template UA_NodeId TestServer::addVariable<UA_Double, double, UA_TYPES_DOUBLE>(const UA_NodeId &, const QString &, const QString &, double);
template UA_NodeId TestServer::addVariable<UA_Boolean, bool, UA_TYPES_BOOLEAN>(const UA_NodeId &, const QString &, const QString &, bool);
template UA_NodeId TestServer::addVariable<UA_Byte, uchar, UA_TYPES_BYTE>(const UA_NodeId &, const QString &, const QString &, uchar);
template UA_NodeId TestServer::addVariable<UA_SByte, char, UA_TYPES_SBYTE>(const UA_NodeId &, const QString &, const QString &, char);
template UA_NodeId TestServer::addVariable<UA_Float, float, UA_TYPES_FLOAT>(const UA_NodeId &, const QString &, const QString &, float);
template UA_NodeId TestServer::addVariable<UA_Int16, qint16, UA_TYPES_INT16>(const UA_NodeId &, const QString &, const QString &, qint16);
template UA_NodeId TestServer::addVariable<UA_Int32, qint32, UA_TYPES_INT32>(const UA_NodeId &, const QString &, const QString &, qint32);
template UA_NodeId TestServer::addVariable<UA_Int64, qint64, UA_TYPES_INT64>(const UA_NodeId &, const QString &, const QString &, qint64);
template UA_NodeId TestServer::addVariable<UA_UInt16, quint16, UA_TYPES_UINT16>(const UA_NodeId &, const QString &, const QString &, quint16);
template UA_NodeId TestServer::addVariable<UA_UInt32, quint32, UA_TYPES_UINT32>(const UA_NodeId &, const QString &, const QString &, quint32);
template UA_NodeId TestServer::addVariable<UA_UInt64, quint64, UA_TYPES_UINT64>(const UA_NodeId &, const QString &, const QString &, quint64);


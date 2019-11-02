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
#include "qopen62541valueconverter.h"
#include <QtOpcUa/qopcuatype.h>

#include <QtCore/QDebug>
#include <QtCore/QLoggingCategory>
#include <QDir>
#include <QFile>

#include <cstring>

QT_BEGIN_NAMESPACE

#if defined UA_ENABLE_ENCRYPTION
static const size_t usernamePasswordsSize = 2;
static UA_UsernamePasswordLogin usernamePasswords[2] = {
    {UA_STRING_STATIC("user1"), UA_STRING_STATIC("password")},
    {UA_STRING_STATIC("user2"), UA_STRING_STATIC("password1")}};
#endif

const UA_UInt16 portNumber = 43344;

// Node ID conversion is included from the open62541 plugin but warnings from there should be logged
// using qt.opcua.testserver instead of qt.opcua.plugins.open62541 for usage in the test server
Q_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_OPEN62541, "qt.opcua.testserver")


TestServer::TestServer(QObject *parent) : QObject(parent)
{
    m_timer.setInterval(30);
    m_timer.setSingleShot(false);
    connect(&m_timer, &QTimer::timeout, this, &TestServer::processServerEvents);
}

TestServer::~TestServer()
{
    shutdown();
    UA_Server_delete(m_server);
}

bool TestServer::createInsecureServerConfig(UA_ServerConfig *config)
{
    UA_StatusCode result = UA_ServerConfig_setMinimal(config, portNumber, nullptr);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Failed to create server config without encryption";
        return false;
    }

    // This is needed for COIN because the hostname returned by gethostname() is not resolvable.
    config->customHostname = UA_String_fromChars("localhost");

    return true;
}

#if defined UA_ENABLE_ENCRYPTION
static UA_ByteString loadFile(const QString &filePath) {
    UA_ByteString fileContents = UA_STRING_NULL;
    fileContents.length = 0;

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly))
        return fileContents;

    fileContents.length = file.size();
    fileContents.data = (UA_Byte *)UA_malloc(fileContents.length * sizeof(UA_Byte));
    if (!fileContents.data)
        return fileContents;

    if (file.read(reinterpret_cast<char*>(fileContents.data), fileContents.length) != static_cast<qint64>(fileContents.length)) {
        UA_ByteString_deleteMembers(&fileContents);
        fileContents.length = 0;
        return fileContents;
    }
    return fileContents;
}

bool TestServer::createSecureServerConfig(UA_ServerConfig *config)
{
    const QString certificateFilePath = QLatin1String(":/pki/own/certs/open62541-testserver.der");
    const QString privateKeyFilePath = QLatin1String(":/pki/own/private/open62541-testserver.der");

    UA_ByteString certificate = loadFile(certificateFilePath);
    UaDeleter<UA_ByteString> certificateDeleter(&certificate, UA_ByteString_deleteMembers);
    UA_ByteString privateKey = loadFile(privateKeyFilePath);
    UaDeleter<UA_ByteString> privateKeyDeleter(&privateKey, UA_ByteString_deleteMembers);

    if (certificate.length == 0) {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
           "Failed to load certificate %s", certificateFilePath.toLocal8Bit().constData());
        return false;
    }
    if (privateKey.length == 0) {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
           "Failed to load private key %s", privateKeyFilePath.toLocal8Bit().constData());
        return false;
    }

    // Load the trustlist
    QDir trustDir(":/pki/trusted/certs");
    if (!trustDir.exists()) {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Trust directory does not exist");
        return false;
    }

    const auto trustedCerts = trustDir.entryList(QDir::Files);
    const size_t trustListSize = trustedCerts.size();
    int i = 0;

    UA_STACKARRAY(UA_ByteString, trustList, trustListSize);
    UaArrayDeleter<UA_TYPES_BYTESTRING> trustListDeleter(&trustList, trustListSize);

    for (const auto &entry : trustedCerts) {
        trustList[i] = loadFile(trustDir.filePath(entry));
        if (trustList[i].length == 0) {
            UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Failed to load trusted certificate");
            return false;
        } else {
            UA_LOG_DEBUG(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Trusted certificate %s loaded", entry.toLocal8Bit().constData());
        }
        ++i;
    }

    // Loading of a revocation list currently unsupported
    UA_ByteString *revocationList = nullptr;
    size_t revocationListSize = 0;

    if (trustListSize == 0)
       UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
          "No CA trust-list provided. Any remote certificate will be accepted.");

    UA_ServerConfig_setBasics(config);

    // This is needed for COIN because the hostname returned by gethostname() is not resolvable.
    config->customHostname = UA_String_fromChars("localhost");

    UA_StatusCode result = UA_CertificateVerification_Trustlist(&config->certificateVerification,
                                                  trustList, trustListSize,
                                                  nullptr, 0,
                                                  revocationList, revocationListSize);
    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Failed to initialize certificate verification";
        return false;
    }

    // Do not delete items on success.
    // They will be used by the server.
    trustListDeleter.release();

    result = UA_ServerConfig_addNetworkLayerTCP(config, portNumber, 0, 0);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Failed to add network layer";
        return false;
    }

    result = UA_ServerConfig_addAllSecurityPolicies(config, &certificate, &privateKey);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Failed to add security policies";
        return false;
    }

    // Do not delete items on success.
    // They will be used by the server.
    certificateDeleter.release();
    privateKeyDeleter.release();

    result = UA_AccessControl_default(config, true,
                &config->securityPolicies[0].policyUri,
                usernamePasswordsSize, usernamePasswords);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Failed to create access control";
        return false;
    }

    result = UA_ServerConfig_addAllEndpoints(config);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Failed to add endpoints";
        return false;
    }

    return true;
}
#endif

bool TestServer::init()
{
    bool success;

    m_server = UA_Server_new();

    if (!m_server)
        return false;

    // This member is managed by the server
    m_config = UA_Server_getConfig(m_server);

#if defined UA_ENABLE_ENCRYPTION
    success = createSecureServerConfig(m_config);
#else
    success = createInsecureServerConfig(m_config);
#endif

    if (!success || !m_config)
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

    oAttr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", displayName.toUtf8().constData());
    if (description.size())
        oAttr.description = UA_LOCALIZEDTEXT_ALLOC("en-US", description.toUtf8().constData());

    UA_StatusCode result;
    UA_NodeId requestedNodeId = Open62541Utils::nodeIdFromQString(nodeString);

    UA_QualifiedName nodeBrowseName = UA_QUALIFIEDNAME_ALLOC(requestedNodeId.namespaceIndex, displayName.toUtf8().constData());

    result = UA_Server_addObjectNode(m_server,
                                     requestedNodeId,
                                     UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                                     UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                     nodeBrowseName,
                                     UA_NODEID_NULL,
                                     oAttr,
                                     nullptr,
                                     &resultNode);

    UA_QualifiedName_deleteMembers(&nodeBrowseName);
    UA_NodeId_deleteMembers(&requestedNodeId);
    UA_ObjectAttributes_deleteMembers(&oAttr);

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
        oAttr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", objectName.toUtf8().constData());

    UA_QualifiedName nodeBrowseName = UA_QUALIFIEDNAME_ALLOC(namespaceIndex, objectName.toUtf8().constData());

    UA_StatusCode result;
    result = UA_Server_addObjectNode(m_server, UA_NODEID_NULL,
                                     parentFolder,
                                     UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                     nodeBrowseName,
                                     UA_NODEID_NULL,
                                     oAttr,
                                     nullptr,
                                     &resultNode);

    UA_QualifiedName_deleteMembers(&nodeBrowseName);
    UA_ObjectAttributes_deleteMembers(&oAttr);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add object to folder:" << result;
        return UA_NODEID_NULL;
    }
    return resultNode;
}

UA_NodeId TestServer::addVariableWithWriteMask(const UA_NodeId &folder, const QString &variableNode, const QString &name, const QVariant &value,
                                  QOpcUa::Types type, quint32 writeMask)
{
    UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    const QString description = QStringLiteral("Description for %1").arg(variableNode);

    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.value = QOpen62541ValueConverter::toOpen62541Variant(value, type);
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", name.toUtf8().constData());
    attr.dataType = attr.value.type ? attr.value.type->typeId : UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    attr.description = UA_LOCALIZEDTEXT_ALLOC("en-US", description.toUtf8().constData());
    attr.writeMask = writeMask;

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
                                                     nullptr,
                                                     &resultId);


    UA_NodeId_deleteMembers(&variableNodeId);
    UA_VariableAttributes_deleteMembers(&attr);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add variable:" << result;
        return UA_NODEID_NULL;
    }

    return resultId;
}

UA_NodeId TestServer::addVariable(const UA_NodeId &folder, const QString &variableNode, const QString &name, const QVariant &value,
                                  QOpcUa::Types type, QVector<quint32> arrayDimensions, int valueRank)
{
    UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    const QString description = QStringLiteral("Description for %1").arg(variableNode);

    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.value = QOpen62541ValueConverter::toOpen62541Variant(value, type);
    attr.valueRank = valueRank;
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", name.toUtf8().constData());
    attr.dataType = attr.value.type ? attr.value.type->typeId : UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    attr.description = UA_LOCALIZEDTEXT_ALLOC("en-US", description.toUtf8().constData());

    if (arrayDimensions.size()) {
        attr.arrayDimensionsSize = arrayDimensions.size();
        attr.arrayDimensions = arrayDimensions.data();
    }

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
                                                     nullptr,
                                                     &resultId);

    // Prevent deletion of the QVector's value by UA_VariableAttribute_deleteMembers
    attr.arrayDimensions = nullptr;
    attr.arrayDimensionsSize = 0;

    UA_NodeId_deleteMembers(&variableNodeId);
    UA_VariableAttributes_deleteMembers(&attr);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add variable:" << result << "for node" << variableNode;
        return UA_NODEID_NULL;
    }

    return resultId;
}

UA_NodeId TestServer::addEmptyArrayVariable(const UA_NodeId &folder, const QString &variableNode, const QString &name)
{
    UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", name.toUtf8().constData());
    attr.dataType = UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ;
    attr.valueRank = 1;
    attr.arrayDimensionsSize = 1;
    attr.arrayDimensions = UA_UInt32_new();
    *attr.arrayDimensions = 1;
    attr.value.arrayLength = 0;
    attr.value.type = &UA_TYPES[UA_TYPES_BOOLEAN];
    attr.value.data = UA_EMPTY_ARRAY_SENTINEL;
    attr.value.arrayDimensionsSize = 1;
    attr.value.arrayDimensions = UA_UInt32_new();
    *attr.value.arrayDimensions = 1;

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
                                                     nullptr,
                                                     &resultId);

    UA_NodeId_deleteMembers(&variableNodeId);
    UA_VariableAttributes_deleteMembers(&attr);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add empty array variable:" << result;
        return UA_NODEID_NULL;
    }

    return resultId;
}

UA_StatusCode TestServer::multiplyMethod(UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle, const UA_NodeId *methodId, void *methodContext, const UA_NodeId *objectId, void *objectContext, size_t inputSize, const UA_Variant *input, size_t outputSize, UA_Variant *output)
{
    Q_UNUSED(server)
    Q_UNUSED(sessionId)
    Q_UNUSED(sessionHandle)
    Q_UNUSED(methodId)
    Q_UNUSED(methodContext)
    Q_UNUSED(objectId)
    Q_UNUSED(objectContext)

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

UA_NodeId TestServer::addMultiplyMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description)
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

    attr.description = UA_LOCALIZEDTEXT_ALLOC("en-US", description.toUtf8().constData());
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", variableNode.toUtf8().constData());
    attr.executable = true;

    UA_QualifiedName nodeBrowseName = UA_QUALIFIEDNAME_ALLOC(methodNodeId.namespaceIndex, "multiplyArguments");

    UA_NodeId resultId;
    UA_StatusCode result = UA_Server_addMethodNode(m_server, methodNodeId, folder,
                                                     UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                                     nodeBrowseName,
                                                     attr, &multiplyMethod,
                                                     2, inputArguments,
                                                     1, &outputArgument,
                                                     nullptr, &resultId);

    UA_QualifiedName_deleteMembers(&nodeBrowseName);
    UA_NodeId_deleteMembers(&methodNodeId);
    UA_MethodAttributes_deleteMembers(&attr);
    UA_Argument_deleteMembers(&inputArguments[0]);
    UA_Argument_deleteMembers(&inputArguments[1]);
    UA_Argument_deleteMembers(&outputArgument);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add variable:" << result;
        return UA_NODEID_NULL;
    }
    return resultId;
}

UA_StatusCode TestServer::multipleOutputArgumentsMethod(UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle, const UA_NodeId *methodId, void *methodContext, const UA_NodeId *objectId, void *objectContext, size_t inputSize, const UA_Variant *input, size_t outputSize, UA_Variant *output)
{
    Q_UNUSED(server)
    Q_UNUSED(sessionId)
    Q_UNUSED(sessionHandle)
    Q_UNUSED(methodId)
    Q_UNUSED(methodContext)
    Q_UNUSED(objectId)
    Q_UNUSED(objectContext)

    if (inputSize < 2)
        return QOpcUa::UaStatusCode::BadArgumentsMissing;
    if (inputSize > 2)
        return QOpcUa::UaStatusCode::BadTooManyArguments;
    if (outputSize != 2)
        return QOpcUa::UaStatusCode::BadInvalidArgument;

    double arg1 = *static_cast<double *>(input[0].data);
    double arg2 = *static_cast<double *>(input[1].data);

    double product = arg1 * arg2;
    auto someText = UA_LOCALIZEDTEXT_ALLOC("en-US", "some text argument");
    UA_Variant_setScalarCopy(&output[0], &product, &UA_TYPES[UA_TYPES_DOUBLE]);
    UA_Variant_setScalarCopy(&output[1], &someText, &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);

    return UA_STATUSCODE_GOOD;
}

UA_NodeId TestServer::addMultipleOutputArgumentsMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description)
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

    UA_Argument outputArgument[2];
    UA_Argument_init(&outputArgument[0]);
    UA_Argument_init(&outputArgument[1]);

    outputArgument[0].description = UA_LOCALIZEDTEXT_ALLOC("en", "The product of the two arguments");
    outputArgument[0].name = UA_STRING_ALLOC("The product of the two arguments");
    outputArgument[0].dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;
    outputArgument[0].valueRank = -1;

    outputArgument[1].description = UA_LOCALIZEDTEXT_ALLOC("en", "Some additional text argument");
    outputArgument[1].name = UA_STRING_ALLOC("Text argument");
    outputArgument[1].dataType = UA_TYPES[UA_TYPES_LOCALIZEDTEXT].typeId;
    outputArgument[1].valueRank = -1;

    UA_MethodAttributes attr = UA_MethodAttributes_default;

    attr.description = UA_LOCALIZEDTEXT_ALLOC("en-US", description.toUtf8().constData());
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", variableNode.toUtf8().constData());
    attr.executable = true;

    UA_QualifiedName nodeBrowseName = UA_QUALIFIEDNAME_ALLOC(methodNodeId.namespaceIndex, "multipleOutputArguments");

    UA_NodeId resultId;
    UA_StatusCode result = UA_Server_addMethodNode(m_server, methodNodeId, folder,
                                                     UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                                     nodeBrowseName,
                                                     attr, &multipleOutputArgumentsMethod,
                                                     2, inputArguments,
                                                     2, outputArgument,
                                                     nullptr, &resultId);

    UA_QualifiedName_deleteMembers(&nodeBrowseName);
    UA_NodeId_deleteMembers(&methodNodeId);
    UA_MethodAttributes_deleteMembers(&attr);
    UA_Argument_deleteMembers(&inputArguments[0]);
    UA_Argument_deleteMembers(&inputArguments[1]);
    UA_Argument_deleteMembers(&outputArgument[0]);
    UA_Argument_deleteMembers(&outputArgument[1]);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add variable:" << result;
        return UA_NODEID_NULL;
    }
    return resultId;
}

UA_NodeId TestServer::addAddNamespaceMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description)
{
    UA_NodeId methodNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    UA_Argument inputArguments[1];
    UA_Argument_init(&inputArguments[0]);

    inputArguments[0].description = UA_LOCALIZEDTEXT_ALLOC("en", "Namespace name to be added");
    inputArguments[0].name = UA_STRING_ALLOC("Namespace name");
    inputArguments[0].dataType = UA_TYPES[UA_TYPES_STRING].typeId;
    inputArguments[0].valueRank = -1;

    UA_Argument outputArgument;
    UA_Argument_init(&outputArgument);

    outputArgument.description = UA_LOCALIZEDTEXT_ALLOC("en", "Index of the added namespace");
    outputArgument.name = UA_STRING_ALLOC("Namespace index");
    outputArgument.dataType = UA_TYPES[UA_TYPES_UINT16].typeId;
    outputArgument.valueRank = -1;

    UA_MethodAttributes attr = UA_MethodAttributes_default;

    attr.description = UA_LOCALIZEDTEXT_ALLOC("en-US", description.toUtf8().constData());
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", variableNode.toUtf8().constData());
    attr.executable = true;

    UA_QualifiedName nodeBrowseName = UA_QUALIFIEDNAME_ALLOC(methodNodeId.namespaceIndex, "addNamespace");

    UA_NodeId resultId;
    UA_StatusCode result = UA_Server_addMethodNode(m_server, methodNodeId, folder,
                                                     UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                                     nodeBrowseName,
                                                     attr, &addNamespaceMethod,
                                                     1, inputArguments,
                                                     1, &outputArgument,
                                                     nullptr, &resultId);

    UA_QualifiedName_deleteMembers(&nodeBrowseName);
    UA_NodeId_deleteMembers(&methodNodeId);
    UA_MethodAttributes_deleteMembers(&attr);
    UA_Argument_deleteMembers(&inputArguments[0]);
    UA_Argument_deleteMembers(&outputArgument);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add method:" << result;
        return UA_NODEID_NULL;
    }
    return resultId;
}

UA_StatusCode TestServer::addNamespaceMethod(UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle, const UA_NodeId *methodId, void *methodContext, const UA_NodeId *objectId, void *objectContext, size_t inputSize, const UA_Variant *input, size_t outputSize, UA_Variant *output)
{
    Q_UNUSED(sessionId)
    Q_UNUSED(sessionHandle)
    Q_UNUSED(methodId)
    Q_UNUSED(methodContext)
    Q_UNUSED(objectId)
    Q_UNUSED(objectContext)

    if (inputSize < 1)
        return QOpcUa::UaStatusCode::BadArgumentsMissing;
    if (inputSize > 1)
        return QOpcUa::UaStatusCode::BadTooManyArguments;
    if (outputSize != 1)
        return QOpcUa::UaStatusCode::BadInvalidArgument;

    if (input[0].type != &UA_TYPES[UA_TYPES_STRING])
        return UA_STATUSCODE_BADTYPEMISMATCH;

    QString uri = QOpen62541ValueConverter::scalarToQt<QString>(static_cast<UA_String *>(input[0].data));
    UA_UInt16 namespaceIndex = UA_Server_addNamespace(server, uri.toUtf8().constData());
    UA_Variant_setScalarCopy(output, &namespaceIndex, &UA_TYPES[UA_TYPES_UINT16]);
    return UA_STATUSCODE_GOOD;
}

UA_NodeId TestServer::addNodeWithFixedTimestamp(const UA_NodeId &folder, const QString &nodeId, const QString &displayName)
{
    UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(nodeId);

    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.value = QOpen62541ValueConverter::toOpen62541Variant(QDateTime(QDate(2012, 12, 19), QTime(13, 37)), QOpcUa::DateTime);
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", displayName.toUtf8().constData());
    attr.dataType = attr.value.type->typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ; // Read Only

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

QT_END_NAMESPACE

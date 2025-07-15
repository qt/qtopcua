// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "testserver.h"

#ifdef USE_SYSTEM_OPEN62541
#include <open62541/plugin/log_stdout.h>
#include <open62541/plugin/pki_default.h>
#endif

#include "generated/namespace_qtopcuatestmodel_generated.h"
#include "generated/types_qtopcuatestmodel_generated.h"
#include "generated/types_qtopcuatestmodel_generated_handling.h"
#include "generated/qtopcuatestmodel_nodeids.h"

#include "qopen62541utils.h"
#include "qopen62541valueconverter.h"
#include <QtOpcUa/qopcuatype.h>

#include <QtCore/QDebug>
#include <QtCore/QLoggingCategory>
#include <QDir>
#include <QFile>
#include <QMap>

#include <cstring>

QT_BEGIN_NAMESPACE

using namespace Qt::Literals::StringLiterals;

static const size_t usernamePasswordsSize = 2;
static UA_UsernamePasswordLogin usernamePasswords[2] = {
    {UA_STRING_STATIC("user1"), UA_STRING_STATIC("password")},
    {UA_STRING_STATIC("user2"), UA_STRING_STATIC("password1")}};

const UA_UInt16 portNumber = 43344;

// Node ID conversion is included from the open62541 plugin but warnings from there should be logged
// using qt.opcua.testserver instead of qt.opcua.plugins.open62541 for usage in the test server
Q_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_OPEN62541, "qt.opcua.testserver")


TestServer::TestServer(QObject *parent) : QObject(parent)
{
    m_historyDataBackend = UA_HistoryDataBackend_Memory(1, 10);
}

TestServer::~TestServer()
{
    m_historyDataBackend.deleteMembers(&m_historyDataBackend);
    UA_Server_delete(m_server);
}

bool TestServer::createInsecureServerConfig(UA_ServerConfig *config)
{
    UA_StatusCode result = UA_ServerConfig_setMinimal(config, portNumber, nullptr);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Failed to create server config without encryption";
        return false;
    }

    result = UA_AccessControl_default(config, true, nullptr, usernamePasswordsSize, usernamePasswords);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Failed to create access control";
        return false;
    }

    // This is needed for COIN because the hostname returned by gethostname() is not resolvable.
    UA_Array_delete(config->serverUrls, config->serverUrlsSize, &UA_TYPES[UA_TYPES_STRING]);
    config->serverUrls = UA_String_new();
    config->serverUrls[0] = UA_STRING_ALLOC("opc.tcp://localhost:43344");
    config->serverUrlsSize = 1;

    config->tcpReuseAddr = true;

    return true;
}

#if defined UA_ENABLE_ENCRYPTION
static UA_ByteString loadFile(const QString &filePath) {
    UA_ByteString fileContents = UA_STRING_NULL;
    fileContents.length = 0;

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly))
        return fileContents;

    const auto allocResult = UA_ByteString_allocBuffer(&fileContents, file.size());

    if (allocResult != UA_STATUSCODE_GOOD)
        return fileContents;

    if (file.read(reinterpret_cast<char*>(fileContents.data), fileContents.length) != static_cast<qint64>(fileContents.length)) {
        UA_ByteString_clear(&fileContents);
        fileContents.length = 0;
        return fileContents;
    }
    return fileContents;
}

bool TestServer::createSecureServerConfig(UA_ServerConfig *config)
{
    const QString certificateFilePath = u":/pki/own/certs/open62541-testserver.der"_s;
    const QString privateKeyFilePath = u":/pki/own/private/open62541-testserver.der"_s;

    UA_ByteString certificate = loadFile(certificateFilePath);
    UaDeleter<UA_ByteString> certificateDeleter(&certificate, UA_ByteString_clear);
    UA_ByteString privateKey = loadFile(privateKeyFilePath);
    UaDeleter<UA_ByteString> privateKeyDeleter(&privateKey, UA_ByteString_clear);

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
    QDir trustDir(u":/pki/trusted/certs"_s);
    if (!trustDir.exists()) {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Trust directory does not exist");
        return false;
    }

    const auto trustedCerts = trustDir.entryList(QDir::Files);
    const size_t trustListSize = trustedCerts.size();
    int i = 0;

    // UA_STACKARRAY(UA_ByteString, trustList, trustListSize);
    auto trustList = static_cast<UA_ByteString *>(UA_Array_new(trustListSize, &UA_TYPES[UA_TYPES_BYTESTRING]));
    UaArrayDeleter<UA_TYPES_BYTESTRING> trustListDeleter(trustList, trustListSize);

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

    UA_ServerConfig_setBasics_withPort(config, 43344);

    // This is needed for COIN because the hostname returned by gethostname() is not resolvable.
    UA_Array_delete(config->serverUrls, config->serverUrlsSize, &UA_TYPES[UA_TYPES_STRING]);
    config->serverUrls = UA_String_new();
    config->serverUrls[0] = UA_STRING_ALLOC("opc.tcp://localhost:43344");
    config->serverUrlsSize = 1;

    config->tcpReuseAddr = true;

    UA_StatusCode result = UA_CertificateVerification_Trustlist(&config->sessionPKI,
                                                                trustList, trustListSize,
                                                                nullptr, 0,
                                                                revocationList, revocationListSize);
    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Failed to initialize certificate verification";
        return false;
    }

    result = UA_CertificateVerification_Trustlist(&config->secureChannelPKI,
                                                  trustList, trustListSize,
                                                  nullptr, 0,
                                                  revocationList, revocationListSize);
    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Failed to initialize certificate verification";
        return false;
    }

    // Add the security policies manually because we need to skip Basic128Rsa15 and Basic256
    // if OpenSSL doesn't support SHA-1 signatures (e.g. RHEL 9).

    UA_StatusCode retval = UA_ServerConfig_addSecurityPolicyNone(config, &certificate);
    if(retval != UA_STATUSCODE_GOOD) {
        qWarning() << "Failed to add security policy None";
        return false;
    }

    if (Open62541Utils::checkSha1SignatureSupport()) {
        retval = UA_ServerConfig_addSecurityPolicyBasic128Rsa15(config, &certificate, &privateKey);
        if(retval != UA_STATUSCODE_GOOD) {
            qWarning() << "Failed to add security policy Basic128Rsa15";
            return false;
        }

        retval = UA_ServerConfig_addSecurityPolicyBasic256(config, &certificate, &privateKey);
        if(retval != UA_STATUSCODE_GOOD) {
            qWarning() << "Failed to add security policy Basic256";
            return false;
        }
    }

    retval = UA_ServerConfig_addSecurityPolicyAes256Sha256RsaPss(config, &certificate, &privateKey);
    if (retval != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add SecurityPolicy#Aes256Sha256RsaPss";
        return false;
    }

    retval = UA_ServerConfig_addSecurityPolicyBasic256Sha256(config, &certificate, &privateKey);
    if(retval != UA_STATUSCODE_GOOD) {
        qWarning() << "Failed to add security policy Basic256Sha256";
        return false;
    }

    retval = UA_ServerConfig_addSecurityPolicyAes128Sha256RsaOaep(config, &certificate, &privateKey);
    if(retval != UA_STATUSCODE_GOOD) {
        qWarning() << "Failed to add security policy Aes128Sha256RsaOaep";
        return false;
    }

    result = UA_AccessControl_default(config, true, nullptr, usernamePasswordsSize, usernamePasswords);

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

bool TestServer::init(bool noNonePolicyPassword)
{
    bool success;

    m_server = UA_Server_new();

    if (!m_server)
        return false;

    // This member is managed by the server
    m_config = UA_Server_getConfig(m_server);

#if defined UA_ENABLE_ENCRYPTION
    success = createSecureServerConfig(m_config);
    m_config->allowNonePolicyPassword = !noNonePolicyPassword;
#else
    success = createInsecureServerConfig(m_config);
#endif

    m_config->maxReferencesPerNode = 10;

    m_gathering = UA_HistoryDataGathering_Default(1);
    m_config->historyDatabase = UA_HistoryDatabase_default(m_gathering);

    if (!success || !m_config)
        return false;

    return true;
}

int TestServer::registerNamespace(const QString &ns)
{
    return UA_Server_addNamespace(m_server, ns.toUtf8().constData());
}

ManagedUaNodeId TestServer::addFolder(const QString &nodeString, const QString &displayName, const QString &description)
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

    UA_QualifiedName_clear(&nodeBrowseName);
    UA_NodeId_clear(&requestedNodeId);
    UA_ObjectAttributes_clear(&oAttr);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add folder:" << nodeString << " :" << result;
        return UA_NODEID_NULL;
    }
    return resultNode;
}

ManagedUaNodeId TestServer::addObject(const UA_NodeId &parentFolder, int namespaceIndex, const QString &objectName)
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

    UA_QualifiedName_clear(&nodeBrowseName);
    UA_ObjectAttributes_clear(&oAttr);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add object to folder:" << result;
        return UA_NODEID_NULL;
    }
    return resultNode;
}

ManagedUaNodeId TestServer::addVariableWithWriteMask(const UA_NodeId &folder, const QString &variableNode, const QString &name, const QVariant &value,
                                  QOpcUa::Types type, quint32 writeMask)
{
    UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    const QString description = u"Description for %1"_s.arg(variableNode);

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


    UA_NodeId_clear(&variableNodeId);
    UA_VariableAttributes_clear(&attr);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add variable:" << result;
        return UA_NODEID_NULL;
    }

    return resultId;
}

ManagedUaNodeId TestServer::addVariable(const UA_NodeId &folder, const QString &variableNode, const QString &name, const QVariant &value,
                                  QOpcUa::Types type, QList<quint32> arrayDimensions, int valueRank, bool enableHistorizing, quint32 historyNumValuesPerNode)
{
    UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    const QString description = u"Description for %1"_s.arg(variableNode);

    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.value = QOpen62541ValueConverter::toOpen62541Variant(value, type);
    attr.valueRank = valueRank;
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", name.toUtf8().constData());
    attr.dataType = attr.value.type ? attr.value.type->typeId : UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    if (enableHistorizing) {
        attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE | UA_ACCESSLEVELMASK_HISTORYREAD;
    } else {
        attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    }
    attr.description = UA_LOCALIZEDTEXT_ALLOC("en-US", description.toUtf8().constData());
    attr.historizing = enableHistorizing;

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

    // Prevent deletion of the QList's value by UA_VariableAttribute_clear
    attr.arrayDimensions = nullptr;
    attr.arrayDimensionsSize = 0;

    if (enableHistorizing) {
        UA_HistorizingNodeIdSettings setting;
        setting.historizingBackend = m_historyDataBackend;
        setting.maxHistoryDataResponseSize = historyNumValuesPerNode;
        setting.historizingUpdateStrategy = UA_HISTORIZINGUPDATESTRATEGY_VALUESET;
        result = m_gathering.registerNodeId(m_server, m_gathering.context, &resultId, setting);
        if (result != UA_STATUSCODE_GOOD) {
            qWarning() << "Could not register node for historical data:" << result << "for node" << variableNode;
            return UA_NODEID_NULL;
        }
    }

    UA_NodeId_clear(&variableNodeId);
    UA_VariableAttributes_clear(&attr);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add variable:" << result << "for node" << variableNode;
        return UA_NODEID_NULL;
    }

    return resultId;
}

ManagedUaNodeId TestServer::addEmptyArrayVariable(const UA_NodeId &folder, const QString &variableNode, const QString &name)
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

    UA_NodeId_clear(&variableNodeId);
    UA_VariableAttributes_clear(&attr);

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

ManagedUaNodeId TestServer::addMultiplyMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description)
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

    UA_QualifiedName_clear(&nodeBrowseName);
    UA_NodeId_clear(&methodNodeId);
    UA_MethodAttributes_clear(&attr);
    UA_Argument_clear(&inputArguments[0]);
    UA_Argument_clear(&inputArguments[1]);
    UA_Argument_clear(&outputArgument);

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

ManagedUaNodeId TestServer::addMultipleOutputArgumentsMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description)
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

    UA_QualifiedName_clear(&nodeBrowseName);
    UA_NodeId_clear(&methodNodeId);
    UA_MethodAttributes_clear(&attr);
    UA_Argument_clear(&inputArguments[0]);
    UA_Argument_clear(&inputArguments[1]);
    UA_Argument_clear(&outputArgument[0]);
    UA_Argument_clear(&outputArgument[1]);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add variable:" << result;
        return UA_NODEID_NULL;
    }
    return resultId;
}

ManagedUaNodeId TestServer::addAddNamespaceMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description)
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

    UA_QualifiedName_clear(&nodeBrowseName);
    UA_NodeId_clear(&methodNodeId);
    UA_MethodAttributes_clear(&attr);
    UA_Argument_clear(&inputArguments[0]);
    UA_Argument_clear(&outputArgument);

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

ManagedUaNodeId TestServer::addNodeWithFixedTimestamp(const UA_NodeId &folder, const QString &nodeId, const QString &displayName)
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

    UA_VariableAttributes_clear(&attr);
    UA_NodeId_clear(&variableNodeId);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add variable:" << result;
        return UA_NODEID_NULL;
    }

    return resultId;
}

UA_StatusCode TestServer::addServerStatusTypeTestNodes(const UA_NodeId &parent)
{
    const auto createTestValue = [](int index) -> UA_ServerStatusDataType {
        UA_ServerStatusDataType s;
        UA_ServerStatusDataType_init(&s);

        const UA_DateTime testDateTime = UA_DATETIME_UNIX_EPOCH + 1691996809123 * UA_DATETIME_MSEC;

        s.currentTime = testDateTime + index * UA_DATETIME_MSEC;
        s.startTime = testDateTime + index * UA_DATETIME_MSEC;
        s.secondsTillShutdown = 23;
        s.shutdownReason = UA_LOCALIZEDTEXT_ALLOC("en", "ShutdownReason");
        s.state = UA_SERVERSTATE_TEST;
        s.buildInfo.buildDate = testDateTime + index * UA_DATETIME_MSEC;
        s.buildInfo.buildNumber = UA_STRING_ALLOC("BuildNumber");
        s.buildInfo.manufacturerName = UA_STRING_ALLOC("ManufacturerName");
        s.buildInfo.productName = UA_STRING_ALLOC("ProductName");
        s.buildInfo.productUri = UA_STRING_ALLOC("ProductUri");
        s.buildInfo.softwareVersion = UA_STRING_ALLOC("SoftwareVersion");

        return s;
    };

    {
        UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(u"ns=3;s=ServerStatusScalar"_s);

        auto value = UA_ServerStatusDataType_new();
        *value = createTestValue(0);
        auto var = UA_Variant();
        UA_Variant_init(&var);
        UA_Variant_setScalar(&var, value, &UA_TYPES[UA_TYPES_SERVERSTATUSDATATYPE]);

        UA_VariableAttributes attr = UA_VariableAttributes_default;
        attr.value = var;
        attr.valueRank = UA_VALUERANK_SCALAR;
        attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", "ServerStatusScalar");
        attr.dataType = attr.value.type->typeId;
        attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

        UA_QualifiedName variableName;
        variableName.namespaceIndex = variableNodeId.namespaceIndex;
        variableName.name = UA_STRING_STATIC("ServerStatusScalar");

        UA_StatusCode result = UA_Server_addVariableNode(m_server,
                                                         variableNodeId,
                                                         parent,
                                                         UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                                         variableName,
                                                         UA_NODEID_NULL,
                                                         attr,
                                                         nullptr,
                                                         nullptr);

        UA_NodeId_clear(&variableNodeId);
        UA_VariableAttributes_clear(&attr);

        if (result != UA_STATUSCODE_GOOD) {
            qWarning() << "Could not add scalar server status variable";
            return result;
        }
    }

    {
        UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(u"ns=3;s=ServerStatusArray"_s);

        auto value = static_cast<UA_ServerStatusDataType *>(UA_Array_new(2, &UA_TYPES[UA_TYPES_SERVERSTATUSDATATYPE]));
        for (int i = 0; i < 2; ++i)
            value[i] = createTestValue(i);
        auto var = UA_Variant();
        UA_Variant_init(&var);
        UA_Variant_setArray(&var, value, 2, &UA_TYPES[UA_TYPES_SERVERSTATUSDATATYPE]);

        UA_VariableAttributes attr = UA_VariableAttributes_default;
        attr.value = var;
        attr.valueRank = UA_VALUERANK_ONE_DIMENSION;
        quint32 arrayDimensions[] = { 0 };
        attr.arrayDimensionsSize = 1;
        attr.arrayDimensions = arrayDimensions;
        attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", "ServerStatusArray");
        attr.dataType = attr.value.type->typeId;
        attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

        UA_QualifiedName variableName;
        variableName.namespaceIndex = variableNodeId.namespaceIndex;
        variableName.name = UA_STRING_STATIC("ServerStatusArray");

        UA_StatusCode result = UA_Server_addVariableNode(m_server,
                                                         variableNodeId,
                                                         parent,
                                                         UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                                         variableName,
                                                         UA_NODEID_NULL,
                                                         attr,
                                                         nullptr,
                                                         nullptr);

        attr.arrayDimensionsSize = 0;
        attr.arrayDimensions = nullptr;

        UA_NodeId_clear(&variableNodeId);
        UA_VariableAttributes_clear(&attr);

        if (result != UA_STATUSCODE_GOOD) {
            qWarning() << "Could not add array server status variable:" << UA_StatusCode_name(result);
            return result;
        }
    }

    {
        UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(u"ns=3;s=ServerStatusMultiDimensionalArray"_s);

        auto value = static_cast<UA_ServerStatusDataType *>(UA_Array_new(4, &UA_TYPES[UA_TYPES_SERVERSTATUSDATATYPE]));
        for (int i = 0; i < 4; ++i)
            value[i] = createTestValue(i);
        auto var = UA_Variant();
        UA_Variant_init(&var);
        UA_Variant_setArray(&var, value, 4, &UA_TYPES[UA_TYPES_SERVERSTATUSDATATYPE]);

        quint32 arrayDimensions[] = { 2, 2 };

        var.arrayDimensionsSize = 2;
        var.arrayDimensions = arrayDimensions;

        UA_VariableAttributes attr = UA_VariableAttributes_default;
        attr.value = var;
        attr.valueRank = UA_VALUERANK_TWO_DIMENSIONS;
        attr.arrayDimensionsSize = 2;
        attr.arrayDimensions = arrayDimensions;
        attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", "ServerStatusMultiDimensionalArray");
        attr.dataType = attr.value.type->typeId;
        attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

        UA_QualifiedName variableName;
        variableName.namespaceIndex = variableNodeId.namespaceIndex;
        variableName.name = UA_STRING_STATIC("ServerStatusMultiDimensionalArray");

        UA_StatusCode result = UA_Server_addVariableNode(m_server,
                                                         variableNodeId,
                                                         parent,
                                                         UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                                         variableName,
                                                         UA_NODEID_NULL,
                                                         attr,
                                                         nullptr,
                                                         nullptr);

        attr.arrayDimensionsSize = 0;
        attr.arrayDimensions = nullptr;
        attr.value.arrayDimensionsSize = 0;
        attr.value.arrayDimensions = nullptr;

        UA_NodeId_clear(&variableNodeId);
        UA_VariableAttributes_clear(&attr);

        if (result != UA_STATUSCODE_GOOD) {
            qWarning() << "Could not add multi dimensional array server status variable:" << UA_StatusCode_name(result);
            return result;
        }
    }

    return UA_STATUSCODE_GOOD;
}

UA_StatusCode TestServer::addLocalizedTextNodeWithCallback(const UA_NodeId &parent)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LocalizedText{UA_STRING_STATIC("en"), UA_STRING_STATIC("LocalizedTextWithCallback")};
    attr.dataType = UA_TYPES[UA_TYPES_LOCALIZEDTEXT].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ; // Read Only

    const UA_QualifiedName variableName{2, UA_STRING_STATIC("LocalizedTextWithCallback")};

    auto variableNodeId = UA_NODEID_STRING_ALLOC(2, "LocalizedTextWithCallback");

    const auto result = UA_Server_addDataSourceVariableNode(m_server, variableNodeId, parent,
                                                            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                                            variableName, UA_NODEID_NULL, attr,
                                                            { readLocalizedTextCallback, nullptr },
                                                            this, nullptr);

    UA_NodeId_clear(&variableNodeId);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add variable:" << result;
        return result;
    }

    return result;
}

// The event test methods are based on the open62541 tutorial_server_events.c example
UA_StatusCode TestServer::generateEventCallback(UA_Server *server,
                                                const UA_NodeId *sessionId, void *sessionHandle,
                                                const UA_NodeId *methodId, void *methodContext,
                                                const UA_NodeId *objectId, void *objectContext,
                                                size_t inputSize, const UA_Variant *input,
                                                size_t outputSize, UA_Variant *output) {
    Q_UNUSED(sessionId)
    Q_UNUSED(sessionHandle)
    Q_UNUSED(methodId)
    Q_UNUSED(methodContext)
    Q_UNUSED(objectId)
    Q_UNUSED(objectContext)
    Q_UNUSED(outputSize)
    Q_UNUSED(output)

    // Setup event
    UA_NodeId eventNodeId;

    UA_StatusCode ret = UA_Server_createEvent(server, UA_NODEID_NUMERIC(2, 12345), &eventNodeId);
    if (ret != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not create event:" << UA_StatusCode_name(ret);
        return ret;
    }

    quint16 eventSeverity = 100;

    if (inputSize && input[0].type == &UA_TYPES[UA_TYPES_UINT16] && input[0].data) {
        eventSeverity = *reinterpret_cast<quint16 *>(input[0].data);
    }

    qDebug() << "Creating event with severity" << eventSeverity;

    auto timePropertyName = UA_QUALIFIEDNAME_ALLOC(0, "Time");
    auto severityPropertyName = UA_QUALIFIEDNAME_ALLOC(0, "Severity");
    auto messagePropertyName = UA_QUALIFIEDNAME_ALLOC(0, "Message");
    auto sourceNamePropertyName = UA_QUALIFIEDNAME_ALLOC(0, "SourceName");
    auto eventMessage = UA_LOCALIZEDTEXT_ALLOC("en", "An event has been generated");
    auto eventSourceName = UA_STRING_ALLOC("Server");

    // Setting the Time is required or else the event will not show up in UAExpert! */
    UA_DateTime eventTime = UA_DateTime_now();
    UA_Server_writeObjectProperty_scalar(server, eventNodeId, timePropertyName,
                                         &eventTime, &UA_TYPES[UA_TYPES_DATETIME]);

    UA_Server_writeObjectProperty_scalar(server, eventNodeId, severityPropertyName,
                                         &eventSeverity, &UA_TYPES[UA_TYPES_UINT16]);

    UA_Server_writeObjectProperty_scalar(server, eventNodeId, messagePropertyName,
                                         &eventMessage, &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);

    UA_Server_writeObjectProperty_scalar(server, eventNodeId, sourceNamePropertyName,
                                         &eventSourceName, &UA_TYPES[UA_TYPES_STRING]);

    UA_QualifiedName_clear(&timePropertyName);
    UA_QualifiedName_clear(&severityPropertyName);
    UA_QualifiedName_clear(&messagePropertyName);
    UA_QualifiedName_clear(&sourceNamePropertyName);
    UA_LocalizedText_clear(&eventMessage);
    UA_String_clear(&eventSourceName);

    // End setup event

    ret = UA_Server_triggerEvent(server, eventNodeId,
                                 UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER),
                                 nullptr, true);

    if (ret != UA_STATUSCODE_GOOD)
        qWarning() << "Failed to trigger event:" << UA_StatusCode_name(ret);

    return ret;
}

void TestServer::readHistoryEventCallback(UA_Server *server, void *hdbContext, const UA_NodeId *sessionId, void *sessionContext,
                                   const UA_RequestHeader *requestHeader, const UA_ReadEventDetails *historyReadDetails,
                                   UA_TimestampsToReturn timestampsToReturn, UA_Boolean releaseContinuationPoints,
                                   size_t nodesToReadSize, const UA_HistoryReadValueId *nodesToRead, UA_HistoryReadResponse *response,
                                   UA_HistoryEvent * const * const historyData)
{
    Q_UNUSED(server)
    Q_UNUSED(hdbContext)
    Q_UNUSED(sessionId)
    Q_UNUSED(sessionContext)
    Q_UNUSED(requestHeader)
    Q_UNUSED(timestampsToReturn)
    Q_UNUSED(releaseContinuationPoints)

    QMap<UA_DateTime, UA_LocalizedText> historyEvents {
        { UA_DateTime_fromUnixTime(1694153836 - 120 * 60), UA_LocalizedText{UA_STRING_STATIC("en"), UA_STRING_STATIC("Message 1")}},
        { UA_DateTime_fromUnixTime(1694153836 - 60 * 60), UA_LocalizedText{UA_STRING_STATIC("en"), UA_STRING_STATIC("Message 2")}},
        { UA_DateTime_fromUnixTime(1694153836), UA_LocalizedText{UA_STRING_STATIC("en"), UA_STRING_STATIC("Message 3")}}
    };

    const auto historian1Id = u"ns=2;s=EventHistorian"_s;
    const auto historian2Id = u"ns=2;s=EventHistorian2"_s;

    for (size_t i = 0; i < nodesToReadSize; ++i) {
        const auto idToRead = QOpen62541ValueConverter::scalarToQt<QString, UA_NodeId>(&nodesToRead[i].nodeId);

        if (idToRead != historian1Id && idToRead != historian2Id) {
            response->results[i].statusCode = UA_STATUSCODE_BADNODEIDINVALID;
            continue;
        }

        if (releaseContinuationPoints)
            continue;

        QMap<UA_DateTime, UA_LocalizedText> eventsToReturn;
        UA_DateTime continuationPoint = 0;

        UA_DateTime providedContinuationPoint = 0;
        if (nodesToRead[i].continuationPoint.length) {
            if (nodesToRead[i].continuationPoint.length == sizeof(UA_DateTime)) {
                providedContinuationPoint = *reinterpret_cast<UA_DateTime *>(nodesToRead[i].continuationPoint.data);
            } else {
                response->results[i].statusCode = UA_STATUSCODE_BADCONTINUATIONPOINTINVALID;
                continue;
            }
        }

        const auto isHistorian2 = QOpen62541ValueConverter::scalarToQt<QString, UA_NodeId>(&nodesToRead[i].nodeId) == historian2Id;

        for (auto it = historyEvents.constBegin(); it != historyEvents.constEnd(); ++it) {
            if (isHistorian2 && it.key() == UA_DateTime_fromUnixTime(1694153836))
                break;

            if (providedContinuationPoint && it.key() < providedContinuationPoint)
                continue;

            if (it.key() < historyReadDetails->startTime || it.key() > historyReadDetails->endTime)
                continue;

            if (eventsToReturn.size() && eventsToReturn.size() == historyReadDetails->numValuesPerNode) {
                continuationPoint = it.key();
                break;
            }

            eventsToReturn.insert(it.key(), it.value());
        }

        historyData[i]->eventsSize = eventsToReturn.size();
        historyData[i]->events = static_cast<UA_HistoryEventFieldList *>(UA_Array_new(eventsToReturn.size(), &UA_TYPES[UA_TYPES_HISTORYEVENTFIELDLIST]));

        size_t j = 0;
        for (auto it = eventsToReturn.constBegin(); it != eventsToReturn.constEnd(); ++it, ++j) {
            historyData[i]->events[j].eventFieldsSize = historyReadDetails->filter.selectClausesSize;
            historyData[i]->events[j].eventFields = static_cast<UA_Variant *>(
                UA_Array_new(historyData[i]->events[j].eventFieldsSize, &UA_TYPES[UA_TYPES_VARIANT]));

            for (size_t k = 0; k < historyReadDetails->filter.selectClausesSize; ++k) {
                if (historyReadDetails->filter.selectClauses[k].browsePathSize != 1)
                    continue;

                const auto timeName = UA_QualifiedName{0, UA_STRING_STATIC("Time")};
                const auto messageName = UA_QualifiedName{0, UA_STRING_STATIC("Message")};
                auto path = historyReadDetails->filter.selectClauses[k].browsePath;

                if (UA_QualifiedName_equal(path, &timeName)) {
                    UA_Variant_setScalarCopy(&historyData[i]->events[j].eventFields[k], &it.key(), &UA_TYPES[UA_TYPES_DATETIME]);
                } else if (UA_QualifiedName_equal(path, &messageName)) {
                    UA_Variant_setScalarCopy(&historyData[i]->events[j].eventFields[k], &it.value(), &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);
                }
            }
        }

        if (continuationPoint) {
            response->results[i].continuationPoint.length = sizeof(UA_DateTime);
            auto dt = UA_DateTime_new();
            *dt = continuationPoint;
            response->results[i].continuationPoint.data = reinterpret_cast<UA_Byte *>(dt);
        }
    }
}

UA_StatusCode TestServer::readLocalizedTextCallback(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext,
                                                    const UA_NodeId *nodeId, void *nodeContext, UA_Boolean includeSourceTimeStamp,
                                                    const UA_NumericRange *range, UA_DataValue *value)
{
    Q_UNUSED(sessionContext);
    Q_UNUSED(nodeId);
    Q_UNUSED(nodeContext);
    Q_UNUSED(includeSourceTimeStamp);
    Q_UNUSED(range);

    UA_Variant localeIdsVar;
    UA_Variant_init(&localeIdsVar);

    auto result = UA_Server_getSessionAttribute(server, sessionId, {0, UA_STRING_STATIC("localeIds")}, &localeIdsVar);

    if (result != UA_STATUSCODE_GOOD)
        return result;

    if (localeIdsVar.type != &UA_TYPES[UA_TYPES_STRING])
        return UA_STATUSCODE_BADINTERNALERROR;

    const auto localeIds = static_cast<UA_LocaleId *>(localeIdsVar.data);

    const auto english = UA_LocalizedText{ UA_STRING_STATIC("en"), UA_STRING_STATIC("Hello")};
    const auto german = UA_LocalizedText{ UA_STRING_STATIC("de"), UA_STRING_STATIC("Guten Tag")};
    const auto french = UA_LocalizedText{ UA_STRING_STATIC("fr"), UA_STRING_STATIC("Bonjour")};

    const UA_LocalizedText *resultValue = &english;

    for (size_t i = 0; i < localeIdsVar.arrayLength; ++i) {
        const auto currentLocaleId = QOpen62541ValueConverter::scalarToQt<QString, UA_String>(&localeIds[i]);
        if (currentLocaleId.startsWith(u"de"_s))
            resultValue = &german;
        if (currentLocaleId.startsWith(u"fr"_s))
            resultValue = &french;
    }

    UA_Variant_setScalarCopy(&value->value, resultValue, &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);
    value->hasValue = true;

    return UA_STATUSCODE_GOOD;
}

UA_StatusCode TestServer::run(volatile bool *running)
{
    return UA_Server_run(m_server, running);
}

UA_StatusCode TestServer::addEventTrigger(const UA_NodeId &parent)
{
    UA_ObjectTypeAttributes objectAttr = UA_ObjectTypeAttributes_default;
    auto browseName = UA_QUALIFIEDNAME_ALLOC(2, "QtOpcUaEventType");
    auto result = UA_Server_addObjectTypeNode(m_server, UA_NODEID_NUMERIC(2, 12345),
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_BASEEVENTTYPE),
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
                                       browseName,
                                       objectAttr, nullptr, nullptr);

    UA_QualifiedName_clear(&browseName);

    if (result != UA_STATUSCODE_GOOD)
        return result;

    UA_MethodAttributes methodAttr = UA_MethodAttributes_default;
    methodAttr.description = UA_LOCALIZEDTEXT_ALLOC("en","Generates an event");
    methodAttr.displayName = UA_LOCALIZEDTEXT_ALLOC("en","Generates an event");
    methodAttr.executable = true;
    methodAttr.userExecutable = true;

    UA_Argument severityArgument;
    UA_Argument_init(&severityArgument);
    severityArgument.name = UA_STRING_ALLOC("Severity");
    severityArgument.dataType = UA_TYPES[UA_TYPES_UINT16].typeId;
    severityArgument.valueRank = -1;
    severityArgument.description = UA_LOCALIZEDTEXT_ALLOC("en", "The severity for the event to emit");

    UA_NodeId eventTriggerMethodId = UA_NODEID_STRING_ALLOC(2, "TriggerEvent");

    browseName = UA_QUALIFIEDNAME_ALLOC(2, "TriggerEvent");
    result = UA_Server_addMethodNode(m_server, eventTriggerMethodId,
                                     parent,
                                     UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                     browseName,
                                     methodAttr, &generateEventCallback,
                                     1, &severityArgument, 0, nullptr, nullptr, nullptr);

    UA_NodeId_clear(&eventTriggerMethodId);
    UA_QualifiedName_clear(&browseName);
    UA_MethodAttributes_clear(&methodAttr);
    UA_Argument_clear(&severityArgument);

    return result;
}

static UA_DataTypeArray customTypes = {
    nullptr,
    UA_TYPES_QTOPCUATESTMODEL_COUNT,
    &UA_TYPES_QTOPCUATESTMODEL[0],
    false
};

// This method must be called after the other test namespaces have been added
UA_StatusCode TestServer::addEncoderTestModel()
{
    auto result = namespace_qtopcuatestmodel_generated(m_server);

    if (result != UA_STATUSCODE_GOOD)
        return result;

    auto config = UA_Server_getConfig(m_server);
    customTypes.next = config->customDataTypes;
    config->customDataTypes = &customTypes;

    {
        auto data = UA_QtTestStructType_new();

        data->enumMember = UA_QTTESTENUMERATION_FIRSTOPTION;
        data->int64ArrayMemberSize = 3;
        data->int64ArrayMember = static_cast<UA_Int64 *>(UA_Array_new(3, &UA_TYPES[UA_TYPES_INT64]));
        data->int64ArrayMember[0] = std::numeric_limits<qint64>::max();
        data->int64ArrayMember[1] = std::numeric_limits<qint64>::max() - 1;
        data->int64ArrayMember[2] = std::numeric_limits<qint64>::min();

        data->stringMember = UA_STRING_ALLOC("TestString");
        data->localizedTextMember = UA_LOCALIZEDTEXT_ALLOC("en", "TestText");
        data->qualifiedNameMember = UA_QUALIFIEDNAME_ALLOC(1, "TestName");

        data->nestedStructMember.doubleSubtypeMember = 42;

        data->nestedStructArrayMemberSize = 2;
        data->nestedStructArrayMember = static_cast<UA_QtInnerTestStructType *>(
            UA_Array_new(2, &UA_TYPES_QTOPCUATESTMODEL[UA_TYPES_QTOPCUATESTMODEL_QTINNERTESTSTRUCTTYPE]));

        data->nestedStructArrayMember[0].doubleSubtypeMember = 23.0;
        data->nestedStructArrayMember[1].doubleSubtypeMember = 42.0;

        UA_Variant var;
        UA_Variant_init(&var);
        UA_Variant_setScalar(&var, data, &UA_TYPES_QTOPCUATESTMODEL[UA_TYPES_QTOPCUATESTMODEL_QTTESTSTRUCTTYPE]);

        result = UA_Server_writeValue(m_server, UA_NODEID_NUMERIC(4, UA_QTOPCUATESTMODELID_DECODERTESTNODES_NESTEDSTRUCT), var);
        UA_Variant_clear(&var);

        if (result != UA_STATUSCODE_GOOD) {
            qWarning() << "Failed to write nested struct node";
            return result;
        }
    }

    {
        UA_QtTestUnionType data;
        UA_QtTestUnionType_init(&data);

        data.switchField = UA_QTTESTUNIONTYPESWITCH_MEMBER1;
        data.fields.member1 = 42;

        UA_Variant var;
        UA_Variant_init(&var);
        UA_Variant_setScalarCopy(&var, &data, &UA_TYPES_QTOPCUATESTMODEL[UA_TYPES_QTOPCUATESTMODEL_QTTESTUNIONTYPE]);

        result = UA_Server_writeValue(m_server, UA_NODEID_NUMERIC(4, UA_QTOPCUATESTMODELID_DECODERTESTNODES_UNIONWITHFIRSTMEMBER), var);
        UA_Variant_clear(&var);

        if (result != UA_STATUSCODE_GOOD) {
            qWarning() << "Failed to write union struct node";
            return result;
        }
    }

    {
        UA_QtTestUnionType data;
        UA_QtTestUnionType_init(&data);

        data.switchField = UA_QTTESTUNIONTYPESWITCH_MEMBER2;
        data.fields.member2.doubleSubtypeMember = 23.0;

        UA_Variant var;
        UA_Variant_init(&var);
        UA_Variant_setScalarCopy(&var, &data, &UA_TYPES_QTOPCUATESTMODEL[UA_TYPES_QTOPCUATESTMODEL_QTTESTUNIONTYPE]);

        result = UA_Server_writeValue(m_server, UA_NODEID_NUMERIC(4, UA_QTOPCUATESTMODELID_DECODERTESTNODES_UNIONWITHSECONDMEMBER), var);
        UA_Variant_clear(&var);

        if (result != UA_STATUSCODE_GOOD) {
            qWarning() << "Failed to write union struct node";
            return result;
        }
    }

    {
        auto data = UA_QtStructWithOptionalFieldType_new();

        data->mandatoryMember =  42.0;
        data->optionalMember = UA_Double_new();
        *data->optionalMember = 23.0;

        UA_Variant var;
        UA_Variant_init(&var);
        UA_Variant_setScalar(&var, data, &UA_TYPES_QTOPCUATESTMODEL[UA_TYPES_QTOPCUATESTMODEL_QTSTRUCTWITHOPTIONALFIELDTYPE]);

        result = UA_Server_writeValue(m_server, UA_NODEID_NUMERIC(4, UA_QTOPCUATESTMODELID_DECODERTESTNODES_STRUCTWITHOPTIONALFIELD), var);
        UA_Variant_clear(&var);

        if (result != UA_STATUSCODE_GOOD) {
            qWarning() << "Failed to write optional field struct node";
            return result;
        }
    }

    {
        UA_QtStructWithOptionalFieldType data;
        UA_QtStructWithOptionalFieldType_init(&data);

        data.mandatoryMember =  42.0;

        UA_Variant var;
        UA_Variant_init(&var);
        UA_Variant_setScalarCopy(&var, &data, &UA_TYPES_QTOPCUATESTMODEL[UA_TYPES_QTOPCUATESTMODEL_QTSTRUCTWITHOPTIONALFIELDTYPE]);

        result = UA_Server_writeValue(m_server, UA_NODEID_NUMERIC(4, UA_QTOPCUATESTMODELID_DECODERTESTNODES_STRUCTWIHOUTOPTIONALFIELD), var);
        UA_Variant_clear(&var);

        if (result != UA_STATUSCODE_GOOD) {
            qWarning() << "Failed to write optional field struct node";
            return result;
        }
    }

    {
        auto data = UA_QtTestStructWithDiagnosticInfo_new();

        data->diagnosticInfoMember.hasSymbolicId = true;
        data->diagnosticInfoMember.symbolicId = 1;
        data->diagnosticInfoMember.hasNamespaceUri = true;
        data->diagnosticInfoMember.namespaceUri = 2;
        data->diagnosticInfoMember.hasLocale = true;
        data->diagnosticInfoMember.locale = 3;
        data->diagnosticInfoMember.hasLocalizedText = true;
        data->diagnosticInfoMember.localizedText = 4;
        data->diagnosticInfoMember.hasAdditionalInfo = true;
        data->diagnosticInfoMember.additionalInfo = UA_STRING_ALLOC("My additional info");
        data->diagnosticInfoMember.hasInnerStatusCode = true;
        data->diagnosticInfoMember.innerStatusCode = UA_STATUSCODE_BADINTERNALERROR;
        data->diagnosticInfoMember.hasInnerDiagnosticInfo = true;

        data->diagnosticInfoMember.innerDiagnosticInfo = UA_DiagnosticInfo_new();
        data->diagnosticInfoMember.innerDiagnosticInfo->hasAdditionalInfo = true;
        data->diagnosticInfoMember.innerDiagnosticInfo->additionalInfo = UA_STRING_ALLOC("My inner additional info");

        data->diagnosticInfoArrayMemberSize = 2;
        data->diagnosticInfoArrayMember = static_cast<UA_DiagnosticInfo *>(UA_Array_new(2, &UA_TYPES[UA_TYPES_DIAGNOSTICINFO]));

        UA_DiagnosticInfo_copy(&data->diagnosticInfoMember, &data->diagnosticInfoArrayMember[0]);

        data->diagnosticInfoArrayMember[1].hasLocale = true;
        data->diagnosticInfoArrayMember[1].locale = 1;
        data->diagnosticInfoArrayMember[1].hasInnerStatusCode = true;
        data->diagnosticInfoArrayMember[1].innerStatusCode = UA_STATUSCODE_BADTYPEMISMATCH;

        UA_Variant var;
        UA_Variant_init(&var);
        UA_Variant_setScalar(&var, data, &UA_TYPES_QTOPCUATESTMODEL[UA_TYPES_QTOPCUATESTMODEL_QTTESTSTRUCTWITHDIAGNOSTICINFO]);

        result = UA_Server_writeValue(m_server, UA_NODEID_NUMERIC(4, UA_QTOPCUATESTMODELID_DECODERTESTNODES_STRUCTWITHDIAGNOSTICINFO), var);
        UA_Variant_clear(&var);

        if (result != UA_STATUSCODE_GOOD) {
            qWarning() << "Failed to write diagnostic info field struct node";
            return result;
        }
    }

    {
        auto data = UA_QtTestStructWithDataValue_new();

        auto stringArray = static_cast<UA_String *>(UA_Array_new(4, &UA_TYPES[UA_TYPES_STRING]));
        stringArray[0] = UA_STRING_ALLOC("TestString 1");
        stringArray[1] = UA_STRING_ALLOC("TestString 2");
        stringArray[2] = UA_STRING_ALLOC("TestString 3");
        stringArray[3] = UA_STRING_ALLOC("TestString 4");
        UA_Variant_setArray(&data->dataValueMember.value, stringArray, 4, &UA_TYPES[UA_TYPES_STRING]);

        data->dataValueMember.value.arrayLength = 4;
        data->dataValueMember.value.arrayDimensions = static_cast<quint32 *>(UA_Array_new(2, &UA_TYPES[UA_TYPES_UINT32]));
        data->dataValueMember.value.arrayDimensions[0] = 2;
        data->dataValueMember.value.arrayDimensions[1] = 2;
        data->dataValueMember.value.arrayDimensionsSize = 2;

        data->dataValueMember.hasValue = true;
        data->dataValueMember.status = UA_STATUSCODE_BADINTERNALERROR;
        data->dataValueMember.hasStatus = true;
        data->dataValueMember.serverTimestamp = UA_DateTime_fromUnixTime(1698655307);
        data->dataValueMember.hasServerTimestamp = true;
        data->dataValueMember.sourceTimestamp = UA_DateTime_fromUnixTime(1698655306);
        data->dataValueMember.hasSourceTimestamp = true;
        data->dataValueMember.serverPicoseconds = 23;
        data->dataValueMember.hasServerPicoseconds = true;
        data->dataValueMember.sourcePicoseconds = 42;
        data->dataValueMember.hasSourcePicoseconds = true;

        const quint64 num = 42;
        UA_Variant_setScalarCopy(&data->variantMember, &num, &UA_TYPES[UA_TYPES_UINT64]);

        UA_Variant var;
        UA_Variant_init(&var);
        UA_Variant_setScalar(&var, data, &UA_TYPES_QTOPCUATESTMODEL[UA_TYPES_QTOPCUATESTMODEL_QTTESTSTRUCTWITHDATAVALUE]);

        result = UA_Server_writeValue(m_server, UA_NODEID_NUMERIC(4, UA_QTOPCUATESTMODELID_DECODERTESTNODES_STRUCTWITHDATAVALUE), var);
        UA_Variant_clear(&var);

        if (result != UA_STATUSCODE_GOOD) {
            qWarning() << "Failed to write data value field struct node";
            return result;
        }
    }

    {
        auto data = UA_QtRecursiveTestStruct_new();

        auto nestedArray = static_cast<UA_QtRecursiveTestStruct *>(UA_Array_new(2, &UA_TYPES_QTOPCUATESTMODEL[UA_TYPES_QTOPCUATESTMODEL_QTRECURSIVETESTSTRUCT]));
        nestedArray[0].stringMember = UA_STRING_ALLOC("Nested string 1");
        nestedArray[1].stringMember = UA_STRING_ALLOC("Nested string 2");

        nestedArray[0].recursiveArrayMember = UA_QtRecursiveTestStruct_new();
        nestedArray[0].recursiveArrayMemberSize = 1;
        nestedArray[0].recursiveArrayMember->stringMember = UA_STRING_ALLOC("Innermost string");

        data->recursiveArrayMember = nestedArray;
        data->recursiveArrayMemberSize = 2;
        data->stringMember = UA_STRING_ALLOC("Outer string");

        UA_Variant var;
        UA_Variant_init(&var);
        UA_Variant_setScalar(&var, data, &UA_TYPES_QTOPCUATESTMODEL[UA_TYPES_QTOPCUATESTMODEL_QTRECURSIVETESTSTRUCT]);

        result = UA_Server_writeValue(m_server, UA_NODEID_NUMERIC(4, UA_QTOPCUATESTMODELID_DECODERTESTNODES_RECURSIVESTRUCT), var);
        UA_Variant_clear(&var);

        if (result != UA_STATUSCODE_GOOD) {
            qWarning() << "Failed to write recursive struct node";
            return result;
        }
    }

    return result;
}

UA_StatusCode TestServer::addUnreadableVariableNode(const UA_NodeId &parent)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.value = QOpen62541ValueConverter::toOpen62541Variant(42, QOpcUa::Int32);
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", "VariableWithoutReadPermission");
    attr.dataType = attr.value.type->typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_WRITE; // Write only

    UA_QualifiedName variableName;
    variableName.namespaceIndex = parent.namespaceIndex;
    variableName.name = attr.displayName.text;

    auto nodeId = UA_NODEID_STRING_ALLOC(parent.namespaceIndex, "VariableWithoutReadPermission");
    UA_StatusCode result = UA_Server_addVariableNode(m_server,
                                                     nodeId,
                                                     parent,
                                                     UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                                     variableName,
                                                     UA_NODEID_NULL,
                                                     attr,
                                                     nullptr,
                                                     nullptr);
    UA_NodeId_clear(&nodeId);
    UA_VariableAttributes_clear(&attr);
    return result;
}

UA_StatusCode TestServer::addByteStringNodeIdWithNullIdVariableNode(const UA_NodeId &parent)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.value = QOpen62541ValueConverter::toOpen62541Variant(42, QOpcUa::Int32);
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", "VariableWithByteStringWithNullByteId");
    attr.dataType = attr.value.type->typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ;

    UA_QualifiedName variableName;
    variableName.namespaceIndex = parent.namespaceIndex;
    variableName.name = attr.displayName.text;

    UA_NodeId nodeId = UA_NODEID_NULL;
    nodeId.namespaceIndex = 1;
    nodeId.identifierType = UA_NODEIDTYPE_BYTESTRING;
    const auto bytes = QByteArray::fromBase64(u"AAABAAIADoo="_s.toLatin1());
    UA_ByteString_allocBuffer(&nodeId.identifier.byteString, bytes.length());
    memcpy(nodeId.identifier.byteString.data, bytes.constData(), bytes.length());

    UA_StatusCode result = UA_Server_addVariableNode(m_server,
                                                     nodeId,
                                                     parent,
                                                     UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                                     variableName,
                                                     UA_NODEID_NULL,
                                                     attr,
                                                     nullptr,
                                                     nullptr);
    UA_NodeId_clear(&nodeId);
    UA_VariableAttributes_clear(&attr);
    return result;
}

UA_StatusCode TestServer::addEventHistorian(const UA_NodeId &parent)
{
    UA_ObjectAttributes attr = UA_ObjectAttributes_default;
    attr.eventNotifier = UA_EVENTNOTIFIER_SUBSCRIBE_TO_EVENT | UA_EVENTNOTIFIER_HISTORY_READ;
    attr.description = UA_LocalizedText{UA_STRING_STATIC("en"),
                                        UA_STRING_STATIC("This node does not conform to part 11 of the OPC UA specification"
                                                         "and only supports the event history unit tests scenario")};

    UA_NodeId objectId = UA_NODEID_STRING_ALLOC(2, "EventHistorian");
    auto result = UA_Server_addObjectNode(m_server, objectId, parent, UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                          UA_QualifiedName{2, UA_STRING_STATIC("EventHistorian")},
                                          UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE), attr, this, nullptr);

    UA_NodeId_clear(&objectId);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Failed to add event historian object:" << UA_StatusCode_name(result);
        return result;
    }

    m_config->historyDatabase.readEvent = readHistoryEventCallback;

    return UA_STATUSCODE_GOOD;
}

QT_END_NAMESPACE

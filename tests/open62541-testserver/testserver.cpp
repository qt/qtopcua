// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
}

TestServer::~TestServer()
{
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
        UA_ByteString_clear(&fileContents);
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

    // result = UA_ServerConfig_addAllSecurityPolicies(config, &certificate, &privateKey);

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

    // Do not delete items on success.
    // They will be used by the server.
    certificateDeleter.release();
    privateKeyDeleter.release();

    result = UA_AccessControl_default(config, true, nullptr,
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

    UA_QualifiedName_clear(&nodeBrowseName);
    UA_NodeId_clear(&requestedNodeId);
    UA_ObjectAttributes_clear(&oAttr);

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

    UA_QualifiedName_clear(&nodeBrowseName);
    UA_ObjectAttributes_clear(&oAttr);

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


    UA_NodeId_clear(&variableNodeId);
    UA_VariableAttributes_clear(&attr);

    if (result != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not add variable:" << result;
        return UA_NODEID_NULL;
    }

    return resultId;
}

UA_NodeId TestServer::addVariable(const UA_NodeId &folder, const QString &variableNode, const QString &name, const QVariant &value,
                                  QOpcUa::Types type, QList<quint32> arrayDimensions, int valueRank, bool enableHistorizing, quint32 historyNumValuesPerNode)
{
    UA_NodeId variableNodeId = Open62541Utils::nodeIdFromQString(variableNode);

    const QString description = QStringLiteral("Description for %1").arg(variableNode);

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
        setting.historizingBackend = UA_HistoryDataBackend_Memory(1, 10);
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

    UA_QualifiedName_clear(&browseName);
    UA_MethodAttributes_clear(&methodAttr);
    UA_Argument_clear(&severityArgument);

    return result;
}

QT_END_NAMESPACE

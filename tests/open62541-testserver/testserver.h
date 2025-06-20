// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef TESTSERVER_H
#define TESTSERVER_H

#ifdef USE_SYSTEM_OPEN62541
#include <open62541/server_config_default.h>
#include <open62541/plugin/historydata/history_data_gathering_default.h>
#include <open62541/plugin/historydata/history_data_backend_memory.h>
#include <open62541/plugin/historydata/history_database_default.h>
#include <open62541/plugin/accesscontrol_default.h>
#else
#include "qopen62541.h"
#endif

#include <QtOpcUa/qopcuatype.h>

#include <QtCore/QDateTime>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QVariant>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

class ManagedUaNodeId {
private:
    struct NodeIdDeleter {
        void operator()(UA_NodeId* nodeId) const {
            if (nodeId)
                UA_NodeId_delete(nodeId);
        }
    };

public:

    ManagedUaNodeId(UA_NodeId nodeId)
        : m_nodeId(std::shared_ptr<UA_NodeId>(UA_NodeId_new(), NodeIdDeleter()))
    {
        *m_nodeId = nodeId;
    }

    ManagedUaNodeId(const ManagedUaNodeId &other)
        : m_nodeId(other.m_nodeId)
    {}

    operator const UA_NodeId() const
    {
        return *m_nodeId.get();
    }

private:
    std::shared_ptr<UA_NodeId> m_nodeId;
};

class TestServer : public QObject
{
    Q_OBJECT
public:
    explicit TestServer(QObject *parent = nullptr);
    ~TestServer();
    bool init(bool noNonePolicyPassword);
    bool createInsecureServerConfig(UA_ServerConfig *config);
#if defined UA_ENABLE_ENCRYPTION
    bool createSecureServerConfig(UA_ServerConfig *config);
#endif

    int registerNamespace(const QString &ns);
    ManagedUaNodeId addFolder(const QString &nodeString, const QString &displayName, const QString &description = QString());
    ManagedUaNodeId addObject(const UA_NodeId &folderId, int namespaceIndex, const QString &objectName = QString());

    ManagedUaNodeId addVariable(const UA_NodeId &folder, const QString &variableNode, const QString &name, const QVariant &value,
                          QOpcUa::Types type, QList<quint32> arrayDimensions = QList<quint32>(), int valueRank = UA_VALUERANK_ANY,
                          bool enableHistorizing = false, quint32 historyNumValuesPerNode = 100);
    ManagedUaNodeId addVariableWithWriteMask(const UA_NodeId &folder, const QString &variableNode, const QString &name, const QVariant &value,
                          QOpcUa::Types type, quint32 writeMask);
    ManagedUaNodeId addEmptyArrayVariable(const UA_NodeId &folder, const QString &variableNode, const QString &name);

    ManagedUaNodeId addMultiplyMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description);
    ManagedUaNodeId addMultipleOutputArgumentsMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description);
    ManagedUaNodeId addAddNamespaceMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description);
    ManagedUaNodeId addNodeWithFixedTimestamp(const UA_NodeId &folder, const QString &nodeId, const QString &displayName);

    UA_StatusCode addServerStatusTypeTestNodes(const UA_NodeId &parent);
    UA_StatusCode addLocalizedTextNodeWithCallback(const UA_NodeId &parent);

    UA_StatusCode addEventTrigger(const UA_NodeId &parent);
    UA_StatusCode addEventHistorian(const UA_NodeId &parent);

    UA_StatusCode addEncoderTestModel();

    UA_StatusCode addUnreadableVariableNode(const UA_NodeId &parent);

    UA_StatusCode addByteStringNodeIdWithNullIdVariableNode(const UA_NodeId &parent);

    static UA_StatusCode multiplyMethod(UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle,
                                            const UA_NodeId *methodId, void *methodContext,
                                            const UA_NodeId *objectId, void *objectContext,
                                            size_t inputSize, const UA_Variant *input,
                                            size_t outputSize, UA_Variant *output);

    static UA_StatusCode multipleOutputArgumentsMethod(UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle,
                                            const UA_NodeId *methodId, void *methodContext,
                                            const UA_NodeId *objectId, void *objectContext,
                                            size_t inputSize, const UA_Variant *input,
                                            size_t outputSize, UA_Variant *output);

    static UA_StatusCode addNamespaceMethod(UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle,
                                            const UA_NodeId *methodId, void *methodContext, const UA_NodeId *objectId,
                                            void *objectContext, size_t inputSize, const UA_Variant *input, size_t outputSize,
                                            UA_Variant *output);

    static UA_StatusCode generateEventCallback(UA_Server *server,
                             const UA_NodeId *sessionId, void *sessionHandle,
                             const UA_NodeId *methodId, void *methodContext,
                             const UA_NodeId *objectId, void *objectContext,
                             size_t inputSize, const UA_Variant *input,
                             size_t outputSize, UA_Variant *output);

    static void readHistoryEventCallback(UA_Server *server,
                                  void *hdbContext,
                                  const UA_NodeId *sessionId,
                                  void *sessionContext,
                                  const UA_RequestHeader *requestHeader,
                                  const UA_ReadEventDetails *historyReadDetails,
                                  UA_TimestampsToReturn timestampsToReturn,
                                  UA_Boolean releaseContinuationPoints,
                                  size_t nodesToReadSize,
                                  const UA_HistoryReadValueId *nodesToRead,
                                  UA_HistoryReadResponse *response,
                                  UA_HistoryEvent * const * const historyData);

    static UA_StatusCode readLocalizedTextCallback(UA_Server *server, const UA_NodeId *sessionId,
                                                   void *sessionContext, const UA_NodeId *nodeId,
                                                   void *nodeContext, UA_Boolean includeSourceTimeStamp,
                                                   const UA_NumericRange *range, UA_DataValue *value);

    UA_ServerConfig *m_config{nullptr};
    UA_Server *m_server{nullptr};
    UA_HistoryDataGathering m_gathering;
    UA_HistoryDataBackend m_historyDataBackend;
public slots:
    UA_StatusCode run(volatile bool *running);
};

QT_END_NAMESPACE

#endif // TESTSERVER_H

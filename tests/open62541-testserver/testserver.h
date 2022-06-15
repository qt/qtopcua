// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef TESTSERVER_H
#define TESTSERVER_H

#include <qopen62541.h>
#include <QtOpcUa/qopcuatype.h>

#include <QtCore/QDateTime>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QVariant>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

class TestServer : public QObject
{
    Q_OBJECT
public:
    explicit TestServer(QObject *parent = nullptr);
    ~TestServer();
    bool init();
    bool createInsecureServerConfig(UA_ServerConfig *config);
#if defined UA_ENABLE_ENCRYPTION
    bool createSecureServerConfig(UA_ServerConfig *config);
#endif

    int registerNamespace(const QString &ns);
    UA_NodeId addFolder(const QString &nodeString, const QString &displayName, const QString &description = QString());
    UA_NodeId addObject(const UA_NodeId &folderId, int namespaceIndex, const QString &objectName = QString());

    UA_NodeId addVariable(const UA_NodeId &folder, const QString &variableNode, const QString &name, const QVariant &value,
                          QOpcUa::Types type, QList<quint32> arrayDimensions = QList<quint32>(), int valueRank = UA_VALUERANK_ANY,
                          bool enableHistorizing = false, quint32 historyNumValuesPerNode = 100);
    UA_NodeId addVariableWithWriteMask(const UA_NodeId &folder, const QString &variableNode, const QString &name, const QVariant &value,
                          QOpcUa::Types type, quint32 writeMask);
    UA_NodeId addEmptyArrayVariable(const UA_NodeId &folder, const QString &variableNode, const QString &name);

    UA_NodeId addMultiplyMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description);
    UA_NodeId addMultipleOutputArgumentsMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description);
    UA_NodeId addAddNamespaceMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description);
    UA_NodeId addNodeWithFixedTimestamp(const UA_NodeId &folder, const QString &nodeId, const QString &displayName);

    UA_StatusCode addEventTrigger(const UA_NodeId &parent);

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

    UA_ServerConfig *m_config{nullptr};
    UA_Server *m_server{nullptr};
    UA_HistoryDataGathering m_gathering;

public slots:
    UA_StatusCode run(volatile bool *running);
};

QT_END_NAMESPACE

#endif // TESTSERVER_H

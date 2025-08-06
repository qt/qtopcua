// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPEN62541BACKEND_H
#define QOPEN62541BACKEND_H

#include "qopen62541client.h"
#include "qopen62541subscription.h"
#include <private/qopcuabackend_p.h>

#include <QtCore/qset.h>
#include <QtCore/qstring.h>
#include <QtCore/qtimer.h>

QT_BEGIN_NAMESPACE

class Open62541AsyncBackend : public QOpcUaBackend
{
    Q_OBJECT
public:
    Open62541AsyncBackend(QOpen62541Client *parent);
    ~Open62541AsyncBackend();

public Q_SLOTS:
    void connectToEndpoint(const QOpcUaEndpointDescription &endpoint);
    void disconnectFromEndpoint();
    void requestEndpoints(const QUrl &url);

    void handleConnectionSettingsChanged(const QOpcUaConnectionSettings &settings);

    // Node functions
    void browse(quint64 handle, UA_NodeId id, const QOpcUaBrowseRequest &request);
    void readAttributes(quint64 handle, UA_NodeId id, QOpcUa::NodeAttributes attr, QString indexRange);

    void writeAttribute(quint64 handle, UA_NodeId id, QOpcUa::NodeAttribute attrId, QVariant value, QOpcUa::Types type, QString indexRange);
    void writeAttributes(quint64 handle, UA_NodeId id, QOpcUaNode::AttributeMap toWrite, QOpcUa::Types valueAttributeType);
    void enableMonitoring(quint64 handle, UA_NodeId id, QOpcUa::NodeAttributes attr, const QOpcUaMonitoringParameters &settings);
    void disableMonitoring(quint64 handle, QOpcUa::NodeAttributes attr);
    void modifyMonitoring(quint64 handle, QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, QVariant value);
    void callMethod(quint64 handle, UA_NodeId objectId, UA_NodeId methodId, QList<QOpcUa::TypedVariant> args);
    void resolveBrowsePath(quint64 handle, UA_NodeId startNode, const QList<QOpcUaRelativePathElement> &path);
    void findServers(const QUrl &url, const QStringList &localeIds, const QStringList &serverUris);

    void readNodeAttributes(const QList<QOpcUaReadItem> &nodesToRead);
    void writeNodeAttributes(const QList<QOpcUaWriteItem> &nodesToWrite);

    void readHistoryRaw(QOpcUaHistoryReadRawRequest request, QList<QByteArray> continuationPoints, bool releaseContinuationPoints, quint64 handle);
    void readHistoryEvents(const QOpcUaHistoryReadEventRequest &request, const QList<QByteArray> &continuationPoints,
                           bool releaseContinuationPoints, quint64 handle);

    // Node management
    void addNode(const QOpcUaAddNodeItem &nodeToAdd);
    void deleteNode(const QString &nodeId, bool deleteTargetReferences);
    void addReference(const QOpcUaAddReferenceItem &referenceToAdd);
    void deleteReference(const QOpcUaDeleteReferenceItem &referenceToDelete);

    // Subscription
    QOpen62541Subscription *getSubscription(const QOpcUaMonitoringParameters &settings);
    bool removeSubscription(UA_UInt32 subscriptionId);
    void iterateClient();
    void triggerIterateClient();
    void handleSubscriptionTimeout(QOpen62541Subscription *sub, QList<QPair<quint64, QOpcUa::NodeAttribute>> items);
    void cleanupSubscriptions();

    // Register and unregister nodes
    void registerNodes(const QStringList &nodesToRegister);
    void unregisterNodes(const QStringList &nodesToUnregister);

    // Callbacks
    static void asyncMethodCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncTranslateBrowsePathCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncAddNodeCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncDeleteNodeCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncAddReferenceCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncDeleteReferenceCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncReadCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncWriteAttributesCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncBrowseCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncBatchReadCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncBatchWriteCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncReadHistoryDataCallBack(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncRegisterNodesCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncUnregisterNodesCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncReadHistoryEventsCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);

public:
    UA_Client *m_uaclient;
    QOpen62541Client *m_clientImpl;
    quint32 m_clientIterateInterval;
    quint32 m_asyncRequestTimeout;

private:
    static void clientStateCallback(UA_Client *client,
                                    UA_SecureChannelState channelState,
                                    UA_SessionState sessionState,
                                    UA_StatusCode connectStatus);

    static void inactivityCallback(UA_Client *client);

    static void open62541LogHandler(void *logContext, UA_LogLevel level, UA_LogCategory category,
                                    const char *msg, va_list args);

    QOpen62541Subscription *getSubscriptionForItem(quint64 handle, QOpcUa::NodeAttribute attr);
    QOpcUaApplicationDescription convertApplicationDescription(UA_ApplicationDescription &desc);

    UA_ExtensionObject assembleNodeAttributes(const QOpcUaNodeCreationAttributes &nodeAttributes, QOpcUa::NodeClass nodeClass);
    UA_UInt32 *copyArrayDimensions(const QList<quint32> &arrayDimensions, size_t *outputSize);

    // Helper
    bool loadFileToByteString(const QString &location, UA_ByteString *target) const;
    bool loadAllFilesInDirectory(const QString &location, UA_ByteString **target, qsizetype *size) const;

    void disconnectInternal(QOpcUaClient::ClientError error = QOpcUaClient::ClientError::NoError);

#ifdef UA_ENABLE_ENCRYPTION
    bool setupClientConfigSecurity(const QOpcUaAuthenticationInformation &authInfo,
                                   const QOpcUaPkiConfiguration &pkiConfig,
                                   const QOpcUaEndpointDescription &endpoint);
#endif

    void establishConnectionInternal(const QOpcUaAuthenticationInformation &authInfo,
#ifdef UA_ENABLE_ENCRYPTION
                                     const QOpcUaPkiConfiguration &pkiConfig,
#endif
                         const QOpcUaEndpointDescription &endpoint);

#ifdef UA_ENABLE_ENCRYPTION
    bool loadPrivateKeyWithPotentialPassword(const QString &privateKeyPath, UA_ByteString &privateKey);
    UA_StatusCode setSecurityPolicyInClientConfig(UA_ClientConfig *conf, const UA_ByteString &cert, const UA_ByteString &key,
                                                  const QOpcUaEndpointDescription &desc);
    UA_StatusCode setAuthSecurityPolicyInClientConfig(UA_ClientConfig *conf, const UA_ByteString &cert, const UA_ByteString &key,
                                                      const QOpcUaEndpointDescription &desc, QOpcUaUserTokenPolicy::TokenType tokenType);
#endif

    QTimer m_clientIterateTimer;
    QTimer m_clientIterateOnDemandTimer;

    QHash<quint32, QOpen62541Subscription *> m_subscriptions;

    QHash<quint64, QHash<QOpcUa::NodeAttribute, QOpen62541Subscription *>> m_attributeMapping; // Handle -> Attribute -> Subscription

    double m_minPublishingInterval;

    UA_Logger m_open62541Logger {open62541LogHandler, nullptr, nullptr};

    QOpcUaConnectionSettings m_currentConnectionSettings;

    // Async contexts

    struct AsyncCallContext {
        quint64 handle;
        QString methodNodeId;
    };
    QMap<quint32, AsyncCallContext> m_asyncCallContext;

    struct AsyncTranslateContext {
        quint64 handle;
        QList<QOpcUaRelativePathElement> path;
    };
    QMap<quint32, AsyncTranslateContext> m_asyncTranslateContext;

    struct AsyncAddNodeContext {
        QOpcUaExpandedNodeId requestedNodeId;
    };
    QMap<quint32, AsyncAddNodeContext> m_asyncAddNodeContext;

    struct AsyncDeleteNodeContext {
        QString nodeId;
    };
    QMap<quint32, AsyncDeleteNodeContext> m_asyncDeleteNodeContext;

    struct AsyncAddReferenceContext {
        QString sourceNodeId;
        QString referenceTypeId;
        QOpcUaExpandedNodeId targetNodeId;
        bool isForwardReference;
    };
    QMap<quint32, AsyncAddReferenceContext> m_asyncAddReferenceContext;

    struct AsyncDeleteReferenceContext {
        QString sourceNodeId;
        QString referenceTypeId;
        QOpcUaExpandedNodeId targetNodeId;
        bool isForwardReference;
    };
    QMap<quint32, AsyncDeleteReferenceContext> m_asyncDeleteReferenceContext;

    struct AsyncReadContext {
        quint64 handle;
        QList<QOpcUaReadResult> results;
    };
    QMap<quint32, AsyncReadContext> m_asyncReadContext;

    struct AsyncWriteAttributesContext {
        quint64 handle;
        QOpcUaNode::AttributeMap toWrite;
    };
    QMap<quint32, AsyncWriteAttributesContext> m_asyncWriteAttributesContext;

    struct AsyncBrowseContext {
        quint64 handle;
        bool isBrowseNext;
        QList<QOpcUaReferenceDescription> results;
    };
    QMap<quint32, AsyncBrowseContext> m_asyncBrowseContext;

    struct AsyncBatchReadContext {
        QList<QOpcUaReadItem> nodesToRead;
    };
    QMap<quint32, AsyncBatchReadContext> m_asyncBatchReadContext;

    struct AsyncBatchWriteContext {
        QList<QOpcUaWriteItem> nodesToWrite;
    };
    QMap<quint32, AsyncBatchWriteContext> m_asyncBatchWriteContext;

    struct AsyncReadHistoryDataContext {
        quint64 handle;
        QOpcUaHistoryReadRawRequest historyReadRawRequest;
    };
    QMap<quint32, AsyncReadHistoryDataContext> m_asyncReadHistoryDataContext;

    struct AsyncRegisterUnregisterNodesContext {
        QStringList nodeIds;
    };
    QMap<quint32, AsyncRegisterUnregisterNodesContext> m_asyncRegisterUnregisterNodesContext;

    struct AsyncReadHistoryEventsContext {
        quint64 handle;
        QOpcUaHistoryReadEventRequest historyReadEventRequest;
    };
    QMap<quint32, AsyncReadHistoryEventsContext> m_asyncReadHistoryEventsContext;
};

QT_END_NAMESPACE

#endif // QOPEN62541BACKEND_H

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

    // Node management
    void addNode(const QOpcUaAddNodeItem &nodeToAdd);
    void deleteNode(const QString &nodeId, bool deleteTargetReferences);
    void addReference(const QOpcUaAddReferenceItem &referenceToAdd);
    void deleteReference(const QOpcUaDeleteReferenceItem &referenceToDelete);

    // Subscription
    QOpen62541Subscription *getSubscription(const QOpcUaMonitoringParameters &settings);
    bool removeSubscription(UA_UInt32 subscriptionId);
    void iterateClient();
    void handleSubscriptionTimeout(QOpen62541Subscription *sub, QList<QPair<quint64, QOpcUa::NodeAttribute>> items);
    void cleanupSubscriptions();

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

public:
    UA_Client *m_uaclient;
    QOpen62541Client *m_clientImpl;
    bool m_useStateCallback;
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
    bool loadAllFilesInDirectory(const QString &location, UA_ByteString **target, int *size) const;

    void disconnectInternal(QOpcUaClient::ClientError error = QOpcUaClient::ClientError::NoError);

    QTimer m_clientIterateTimer;
    QTimer m_disconnectAfterStateChangeTimer;

    QHash<quint32, QOpen62541Subscription *> m_subscriptions;

    QHash<quint64, QHash<QOpcUa::NodeAttribute, QOpen62541Subscription *>> m_attributeMapping; // Handle -> Attribute -> Subscription

    double m_minPublishingInterval;

    const UA_Logger m_open62541Logger {open62541LogHandler, nullptr, nullptr};

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
};

QT_END_NAMESPACE

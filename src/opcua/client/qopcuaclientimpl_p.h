// Copyright (C) 2016 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUACLIENTIMPL_P_H
#define QOPCUACLIENTIMPL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtOpcUa/qopcuaclient.h>
#include <QtOpcUa/qopcuaglobal.h>
#include <QtOpcUa/qopcuaendpointdescription.h>
#include <private/qopcuanodeimpl_p.h>

#include <QtCore/qobject.h>
#include <QtCore/qpointer.h>
#include <QtCore/qset.h>

QT_BEGIN_NAMESPACE

class QOpcUaNode;
class QOpcUaClient;
class QOpcUaBackend;
class QOpcUaMonitoringParameters;

class Q_OPCUA_EXPORT QOpcUaClientImpl : public QObject
{
    Q_OBJECT

public:
    QOpcUaClientImpl(QObject *parent = 0);
    virtual ~QOpcUaClientImpl();

    virtual void connectToEndpoint(const QOpcUaEndpointDescription &endpoint) = 0;
    virtual void disconnectFromEndpoint() = 0;
    virtual QOpcUaNode *node(const QString &nodeId) = 0;
    virtual QString backend() const = 0;
    virtual bool requestEndpoints(const QUrl &url) = 0;
    virtual bool findServers(const QUrl &url, const QStringList &localeIds, const QStringList &serverUris) = 0;
    virtual bool readNodeAttributes(const QList<QOpcUaReadItem> &nodesToRead) = 0;
    virtual bool writeNodeAttributes(const QList<QOpcUaWriteItem> &nodesToWrite) = 0;

    virtual QOpcUaHistoryReadResponse *readHistoryData(const QOpcUaHistoryReadRawRequest &request) = 0;

    bool registerNode(QPointer<QOpcUaNodeImpl> obj);
    void unregisterNode(QPointer<QOpcUaNodeImpl> obj);

    virtual bool addNode(const QOpcUaAddNodeItem &nodeToAdd) = 0;
    virtual bool deleteNode(const QString &nodeId, bool deleteTargetReferences) = 0;

    virtual bool addReference(const QOpcUaAddReferenceItem &referenceToAdd) = 0;
    virtual bool deleteReference(const QOpcUaDeleteReferenceItem &referenceToDelete) = 0;

    void connectBackendWithClient(QOpcUaBackend *backend);

    virtual QStringList supportedSecurityPolicies() const = 0;
    virtual QList<QOpcUaUserTokenPolicy::TokenType> supportedUserTokenTypes() const = 0;

    QOpcUaClient *m_client;

private Q_SLOTS:
    void handleAttributesRead(quint64 handle, QList<QOpcUaReadResult> attr, QOpcUa::UaStatusCode serviceResult);
    void handleAttributeWritten(quint64 handle, QOpcUa::NodeAttribute attr, const QVariant &value, QOpcUa::UaStatusCode statusCode);
    void handleDataChangeOccurred(quint64 handle, const QOpcUaReadResult &value);
    void handleMonitoringEnableDisable(quint64 handle, QOpcUa::NodeAttribute attr, bool subscribe, QOpcUaMonitoringParameters status);
    void handleMonitoringStatusChanged(quint64 handle, QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameters items,
                                 QOpcUaMonitoringParameters param);
    void handleMethodCallFinished(quint64 handle, QString methodNodeId, QVariant result, QOpcUa::UaStatusCode statusCode);
    void handleBrowseFinished(quint64 handle, const QList<QOpcUaReferenceDescription> &children, QOpcUa::UaStatusCode statusCode);

    void handleResolveBrowsePathFinished(quint64 handle, QList<QOpcUaBrowsePathTarget> targets,
                                           QList<QOpcUaRelativePathElement> path, QOpcUa::UaStatusCode status);

    void handleNewEvent(quint64 handle, QVariantList eventFields);

signals:
    void connected();
    void disconnected();
    void stateAndOrErrorChanged(QOpcUaClient::ClientState state,
                                QOpcUaClient::ClientError error);
    void endpointsRequestFinished(QList<QOpcUaEndpointDescription> endpoints, QOpcUa::UaStatusCode statusCode, QUrl requestUrl);
    void findServersFinished(QList<QOpcUaApplicationDescription> servers, QOpcUa::UaStatusCode statusCode, QUrl requestUrl);
    void readNodeAttributesFinished(QList<QOpcUaReadResult> results, QOpcUa::UaStatusCode serviceResult);
    void writeNodeAttributesFinished(QList<QOpcUaWriteResult> results, QOpcUa::UaStatusCode serviceResult);
    void addNodeFinished(QOpcUaExpandedNodeId requestedNodeId, QString assignedNodeId, QOpcUa::UaStatusCode statusCode);
    void deleteNodeFinished(QString nodeId, QOpcUa::UaStatusCode statusCode);
    void addReferenceFinished(QString sourceNodeId, QString referenceTypeId, QOpcUaExpandedNodeId targetNodeId, bool isForwardReference,
                              QOpcUa::UaStatusCode statusCode);
    void deleteReferenceFinished(QString sourceNodeId, QString referenceTypeId, QOpcUaExpandedNodeId targetNodeId, bool isForwardReference,
                              QOpcUa::UaStatusCode statusCode);
    void connectError(QOpcUaErrorState *errorState);
    void passwordForPrivateKeyRequired(const QString keyFilePath, QString *password, bool previousTryWasInvalid);

private:
    Q_DISABLE_COPY(QOpcUaClientImpl)
    QHash<quint64, QPointer<QOpcUaNodeImpl>> m_handles;
    quint64 m_handleCounter;
};

#if QT_VERSION >= 0x060000
inline size_t qHash(const QPointer<QOpcUaNodeImpl>& n)
#else
inline uint qHash(const QPointer<QOpcUaNodeImpl>& n)
#endif
{
    return ::qHash(n.data());
}

QT_END_NAMESPACE

#endif // QOPCUACLIENTIMPL_P_H

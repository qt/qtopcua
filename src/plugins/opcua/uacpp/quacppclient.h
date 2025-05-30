// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QUACPPCLIENT_H
#define QUACPPCLIENT_H

#include <private/qopcuaclientimpl_p.h>

#include <QtCore/QTimer>

QT_BEGIN_NAMESPACE

class UACppAsyncBackend;

class QUACppClient : public QOpcUaClientImpl
{
    Q_OBJECT

public:
    explicit QUACppClient(const QVariantMap &backendProperties);
    ~QUACppClient();

    void connectToEndpoint(const QOpcUaEndpointDescription &endpoint) override;
    void disconnectFromEndpoint() override;

    QOpcUaNode *node(const QString &nodeId) override;

    QString backend() const override;

    bool requestEndpoints(const QUrl &url) override;

    bool findServers(const QUrl &url, const QStringList &localeIds, const QStringList &serverUris) override;

    bool readNodeAttributes(const QList<QOpcUaReadItem> &nodesToRead) override;
    bool writeNodeAttributes(const QList<QOpcUaWriteItem> &nodesToWrite) override;

    QOpcUaHistoryReadResponse *readHistoryData(const QOpcUaHistoryReadRawRequest &request) override;

    bool addNode(const QOpcUaAddNodeItem &nodeToAdd) override;
    bool deleteNode(const QString &nodeId, bool deleteTargetReferences) override;

    bool addReference(const QOpcUaAddReferenceItem &referenceToAdd) override;
    bool deleteReference(const QOpcUaDeleteReferenceItem &referenceToDelete) override;

    QStringList supportedSecurityPolicies() const override;
    QList<QOpcUaUserTokenPolicy::TokenType> supportedUserTokenTypes() const override;

private:
    friend class QUACppNode;
    QThread *m_thread;
    UACppAsyncBackend *m_backend;
};

QT_END_NAMESPACE

#endif // QUACPPCLIENT_H

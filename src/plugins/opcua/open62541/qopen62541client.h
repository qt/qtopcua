// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPEN62541CLIENT_H
#define QOPEN62541CLIENT_H

#include "qopen62541.h"
#include <private/qopcuaclientimpl_p.h>

#include <QtCore/qtimer.h>

QT_BEGIN_NAMESPACE

class Open62541AsyncBackend;

class QOpen62541Client : public QOpcUaClientImpl
{
    Q_OBJECT

public:
    explicit QOpen62541Client(const QVariantMap &backendProperties);
    ~QOpen62541Client();

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

    Q_INVOKABLE bool handleHistoryReadRawRequested(const QOpcUaHistoryReadRawRequest &request, const QList<QByteArray> &continuationPoints,
                                                   bool releaseContinuationPoints, quint64 handle);

signals:
    void historyReadRequestError(quint64 handle);

private slots:

private:
    friend class QOpen62541Node;
    QThread *m_thread;
    Open62541AsyncBackend *m_backend;

#ifdef UA_ENABLE_ENCRYPTION
    bool m_hasSha1SignatureSupport = false;
#endif
};

QT_END_NAMESPACE

#endif // QOPEN62541CLIENT_H

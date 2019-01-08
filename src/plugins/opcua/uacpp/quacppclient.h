/******************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

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

    void connectToEndpoint(const QUrl &url) override;
    void disconnectFromEndpoint() override;

    QOpcUaNode *node(const QString &nodeId) override;

    QString backend() const override;

    bool requestEndpoints(const QUrl &url) override;

    bool findServers(const QUrl &url, const QStringList &localeIds, const QStringList &serverUris) override;

    bool readNodeAttributes(const QVector<QOpcUaReadItem> &nodesToRead) override;
    bool writeNodeAttributes(const QVector<QOpcUaWriteItem> &nodesToWrite) override;

    bool addNode(const QOpcUaAddNodeItem &nodeToAdd) override;
    bool deleteNode(const QString &nodeId, bool deleteTargetReferences) override;

    bool addReference(const QOpcUaAddReferenceItem &referenceToAdd) override;
    bool deleteReference(const QOpcUaDeleteReferenceItem &referenceToDelete) override;

private:
    friend class QUACppNode;
    QThread *m_thread;
    UACppAsyncBackend *m_backend;
};

QT_END_NAMESPACE

#endif // QUACPPCLIENT_H

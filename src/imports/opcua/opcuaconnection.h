/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#pragma once

#include "opcuareaditem.h"
#include <QJSValue>
#include <QObject>
#include <QOpcUaClient>
#include <QOpcUaAuthenticationInformation>
#include "opcuaserverdiscovery.h"

QT_BEGIN_NAMESPACE

class QOpcUaReadResult;
class OpcUaEndpointDiscovery;

class OpcUaConnection : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(OpcUaConnection)
    Q_PROPERTY(QStringList availableBackends READ availableBackends NOTIFY availableBackendsChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QString backend READ backend WRITE setBackend NOTIFY backendChanged)
    Q_PROPERTY(bool defaultConnection READ defaultConnection WRITE setDefaultConnection NOTIFY defaultConnectionChanged)
    Q_PROPERTY(QStringList namespaces READ namespaces NOTIFY namespacesChanged)
    Q_PROPERTY(QOpcUaAuthenticationInformation authenticationInformation READ authenticationInformation WRITE setAuthenticationInformation)
    Q_PROPERTY(QStringList supportedSecurityPolicies READ supportedSecurityPolicies CONSTANT)
    Q_PROPERTY(QJSValue supportedUserTokenTypes READ supportedUserTokenTypes CONSTANT)
    Q_PROPERTY(QOpcUaEndpointDescription currentEndpoint READ currentEndpoint)
    Q_PROPERTY(QOpcUaClient* connection READ connection WRITE setConnection NOTIFY connectionChanged)

public:
    OpcUaConnection(QObject *parent = nullptr);
    ~OpcUaConnection();
    QStringList availableBackends() const;
    bool connected() const;
    void setBackend(const QString &name);
    QString backend() const;
    static OpcUaConnection *defaultConnection();
    bool isDefaultConnection() const;
    QStringList namespaces() const;

    QOpcUaEndpointDescription currentEndpoint() const;

    QOpcUaAuthenticationInformation authenticationInformation() const;
    Q_INVOKABLE bool readNodeAttributes(const QJSValue &value);
    Q_INVOKABLE bool writeNodeAttributes(const QJSValue &value);

    QStringList supportedSecurityPolicies() const;
    QJSValue supportedUserTokenTypes() const;

    QOpcUaClient *connection() const;

public slots:
    void connectToEndpoint(const QOpcUaEndpointDescription &endpointDescription);
    void disconnectFromEndpoint();
    void setDefaultConnection(bool defaultConnection = true);
    void setAuthenticationInformation(const QOpcUaAuthenticationInformation &authenticationInformation);
    void setConnection(QOpcUaClient *client);

signals:
    void availableBackendsChanged();
    void connectedChanged();
    void backendChanged();
    void defaultConnectionChanged();
    void namespacesChanged();
    void readNodeAttributesFinished(const QVariant &value);
    void writeNodeAttributesFinished(const QVariant &value);
    void connectionChanged();

private slots:
    void clientStateHandler(QOpcUaClient::ClientState state);
    void handleReadNodeAttributesFinished(const QList<QOpcUaReadResult> &results);
    void handleWriteNodeAttributesFinished(const QList<QOpcUaWriteResult> &results);

private:
    void removeConnection();
    void setupConnection();

    QOpcUaClient *m_client = nullptr;
    bool m_connected = false;
    static OpcUaConnection* m_defaultConnection;

friend class OpcUaNode;
friend class OpcUaValueNode;
friend class OpcUaMethodNode;
friend class OpcUaEndpointDiscovery;
friend class OpcUaServerDiscovery;
};

QT_END_NAMESPACE

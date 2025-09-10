// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUA_OPCUACONNECTION_P_H
#define QOPCUA_OPCUACONNECTION_P_H

#include <private/opcuareaditem_p.h>
#include <private/opcuaserverdiscovery_p.h>

#include <QJSValue>
#include <QObject>
#include <QOpcUaClient>
#include <QOpcUaAuthenticationInformation>

#include <QtQml/qqml.h>

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
    QML_NAMED_ELEMENT(Connection)
    QML_ADDED_IN_VERSION(5, 12)

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

#endif // QOPCUA_OPCUACONNECTION_P_H

/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QOPCUACLIENT_H
#define QOPCUACLIENT_H

#include <QtOpcUa/qopcuaglobal.h>
#include <QtOpcUa/qopcuaapplicationidentity.h>
#include <QtOpcUa/qopcuapkiconfiguration.h>
#include <QtOpcUa/qopcuanode.h>
#include <QtOpcUa/qopcuareaditem.h>
#include <QtOpcUa/qopcuareadresult.h>
#include <QtOpcUa/qopcuawriteitem.h>
#include <QtOpcUa/qopcuawriteresult.h>
#include <QtOpcUa/qopcuaaddnodeitem.h>
#include <QtOpcUa/qopcuaaddreferenceitem.h>
#include <QtOpcUa/qopcuadeletereferenceitem.h>
#include <QtOpcUa/qopcuaendpointdescription.h>
#include <QtOpcUa/qopcuahistoryreadrawrequest.h>

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>

QT_BEGIN_NAMESPACE

class QOpcUaAuthenticationInformation;
class QOpcUaApplicationDescription;
class QOpcUaClientPrivate;
class QOpcUaClientImpl;
class QOpcUaErrorState;
class QOpcUaExpandedNodeId;
class QOpcUaQualifiedName;
class QOpcUaEndpointDescription;

class Q_OPCUA_EXPORT QOpcUaClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ClientState state READ state NOTIFY stateChanged)
    Q_PROPERTY(ClientError error READ error NOTIFY errorChanged)
    Q_DECLARE_PRIVATE(QOpcUaClient)

public:
    enum ClientState {
        Disconnected,
        Connecting,
        Connected,
        Closing
    };
    Q_ENUM(ClientState)

    enum ClientError {
        NoError,
        InvalidUrl,
        AccessDenied,
        ConnectionError,
        UnknownError,
        UnsupportedAuthenticationInformation
    };
    Q_ENUM(ClientError)

    explicit QOpcUaClient(QOpcUaClientImpl *impl, QObject *parent = nullptr);
    ~QOpcUaClient();

    void setApplicationIdentity(const QOpcUaApplicationIdentity &identity);
    QOpcUaApplicationIdentity applicationIdentity() const;

    void setPkiConfiguration(const QOpcUaPkiConfiguration &config);
    QOpcUaPkiConfiguration pkiConfiguration() const;

    Q_INVOKABLE void connectToEndpoint(const QOpcUaEndpointDescription &endpoint);
    Q_INVOKABLE void disconnectFromEndpoint();
    QOpcUaNode *node(const QString &nodeId);
    QOpcUaNode *node(const QOpcUaExpandedNodeId &expandedNodeId);

    bool updateNamespaceArray();
    QStringList namespaceArray() const;

    QString resolveExpandedNodeId(const QOpcUaExpandedNodeId &expandedNodeId, bool *ok = nullptr) const;
    QOpcUaQualifiedName qualifiedNameFromNamespaceUri(const QString &namespaceUri, const QString &name, bool *ok = nullptr) const;

    bool requestEndpoints(const QUrl &url);
    bool findServers(const QUrl &url, const QStringList &localeIds = QStringList(),
                     const QStringList &serverUris = QStringList());

    bool readNodeAttributes(const QList<QOpcUaReadItem> &nodesToRead);
    bool writeNodeAttributes(const QList<QOpcUaWriteItem> &nodesToWrite);

    bool addNode(const QOpcUaAddNodeItem &nodeToAdd);
    bool deleteNode(const QString &nodeId, bool deleteTargetReferences = true);

    bool addReference(const QOpcUaAddReferenceItem &referenceToAdd);
    bool deleteReference(const QOpcUaDeleteReferenceItem &referenceToDelete);

    QOpcUaEndpointDescription endpoint() const;

    ClientState state() const;
    ClientError error() const;

    QString backend() const;

    void setNamespaceAutoupdate(bool isEnabled);
    bool isNamespaceAutoupdateEnabled() const;
    void setNamespaceAutoupdateInterval(int interval);
    int namespaceAutoupdateInterval() const;

    void setAuthenticationInformation(const QOpcUaAuthenticationInformation &authenticationInformation);
    const QOpcUaAuthenticationInformation &authenticationInformation() const;

    QStringList supportedSecurityPolicies() const;
    QList<QOpcUaUserTokenPolicy::TokenType> supportedUserTokenTypes() const;

    QOpcUaHistoryReadResponse *readHistoryData(const QOpcUaHistoryReadRawRequest &request);

Q_SIGNALS:
    void connected();
    void disconnected();
    void stateChanged(QOpcUaClient::ClientState state);
    void errorChanged(QOpcUaClient::ClientError error);
    void connectError(QOpcUaErrorState *errorState);
    void namespaceArrayUpdated(QStringList namespaces);
    void namespaceArrayChanged(QStringList namespaces);
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
    void passwordForPrivateKeyRequired(QString keyFilePath, QString *password, bool previousTryWasInvalid);

private:
    Q_DISABLE_COPY(QOpcUaClient)
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaClient::ClientState)
Q_DECLARE_METATYPE(QOpcUaClient::ClientError)

#endif // QOPCUACLIENT_H

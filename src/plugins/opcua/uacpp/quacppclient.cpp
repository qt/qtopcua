// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "quacppbackend.h"
#include "quacppclient.h"
#include "quacppnode.h"
#include "quacpputils.h"

#include <private/qopcuaclient_p.h>

#include <QtCore/QLoggingCategory>
#include <QtCore/QStringList>
#include <QtCore/QThread>
#include <QtCore/QUrl>

#include <opcua_trace.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_UACPP)

QUACppClient::QUACppClient(const QVariantMap &backendProperties)
    : QOpcUaClientImpl()
    , m_backend(new UACppAsyncBackend(this))
{
    if (backendProperties.value(QLatin1String("disableEncryptedPasswordCheck"), false).toBool()) {
        qCDebug(QT_OPCUA_PLUGINS_UACPP) << "Disabling encrypted password check.";
        m_backend->m_disableEncryptedPasswordCheck = true;
    }

    if (backendProperties.value(QLatin1String("enableVerboseDebugOutput"), false).toBool()) {
        OpcUa_Trace_Initialize();
        OpcUa_Trace_ChangeTraceLevel(OPCUA_TRACE_OUTPUT_LEVEL_ALL);
        OpcUa_Trace_Toggle(true);
    }

    m_thread = new QThread();
    m_thread->setObjectName("QUaCppClient");
    connectBackendWithClient(m_backend);
    m_backend->moveToThread(m_thread);
    connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);
    connect(m_thread, &QThread::finished, m_backend, &QObject::deleteLater);
    m_thread->start();
}

QUACppClient::~QUACppClient()
{
    if (m_thread->isRunning())
        m_thread->quit();
}

void QUACppClient::connectToEndpoint(const QOpcUaEndpointDescription &endpoint)
{
    QMetaObject::invokeMethod(m_backend, "connectToEndpoint", Qt::QueuedConnection, Q_ARG(QOpcUaEndpointDescription, endpoint));
}

void QUACppClient::disconnectFromEndpoint()
{
    QMetaObject::invokeMethod(m_backend, "disconnectFromEndpoint", Qt::QueuedConnection);
}

QOpcUaNode *QUACppClient::node(const QString &nodeId)
{
    UaNodeId nativeId = UACppUtils::nodeIdFromQString(nodeId);
    if (nativeId.isNull())
        return nullptr;

    auto tempNode = new QUACppNode(nativeId, this, nodeId);
    if (!tempNode->registered()) {
        qCDebug(QT_OPCUA_PLUGINS_UACPP) << "Failed to register node with backend, maximum number of nodes reached.";
        delete tempNode;
        return nullptr;
    }
    return new QOpcUaNode(tempNode, m_client);
}

QString QUACppClient::backend() const
{
    return QStringLiteral("uacpp");
}

bool QUACppClient::requestEndpoints(const QUrl &url)
{
    return QMetaObject::invokeMethod(m_backend, "requestEndpoints", Qt::QueuedConnection, Q_ARG(QUrl, url));
}

bool QUACppClient::findServers(const QUrl &url, const QStringList &localeIds, const QStringList &serverUris)
{
    return QMetaObject::invokeMethod(m_backend, "findServers", Qt::QueuedConnection,
                                     Q_ARG(QUrl, url),
                                     Q_ARG(QStringList, localeIds),
                                     Q_ARG(QStringList, serverUris));
}

bool QUACppClient::readNodeAttributes(const QList<QOpcUaReadItem> &nodesToRead)
{
    return QMetaObject::invokeMethod(m_backend, "readNodeAttributes", Qt::QueuedConnection,
                                     Q_ARG(QList<QOpcUaReadItem>, nodesToRead));
}

bool QUACppClient::writeNodeAttributes(const QList<QOpcUaWriteItem> &nodesToWrite)
{
    return QMetaObject::invokeMethod(m_backend, "writeNodeAttributes", Qt::QueuedConnection,
                                     Q_ARG(QList<QOpcUaWriteItem>, nodesToWrite));
}

QOpcUaHistoryReadResponse *QUACppClient::readHistoryData(const QOpcUaHistoryReadRawRequest &request)
{
    Q_UNUSED(request)
    qCWarning(QT_OPCUA_PLUGINS_UACPP) << "readHistoryData not implemented";
    return nullptr;
}

bool QUACppClient::addNode(const QOpcUaAddNodeItem &nodeToAdd)
{
    return QMetaObject::invokeMethod(m_backend, "addNode", Qt::QueuedConnection,
                                     Q_ARG(QOpcUaAddNodeItem, nodeToAdd));
}

bool QUACppClient::deleteNode(const QString &nodeId, bool deleteTargetReferences)
{
    return QMetaObject::invokeMethod(m_backend, "deleteNode", Qt::QueuedConnection,
                                                 Q_ARG(QString, nodeId),
                                                 Q_ARG(bool, deleteTargetReferences));
}

bool QUACppClient::addReference(const QOpcUaAddReferenceItem &referenceToAdd)
{
    return QMetaObject::invokeMethod(m_backend, "addReference", Qt::QueuedConnection,
                                                 Q_ARG(QOpcUaAddReferenceItem, referenceToAdd));
}

bool QUACppClient::deleteReference(const QOpcUaDeleteReferenceItem &referenceToDelete)
{
    return QMetaObject::invokeMethod(m_backend, "deleteReference", Qt::QueuedConnection,
                                     Q_ARG(QOpcUaDeleteReferenceItem, referenceToDelete));
}

QStringList QUACppClient::supportedSecurityPolicies() const
{
    return QStringList {
#if OPCUA_SUPPORT_SECURITYPOLICY_BASIC128RSA15
        "http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15",
#endif
#if OPCUA_SUPPORT_SECURITYPOLICY_BASIC256
        "http://opcfoundation.org/UA/SecurityPolicy#Basic256",
#endif
#if OPCUA_SUPPORT_SECURITYPOLICY_BASIC256SHA256
        "http://opcfoundation.org/UA/SecurityPolicy#Basic256Sha256",
#endif
#if OPCUA_SUPPORT_SECURITYPOLICY_AES128SHA256RSAOAEP
        "http://opcfoundation.org/UA/SecurityPolicy#Aes128_Sha256_RsaOaep",
#endif
#if OPCUA_SUPPORT_SECURITYPOLICY_AES256SHA256RSAPSS
        "http://opcfoundation.org/UA/SecurityPolicy#Aes256_Sha256_RsaPss",
#endif
#if OPCUA_SUPPORT_SECURITYPOLICY_NONE
        "http://opcfoundation.org/UA/SecurityPolicy#None",
#endif
    };
}

QList<QOpcUaUserTokenPolicy::TokenType> QUACppClient::supportedUserTokenTypes() const
{
    return QList<QOpcUaUserTokenPolicy::TokenType> {
        QOpcUaUserTokenPolicy::TokenType::Certificate,
        QOpcUaUserTokenPolicy::TokenType::Username,
        QOpcUaUserTokenPolicy::TokenType::Anonymous
    };
}

QT_END_NAMESPACE

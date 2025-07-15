// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifdef USE_SYSTEM_OPEN62541
#include <open62541/client.h>
#include <open62541/client_config_default.h>
#else
#include "qopen62541.h"
#endif

#include "qopen62541backend.h"
#include "qopen62541client.h"
#include "qopen62541node.h"
#include "qopen62541subscription.h"
#include "qopen62541utils.h"
#include "qopen62541valueconverter.h"
#include <private/qopcuaclient_p.h>
#include <private/qopcuahistoryreadresponseimpl_p.h>
#include <private/qopcuasecuritypolicyuris_p.h>

#include <QtCore/qloggingcategory.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qthread.h>
#include <QtCore/qurl.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_OPEN62541)

using namespace Qt::Literals::StringLiterals;

QOpen62541Client::QOpen62541Client(const QVariantMap &backendProperties)
    : QOpcUaClientImpl()
    , m_backend(new Open62541AsyncBackend(this))
{
#ifdef UA_ENABLE_ENCRYPTION
    m_hasSha1SignatureSupport = Open62541Utils::checkSha1SignatureSupport();

    if (!m_hasSha1SignatureSupport)
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "SHA-1 signatures are not supported by OpenSSL"
                                              << "The security policies Basic128Rsa15 and Basic256 will not be available";
#endif

    bool ok = false;
    const quint32 clientIterateInterval = backendProperties.value(u"clientIterateIntervalMs"_s, 50)
            .toUInt(&ok);

    if (ok)
        m_backend->m_clientIterateInterval = clientIterateInterval;

    const quint32 asyncRequestTimeout = backendProperties.value(u"asyncRequestTimeoutMs"_s, 15000)
            .toUInt(&ok);

    if (ok)
        m_backend->m_asyncRequestTimeout = asyncRequestTimeout;

    m_thread = new QThread();
    m_thread->setObjectName("QOpen62541Client");
    connectBackendWithClient(m_backend);
    m_backend->moveToThread(m_thread);
    connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);
    connect(m_thread, &QThread::finished, m_backend, &QObject::deleteLater);
    m_thread->start();
}

QOpen62541Client::~QOpen62541Client()
{
    // The connectError() and passwordForPrivateKeyRequired() signals use a blocking queued connection.
    // They must be disconnected before waiting for the thread to avoid a deadlock.
    QObject::disconnect(m_backend, &Open62541AsyncBackend::connectError, this, &QOpcUaClientImpl::connectError);
    QObject::disconnect(m_backend, &Open62541AsyncBackend::passwordForPrivateKeyRequired,
                        this, &QOpcUaClientImpl::passwordForPrivateKeyRequired);

    if (m_thread->isRunning())
        m_thread->quit();

    m_thread->wait();
}

void QOpen62541Client::connectToEndpoint(const QOpcUaEndpointDescription &endpoint)
{
    QMetaObject::invokeMethod(m_backend, &Open62541AsyncBackend::connectToEndpoint, Qt::QueuedConnection, endpoint);
}

void QOpen62541Client::disconnectFromEndpoint()
{
    QMetaObject::invokeMethod(m_backend, &Open62541AsyncBackend::disconnectFromEndpoint, Qt::QueuedConnection);
}

QOpcUaNode *QOpen62541Client::node(const QString &nodeId)
{
    UA_NodeId uaNodeId = Open62541Utils::nodeIdFromQString(nodeId);
    if (UA_NodeId_isNull(&uaNodeId))
        return nullptr;

    auto tempNode = new QOpen62541Node(uaNodeId, this, nodeId);
    if (!tempNode->registered()) {
        qCDebug(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to register node with backend, maximum number of nodes reached.";
        delete tempNode;
        return nullptr;
    }
    return new QOpcUaNode(tempNode, m_client);
}

QString QOpen62541Client::backend() const
{
    return u"open62541"_s;
}

bool QOpen62541Client::requestEndpoints(const QUrl &url)
{
    return QMetaObject::invokeMethod(m_backend, &Open62541AsyncBackend::requestEndpoints,
                                     Qt::QueuedConnection, url);
}

bool QOpen62541Client::findServers(const QUrl &url, const QStringList &localeIds, const QStringList &serverUris)
{
   return QMetaObject::invokeMethod(m_backend, &Open62541AsyncBackend::findServers,
                                    Qt::QueuedConnection, url, localeIds, serverUris);
}

bool QOpen62541Client::readNodeAttributes(const QList<QOpcUaReadItem> &nodesToRead)
{
    return QMetaObject::invokeMethod(m_backend, &Open62541AsyncBackend::readNodeAttributes,
                                     Qt::QueuedConnection, nodesToRead);
}

bool QOpen62541Client::writeNodeAttributes(const QList<QOpcUaWriteItem> &nodesToWrite)
{
    return QMetaObject::invokeMethod(m_backend, &Open62541AsyncBackend::writeNodeAttributes,
                                     Qt::QueuedConnection, nodesToWrite);
}

QOpcUaHistoryReadResponse *QOpen62541Client::readHistoryData(const QOpcUaHistoryReadRawRequest &request)
{
    if (!m_client)
        return nullptr;

    auto impl = new QOpcUaHistoryReadResponseImpl(request);
    auto result = new QOpcUaHistoryReadResponse(impl);

    // Connect signals
    QObject::connect(m_backend, &QOpcUaBackend::historyDataAvailable, impl, &QOpcUaHistoryReadResponseImpl::handleDataAvailable);
    QObject::connect(impl, &QOpcUaHistoryReadResponseImpl::historyReadRawRequested, this, &QOpen62541Client::handleHistoryReadRawRequested);
    QObject::connect(this, &QOpen62541Client::historyReadRequestError, impl, &QOpcUaHistoryReadResponseImpl::handleRequestError);

    auto success = handleHistoryReadRawRequested(request, {}, false, impl->handle());

    if (!success) {
        delete result;
        return nullptr;
    }

    return result;
}

QOpcUaHistoryReadResponse *QOpen62541Client::readHistoryEvents(const QOpcUaHistoryReadEventRequest &request)
{
    if (!m_client)
        return nullptr;

    auto impl = new QOpcUaHistoryReadResponseImpl(request);
    const auto result = new QOpcUaHistoryReadResponse(impl);

    // Connect signals
    QObject::connect(m_backend, &QOpcUaBackend::historyEventsAvailable, impl, &QOpcUaHistoryReadResponseImpl::handleEventsAvailable);
    QObject::connect(impl, &QOpcUaHistoryReadResponseImpl::historyReadEventsRequested, this, &QOpen62541Client::handleHistoryReadEventsRequested);
    QObject::connect(this, &QOpen62541Client::historyReadRequestError, impl, &QOpcUaHistoryReadResponseImpl::handleRequestError);

    const auto success = handleHistoryReadEventsRequested(request, {}, false, impl->handle());

    if (!success) {
        delete result;
        return nullptr;
    }

    return result;
}

bool QOpen62541Client::addNode(const QOpcUaAddNodeItem &nodeToAdd)
{
    return QMetaObject::invokeMethod(m_backend, &Open62541AsyncBackend::addNode,
                                     Qt::QueuedConnection, nodeToAdd);
}

bool QOpen62541Client::deleteNode(const QString &nodeId, bool deleteTargetReferences)
{
    return QMetaObject::invokeMethod(m_backend, &Open62541AsyncBackend::deleteNode, Qt::QueuedConnection,
                                     nodeId, deleteTargetReferences);
}

bool QOpen62541Client::addReference(const QOpcUaAddReferenceItem &referenceToAdd)
{
    return QMetaObject::invokeMethod(m_backend, &Open62541AsyncBackend::addReference,
                                     Qt::QueuedConnection, referenceToAdd);
}

bool QOpen62541Client::deleteReference(const QOpcUaDeleteReferenceItem &referenceToDelete)
{
    return QMetaObject::invokeMethod(m_backend, &Open62541AsyncBackend::deleteReference,
                                     Qt::QueuedConnection, referenceToDelete);
}

QStringList QOpen62541Client::supportedSecurityPolicies() const
{
#ifdef UA_ENABLE_ENCRYPTION
    size_t numPolicies = m_hasSha1SignatureSupport ? 6 : 4;
#else
    size_t numPolicies = 1;
#endif

    QStringList result;
    result.reserve(numPolicies);
    result.append(QOpcUa::NonePolicy);

#ifdef UA_ENABLE_ENCRYPTION
    // Sort by strength
    if (m_hasSha1SignatureSupport) {
        result.append(QOpcUa::Basic128Rsa15Policy);
        result.append(QOpcUa::Basic256Policy);
    }
    result.append(QOpcUa::Aes128Sha256RsaOaepPolicy);
    result.append(QOpcUa::Basic256Sha256Policy);
    result.append(QOpcUa::Aes256Sha256RsaPssPolicy);
#endif

    return result;
}

QList<QOpcUaUserTokenPolicy::TokenType> QOpen62541Client::supportedUserTokenTypes() const
{
    return QList<QOpcUaUserTokenPolicy::TokenType> {
        QOpcUaUserTokenPolicy::TokenType::Anonymous,
        QOpcUaUserTokenPolicy::TokenType::Username
    };
}

bool QOpen62541Client::handleHistoryReadRawRequested(const QOpcUaHistoryReadRawRequest &request, const QList<QByteArray> &continuationPoints,
                                                     bool releaseContinuationPoints, quint64 handle)
{
    const auto success = QMetaObject::invokeMethod(m_backend, &Open62541AsyncBackend::readHistoryRaw,
                                                   Qt::QueuedConnection, request, continuationPoints,
                                                   releaseContinuationPoints, handle);

    if (!success)
        emit historyReadRequestError(handle);

    return success;
}

bool QOpen62541Client::registerNodes(const QStringList &nodesToRegister)
{
    return QMetaObject::invokeMethod(m_backend, &Open62541AsyncBackend::registerNodes,
                                     Qt::QueuedConnection, nodesToRegister);
}

bool QOpen62541Client::unregisterNodes(const QStringList &nodesToUnregister)
{
    return QMetaObject::invokeMethod(m_backend, &Open62541AsyncBackend::unregisterNodes,
                                     Qt::QueuedConnection, nodesToUnregister);
}

bool QOpen62541Client::handleHistoryReadEventsRequested(const QOpcUaHistoryReadEventRequest &request, const QList<QByteArray> &continuationPoints,
                                                     bool releaseContinuationPoints, quint64 handle)
{
    const auto success = QMetaObject::invokeMethod(m_backend, &Open62541AsyncBackend::readHistoryEvents,
                                                   Qt::QueuedConnection, request, continuationPoints,
                                                   releaseContinuationPoints, handle);

    if (!success)
        emit historyReadRequestError(handle);

    return success;
}

QT_END_NAMESPACE

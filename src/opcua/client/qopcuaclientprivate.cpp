// Copyright (C) 2016 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/qopcuaclient_p.h>

#include <QtCore/qloggingcategory.h>
#include <QtOpcUa/qopcuaendpointdescription.h>

#include "qopcuaerrorstate.h"

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA)

QOpcUaClientPrivate::QOpcUaClientPrivate(QOpcUaClientImpl *impl)
    : QObjectPrivate()
    , m_impl(impl)
    , m_state(QOpcUaClient::Disconnected)
    , m_error(QOpcUaClient::NoError)
    , m_enableNamespaceArrayAutoupdate(false)
    , m_authenticationInformation(QOpcUaAuthenticationInformation())
    , m_namespaceArrayAutoupdateEnabled(false)
    , m_namespaceArrayUpdateInterval(1000)
{
    // callback from client implementation
    QObject::connect(m_impl.data(), &QOpcUaClientImpl::stateAndOrErrorChanged,
                    [this](QOpcUaClient::ClientState state, QOpcUaClient::ClientError error) {
        setStateAndError(state, error);
        if (state == QOpcUaClient::ClientState::Connected) {
            updateNamespaceArray();
            setupNamespaceArrayMonitoring();
        }
    });

    QObject::connect(m_impl.data(), &QOpcUaClientImpl::endpointsRequestFinished, m_impl.data(),
                     [this](const QList<QOpcUaEndpointDescription> &e, QOpcUa::UaStatusCode s, const QUrl &requestUrl) {
        Q_Q(QOpcUaClient);
        emit q->endpointsRequestFinished(e, s, requestUrl);
    });

    QObject::connect(m_impl.data(), &QOpcUaClientImpl::findServersFinished, [this](const QList<QOpcUaApplicationDescription> &a, QOpcUa::UaStatusCode s, const QUrl &requestUrl) {
        Q_Q(QOpcUaClient);
        emit q->findServersFinished(a, s, requestUrl);
    });

    QObject::connect(m_impl.data(), &QOpcUaClientImpl::readNodeAttributesFinished, [this](const QList<QOpcUaReadResult> &results, QOpcUa::UaStatusCode serviceResult) {
        Q_Q(QOpcUaClient);
        emit q->readNodeAttributesFinished(results, serviceResult);
    });

    QObject::connect(m_impl.data(), &QOpcUaClientImpl::writeNodeAttributesFinished, [this](const QList<QOpcUaWriteResult> &results, QOpcUa::UaStatusCode serviceResult) {
        Q_Q(QOpcUaClient);
        emit q->writeNodeAttributesFinished(results, serviceResult);
    });

    QObject::connect(m_impl.data(), &QOpcUaClientImpl::addNodeFinished, [this](const QOpcUaExpandedNodeId &requestedNodeId, const QString &assignedNodeId, QOpcUa::UaStatusCode statusCode) {
        Q_Q(QOpcUaClient);
        emit q->addNodeFinished(requestedNodeId, assignedNodeId, statusCode);
    });

    QObject::connect(m_impl.data(), &QOpcUaClientImpl::deleteNodeFinished, [this](const QString &nodeId, QOpcUa::UaStatusCode statusCode) {
        Q_Q(QOpcUaClient);
        emit q->deleteNodeFinished(nodeId, statusCode);
    });

    QObject::connect(m_impl.data(), &QOpcUaClientImpl::addReferenceFinished, [this](const QString &sourceNodeId, const QString &referenceTypeId,
                     const QOpcUaExpandedNodeId &targetNodeId, bool isForwardReference, QOpcUa::UaStatusCode statusCode) {
        Q_Q(QOpcUaClient);
        emit q->addReferenceFinished(sourceNodeId, referenceTypeId, targetNodeId, isForwardReference, statusCode);
    });

    QObject::connect(m_impl.data(), &QOpcUaClientImpl::deleteReferenceFinished, [this](const QString &sourceNodeId, const QString &referenceTypeId,
                     const QOpcUaExpandedNodeId &targetNodeId, bool isForwardReference, QOpcUa::UaStatusCode statusCode) {
        Q_Q(QOpcUaClient);
        emit q->deleteReferenceFinished(sourceNodeId, referenceTypeId, targetNodeId, isForwardReference, statusCode);
    });

    QObject::connect(m_impl.data(), &QOpcUaClientImpl::connectError, [this](QOpcUaErrorState *errorState) {
        Q_Q(QOpcUaClient);
        emit q->connectError(errorState);
    });

    QObject::connect(m_impl.data(), &QOpcUaClientImpl::passwordForPrivateKeyRequired, [this](QString privateKeyFilePath, QString *password, bool previousTryWasInvalid) {
        Q_Q(QOpcUaClient);
        emit q->passwordForPrivateKeyRequired(privateKeyFilePath, password, previousTryWasInvalid);
    });
}

QOpcUaClientPrivate::~QOpcUaClientPrivate()
{
}

void QOpcUaClientPrivate::connectToEndpoint(const QOpcUaEndpointDescription &endpoint)
{
    // Some pre-connection checks
    if (QOpcUa::isSecurePolicy(endpoint.securityPolicy())) {
        // We are going to connect to a secure endpoint

        if (!m_pkiConfig.isPkiValid()) {
            qCWarning(QT_OPCUA) << "Can not connect to a secure endpoint without a valid PKI setup.";
            setStateAndError(m_state, QOpcUaClient::AccessDenied);
            return;
        }

        if (!m_pkiConfig.isKeyAndCertificateFileSet()) {
            qCWarning(QT_OPCUA) << "Can not connect to a secure endpoint without a client certificate.";
            setStateAndError(m_state, QOpcUaClient::AccessDenied);
            return;
        }
    }

    m_endpoint = endpoint;
    m_impl->connectToEndpoint(endpoint);
}

void QOpcUaClientPrivate::disconnectFromEndpoint()
{
    if (m_state != QOpcUaClient::Connected) {
        qCWarning(QT_OPCUA) << "Closing a connection without being connected";
        return;
    }

    setStateAndError(QOpcUaClient::Closing);
    m_impl->disconnectFromEndpoint();
}

void QOpcUaClientPrivate::setStateAndError(QOpcUaClient::ClientState state,
                                           QOpcUaClient::ClientError error)
{
     Q_Q(QOpcUaClient);

    // ensure that state and error transition are atomic before signal emission
    bool stateChanged = false;
    bool errorOccurred = false;

    if (m_state != state) {
        m_state = state;
        stateChanged = true;
    }
    if (error != QOpcUaClient::NoError && m_error != error) {
        errorOccurred = true;
    }
    m_error = error;

    if (errorOccurred)
        emit q->errorChanged(m_error);
    if (stateChanged) {
        emit q->stateChanged(m_state);

        if (m_state == QOpcUaClient::Connected)
            emit q->connected();
        else if (m_state == QOpcUaClient::Disconnected)
            emit q->disconnected();
    }

    // According to UPC-UA part 5, page 23, the server is allowed to change entries of the namespace
    // array if there is no active session. This could invalidate the cached namespaces table.
    if (state == QOpcUaClient::Disconnected) {
        m_namespaceArray.clear();
    }
}

bool QOpcUaClientPrivate::updateNamespaceArray()
{
    if (m_state != QOpcUaClient::ClientState::Connected)
        return false;

    if (!m_namespaceArrayNode) {
        m_namespaceArrayNode.reset(m_impl->node(QStringLiteral("ns=0;i=2255")));
        if (!m_namespaceArrayNode)
            return false;
        QObjectPrivate::connect(m_namespaceArrayNode.data(), &QOpcUaNode::attributeRead, this, &QOpcUaClientPrivate::namespaceArrayUpdated);
    }

    return m_namespaceArrayNode->readAttributes(QOpcUa::NodeAttribute::Value);
}

QStringList QOpcUaClientPrivate::namespaceArray() const
{
    return m_namespaceArray;
}

void QOpcUaClientPrivate::namespaceArrayUpdated(QOpcUa::NodeAttributes attr)
{
    Q_Q(QOpcUaClient);

    const QVariant value = m_namespaceArrayNode->attribute(QOpcUa::NodeAttribute::Value);

    if (!(attr & QOpcUa::NodeAttribute::Value) || value.metaType().id() != QMetaType::QVariantList) {
        m_namespaceArray.clear();
        emit q->namespaceArrayUpdated(QStringList());
        return;
    }

    QStringList updatedNamespaceArray;
    for (const auto &it : value.toList())
        updatedNamespaceArray.append(it.toString());

    if (updatedNamespaceArray != m_namespaceArray) {
        m_namespaceArray = updatedNamespaceArray;
        emit q->namespaceArrayChanged(m_namespaceArray);
    }
    emit q->namespaceArrayUpdated(m_namespaceArray);
}

void QOpcUaClientPrivate::setupNamespaceArrayMonitoring()
{
    Q_Q(QOpcUaClient);

    if (!m_namespaceArrayNode || m_state != QOpcUaClient::ClientState::Connected)
        return;

    if (!m_enableNamespaceArrayAutoupdate && m_namespaceArrayAutoupdateEnabled) {
        m_namespaceArrayNode->disableMonitoring(QOpcUa::NodeAttribute::Value);
        m_namespaceArrayAutoupdateEnabled = false;
        return;
    }

    if (m_enableNamespaceArrayAutoupdate && !m_namespaceArrayAutoupdateEnabled) {
        QOpcUaMonitoringParameters options;
        options.setSubscriptionType(QOpcUaMonitoringParameters::SubscriptionType::Exclusive);
        options.setMaxKeepAliveCount((std::numeric_limits<quint32>::max)() - 1);
        options.setPublishingInterval(m_namespaceArrayUpdateInterval);
        m_namespaceArrayAutoupdateEnabled = true;

        QObject::connect(m_namespaceArrayNode.data(), &QOpcUaNode::enableMonitoringFinished, q,
            [&] (QOpcUa::NodeAttribute, QOpcUa::UaStatusCode statusCode) {
                if (statusCode == QOpcUa::Good) {
                    // Update interval member to the revised value from the server
                    m_namespaceArrayUpdateInterval = m_namespaceArrayNode->monitoringStatus(QOpcUa::NodeAttribute::Value).publishingInterval();
                } else {
                    m_namespaceArrayAutoupdateEnabled = m_enableNamespaceArrayAutoupdate = false;
                }
            }
        );
        QObject::connect(m_namespaceArrayNode.data(), &QOpcUaNode::attributeUpdated, q,
            [&] (QOpcUa::NodeAttribute attr, QVariant /*value*/) {
                namespaceArrayUpdated(attr);
            }
        );

        m_namespaceArrayNode->enableMonitoring(QOpcUa::NodeAttribute::Value, options);
    }
}

void QOpcUaClientPrivate::setApplicationIdentity(const QOpcUaApplicationIdentity &identity)
{
    m_applicationIdentity = identity;
}

QOpcUaApplicationIdentity QOpcUaClientPrivate::applicationIdentity() const
{
    return m_applicationIdentity;
}

void QOpcUaClientPrivate::setPkiConfiguration(const QOpcUaPkiConfiguration &config)
{
    m_pkiConfig = config;
}

QOpcUaPkiConfiguration QOpcUaClientPrivate::pkiConfiguration() const
{
    return m_pkiConfig;
}

QT_END_NAMESPACE

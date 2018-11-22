/****************************************************************************
**
** Copyright (C) 2016 basysKom GmbH, opensource@basyskom.com
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
                     [this](const QVector<QOpcUaEndpointDescription> &e, QOpcUa::UaStatusCode s) {
        Q_Q(QOpcUaClient);
        emit q->endpointsRequestFinished(e, s);
    });

    QObject::connect(m_impl.data(), &QOpcUaClientImpl::findServersFinished, [this](const QVector<QOpcUaApplicationDescription> &a, QOpcUa::UaStatusCode s) {
        Q_Q(QOpcUaClient);
        emit q->findServersFinished(a, s);
    });

    QObject::connect(m_impl.data(), &QOpcUaClientImpl::readNodeAttributesFinished, [this](const QVector<QOpcUaReadResult> &results, QOpcUa::UaStatusCode serviceResult) {
        Q_Q(QOpcUaClient);
        emit q->readNodeAttributesFinished(results, serviceResult);
    });

    QObject::connect(m_impl.data(), &QOpcUaClientImpl::writeNodeAttributesFinished, [this](const QVector<QOpcUaWriteResult> &results, QOpcUa::UaStatusCode serviceResult) {
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
}

QOpcUaClientPrivate::~QOpcUaClientPrivate()
{
}

void QOpcUaClientPrivate::connectToEndpoint(const QOpcUaEndpointDescription &endpoint)
{
    m_endpoint = endpoint;
    setStateAndError(QOpcUaClient::Connecting);
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

    if (!(attr & QOpcUa::NodeAttribute::Value) || value.type() != QVariant::Type::List) {
        m_namespaceArray.clear();
        emit q->namespaceArrayUpdated(QStringList());
        return;
    }

    QStringList updatedNamespaceArray;
    for (auto it : value.toList())
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

void QOpcUaClientPrivate::setIdentity(const QOpcUaApplicationIdentity &identity)
{
    m_identity = identity;
}

QOpcUaApplicationIdentity QOpcUaClientPrivate::identity() const
{
    return m_identity;
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

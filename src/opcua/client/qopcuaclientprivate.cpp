// Copyright (C) 2016 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/qopcuaclient_p.h>

#include <QtCore/qloggingcategory.h>
#include <QtOpcUa/qopcuaendpointdescription.h>

#include "qopcuaerrorstate.h"

QT_BEGIN_NAMESPACE

using namespace Qt::Literals::StringLiterals;

QOpcUaClientPrivate::QOpcUaClientPrivate(QOpcUaClientImpl *impl)
    : QObjectPrivate()
    , m_impl(impl)
    , m_state(QOpcUaClient::Disconnected)
    , m_error(QOpcUaClient::NoError)
    , m_enableNamespaceArrayAutoupdate(false)
    , m_namespaceArrayAutoupdateEnabled(false)
    , m_namespaceArrayUpdateInterval(1000)
{
}

QOpcUaClientPrivate::~QOpcUaClientPrivate()
{
}

void QOpcUaClientPrivate::connectToEndpoint(const QOpcUaEndpointDescription &endpoint)
{
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
        m_namespaceArrayNode.reset(m_impl->node(u"ns=0;i=2255"_s));
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

    QStringList updatedNamespaceArray(value.toList().size());
    int index = 0;
    const auto list = value.toList();
    for (const auto &it : list)
        updatedNamespaceArray[index++] = (it.toString());

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

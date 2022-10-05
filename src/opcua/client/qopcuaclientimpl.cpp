// Copyright (C) 2016 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/qopcuabackend_p.h>
#include <private/qopcuaclientimpl_p.h>
#include <QtOpcUa/qopcuamonitoringparameters.h>
#include "qopcuaclient_p.h"
#include "qopcuaerrorstate.h"

QT_BEGIN_NAMESPACE

QOpcUaClientImpl::QOpcUaClientImpl(QObject *parent)
    : QObject(parent)
    , m_client(nullptr)
    , m_handleCounter(0)
{}

QOpcUaClientImpl::~QOpcUaClientImpl()
{}

bool QOpcUaClientImpl::registerNode(QPointer<QOpcUaNodeImpl> obj)
{
    if (m_handles.size() == (std::numeric_limits<int>::max)())
        return false;

    while (true) {
        ++m_handleCounter;

        if (!m_handles.contains(m_handleCounter)) {
            obj->setHandle(m_handleCounter);
            m_handles[m_handleCounter] = obj;
            return true;
        }
    }
}

void QOpcUaClientImpl::unregisterNode(QPointer<QOpcUaNodeImpl> obj)
{
    m_handles.remove(obj->handle());
}

void QOpcUaClientImpl::connectBackendWithClient(QOpcUaBackend *backend)
{
    connect(backend, &QOpcUaBackend::attributesRead, this, &QOpcUaClientImpl::handleAttributesRead);
    connect(backend, &QOpcUaBackend::stateAndOrErrorChanged, this, &QOpcUaClientImpl::stateAndOrErrorChanged);
    connect(backend, &QOpcUaBackend::attributeWritten, this, &QOpcUaClientImpl::handleAttributeWritten);
    connect(backend, &QOpcUaBackend::dataChangeOccurred, this, &QOpcUaClientImpl::handleDataChangeOccurred);
    connect(backend, &QOpcUaBackend::monitoringEnableDisable, this, &QOpcUaClientImpl::handleMonitoringEnableDisable);
    connect(backend, &QOpcUaBackend::monitoringStatusChanged, this, &QOpcUaClientImpl::handleMonitoringStatusChanged);
    connect(backend, &QOpcUaBackend::methodCallFinished, this, &QOpcUaClientImpl::handleMethodCallFinished);
    connect(backend, &QOpcUaBackend::browseFinished, this, &QOpcUaClientImpl::handleBrowseFinished);
    connect(backend, &QOpcUaBackend::resolveBrowsePathFinished, this, &QOpcUaClientImpl::handleResolveBrowsePathFinished);
    connect(backend, &QOpcUaBackend::eventOccurred, this, &QOpcUaClientImpl::handleNewEvent);
    connect(backend, &QOpcUaBackend::endpointsRequestFinished, this, &QOpcUaClientImpl::endpointsRequestFinished);
    connect(backend, &QOpcUaBackend::findServersFinished, this, &QOpcUaClientImpl::findServersFinished);
    connect(backend, &QOpcUaBackend::readNodeAttributesFinished, this, &QOpcUaClientImpl::readNodeAttributesFinished);
    connect(backend, &QOpcUaBackend::writeNodeAttributesFinished, this, &QOpcUaClientImpl::writeNodeAttributesFinished);
    connect(backend, &QOpcUaBackend::addNodeFinished, this, &QOpcUaClientImpl::addNodeFinished);
    connect(backend, &QOpcUaBackend::deleteNodeFinished, this, &QOpcUaClientImpl::deleteNodeFinished);
    connect(backend, &QOpcUaBackend::addReferenceFinished, this, &QOpcUaClientImpl::addReferenceFinished);
    connect(backend, &QOpcUaBackend::deleteReferenceFinished, this, &QOpcUaClientImpl::deleteReferenceFinished);
    // This needs to be blocking queued because it is called from another thread, which needs to wait for a result.
    connect(backend, &QOpcUaBackend::connectError, this, &QOpcUaClientImpl::connectError, Qt::BlockingQueuedConnection);
    connect(backend, &QOpcUaBackend::passwordForPrivateKeyRequired, this, &QOpcUaClientImpl::passwordForPrivateKeyRequired, Qt::BlockingQueuedConnection);
}

void QOpcUaClientImpl::handleAttributesRead(quint64 handle, QList<QOpcUaReadResult> attr, QOpcUa::UaStatusCode serviceResult)
{
    auto it = m_handles.constFind(handle);
    if (it != m_handles.constEnd() && !it->isNull())
        emit (*it)->attributesRead(attr, serviceResult);
}

void QOpcUaClientImpl::handleAttributeWritten(quint64 handle, QOpcUa::NodeAttribute attr, const QVariant &value, QOpcUa::UaStatusCode statusCode)
{
    auto it = m_handles.constFind(handle);
    if (it != m_handles.constEnd() && !it->isNull())
        emit (*it)->attributeWritten(attr, value, statusCode);
}

void QOpcUaClientImpl::handleDataChangeOccurred(quint64 handle, const QOpcUaReadResult &value)
{
    auto it = m_handles.constFind(handle);
    if (it != m_handles.constEnd() && !it->isNull())
        emit (*it)->dataChangeOccurred(value.attribute(), value);
}

void QOpcUaClientImpl::handleMonitoringEnableDisable(quint64 handle, QOpcUa::NodeAttribute attr, bool subscribe, QOpcUaMonitoringParameters status)
{
    auto it = m_handles.constFind(handle);
    if (it != m_handles.constEnd() && !it->isNull())
        emit (*it)->monitoringEnableDisable(attr, subscribe, status);
}

void QOpcUaClientImpl::handleMonitoringStatusChanged(quint64 handle, QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameters items, QOpcUaMonitoringParameters param)
{
    auto it = m_handles.constFind(handle);
    if (it != m_handles.constEnd() && !it->isNull())
        emit (*it)->monitoringStatusChanged(attr, items, param);
}

void QOpcUaClientImpl::handleMethodCallFinished(quint64 handle, QString methodNodeId, QVariant result, QOpcUa::UaStatusCode statusCode)
{
    auto it = m_handles.constFind(handle);
    if (it != m_handles.constEnd() && !it->isNull())
        emit (*it)->methodCallFinished(methodNodeId, result, statusCode);
}

void QOpcUaClientImpl::handleBrowseFinished(quint64 handle, const QList<QOpcUaReferenceDescription> &children, QOpcUa::UaStatusCode statusCode)
{
    auto it = m_handles.constFind(handle);
    if (it != m_handles.constEnd() && !it->isNull())
        emit (*it)->browseFinished(children, statusCode);
}

void QOpcUaClientImpl::handleResolveBrowsePathFinished(quint64 handle, QList<QOpcUaBrowsePathTarget> targets,
                                                       QList<QOpcUaRelativePathElement> path, QOpcUa::UaStatusCode status)
{
    auto it = m_handles.constFind(handle);
    if (it != m_handles.constEnd() && !it->isNull())
        emit (*it)->resolveBrowsePathFinished(targets, path, status);
}

void QOpcUaClientImpl::handleNewEvent(quint64 handle, QVariantList eventFields)
{
    auto it = m_handles.constFind(handle);
    if (it != m_handles.constEnd() && !it->isNull())
        emit (*it)->eventOccurred(eventFields);
}

QT_END_NAMESPACE

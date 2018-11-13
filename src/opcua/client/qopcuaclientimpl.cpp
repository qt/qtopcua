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
    if (m_handles.count() == (std::numeric_limits<int>::max)())
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
}

void QOpcUaClientImpl::handleAttributesRead(quint64 handle, QVector<QOpcUaReadResult> attr, QOpcUa::UaStatusCode serviceResult)
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

void QOpcUaClientImpl::handleBrowseFinished(quint64 handle, const QVector<QOpcUaReferenceDescription> &children, QOpcUa::UaStatusCode statusCode)
{
    auto it = m_handles.constFind(handle);
    if (it != m_handles.constEnd() && !it->isNull())
        emit (*it)->browseFinished(children, statusCode);
}

void QOpcUaClientImpl::handleResolveBrowsePathFinished(quint64 handle, QVector<QOpcUa::QBrowsePathTarget> targets,
                                                         QVector<QOpcUa::QRelativePathElement> path, QOpcUa::UaStatusCode status)
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

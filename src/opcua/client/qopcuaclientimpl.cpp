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

QT_BEGIN_NAMESPACE

QOpcUaClientImpl::QOpcUaClientImpl(QObject *parent)
    : QObject(parent)
{}

QOpcUaClientImpl::~QOpcUaClientImpl()
{}

void QOpcUaClientImpl::registerNode(QPointer<QOpcUaNodeImpl> obj)
{
    m_handles[reinterpret_cast<uintptr_t>(obj.data())] = obj;
}

void QOpcUaClientImpl::unregisterNode(QPointer<QOpcUaNodeImpl> obj)
{
    m_handles.remove(reinterpret_cast<uintptr_t>(obj.data()));
}

void QOpcUaClientImpl::connectBackendWithClient(QOpcUaBackend *backend)
{
    connect(backend, &QOpcUaBackend::attributesRead, this, &QOpcUaClientImpl::handleAttributesRead);
    connect(backend, &QOpcUaBackend::stateAndOrErrorChanged, this, &QOpcUaClientImpl::stateAndOrErrorChanged);
    connect(backend, &QOpcUaBackend::attributeWritten, this, &QOpcUaClientImpl::handleAttributeWritten);
    connect(backend, &QOpcUaBackend::attributeUpdated, this, &QOpcUaClientImpl::handleAttributeUpdated);
    connect(backend, &QOpcUaBackend::monitoringEnableDisable, this, &QOpcUaClientImpl::handleMonitoringEnableDisable);
    connect(backend, &QOpcUaBackend::monitoringStatusChanged, this, &QOpcUaClientImpl::handleMonitoringStatusChanged);
    connect(backend, &QOpcUaBackend::methodCallFinished, this, &QOpcUaClientImpl::handleMethodCallFinished);
}

void QOpcUaClientImpl::handleAttributesRead(uintptr_t handle, QVector<QOpcUaReadResult> attr, QOpcUa::UaStatusCode serviceResult)
{
    auto it = m_handles.constFind(handle);
    if (it != m_handles.constEnd() && !it->isNull())
        emit (*it)->attributesRead(attr, serviceResult);
}

void QOpcUaClientImpl::handleAttributeWritten(uintptr_t handle, QOpcUaNode::NodeAttribute attr, const QVariant &value, QOpcUa::UaStatusCode statusCode)
{
    auto it = m_handles.constFind(handle);
    if (it != m_handles.constEnd() && !it->isNull())
        emit (*it)->attributeWritten(attr, value, statusCode);
}

void QOpcUaClientImpl::handleAttributeUpdated(uintptr_t handle, QOpcUaNode::NodeAttribute attr, const QVariant &value)
{
    auto it = m_handles.constFind(handle);
    if (it != m_handles.constEnd() && !it->isNull())
        emit (*it)->attributeUpdated(attr, value);
}

void QOpcUaClientImpl::handleMonitoringEnableDisable(uintptr_t handle, QOpcUaNode::NodeAttribute attr, bool subscribe, QOpcUaMonitoringParameters status)
{
    auto it = m_handles.constFind(handle);
    if (it != m_handles.constEnd() && !it->isNull())
        emit (*it)->monitoringEnableDisable(attr, subscribe, status);
}

void QOpcUaClientImpl::handleMonitoringStatusChanged(uintptr_t handle, QOpcUaNode::NodeAttribute attr, QOpcUaMonitoringParameters::Parameters items, QOpcUaMonitoringParameters param)
{
    auto it = m_handles.constFind(handle);
    if (it != m_handles.constEnd() && !it->isNull())
        emit (*it)->monitoringStatusChanged(attr, items, param);
}

void QOpcUaClientImpl::handleMethodCallFinished(uintptr_t handle, QString methodNodeId, QVariant result, QOpcUa::UaStatusCode statusCode)
{
    auto it = m_handles.constFind(handle);
    if (it != m_handles.constEnd() && !it->isNull())
        emit (*it)->methodCallFinished(methodNodeId, result, statusCode);
}

QT_END_NAMESPACE

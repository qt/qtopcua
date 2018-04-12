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

#ifndef QOPCUANODE_P_H
#define QOPCUANODE_P_H

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

#include <QtOpcUa/qopcuaclient.h>
#include <QtOpcUa/qopcuanode.h>
#include <private/qopcuanodeimpl_p.h>

#include <private/qobject_p.h>
#include <QtCore/qpointer.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qhash.h>

QT_BEGIN_NAMESPACE

class QOpcUaNodePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QOpcUaNode)

public:
    QOpcUaNodePrivate(QOpcUaNodeImpl *impl, QOpcUaClient *client)
        : m_impl(impl)
        , m_client(client)
    {
        m_attributesReadConnection = QObject::connect(impl, &QOpcUaNodeImpl::attributesRead,
                [this](QVector<QOpcUaReadResult> attr, QOpcUa::UaStatusCode serviceResult)
        {
            QOpcUa::NodeAttributes updatedAttributes;

            for (auto &entry : qAsConst(attr)) {
                if (serviceResult == QOpcUa::UaStatusCode::Good)
                    m_nodeAttributes[entry.attribute()] = entry;
                else {
                    QOpcUaReadResult temp = entry;
                    temp.setStatusCode(serviceResult);
                    temp.setValue(QVariant());
                    m_nodeAttributes[entry.attribute()] = temp;
                }

                updatedAttributes |= entry.attribute();
            }

            Q_Q(QOpcUaNode);
            emit q->attributeRead(updatedAttributes);
        });

        m_attributeWrittenConnection = QObject::connect(impl, &QOpcUaNodeImpl::attributeWritten,
                [this](QOpcUa::NodeAttribute attr, QVariant value, QOpcUa::UaStatusCode statusCode)
        {
            m_nodeAttributes[attr].setStatusCode(statusCode);
            if (statusCode == QOpcUa::UaStatusCode::Good)
                m_nodeAttributes[attr].setValue(value);

            Q_Q(QOpcUaNode);
            emit q->attributeWritten(attr, statusCode);
        });

        m_attributeUpdatedConnection = QObject::connect(impl, &QOpcUaNodeImpl::attributeUpdated,
                [this](QOpcUa::NodeAttribute attr, QOpcUaReadResult value)
        {
            this->m_nodeAttributes[attr] = value;
            Q_Q(QOpcUaNode);
            emit q->attributeUpdated(attr, value.value());
        });

        m_monitoringEnableDisableConnection = QObject::connect(impl, &QOpcUaNodeImpl::monitoringEnableDisable,
                [this](QOpcUa::NodeAttribute attr, bool subscribe, QOpcUaMonitoringParameters status)
        {
            if (subscribe == true) {
                if (status.statusCode() != QOpcUa::UaStatusCode::BadEntryExists) // Don't overwrite a valid entry
                    m_monitoringStatus[attr] = status;
                Q_Q(QOpcUaNode);
                emit q->enableMonitoringFinished(attr, status.statusCode());
            }
            else {
                m_monitoringStatus.remove(attr);
                Q_Q(QOpcUaNode);
                emit q->disableMonitoringFinished(attr, status.statusCode());
            }
        });

        m_monitoringStatusChangedConnection = QObject::connect(impl, &QOpcUaNodeImpl::monitoringStatusChanged,
                [this](QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameters items, QOpcUaMonitoringParameters param)
        {
            auto it = m_monitoringStatus.find(attr);
            if (param.statusCode() == QOpcUa::UaStatusCode::Good && it != m_monitoringStatus.end()) {
                if (items & QOpcUaMonitoringParameters::Parameter::PublishingEnabled)
                    it->setPublishingEnabled(param.publishingEnabled());
                if (items & QOpcUaMonitoringParameters::Parameter::PublishingInterval)
                    it->setPublishingInterval(param.publishingInterval());
                if (items & QOpcUaMonitoringParameters::Parameter::LifetimeCount)
                    it->setLifetimeCount(param.lifetimeCount());
                if (items & QOpcUaMonitoringParameters::Parameter::MaxKeepAliveCount)
                    it->setMaxKeepAliveCount(param.maxKeepAliveCount());
                if (items & QOpcUaMonitoringParameters::Parameter::MaxNotificationsPerPublish)
                    it->setMaxNotificationsPerPublish(param.maxNotificationsPerPublish());
                if (items & QOpcUaMonitoringParameters::Parameter::Priority)
                    it->setPriority(param.priority());
                if (items & QOpcUaMonitoringParameters::Parameter::SamplingInterval)
                    it->setSamplingInterval(param.samplingInterval());
                if (items & QOpcUaMonitoringParameters::Parameter::Filter)
                    it->setFilter(param.filter());
                if (items & QOpcUaMonitoringParameters::Parameter::QueueSize)
                    it->setQueueSize(param.queueSize());
                if (items & QOpcUaMonitoringParameters::Parameter::DiscardOldest)
                    it->setDiscardOldest(param.discardOldest());
                if (items & QOpcUaMonitoringParameters::Parameter::MonitoringMode)
                    it->setMonitoringMode(param.monitoringMode());
            }

            Q_Q(QOpcUaNode);
            emit q->monitoringStatusChanged(attr, items, param.statusCode());
        });


        m_methodCallFinishedConnection = QObject::connect(impl, &QOpcUaNodeImpl::methodCallFinished,
            [this](QString methodNodeId, QVariant result, QOpcUa::UaStatusCode statusCode)
        {
            Q_Q(QOpcUaNode);
            emit q->methodCallFinished(methodNodeId, result, statusCode);
        });

        m_browseFinishedConnection = QObject::connect(impl, &QOpcUaNodeImpl::browseFinished,
                [this](QVector<QOpcUaReferenceDescription> children, QOpcUa::UaStatusCode statusCode)
        {
            Q_Q(QOpcUaNode);
            emit q->browseFinished(children, statusCode);
        });

        m_resolveBrowsePathFinishedConnection = QObject::connect(impl, &QOpcUaNodeImpl::resolveBrowsePathFinished,
                [this](QVector<QOpcUa::QBrowsePathTarget> targets, QVector<QOpcUa::QRelativePathElement> path,
                                                                   QOpcUa::UaStatusCode statusCode)
        {
            Q_Q(QOpcUaNode);
            emit q->resolveBrowsePathFinished(targets, path, statusCode);
        });

        m_eventOccurredConnection = QObject::connect(impl, &QOpcUaNodeImpl::eventOccurred,
            [this](QVariantList eventFields)
        {
            Q_Q(QOpcUaNode);
            emit q->eventOccurred(eventFields);
        });
    }

    ~QOpcUaNodePrivate()
    {
        QObject::disconnect(m_attributesReadConnection);
        QObject::disconnect(m_attributeWrittenConnection);
        QObject::disconnect(m_attributeUpdatedConnection);
        QObject::disconnect(m_monitoringEnableDisableConnection);
        QObject::disconnect(m_monitoringStatusChangedConnection);
        QObject::disconnect(m_methodCallFinishedConnection);
        QObject::disconnect(m_browseFinishedConnection);
        QObject::disconnect(m_resolveBrowsePathFinishedConnection);
        QObject::disconnect(m_eventOccurredConnection);

        // Disable remaining monitorings
        QOpcUa::NodeAttributes attr;
        for (auto it = m_monitoringStatus.constBegin(); it != m_monitoringStatus.constEnd(); ++it) {
            if (it->statusCode() == QOpcUa::UaStatusCode::Good)
                attr |= it.key();
        }
        if (attr != 0 && m_impl) {
            m_impl->disableMonitoring(attr);
        }
    }

    QScopedPointer<QOpcUaNodeImpl> m_impl;
    QPointer<QOpcUaClient> m_client;

    QHash<QOpcUa::NodeAttribute, QOpcUaReadResult> m_nodeAttributes;
    QHash<QOpcUa::NodeAttribute, QOpcUaMonitoringParameters> m_monitoringStatus;

    QMetaObject::Connection m_attributesReadConnection;
    QMetaObject::Connection m_attributeWrittenConnection;
    QMetaObject::Connection m_attributeUpdatedConnection;
    QMetaObject::Connection m_monitoringEnableDisableConnection;
    QMetaObject::Connection m_monitoringStatusChangedConnection;
    QMetaObject::Connection m_methodCallFinishedConnection;
    QMetaObject::Connection m_browseFinishedConnection;
    QMetaObject::Connection m_resolveBrowsePathFinishedConnection;
    QMetaObject::Connection m_eventOccurredConnection;
};

QT_END_NAMESPACE

#endif // QOPCUANODE_P_H

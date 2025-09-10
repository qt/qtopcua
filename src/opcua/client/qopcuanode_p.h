// Copyright (C) 2016 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
#include <QtOpcUa/qopcuaeventfilterresult.h>
#include <private/qopcuanodeimpl_p.h>

#include <private/qobject_p.h>
#include <QtCore/qpointer.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qhash.h>

#include <array>

QT_BEGIN_NAMESPACE

class QOpcUaNodePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QOpcUaNode)

public:
    QOpcUaNodePrivate(QOpcUaNodeImpl *impl, QOpcUaClient *client)
        : m_impl(impl)
        , m_client(client)
    {
    }

    void createConnections()
    {
        Q_Q(QOpcUaNode);

        size_t index = 0;
        m_connections[index++] = QObject::connect(m_impl.get(), &QOpcUaNodeImpl::attributesRead,
                q, [this](QList<QOpcUaReadResult> attr, QOpcUa::UaStatusCode serviceResult)
        {
            handleAttributesRead(attr, serviceResult);
        });

        m_connections[index++] = QObject::connect(m_impl.get(), &QOpcUaNodeImpl::attributeWritten,
                q, [this](QOpcUa::NodeAttribute attr, QVariant value, QOpcUa::UaStatusCode statusCode)
        {
            handleAttributesWritten(attr, value, statusCode);
        });

        m_connections[index++] = QObject::connect(m_impl.get(), &QOpcUaNodeImpl::dataChangeOccurred,
                q, [this](QOpcUa::NodeAttribute attr, QOpcUaReadResult value)
        {
            this->m_nodeAttributes[attr] = value;
            Q_Q(QOpcUaNode);
            emit q->dataChangeOccurred(attr, value.value());
            emit q->attributeUpdated(attr, value.value());

            if (attr == QOpcUa::NodeAttribute::Value)
                emit q->valueAttributeUpdated(value.value());
        });

        m_connections[index++] = QObject::connect(m_impl.get(), &QOpcUaNodeImpl::monitoringEnableDisable,
               q, [this](QOpcUa::NodeAttribute attr, bool subscribe, QOpcUaMonitoringParameters status)
        {
            handleMonitoringEnableDisable(attr, subscribe, status);
        });

        m_connections[index++] = QObject::connect(m_impl.get(), &QOpcUaNodeImpl::monitoringStatusChanged,
                q, [this](QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameters items, QOpcUaMonitoringParameters param)
        {
            handleMonitoringStatusChange(attr, items, param);
        });

        m_connections[index++] =
                QObject::connect(m_impl.get(), &QOpcUaNodeImpl::methodCallFinished, q,
                                 &QOpcUaNode::methodCallFinished);

        m_connections[index++] = QObject::connect(m_impl.get(), &QOpcUaNodeImpl::browseFinished,
                                                      q, &QOpcUaNode::browseFinished);

        m_connections[index++] =
                QObject::connect(m_impl.get(), &QOpcUaNodeImpl::resolveBrowsePathFinished, q,
                                 &QOpcUaNode::resolveBrowsePathFinished);

        m_connections[index++] = QObject::connect(m_impl.get(), &QOpcUaNodeImpl::eventOccurred,
                                                  q, &QOpcUaNode::eventOccurred);

        Q_ASSERT(index == m_connections.size());
    }

    ~QOpcUaNodePrivate()
    {
        for (auto &c : m_connections)
            QObject::disconnect(c);

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

    void handleAttributesRead(const QList<QOpcUaReadResult> &attr,
                              QOpcUa::UaStatusCode serviceResult)
    {
        QOpcUa::NodeAttributes updatedAttributes;
        Q_Q(QOpcUaNode);

        for (auto &entry : std::as_const(attr)) {
            if (serviceResult == QOpcUa::UaStatusCode::Good)
                m_nodeAttributes.insert(entry.attribute(), entry);
            else {
                QOpcUaReadResult temp = entry;
                temp.setStatusCode(serviceResult);
                temp.setValue(QVariant());
                m_nodeAttributes.insert(entry.attribute(), temp);
            }

            updatedAttributes |= entry.attribute();
            emit q->attributeUpdated(entry.attribute(), entry.value());

            if (entry.attribute() == QOpcUa::NodeAttribute::Value)
                emit q->valueAttributeUpdated(entry.value());
        }

        emit q->attributeRead(updatedAttributes);
    }

    void handleAttributesWritten(QOpcUa::NodeAttribute attr, const QVariant &value,
                                 QOpcUa::UaStatusCode statusCode)
    {
        m_nodeAttributes[attr].setStatusCode(statusCode);
        Q_Q(QOpcUaNode);

        if (statusCode == QOpcUa::UaStatusCode::Good) {
            m_nodeAttributes[attr].setValue(value);
            emit q->attributeUpdated(attr, value);

            if (attr == QOpcUa::NodeAttribute::Value)
                emit q->valueAttributeUpdated(value);
        }

        emit q->attributeWritten(attr, statusCode);
    }

    void handleMonitoringEnableDisable(QOpcUa::NodeAttribute attr, bool subscribe,
                                       const QOpcUaMonitoringParameters &status)
    {
        if (subscribe == true) {
            if (status.statusCode() != QOpcUa::UaStatusCode::BadEntryExists) // Don't overwrite a valid entry
                m_monitoringStatus.insert(attr, status);
            Q_Q(QOpcUaNode);
            emit q->enableMonitoringFinished(attr, status.statusCode());
        }
        else {
            m_monitoringStatus.remove(attr);
            Q_Q(QOpcUaNode);
            emit q->disableMonitoringFinished(attr, status.statusCode());
        }
    }

    void handleMonitoringStatusChange(QOpcUa::NodeAttribute attr,
                                      QOpcUaMonitoringParameters::Parameters items,
                                      const QOpcUaMonitoringParameters &param)
    {
        auto it = m_monitoringStatus.find(attr);
        if (param.statusCode() == QOpcUa::UaStatusCode::Good && it != m_monitoringStatus.end()) {
            if (items & QOpcUaMonitoringParameters::Parameter::PublishingEnabled)
                it->setPublishingEnabled(param.isPublishingEnabled());
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
            if (items & QOpcUaMonitoringParameters::Parameter::Filter) {
                if (param.filter().canConvert<QOpcUaMonitoringParameters::DataChangeFilter>())
                    it->setFilter(param.filter().value<QOpcUaMonitoringParameters::DataChangeFilter>());
                else if (param.filter().canConvert<QOpcUaMonitoringParameters::EventFilter>())
                    it->setFilter(param.filter().value<QOpcUaMonitoringParameters::EventFilter>());
                else if (param.filter().isNull())
                    it->clearFilter();
                if (param.filterResult().canConvert<QOpcUaEventFilterResult>())
                    it->setFilterResult(param.filterResult().value<QOpcUaEventFilterResult>());
                else if (param.filterResult().isNull())
                    it->clearFilterResult();
            }
            if (items & QOpcUaMonitoringParameters::Parameter::QueueSize)
                it->setQueueSize(param.queueSize());
            if (items & QOpcUaMonitoringParameters::Parameter::DiscardOldest)
                it->setDiscardOldest(param.discardOldest());
            if (items & QOpcUaMonitoringParameters::Parameter::MonitoringMode)
                it->setMonitoringMode(param.monitoringMode());
            if (items & QOpcUaMonitoringParameters::Parameter::TriggeredItemIds) {
                it->setTriggeredItemIds(param.triggeredItemIds());
                it->setFailedTriggeredItemsStatus(param.failedTriggeredItemsStatus());
            }
        }

        Q_Q(QOpcUaNode);
        emit q->monitoringStatusChanged(attr, items, param.statusCode());
    }

    QScopedPointer<QOpcUaNodeImpl> m_impl;
    QPointer<QOpcUaClient> m_client;

    QHash<QOpcUa::NodeAttribute, QOpcUaReadResult> m_nodeAttributes;
    QHash<QOpcUa::NodeAttribute, QOpcUaMonitoringParameters> m_monitoringStatus;

    std::array<QMetaObject::Connection, 9> m_connections;
};

QT_END_NAMESPACE

#endif // QOPCUANODE_P_H

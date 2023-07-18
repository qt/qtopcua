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
        m_attributesReadConnection = QObject::connect(m_impl.get(), &QOpcUaNodeImpl::attributesRead,
                q, [this](QList<QOpcUaReadResult> attr, QOpcUa::UaStatusCode serviceResult)
        {
            handleAttributesRead(attr, serviceResult);
        });

        m_attributeWrittenConnection = QObject::connect(m_impl.get(), &QOpcUaNodeImpl::attributeWritten,
                q, [this](QOpcUa::NodeAttribute attr, QVariant value, QOpcUa::UaStatusCode statusCode)
        {
            handleAttributesWritten(attr, value, statusCode);
        });

        m_dataChangeOccurredConnection = QObject::connect(m_impl.get(), &QOpcUaNodeImpl::dataChangeOccurred,
                q, [this](QOpcUa::NodeAttribute attr, QOpcUaReadResult value)
        {
            this->m_nodeAttributes[attr] = value;
            Q_Q(QOpcUaNode);
            emit q->dataChangeOccurred(attr, value.value());
            emit q->attributeUpdated(attr, value.value());
        });

        m_monitoringEnableDisableConnection = QObject::connect(m_impl.get(), &QOpcUaNodeImpl::monitoringEnableDisable,
               q, [this](QOpcUa::NodeAttribute attr, bool subscribe, QOpcUaMonitoringParameters status)
        {
            handleMonitoringEnableDisable(attr, subscribe, status);
        });

        m_monitoringStatusChangedConnection = QObject::connect(m_impl.get(), &QOpcUaNodeImpl::monitoringStatusChanged,
                q, [this](QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameters items, QOpcUaMonitoringParameters param)
        {
            handleMonitoringStatusChange(attr, items, param);
        });

        m_methodCallFinishedConnection =
                QObject::connect(m_impl.get(), &QOpcUaNodeImpl::methodCallFinished, q,
                                 &QOpcUaNode::methodCallFinished);

        m_browseFinishedConnection = QObject::connect(m_impl.get(), &QOpcUaNodeImpl::browseFinished,
                                                      q, &QOpcUaNode::browseFinished);

        m_resolveBrowsePathFinishedConnection =
                QObject::connect(m_impl.get(), &QOpcUaNodeImpl::resolveBrowsePathFinished, q,
                                 &QOpcUaNode::resolveBrowsePathFinished);

        m_eventOccurredConnection = QObject::connect(m_impl.get(), &QOpcUaNodeImpl::eventOccurred,
                                                     q, &QOpcUaNode::eventOccurred);
    }

    ~QOpcUaNodePrivate()
    {
        QObject::disconnect(m_attributesReadConnection);
        QObject::disconnect(m_attributeWrittenConnection);
        QObject::disconnect(m_dataChangeOccurredConnection);
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

    void handleAttributesRead(const QList<QOpcUaReadResult> &attr,
                              QOpcUa::UaStatusCode serviceResult)
    {
        QOpcUa::NodeAttributes updatedAttributes;
        Q_Q(QOpcUaNode);

        for (auto &entry : std::as_const(attr)) {
            if (serviceResult == QOpcUa::UaStatusCode::Good)
                m_nodeAttributes[entry.attribute()] = entry;
            else {
                QOpcUaReadResult temp = entry;
                temp.setStatusCode(serviceResult);
                temp.setValue(QVariant());
                m_nodeAttributes[entry.attribute()] = temp;
            }

            updatedAttributes |= entry.attribute();
            emit q->attributeUpdated(entry.attribute(), entry.value());
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
        }

        emit q->attributeWritten(attr, statusCode);
    }

    void handleMonitoringEnableDisable(QOpcUa::NodeAttribute attr, bool subscribe,
                                       const QOpcUaMonitoringParameters &status)
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
        }

        Q_Q(QOpcUaNode);
        emit q->monitoringStatusChanged(attr, items, param.statusCode());
    }

    QScopedPointer<QOpcUaNodeImpl> m_impl;
    QPointer<QOpcUaClient> m_client;

    QHash<QOpcUa::NodeAttribute, QOpcUaReadResult> m_nodeAttributes;
    QHash<QOpcUa::NodeAttribute, QOpcUaMonitoringParameters> m_monitoringStatus;

    QMetaObject::Connection m_attributesReadConnection;
    QMetaObject::Connection m_attributeWrittenConnection;
    QMetaObject::Connection m_dataChangeOccurredConnection;
    QMetaObject::Connection m_monitoringEnableDisableConnection;
    QMetaObject::Connection m_monitoringStatusChangedConnection;
    QMetaObject::Connection m_methodCallFinishedConnection;
    QMetaObject::Connection m_browseFinishedConnection;
    QMetaObject::Connection m_resolveBrowsePathFinishedConnection;
    QMetaObject::Connection m_eventOccurredConnection;
};

QT_END_NAMESPACE

#endif // QOPCUANODE_P_H

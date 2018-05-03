/******************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#include "quacppsubscription.h"
#include "quacppclient.h"
#include "quacppvalueconverter.h"

#include <QtCore/QLoggingCategory>

#include <uasession.h>

using namespace UaClientSdk;

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_UACPP)

QUACppSubscription::QUACppSubscription(UACppAsyncBackend *backend, const QOpcUaMonitoringParameters &settings)
    : UaSubscriptionCallback()
    , m_backend(backend)
    , m_subscriptionParameters(settings)
    , m_nativeSubscription(nullptr)
{
}

QUACppSubscription::~QUACppSubscription()
{
}

quint32 QUACppSubscription::createOnServer()
{
    UaStatus result;
    ServiceSettings serviceSettings;
    SubscriptionSettings subscriptionSettings;
    subscriptionSettings.publishingInterval = m_subscriptionParameters.publishingInterval();

    result = m_backend->m_nativeSession->createSubscription(
                serviceSettings,
                this,
                1,
                subscriptionSettings,
                OpcUa_True,
                &m_nativeSubscription);

    if (result.isGood()) {
        const OpcUa_UInt32 subId = m_nativeSubscription->subscriptionId();
        qCDebug(QT_OPCUA_PLUGINS_UACPP) << "Subscription suceeded, returning id:" << subId;
        return subId;
    }

    qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Subscription creation failed.";
    return 0;
}

bool QUACppSubscription::removeOnServer()
{
    if (!m_nativeSubscription) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Removing non existing subscription";
        return false;
    }

    UaStatus result;
    ServiceSettings settings;

    result = m_backend->m_nativeSession->deleteSubscription(settings, &m_nativeSubscription);
    if (result.isBad()) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Failed to delete subscription:";
        return false;
    }
    m_nativeSubscription = nullptr;
    return true;
}

bool QUACppSubscription::addAttributeMonitoredItem(uintptr_t handle, QOpcUa::NodeAttribute attr, const UaNodeId &id, QOpcUaMonitoringParameters parameters)
{
    qCDebug(QT_OPCUA_PLUGINS_UACPP) << "Adding monitored Item: " << handle << ":" << attr;
    static quint32 monitorId = 100;

    UaStatus result;
    ServiceSettings settings;
    UaMonitoredItemCreateRequests createRequests;
    UaMonitoredItemCreateResults createResults;

    createRequests.create(1);
    createRequests[0].ItemToMonitor.AttributeId = QUACppValueConverter::toUaAttributeId(attr);
    id.copyTo(&createRequests[0].ItemToMonitor.NodeId);
    const UaString uaiR(parameters.indexRange().toUtf8().constData());
    uaiR.copyTo(&createRequests[0].ItemToMonitor.IndexRange);
    createRequests[0].RequestedParameters.ClientHandle = monitorId;
    createRequests[0].RequestedParameters.SamplingInterval = parameters.samplingInterval();
    if (createRequests[0].RequestedParameters.SamplingInterval == 0.)
        createRequests[0].RequestedParameters.SamplingInterval = parameters.publishingInterval();
    createRequests[0].RequestedParameters.QueueSize = 1;
    createRequests[0].RequestedParameters.DiscardOldest = OpcUa_True;
    createRequests[0].MonitoringMode = static_cast<OpcUa_MonitoringMode>(parameters.monitoringMode());
    if (parameters.filter().type() == QVariant::UserType && parameters.filter().userType() == QMetaType::type("QOpcUaMonitoringParameters::DataChangeFilter"))
        createRequests[0].RequestedParameters.Filter = createFilter(parameters.filter());

    result = m_nativeSubscription->createMonitoredItems(settings, OpcUa_TimestampsToReturn_Both,
                                                        createRequests, createResults);

    OpcUa_MonitoredItemCreateRequest_Clear(&createRequests[0]); // The C++ destructor does not free the members of the requests

    if (result.isBad() || createResults.length() != 1 || OpcUa_IsBad(createResults[0].StatusCode)) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "creating monitored item failed.";
        QOpcUaMonitoringParameters s;
        s.setStatusCode(static_cast<QOpcUa::UaStatusCode>(createResults[0].StatusCode));
        emit m_backend->monitoringEnableDisable(handle, attr, true, s);
        return false;
    }

    QOpcUaMonitoringParameters s = parameters;
    s.setSubscriptionId(m_nativeSubscription->subscriptionId());
    s.setPublishingInterval(m_nativeSubscription->publishingInterval());
    s.setMaxKeepAliveCount(m_nativeSubscription->maxKeepAliveCount());
    s.setLifetimeCount(m_nativeSubscription->lifetimeCount());
    s.setStatusCode(static_cast<QOpcUa::UaStatusCode>(result.statusCode()));
    s.setSamplingInterval(createResults[0].RevisedSamplingInterval);

    // Store information
    const auto key = qMakePair(handle, attr);
    const auto value = qMakePair(createResults[0], parameters);
    m_monitoredItems.insert(key, value);
    m_monitoredIds.insert(monitorId, key);
    monitorId++;

    s.setFilter(QVariant());
    emit m_backend->monitoringEnableDisable(handle, attr, true, s);

    return true;
}

void QUACppSubscription::modifyMonitoring(uintptr_t handle, QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, QVariant value)
{
    QOpcUaMonitoringParameters p;
    p.setStatusCode(QOpcUa::UaStatusCode::BadNotImplemented);
    const auto key = qMakePair(handle, attr);

    if (!m_monitoredItems.contains(key)) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP, "Could not modify parameter for %lu, there are no monitored items", handle);
        p.setStatusCode(QOpcUa::UaStatusCode::BadAttributeIdInvalid);
        emit m_backend->monitoringStatusChanged(handle, attr, item, p);
        return;
    }

    // SetPublishingMode service
    if (item == QOpcUaMonitoringParameters::Parameter::PublishingEnabled) {
        if (value.type() != QVariant::Bool) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "New value for PublishingEnabled is not a boolean";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return;
        }
        ServiceSettings service;
        const OpcUa_Boolean enable = value.toBool();
        UaStatus result = m_nativeSubscription->setPublishingMode(service, enable);

        if (result.isNotGood()) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Failed to set publishing mode:" << result.statusCode();
            p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(result.statusCode()));
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return;
        }

        p.setPublishingEnabled(value.toBool());
        p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(result.statusCode()));
        emit m_backend->monitoringStatusChanged(handle, attr, item, p);
        return;
    }

    // SetMonitoringMode service
    if (item == QOpcUaMonitoringParameters::Parameter::MonitoringMode) {
        if (value.type() != QVariant::UserType || value.userType() != QMetaType::type("QOpcUaMonitoringParameters::MonitoringMode")) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "New value for MonitoringMode is not a monitoring mode";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return;
        }

        ServiceSettings service;
        OpcUa_MonitoringMode mode = static_cast<OpcUa_MonitoringMode>(value.value<QOpcUaMonitoringParameters::MonitoringMode>());
        UaUInt32Array ids;
        ids.create(1);
        ids[0] = m_monitoredItems[key].first.MonitoredItemId;
        UaStatusCodeArray results;
        UaStatusCode result = m_nativeSubscription->setMonitoringMode(service, mode, ids, results);

        if (result.isNotGood()) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Failed to set monitoring mode:" << result.statusCode();
            p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(result.statusCode()));
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return;
        }

        if (results.length() && OpcUa_IsGood(results[0]))
            p.setMonitoringMode(value.value<QOpcUaMonitoringParameters::MonitoringMode>());

        p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(results.length() ? results[0] : result.statusCode()));
        emit m_backend->monitoringStatusChanged(handle, attr, item, p);
        return;
    }

    if (modifySubscriptionParameters(handle, attr, item, value))
        return;
    if (modifyMonitoredItemParameters(handle, attr, item, value))
        return;

    qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Modifying" << item << "is not implemented.";
    p.setStatusCode(QOpcUa::UaStatusCode::BadNotImplemented);
    emit m_backend->monitoringStatusChanged(handle, attr, item, p);
}

bool QUACppSubscription::removeAttributeMonitoredItem(uintptr_t handle, QOpcUa::NodeAttribute attr)
{
    qCDebug(QT_OPCUA_PLUGINS_UACPP) << "Removing monitored Item: " << handle << ":" << attr;

    const QPair<uintptr_t, QOpcUa::NodeAttribute> pair(handle, attr);
    if (!m_monitoredItems.contains(pair)) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Trying to remove unknown monitored item:" << handle << ":" << attr;
        return false;
    }

    auto monitoredItem = m_monitoredItems.take(pair);
    UaStatus result;
    ServiceSettings settings;

    UaUInt32Array removeIds;
    removeIds.resize(1);
    removeIds[0] = monitoredItem.first.MonitoredItemId;
    UaStatusCodeArray removeResults;
    result = m_nativeSubscription->deleteMonitoredItems(settings, removeIds, removeResults);
    if (result.isBad() || removeResults.length() != 1 || OpcUa_IsBad(removeResults[0])) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Failed to remove monitored item:" << handle << ":" << attr;
        return false;
    }

    QOpcUaMonitoringParameters s;
    s.setSubscriptionId(m_nativeSubscription->subscriptionId());
    s.setPublishingInterval(m_nativeSubscription->publishingInterval());
    s.setMaxKeepAliveCount(m_nativeSubscription->maxKeepAliveCount());
    s.setLifetimeCount(m_nativeSubscription->lifetimeCount());
    s.setStatusCode(static_cast<QOpcUa::UaStatusCode>(result.statusCode()));
    emit m_backend->monitoringEnableDisable(handle, attr, false, s);

    return true;
}

double QUACppSubscription::interval() const
{
    return m_subscriptionParameters.publishingInterval();
}

quint32 QUACppSubscription::subscriptionId() const
{
    return m_nativeSubscription->subscriptionId();
}

int QUACppSubscription::monitoredItemsCount() const
{
    return m_monitoredItems.size();
}

QOpcUaMonitoringParameters::SubscriptionType QUACppSubscription::shared() const
{
    return m_subscriptionParameters.shared();
}

void QUACppSubscription::subscriptionStatusChanged(OpcUa_UInt32 clientSubscriptionHandle, const UaStatus &status)
{
    Q_UNUSED(clientSubscriptionHandle);
    Q_UNUSED(status);
    Q_UNIMPLEMENTED();
    qCWarning(QT_OPCUA_PLUGINS_UACPP) << "subscriptionStatusChange unhandled";
}

void QUACppSubscription::dataChange(OpcUa_UInt32 clientSubscriptionHandle, const UaMonitoredItemNotifications &dataNotifications, const UaDiagnosticInfos &diagnosticInfos)
{
    Q_UNUSED(diagnosticInfos);
    qCDebug(QT_OPCUA_PLUGINS_UACPP) << "Data Change on:" << clientSubscriptionHandle << ":" << m_nativeSubscription->subscriptionId();

    for (quint32 i = 0; i < dataNotifications.length(); ++i) {
        const quint32 monitorId = dataNotifications[i].ClientHandle;
        const QVariant var = QUACppValueConverter::toQVariant(dataNotifications[i].Value.Value);
        if (!m_monitoredIds.contains(monitorId))
            continue;

        QOpcUaReadResult temp;
        temp.value = var;
        temp.serverTimestamp = QUACppValueConverter::toQDateTime(&dataNotifications[i].Value.ServerTimestamp);
        temp.sourceTimestamp = QUACppValueConverter::toQDateTime(&dataNotifications[i].Value.SourceTimestamp);
        temp.attributeId = m_monitoredIds[monitorId].second;
        temp.statusCode = QOpcUa::UaStatusCode::Good;

        emit m_backend->attributeUpdated(m_monitoredIds[monitorId].first, temp);
    }
}

void QUACppSubscription::newEvents(OpcUa_UInt32 clientSubscriptionHandle, UaEventFieldLists &eventFieldList)
{
    Q_UNUSED(clientSubscriptionHandle);
    Q_UNUSED(eventFieldList);
    Q_UNIMPLEMENTED();
    qCWarning(QT_OPCUA_PLUGINS_UACPP) << "eventsChange unhandled";
}

OpcUa_ExtensionObject QUACppSubscription::createFilter(const QVariant &filterData)
{
    OpcUa_ExtensionObject obj;
    OpcUa_ExtensionObject_Initialize(&obj);

    if (filterData.type() == QVariant::UserType && filterData.userType() == QMetaType::type("QOpcUaMonitoringParameters::DataChangeFilter")) {
        const QOpcUaMonitoringParameters::DataChangeFilter temp = filterData.value<QOpcUaMonitoringParameters::DataChangeFilter>();

        OpcUa_DataChangeFilter *filter = nullptr;

        OpcUa_EncodeableObject_CreateExtension(&OpcUa_DataChangeFilter_EncodeableType,
                                               &obj,
                                               reinterpret_cast<OpcUa_Void **>(&filter));

        if (!filter) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Could not create DataChangeFilter";
            return obj;
        }
        filter->DeadbandType = static_cast<OpcUa_UInt32>(temp.deadbandType);
        filter->DeadbandValue = static_cast<OpcUa_Double>(temp.deadbandValue);
        filter->Trigger = static_cast<OpcUa_DataChangeTrigger>(temp.trigger);

        return obj;
    }

    if (filterData.isValid())
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Could not create filter, invalid input.";

    return obj;
}

bool QUACppSubscription::modifySubscriptionParameters(uintptr_t handle, QOpcUa::NodeAttribute attr, const QOpcUaMonitoringParameters::Parameter &item, const QVariant &value)
{
    QOpcUaMonitoringParameters p;
    SubscriptionSettings settings;
    settings.maxNotificationsPerPublish = m_nativeSubscription->maxNotificationsPerPublish();
    settings.publishingInterval = m_nativeSubscription->publishingInterval();
    settings.lifetimeCount = m_nativeSubscription->lifetimeCount();
    settings.maxKeepAliveCount = m_nativeSubscription->maxKeepAliveCount();

    bool match = true;

    switch (item) {
    case QOpcUaMonitoringParameters::Parameter::PublishingInterval: {
        bool ok;
        settings.publishingInterval = value.toDouble(&ok);

        if (!ok) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP, "Could not modify PublishingInterval for %lu, value is not a double", handle);
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return true;
        }
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::LifetimeCount: {
        bool ok;
        settings.lifetimeCount = value.toUInt(&ok);

        if (!ok) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP, "Could not modify LifetimeCount for %lu, value is not an integer", handle);
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return true;
        }
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::MaxKeepAliveCount: {
        bool ok;
        settings.maxKeepAliveCount = value.toUInt(&ok);

        if (!ok) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP, "Could not modify MaxKeepAliveCount for %lu, value is not an integer", handle);
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return true;
        }
        break;
    }
    default:
        match = false;
        break;
    }

    if (match) {
        ServiceSettings service;
        const double oldPublishInterval = m_nativeSubscription->publishingInterval();
        const quint32 oldLifeTimeCount = m_nativeSubscription->lifetimeCount();
        const quint32 oldMaxKeepAlive = m_nativeSubscription->maxKeepAliveCount();
        UaStatus result = m_nativeSubscription->modifySubscription(service, settings);

        if (result.isBad()) {
            p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(result.statusCode()));
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
        } else {
            p.setStatusCode(QOpcUa::UaStatusCode::Good);
            p.setPublishingInterval(settings.publishingInterval);
            p.setLifetimeCount(settings.lifetimeCount);
            p.setMaxKeepAliveCount(settings.maxKeepAliveCount);

            QOpcUaMonitoringParameters::Parameters changed = item;
            if (!qFuzzyCompare(p.publishingInterval(), oldPublishInterval))
                changed |= QOpcUaMonitoringParameters::Parameter::PublishingInterval;
            if (p.lifetimeCount() != oldLifeTimeCount)
                changed |= QOpcUaMonitoringParameters::Parameter::LifetimeCount;
            if (p.maxKeepAliveCount() != oldMaxKeepAlive)
                changed |= QOpcUaMonitoringParameters::Parameter::MaxKeepAliveCount;

            m_subscriptionParameters.setLifetimeCount(p.lifetimeCount());
            m_subscriptionParameters.setMaxKeepAliveCount(p.maxKeepAliveCount());
            m_subscriptionParameters.setPublishingInterval(p.publishingInterval());

            for (auto it : qAsConst(m_monitoredIds))
                emit m_backend->monitoringStatusChanged(it.first, it.second, changed, p);
        }
        return true;
    }
    return false;
}

bool QUACppSubscription::modifyMonitoredItemParameters(uintptr_t handle, QOpcUa::NodeAttribute attr, const QOpcUaMonitoringParameters::Parameter &item, const QVariant &value)
{
    // Get hold of OpcUa_MonitoredItemCreateResult
    const QPair<uintptr_t, QOpcUa::NodeAttribute> key(handle, attr);
    if (!m_monitoredItems.contains(key)) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Did not find monitored item";
        return false;
    }
    auto valuePair = m_monitoredItems[key];
    OpcUa_MonitoredItemCreateResult createResult = valuePair.first;

    OpcUa_MonitoredItemModifyRequest modifyRequest;
    OpcUa_MonitoredItemModifyRequest_Initialize(&modifyRequest);
    modifyRequest.MonitoredItemId = createResult.MonitoredItemId;

    modifyRequest.RequestedParameters.ClientHandle = m_monitoredIds.key(key);
    QOpcUaMonitoringParameters p = valuePair.second;

    if (item != QOpcUaMonitoringParameters::Parameter::Filter)
        modifyRequest.RequestedParameters.Filter = createFilter(valuePair.second.filter());

    bool match = true;

    switch (item) {
    case QOpcUaMonitoringParameters::Parameter::DiscardOldest: {
        if (value.type() != QVariant::Bool) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Could not modify DiscardOldest for" << handle << ", value is not a bool";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return true;
        }
        modifyRequest.RequestedParameters.DiscardOldest = value.toBool();
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::QueueSize: {
        if (value.type() != QVariant::UInt) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Could not modify QueueSize for" << handle << ", value is not an integer";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return true;
        }
        modifyRequest.RequestedParameters.QueueSize = value.toUInt();
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::SamplingInterval: {
        if (value.type() != QVariant::Double) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Could not modify SamplingInterval for" << handle << ", value is not a double";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return true;
        }
        modifyRequest.RequestedParameters.SamplingInterval = value.toDouble();
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::Filter: {
        modifyRequest.RequestedParameters.Filter = createFilter(value);
        break;
    }
    default:
        match = false;
        break;
    }

    if (match) {
        ServiceSettings service;
        UaMonitoredItemModifyRequests requests(1, &modifyRequest);
        UaMonitoredItemModifyResults results;
        UaStatus result = m_nativeSubscription->modifyMonitoredItems(service, OpcUa_TimestampsToReturn_Both, requests, results);

        OpcUa_MonitoredItemModifyRequest_Clear(&modifyRequest); // The C++ destructor does not free the members of the requests

        if (result.isBad() || OpcUa_IsNotGood(results[0].StatusCode)) {
            p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(result.isGood() ?
                                                                  results[0].StatusCode :
                                                                  result.statusCode()));
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
        } else {
            p.setStatusCode(QOpcUa::UaStatusCode::Good);
            QOpcUaMonitoringParameters::Parameters changed = item;
            if (!qFuzzyCompare(p.samplingInterval(), results[0].RevisedSamplingInterval)) {
                p.setSamplingInterval(results[0].RevisedSamplingInterval);
                changed |= QOpcUaMonitoringParameters::Parameter::SamplingInterval;
            }
            if (p.queueSize() != results[0].RevisedQueueSize) {
                p.setQueueSize(results[0].RevisedQueueSize);
                changed |= QOpcUaMonitoringParameters::Parameter::QueueSize;
            }
            if (item == QOpcUaMonitoringParameters::Parameter::DiscardOldest) {
                p.setDiscardOldest(value.toBool());
                changed | QOpcUaMonitoringParameters::Parameter::DiscardOldest;
            }

            emit m_backend->monitoringStatusChanged(handle, attr, changed, p);
            m_monitoredItems[key].second = p;
        }
        return true;
    }

    return false;
}

QT_END_NAMESPACE

// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopen62541backend.h"
#include "qopen62541client.h"
#include "qopen62541node.h"
#include "qopen62541subscription.h"
#include "qopen62541utils.h"
#include "qopen62541valueconverter.h"
#include "qopen62541utils.h"
#include <private/qopcuanode_p.h>

#include "qopcuaelementoperand.h"
#include "qopcualiteraloperand.h"
#include "qopcuaattributeoperand.h"
#include "qopcuacontentfilterelementresult.h"

#include <QtCore/qloggingcategory.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_OPEN62541)

static void monitoredValueHandler(UA_Client *client, UA_UInt32 subId, void *subContext, UA_UInt32 monId, void *monContext, UA_DataValue *value)
{
    Q_UNUSED(client)
    Q_UNUSED(subId)
    Q_UNUSED(subContext)
    QOpen62541Subscription *subscription = static_cast<QOpen62541Subscription *>(monContext);
    subscription->monitoredValueUpdated(monId, value);
}

static void stateChangeHandler(UA_Client *client, UA_UInt32 subId, void *subContext, UA_StatusChangeNotification *notification)
{
    Q_UNUSED(client);
    Q_UNUSED(subId);

    if (notification->status != UA_STATUSCODE_BADTIMEOUT)
        return;

    QOpen62541Subscription *sub = static_cast<QOpen62541Subscription *>(subContext);
    sub->sendTimeoutNotification();
}

static void eventHandler(UA_Client *client, UA_UInt32 subId, void *subContext, UA_UInt32 monId, void *monContext,
                         size_t numFields, UA_Variant *eventFields)
{
    Q_UNUSED(client);
    Q_UNUSED(subId);
    Q_UNUSED(subContext);

    QOpen62541Subscription *subscription = static_cast<QOpen62541Subscription *>(monContext);

    QVariantList list;
    for (size_t i = 0; i < numFields; ++i)
        list.append(QOpen62541ValueConverter::toQVariant(eventFields[i]));
    subscription->eventReceived(monId, list);
}

QOpen62541Subscription::QOpen62541Subscription(Open62541AsyncBackend *backend, const QOpcUaMonitoringParameters &settings)
    : m_backend(backend)
    , m_interval(settings.publishingInterval())
    , m_subscriptionId(0)
    , m_lifetimeCount(settings.lifetimeCount() ? settings.lifetimeCount() : UA_CreateSubscriptionRequest_default().requestedLifetimeCount)
    , m_maxKeepaliveCount(settings.maxKeepAliveCount() ? settings.maxKeepAliveCount() : UA_CreateSubscriptionRequest_default().requestedMaxKeepAliveCount)
    , m_shared(settings.subscriptionType())
    , m_priority(settings.priority())
    , m_maxNotificationsPerPublish(settings.maxNotificationsPerPublish())
    , m_clientHandle(0)
    , m_timeout(false)
{
}

QOpen62541Subscription::~QOpen62541Subscription()
{
    removeOnServer();
}

UA_UInt32 QOpen62541Subscription::createOnServer()
{
    UA_CreateSubscriptionRequest req = UA_CreateSubscriptionRequest_default();
    req.requestedPublishingInterval = m_interval;
    req.requestedLifetimeCount = m_lifetimeCount;
    req.requestedMaxKeepAliveCount = m_maxKeepaliveCount;
    req.priority = m_priority;
    req.maxNotificationsPerPublish = m_maxNotificationsPerPublish;
    UA_CreateSubscriptionResponse res = UA_Client_Subscriptions_create(m_backend->m_uaclient, req, this, stateChangeHandler, nullptr);

    if (res.responseHeader.serviceResult != UA_STATUSCODE_GOOD) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not create subscription with interval" << m_interval << UA_StatusCode_name(res.responseHeader.serviceResult);
        return 0;
    }

    m_subscriptionId = res.subscriptionId;
    m_maxKeepaliveCount = res.revisedMaxKeepAliveCount;
    m_lifetimeCount = res.revisedLifetimeCount;
    m_interval = res.revisedPublishingInterval;
    return m_subscriptionId;
}

bool QOpen62541Subscription::removeOnServer()
{
    UA_StatusCode res = UA_STATUSCODE_GOOD;
    if (m_subscriptionId) {
        res = UA_Client_Subscriptions_deleteSingle(m_backend->m_uaclient, m_subscriptionId);
        m_subscriptionId = 0;
    }

    for (auto it : std::as_const(m_itemIdToItemMapping)) {
        QOpcUaMonitoringParameters s;
        s.setStatusCode(m_timeout ? QOpcUa::UaStatusCode::BadTimeout : QOpcUa::UaStatusCode::BadDisconnect);
        emit m_backend->monitoringEnableDisable(it->handle, it->attr, false, s);
    }

    qDeleteAll(m_itemIdToItemMapping);

    m_itemIdToItemMapping.clear();
    m_nodeHandleToItemMapping.clear();

    return (res == UA_STATUSCODE_GOOD) ? true : false;
}

void QOpen62541Subscription::modifyMonitoring(quint64 handle, QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, QVariant value)
{
    QOpcUaMonitoringParameters p;
    p.setStatusCode(QOpcUa::UaStatusCode::BadNotImplemented);

    MonitoredItem *monItem = getItemForAttribute(handle, attr);
    if (!monItem) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify parameter" << item << "there are no monitored items";
        p.setStatusCode(QOpcUa::UaStatusCode::BadAttributeIdInvalid);
        emit m_backend->monitoringStatusChanged(handle, attr, item, p);
        return;
    }

    p = monItem->parameters;

    // SetPublishingMode service
    if (item == QOpcUaMonitoringParameters::Parameter::PublishingEnabled) {
        if (value.metaType().id() != QMetaType::Bool) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "New value for PublishingEnabled is not a boolean";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return;
        }

        UA_SetPublishingModeRequest req;
        UA_SetPublishingModeRequest_init(&req);
        UaDeleter<UA_SetPublishingModeRequest> requestDeleter(&req, UA_SetPublishingModeRequest_clear);
        req.publishingEnabled = value.toBool();
        req.subscriptionIdsSize = 1;
        req.subscriptionIds = UA_UInt32_new();
        *req.subscriptionIds = m_subscriptionId;
        UA_SetPublishingModeResponse res = UA_Client_Subscriptions_setPublishingMode(m_backend->m_uaclient, req);
        UaDeleter<UA_SetPublishingModeResponse> responseDeleter(&res, UA_SetPublishingModeResponse_clear);

        if (res.responseHeader.serviceResult != UA_STATUSCODE_GOOD) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to set publishing mode:" << res.responseHeader.serviceResult;
            p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult));
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return;
        }

        if (res.resultsSize && res.results[0] == UA_STATUSCODE_GOOD)
            p.setPublishingEnabled(value.toBool());

        p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(res.results[0]));
        emit m_backend->monitoringStatusChanged(handle, attr, item, p);

        return;
    }

    // SetMonitoringMode service
    if (item == QOpcUaMonitoringParameters::Parameter::MonitoringMode) {
        if (value.userType() != QMetaType::fromType<QOpcUaMonitoringParameters::MonitoringMode>().id()) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "New value for MonitoringMode is not a monitoring mode";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return;
        }

        UA_SetMonitoringModeRequest req;
        UA_SetMonitoringModeRequest_init(&req);
        UaDeleter<UA_SetMonitoringModeRequest> requestDeleter(&req, UA_SetMonitoringModeRequest_clear);
        req.monitoringMode = static_cast<UA_MonitoringMode>(value.value<QOpcUaMonitoringParameters::MonitoringMode>());
        req.monitoredItemIdsSize = 1;
        req.monitoredItemIds = UA_UInt32_new();
        *req.monitoredItemIds = monItem->monitoredItemId;
        req.subscriptionId = m_subscriptionId;
        UA_SetMonitoringModeResponse res = UA_Client_MonitoredItems_setMonitoringMode(m_backend->m_uaclient, req);
        UaDeleter<UA_SetMonitoringModeResponse> responseDeleter(&res, UA_SetMonitoringModeResponse_clear);

        if (res.responseHeader.serviceResult != UA_STATUSCODE_GOOD) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to set monitoring mode:" << res.responseHeader.serviceResult;
            p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult));
            emit m_backend->monitoringStatusChanged(handle, attr, item, p);
            return;
        }

        if (res.resultsSize && res.results[0] == UA_STATUSCODE_GOOD)
            p.setMonitoringMode(value.value<QOpcUaMonitoringParameters::MonitoringMode>());

        p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(res.results[0]));
        emit m_backend->monitoringStatusChanged(handle, attr, item, p);
        return;
    }

    if (modifySubscriptionParameters(handle, attr, item, value))
        return;
    if (modifyMonitoredItemParameters(handle, attr, item, value))
        return;

    qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Modifying" << item << "is not implemented";
    p.setStatusCode(QOpcUa::UaStatusCode::BadNotImplemented);
    emit m_backend->monitoringStatusChanged(handle, attr, item, p);
}

bool QOpen62541Subscription::addAttributeMonitoredItem(quint64 handle, QOpcUa::NodeAttribute attr, const UA_NodeId &id, QOpcUaMonitoringParameters settings)
{
    UA_MonitoredItemCreateRequest req;
    UA_MonitoredItemCreateRequest_init(&req);
    UaDeleter<UA_MonitoredItemCreateRequest> requestDeleter(&req, UA_MonitoredItemCreateRequest_clear);
    req.itemToMonitor.attributeId = QOpen62541ValueConverter::toUaAttributeId(attr);
    UA_NodeId_copy(&id, &(req.itemToMonitor.nodeId));
    if (settings.indexRange().size())
        QOpen62541ValueConverter::scalarFromQt<UA_String, QString>(settings.indexRange(), &req.itemToMonitor.indexRange);
    req.monitoringMode = static_cast<UA_MonitoringMode>(settings.monitoringMode());
    req.requestedParameters.samplingInterval = qFuzzyCompare(settings.samplingInterval(), 0.0) ? m_interval : settings.samplingInterval();
    req.requestedParameters.queueSize = settings.queueSize() == 0 ? 1 : settings.queueSize();
    req.requestedParameters.discardOldest = settings.discardOldest();
    req.requestedParameters.clientHandle = ++m_clientHandle;

    if (settings.filter().isValid()) {
        UA_ExtensionObject filter = createFilter(settings.filter());
        if (filter.content.decoded.data)
            req.requestedParameters.filter = filter;
        else {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not create monitored item, filter creation failed";
            QOpcUaMonitoringParameters s;
            s.setStatusCode(QOpcUa::UaStatusCode::BadInternalError);
            emit m_backend->monitoringEnableDisable(handle, attr, true, s);
            return false;
        }
    }

    UA_MonitoredItemCreateResult res;
    UaDeleter<UA_MonitoredItemCreateResult> resultDeleter(&res, UA_MonitoredItemCreateResult_clear);

    if (attr == QOpcUa::NodeAttribute::EventNotifier && settings.filter().canConvert<QOpcUaMonitoringParameters::EventFilter>())
        res = UA_Client_MonitoredItems_createEvent(m_backend->m_uaclient, m_subscriptionId,
                                                   UA_TIMESTAMPSTORETURN_BOTH, req, this, eventHandler, nullptr);
    else
        res = UA_Client_MonitoredItems_createDataChange(m_backend->m_uaclient, m_subscriptionId, UA_TIMESTAMPSTORETURN_BOTH, req, this, monitoredValueHandler, nullptr);

    if (res.statusCode != UA_STATUSCODE_GOOD) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not add monitored item for" << attr << "of node" << Open62541Utils::nodeIdToQString(id) << ":" << UA_StatusCode_name(res.statusCode);
        QOpcUaMonitoringParameters s;
        s.setStatusCode(static_cast<QOpcUa::UaStatusCode>(res.statusCode));
        emit m_backend->monitoringEnableDisable(handle, attr, true, s);
        return false;
    }

    MonitoredItem *temp = new MonitoredItem(handle, attr, res.monitoredItemId);
    m_nodeHandleToItemMapping[handle][attr] = temp;
    m_itemIdToItemMapping[res.monitoredItemId] = temp;

    QOpcUaMonitoringParameters s = settings;
    s.setSubscriptionId(m_subscriptionId);
    s.setPublishingInterval(m_interval);
    s.setMaxKeepAliveCount(m_maxKeepaliveCount);
    s.setLifetimeCount(m_lifetimeCount);
    s.setStatusCode(QOpcUa::UaStatusCode::Good);
    s.setSamplingInterval(res.revisedSamplingInterval);
    s.setQueueSize(res.revisedQueueSize);
    s.setMonitoredItemId(res.monitoredItemId);
    temp->parameters = s;
    temp->clientHandle = m_clientHandle;

    if (res.filterResult.encoding >= UA_EXTENSIONOBJECT_DECODED &&
            res.filterResult.content.decoded.type == &UA_TYPES[UA_TYPES_EVENTFILTERRESULT])
        s.setFilterResult(convertEventFilterResult(&res.filterResult));
    else
        s.clearFilterResult();

    emit m_backend->monitoringEnableDisable(handle, attr, true, s);

    return true;
}

bool QOpen62541Subscription::removeAttributeMonitoredItem(quint64 handle, QOpcUa::NodeAttribute attr)
{
    MonitoredItem *item = getItemForAttribute(handle, attr);
    if (!item) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "There is no monitored item for this attribute";
        QOpcUaMonitoringParameters s;
        s.setStatusCode(QOpcUa::UaStatusCode::BadMonitoredItemIdInvalid);
        emit m_backend->monitoringEnableDisable(handle, attr, false, s);
        return false;
    }

    UA_StatusCode res = UA_Client_MonitoredItems_deleteSingle(m_backend->m_uaclient, m_subscriptionId, item->monitoredItemId);
    if (res != UA_STATUSCODE_GOOD)
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not remove monitored item" << item->monitoredItemId << "from subscription" << m_subscriptionId << ":" << UA_StatusCode_name(res);

    m_itemIdToItemMapping.remove(item->monitoredItemId);
    auto it = m_nodeHandleToItemMapping.find(handle);
    it->remove(attr);
    if (it->empty())
        m_nodeHandleToItemMapping.erase(it);

    delete item;

    QOpcUaMonitoringParameters s;
    s.setStatusCode(static_cast<QOpcUa::UaStatusCode>(res));
    emit m_backend->monitoringEnableDisable(handle, attr, false, s);

    return true;
}

void QOpen62541Subscription::monitoredValueUpdated(UA_UInt32 monId, UA_DataValue *value)
{
    auto item = m_itemIdToItemMapping.constFind(monId);
    if (item == m_itemIdToItemMapping.constEnd())
        return;
    QOpcUaReadResult res;

    if (!value || value == UA_EMPTY_ARRAY_SENTINEL) {
        res.setStatusCode(QOpcUa::UaStatusCode::Good);
        emit m_backend->dataChangeOccurred(item.value()->handle, res);
        return;
    }

    res.setValue(QOpen62541ValueConverter::toQVariant(value->value));
    res.setAttribute(item.value()->attr);
    if (value->hasServerTimestamp)
        res.setServerTimestamp(QOpen62541ValueConverter::scalarToQt<QDateTime, UA_DateTime>(&value->serverTimestamp));
    if (value->hasSourceTimestamp)
        res.setSourceTimestamp(QOpen62541ValueConverter::scalarToQt<QDateTime, UA_DateTime>(&value->sourceTimestamp));
    res.setStatusCode(QOpcUa::UaStatusCode::Good);
    emit m_backend->dataChangeOccurred(item.value()->handle, res);
}

void QOpen62541Subscription::sendTimeoutNotification()
{
    QList<QPair<quint64, QOpcUa::NodeAttribute>> items;
    for (const auto &it : std::as_const(m_nodeHandleToItemMapping)) {
        for (auto item : it) {
            items.push_back({item->handle, item->attr});
        }
    }
    emit timeout(this, items);
    m_timeout = true;
}

void QOpen62541Subscription::eventReceived(UA_UInt32 monId, QVariantList list)
{
    auto item = m_itemIdToItemMapping.constFind(monId);
    if (item == m_itemIdToItemMapping.constEnd())
        return;
    emit m_backend->eventOccurred(item.value()->handle, list);
}

double QOpen62541Subscription::interval() const
{
    return m_interval;
}

UA_UInt32 QOpen62541Subscription::subscriptionId() const
{
    return m_subscriptionId;
}

int QOpen62541Subscription::monitoredItemsCount() const
{
    return m_itemIdToItemMapping.size();
}

QOpcUaMonitoringParameters::SubscriptionType QOpen62541Subscription::shared() const
{
    return m_shared;
}

QOpen62541Subscription::MonitoredItem *QOpen62541Subscription::getItemForAttribute(quint64 nodeHandle, QOpcUa::NodeAttribute attr)
{
    auto nodeEntry = m_nodeHandleToItemMapping.constFind(nodeHandle);

    if (nodeEntry == m_nodeHandleToItemMapping.constEnd())
        return nullptr;

    auto item = nodeEntry->constFind(attr);
    if (item == nodeEntry->constEnd())
        return nullptr;

    return item.value();
}

UA_ExtensionObject QOpen62541Subscription::createFilter(const QVariant &filterData)
{
    UA_ExtensionObject obj;
    UA_ExtensionObject_init(&obj);

    if (filterData.canConvert<QOpcUaMonitoringParameters::DataChangeFilter>()) {
        createDataChangeFilter(filterData.value<QOpcUaMonitoringParameters::DataChangeFilter>(), &obj);
        return obj;
    }

    if (filterData.canConvert<QOpcUaMonitoringParameters::EventFilter>()) {
        createEventFilter(filterData.value<QOpcUaMonitoringParameters::EventFilter>(), &obj);
        return obj;
    }

    if (filterData.isValid())
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not create filter, invalid input.";

    return obj;
}

void QOpen62541Subscription::createDataChangeFilter(const QOpcUaMonitoringParameters::DataChangeFilter &filter, UA_ExtensionObject *out)
{
    UA_DataChangeFilter *uaFilter = UA_DataChangeFilter_new();
    uaFilter->deadbandType = static_cast<UA_UInt32>(filter.deadbandType());
    uaFilter->deadbandValue = filter.deadbandValue();
    uaFilter->trigger = static_cast<UA_DataChangeTrigger>(filter.trigger());
    out->encoding = UA_EXTENSIONOBJECT_DECODED;
    out->content.decoded.type = &UA_TYPES[UA_TYPES_DATACHANGEFILTER];
    out->content.decoded.data = uaFilter;
}

void QOpen62541Subscription::createEventFilter(const QOpcUaMonitoringParameters::EventFilter &filter, UA_ExtensionObject *out)
{
    UA_EventFilter *uaFilter = UA_EventFilter_new();
    UA_EventFilter_init(uaFilter);
    out->encoding = UA_EXTENSIONOBJECT_DECODED;
    out->content.decoded.data = uaFilter;
    out->content.decoded.type = &UA_TYPES[UA_TYPES_EVENTFILTER];

    convertSelectClause(filter, &uaFilter->selectClauses, &uaFilter->selectClausesSize);
    if (!convertWhereClause(filter, &uaFilter->whereClause))
        UA_ExtensionObject_clear(out);
}

bool QOpen62541Subscription::convertSelectClause(const QOpcUaMonitoringParameters::EventFilter &filter,
                                                 UA_SimpleAttributeOperand **selectClauses, size_t *size)
{
    if (!filter.selectClauses().isEmpty()) {
        UA_SimpleAttributeOperand *select = static_cast<UA_SimpleAttributeOperand *>(
                    UA_Array_new(filter.selectClauses().size(), &UA_TYPES[UA_TYPES_SIMPLEATTRIBUTEOPERAND]));

        for (int i = 0; i < filter.selectClauses().size(); ++i) {
            UA_SimpleAttributeOperand_init(&select[i]);
            if (!filter.selectClauses().at(i).typeId().isEmpty())
                select[i].typeDefinitionId = Open62541Utils::nodeIdFromQString(filter.selectClauses().at(i).typeId());
            select[i].browsePathSize = filter.selectClauses().at(i).browsePath().size();
            if (select[i].browsePathSize) {
                select[i].browsePath = static_cast<UA_QualifiedName *>(
                            UA_Array_new(select[i].browsePathSize, &UA_TYPES[UA_TYPES_QUALIFIEDNAME]));
                for (size_t j = 0; j < select[i].browsePathSize; ++j)
                    QOpen62541ValueConverter::scalarFromQt<UA_QualifiedName, QOpcUaQualifiedName>(
                                filter.selectClauses().at(i).browsePath().at(j), &select[i].browsePath[j]);
            }
            if (!filter.selectClauses().at(i).indexRange().isEmpty())
                QOpen62541ValueConverter::scalarFromQt<UA_String, QString>(filter.selectClauses().at(i).indexRange(),
                                                                           &select[i].indexRange);
            select[i].attributeId = QOpen62541ValueConverter::toUaAttributeId(filter.selectClauses().at(i).attributeId());
        }

        *selectClauses = select;
        *size = filter.selectClauses().size();

        return true;
    }

    *selectClauses = nullptr;
    *size = 0;
    return true;
}

bool QOpen62541Subscription::convertWhereClause(const QOpcUaMonitoringParameters::EventFilter &filter, UA_ContentFilter *result)
{
    if (!filter.whereClause().isEmpty()) {
        result->elementsSize = filter.whereClause().size();
        result->elements = static_cast<UA_ContentFilterElement *>(
                    UA_Array_new(filter.whereClause().size(), &UA_TYPES[UA_TYPES_CONTENTFILTERELEMENT]));
        for (int i = 0; i < filter.whereClause().size(); ++i) {
            UA_ContentFilterElement_init(&result->elements[i]);
            result->elements[i].filterOperator = static_cast<UA_FilterOperator>(filter.whereClause().at(i).filterOperator());
            result->elements[i].filterOperandsSize = filter.whereClause().at(i).filterOperands().size();
            result->elements[i].filterOperands = static_cast<UA_ExtensionObject *>(
                        UA_Array_new(filter.whereClause().at(i).filterOperands().size(), &UA_TYPES[UA_TYPES_EXTENSIONOBJECT]));
            for (int j = 0; j < filter.whereClause().at(i).filterOperands().size(); ++j) {
                UA_ExtensionObject_init(&result->elements[i].filterOperands[j]);
                result->elements[i].filterOperands[j].encoding = UA_EXTENSIONOBJECT_DECODED;
                const QVariant currentOperand = filter.whereClause().at(i).filterOperands().at(j);
                if (currentOperand.canConvert<QOpcUaElementOperand>()) {
                    UA_ElementOperand *op = UA_ElementOperand_new();
                    UA_ElementOperand_init(op);
                    op->index = currentOperand.value<QOpcUaElementOperand>().index();
                    result->elements[i].filterOperands[j].content.decoded.data = op;
                    result->elements[i].filterOperands[j].content.decoded.type = &UA_TYPES[UA_TYPES_ELEMENTOPERAND];
                } else if (currentOperand.canConvert<QOpcUaLiteralOperand>()) {
                    UA_LiteralOperand *op = UA_LiteralOperand_new();
                    UA_LiteralOperand_init(op);
                    QOpcUaLiteralOperand litOp = currentOperand.value<QOpcUaLiteralOperand>();
                    op->value = QOpen62541ValueConverter::toOpen62541Variant(litOp.value(), litOp.type());
                    result->elements[i].filterOperands[j].content.decoded.data = op;
                    result->elements[i].filterOperands[j].content.decoded.type = &UA_TYPES[UA_TYPES_LITERALOPERAND];
                } else if (currentOperand.canConvert<QOpcUaSimpleAttributeOperand>()) {
                    UA_SimpleAttributeOperand *op = UA_SimpleAttributeOperand_new();
                    UA_SimpleAttributeOperand_init(op);
                    QOpcUaSimpleAttributeOperand operand = currentOperand.value<QOpcUaSimpleAttributeOperand>();
                    op->attributeId = QOpen62541ValueConverter::toUaAttributeId(operand.attributeId());
                    QOpen62541ValueConverter::scalarFromQt<UA_String, QString>(operand.indexRange(), &op->indexRange);
                    if (!operand.typeId().isEmpty())
                        op->typeDefinitionId = Open62541Utils::nodeIdFromQString(operand.typeId());
                    op->browsePathSize = operand.browsePath().size();
                    op->browsePath = static_cast<UA_QualifiedName *>(UA_Array_new(operand.browsePath().size(),
                                                                                  &UA_TYPES[UA_TYPES_QUALIFIEDNAME]));
                    for (int k = 0; k < operand.browsePath().size(); ++k)
                        QOpen62541ValueConverter::scalarFromQt<UA_QualifiedName, QOpcUaQualifiedName>(
                                    operand.browsePath().at(k), &op->browsePath[k]);
                    result->elements[i].filterOperands[j].content.decoded.data = op;
                    result->elements[i].filterOperands[j].content.decoded.type = &UA_TYPES[UA_TYPES_SIMPLEATTRIBUTEOPERAND];
                } else if (currentOperand.canConvert<QOpcUaAttributeOperand>()) {
                    UA_AttributeOperand *op = UA_AttributeOperand_new();
                    UA_AttributeOperand_init(op);
                    QOpcUaAttributeOperand operand = currentOperand.value<QOpcUaAttributeOperand>();
                    op->attributeId = QOpen62541ValueConverter::toUaAttributeId(operand.attributeId());
                    QOpen62541ValueConverter::scalarFromQt<UA_String, QString>(operand.indexRange(), &op->indexRange);
                    op->alias = UA_STRING_ALLOC(operand.alias().toUtf8().constData());
                    if (!operand.nodeId().isEmpty())
                        op->nodeId = Open62541Utils::nodeIdFromQString(operand.nodeId());
                    op->browsePath.elementsSize = operand.browsePath().size();
                    op->browsePath.elements = static_cast<UA_RelativePathElement *>(
                                UA_Array_new(operand.browsePathRef().size(), &UA_TYPES[UA_TYPES_RELATIVEPATHELEMENT]));

                    for (int k = 0; k < operand.browsePathRef().size(); ++k) {
                        UA_RelativePathElement_init(&op->browsePath.elements[k]);
                        op->browsePath.elements[k].includeSubtypes = operand.browsePath().at(k).includeSubtypes();
                        op->browsePath.elements[k].isInverse = operand.browsePath().at(k).isInverse();
                        if (!operand.browsePath().at(k).referenceTypeId().isEmpty())
                            op->browsePath.elements[k].referenceTypeId = Open62541Utils::nodeIdFromQString(
                                        operand.browsePath().at(k).referenceTypeId());
                        QOpen62541ValueConverter::scalarFromQt<UA_QualifiedName, QOpcUaQualifiedName>(
                                    operand.browsePath().at(k).targetName(), &op->browsePath.elements[k].targetName);
                    }

                    result->elements[i].filterOperands[j].content.decoded.data = op;
                    result->elements[i].filterOperands[j].content.decoded.type = &UA_TYPES[UA_TYPES_ATTRIBUTEOPERAND];
                } else {
                    qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Unknown filter operand type for event filter" <<
                                                             filter.whereClause().at(i).filterOperands().at(j).typeName();
                    UA_ContentFilter_clear(result);
                    return false;
                }
            }
        }
    }
    return true;
}

QOpcUaEventFilterResult QOpen62541Subscription::convertEventFilterResult(UA_ExtensionObject *obj)
{
    QOpcUaEventFilterResult result;

    if (!obj)
        return result;

    if (obj->encoding == UA_EXTENSIONOBJECT_DECODED && obj->content.decoded.type == &UA_TYPES[UA_TYPES_EVENTFILTERRESULT]) {
        UA_EventFilterResult *filterResult = static_cast<UA_EventFilterResult *>(obj->content.decoded.data);

        for (size_t i = 0; i < filterResult->selectClauseResultsSize; ++i)
            result.selectClauseResultsRef().append(static_cast<QOpcUa::UaStatusCode>(filterResult->selectClauseResults[i]));

        for (size_t i = 0; i < filterResult->whereClauseResult.elementResultsSize; ++i) {
            QOpcUaContentFilterElementResult temp;
            temp.setStatusCode(static_cast<QOpcUa::UaStatusCode>(filterResult->whereClauseResult.elementResults[i].statusCode));
            for (size_t j = 0; j < filterResult->whereClauseResult.elementResults[i].operandStatusCodesSize; ++j)
                temp.operandStatusCodesRef().append(static_cast<QOpcUa::UaStatusCode>(
                                                        filterResult->whereClauseResult.elementResults[i].operandStatusCodes[j]));
            result.whereClauseResultsRef().append(temp);
        }
    }

    return result;
}

bool QOpen62541Subscription::modifySubscriptionParameters(quint64 nodeHandle, QOpcUa::NodeAttribute attr, const QOpcUaMonitoringParameters::Parameter &item, const QVariant &value)
{
    QOpcUaMonitoringParameters p;

    UA_ModifySubscriptionRequest req;
    UA_ModifySubscriptionRequest_init(&req);
    req.subscriptionId = m_subscriptionId;
    req.requestedPublishingInterval = m_interval;
    req.requestedLifetimeCount = m_lifetimeCount;
    req.requestedMaxKeepAliveCount = m_maxKeepaliveCount;
    req.maxNotificationsPerPublish = m_maxNotificationsPerPublish;

    bool match = true;

    switch (item) {
    case QOpcUaMonitoringParameters::Parameter::PublishingInterval: {
        bool ok;
        req.requestedPublishingInterval = value.toDouble(&ok);
        if (!ok) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify PublishingInterval, value is not a double";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
            return true;
        }
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::LifetimeCount: {
        bool ok;
        req.requestedLifetimeCount = value.toUInt(&ok);
        if (!ok) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify LifetimeCount, value is not an integer";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
            return true;
        }
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::MaxKeepAliveCount: {
        bool ok;
        req.requestedMaxKeepAliveCount = value.toUInt(&ok);
        if (!ok) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify MaxKeepAliveCount, value is not an integer";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
            return true;
        }
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::Priority: {
        bool ok;
        req.priority = value.toUInt(&ok);
        if (!ok) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify Priority, value is not an integer";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
            return true;
        }
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::MaxNotificationsPerPublish: {
        bool ok;
        req.maxNotificationsPerPublish = value.toUInt(&ok);
        if (!ok) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify MaxNotificationsPerPublish, value is not an integer";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
            return true;
        }
        break;
    }
    default:
        match = false;
        break;
    }

    if (match) {
        UA_ModifySubscriptionResponse res = UA_Client_Subscriptions_modify(m_backend->m_uaclient, req);

        if (res.responseHeader.serviceResult != UA_STATUSCODE_GOOD) {
            p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult));
            emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
        } else {
            QOpcUaMonitoringParameters::Parameters changed = item;
            if (!qFuzzyCompare(res.revisedPublishingInterval, m_interval))
                changed |= QOpcUaMonitoringParameters::Parameter::PublishingInterval;
            if (res.revisedLifetimeCount != m_lifetimeCount)
                changed |= QOpcUaMonitoringParameters::Parameter::LifetimeCount;
            if (res.revisedMaxKeepAliveCount != m_maxKeepaliveCount)
                changed |= QOpcUaMonitoringParameters::Parameter::MaxKeepAliveCount;

            m_lifetimeCount = res.revisedLifetimeCount;
            m_maxKeepaliveCount = res.revisedMaxKeepAliveCount;
            m_interval = res.revisedPublishingInterval;
            if (item == QOpcUaMonitoringParameters::Parameter::Priority)
                m_priority = value.toUInt();
            if (item == QOpcUaMonitoringParameters::Parameter::MaxNotificationsPerPublish)
                m_maxNotificationsPerPublish = value.toUInt();

            p.setStatusCode(QOpcUa::UaStatusCode::Good);
            p.setPublishingInterval(m_interval);
            p.setLifetimeCount(m_lifetimeCount);
            p.setMaxKeepAliveCount(m_maxKeepaliveCount);
            p.setPriority(m_priority);
            p.setMaxNotificationsPerPublish(m_maxNotificationsPerPublish);

            for (auto it : std::as_const(m_itemIdToItemMapping))
                emit m_backend->monitoringStatusChanged(it->handle, it->attr, changed, p);
        }
        return true;
    }
    return false;
}

bool QOpen62541Subscription::modifyMonitoredItemParameters(quint64 nodeHandle, QOpcUa::NodeAttribute attr, const QOpcUaMonitoringParameters::Parameter &item, const QVariant &value)
{
    MonitoredItem *monItem = getItemForAttribute(nodeHandle, attr);
    QOpcUaMonitoringParameters p = monItem->parameters;

    UA_ModifyMonitoredItemsRequest req;
    UA_ModifyMonitoredItemsRequest_init(&req);
    UaDeleter<UA_ModifyMonitoredItemsRequest> requestDeleter(&req, UA_ModifyMonitoredItemsRequest_clear);
    req.subscriptionId = m_subscriptionId;
    req.itemsToModifySize = 1;
    req.itemsToModify = UA_MonitoredItemModifyRequest_new();
    UA_MonitoredItemModifyRequest_init(req.itemsToModify);
    req.itemsToModify->monitoredItemId = monItem->monitoredItemId;
    req.itemsToModify->requestedParameters.discardOldest = monItem->parameters.discardOldest();
    req.itemsToModify->requestedParameters.queueSize = monItem->parameters.queueSize();
    req.itemsToModify->requestedParameters.samplingInterval = monItem->parameters.samplingInterval();
    req.itemsToModify->monitoredItemId = monItem->monitoredItemId;
    req.itemsToModify->requestedParameters.clientHandle = monItem->clientHandle;

    bool match = true;

    switch (item) {
    case QOpcUaMonitoringParameters::Parameter::DiscardOldest: {
        if (value.metaType().id() != QMetaType::Bool) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify DiscardOldest, value is not a bool";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
            return true;
        }
        req.itemsToModify->requestedParameters.discardOldest = value.toBool();
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::QueueSize: {
        if (value.metaType().id() != QMetaType::UInt) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify QueueSize, value is not an integer";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
            return true;
        }
        req.itemsToModify->requestedParameters.queueSize = value.toUInt();
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::SamplingInterval: {
        if (value.metaType().id() != QMetaType::Double) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify SamplingInterval, value is not a double";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
            return true;
        }
        req.itemsToModify->requestedParameters.samplingInterval = value.toDouble();
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::Filter: {
        UA_ExtensionObject filter = createFilter(value);
        if (filter.content.decoded.data)
            req.itemsToModify->requestedParameters.filter = filter;
        else {
            qCDebug(QT_OPCUA_PLUGINS_OPEN62541) << "Unable to modify filter, filter creation failed";
            p.setStatusCode(QOpcUa::UaStatusCode::BadInternalError);
            emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
            return true;
        }
        break;
    }
    default:
        match = false;
        break;
    }

    if (match) {
        if (item != QOpcUaMonitoringParameters::Parameter::Filter && p.filter().isValid()) {
            UA_ExtensionObject filter = createFilter(monItem->parameters.filter());
            if (filter.content.decoded.data)
                req.itemsToModify->requestedParameters.filter = filter;
            else {
                qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not modify monitored item, filter creation failed";
                p.setStatusCode(QOpcUa::UaStatusCode::BadInternalError);
                emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
                return true;
            }
        }

        UA_ModifyMonitoredItemsResponse res = UA_Client_MonitoredItems_modify(m_backend->m_uaclient, req);
        UaDeleter<UA_ModifyMonitoredItemsResponse> responseDeleter(
                    &res, UA_ModifyMonitoredItemsResponse_clear);

        if (res.responseHeader.serviceResult != UA_STATUSCODE_GOOD || res.results[0].statusCode != UA_STATUSCODE_GOOD) {
            p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult == UA_STATUSCODE_GOOD ? res.results[0].statusCode : res.responseHeader.serviceResult));
            emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
            return true;
        } else {
            p.setStatusCode(QOpcUa::UaStatusCode::Good);
            QOpcUaMonitoringParameters::Parameters changed = item;
            if (!qFuzzyCompare(p.samplingInterval(), res.results[0].revisedSamplingInterval)) {
                p.setSamplingInterval(res.results[0].revisedSamplingInterval);
                changed |= QOpcUaMonitoringParameters::Parameter::SamplingInterval;
            }
            if (p.queueSize() != res.results[0].revisedQueueSize) {
                p.setQueueSize(res.results[0].revisedQueueSize);
                changed |= QOpcUaMonitoringParameters::Parameter::QueueSize;
            }

            if (item == QOpcUaMonitoringParameters::Parameter::DiscardOldest) {
                p.setDiscardOldest(value.toBool());
                changed |= QOpcUaMonitoringParameters::Parameter::DiscardOldest;
            }

            if (item == QOpcUaMonitoringParameters::Parameter::Filter) {
                changed |= QOpcUaMonitoringParameters::Parameter::Filter;
                if (value.canConvert<QOpcUaMonitoringParameters::DataChangeFilter>())
                    p.setFilter(value.value<QOpcUaMonitoringParameters::DataChangeFilter>());
                else if (value.canConvert<QOpcUaMonitoringParameters::EventFilter>())
                    p.setFilter(value.value<QOpcUaMonitoringParameters::EventFilter>());
                if (res.results[0].filterResult.content.decoded.type == &UA_TYPES[UA_TYPES_EVENTFILTERRESULT])
                    p.setFilterResult(convertEventFilterResult(&res.results[0].filterResult));
            }

            emit m_backend->monitoringStatusChanged(nodeHandle, attr, changed, p);

            monItem->parameters = p;
        }
        return true;
    }
    return false;
}

QT_END_NAMESPACE

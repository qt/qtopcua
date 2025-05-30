// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "quacppsubscription.h"
#include "quacppclient.h"
#include "quacpputils.h"
#include "quacppvalueconverter.h"

#include "qopcuaattributeoperand.h"
#include "qopcuacontentfilterelementresult.h"
#include "qopcuaelementoperand.h"
#include "qopcualiteraloperand.h"
#include "qopcuaeventfilterresult.h"

#include <QtCore/QLoggingCategory>

#include <uasession.h>
#include <uaeventfilterresult.h>

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
        qCDebug(QT_OPCUA_PLUGINS_UACPP) << "Subscription succeeded, returning id:" << subId;
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

bool QUACppSubscription::addAttributeMonitoredItem(quint64 handle, QOpcUa::NodeAttribute attr, const UaNodeId &id, QOpcUaMonitoringParameters parameters)
{
    qCDebug(QT_OPCUA_PLUGINS_UACPP) << "Adding monitored Item for" << id.toString().toUtf8() << attr;
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
    if (parameters.filter().isValid()) {
        createRequests[0].RequestedParameters.Filter = createFilter(parameters.filter());
        if (!createRequests[0].RequestedParameters.Filter.Body.EncodeableObject.Object) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Unable to create the monitored item, filter creation failed";
            QOpcUaMonitoringParameters s;
            s.setStatusCode(QOpcUa::UaStatusCode::BadInternalError);
            emit m_backend->monitoringEnableDisable(handle, attr, true, s);
            return false;
        }
    }

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
    s.setMonitoredItemId(createResults[0].MonitoredItemId);

    // Store information
    const auto key = qMakePair(handle, attr);
    const auto value = qMakePair(createResults[0], parameters);
    m_monitoredItems.insert(key, value);
    m_monitoredIds.insert(monitorId, key);
    monitorId++;

    if (UaNodeId(createResults[0].FilterResult.TypeId.NodeId) == UaNodeId(OpcUaId_EventFilterResult_Encoding_DefaultBinary, 0))
        s.setFilterResult(convertEventFilterResult(createResults[0].FilterResult));
    else
        s.clearFilterResult(); // The server did not return an EventFilterResult
    emit m_backend->monitoringEnableDisable(handle, attr, true, s);

    return true;
}

void QUACppSubscription::modifyMonitoring(quint64 handle, QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, QVariant value)
{
    QOpcUaMonitoringParameters p;
    p.setStatusCode(QOpcUa::UaStatusCode::BadNotImplemented);
    const auto key = qMakePair(handle, attr);

    if (!m_monitoredItems.contains(key)) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP, "Could not modify parameter, there are no monitored items");
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

bool QUACppSubscription::removeAttributeMonitoredItem(quint64 nodeHandle, QOpcUa::NodeAttribute attr)
{
    qCDebug(QT_OPCUA_PLUGINS_UACPP) << "Removing monitored Item for" << attr;

    const QPair<quint64, QOpcUa::NodeAttribute> pair(nodeHandle, attr);
    if (!m_monitoredItems.contains(pair)) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Trying to remove unknown monitored item for" << attr;
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
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Failed to remove monitored item for" << attr;
        return false;
    }

    QOpcUaMonitoringParameters s;
    s.setSubscriptionId(m_nativeSubscription->subscriptionId());
    s.setPublishingInterval(m_nativeSubscription->publishingInterval());
    s.setMaxKeepAliveCount(m_nativeSubscription->maxKeepAliveCount());
    s.setLifetimeCount(m_nativeSubscription->lifetimeCount());
    s.setStatusCode(static_cast<QOpcUa::UaStatusCode>(result.statusCode()));
    emit m_backend->monitoringEnableDisable(nodeHandle, attr, false, s);

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
    return m_subscriptionParameters.subscriptionType();
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
        temp.setValue(var);
        temp.setServerTimestamp(QUACppValueConverter::toQDateTime(&dataNotifications[i].Value.ServerTimestamp));
        temp.setSourceTimestamp(QUACppValueConverter::toQDateTime(&dataNotifications[i].Value.SourceTimestamp));
        temp.setAttribute(m_monitoredIds[monitorId].second);
        temp.setStatusCode(QOpcUa::UaStatusCode::Good);

        emit m_backend->dataChangeOccurred(m_monitoredIds[monitorId].first, temp);
    }
}

void QUACppSubscription::newEvents(OpcUa_UInt32 clientSubscriptionHandle, UaEventFieldLists &eventFieldList)
{
    Q_UNUSED(clientSubscriptionHandle);

    for (quint32 i = 0; i < eventFieldList.length(); ++i) {
        const quint32 monitorId = eventFieldList[i].ClientHandle;

        if (!m_monitoredIds.contains(monitorId))
            continue;

        QVariantList eventFields;

        for (int j = 0; j < eventFieldList[i].NoOfEventFields; ++j)
            eventFields.append(QUACppValueConverter::toQVariant(eventFieldList[i].EventFields[j]));

        emit m_backend->eventOccurred(m_monitoredIds[monitorId].first, eventFields);
    }
}

OpcUa_ExtensionObject QUACppSubscription::createFilter(const QVariant &filterData)
{
    OpcUa_ExtensionObject obj;
    OpcUa_ExtensionObject_Initialize(&obj);

    if (filterData.canConvert<QOpcUaMonitoringParameters::DataChangeFilter>()) {
        createDataChangeFilter(filterData.value<QOpcUaMonitoringParameters::DataChangeFilter>(), &obj);
        return obj;
    }

    if (filterData.canConvert<QOpcUaMonitoringParameters::EventFilter>()) {
        createEventFilter(filterData.value<QOpcUaMonitoringParameters::EventFilter>(), &obj);
        return obj;
    }

    if (filterData.isValid())
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Could not create filter, invalid input.";

    return obj;
}

void QUACppSubscription::createDataChangeFilter(const QOpcUaMonitoringParameters::DataChangeFilter &filter, OpcUa_ExtensionObject *out)
{
    OpcUa_DataChangeFilter *uaFilter = nullptr;
    OpcUa_EncodeableObject_CreateExtension(&OpcUa_DataChangeFilter_EncodeableType,
                                           out, reinterpret_cast<OpcUa_Void **>(&uaFilter));
    if (!uaFilter) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Could not create DataChangeFilter";
        return;
    }

    uaFilter->DeadbandType = static_cast<OpcUa_UInt32>(filter.deadbandType());
    uaFilter->DeadbandValue = static_cast<OpcUa_Double>(filter.deadbandValue());
    uaFilter->Trigger = static_cast<OpcUa_DataChangeTrigger>(filter.trigger());
}

void QUACppSubscription::createEventFilter(const QOpcUaMonitoringParameters::EventFilter &filter, OpcUa_ExtensionObject *out)
{
    OpcUa_EventFilter *uaFilter = nullptr; // Use the stack structures because the C++ layer does not support AttributeOperand
    OpcUa_EncodeableObject_CreateExtension(&OpcUa_EventFilter_EncodeableType,
                                           out, reinterpret_cast<OpcUa_Void **>(&uaFilter));
    if (!uaFilter) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Could not create EventFilter";
        return;
    }

    // Select clause
    if (filter.selectClauses().size()) {
        UaSimpleAttributeOperands select;
        select.create(filter.selectClauses().size());

        for (int i = 0; i < filter.selectClauses().size(); ++i) {
            if (!filter.selectClauses()[i].typeId().isEmpty())
                UACppUtils::nodeIdFromQString(filter.selectClauses()[i].typeId()).copyTo(&select[i].TypeDefinitionId);
            UaString(filter.selectClauses()[i].indexRange().toUtf8().constData()).copyTo(&select[i].IndexRange);
            select[i].AttributeId = QUACppValueConverter::toUaAttributeId(filter.selectClauses()[i].attributeId());
            UaQualifiedNameArray path;
            path.create(filter.selectClauses()[i].browsePathRef().size());
            for (int j = 0; j < filter.selectClauses()[i].browsePathRef().size(); ++j) {
                UaQualifiedName(UaString(filter.selectClauses()[i].browsePathRef()[j].name().toUtf8().constData()),
                                filter.selectClauses()[i].browsePathRef()[j].namespaceIndex()).copyTo(&path[j]);
            }
            select[i].NoOfBrowsePath = filter.selectClauses()[i].browsePathRef().size();
            select[i].BrowsePath = path.detach();
        }
        uaFilter->NoOfSelectClauses = filter.selectClauses().size();
        uaFilter->SelectClauses = select.detach();
    }

    // Where clause
    if (filter.whereClause().size()) {
        uaFilter->WhereClause.NoOfElements = filter.whereClause().size();
        uaFilter->WhereClause.Elements = static_cast<OpcUa_ContentFilterElement *>(malloc(filter.whereClause().size()*sizeof(OpcUa_ContentFilterElement)));

        for (int i = 0; i < filter.whereClause().size(); ++i) {
            uaFilter->WhereClause.Elements[i].FilterOperator = static_cast<OpcUa_FilterOperator>(filter.whereClause()[i].filterOperator());
            uaFilter->WhereClause.Elements[i].NoOfFilterOperands = filter.whereClause()[i].filterOperandsRef().size();
            UaExtensionObjectArray operands;
            operands.create(filter.whereClause()[i].filterOperandsRef().size());

            for (int j = 0; j < filter.whereClause()[i].filterOperandsRef().size(); ++j) {
                if (filter.whereClause()[i].filterOperandsRef()[j].canConvert<QOpcUaElementOperand>()) {
                    OpcUa_ElementOperand *op;
                    OpcUa_EncodeableObject_CreateExtension(&OpcUa_ElementOperand_EncodeableType,
                                                           &operands[j],
                                                           reinterpret_cast<OpcUa_Void **>(&op));
                    if (!op) {
                        OpcUa_ExtensionObject_Clear(out);
                        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Could not allocate an ElementOperand for the event filter";
                        return;
                    }
                    op->Index = filter.whereClause()[i].filterOperandsRef()[j].value<QOpcUaElementOperand>().index();
                } else if (filter.whereClause()[i].filterOperandsRef()[j].canConvert<QOpcUaLiteralOperand>()) {
                    OpcUa_LiteralOperand *op;
                    OpcUa_EncodeableObject_CreateExtension(&OpcUa_LiteralOperand_EncodeableType,
                                                           &operands[j],
                                                           reinterpret_cast<OpcUa_Void **>(&op));
                    if (!op) {
                        OpcUa_ExtensionObject_Clear(out);
                        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Could not allocate a LiteralOperand for the event filter";
                        return ;
                    }
                    QOpcUaLiteralOperand litOp = filter.whereClause()[i].filterOperandsRef()[j].value<QOpcUaLiteralOperand>();
                    op->Value = QUACppValueConverter::toUACppVariant(litOp.value(), litOp.type());
                } else if (filter.whereClause()[i].filterOperandsRef()[j].canConvert<QOpcUaSimpleAttributeOperand>()) {
                    OpcUa_SimpleAttributeOperand *op;
                    OpcUa_EncodeableObject_CreateExtension(&OpcUa_SimpleAttributeOperand_EncodeableType,
                                                           &operands[j],
                                                           reinterpret_cast<OpcUa_Void **>(&op));
                    if (!op) {
                        OpcUa_ExtensionObject_Clear(out);
                        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Could not allocate a SimpleAttributeOperand for the event filter";
                        return;
                    }
                    QOpcUaSimpleAttributeOperand operand = filter.whereClause()[i].filterOperandsRef()[j].value<QOpcUaSimpleAttributeOperand>();
                    op->AttributeId = QUACppValueConverter::toUaAttributeId(operand.attributeId());
                    UaString(operand.indexRange().toUtf8().constData()).copyTo(&op->IndexRange);
                    if (!operand.typeId().isEmpty())
                        UACppUtils::nodeIdFromQString(operand.typeId()).copyTo(&op->TypeDefinitionId);
                    UaQualifiedNameArray path;
                    path.create(operand.browsePathRef().size());
                    for (int k = 0; k < operand.browsePathRef().size(); ++k) {
                        UaQualifiedName(UaString(operand.browsePathRef()[k].name().toUtf8().constData()), operand.browsePathRef()[k].namespaceIndex()).copyTo(&path[k]);
                    }
                    op->NoOfBrowsePath = operand.browsePathRef().size();
                    op->BrowsePath = path.detach();
                } else if (filter.whereClause()[i].filterOperandsRef()[j].canConvert<QOpcUaAttributeOperand>()) {
                    OpcUa_AttributeOperand *op;
                    OpcUa_EncodeableObject_CreateExtension(&OpcUa_AttributeOperand_EncodeableType,
                                                           &operands[j],
                                                           reinterpret_cast<OpcUa_Void **>(&op));
                    if (!op) {
                        OpcUa_ExtensionObject_Clear(out);
                        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Could not allocate an AttributeOperand for the event filter";
                        return;
                    }
                    QOpcUaAttributeOperand operand = filter.whereClause()[i].filterOperandsRef()[j].value<QOpcUaAttributeOperand>();
                    op->AttributeId = QUACppValueConverter::toUaAttributeId(operand.attributeId());
                    UaString(operand.indexRange().toUtf8().constData()).copyTo(&op->IndexRange);
                    if (!operand.nodeId().isEmpty())
                        UACppUtils::nodeIdFromQString(operand.nodeId()).copyTo(&op->NodeId);
                    UaString(operand.alias().toUtf8().constData()).copyTo(&op->Alias);
                    UaRelativePathElements path;
                    path.create(operand.browsePathRef().size());
                    op->BrowsePath.NoOfElements = operand.browsePathRef().size();
                    for (int k = 0; k < operand.browsePathRef().size(); ++k) {
                        path[k].IncludeSubtypes = operand.browsePathRef()[k].includeSubtypes();
                        path[k].IsInverse = operand.browsePathRef()[k].isInverse();
                        if (!operand.browsePathRef()[k].referenceTypeId().isEmpty())
                            UACppUtils::nodeIdFromQString(operand.browsePathRef()[k].referenceTypeId()).copyTo(&path[k].ReferenceTypeId);
                        UaQualifiedName(UaString(operand.browsePathRef()[k].targetName().name().toUtf8().constData()),
                                        operand.browsePathRef()[k].targetName().namespaceIndex()).copyTo(&path[k].TargetName);
                    }
                    op->BrowsePath.Elements = path.detach();

                } else {
                    qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Unknown filter operand type for event filter" << filter.whereClause()[i].filterOperands()[j].typeName();
                    OpcUa_ExtensionObject_Clear(out);
                    return;
                }
            }
            uaFilter->WhereClause.Elements[i].FilterOperands = operands.detach();
        }
    }
}

QOpcUaEventFilterResult QUACppSubscription::convertEventFilterResult(const OpcUa_ExtensionObject &obj)
{
    QOpcUaEventFilterResult result;

    if (UaNodeId(obj.TypeId.NodeId) == UaNodeId(OpcUaId_EventFilterResult_Encoding_DefaultBinary, 0)) {
        UaEventFilterResult filterResult(obj);

        UaStatusCodeArray arr;
        filterResult.getSelectClauseResults(arr);
        for (size_t i = 0; i < arr.length(); ++i)
            result.selectClauseResultsRef().append(static_cast<QOpcUa::UaStatusCode>(arr[i]));

        UaContentFilterResult contentFilterResult = filterResult.getWhereClauseResult();

        UaContentFilterElementResults elementResults;
        contentFilterResult.getElementResults(elementResults);

        for (size_t i = 0; i < elementResults.length(); ++i) {
            QOpcUaContentFilterElementResult temp;
            temp.setStatusCode(static_cast<QOpcUa::UaStatusCode>(elementResults[i].StatusCode));
            for (int j = 0; j < elementResults[i].NoOfOperandStatusCodes; ++j)
                temp.operandStatusCodesRef().append(static_cast<QOpcUa::UaStatusCode>(elementResults[i].OperandStatusCodes[j]));
            result.whereClauseResultsRef().append(temp);
        }
    }

    return result;
}

bool QUACppSubscription::modifySubscriptionParameters(quint64 nodeHandle, QOpcUa::NodeAttribute attr, const QOpcUaMonitoringParameters::Parameter &item, const QVariant &value)
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
            qCWarning(QT_OPCUA_PLUGINS_UACPP, "Could not modify PublishingInterval, value is not a double");
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
            return true;
        }
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::LifetimeCount: {
        bool ok;
        settings.lifetimeCount = value.toUInt(&ok);

        if (!ok) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP, "Could not modify LifetimeCount, value is not an integer");
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
            return true;
        }
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::MaxKeepAliveCount: {
        bool ok;
        settings.maxKeepAliveCount = value.toUInt(&ok);

        if (!ok) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP, "Could not modify MaxKeepAliveCount, value is not an integer");
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
        ServiceSettings service;
        const double oldPublishInterval = m_nativeSubscription->publishingInterval();
        const quint32 oldLifeTimeCount = m_nativeSubscription->lifetimeCount();
        const quint32 oldMaxKeepAlive = m_nativeSubscription->maxKeepAliveCount();
        UaStatus result = m_nativeSubscription->modifySubscription(service, settings);

        if (result.isBad()) {
            p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(result.statusCode()));
            emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
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

            for (auto it : std::as_const(m_monitoredIds))
                emit m_backend->monitoringStatusChanged(it.first, it.second, changed, p);
        }
        return true;
    }
    return false;
}

bool QUACppSubscription::modifyMonitoredItemParameters(quint64 nodeHandle, QOpcUa::NodeAttribute attr, const QOpcUaMonitoringParameters::Parameter &item, const QVariant &value)
{
    // Get hold of OpcUa_MonitoredItemCreateResult
    const QPair<quint64, QOpcUa::NodeAttribute> key(nodeHandle, attr);
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

    bool match = true;

    switch (item) {
    case QOpcUaMonitoringParameters::Parameter::DiscardOldest: {
        if (value.type() != QVariant::Bool) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Could not modify DiscardOldest, value is not a bool";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
            return true;
        }
        modifyRequest.RequestedParameters.DiscardOldest = value.toBool();
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::QueueSize: {
        if (value.type() != QVariant::UInt) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Could not modify QueueSize, value is not an integer";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
            return true;
        }
        modifyRequest.RequestedParameters.QueueSize = value.toUInt();
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::SamplingInterval: {
        if (value.type() != QVariant::Double) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Could not modify SamplingInterval, value is not a double";
            p.setStatusCode(QOpcUa::UaStatusCode::BadTypeMismatch);
            emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
            return true;
        }
        modifyRequest.RequestedParameters.SamplingInterval = value.toDouble();
        break;
    }
    case QOpcUaMonitoringParameters::Parameter::Filter: {
        modifyRequest.RequestedParameters.Filter = createFilter(value);
        if (!modifyRequest.RequestedParameters.Filter.Body.EncodeableObject.Object) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Could not modify the filter, filter creation failed";
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
        if (item != QOpcUaMonitoringParameters::Parameter::Filter && valuePair.second.filter().isValid()) {
            modifyRequest.RequestedParameters.Filter = createFilter(valuePair.second.filter());
            if (!modifyRequest.RequestedParameters.Filter.Body.EncodeableObject.Object) {
                qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Unable to modify the monitored item, filter creation failed";
                p.setStatusCode(QOpcUa::UaStatusCode::BadInternalError);
                emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
                return true;
            }
        }

        ServiceSettings service;
        UaMonitoredItemModifyRequests requests(1, &modifyRequest);
        UaMonitoredItemModifyResults results;
        UaStatus result = m_nativeSubscription->modifyMonitoredItems(service, OpcUa_TimestampsToReturn_Both, requests, results);

        OpcUa_MonitoredItemModifyRequest_Clear(&modifyRequest); // The C++ destructor does not free the members of the requests

        if (result.isBad() || OpcUa_IsNotGood(results[0].StatusCode)) {
            p.setStatusCode(static_cast<QOpcUa::UaStatusCode>(result.isGood() ?
                                                                  results[0].StatusCode :
                                                                  result.statusCode()));
            emit m_backend->monitoringStatusChanged(nodeHandle, attr, item, p);
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

            if (item == QOpcUaMonitoringParameters::Parameter::Filter) {
                changed |= QOpcUaMonitoringParameters::Parameter::Filter;
                if (value.canConvert<QOpcUaMonitoringParameters::DataChangeFilter>())
                    p.setFilter(value.value<QOpcUaMonitoringParameters::DataChangeFilter>());
                else if (value.canConvert<QOpcUaMonitoringParameters::EventFilter>())
                    p.setFilter(value.value<QOpcUaMonitoringParameters::EventFilter>());
                if (UaNodeId(results[0].FilterResult.TypeId.NodeId) == UaNodeId(OpcUaId_EventFilterResult_Encoding_DefaultBinary, 0))
                    p.setFilterResult(convertEventFilterResult(results[0].FilterResult));
            }

            emit m_backend->monitoringStatusChanged(nodeHandle, attr, changed, p);

            m_monitoredItems[key].second = p;
        }
        return true;
    }

    return false;
}

QT_END_NAMESPACE

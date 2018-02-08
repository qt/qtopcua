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

#include "quacppbackend.h"
#include "quacppclient.h"
#include "quacppsubscription.h"
#include "quacpputils.h"
#include "quacppvalueconverter.h"

#include <private/qopcuaclient_p.h>

#include <QtCore/QLoggingCategory>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QUuid>

#include <QtNetwork/QHostInfo>

#include <uaplatformlayer.h>
#include <uasession.h>
#include <uastring.h>

#include <limits>

// We only undef max and do not use NOMINMAX, as the UA SDK seems to rely on it.
#ifdef max
#undef max
#endif

quint32 UACppAsyncBackend::m_numClients = 0;
bool UACppAsyncBackend::m_platformLayerInitialized = false;

using namespace UaClientSdk;

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_UACPP)

UACppAsyncBackend::UACppAsyncBackend(QUACppClient *parent)
    : QOpcUaBackend()
    , m_clientImpl(parent)
{
    QMutexLocker locker(&m_lifecycleMutex);
    if (!m_platformLayerInitialized) {
        UaPlatformLayer::init();
        m_platformLayerInitialized = true;
    }
    m_numClients++;
    m_nativeSession = new UaSession();
}

UACppAsyncBackend::~UACppAsyncBackend()
{
    if (m_nativeSession) {
        if (m_nativeSession->isConnected() != OpcUa_False) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "UACPP: Deleting backend while still connected";
            ServiceSettings serviceSettings;
            m_nativeSession->disconnect(serviceSettings, OpcUa_True);
        }
        qCDebug(QT_OPCUA_PLUGINS_UACPP) << "UACPP: Deleting session";
        delete m_nativeSession;
    }
    QMutexLocker locker(&m_lifecycleMutex);
    m_numClients--;
    if (!m_numClients && m_platformLayerInitialized) {
        UaPlatformLayer::cleanup();
        m_platformLayerInitialized = false;
    }
}

void UACppAsyncBackend::connectionStatusChanged(OpcUa_UInt32 clientConnectionId, UaClientSdk::UaClient::ServerStatus serverStatus)
{
    Q_UNUSED(clientConnectionId);

    switch (serverStatus) {
    case UaClient::Disconnected:
        qCDebug(QT_OPCUA_PLUGINS_UACPP) << "Connection closed";
        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::NoError);
        break;
    case UaClient::Connected:
        qCDebug(QT_OPCUA_PLUGINS_UACPP) << "Connection established";
        emit stateAndOrErrorChanged(QOpcUaClient::Connected, QOpcUaClient::NoError);
        break;
    case UaClient::ConnectionWarningWatchdogTimeout:
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Unimplemented: Connection status changed to ConnectionWarningWatchdogTimeout";
        break;
    case UaClient::ConnectionErrorApiReconnect:
        qCDebug(QT_OPCUA_PLUGINS_UACPP) << "Connection status changed to ConnectionErrorApiReconnect";
        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::ConnectionError);
        break;
    case UaClient::ServerShutdown:
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Unimplemented: Connection status changed to ServerShutdown";
        break;
    case UaClient::NewSessionCreated:
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Unimplemented: Connection status changed to NewSessionCreated";
        break;
    }
}

void UACppAsyncBackend::browseChildren(uintptr_t handle, const UaNodeId &id, QOpcUa::ReferenceTypeId referenceType, QOpcUa::NodeClasses nodeClassMask)
{
    UaStatus status;
    ServiceSettings serviceSettings;
    BrowseContext browseContext;
    UaByteString continuationPoint;
    UaReferenceDescriptions referenceDescriptions;

    browseContext.referenceTypeId = UaNodeId(static_cast<OpcUa_UInt32>(referenceType));
    browseContext.nodeClassMask = nodeClassMask;

    QStringList result;
    QVector<QOpcUaReferenceDescription> ret;
    status = m_nativeSession->browse(serviceSettings, id, browseContext, continuationPoint, referenceDescriptions);
    bool initialBrowse = true;
    do {
        if (!initialBrowse)
            status = m_nativeSession->browseNext(serviceSettings, OpcUa_False, continuationPoint, referenceDescriptions);

        if (status.isBad()) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Could not browse children.";
            break;
        }

        initialBrowse = false;

        for (quint32 i = 0; i < referenceDescriptions.length(); ++i)
        {
            const UaNodeId id(referenceDescriptions[i].NodeId.NodeId);
            const UaString uastr(id.toXmlString());
            result.append(QString::fromUtf8(uastr.toUtf8(), uastr.size()));

            QOpcUaReferenceDescription temp;
            temp.setNodeId(UACppUtils::nodeIdToQString(referenceDescriptions[i].NodeId.NodeId));
            temp.setRefType(static_cast<QOpcUa::ReferenceTypeId>(UaNodeId(referenceDescriptions[i].ReferenceTypeId).identifierNumeric()));
            temp.setNodeClass(static_cast<QOpcUa::NodeClass>(referenceDescriptions[i].NodeClass));
            temp.setBrowseName(QUACppValueConverter::scalarToQVariant<QOpcUa::QQualifiedName, OpcUa_QualifiedName>(
                                   &referenceDescriptions[i].BrowseName, QMetaType::Type::UnknownType).value<QOpcUa::QQualifiedName>());
            temp.setDisplayName(QUACppValueConverter::scalarToQVariant<QOpcUa::QLocalizedText, OpcUa_LocalizedText>(
                                    &referenceDescriptions[i].DisplayName, QMetaType::Type::UnknownType).value<QOpcUa::QLocalizedText>());
            ret.append(temp);
        }
    } while (continuationPoint.length() > 0);

    emit browseFinished(handle, ret, static_cast<QOpcUa::UaStatusCode>(status.statusCode()));
}

void UACppAsyncBackend::connectToEndpoint(const QUrl &url)
{
    UaStatus result;

    UaString uaUrl(url.toString(QUrl::RemoveUserInfo).toUtf8().constData());
    SessionConnectInfo sessionConnectInfo;
    UaString sNodeName(QHostInfo::localHostName().toUtf8().constData());

    sessionConnectInfo.sApplicationName = "QtOpcUA Unified Automation Backend";
    // Use the host name to generate a unique application URI
    sessionConnectInfo.sApplicationUri  = UaString("urn:%1:Qt:OpcUAClient").arg(sNodeName);
    sessionConnectInfo.sProductUri      = "urn:Qt:OpcUAClient";
    sessionConnectInfo.sSessionName     = sessionConnectInfo.sApplicationUri;
    sessionConnectInfo.applicationType = OpcUa_ApplicationType_Client;

    SessionSecurityInfo sessionSecurityInfo;
    if (url.userName().length()) {
        UaString username(url.userName().toUtf8().constData());
        UaString password(url.password().toUtf8().constData());
        sessionSecurityInfo.setUserPasswordUserIdentity(username, password);
    }

    result = m_nativeSession->connect(uaUrl, sessionConnectInfo, sessionSecurityInfo, this);

    if (result.isNotGood()) {
        // ### TODO: Check for bad syntax, which is the "wrong url" part
        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::AccessDenied);
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Failed to connect: " << QString::fromUtf8(result.toString().toUtf8());
        return;
    }
}

void UACppAsyncBackend::disconnectFromEndpoint()
{
    UaStatus result;

    ServiceSettings serviceSettings; // Default settings
    const OpcUa_Boolean deleteSubscriptions{OpcUa_True};

    result = m_nativeSession->disconnect(serviceSettings, deleteSubscriptions);
    QOpcUaClient::ClientError err = QOpcUaClient::NoError;
    if (result.isNotGood()) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Failed to disconnect.";
        err = QOpcUaClient::UnknownError;
    }

    emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, err);
}

inline OpcUa_UInt32 toUaAttributeId(QOpcUa::NodeAttribute attr)
{
    const int attributeIdUsedBits = 22;
    for (int i = 0; i < attributeIdUsedBits; ++i)
        if (static_cast<int>(attr) == (1 << i))
            return static_cast<OpcUa_UInt32>(i + 1);

    return static_cast<OpcUa_UInt32>(0);
}

void UACppAsyncBackend::readAttributes(uintptr_t handle, const UaNodeId &id, QOpcUa::NodeAttributes attr, QString indexRange)
{
    UaStatus result;

    ServiceSettings settings;
    UaReadValueIds nodeToRead;
    UaDataValues values;
    UaDiagnosticInfos diagnosticInfos;

    QVector<QOpcUaReadResult> vec;

    int attributeSize = 0;

    qt_forEachAttribute(attr, [&](QOpcUa::NodeAttribute attribute){
        attributeSize++;
        nodeToRead.resize(attributeSize);
        id.copyTo(&nodeToRead[attributeSize - 1].NodeId);
        nodeToRead[attributeSize - 1].AttributeId = toUaAttributeId(attribute);
        if (indexRange.size()) {
            UaString ir(indexRange.toUtf8().constData());
            ir.copyTo(&nodeToRead[attributeSize - 1].IndexRange);
        }
        QOpcUaReadResult temp;
        temp.attributeId = attribute;
        vec.push_back(temp);
    });

    result = m_nativeSession->read(settings,
                                   0,
                                   OpcUa_TimestampsToReturn_Both,
                                   nodeToRead,
                                   values,
                                   diagnosticInfos);
    if (result.isBad()) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Reading attributes failed:" << result.toString().toUtf8();
    } else {
        for (int i = 0; i < vec.size(); ++i) {
            vec[i].statusCode = static_cast<QOpcUa::UaStatusCode>(values[i].StatusCode);
            vec[i].value = QUACppValueConverter::toQVariant(values[i].Value);
            vec[i].serverTimestamp = QUACppValueConverter::toQDateTime(&values[i].ServerTimestamp);
            vec[i].sourceTimestamp = QUACppValueConverter::toQDateTime(&values[i].SourceTimestamp);
        }
    }

    emit attributesRead(handle, vec, static_cast<QOpcUa::UaStatusCode>(result.statusCode()));
}

void UACppAsyncBackend::writeAttribute(uintptr_t handle, const UaNodeId &id, QOpcUa::NodeAttribute attrId, QVariant value, QOpcUa::Types type, QString indexRange)
{
    if (type == QOpcUa::Types::Undefined && attrId != QOpcUa::NodeAttribute::Value)
        type = attributeIdToTypeId(attrId);

    UaStatus result;
    ServiceSettings settings;
    UaWriteValues nodesToWrite;
    UaStatusCodeArray writeResults;
    UaDiagnosticInfos diagnosticInfos;

    nodesToWrite.create(1);
    id.copyTo(&nodesToWrite[0].NodeId);
    nodesToWrite[0].AttributeId = QUACppValueConverter::toUaAttributeId(attrId);
    nodesToWrite[0].Value.Value = QUACppValueConverter::toUACppVariant(value, type);
    if (indexRange.size()) {
        UaString ir(indexRange.toUtf8());
        ir.copyTo(&nodesToWrite[0].IndexRange);
    }
    result = m_nativeSession->write(settings, nodesToWrite, writeResults, diagnosticInfos);

    if (result.isBad())
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Writing attribute failed:" << result.toString().toUtf8();

    emit attributeWritten(handle, attrId, result.isGood() ? value : QVariant(), writeResults.length() ?
                              static_cast<QOpcUa::UaStatusCode>(writeResults[0]) : static_cast<QOpcUa::UaStatusCode>(result.statusCode()));
}

void UACppAsyncBackend::writeAttributes(uintptr_t handle, const UaNodeId &id, QOpcUaNode::AttributeMap toWrite, QOpcUa::Types /*valueAttributeType*/)
{
    if (toWrite.size() == 0) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP, "No values to be written");
        emit attributeWritten(handle, QOpcUa::NodeAttribute::None, QVariant(), QOpcUa::UaStatusCode::BadNothingToDo);
        return;
    }

    UaStatus result;
    ServiceSettings settings;
    UaWriteValues nodesToWrite;
    UaStatusCodeArray writeResults;
    UaDiagnosticInfos diagnosticInfos;

    nodesToWrite.create(toWrite.size());
    quint32 index = 0;
    for (auto it = toWrite.constBegin(); it != toWrite.constEnd(); ++it, ++index) {
        id.copyTo(&nodesToWrite[index].NodeId);
        QOpcUa::Types type = attributeIdToTypeId(it.key());
        nodesToWrite[index].AttributeId = QUACppValueConverter::toUaAttributeId(it.key());
        nodesToWrite[index].Value.Value = QUACppValueConverter::toUACppVariant(it.value(), type);
    }

    result = m_nativeSession->write(settings, nodesToWrite, writeResults, diagnosticInfos);

    if (result.isBad())
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Writing attribute failed:" << result.toString().toUtf8();

    index = 0;
    for (auto it = toWrite.constBegin(); it != toWrite.constEnd(); ++it, ++index) {
        QOpcUa::UaStatusCode status = index < writeResults.length() ?
                    static_cast<QOpcUa::UaStatusCode>(writeResults[index]) : static_cast<QOpcUa::UaStatusCode>(result.statusCode());
        emit attributeWritten(handle, it.key(), it.value(), status);
    }
}

void UACppAsyncBackend::enableMonitoring(uintptr_t handle, const UaNodeId &id, QOpcUa::NodeAttributes attr, const QOpcUaMonitoringParameters &settings)
{
    QUACppSubscription *usedSubscription = nullptr;

    // Create a new subscription if necessary
    if (settings.subscriptionId()) {
        auto sub = m_subscriptions.find(settings.subscriptionId());
        if (sub == m_subscriptions.end()) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP, "There is no subscription with id %u", settings.subscriptionId());

            qt_forEachAttribute(attr, [&](QOpcUa::NodeAttribute attribute){
                QOpcUaMonitoringParameters s;
                s.setStatusCode(QOpcUa::UaStatusCode::BadSubscriptionIdInvalid);
                emit monitoringEnableDisable(handle, attribute, true, s);
            });
            return;
        }
        usedSubscription = sub.value(); // Ignore interval != subscription.interval
    } else {
        usedSubscription = getSubscription(settings);
    }

    if (!usedSubscription) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP, "Could not create subscription with interval %f", settings.publishingInterval());
        qt_forEachAttribute(attr, [&](QOpcUa::NodeAttribute attribute){
            QOpcUaMonitoringParameters s;
            s.setStatusCode(QOpcUa::UaStatusCode::BadSubscriptionIdInvalid);
            emit monitoringEnableDisable(handle, attribute, true, s);
        });
        return;
    }

    qt_forEachAttribute(attr, [&](QOpcUa::NodeAttribute attribute){
        bool success = usedSubscription->addAttributeMonitoredItem(handle, attribute, id, settings);
        if (success)
            m_attributeMapping[handle][attribute] = usedSubscription;
    });

    if (usedSubscription->monitoredItemsCount() == 0)
        removeSubscription(usedSubscription->subscriptionId()); // No items were added
}

void UACppAsyncBackend::modifyMonitoring(uintptr_t handle, QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, QVariant value)
{
    QUACppSubscription *subscription = getSubscriptionForItem(handle, attr);
    if (!subscription) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP, "Could not modify parameter for %lu, the monitored item does not exist", handle);
        QOpcUaMonitoringParameters p;
        p.setStatusCode(QOpcUa::UaStatusCode::BadMonitoredItemIdInvalid);
        emit monitoringStatusChanged(handle, attr, item, p);
        return;
    }

    subscription->modifyMonitoring(handle, attr, item, value);
}

void UACppAsyncBackend::disableMonitoring(uintptr_t handle, QOpcUa::NodeAttributes attr)
{
    qt_forEachAttribute(attr, [&](QOpcUa::NodeAttribute attribute){
        QUACppSubscription *sub = getSubscriptionForItem(handle, attribute);
        if (sub) {
            sub->removeAttributeMonitoredItem(handle, attribute);
            if (sub->monitoredItemsCount() == 0)
                removeSubscription(sub->subscriptionId());
        }
    });
}

void UACppAsyncBackend::callMethod(uintptr_t handle, const UaNodeId &objectId, const UaNodeId &methodId, QVector<QOpcUa::TypedVariant> args)
{
    ServiceSettings settings;
    CallIn in;

    in.objectId = objectId;
    in.methodId = methodId;

    if (args.size()) {
        in.inputArguments.resize(args.size());
        for (int i = 0; i < args.size(); ++i)
            in.inputArguments[i] = QUACppValueConverter::toUACppVariant(args[i].first, args[i].second);
    }

    CallOut out;

    UaStatus status = m_nativeSession->call(settings, in, out);
    if (status.isBad())
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Calling method failed";

    if (out.callResult.isBad())
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Method call returned a failure";

    QVariant result;

    if (out.outputArguments.length() > 1) {
        QVariantList resultList;
        for (quint32 i = 0; i < out.outputArguments.length(); ++i)
            resultList.append(QUACppValueConverter::toQVariant(out.outputArguments[i]));
        result = resultList;
    } else if (out.outputArguments.length() == 1) {
        result = QUACppValueConverter::toQVariant(out.outputArguments[0]);
    }

    emit methodCallFinished(handle, UACppUtils::nodeIdToQString(methodId), result, static_cast<QOpcUa::UaStatusCode>(status.statusCode()));
}

QUACppSubscription *UACppAsyncBackend::getSubscription(const QOpcUaMonitoringParameters &settings)
{
    if (settings.shared() == QOpcUaMonitoringParameters::SubscriptionType::Shared) {
        for (auto entry : qAsConst(m_subscriptions)) {
            if (qFuzzyCompare(entry->interval(), settings.publishingInterval()) && entry->shared() == QOpcUaMonitoringParameters::SubscriptionType::Shared)
                return entry;
        }
    }

    QUACppSubscription *sub = new QUACppSubscription(this, settings);
    quint32 id = sub->createOnServer();
    if (!id) {
        delete sub;
        return nullptr;
    }
    m_subscriptions[id] = sub;
    return sub;
}

QUACppSubscription *UACppAsyncBackend::getSubscriptionForItem(uintptr_t handle, QOpcUa::NodeAttribute attr)
{
    auto entriesForHandle = m_attributeMapping.find(handle);
    if (entriesForHandle == m_attributeMapping.end())
        return nullptr;
    auto subscription = entriesForHandle->find(attr);
    if (subscription == entriesForHandle->end())
        return nullptr;

    return subscription.value();
}

bool UACppAsyncBackend::removeSubscription(quint32 subscriptionId)
{
    auto sub = m_subscriptions.find(subscriptionId);
    if (sub != m_subscriptions.end()) {
        sub.value()->removeOnServer();
        delete sub.value();
        m_subscriptions.remove(subscriptionId);
        return true;
    }
    return false;
}

QT_END_NAMESPACE

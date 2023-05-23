// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "quacppbackend.h"
#include "quacppclient.h"
#include "quacppsubscription.h"
#include "quacpputils.h"
#include "quacppvalueconverter.h"

#include "qopcuaauthenticationinformation.h"

#include <private/qopcuaclient_p.h>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QLoggingCategory>
#include <QtCore/QStringList>
#include <QtCore/QUrl>

#include <QtNetwork/QHostInfo>

#include <uaplatformlayer.h>
#include <uasession.h>
#include <uastring.h>
#include <uadiscovery.h>
#include <uadatavalue.h>
#include <uapkicertificate.h>

#include <limits>

quint32 UACppAsyncBackend::m_numClients = 0;
bool UACppAsyncBackend::m_platformLayerInitialized = false;

using namespace UaClientSdk;

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_UACPP)

UACppAsyncBackend::UACppAsyncBackend(QUACppClient *parent)
    : QOpcUaBackend()
    , m_clientImpl(parent)
    , m_minPublishingInterval(0)
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
    cleanupSubscriptions();

    if (m_nativeSession) {
        if (m_nativeSession->isConnected() != OpcUa_False) {
            if (m_nativeSession->serverStatus() != UaClient::ConnectionErrorApiReconnect)
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
        cleanupSubscriptions();
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
        cleanupSubscriptions();
        break;
    case UaClient::ServerShutdown:
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Unimplemented: Connection status changed to ServerShutdown";
        break;
    case UaClient::NewSessionCreated:
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Unimplemented: Connection status changed to NewSessionCreated";
        break;
    }
}

bool UACppAsyncBackend::connectError(OpcUa_UInt32 clientConnectionId, UaClientSdk::UaClient::ConnectServiceType serviceType,
                                     const UaStatus &error, bool clientSideError)
{
    Q_UNUSED(clientConnectionId);

    QOpcUaErrorState errorState;
    errorState.setConnectionStep(connectionStepFromUaServiceType(serviceType));
    errorState.setErrorCode(static_cast<QOpcUa::UaStatusCode>(error.statusCode()));
    errorState.setClientSideError(clientSideError);
    errorState.setIgnoreError(false);

    // This signal is connected using Qt::BlockingQueuedConnection. It will place a metacall to a different thread and waits
    // until this metacall is fully handled before returning.
    emit QOpcUaBackend::connectError(&errorState);

    return errorState.ignoreError();
}

void UACppAsyncBackend::browse(quint64 handle, const UaNodeId &id, const QOpcUaBrowseRequest &request)
{
    UaStatus status;
    ServiceSettings serviceSettings;
    BrowseContext browseContext;
    UaByteString continuationPoint;
    UaReferenceDescriptions referenceDescriptions;

    browseContext.referenceTypeId = UACppUtils::nodeIdFromQString(request.referenceTypeId());
    browseContext.nodeClassMask = request.nodeClassMask();
    browseContext.includeSubtype = request.includeSubtypes();
    browseContext.browseDirection = static_cast<OpcUa_BrowseDirection>(request.browseDirection());

    QStringList result;
    QList<QOpcUaReferenceDescription> ret;
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
            QOpcUaExpandedNodeId expandedId;
            expandedId.setNamespaceUri(QString::fromUtf8(UaString(referenceDescriptions[i].NodeId.NamespaceUri).toUtf8()));
            expandedId.setServerIndex(referenceDescriptions[i].NodeId.ServerIndex);
            expandedId.setNodeId(UACppUtils::nodeIdToQString(referenceDescriptions[i].NodeId.NodeId));
            temp.setTargetNodeId(expandedId);
            expandedId.setNamespaceUri(QString::fromUtf8(UaString(referenceDescriptions[i].TypeDefinition.NamespaceUri).toUtf8()));
            expandedId.setServerIndex(referenceDescriptions[i].TypeDefinition.ServerIndex);
            expandedId.setNodeId(UACppUtils::nodeIdToQString(referenceDescriptions[i].TypeDefinition.NodeId));
            temp.setTypeDefinition(expandedId);
            temp.setRefTypeId(UACppUtils::nodeIdToQString(UaNodeId(referenceDescriptions[i].ReferenceTypeId)));
            temp.setNodeClass(static_cast<QOpcUa::NodeClass>(referenceDescriptions[i].NodeClass));
            temp.setBrowseName(QUACppValueConverter::scalarToQVariant<QOpcUaQualifiedName, OpcUa_QualifiedName>(
                                   &referenceDescriptions[i].BrowseName, QMetaType::Type::UnknownType).value<QOpcUaQualifiedName>());
            temp.setDisplayName(QUACppValueConverter::scalarToQVariant<QOpcUaLocalizedText, OpcUa_LocalizedText>(
                                    &referenceDescriptions[i].DisplayName, QMetaType::Type::UnknownType).value<QOpcUaLocalizedText>());
            temp.setIsForwardReference(referenceDescriptions[i].IsForward);
            ret.append(temp);
        }
    } while (continuationPoint.length() > 0);

    emit browseFinished(handle, ret, static_cast<QOpcUa::UaStatusCode>(status.statusCode()));
}

void UACppAsyncBackend::connectToEndpoint(const QOpcUaEndpointDescription &endpoint)
{
    if (m_nativeSession->isConnected())
        disconnectFromEndpoint();

    QString errorMessage;
    if (!verifyEndpointDescription(endpoint, &errorMessage)) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << errorMessage;
        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::ClientError::InvalidUrl);
        return;
    }

    emit stateAndOrErrorChanged(QOpcUaClient::Connecting, QOpcUaClient::NoError);

    const auto identity = m_clientImpl->m_client->applicationIdentity();
    const auto authInfo = m_clientImpl->m_client->authenticationInformation();
    const auto pkiConfig = m_clientImpl->m_client->pkiConfiguration();

    SessionConnectInfo sessionConnectInfo;
    sessionConnectInfo.sApplicationName = identity.applicationName().toUtf8().constData();
    sessionConnectInfo.sApplicationUri  = identity.applicationUri().toUtf8().constData();
    sessionConnectInfo.sProductUri      = identity.productUri().toUtf8().constData();
    sessionConnectInfo.sSessionName     = sessionConnectInfo.sApplicationUri;
    sessionConnectInfo.bAutomaticReconnect = OpcUa_False;
    sessionConnectInfo.applicationType = static_cast<OpcUa_ApplicationType>(identity.applicationType());

    SessionSecurityInfo sessionSecurityInfo;

    if (pkiConfig.isPkiValid()) {
        auto result = sessionSecurityInfo.initializePkiProviderOpenSSL(
                pkiConfig.revocationListDirectory().toUtf8().constData(),
                pkiConfig.trustListDirectory().toUtf8().constData(),
                pkiConfig.issuerRevocationListDirectory().toUtf8().constData(),
                pkiConfig.issuerListDirectory().toUtf8().constData());

        if (result.isNotGood()) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Failed to set up PKI: " << QString::fromUtf8(result.toString().toUtf8());
            emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::AccessDenied);
            return;
        }

        // set server certificate from endpoint
        sessionSecurityInfo.serverCertificate = UaByteString(endpoint.serverCertificate().length(), (OpcUa_Byte*)(endpoint.serverCertificate().constData()));
        // set SecurityPolicy
        sessionSecurityInfo.sSecurityPolicy = endpoint.securityPolicy().toUtf8().constData();
        // set MessageSecurityMode
        sessionSecurityInfo.messageSecurityMode = static_cast<OpcUa_MessageSecurityMode>(endpoint.securityMode());
    }

    if (QOpcUa::isSecurePolicy(endpoint.securityPolicy())) {
        // We are going to connect to a secure endpoint

        // try to load the client certificate
        const UaString certificateFilePath(pkiConfig.clientCertificateFile().toUtf8());
        const UaString privateKeyFilePath(pkiConfig.privateKeyFile().toUtf8());
        UaStatus result;

        QFile certFile(certificateFilePath.toUtf8());
        if (certFile.open(QIODevice::ReadOnly)) {
            const auto data = certFile.read(1000);
            certFile.close();
            if (data.contains("--BEGIN CERTIFICATE")) {
                qCWarning(QT_OPCUA_PLUGINS_UACPP) << "The UACPP backend supports only DER encoded certificates.";
                emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::AccessDenied);
                return;
            }
        } else {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Failed to load certificate: " << pkiConfig.clientCertificateFile();
            result = OpcUa_BadNotFound;
        }

        QFile keyFile(privateKeyFilePath.toUtf8());
        if (keyFile.open(QIODevice::ReadOnly)) {
            keyFile.close();
        } else {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Failed to load private key: " << pkiConfig.privateKeyFile();
            result = OpcUa_BadNotFound;
        }

        if (result.isGood()) {
            result = sessionSecurityInfo.loadClientCertificateOpenSSL(certificateFilePath, privateKeyFilePath);
            if (!result.isGood()) {
                qCWarning(QT_OPCUA_PLUGINS_UACPP) << "sessionSecurityInfo.loadClientCertificateOpenSSL failed";
                QString password;

                do {
                    // This signal is connected using Qt::BlockingQueuedConnection. It will place a metacall to a different thread and waits
                    // until this metacall is fully handled before returning.
                    emit QOpcUaBackend::passwordForPrivateKeyRequired(pkiConfig.privateKeyFile(), &password, !password.isEmpty());

                    if (password.isEmpty())
                        break;

                    result = sessionSecurityInfo.loadClientCertificateOpenSSL(certificateFilePath, privateKeyFilePath, UaString(password.toUtf8()));

                    if (result.isGood())
                        break;

                    qCWarning(QT_OPCUA_PLUGINS_UACPP) << "sessionSecurityInfo.loadClientCertificateOpenSSL failed";
                } while (true);
            }
        }

        if (result.isNotGood()) {
            emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::AccessDenied);
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Failed to connect using certificate authentication: " << QString::fromUtf8(result.toString().toUtf8());
            return;
        }
    } // end secure endpoint

    if (authInfo.authenticationType() == QOpcUaUserTokenPolicy::TokenType::Anonymous) {
        // nothing to do
    } else if (authInfo.authenticationType() == QOpcUaUserTokenPolicy::TokenType::Username) {
        const auto credentials = authInfo.authenticationData().value<QPair<QString, QString>>();
        UaString username(credentials.first.toUtf8().constData());
        UaString password(credentials.second.toUtf8().constData());
        sessionSecurityInfo.setUserPasswordUserIdentity(username, password);
        if (m_disableEncryptedPasswordCheck)
            sessionSecurityInfo.disableEncryptedPasswordCheck = OpcUa_True;
    } else {
        // QOpcUaUserTokenPolicy::TokenType::Certificate is currently unsupported

        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::UnsupportedAuthenticationInformation);
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Failed to connect: Selected authentication type"
                                          << authInfo.authenticationType() << "is not supported.";
        return;
    }

    const UaString uaUrl(endpoint.endpointUrl().toUtf8().constData());
    auto result = m_nativeSession->connect(uaUrl, sessionConnectInfo, sessionSecurityInfo, this);

    if (result.isNotGood()) {
        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::AccessDenied);
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Failed to connect:" << QString::fromUtf8(result.toString().toUtf8());

        if (result.code() == OpcUa_BadEncodingLimitsExceeded && QOpcUa::isSecurePolicy(endpoint.securityPolicy()))
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Reason may be not using a DER encoded client certificate";

        return;
    }
}

void UACppAsyncBackend::disconnectFromEndpoint()
{
    cleanupSubscriptions();

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

void UACppAsyncBackend::requestEndpoints(const QUrl &url)
{
    UaDiscovery discovery;
    ServiceSettings ServiceSettings;
    ClientSecurityInfo clientSecurityInfo;
    UaEndpointDescriptions endpoints;
    QList<QOpcUaEndpointDescription> ret;

    UaStatus res = discovery.getEndpoints(ServiceSettings, UaString(url.toString(QUrl::RemoveUserInfo).toUtf8().data()), clientSecurityInfo, endpoints);

    if (res.isGood() && endpoints.length()) {
        for (size_t i = 0; i < endpoints.length() ; ++i) {
            QOpcUaEndpointDescription temp;
            temp.setEndpointUrl(QString::fromUtf8(UaString(endpoints[i].EndpointUrl).toUtf8()));
            temp.serverRef().setApplicationUri(QString::fromUtf8(UaString(endpoints[i].Server.ApplicationUri).toUtf8()));
            temp.serverRef().setProductUri(QString::fromUtf8(UaString(endpoints[i].Server.ProductUri).toUtf8()));
            temp.serverRef().setApplicationName(QOpcUaLocalizedText(QString::fromUtf8(UaString(endpoints[i].Server.ApplicationName.Locale).toUtf8()),
                                                                 QString::fromUtf8(UaString(endpoints[i].Server.ApplicationName.Text).toUtf8())));
            temp.serverRef().setApplicationType(static_cast<QOpcUaApplicationDescription::ApplicationType>(endpoints[i].Server.ApplicationType));
            temp.serverRef().setGatewayServerUri(QString::fromUtf8(UaString(endpoints[i].Server.GatewayServerUri).toUtf8()));
            temp.serverRef().setDiscoveryProfileUri(QString::fromUtf8(UaString(endpoints[i].Server.DiscoveryProfileUri).toUtf8()));
            for (int j = 0; j < endpoints[i].Server.NoOfDiscoveryUrls; ++j) {
                QString url = QString::fromUtf8(UaString(endpoints[i].Server.DiscoveryUrls[j]).toUtf8());
                temp.serverRef().discoveryUrlsRef().append(url);
            }
            temp.setServerCertificate(QByteArray(reinterpret_cast<char *>(endpoints[i].ServerCertificate.Data), endpoints[i].ServerCertificate.Length));
            temp.setSecurityMode(static_cast<QOpcUaEndpointDescription::MessageSecurityMode>(endpoints[i].SecurityMode));
            temp.setSecurityPolicy(QString::fromUtf8(UaString(endpoints[i].SecurityPolicyUri).toUtf8()));
            for (int j = 0; j < endpoints[i].NoOfUserIdentityTokens; ++j) {
                QOpcUaUserTokenPolicy policy;
                policy.setPolicyId(QString::fromUtf8(UaString(endpoints[i].UserIdentityTokens[j].PolicyId).toUtf8()));
                policy.setTokenType(static_cast<QOpcUaUserTokenPolicy::TokenType>(endpoints[i].UserIdentityTokens[j].TokenType));
                policy.setIssuedTokenType(QString::fromUtf8(UaString(endpoints[i].UserIdentityTokens[j].IssuedTokenType).toUtf8()));
                policy.setIssuerEndpointUrl(QString::fromUtf8(UaString(endpoints[i].UserIdentityTokens[j].IssuerEndpointUrl).toUtf8()));
                policy.setSecurityPolicy(QString::fromUtf8(UaString(endpoints[i].UserIdentityTokens[j].SecurityPolicyUri).toUtf8()));
                temp.userIdentityTokensRef().append(policy);
            }
            temp.setTransportProfileUri(QString::fromUtf8(UaString(endpoints[i].TransportProfileUri).toUtf8()));
            temp.setSecurityLevel(endpoints[i].SecurityLevel);
            ret.append(temp);
        }
    } else {
        if (res.isGood())
            qWarning() << "Server returned an empty endpoint list";
        else
            qWarning() << "Failed to retrieve endpoints from " << url.toString(QUrl::RemoveUserInfo).toUtf8().constData()
                       << "with status" << res.toString().toUtf8();
    }

    emit endpointsRequestFinished(ret, static_cast<QOpcUa::UaStatusCode>(res.code()), url);
}

void UACppAsyncBackend::readAttributes(quint64 handle, const UaNodeId &id, QOpcUa::NodeAttributes attr, QString indexRange)
{
    UaStatus result;

    ServiceSettings settings;
    UaReadValueIds nodeToRead;
    UaDataValues values;
    UaDiagnosticInfos diagnosticInfos;

    QList<QOpcUaReadResult> vec;

    int attributeSize = 0;

    qt_forEachAttribute(attr, [&](QOpcUa::NodeAttribute attribute){
        attributeSize++;
        nodeToRead.resize(attributeSize);
        id.copyTo(&nodeToRead[attributeSize - 1].NodeId);
        nodeToRead[attributeSize - 1].AttributeId = QUACppValueConverter::toUaAttributeId(attribute);
        if (indexRange.size()) {
            UaString ir(indexRange.toUtf8().constData());
            ir.copyTo(&nodeToRead[attributeSize - 1].IndexRange);
        }
        QOpcUaReadResult temp;
        temp.setAttribute(attribute);
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
            vec[i].setStatusCode(static_cast<QOpcUa::UaStatusCode>(values[i].StatusCode));
            vec[i].setValue(QUACppValueConverter::toQVariant(values[i].Value));
            vec[i].setServerTimestamp(QUACppValueConverter::toQDateTime(&values[i].ServerTimestamp));
            vec[i].setSourceTimestamp(QUACppValueConverter::toQDateTime(&values[i].SourceTimestamp));
        }
    }

    emit attributesRead(handle, vec, static_cast<QOpcUa::UaStatusCode>(result.statusCode()));
}

void UACppAsyncBackend::writeAttribute(quint64 handle, const UaNodeId &id, QOpcUa::NodeAttribute attrId, QVariant value, QOpcUa::Types type, QString indexRange)
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

void UACppAsyncBackend::writeAttributes(quint64 handle, const UaNodeId &id, QOpcUaNode::AttributeMap toWrite, QOpcUa::Types valueAttributeType)
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
        if (type == QOpcUa::Types::Undefined)
            type = valueAttributeType;
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

void UACppAsyncBackend::enableMonitoring(quint64 handle, const UaNodeId &id, QOpcUa::NodeAttributes attr, const QOpcUaMonitoringParameters &settings)
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
        if (getSubscriptionForItem(handle, attribute)) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Monitored item for" << attribute << "has already been created";
            QOpcUaMonitoringParameters s;
            s.setStatusCode(QOpcUa::UaStatusCode::BadEntryExists);
            emit monitoringEnableDisable(handle, attribute, true, s);
        } else {
            bool success = usedSubscription->addAttributeMonitoredItem(handle, attribute, id, settings);
            if (success)
                m_attributeMapping[handle][attribute] = usedSubscription;
        }
    });

    if (usedSubscription->monitoredItemsCount() == 0)
        removeSubscription(usedSubscription->subscriptionId()); // No items were added
}

void UACppAsyncBackend::modifyMonitoring(quint64 handle, QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, QVariant value)
{
    QUACppSubscription *subscription = getSubscriptionForItem(handle, attr);
    if (!subscription) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Could not modify" << item << ", the monitored item does not exist";
        QOpcUaMonitoringParameters p;
        p.setStatusCode(QOpcUa::UaStatusCode::BadMonitoredItemIdInvalid);
        emit monitoringStatusChanged(handle, attr, item, p);
        return;
    }

    subscription->modifyMonitoring(handle, attr, item, value);
}

void UACppAsyncBackend::disableMonitoring(quint64 handle, QOpcUa::NodeAttributes attr)
{
    qt_forEachAttribute(attr, [&](QOpcUa::NodeAttribute attribute){
        QUACppSubscription *sub = getSubscriptionForItem(handle, attribute);
        if (sub) {
            sub->removeAttributeMonitoredItem(handle, attribute);
            m_attributeMapping[handle].remove(attribute);
            if (sub->monitoredItemsCount() == 0)
                removeSubscription(sub->subscriptionId());
        }
    });
}

void UACppAsyncBackend::callMethod(quint64 handle, const UaNodeId &objectId, const UaNodeId &methodId, QList<QOpcUa::TypedVariant> args)
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
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Calling method" << methodId.toFullString().toUtf8() << "failed";

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

void UACppAsyncBackend::resolveBrowsePath(quint64 handle, const UaNodeId &startNode, const QList<QOpcUaRelativePathElement> &path)
{
    ServiceSettings settings;
    UaDiagnosticInfos diagnosticInfos;
    UaBrowsePaths paths;
    UaBrowsePathResults result;
    UaRelativePathElements pathElements;

    paths.create(1);
    startNode.copyTo(&paths[0].StartingNode);
    pathElements.create(path.size());

    for (int i = 0; i < path.size(); ++i) {
        pathElements[i].IncludeSubtypes = path[i].includeSubtypes();
        pathElements[i].IsInverse = path[i].isInverse();
        UaNodeId(UACppUtils::nodeIdFromQString(path[i].referenceTypeId())).copyTo(&pathElements[i].ReferenceTypeId);
        UaQualifiedName(UaString(path[i].targetName().name().toUtf8().constData()), path[i].targetName().namespaceIndex()).copyTo(&pathElements[i].TargetName);
    }

    paths[0].RelativePath.Elements = pathElements.detach();
    paths[0].RelativePath.NoOfElements = path.size();

    UaStatusCode serviceResult = m_nativeSession->translateBrowsePathsToNodeIds(settings, paths, result, diagnosticInfos);
    QOpcUa::UaStatusCode status = static_cast<QOpcUa::UaStatusCode>(serviceResult.code());

    QList<QOpcUaBrowsePathTarget> ret;

    if (status == QOpcUa::UaStatusCode::Good && result.length()) {
        status = static_cast<QOpcUa::UaStatusCode>(result[0].StatusCode);
        for (int i = 0; i < result[0].NoOfTargets; ++i) {
            QOpcUaBrowsePathTarget temp;
            temp.setRemainingPathIndex(result[0].Targets[i].RemainingPathIndex);
            temp.targetIdRef().setNamespaceUri(QString::fromUtf8(UaString(result[0].Targets[i].TargetId.NamespaceUri).toUtf8()));
            temp.targetIdRef().setServerIndex(result[0].Targets[i].TargetId.ServerIndex);
            temp.targetIdRef().setNodeId(UACppUtils::nodeIdToQString(result[0].Targets[i].TargetId.NodeId));
            ret.append(temp);
        }
    }

    emit resolveBrowsePathFinished(handle, ret, path, status);
}

QUACppSubscription *UACppAsyncBackend::getSubscription(const QOpcUaMonitoringParameters &settings)
{
    if (settings.subscriptionType() == QOpcUaMonitoringParameters::SubscriptionType::Shared) {
        // Requesting multiple subscriptions with publishing interval < minimum publishing interval breaks subscription sharing
        double interval = revisePublishingInterval(settings.publishingInterval(), m_minPublishingInterval);
        for (auto entry : std::as_const(m_subscriptions)) {
            if (qFuzzyCompare(entry->interval(), interval) && entry->shared() == QOpcUaMonitoringParameters::SubscriptionType::Shared)
                return entry;
        }
    }

    QUACppSubscription *sub = new QUACppSubscription(this, settings);
    quint32 id = sub->createOnServer();
    if (!id) {
        delete sub;
        return nullptr;
    }
    if (sub->interval() > settings.publishingInterval()) // The publishing interval has been revised by the server.
        m_minPublishingInterval = sub->interval();
    m_subscriptions[id] = sub;
    return sub;
}

QUACppSubscription *UACppAsyncBackend::getSubscriptionForItem(quint64 handle, QOpcUa::NodeAttribute attr)
{
    auto entriesForHandle = m_attributeMapping.find(handle);
    if (entriesForHandle == m_attributeMapping.end())
        return nullptr;
    auto subscription = entriesForHandle->find(attr);
    if (subscription == entriesForHandle->end())
        return nullptr;

    return subscription.value();
}

void UACppAsyncBackend::cleanupSubscriptions()
{
    qDeleteAll(m_subscriptions);
    m_subscriptions.clear();
    m_attributeMapping.clear();
    m_minPublishingInterval = 0;
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

void UACppAsyncBackend::findServers(const QUrl &url, const QStringList &localeIds, const QStringList &serverUris)
{
    UaStringArray uaServerUris = QUACppValueConverter::toUaStringArray(serverUris);
    UaStringArray uaLocaleIds = QUACppValueConverter::toUaStringArray(localeIds);
    UaDiscovery discovery;
    ServiceSettings serviceSettings;
    ClientSecurityInfo clientSecurityInfo;
    UaApplicationDescriptions applicationDescriptions;

    UaStatus result = discovery.findServers(serviceSettings,
                                            UaString(url.toString(QUrl::RemoveUserInfo).toUtf8().constData()),
                                            clientSecurityInfo,
                                            uaLocaleIds,
                                            uaServerUris,
                                            applicationDescriptions);

    QList<QOpcUaApplicationDescription> ret;

    for (OpcUa_UInt32 i = 0; i < applicationDescriptions.length(); ++i) {
        QOpcUaApplicationDescription temp;
        const UaApplicationDescription desc = applicationDescriptions[i];

        temp.setApplicationUri(QString::fromUtf8(desc.getApplicationUri().toUtf8()));
        temp.setProductUri(QString::fromUtf8(desc.getProductUri().toUtf8()));
        UaLocalizedText uaApplicationName(desc.getApplicationName());
        temp.setApplicationName(QUACppValueConverter::toQOpcUaLocalizedText(&uaApplicationName));
        temp.setApplicationType(static_cast<QOpcUaApplicationDescription::ApplicationType>(desc.getApplicationType()));
        temp.setGatewayServerUri(QString::fromUtf8(desc.getGatewayServerUri().toUtf8()));
        temp.setDiscoveryProfileUri(QString::fromUtf8(desc.getDiscoveryProfileUri().toUtf8()));

        UaStringArray discoveryUrls;
        desc.getDiscoveryUrls(discoveryUrls);

        for (OpcUa_UInt32 i = 0; i < discoveryUrls.length(); ++i)
            temp.discoveryUrlsRef().append(QString::fromUtf8(UaString(discoveryUrls[i]).toUtf8()));

        ret.append(temp);
    }

    if (result.isNotGood()) {
        qCDebug(QT_OPCUA_PLUGINS_UACPP) << "Failed to get servers:" << static_cast<QOpcUa::UaStatusCode>(result.statusCode());
    }

    emit findServersFinished(ret, static_cast<QOpcUa::UaStatusCode>(result.statusCode()), url);
}

void UACppAsyncBackend::readNodeAttributes(const QList<QOpcUaReadItem> &nodesToRead)
{
    if (nodesToRead.size() == 0) {
        emit readNodeAttributesFinished(QList<QOpcUaReadResult>(), QOpcUa::UaStatusCode::BadNothingToDo);
        return;
    }

    UaReadValueIds nodesToReadNativeType;
    nodesToReadNativeType.resize(nodesToRead.size());

    for (int i = 0; i < nodesToRead.size(); ++i) {
        UACppUtils::nodeIdFromQString(nodesToRead[i].nodeId()).copyTo(&nodesToReadNativeType[i].NodeId);
        nodesToReadNativeType[i].AttributeId = QUACppValueConverter::toUaAttributeId(nodesToRead[i].attribute());
        if (!nodesToRead[i].indexRange().isEmpty()) {
            UaString ir(nodesToRead[i].indexRange().toUtf8());
            ir.copyTo(&nodesToReadNativeType[i].IndexRange);
        }
    }

    UaDataValues values;
    UaDiagnosticInfos diagnosticInfos;
    ServiceSettings serviceSettings;

    UaStatus result  = m_nativeSession->read(serviceSettings, 0.0 /* maxAge */,
                                             OpcUa_TimestampsToReturn_Both,
                                             nodesToReadNativeType, values,
                                             diagnosticInfos);
    QOpcUa::UaStatusCode status = static_cast<QOpcUa::UaStatusCode>(result.code());

    if (result.isBad()) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Batch read failed:" << result.toString();
        emit readNodeAttributesFinished(QList<QOpcUaReadResult>(), status);
    } else {
        QList<QOpcUaReadResult> ret;

        for (int i = 0; i < nodesToRead.size(); ++i) {
            QOpcUaReadResult item;
            item.setAttribute(nodesToRead.at(i).attribute());
            item.setNodeId(nodesToRead.at(i).nodeId());
            item.setIndexRange(nodesToRead.at(i).indexRange());

            if (static_cast<size_t>(i) < values.length()) {
                UaDataValue dataValue(values[i]);
                item.setValue(QUACppValueConverter::toQVariant(*dataValue.value()));
                item.setStatusCode(static_cast<QOpcUa::UaStatusCode>(dataValue.statusCode()));
                if (dataValue.isServerTimestampSet()) {
                    auto dt = dataValue.serverTimestamp();
                    item.setServerTimestamp(QUACppValueConverter::toQDateTime(&dt));
                }
                if (dataValue.isSourceTimestampSet()) {
                    auto dt = dataValue.sourceTimestamp();
                    item.setSourceTimestamp(QUACppValueConverter::toQDateTime(&dt));
                }
            } else {
                item.setStatusCode(status);
            }
            ret.push_back(item);
        }
        emit readNodeAttributesFinished(ret, status);
    }
}

void UACppAsyncBackend::writeNodeAttributes(const QList<QOpcUaWriteItem> &nodesToWrite)
{
    if (nodesToWrite.isEmpty()) {
        emit writeNodeAttributesFinished(QList<QOpcUaWriteResult>(), QOpcUa::UaStatusCode::BadNothingToDo);
        return;
    }

    UaWriteValues nodesToWriteNativeType;
    nodesToWriteNativeType.create(nodesToWrite.size());

    for (int i = 0; i < nodesToWrite.size(); ++i) {
        const auto &currentItem = nodesToWrite[i];
        UaDataValue dataValue;
        auto value = QUACppValueConverter::toUACppVariant(currentItem.value(), currentItem.type());
        dataValue.setValue(value, true); // true == detach value
        if (currentItem.hasStatusCode())
            dataValue.setStatusCode(currentItem.statusCode());
        if (currentItem.serverTimestamp().isValid())
            dataValue.setServerTimestamp(QUACppValueConverter::toUACppDateTime(currentItem.serverTimestamp()));
        if (currentItem.sourceTimestamp().isValid())
            dataValue.setSourceTimestamp(QUACppValueConverter::toUACppDateTime(currentItem.sourceTimestamp()));
        dataValue.copyTo(&nodesToWriteNativeType[i].Value);
        UACppUtils::nodeIdFromQString(currentItem.nodeId()).copyTo(&nodesToWriteNativeType[i].NodeId);
        nodesToWriteNativeType[i].AttributeId = QUACppValueConverter::toUaAttributeId(currentItem.attribute());

        if (!currentItem.indexRange().isEmpty()) {
            UaString ir(currentItem.indexRange().toUtf8());
            ir.copyTo(&nodesToWriteNativeType[i].IndexRange);
        }
    }

    UaStatusCodeArray results;
    UaDiagnosticInfos diagnosticInfos;
    ServiceSettings serviceSettings;

    UaStatus result  = m_nativeSession->write(serviceSettings, nodesToWriteNativeType,
                                              results, diagnosticInfos);
    const QOpcUa::UaStatusCode status = static_cast<QOpcUa::UaStatusCode>(result.code());

    if (result.isBad()) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Batch write failed:" << result.toString();
        emit writeNodeAttributesFinished(QList<QOpcUaWriteResult>(), status);
    } else {
        QList<QOpcUaWriteResult> ret;

        for (int i = 0; i < nodesToWrite.size(); ++i) {
            QOpcUaWriteResult item;
            item.setAttribute(nodesToWrite.at(i).attribute());
            item.setNodeId(nodesToWrite.at(i).nodeId());
            item.setIndexRange(nodesToWrite.at(i).indexRange());

            if (static_cast<size_t>(i) < results.length())
                item.setStatusCode(static_cast<QOpcUa::UaStatusCode>(results[i]));
            else
                item.setStatusCode(status);

            ret.push_back(item);
        }
        emit writeNodeAttributesFinished(ret, status);
    }
}

void UACppAsyncBackend::addNode(const QOpcUaAddNodeItem &nodeToAdd)
{
    OpcUa_AddNodesItem* opcUaNodeToAdd = static_cast<OpcUa_AddNodesItem*>(OpcUa_Alloc(1 * sizeof(OpcUa_AddNodesItem)));
    OpcUa_AddNodesItem_Initialize(opcUaNodeToAdd);

    opcUaNodeToAdd->ParentNodeId = QUACppValueConverter::toUACppExpandedNodeId(nodeToAdd.parentNodeId());
    UACppUtils::nodeIdFromQString(nodeToAdd.referenceTypeId()).detach(&opcUaNodeToAdd->ReferenceTypeId);
    opcUaNodeToAdd->RequestedNewNodeId = QUACppValueConverter::toUACppExpandedNodeId(nodeToAdd.requestedNewNodeId());
    opcUaNodeToAdd->BrowseName = QUACppValueConverter::toUACppQualifiedName(nodeToAdd.browseName());
    opcUaNodeToAdd->NodeClass = static_cast<OpcUa_NodeClass>(nodeToAdd.nodeClass());

    if (!assembleNodeAttributes(&opcUaNodeToAdd->NodeAttributes, nodeToAdd.nodeAttributes(), nodeToAdd.nodeClass())) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Failed to assemble node attributes";
        emit addNodeFinished(nodeToAdd.requestedNewNodeId(), QString(), QOpcUa::BadUnexpectedError);
        return;
    }

    if (!nodeToAdd.typeDefinition().nodeId().isEmpty())
        opcUaNodeToAdd->TypeDefinition = QUACppValueConverter::toUACppExpandedNodeId(nodeToAdd.typeDefinition());

    UaAddNodesItems itemsToAdd;
    itemsToAdd.attach((OpcUa_UInt32)1, opcUaNodeToAdd);
    UaAddNodesResults results;
    UaDiagnosticInfos diagnosticInfos;
    ServiceSettings serviceSettings;

    UaStatus result = m_nativeSession->addNodes(serviceSettings, itemsToAdd, results, diagnosticInfos);
    QOpcUa::UaStatusCode status = static_cast<QOpcUa::UaStatusCode>(result.code());

    QString resultId;

    if (result.isGood() && results.length() > 0) {
        if (OpcUa_IsGood(results[0].StatusCode)) {
            resultId = UACppUtils::nodeIdToQString(results[0].AddedNodeId);
        } else {
           status = static_cast<QOpcUa::UaStatusCode>(results[0].StatusCode);
           qCDebug(QT_OPCUA_PLUGINS_UACPP) << "Failed to add node:" << status;
        }
    } else {
        qCDebug(QT_OPCUA_PLUGINS_UACPP) << "Failed to add node:" << status;
    }

    emit addNodeFinished(nodeToAdd.requestedNewNodeId(), resultId, status);
}

void UACppAsyncBackend::deleteNode(const QString &nodeId, bool deleteTargetReferences)
{
    UaNodeId id = UACppUtils::nodeIdFromQString(nodeId);

    UaDeleteNodesItems nodesToDelete;
    nodesToDelete.attach((OpcUa_UInt32)1, static_cast<OpcUa_DeleteNodesItem*>(OpcUa_Alloc(1 * sizeof(OpcUa_DeleteNodesItem))));
    OpcUa_DeleteNodesItem_Initialize(&nodesToDelete[0]);

    id.copyTo(&nodesToDelete[0].NodeId);
    nodesToDelete[0].DeleteTargetReferences = deleteTargetReferences;

    UaDiagnosticInfos diagnosticInfos;
    ServiceSettings serviceSettings;
    UaStatusCodeArray results;

    UaStatus res = m_nativeSession->deleteNodes(serviceSettings, nodesToDelete, results, diagnosticInfos);

    QOpcUa::UaStatusCode resultStatus = static_cast<QOpcUa::UaStatusCode>(res.statusCode());

    if (resultStatus != QOpcUa::UaStatusCode::Good) {
        qCDebug(QT_OPCUA_PLUGINS_UACPP) << "Failed to delete node" << nodeId << "with status code" << resultStatus;
    }

    emit deleteNodeFinished(nodeId, resultStatus);
}

void UACppAsyncBackend::addReference(const QOpcUaAddReferenceItem &referenceToAdd)
{
    UaAddReferencesItems referencesToAdd;
    referencesToAdd.attach((OpcUa_UInt32)1, static_cast<OpcUa_AddReferencesItem*>(OpcUa_Alloc(1 * sizeof(OpcUa_AddReferencesItem))));
    OpcUa_AddReferencesItem_Initialize(&referencesToAdd[0]);

    UACppUtils::nodeIdFromQString(referenceToAdd.sourceNodeId()).copyTo(&referencesToAdd[0].SourceNodeId);
    UACppUtils::nodeIdFromQString(referenceToAdd.referenceTypeId()).copyTo(&referencesToAdd[0].ReferenceTypeId);
    referencesToAdd[0].IsForward = referenceToAdd.isForwardReference();
    UaString(referenceToAdd.targetServerUri().toUtf8().constData()).detach(&referencesToAdd[0].TargetServerUri);
    referencesToAdd[0].TargetNodeId = QUACppValueConverter::toUACppExpandedNodeId(referenceToAdd.targetNodeId());
    referencesToAdd[0].TargetNodeClass = static_cast<OpcUa_NodeClass>(referenceToAdd.targetNodeClass());

    UaDiagnosticInfos diagnosticInfos;
    ServiceSettings serviceSettings;
    UaStatusCodeArray results;

    UaStatus res = m_nativeSession->addReferences(serviceSettings, referencesToAdd, results, diagnosticInfos);

    QOpcUa::UaStatusCode statusCode = static_cast<QOpcUa::UaStatusCode>(res.statusCode());
    if (res.isBad())
        qCDebug(QT_OPCUA_PLUGINS_UACPP) << "Failed to add reference from" << referenceToAdd.sourceNodeId() << "to"
                                            << referenceToAdd.targetNodeId().nodeId() << ":" << statusCode;

    emit addReferenceFinished(referenceToAdd.sourceNodeId(), referenceToAdd.referenceTypeId(),
                              referenceToAdd.targetNodeId(),
                              referenceToAdd.isForwardReference(), statusCode);
}

void UACppAsyncBackend::deleteReference(const QOpcUaDeleteReferenceItem &referenceToDelete)
{
    UaDeleteReferencesItems referencesToDelete;
    referencesToDelete.attach((OpcUa_UInt32)1, static_cast<OpcUa_DeleteReferencesItem*>(OpcUa_Alloc(1 * sizeof(OpcUa_DeleteReferencesItem))));
    OpcUa_DeleteReferencesItem_Initialize(&referencesToDelete[0]);

    UACppUtils::nodeIdFromQString(referenceToDelete.sourceNodeId()).copyTo(&referencesToDelete[0].SourceNodeId);
    UACppUtils::nodeIdFromQString(referenceToDelete.referenceTypeId()).copyTo(&referencesToDelete[0].ReferenceTypeId);
    referencesToDelete[0].IsForward = referenceToDelete.isForwardReference();
    referencesToDelete[0].TargetNodeId = QUACppValueConverter::toUACppExpandedNodeId(referenceToDelete.targetNodeId());
    referencesToDelete[0].DeleteBidirectional = referenceToDelete.deleteBidirectional();

    UaDiagnosticInfos diagnosticInfos;
    ServiceSettings serviceSettings;
    UaStatusCodeArray results;

    UaStatus res = m_nativeSession->deleteReferences(serviceSettings, referencesToDelete, results, diagnosticInfos);

    QOpcUa::UaStatusCode statusCode = static_cast<QOpcUa::UaStatusCode>(res.statusCode());
    if (res.isBad())
        qCDebug(QT_OPCUA_PLUGINS_UACPP) << "Failed to delete reference from" << referenceToDelete.sourceNodeId() << "to"
                                            << referenceToDelete.targetNodeId().nodeId() << ":" << statusCode;

    emit deleteReferenceFinished(referenceToDelete.sourceNodeId(), referenceToDelete.referenceTypeId(),
                                 referenceToDelete.targetNodeId(),
                                 referenceToDelete.isForwardReference(), statusCode);
}

QOpcUaErrorState::ConnectionStep UACppAsyncBackend::connectionStepFromUaServiceType(
        UaClientSdk::UaClient::ConnectServiceType type) const
{
    switch (type) {
    case UaClientSdk::UaClient::ConnectServiceType::CreateSession:
        return QOpcUaErrorState::ConnectionStep::CreateSession;
    case UaClientSdk::UaClient::ConnectServiceType::ActivateSession:
        return QOpcUaErrorState::ConnectionStep::ActivateSession;
    case UaClientSdk::UaClient::ConnectServiceType::OpenSecureChannel:
        return QOpcUaErrorState::ConnectionStep::OpenSecureChannel;
    case UaClientSdk::UaClient::ConnectServiceType::CertificateValidation:
        return QOpcUaErrorState::ConnectionStep::CertificateValidation;
    default:
        return QOpcUaErrorState::ConnectionStep::Unknown;
    }
}

static void copyArrayDimensions(OpcUa_Int32 *noOfDimensions, OpcUa_UInt32 **arrayDimensions, const QList<quint32> &dimensions)
{
    *noOfDimensions = static_cast<OpcUa_Int32>(dimensions.size());
    *arrayDimensions = static_cast<OpcUa_UInt32 *>(OpcUa_Alloc(*noOfDimensions * sizeof(OpcUa_UInt32)));
    std::copy(dimensions.constBegin(), dimensions.constEnd(), *arrayDimensions);
}

#define CREATE_NODE_ATTRIBUTE(TYPE) \
    OpcUa_ ##TYPE ## Attributes* uaNodeAttributes = nullptr; \
    OpcUa_EncodeableObject_CreateExtension(&OpcUa_ ##TYPE ## Attributes_EncodeableType, \
                                           uaExtensionObject, (OpcUa_Void**)&uaNodeAttributes); \
    if (!uaNodeAttributes) \
        return false; \
    \
    OpcUa_ ##TYPE ## Attributes_Initialize(uaNodeAttributes); \
    \
    if (nodeAttributes.hasDisplayName()) { \
        uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_DisplayName; \
        uaNodeAttributes->DisplayName = QUACppValueConverter::toUACppLocalizedText(nodeAttributes.displayName()); \
    } \
    if (nodeAttributes.hasDescription()) { \
        uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_Description; \
        uaNodeAttributes->Description = QUACppValueConverter::toUACppLocalizedText(nodeAttributes.description()); \
    } \
    if (nodeAttributes.hasWriteMask()) { \
        uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_WriteMask; \
        uaNodeAttributes->WriteMask = nodeAttributes.writeMask(); \
    } \
    if (nodeAttributes.hasUserWriteMask()) { \
        uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_UserWriteMask; \
        uaNodeAttributes->UserWriteMask = nodeAttributes.userWriteMask(); \
    } \

bool UACppAsyncBackend::assembleNodeAttributes(OpcUa_ExtensionObject *uaExtensionObject, const QOpcUaNodeCreationAttributes &nodeAttributes,
                                                                 QOpcUa::NodeClass nodeClass)
{
    switch (nodeClass) {
    case QOpcUa::NodeClass::Object: {
        CREATE_NODE_ATTRIBUTE(Object);

        if (nodeAttributes.hasEventNotifier()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_EventNotifier;
            uaNodeAttributes->EventNotifier = nodeAttributes.eventNotifier();
        }
        break;
    }
    case QOpcUa::NodeClass::Variable: {
        CREATE_NODE_ATTRIBUTE(Variable);

        if (nodeAttributes.hasValue()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_Value;
            uaNodeAttributes->Value = QUACppValueConverter::toUACppVariant(nodeAttributes.value(),
                                                                       nodeAttributes.valueType());
        }
        if (nodeAttributes.hasDataTypeId()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_DataType;
            UACppUtils::nodeIdFromQString(nodeAttributes.dataTypeId()).detach(&uaNodeAttributes->DataType);
        }
        if (nodeAttributes.hasValueRank()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_ValueRank;
            uaNodeAttributes->ValueRank = nodeAttributes.valueRank();
        }
        if (nodeAttributes.hasArrayDimensions()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_ArrayDimensions;
            copyArrayDimensions(&uaNodeAttributes->NoOfArrayDimensions,&uaNodeAttributes->ArrayDimensions, nodeAttributes.arrayDimensions());
        }
        if (nodeAttributes.hasAccessLevel()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_AccessLevel;
            uaNodeAttributes->AccessLevel = nodeAttributes.accessLevel();
        }
        if (nodeAttributes.hasUserAccessLevel()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_UserAccessLevel;
            uaNodeAttributes->UserAccessLevel = nodeAttributes.userAccessLevel();
        }
        if (nodeAttributes.hasMinimumSamplingInterval()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_MinimumSamplingInterval;
            uaNodeAttributes->MinimumSamplingInterval = nodeAttributes.minimumSamplingInterval();
        }
        if (nodeAttributes.hasHistorizing()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_Historizing;
            uaNodeAttributes->Historizing = nodeAttributes.historizing();
        }
        break;
    }
    case QOpcUa::NodeClass::Method: {
        CREATE_NODE_ATTRIBUTE(Method);

        if (nodeAttributes.hasExecutable()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_Executable;
            uaNodeAttributes->Executable = nodeAttributes.executable();
        }
        if (nodeAttributes.hasUserExecutable()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_UserExecutable;
            uaNodeAttributes->UserExecutable = nodeAttributes.userExecutable();
        }
        break;
    }
    case QOpcUa::NodeClass::ObjectType: {
        CREATE_NODE_ATTRIBUTE(ObjectType);

        if (nodeAttributes.hasIsAbstract()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_IsAbstract;
            uaNodeAttributes->IsAbstract = nodeAttributes.isAbstract();
        }
        break;
    }
    case QOpcUa::NodeClass::VariableType: {
        CREATE_NODE_ATTRIBUTE(VariableType);

        if (nodeAttributes.hasValue()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_Value;
            uaNodeAttributes->Value = QUACppValueConverter::toUACppVariant(nodeAttributes.value(),
                                                                       nodeAttributes.valueType());
        }
        if (nodeAttributes.hasDataTypeId()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_DataType;
            UACppUtils::nodeIdFromQString(nodeAttributes.dataTypeId()).detach(&uaNodeAttributes->DataType);
        }
        if (nodeAttributes.hasValueRank()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_ValueRank;
            uaNodeAttributes->ValueRank = nodeAttributes.valueRank();
        }
        if (nodeAttributes.hasArrayDimensions()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_ArrayDimensions;
            copyArrayDimensions(&uaNodeAttributes->NoOfArrayDimensions, &uaNodeAttributes->ArrayDimensions, nodeAttributes.arrayDimensions());
        }
        if (nodeAttributes.hasIsAbstract()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_IsAbstract;
            uaNodeAttributes->IsAbstract = nodeAttributes.isAbstract();
        }
        break;
    }
    case QOpcUa::NodeClass::ReferenceType: {
        CREATE_NODE_ATTRIBUTE(ReferenceType);

        if (nodeAttributes.hasIsAbstract()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_IsAbstract;
            uaNodeAttributes->IsAbstract = nodeAttributes.isAbstract();
        }
        if (nodeAttributes.hasSymmetric()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_Symmetric;
            uaNodeAttributes->Symmetric = nodeAttributes.symmetric();
        }
        if (nodeAttributes.hasInverseName()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_InverseName;
            uaNodeAttributes->InverseName = QUACppValueConverter::toUACppLocalizedText(nodeAttributes.inverseName());
        }
        break;
    }
    case QOpcUa::NodeClass::DataType: {
        CREATE_NODE_ATTRIBUTE(DataType);

        if (nodeAttributes.hasIsAbstract()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_IsAbstract;
            uaNodeAttributes->IsAbstract = nodeAttributes.isAbstract();
        }
        break;
    }
    case QOpcUa::NodeClass::View: {
        CREATE_NODE_ATTRIBUTE(View);

        if (nodeAttributes.hasContainsNoLoops()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_ContainsNoLoops;
            uaNodeAttributes->ContainsNoLoops = nodeAttributes.containsNoLoops();
        }
        if (nodeAttributes.hasEventNotifier()) {
            uaNodeAttributes->SpecifiedAttributes |= OpcUa_NodeAttributesMask_EventNotifier;
            uaNodeAttributes->EventNotifier = nodeAttributes.eventNotifier();
        }
        break;
    }
    default:
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Could not convert node attributes, unknown node class";
        return false;
    }

    return true;
}

QT_END_NAMESPACE

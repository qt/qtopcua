// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef OPCUASTATUS_P_H
#define OPCUASTATUS_P_H

#include <QStandardItemModel>
#include <QtOpcUa/qopcuatype.h>
#include <QOpcUaApplicationDescription>
#include <QtQml/qqml.h>
#include <QtCore/private/qglobal_p.h>

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

QT_BEGIN_NAMESPACE

class OpcUaConnection;

class OpcUaStatus
{
    Q_GADGET
    Q_PROPERTY(bool isGood READ isGood)
    Q_PROPERTY(bool isBad READ isBad)
    Q_PROPERTY(Status status READ status)

    // This is a value type. Thus lowercase. The enums are taken care of below
    QML_VALUE_TYPE(status)
    QML_ADDED_IN_VERSION(5, 13)

public:
    enum class Status {
        Good = 0x0000,
        BadUnexpectedError = 0x8001,
        BadInternalError = 0x8002,
        BadOutOfMemory = 0x8003,
        BadResourceUnavailable = 0x8004,
        BadCommunicationError = 0x8005,
        BadEncodingError = 0x8006,
        BadDecodingError = 0x8007,
        BadEncodingLimitsExceeded = 0x8008,
        BadRequestTooLarge = 0x80B8,
        BadResponseTooLarge = 0x80B9,
        BadUnknownResponse = 0x8009,
        BadTimeout = 0x800A,
        BadServiceUnsupported = 0x800B,
        BadShutdown = 0x800C,
        BadServerNotConnected = 0x800D,
        BadServerHalted = 0x800E,
        BadNothingToDo = 0x800F,
        BadTooManyOperations = 0x8010,
        BadTooManyMonitoredItems = 0x80DB,
        BadDataTypeIdUnknown = 0x8011,
        BadCertificateInvalid = 0x8012,
        BadSecurityChecksFailed = 0x8013,
        BadCertificateTimeInvalid = 0x8014,
        BadCertificateIssuerTimeInvalid = 0x8015,
        BadCertificateHostNameInvalid = 0x8016,
        BadCertificateUriInvalid = 0x8017,
        BadCertificateUseNotAllowed = 0x8018,
        BadCertificateIssuerUseNotAllowed = 0x8019,
        BadCertificateUntrusted = 0x801A,
        BadCertificateRevocationUnknown = 0x801B,
        BadCertificateIssuerRevocationUnknown = 0x801C,
        BadCertificateRevoked = 0x801D,
        BadCertificateIssuerRevoked = 0x801E,
        BadCertificateChainIncomplete = 0x810D,
        BadUserAccessDenied = 0x801F,
        BadIdentityTokenInvalid = 0x8020,
        BadIdentityTokenRejected = 0x8021,
        BadSecureChannelIdInvalid = 0x8022,
        BadInvalidTimestamp = 0x8023,
        BadNonceInvalid = 0x8024,
        BadSessionIdInvalid = 0x8025,
        BadSessionClosed = 0x8026,
        BadSessionNotActivated = 0x8027,
        BadSubscriptionIdInvalid = 0x8028,
        BadRequestHeaderInvalid = 0x802A,
        BadTimestampsToReturnInvalid = 0x802B,
        BadRequestCancelledByClient = 0x802C,
        BadTooManyArguments = 0x80E5,
        GoodSubscriptionTransferred = 0x002D,
        GoodCompletesAsynchronously = 0x002E,
        GoodOverload = 0x002F,
        GoodClamped = 0x0030,
        BadNoCommunication = 0x8031,
        BadWaitingForInitialData = 0x8032,
        BadNodeIdInvalid = 0x8033,
        BadNodeIdUnknown = 0x8034,
        BadAttributeIdInvalid = 0x8035,
        BadIndexRangeInvalid = 0x8036,
        BadIndexRangeNoData = 0x8037,
        BadDataEncodingInvalid = 0x8038,
        BadDataEncodingUnsupported = 0x8039,
        BadNotReadable = 0x803A,
        BadNotWritable = 0x803B,
        BadOutOfRange = 0x803C,
        BadNotSupported = 0x803D,
        BadNotFound = 0x803E,
        BadObjectDeleted = 0x803F,
        BadNotImplemented = 0x8040,
        BadMonitoringModeInvalid = 0x8041,
        BadMonitoredItemIdInvalid = 0x8042,
        BadMonitoredItemFilterInvalid = 0x8043,
        BadMonitoredItemFilterUnsupported = 0x8044,
        BadFilterNotAllowed = 0x8045,
        BadStructureMissing = 0x8046,
        BadEventFilterInvalid = 0x8047,
        BadContentFilterInvalid = 0x8048,
        BadFilterOperatorInvalid = 0x80C1,
        BadFilterOperatorUnsupported = 0x80C2,
        BadFilterOperandCountMismatch = 0x80C3,
        BadFilterOperandInvalid = 0x8049,
        BadFilterElementInvalid = 0x80C4,
        BadFilterLiteralInvalid = 0x80C5,
        BadContinuationPointInvalid = 0x804A,
        BadNoContinuationPoints = 0x804B,
        BadReferenceTypeIdInvalid = 0x804C,
        BadBrowseDirectionInvalid = 0x804D,
        BadNodeNotInView = 0x804E,
        BadServerUriInvalid = 0x804F,
        BadServerNameMissing = 0x8050,
        BadDiscoveryUrlMissing = 0x8051,
        BadSempahoreFileMissing = 0x8052,
        BadRequestTypeInvalid = 0x8053,
        BadSecurityModeRejected = 0x8054,
        BadSecurityPolicyRejected = 0x8055,
        BadTooManySessions = 0x8056,
        BadUserSignatureInvalid = 0x8057,
        BadApplicationSignatureInvalid = 0x8058,
        BadNoValidCertificates = 0x8059,
        BadIdentityChangeNotSupported = 0x80C6,
        BadRequestCancelledByRequest = 0x805A,
        BadParentNodeIdInvalid = 0x805B,
        BadReferenceNotAllowed = 0x805C,
        BadNodeIdRejected = 0x805D,
        BadNodeIdExists = 0x805E,
        BadNodeClassInvalid = 0x805F,
        BadBrowseNameInvalid = 0x8060,
        BadBrowseNameDuplicated = 0x8061,
        BadNodeAttributesInvalid = 0x8062,
        BadTypeDefinitionInvalid = 0x8063,
        BadSourceNodeIdInvalid = 0x8064,
        BadTargetNodeIdInvalid = 0x8065,
        BadDuplicateReferenceNotAllowed = 0x8066,
        BadInvalidSelfReference = 0x8067,
        BadReferenceLocalOnly = 0x8068,
        BadNoDeleteRights = 0x8069,
        UncertainReferenceNotDeleted = 0x40BC,
        BadServerIndexInvalid = 0x806A,
        BadViewIdUnknown = 0x806B,
        BadViewTimestampInvalid = 0x80C9,
        BadViewParameterMismatch = 0x80CA,
        BadViewVersionInvalid = 0x80CB,
        UncertainNotAllNodesAvailable = 0x40C0,
        GoodResultsMayBeIncomplete = 0x00BA,
        BadNotTypeDefinition = 0x80C8,
        UncertainReferenceOutOfServer = 0x406C,
        BadTooManyMatches = 0x806D,
        BadQueryTooComplex = 0x806E,
        BadNoMatch = 0x806F,
        BadMaxAgeInvalid = 0x8070,
        BadSecurityModeInsufficient = 0x80E6,
        BadHistoryOperationInvalid = 0x8071,
        BadHistoryOperationUnsupported = 0x8072,
        BadInvalidTimestampArgument = 0x80BD,
        BadWriteNotSupported = 0x8073,
        BadTypeMismatch = 0x8074,
        BadMethodInvalid = 0x8075,
        BadArgumentsMissing = 0x8076,
        BadTooManySubscriptions = 0x8077,
        BadTooManyPublishRequests = 0x8078,
        BadNoSubscription = 0x8079,
        BadSequenceNumberUnknown = 0x807A,
        BadMessageNotAvailable = 0x807B,
        BadInsufficientClientProfile = 0x807C,
        BadStateNotActive = 0x80BF,
        BadTcpServerTooBusy = 0x807D,
        BadTcpMessageTypeInvalid = 0x807E,
        BadTcpSecureChannelUnknown = 0x807F,
        BadTcpMessageTooLarge = 0x8080,
        BadTcpNotEnoughResources = 0x8081,
        BadTcpInternalError = 0x8082,
        BadTcpEndpointUrlInvalid = 0x8083,
        BadRequestInterrupted = 0x8084,
        BadRequestTimeout = 0x8085,
        BadSecureChannelClosed = 0x8086,
        BadSecureChannelTokenUnknown = 0x8087,
        BadSequenceNumberInvalid = 0x8088,
        BadProtocolVersionUnsupported = 0x80BE,
        BadConfigurationError = 0x8089,
        BadNotConnected = 0x808A,
        BadDeviceFailure = 0x808B,
        BadSensorFailure = 0x808C,
        BadOutOfService = 0x808D,
        BadDeadbandFilterInvalid = 0x808E,
        UncertainNoCommunicationLastUsableValue = 0x408F,
        UncertainLastUsableValue = 0x4090,
        UncertainSubstituteValue = 0x4091,
        UncertainInitialValue = 0x4092,
        UncertainSensorNotAccurate = 0x4093,
        UncertainEngineeringUnitsExceeded = 0x4094,
        UncertainSubNormal = 0x4095,
        GoodLocalOverride = 0x0096,
        BadRefreshInProgress = 0x8097,
        BadConditionAlreadyDisabled = 0x8098,
        BadConditionAlreadyEnabled = 0x80CC,
        BadConditionDisabled = 0x8099,
        BadEventIdUnknown = 0x809A,
        BadEventNotAcknowledgeable = 0x80BB,
        BadDialogNotActive = 0x80CD,
        BadDialogResponseInvalid = 0x80CE,
        BadConditionBranchAlreadyAcked = 0x80CF,
        BadConditionBranchAlreadyConfirmed = 0x80D0,
        BadConditionAlreadyShelved = 0x80D1,
        BadConditionNotShelved = 0x80D2,
        BadShelvingTimeOutOfRange = 0x80D3,
        BadNoData = 0x809B,
        BadBoundNotFound = 0x80D7,
        BadBoundNotSupported = 0x80D8,
        BadDataLost = 0x809D,
        BadDataUnavailable = 0x809E,
        BadEntryExists = 0x809F,
        BadNoEntryExists = 0x80A0,
        BadTimestampNotSupported = 0x80A1,
        GoodEntryInserted = 0x00A2,
        GoodEntryReplaced = 0x00A3,
        UncertainDataSubNormal = 0x40A4,
        GoodNoData = 0x00A5,
        GoodMoreData = 0x00A6,
        BadAggregateListMismatch = 0x80D4,
        BadAggregateNotSupported = 0x80D5,
        BadAggregateInvalidInputs = 0x80D6,
        BadAggregateConfigurationRejected = 0x80DA,
        GoodDataIgnored = 0x00D9,
        BadRequestNotAllowed = 0x80E4,
        GoodEdited = 0x00DC,
        GoodPostActionFailed = 0x00DD,
        UncertainDominantValueChanged = 0x40DE,
        GoodDependentValueChanged = 0x00E0,
        BadDominantValueChanged = 0x80E1,
        UncertainDependentValueChanged = 0x40E2,
        BadDependentValueChanged = 0x80E3,
        GoodCommunicationEvent = 0x00A7,
        GoodShutdownEvent = 0x00A8,
        GoodCallAgain = 0x00A9,
        GoodNonCriticalTimeout = 0x00AA,
        BadInvalidArgument = 0x80AB,
        BadConnectionRejected = 0x80AC,
        BadDisconnect = 0x80AD,
        BadConnectionClosed = 0x80AE,
        BadInvalidState = 0x80AF,
        BadEndOfStream = 0x80B0,
        BadNoDataAvailable = 0x80B1,
        BadWaitingForResponse = 0x80B2,
        BadOperationAbandoned = 0x80B3,
        BadExpectedStreamToBlock = 0x80B4,
        BadWouldBlock = 0x80B5,
        BadSyntaxError = 0x80B6,
        BadMaxConnectionsReached = 0x80B7
    };
    Q_ENUM(Status)

    OpcUaStatus();
    OpcUaStatus(QOpcUa::UaStatusCode);
    OpcUaStatus(Status);
    ~OpcUaStatus();

    bool isGood() const;
    bool isBad() const;
    Status status() const;

private:
    quint32 m_statusCode{QOpcUa::Good};
};

// Expose only the enums with the uppercase "Status" name
namespace OpcuaStatusForeign
{
    Q_NAMESPACE
    QML_FOREIGN_NAMESPACE(OpcUaStatus)
    QML_NAMED_ELEMENT(Status)
    QML_ADDED_IN_VERSION(5, 13)
};

QT_END_NAMESPACE

#endif // OPCUASTATUS_P_H

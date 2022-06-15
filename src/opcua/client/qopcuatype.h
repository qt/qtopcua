// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUATYPE
#define QOPCUATYPE

#include <QtOpcUa/qopcuaglobal.h>
#include <QtOpcUa/qopcuanodeids.h>

#include <QtCore/qmetatype.h>
#include <QtCore/qpair.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

namespace QOpcUa {
Q_OPCUA_EXPORT Q_NAMESPACE
Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")

// see OPC-UA Part 3, 5.2.3 & 8.30
enum class NodeClass {
    Undefined = 0,
    Object = 1,
    Variable = 2,
    Method = 4,
    ObjectType = 8,
    VariableType = 16,
    ReferenceType = 32,
    DataType = 64,
    View = 128,
};
Q_ENUM_NS(NodeClass)
Q_DECLARE_FLAGS(NodeClasses, NodeClass)

enum class NodeAttribute {
    None = 0,
    NodeId = (1 << 0),
    NodeClass = (1 << 1),
    BrowseName = (1 << 2),
    DisplayName = (1 << 3),
    Description = (1 << 4),
    WriteMask = (1 << 5),
    UserWriteMask = (1 << 6), // Base attributes, see part 4, 5.2.1
    IsAbstract = (1 << 7),
    Symmetric = (1 << 8),
    InverseName = (1 << 9),   // Reference attributes, see part 4, 5.3.1
    ContainsNoLoops = (1 << 10),
    EventNotifier = (1 << 11), // View attributes, see part 4, 5.4
    // Objects also add the EventNotifier attribute, see part 4, 5.5.1
    // ObjectType also add the IsAbstract attribute, see part 4, 5.5.2
    Value = (1 << 12),
    DataType = (1 << 13),
    ValueRank = (1 << 14),
    ArrayDimensions = (1 << 15),
    AccessLevel = (1 << 16),
    UserAccessLevel = (1 << 17),
    MinimumSamplingInterval = (1 << 18),
    Historizing = (1 << 19),   // Value attributes, see part 4, 5.6.2
    // VariableType also adds the Value, DataType, ValueRank, ArrayDimensions
    // and isAbstract attributes, see part 4, 5.6.5
    Executable = (1 << 20),
    UserExecutable = (1 << 21), // Method attributes, see part 4, 5.7
};
Q_ENUM_NS(NodeAttribute)
Q_DECLARE_FLAGS(NodeAttributes, NodeAttribute)

// Defined in OPC-UA part 3, Table 8.
enum class WriteMaskBit : quint32 {
    None = 0,
    AccessLevel = (1 << 0),
    ArrayDimensions = (1 << 1),
    BrowseName = (1 << 2),
    ContainsNoLoops = (1 << 3),
    DataType = (1 << 4),
    Description = (1 << 5),
    DisplayName = (1 << 6),
    EventNotifier = (1 << 7),
    Executable = (1 << 8),
    Historizing = (1 << 9),
    InverseName = (1 << 10),
    IsAbstract = (1 << 11),
    MinimumSamplingInterval = (1 << 12),
    NodeClass = (1 << 13),
    NodeId = (1 << 14),
    Symmetric = (1 << 15),
    UserAccessLevel = (1 << 16),
    UserExecutable = (1 << 17),
    UserWriteMask = (1 << 18),
    ValueRank = (1 << 19),
    WriteMask = (1 << 20),
    ValueForVariableType = (1 << 21),
};
Q_ENUM_NS(WriteMaskBit)
Q_DECLARE_FLAGS(WriteMask, WriteMaskBit)

// Defined in OPC-UA part 3, Table 8.
enum class AccessLevelBit : quint8 {
    None = 0,
    CurrentRead = (1 << 0),
    CurrentWrite = (1 << 1),
    HistoryRead = (1 << 2),
    HistoryWrite = (1 << 3),
    SemanticChange = (1 << 4),
    StatusWrite = (1 << 5),
    TimestampWrite = (1 << 6),
};
Q_ENUM_NS(AccessLevelBit)
Q_DECLARE_FLAGS(AccessLevel, AccessLevelBit)

// Defined in OPC-UA part 3, Table 6.
enum class EventNotifierBit : quint8 {
    None = 0,
    SubscribeToEvents = (1 << 0),
    HistoryRead = (1 << 2),
    HistoryWrite = (1 << 3)
};
Q_ENUM_NS(EventNotifierBit)
Q_DECLARE_FLAGS(EventNotifier, EventNotifierBit)

#if QT_VERSION >= 0x060000
inline size_t qHash(const QOpcUa::NodeAttribute& attr)
#else
inline uint qHash(const QOpcUa::NodeAttribute& attr)
#endif
{
    return ::qHash(static_cast<uint>(attr));
}

// The reference types are nodes in namespace 0, the enum value is their numeric identifier.
// Identifiers are specified in https://opcfoundation.org/UA/schemas/1.03/NodeIds.csv
enum class ReferenceTypeId : quint32 {
    Unspecified = 0,
    References = 31,
    NonHierarchicalReferences = 32,
    HierarchicalReferences = 33,
    HasChild = 34,
    Organizes = 35,
    HasEventSource = 36,
    HasModellingRule = 37,
    HasEncoding = 38,
    HasDescription = 39,
    HasTypeDefinition = 40,
    GeneratesEvent = 41,
    Aggregates = 44,
    HasSubtype = 45,
    HasProperty = 46,
    HasComponent = 47,
    HasNotifier = 48,
    HasOrderedComponent = 49,
    FromState = 51,
    ToState = 52,
    HasCause = 53,
    HasEffect = 54,
    HasHistoricalConfiguration = 56,
    HasSubStateMachine = 117,
    HasArgumentDescription = 129,
    HasOptionalInputArgumentDescription = 131,
    AlwaysGeneratesEvent = 3065,
    HasTrueSubState = 9004,
    HasFalseSubState = 9005,
    HasCondition = 9006,
    HasPubSubConnection = 14476,
    DataSetToWriter = 14936,
    HasGuard = 15112,
    HasDataSetWriter = 15296,
    HasDataSetReader = 15297,
    HasAlarmSuppressionGroup = 16361,
    AlarmGroupMember = 16362,
    HasEffectDisable = 17276,
    HasDictionaryEntry = 17597,
    HasInterface = 17603,
    HasAddIn = 17604,
    HasEffectEnable = 17983,
    HasEffectSuppressed = 17984,
    HasEffectUnsuppressed = 17985,
    HasWriterGroup = 18804,
    HasReaderGroup = 18805,
    AliasFor = 23469,
};
Q_ENUM_NS(ReferenceTypeId)

enum Types
{
    Boolean         = 0,
    Int32           = 1,
    UInt32          = 2,
    Double          = 3,
    Float           = 4,
    String          = 5,
    LocalizedText   = 6,
    DateTime        = 7,
    UInt16          = 8,
    Int16           = 9,
    UInt64          = 10,
    Int64           = 11,
    Byte            = 12,
    SByte           = 13,
    ByteString      = 14,
    XmlElement      = 15,
    NodeId          = 16,
    Guid            = 17,
    QualifiedName   = 18,
    StatusCode      = 19,
    ExtensionObject = 20,
    Range           = 21,
    EUInformation   = 22,
    ComplexNumber   = 23,
    DoubleComplexNumber = 24,
    AxisInformation = 25,
    XV              = 26,
    ExpandedNodeId  = 27,
    Argument        = 28,
    Undefined       = 0xFFFFFFFF
};
Q_ENUM_NS(Types)

// OpcUa Specification Part 4, Chapter 7.34 "Status Code"
// OpcUa Specification Part 6, Annex A.2 "Status Codes
enum UaStatusCode : quint32
{
    Good = 0,
    BadUnexpectedError = 0x80010000,
    BadInternalError = 0x80020000,
    BadOutOfMemory = 0x80030000,
    BadResourceUnavailable = 0x80040000,
    BadCommunicationError = 0x80050000,
    BadEncodingError = 0x80060000,
    BadDecodingError = 0x80070000,
    BadEncodingLimitsExceeded = 0x80080000,
    BadRequestTooLarge = 0x80B80000,
    BadResponseTooLarge = 0x80B90000,
    BadUnknownResponse = 0x80090000,
    BadTimeout = 0x800A0000,
    BadServiceUnsupported = 0x800B0000,
    BadShutdown = 0x800C0000,
    BadServerNotConnected = 0x800D0000,
    BadServerHalted = 0x800E0000,
    BadNothingToDo = 0x800F0000,
    BadTooManyOperations = 0x80100000,
    BadTooManyMonitoredItems = 0x80DB0000,
    BadDataTypeIdUnknown = 0x80110000,
    BadCertificateInvalid = 0x80120000,
    BadSecurityChecksFailed = 0x80130000,
    BadCertificateTimeInvalid = 0x80140000,
    BadCertificateIssuerTimeInvalid = 0x80150000,
    BadCertificateHostNameInvalid = 0x80160000,
    BadCertificateUriInvalid = 0x80170000,
    BadCertificateUseNotAllowed = 0x80180000,
    BadCertificateIssuerUseNotAllowed = 0x80190000,
    BadCertificateUntrusted = 0x801A0000,
    BadCertificateRevocationUnknown = 0x801B0000,
    BadCertificateIssuerRevocationUnknown = 0x801C0000,
    BadCertificateRevoked = 0x801D0000,
    BadCertificateIssuerRevoked = 0x801E0000,
    BadCertificateChainIncomplete = 0x810D0000,
    BadUserAccessDenied = 0x801F0000,
    BadIdentityTokenInvalid = 0x80200000,
    BadIdentityTokenRejected = 0x80210000,
    BadSecureChannelIdInvalid = 0x80220000,
    BadInvalidTimestamp = 0x80230000,
    BadNonceInvalid = 0x80240000,
    BadSessionIdInvalid = 0x80250000,
    BadSessionClosed = 0x80260000,
    BadSessionNotActivated = 0x80270000,
    BadSubscriptionIdInvalid = 0x80280000,
    BadRequestHeaderInvalid = 0x802A0000,
    BadTimestampsToReturnInvalid = 0x802B0000,
    BadRequestCancelledByClient = 0x802C0000,
    BadTooManyArguments = 0x80E50000,
    GoodSubscriptionTransferred = 0x002D0000,
    GoodCompletesAsynchronously = 0x002E0000,
    GoodOverload = 0x002F0000,
    GoodClamped = 0x00300000,
    BadNoCommunication = 0x80310000,
    BadWaitingForInitialData = 0x80320000,
    BadNodeIdInvalid = 0x80330000,
    BadNodeIdUnknown = 0x80340000,
    BadAttributeIdInvalid = 0x80350000,
    BadIndexRangeInvalid = 0x80360000,
    BadIndexRangeNoData = 0x80370000,
    BadDataEncodingInvalid = 0x80380000,
    BadDataEncodingUnsupported = 0x80390000,
    BadNotReadable = 0x803A0000,
    BadNotWritable = 0x803B0000,
    BadOutOfRange = 0x803C0000,
    BadNotSupported = 0x803D0000,
    BadNotFound = 0x803E0000,
    BadObjectDeleted = 0x803F0000,
    BadNotImplemented = 0x80400000,
    BadMonitoringModeInvalid = 0x80410000,
    BadMonitoredItemIdInvalid = 0x80420000,
    BadMonitoredItemFilterInvalid = 0x80430000,
    BadMonitoredItemFilterUnsupported = 0x80440000,
    BadFilterNotAllowed = 0x80450000,
    BadStructureMissing = 0x80460000,
    BadEventFilterInvalid = 0x80470000,
    BadContentFilterInvalid = 0x80480000,
    BadFilterOperatorInvalid = 0x80C10000,
    BadFilterOperatorUnsupported = 0x80C20000,
    BadFilterOperandCountMismatch = 0x80C30000,
    BadFilterOperandInvalid = 0x80490000,
    BadFilterElementInvalid = 0x80C40000,
    BadFilterLiteralInvalid = 0x80C50000,
    BadContinuationPointInvalid = 0x804A0000,
    BadNoContinuationPoints = 0x804B0000,
    BadReferenceTypeIdInvalid = 0x804C0000,
    BadBrowseDirectionInvalid = 0x804D0000,
    BadNodeNotInView = 0x804E0000,
    BadServerUriInvalid = 0x804F0000,
    BadServerNameMissing = 0x80500000,
    BadDiscoveryUrlMissing = 0x80510000,
    BadSempahoreFileMissing = 0x80520000,
    BadRequestTypeInvalid = 0x80530000,
    BadSecurityModeRejected = 0x80540000,
    BadSecurityPolicyRejected = 0x80550000,
    BadTooManySessions = 0x80560000,
    BadUserSignatureInvalid = 0x80570000,
    BadApplicationSignatureInvalid = 0x80580000,
    BadNoValidCertificates = 0x80590000,
    BadIdentityChangeNotSupported = 0x80C60000,
    BadRequestCancelledByRequest = 0x805A0000,
    BadParentNodeIdInvalid = 0x805B0000,
    BadReferenceNotAllowed = 0x805C0000,
    BadNodeIdRejected = 0x805D0000,
    BadNodeIdExists = 0x805E0000,
    BadNodeClassInvalid = 0x805F0000,
    BadBrowseNameInvalid = 0x80600000,
    BadBrowseNameDuplicated = 0x80610000,
    BadNodeAttributesInvalid = 0x80620000,
    BadTypeDefinitionInvalid = 0x80630000,
    BadSourceNodeIdInvalid = 0x80640000,
    BadTargetNodeIdInvalid = 0x80650000,
    BadDuplicateReferenceNotAllowed = 0x80660000,
    BadInvalidSelfReference = 0x80670000,
    BadReferenceLocalOnly = 0x80680000,
    BadNoDeleteRights = 0x80690000,
    UncertainReferenceNotDeleted = 0x40BC0000,
    BadServerIndexInvalid = 0x806A0000,
    BadViewIdUnknown = 0x806B0000,
    BadViewTimestampInvalid = 0x80C90000,
    BadViewParameterMismatch = 0x80CA0000,
    BadViewVersionInvalid = 0x80CB0000,
    UncertainNotAllNodesAvailable = 0x40C00000,
    GoodResultsMayBeIncomplete = 0x00BA0000,
    BadNotTypeDefinition = 0x80C80000,
    UncertainReferenceOutOfServer = 0x406C0000,
    BadTooManyMatches = 0x806D0000,
    BadQueryTooComplex = 0x806E0000,
    BadNoMatch = 0x806F0000,
    BadMaxAgeInvalid = 0x80700000,
    BadSecurityModeInsufficient = 0x80E60000,
    BadHistoryOperationInvalid = 0x80710000,
    BadHistoryOperationUnsupported = 0x80720000,
    BadInvalidTimestampArgument = 0x80BD0000,
    BadWriteNotSupported = 0x80730000,
    BadTypeMismatch = 0x80740000,
    BadMethodInvalid = 0x80750000,
    BadArgumentsMissing = 0x80760000,
    BadTooManySubscriptions = 0x80770000,
    BadTooManyPublishRequests = 0x80780000,
    BadNoSubscription = 0x80790000,
    BadSequenceNumberUnknown = 0x807A0000,
    BadMessageNotAvailable = 0x807B0000,
    BadInsufficientClientProfile = 0x807C0000,
    BadStateNotActive = 0x80BF0000,
    BadTcpServerTooBusy = 0x807D0000,
    BadTcpMessageTypeInvalid = 0x807E0000,
    BadTcpSecureChannelUnknown = 0x807F0000,
    BadTcpMessageTooLarge = 0x80800000,
    BadTcpNotEnoughResources = 0x80810000,
    BadTcpInternalError = 0x80820000,
    BadTcpEndpointUrlInvalid = 0x80830000,
    BadRequestInterrupted = 0x80840000,
    BadRequestTimeout = 0x80850000,
    BadSecureChannelClosed = 0x80860000,
    BadSecureChannelTokenUnknown = 0x80870000,
    BadSequenceNumberInvalid = 0x80880000,
    BadProtocolVersionUnsupported = 0x80BE0000,
    BadConfigurationError = 0x80890000,
    BadNotConnected = 0x808A0000,
    BadDeviceFailure = 0x808B0000,
    BadSensorFailure = 0x808C0000,
    BadOutOfService = 0x808D0000,
    BadDeadbandFilterInvalid = 0x808E0000,
    UncertainNoCommunicationLastUsableValue = 0x408F0000,
    UncertainLastUsableValue = 0x40900000,
    UncertainSubstituteValue = 0x40910000,
    UncertainInitialValue = 0x40920000,
    UncertainSensorNotAccurate = 0x40930000,
    UncertainEngineeringUnitsExceeded = 0x40940000,
    UncertainSubNormal = 0x40950000,
    GoodLocalOverride = 0x00960000,
    BadRefreshInProgress = 0x80970000,
    BadConditionAlreadyDisabled = 0x80980000,
    BadConditionAlreadyEnabled = 0x80CC0000,
    BadConditionDisabled = 0x80990000,
    BadEventIdUnknown = 0x809A0000,
    BadEventNotAcknowledgeable = 0x80BB0000,
    BadDialogNotActive = 0x80CD0000,
    BadDialogResponseInvalid = 0x80CE0000,
    BadConditionBranchAlreadyAcked = 0x80CF0000,
    BadConditionBranchAlreadyConfirmed = 0x80D00000,
    BadConditionAlreadyShelved = 0x80D10000,
    BadConditionNotShelved = 0x80D20000,
    BadShelvingTimeOutOfRange = 0x80D30000,
    BadNoData = 0x809B0000,
    BadBoundNotFound = 0x80D70000,
    BadBoundNotSupported = 0x80D80000,
    BadDataLost = 0x809D0000,
    BadDataUnavailable = 0x809E0000,
    BadEntryExists = 0x809F0000,
    BadNoEntryExists = 0x80A00000,
    BadTimestampNotSupported = 0x80A10000,
    GoodEntryInserted = 0x00A20000,
    GoodEntryReplaced = 0x00A30000,
    UncertainDataSubNormal = 0x40A40000,
    GoodNoData = 0x00A50000,
    GoodMoreData = 0x00A60000,
    BadAggregateListMismatch = 0x80D40000,
    BadAggregateNotSupported = 0x80D50000,
    BadAggregateInvalidInputs = 0x80D60000,
    BadAggregateConfigurationRejected = 0x80DA0000,
    GoodDataIgnored = 0x00D90000,
    BadRequestNotAllowed = 0x80E40000,
    GoodEdited = 0x00DC0000,
    GoodPostActionFailed = 0x00DD0000,
    UncertainDominantValueChanged = 0x40DE0000,
    GoodDependentValueChanged = 0x00E00000,
    BadDominantValueChanged = 0x80E10000,
    UncertainDependentValueChanged = 0x40E20000,
    BadDependentValueChanged = 0x80E30000,
    GoodCommunicationEvent = 0x00A70000,
    GoodShutdownEvent = 0x00A80000,
    GoodCallAgain = 0x00A90000,
    GoodNonCriticalTimeout = 0x00AA0000,
    BadInvalidArgument = 0x80AB0000,
    BadConnectionRejected = 0x80AC0000,
    BadDisconnect = 0x80AD0000,
    BadConnectionClosed = 0x80AE0000,
    BadInvalidState = 0x80AF0000,
    BadEndOfStream = 0x80B00000,
    BadNoDataAvailable = 0x80B10000,
    BadWaitingForResponse = 0x80B20000,
    BadOperationAbandoned = 0x80B30000,
    BadExpectedStreamToBlock = 0x80B40000,
    BadWouldBlock = 0x80B50000,
    BadSyntaxError = 0x80B60000,
    BadMaxConnectionsReached = 0x80B70000
};
Q_ENUM_NS(UaStatusCode)

enum class ErrorCategory {
    NoError,
    NodeError,
    AttributeError,
    PermissionError,
    ArgumentError,
    TypeError,
    ConnectionError,
    UnspecifiedError
};
Q_ENUM_NS(ErrorCategory)

Q_OPCUA_EXPORT bool isSuccessStatus(QOpcUa::UaStatusCode statusCode);
Q_OPCUA_EXPORT QOpcUa::ErrorCategory errorCategory(QOpcUa::UaStatusCode statusCode);
Q_OPCUA_EXPORT QString statusToString(QOpcUa::UaStatusCode statusCode);
Q_OPCUA_EXPORT bool isSecurePolicy(const QString &securityPolicy);

// NodeId helpers
Q_OPCUA_EXPORT QString nodeIdFromString(quint16 ns, const QString &identifier);
Q_OPCUA_EXPORT QString nodeIdFromByteString(quint16 ns, const QByteArray &identifier);
Q_OPCUA_EXPORT QString nodeIdFromGuid(quint16 ns, const QUuid &identifier);
Q_OPCUA_EXPORT QString nodeIdFromInteger(quint16 ns, quint32 identifier);
Q_OPCUA_EXPORT QString nodeIdFromReferenceType(QOpcUa::ReferenceTypeId referenceType);
Q_OPCUA_EXPORT bool nodeIdStringSplit(const QString &nodeIdString, quint16 *nsIndex,
                                      QString *identifier, char *identifierType);
Q_OPCUA_EXPORT bool nodeIdEquals(const QString &first, const QString &second);
Q_OPCUA_EXPORT QString namespace0Id(QOpcUa::NodeIds::Namespace0 id);
Q_OPCUA_EXPORT QOpcUa::NodeIds::Namespace0 namespace0IdFromNodeId(const QString &nodeId);
Q_OPCUA_EXPORT QString namespace0IdName(QOpcUa::NodeIds::Namespace0 id);

typedef QPair<QVariant, QOpcUa::Types> TypedVariant;

enum class AxisScale : quint32 {
    Linear = 0,
    Log = 1,
    Ln = 2
};

Q_OPCUA_EXPORT QOpcUa::Types metaTypeToQOpcUaType(QMetaType::Type type);
Q_OPCUA_EXPORT QOpcUa::Types opcUaDataTypeToQOpcUaType(const QString &type);
}

Q_DECLARE_TYPEINFO(QOpcUa::Types, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QOpcUa::UaStatusCode, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QOpcUa::ErrorCategory, Q_PRIMITIVE_TYPE);

Q_DECLARE_TYPEINFO(QOpcUa::NodeClass, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QOpcUa::NodeAttribute, Q_PRIMITIVE_TYPE);
Q_DECLARE_OPERATORS_FOR_FLAGS(QOpcUa::NodeAttributes)
Q_DECLARE_OPERATORS_FOR_FLAGS(QOpcUa::WriteMask)
Q_DECLARE_OPERATORS_FOR_FLAGS(QOpcUa::AccessLevel)
Q_DECLARE_OPERATORS_FOR_FLAGS(QOpcUa::EventNotifier)
Q_DECLARE_OPERATORS_FOR_FLAGS(QOpcUa::NodeClasses)
Q_DECLARE_TYPEINFO(QOpcUa::ReferenceTypeId, Q_PRIMITIVE_TYPE);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUa::Types)
Q_DECLARE_METATYPE(QOpcUa::TypedVariant)
Q_DECLARE_METATYPE(QOpcUa::UaStatusCode)
Q_DECLARE_METATYPE(QOpcUa::ErrorCategory)
Q_DECLARE_METATYPE(QOpcUa::NodeClass)
Q_DECLARE_METATYPE(QOpcUa::NodeAttribute)
Q_DECLARE_METATYPE(QOpcUa::NodeAttributes)
Q_DECLARE_METATYPE(QOpcUa::WriteMaskBit)
Q_DECLARE_METATYPE(QOpcUa::WriteMask)
Q_DECLARE_METATYPE(QOpcUa::AccessLevelBit)
Q_DECLARE_METATYPE(QOpcUa::AccessLevel)
Q_DECLARE_METATYPE(QOpcUa::EventNotifierBit)
Q_DECLARE_METATYPE(QOpcUa::EventNotifier)
Q_DECLARE_METATYPE(QOpcUa::ReferenceTypeId)
Q_DECLARE_METATYPE(QOpcUa::NodeClasses)

#endif // QOPCUATYPE

// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuatype.h"

#include <QMetaEnum>
#include <QRegularExpression>
#include <QUuid>
#include <QString>
#include <QList>

QT_BEGIN_NAMESPACE

/*!
    \namespace QOpcUa
    \inmodule QtOpcua

    \brief The QOpcUa namespace contains miscellaneous identifiers used throughout the Qt OPC UA library.
*/

/*!
    \enum QOpcUa::NodeClass

    This enum specifies the class a node belongs to. OPC-UA specifies a fixed
    set of eight different classes.

    \value Undefined     The node class is not known. This is the case before
                         the NodeClass attribute has been read on the server.

    \value Object        An Object node.
    \value Variable      A Variable node.
    \value Method        A Method node.
    \value ObjectType    An ObjectType node.
    \value VariableType  A VariableType node.
    \value ReferenceType A ReferenceType node.
    \value DataType      A DataType node.
    \value View          A View node.
*/

/*!
    \enum QOpcUa::ReferenceTypeId

    This enum contains the reference types specified in OPC-UA part 3, 7.
    They are used to filter for a certain reference type in \l QOpcUaNode::browseChildren
    and for the reference type information in \l QOpcUaReferenceDescription.

    \value Unspecified Not a valid reference type.
    \value References The abstract base type for all references.
    \value NonHierarchicalReferences The abstract base type for all non-hierarchical references.
    \value HierarchicalReferences The abstract base type for all hierarchical references.
    \value HasChild The abstract base type for all non-looping hierarchical references.
    \value Organizes The type for hierarchical references that are used to organize nodes.
    \value HasEventSource The type for non-looping hierarchical references that are used to organize event sources.
    \value HasModellingRule The type for references from instance declarations to modelling rule nodes.
    \value HasEncoding The type for references from data type nodes to to data type encoding nodes.
    \value HasDescription The type for references from data type encoding nodes to data type description nodes.
    \value HasTypeDefinition The type for references from a instance node to its type definition node.
    \value GeneratesEvent The type for references from a node to an event type that is raised by node.
    \value Aggregates The type for non-looping hierarchical references that are used to aggregate nodes into complex types.
    \value HasSubtype The type for non-looping hierarchical references that are used to define sub types.
    \value HasProperty The type for non-looping hierarchical reference from a node to its property.
    \value HasComponent The type for non-looping hierarchical reference from a node to its component.
    \value HasNotifier The type for non-looping hierarchical references that are used to indicate how events propagate from node to node.
    \value HasOrderedComponent The type for non-looping hierarchical reference from a node to its component when the order of references matters.
    \value FromState The type for a reference to the state before a transition.
    \value ToState The type for a reference to the state after a transition.
    \value HasCause The type for a reference to a method that can cause a transition to occur.
    \value HasEffect The type for a reference to an event that may be raised when a transition occurs.
    \value HasHistoricalConfiguration The type for a reference to the historical configuration for a data variable.
    \value HasSubStateMachine The type for a reference to a substate for a state.
    \value HasArgumentDescription The type for a reference to an argument description
    \value HasOptionalInputArgumentDescription The type for a reference to an optional input argument description
    \value AlwaysGeneratesEvent The type for references from a node to an event type that is always raised by node.
    \value HasTrueSubState The type for references from a TRUE super state node to a subordinate state node.
    \value HasFalseSubState The type for references from a FALSE super state node to a subordinate state node.
    \value HasCondition The type for references from a ConditionSource node to a Condition.
    \value HasPubSubConnection The type for a reference to a PubSub connection
    \value DataSetToWriter The type for a reference to a data set writer
    \value HasGuard The type for a reference to a guard
    \value HasDataSetWriter The type for a reference to a data set writer
    \value HasDataSetReader The type for a reference to data set reader
    \value HasAlarmSuppressionGroup The type for a reference to an alarm suppression group
    \value AlarmGroupMember The type for a reference to an alarm group member
    \value HasEffectDisable The type for a reference to an alarm which is disabled by a transition
    \value HasDictionaryEntry The type for a reference to a dictionary entry
    \value HasInterface The type for a reference to an interface
    \value HasAddIn The type for a reference to an addin
    \value HasEffectEnable The type for a reference to an alarm which is enabled by a transition
    \value HasEffectSuppressed The type for a reference to an alarm which is suppressed by a transition
    \value HasEffectUnsuppressed The type for a reference to an alarm which is unsuppressed by a transition
    \value HasWriterGroup The type for a reference to a writer group
    \value HasReaderGroup The type for a reference to a reader group
    \value AliasFor The type for a reference to the node represented by an alias
*/

/*!
\enum QOpcUa::NodeAttribute

    This enum contains the 22 node attributes defined in OPC-UA part 4, 5.

    \value None No node attribute.
    \value NodeId Mandatory for all nodes. Contains the node's id in the OPC UA address space.
    \value NodeClass Mandatory for all nodes. Contains the node id describing the node class of the node.
    \value BrowseName Mandatory for all nodes. Contains a non-localized human readable name of the node.
    \value DisplayName Mandatory for all nodes. Contains a localized human readable name for display purposes.
    \value Description Contains a localized human readable description of the node.
    \value WriteMask Contains a bit mask. Each bit corresponds to a writable attribute (OPC-UA part 3, Table 3).
    \value UserWriteMask Same as WriteMask but for the current user.
    \value IsAbstract True if the node is an abstract type which means that no nodes of this type shall exist.
    \value Symmetric True if a reference's meaning is the same seen from both ends.
    \value InverseName The localized inverse name of a reference (for example "HasSubtype" has the InverseName "SubtypeOf").
    \value ContainsNoLoops True if there is no way to get back to a node following forward references in the current view.
    \value EventNotifier Contains a bit mask used to indicate if subscribing to events and access to historic events is supported (OPC-UA part 3, Table 5).
    \value Value The value of a Variable node.
    \value DataType The NodeId of the Value attribute's data type (for example "ns=0;i=13" for DateTime, see https://opcfoundation.org/UA/schemas/1.03/NodeIds.csv).
    \value ValueRank Contains information about the structure of the Value attribute (scalar/array) (OPC-UA part 3, Table 8).
    \value ArrayDimensions An array containing the length for each dimension of a multi-dimensional array.
    \value AccessLevel Contains a bit mask. Each bit corresponds to an access capability (OPC-UA part 3, Table 8).
    \value UserAccessLevel Same as AccessLevel, but for the current user.
    \value MinimumSamplingInterval Contains the shortest possible interval in which the server is able to sample the value.
    \value Historizing True if historical data is collected.
    \value Executable True if the node is currently executable. Only relevant for Method nodes.
    \value UserExecutable Same as Executable, but for the current user.
*/

/*!
    \enum QOpcUa::WriteMaskBit

    This enum contains all possible bits for the WriteMask and UserWriteMask node attributes
    defined in OPC-UA part 3, Table 3.

    \value None No attribute is writable.
    \value AccessLevel The AccessLevel attribute is writable.
    \value ArrayDimensions The ArrayDimensions attribute is writable.
    \value BrowseName The BrowseName attribute is writable.
    \value ContainsNoLoops The ContainsNoLoops attribute is writable.
    \value DataType The DataType attribute is writable.
    \value Description The Description attribute is writable.
    \value DisplayName The DisplayName attribute is writable.
    \value EventNotifier The EventNotifier attribute is writable.
    \value Executable The Executable attribute is writable.
    \value Historizing The Historizing attribute is writable.
    \value InverseName The InverseName attribute is writable.
    \value IsAbstract The IsAbstract attribute is writable.
    \value MinimumSamplingInterval The MinimumSamplingInterval attribute is writable.
    \value NodeClass The NodeClass attribute is writable.
    \value NodeId The NodeId attribute is writable.
    \value Symmetric The Symmetric attribute is writable.
    \value UserAccessLevel The UserAccessLevel attribute is writable.
    \value UserExecutable The UserExecutable attribute is writable.
    \value UserWriteMask The UserWriteMask attribute is writable.
    \value ValueRank The ValueRank attribute is writable.
    \value WriteMask The WriteMask attribute is writable.
    \value ValueForVariableType The Value attribute of a variable type is writable.
*/

/*!
    \enum QOpcUa::AccessLevelBit

    This enum contains all possible bits for the AccessLevel and UserAccessLevel node attributes
    defined in OPC-UA part 3, Table 8.

    \value None No read access to the Value attribute is permitted.
    \value CurrentRead The current value can be read.
    \value CurrentWrite The current value can be written.
    \value HistoryRead The history of the value is readable.
    \value HistoryWrite The history of the value is writable.
    \value SemanticChange The property variable generates SemanticChangeEvents.
    \value StatusWrite The status code of the value is writable.
    \value TimestampWrite The SourceTimestamp is writable.
*/

/*!
    \enum QOpcUa::EventNotifierBit

    This enum contains all possible bits for the EventNotifier node attribute
    defined in OPC-UA part 3, Table 6.

    \value None The node can't be used to interact with events.
    \value SubscribeToEvents A client can subscribe to events.
    \value HistoryRead A client can read the event history.
    \value HistoryWrite A client can write the event history.
*/

/*!
    \fn inline uint QOpcUa::qHash(const QOpcUa::NodeAttribute& attr)

    Returns a \l QHash key for \a attr.
*/

/*!
    \enum QOpcUa::Types

    Enumerates the types supported by Qt OPC UA.

    \value Boolean
    \value Int32
    \value UInt32
    \value Double
    \value Float
    \value String
    \value LocalizedText
    \value DateTime
    \value UInt16
    \value Int16
    \value UInt64
    \value Int64
    \value Byte
    \value SByte
    \value ByteString
    \value XmlElement
    \value NodeId
    \value Guid
    \value QualifiedName A name qualified by an OPC UA namespace index.
    \value StatusCode
    \value ExtensionObject A data structure which contains a serialized object.
    \value Range A range composed from the two double values low and high.
    \value EUInformation The unit of measurement for an analog value.
    \value ComplexNumber The OPC UA ComplexNumber type.
    \value DoubleComplexNumber The OPC UA DoubleComplexNumber type.
    \value AxisInformation Information about an axis.
    \value XV A float value with a double precision position on an axis.
    \value ExpandedNodeId A node id with additional namespace URI and server index.
    \value Argument The OPC UA Argument type.
    \value Undefined
*/

/*!
    \enum QOpcUa::UaStatusCode

    Enumerates all status codes from \l {https://opcfoundation.org/UA/schemas/1.03/Opc.Ua.StatusCodes.csv}

    \value Good
    \value BadUnexpectedError
    \value BadInternalError
    \value BadOutOfMemory
    \value BadResourceUnavailable
    \value BadCommunicationError
    \value BadEncodingError
    \value BadDecodingError
    \value BadEncodingLimitsExceeded
    \value BadRequestTooLarge
    \value BadResponseTooLarge
    \value BadUnknownResponse
    \value BadTimeout
    \value BadServiceUnsupported
    \value BadShutdown
    \value BadServerNotConnected
    \value BadServerHalted
    \value BadNothingToDo
    \value BadTooManyOperations
    \value BadTooManyMonitoredItems
    \value BadDataTypeIdUnknown
    \value BadCertificateInvalid
    \value BadSecurityChecksFailed
    \value BadCertificateTimeInvalid
    \value BadCertificateIssuerTimeInvalid
    \value BadCertificateHostNameInvalid
    \value BadCertificateUriInvalid
    \value BadCertificateUseNotAllowed
    \value BadCertificateIssuerUseNotAllowed
    \value BadCertificateUntrusted
    \value BadCertificateRevocationUnknown
    \value BadCertificateIssuerRevocationUnknown
    \value BadCertificateRevoked
    \value BadCertificateIssuerRevoked
    \value BadCertificateChainIncomplete
    \value BadUserAccessDenied
    \value BadIdentityTokenInvalid
    \value BadIdentityTokenRejected
    \value BadSecureChannelIdInvalid
    \value BadInvalidTimestamp
    \value BadNonceInvalid
    \value BadSessionIdInvalid
    \value BadSessionClosed
    \value BadSessionNotActivated
    \value BadSubscriptionIdInvalid
    \value BadRequestHeaderInvalid
    \value BadTimestampsToReturnInvalid
    \value BadRequestCancelledByClient
    \value BadTooManyArguments
    \value GoodSubscriptionTransferred
    \value GoodCompletesAsynchronously
    \value GoodOverload
    \value GoodClamped
    \value BadNoCommunication
    \value BadWaitingForInitialData
    \value BadNodeIdInvalid
    \value BadNodeIdUnknown
    \value BadAttributeIdInvalid
    \value BadIndexRangeInvalid
    \value BadIndexRangeNoData
    \value BadDataEncodingInvalid
    \value BadDataEncodingUnsupported
    \value BadNotReadable
    \value BadNotWritable
    \value BadOutOfRange
    \value BadNotSupported
    \value BadNotFound
    \value BadObjectDeleted
    \value BadNotImplemented
    \value BadMonitoringModeInvalid
    \value BadMonitoredItemIdInvalid
    \value BadMonitoredItemFilterInvalid
    \value BadMonitoredItemFilterUnsupported
    \value BadFilterNotAllowed
    \value BadStructureMissing
    \value BadEventFilterInvalid
    \value BadContentFilterInvalid
    \value BadFilterOperatorInvalid
    \value BadFilterOperatorUnsupported
    \value BadFilterOperandCountMismatch
    \value BadFilterOperandInvalid
    \value BadFilterElementInvalid
    \value BadFilterLiteralInvalid
    \value BadContinuationPointInvalid
    \value BadNoContinuationPoints
    \value BadReferenceTypeIdInvalid
    \value BadBrowseDirectionInvalid
    \value BadNodeNotInView
    \value BadServerUriInvalid
    \value BadServerNameMissing
    \value BadDiscoveryUrlMissing
    \value BadSempahoreFileMissing
    \value BadRequestTypeInvalid
    \value BadSecurityModeRejected
    \value BadSecurityPolicyRejected
    \value BadTooManySessions
    \value BadUserSignatureInvalid
    \value BadApplicationSignatureInvalid
    \value BadNoValidCertificates
    \value BadIdentityChangeNotSupported
    \value BadRequestCancelledByRequest
    \value BadParentNodeIdInvalid
    \value BadReferenceNotAllowed
    \value BadNodeIdRejected
    \value BadNodeIdExists
    \value BadNodeClassInvalid
    \value BadBrowseNameInvalid
    \value BadBrowseNameDuplicated
    \value BadNodeAttributesInvalid
    \value BadTypeDefinitionInvalid
    \value BadSourceNodeIdInvalid
    \value BadTargetNodeIdInvalid
    \value BadDuplicateReferenceNotAllowed
    \value BadInvalidSelfReference
    \value BadReferenceLocalOnly
    \value BadNoDeleteRights
    \value UncertainReferenceNotDeleted
    \value BadServerIndexInvalid
    \value BadViewIdUnknown
    \value BadViewTimestampInvalid
    \value BadViewParameterMismatch
    \value BadViewVersionInvalid
    \value UncertainNotAllNodesAvailable
    \value GoodResultsMayBeIncomplete
    \value BadNotTypeDefinition
    \value UncertainReferenceOutOfServer
    \value BadTooManyMatches
    \value BadQueryTooComplex
    \value BadNoMatch
    \value BadMaxAgeInvalid
    \value BadSecurityModeInsufficient
    \value BadHistoryOperationInvalid
    \value BadHistoryOperationUnsupported
    \value BadInvalidTimestampArgument
    \value BadWriteNotSupported
    \value BadTypeMismatch
    \value BadMethodInvalid
    \value BadArgumentsMissing
    \value BadTooManySubscriptions
    \value BadTooManyPublishRequests
    \value BadNoSubscription
    \value BadSequenceNumberUnknown
    \value BadMessageNotAvailable
    \value BadInsufficientClientProfile
    \value BadStateNotActive
    \value BadTcpServerTooBusy
    \value BadTcpMessageTypeInvalid
    \value BadTcpSecureChannelUnknown
    \value BadTcpMessageTooLarge
    \value BadTcpNotEnoughResources
    \value BadTcpInternalError
    \value BadTcpEndpointUrlInvalid
    \value BadRequestInterrupted
    \value BadRequestTimeout
    \value BadSecureChannelClosed
    \value BadSecureChannelTokenUnknown
    \value BadSequenceNumberInvalid
    \value BadProtocolVersionUnsupported
    \value BadConfigurationError
    \value BadNotConnected
    \value BadDeviceFailure
    \value BadSensorFailure
    \value BadOutOfService
    \value BadDeadbandFilterInvalid
    \value UncertainNoCommunicationLastUsableValue
    \value UncertainLastUsableValue
    \value UncertainSubstituteValue
    \value UncertainInitialValue
    \value UncertainSensorNotAccurate
    \value UncertainEngineeringUnitsExceeded
    \value UncertainSubNormal
    \value GoodLocalOverride
    \value BadRefreshInProgress
    \value BadConditionAlreadyDisabled
    \value BadConditionAlreadyEnabled
    \value BadConditionDisabled
    \value BadEventIdUnknown
    \value BadEventNotAcknowledgeable
    \value BadDialogNotActive
    \value BadDialogResponseInvalid
    \value BadConditionBranchAlreadyAcked
    \value BadConditionBranchAlreadyConfirmed
    \value BadConditionAlreadyShelved
    \value BadConditionNotShelved
    \value BadShelvingTimeOutOfRange
    \value BadNoData
    \value BadBoundNotFound
    \value BadBoundNotSupported
    \value BadDataLost
    \value BadDataUnavailable
    \value BadEntryExists
    \value BadNoEntryExists
    \value BadTimestampNotSupported
    \value GoodEntryInserted
    \value GoodEntryReplaced
    \value UncertainDataSubNormal
    \value GoodNoData
    \value GoodMoreData
    \value BadAggregateListMismatch
    \value BadAggregateNotSupported
    \value BadAggregateInvalidInputs
    \value BadAggregateConfigurationRejected
    \value GoodDataIgnored
    \value BadRequestNotAllowed
    \value GoodEdited
    \value GoodPostActionFailed
    \value UncertainDominantValueChanged
    \value GoodDependentValueChanged
    \value BadDominantValueChanged
    \value UncertainDependentValueChanged
    \value BadDependentValueChanged
    \value GoodCommunicationEvent
    \value GoodShutdownEvent
    \value GoodCallAgain
    \value GoodNonCriticalTimeout
    \value BadInvalidArgument
    \value BadConnectionRejected
    \value BadDisconnect
    \value BadConnectionClosed
    \value BadInvalidState
    \value BadEndOfStream
    \value BadNoDataAvailable
    \value BadWaitingForResponse
    \value BadOperationAbandoned
    \value BadExpectedStreamToBlock
    \value BadWouldBlock
    \value BadSyntaxError
    \value BadMaxConnectionsReached
*/

/*!
    \enum QOpcUa::ErrorCategory

    This enum contains simplified categories for OPC UA errors.

    \value NoError The operation has been successful.
    \value NodeError There is a problem with the node, e. g. it does not exist.
    \value AttributeError The attributes to operate on where invalid.
    \value PermissionError The user did not have the permission to perform the operation.
    \value ArgumentError The arguments supplied by the user were invalid or incomplete.
    \value TypeError There has been a type mismatch for a write operation.
    \value ConnectionError Communication with the server did not work as expected.
    \value UnspecifiedError Any error that is not categorized. The detailed status code must be checked.
*/

/*!
    This method can be used to check if a call has successfully finished.

    Returns \c true if \a statusCode's severity field is Good.
*/
bool QOpcUa::isSuccessStatus(QOpcUa::UaStatusCode statusCode)
{
    return (statusCode & 0xC0000000) == 0;
}

/*!
    \typedef QOpcUa::TypedVariant

    This is QPair<QVariant, QOpcUa::Types>.
*/

static bool isNodeError(QOpcUa::UaStatusCode statusCode)
{
    switch (statusCode) {
    case QOpcUa::UaStatusCode::BadMethodInvalid:
    case QOpcUa::UaStatusCode::BadNodeIdInvalid:
    case QOpcUa::UaStatusCode::BadNodeIdExists:
    case QOpcUa::UaStatusCode::BadNodeIdRejected:
    case QOpcUa::UaStatusCode::BadNodeIdUnknown:
    case QOpcUa::UaStatusCode::BadObjectDeleted:
        return true;
    default:
        return false;
    }
}

static bool isAttributeError(QOpcUa::UaStatusCode statusCode)
{
    switch (statusCode) {
    case QOpcUa::UaStatusCode::BadAttributeIdInvalid:
    case QOpcUa::UaStatusCode::BadNodeAttributesInvalid:
        return true;
    default:
        return false;
    }
}

static bool isPermissionError(QOpcUa::UaStatusCode statusCode)
{
    switch (statusCode) {
    case QOpcUa::UaStatusCode::BadUserAccessDenied:
    case QOpcUa::UaStatusCode::BadNotWritable:
    case QOpcUa::UaStatusCode::BadNoDeleteRights:
    case QOpcUa::UaStatusCode::BadNotReadable:
        return true;
    default:
        return false;
    }
}

static bool isArgumentError(QOpcUa::UaStatusCode statusCode)
{
    switch (statusCode) {
    case QOpcUa::UaStatusCode::BadArgumentsMissing:
    case QOpcUa::UaStatusCode::BadInvalidArgument:
    case QOpcUa::UaStatusCode::BadTooManyArguments:
    case QOpcUa::UaStatusCode::BadInvalidTimestampArgument:
        return true;
    default:
        return false;
    }
}

static bool isTypeError(QOpcUa::UaStatusCode statusCode)
{
    switch (statusCode) {
    case QOpcUa::UaStatusCode::BadTypeMismatch:
    case QOpcUa::UaStatusCode::BadInvalidTimestampArgument:
        return true;
    default:
        return false;
    }
}

static bool isConnectionError(QOpcUa::UaStatusCode statusCode)
{
    switch (statusCode) {
    case QOpcUa::UaStatusCode::BadConnectionClosed:
    case QOpcUa::UaStatusCode::BadNoCommunication:
        return true;
    default:
        return false;
    }
}

/*!
    Converts \a statusCode to an \l ErrorCategory. \l ErrorCategory can be used in cases where the
    exact error is not important.

    For error handling dependent on status codes, the full status code must be used instead.
    The meaning of the status codes for the different services is documented in OPC-UA part 4.

    If \a statusCode has not been categorized, UnspecifiedError is returned. In this case, the user
    must check the full status code.
*/
QOpcUa::ErrorCategory QOpcUa::errorCategory(QOpcUa::UaStatusCode statusCode)
{
    if (isSuccessStatus(statusCode))
        return QOpcUa::ErrorCategory::NoError;
    if (isNodeError(statusCode))
        return QOpcUa::ErrorCategory::NodeError;
    if (isAttributeError(statusCode))
        return QOpcUa::ErrorCategory::AttributeError;
    if (isPermissionError(statusCode))
        return QOpcUa::ErrorCategory::PermissionError;
    if (isArgumentError(statusCode))
        return QOpcUa::ErrorCategory::ArgumentError;
    if (isTypeError(statusCode))
        return QOpcUa::ErrorCategory::TypeError;
    if (isConnectionError(statusCode))
        return QOpcUa::ErrorCategory::ConnectionError;

    return QOpcUa::ErrorCategory::UnspecifiedError;
}

/*!
    Creates a node id string from the namespace index \a ns and the string \a identifier.
    \sa QOpcUaNode
*/
QString QOpcUa::nodeIdFromString(quint16 ns, const QString &identifier)
{
    return QStringLiteral("ns=%1;s=%2").arg(ns).arg(identifier);
}

/*!
    Creates a node id string from the namespace index \a ns and the byte string \a identifier.
    \sa QOpcUaNode
*/
QString QOpcUa::nodeIdFromByteString(quint16 ns, const QByteArray &identifier)
{
    return QStringLiteral("ns=%1;b=%2").arg(ns).arg(QString::fromUtf8(identifier.toBase64()));
}

/*!
    Creates a node id string from the namespace index \a ns and the GUID \a identifier.
    \sa QOpcUaNode
*/
QString QOpcUa::nodeIdFromGuid(quint16 ns, const QUuid &identifier)
{
    return QStringLiteral("ns=%1;g=").arg(ns).append(QStringView(identifier.toString()).mid(1, 36)); // Remove enclosing {...};
}

/*!
    Creates a node id string from the namespace index \a ns and the integer \a identifier.
    \sa QOpcUaNode
*/
QString QOpcUa::nodeIdFromInteger(quint16 ns, quint32 identifier)
{
    return QStringLiteral("ns=%1;i=%2").arg(ns).arg(identifier);
}

/*!
    Creates a node id string for the reference type id \a referenceType.
*/
QString QOpcUa::nodeIdFromReferenceType(QOpcUa::ReferenceTypeId referenceType)
{
    return QStringLiteral("ns=0;i=%1").arg(static_cast<quint32>(referenceType));
}

/*!
    Splits the node id string \a nodeIdString in its components.
    The namespace index of the node id will be copied into \a nsIndex.
    The identifier string is copied into \a identifier and the identifier type
    (i, s, g, b) is copied into \a identifierType.

    Returns \c true if the node id could be split successfully.

    For example, "ns=1;s=MyString" is split into 1, 's' and "MyString".
    If no namespace index is given, ns=0 is assumed.
*/
bool QOpcUa::nodeIdStringSplit(const QString &nodeIdString, quint16 *nsIndex, QString *identifier, char *identifierType)
{
    quint16 namespaceIndex = 0;

    QStringList components = nodeIdString.split(QLatin1String(";"));

    if (components.size() > 2)
        return false;

    if (components.size() == 2 && components.at(0).contains(QRegularExpression(QLatin1String("^ns=[0-9]+")))) {
        bool success = false;
        uint ns = QStringView(components.at(0)).mid(3).toUInt(&success);
        if (!success || ns > (std::numeric_limits<quint16>::max)())
            return false;
        namespaceIndex = ns;
    }

    if (components.last().size() < 3)
        return false;

    if (!components.last().contains(QRegularExpression(QLatin1String("^[isgb]="))))
        return false;

    if (nsIndex)
        *nsIndex = namespaceIndex;
    if (identifier)
        *identifier = QStringView(components.last()).mid(2).toString();
    if (identifierType)
        *identifierType = components.last().at(0).toLatin1();

    return true;
}

/*!
    Returns \c true if the two node ids \a first and \a second have the same namespace index and identifier.
    A node id string without a namespace index is assumed to be in namespace 0.
*/
bool QOpcUa::nodeIdEquals(const QString &first, const QString &second)
{
    const QStringView fView(first);
    const QStringView sView(second);
    if (first.startsWith(QLatin1String("ns=0;")) && !second.startsWith(QLatin1String("ns=")))
        return fView.mid(5) == sView;
    else if (second.startsWith(QLatin1String("ns=0;")) && !first.startsWith(QLatin1String("ns=")))
        return sView.mid(5) == fView;
    else
        return first == second;
}

/*!
    Returns a node id string for the namespace 0 identifier \a id.
*/
QString QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0 id)
{
    return QStringLiteral("ns=0;i=%1").arg(quint32(id));
}

/*!
    Returns the enum value from \l QOpcUa::NodeIds::Namespace0 for \a nodeId.

    If the node id is not in namespace 0 or doesn't have a numeric
    identifier which is part of the OPC Foundation's NodeIds.csv file,
    \l {QOpcUa::NodeIds::Namespace0} {Unknown} is returned.

    If Qt OPC UA has been configured with -no-feature-ns0idnames,
    the check if the numeric identifier is part of the NodeIds.csv
    file is omitted. If the node id is in namespace 0 and has a
    numeric identifier, the identifier is returned regardless if it
    is part of the \l QOpcUa::NodeIds::Namespace0 enum.
*/
QOpcUa::NodeIds::Namespace0 QOpcUa::namespace0IdFromNodeId(const QString &nodeId)
{
    if (!nodeId.startsWith(QLatin1String("ns=0;i=")))
        return QOpcUa::NodeIds::Namespace0::Unknown;

    const QStringView sv = QStringView{nodeId}.mid(7);

    bool ok = false;
    quint32 identifier = sv.toUInt(&ok);

    if (!ok)
        return QOpcUa::NodeIds::Namespace0::Unknown;

#ifndef QT_OPCUA_NO_NS0IDNAMES
    if (!QMetaEnum::fromType<QOpcUa::NodeIds::Namespace0>().valueToKey(identifier))
        return QOpcUa::NodeIds::Namespace0::Unknown;
#endif

    return QOpcUa::NodeIds::Namespace0(identifier);
}

/*!
    Returns the name of the namespace 0 node id \a id.

    If \a id is unknown or Qt OPC UA has been configured with -no-feature-ns0idnames,
    an empty string is returned.
*/
QString QOpcUa::namespace0IdName(QOpcUa::NodeIds::Namespace0 id)
{
#ifdef QT_OPCUA_NO_NS0IDNAMES
    Q_UNUSED(id);
    return QString();
#else
    if (id == QOpcUa::NodeIds::Namespace0::Unknown)
        return QString();
    return QString::fromUtf8(QMetaEnum::fromType<QOpcUa::NodeIds::Namespace0>().valueToKey(quint32(id)));
#endif
}

/*!
    \enum QOpcUa::AxisScale

    The AxisScale enum as defined by OPC-UA part 8, 5.6.7.

    \value Linear
    \value Log
    \value Ln
*/

/*!
    Returns a textual representation of \a statusCode.

    Currently, this is the name of the enum value but may be a real message in future releases.
*/
QString QOpcUa::statusToString(QOpcUa::UaStatusCode statusCode)
{
    const auto enumerator = QMetaEnum::fromType<QOpcUa::UaStatusCode>();
    const auto key = enumerator.valueToKey(statusCode);
    if (key)
        return QString::fromLatin1(key);
    else
        return QLatin1String("Invalid enum value for UaStatusCode");
}

/*!
    \since 5.13

    Returns the Qt OPC UA type from \a type.
    In case the type does not map, \c QOpcUa::Undefined is returned.
*/
QOpcUa::Types QOpcUa::metaTypeToQOpcUaType(QMetaType::Type type) {
    switch (type) {
    case QMetaType::Bool:
        return QOpcUa::Boolean;
    case QMetaType::UChar:
        return QOpcUa::Byte;
    case QMetaType::Char:
        return QOpcUa::SByte;
    case QMetaType::UShort:
        return QOpcUa::UInt16;
    case QMetaType::Short:
        return QOpcUa::Int16;
    case QMetaType::Int:
        return QOpcUa::Int32;
    case QMetaType::UInt:
        return QOpcUa::UInt32;
    case QMetaType::ULongLong:
        return QOpcUa::UInt64;
    case QMetaType::LongLong:
        return QOpcUa::Int64;
    case QMetaType::Double:
        return QOpcUa::Double;
    case QMetaType::Float:
        return QOpcUa::Float;
    case QMetaType::QString:
        return QOpcUa::String;
    case QMetaType::QDateTime:
        return QOpcUa::DateTime;
    case QMetaType::QByteArray:
        return QOpcUa::ByteString;
    case QMetaType::QUuid:
        return QOpcUa::Guid;
    default:
        break;
    }
    return QOpcUa::Undefined;
}

QOpcUa::Types QOpcUa::opcUaDataTypeToQOpcUaType(const QString &type)
{
    if (type == QStringLiteral("ns=0;i=1"))
        return QOpcUa::Boolean;
    else if (type == QStringLiteral("ns=0;i=3"))
        return QOpcUa::Byte;
    else if (type == QStringLiteral("ns=0;i=2"))
        return QOpcUa::SByte;
    else if (type == QStringLiteral("ns=0;i=5"))
        return QOpcUa::UInt16;
    else if (type == QStringLiteral("ns=0;i=4"))
        return QOpcUa::Int16;
    else if (type == QStringLiteral("ns=0;i=6"))
        return QOpcUa::Int32;
    else if (type == QStringLiteral("ns=0;i=7"))
        return QOpcUa::UInt32;
    else if (type == QStringLiteral("ns=0;i=9"))
        return QOpcUa::UInt64;
    else if (type == QStringLiteral("ns=0;i=8"))
        return QOpcUa::Int64;
    else if (type == QStringLiteral("ns=0;i=11"))
        return QOpcUa::Double;
    else if (type == QStringLiteral("ns=0;i=10"))
        return QOpcUa::Float;
    else if (type == QStringLiteral("ns=0;i=12"))
        return QOpcUa::String;
    else if (type == QStringLiteral("ns=0;i=13"))
        return QOpcUa::DateTime;
    else if (type == QStringLiteral("ns=0;i=15"))
        return QOpcUa::ByteString;
    else if (type == QStringLiteral("ns=0;i=14"))
        return QOpcUa::Guid;
    else
        return QOpcUa::Undefined;
}

/*!
    \since QtOpcUa 5.14

    Returns \c true if \a securityPolicy is a secure policy, \c false
    otherwise.
*/
bool QOpcUa::isSecurePolicy(const QString &securityPolicy)
{
    return securityPolicy == QLatin1String("http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15") ||
           securityPolicy == QLatin1String("http://opcfoundation.org/UA/SecurityPolicy#Basic256") ||
           securityPolicy == QLatin1String("http://opcfoundation.org/UA/SecurityPolicy#Basic256Sha256") ||
           securityPolicy == QLatin1String("http://opcfoundation.org/UA/SecurityPolicy#Aes128_Sha256_RsaOaep") ||
           securityPolicy == QLatin1String("http://opcfoundation.org/UA/SecurityPolicy#Aes256_Sha256_RsaPss");
}

QT_END_NAMESPACE


/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qopcuatype.h"

QT_BEGIN_NAMESPACE

/*!
    \namespace QOpcUa
    \ingroup modules
    \inmodule QtOpcua

    \brief The QOpcUa namespace contains miscellaneous identifiers used throughout the Qt OpcUa library.
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
    and for the reference type information in \a QOpcUaReferenceDescription.

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
    \value AlwaysGeneratesEvent The type for references from a node to an event type that is always raised by node.
    \value HasTrueSubState The type for references from a TRUE super state node to a subordinate state node.
    \value HasFalseSubState The type for references from a FALSE super state node to a subordinate state node.
    \value HasCondition The type for references from a ConditionSource node to a Condition.
*/

/*!
    \enum QOpcUa::Types

    Enumerates the types supported by Qt OpcUa.

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
    \value XmlElement \warning Currently not supported by the FreeOPCUA backend
    \value NodeId \warning Currently not supported by the FreeOPCUA backend
    \value Guid
    \value QualifiedName
    \value StatusCode
    \value Undefined
*/

/*!
    \enum QOpcUa::UaStatusCode

    Enumerates all status codes from https://opcfoundation.org/UA/schemas/1.03/Opc.Ua.StatusCodes.csv

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

    Returns true if \a statusCode's serverity field is Good.
*/
bool QOpcUa::isSuccessStatus(QOpcUa::UaStatusCode statusCode)
{
    return (statusCode & 0xC0000000) == 0;
}

/*!
    \typedef QOpcUa::TypedVariant

    This is QPair<QVariant, QOpcUa::Types>.
*/

/*!
  \class QOpcUa::QQualifiedName

  This is the QtOpcUa representation for the OPC UA QualifiedName type defined in OPC-UA part 3, 8.3.
  A QualifiedName is a name qualified by a namespace index. The namespace index corresponds to an entry in the server's namespace array.
  QualifiedName is mainly used to represent the BrowseName attribute of a node.
*/

/*!
    \fn QOpcUa::QQualifiedName::QQualifiedName(quint16 idx, QString text)

    Constructs an instance of QQualifiedName with the namespace index given in \a idx and the name given in \a text.
*/

/*!
    \variable QOpcUa::QQualifiedName::namespaceIndex

    The namespace index of the QQualifiedName.
*/

/*!
    \variable QOpcUa::QQualifiedName::name

    The name string of the QQualifiedName.
*/

/*!
  \class QOpcUa::QLocalizedText

  This is the QtOpcUa representation for the OPC UA LocalizedText type defined in OPC-UA part 3, 8.5.
  A LocalizedText value contains a text string with associated locale information in a second string (e. g. "en" or "en-US").
  The format of the locale information string is <language>[-<country/region>]. Language is usually given as ISO 639 two letter code,
  country/region as ISO 3166 two letter code. Custom codes are also allowed (see OPC-UA part 3, 8.4).
  It can be used to provide multiple text strings in different languages for a value using an array of LocalizedText elements.
*/

/*!
    \fn QOpcUa::QLocalizedText::QLocalizedText(const QString &p_locale, const QString &p_text)

    Constructs an instance of QLocalizedText with the locale given in \a p_locale and the text given in \a p_text.
*/

/*!
    \variable QOpcUa::QLocalizedText::text

    The text string of the QLocalizedText.
*/

/*!
    \variable QOpcUa::QLocalizedText::locale

    The locale of the QLocalizedText.
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

QT_END_NAMESPACE

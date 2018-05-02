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

#include <QUuid>

QT_BEGIN_NAMESPACE

/*!
    \namespace QOpcUa
    \ingroup modules
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

    Returns \c true if \a statusCode's serverity field is Good.
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
    \inmodule QtOpcUa
    \brief The OPC UA QualifiedName type

    This is the Qt OPC UA representation for the OPC UA QualifiedName type defined in OPC-UA part 3, 8.3.
    A QualifiedName is a name qualified by a namespace index. The namespace index corresponds to an entry in the server's namespace array.
    QualifiedName is mainly used to represent the BrowseName attribute of a node.
*/

class QOpcUa::QQualifiedNameData : public QSharedData
{
public:
    QString name;
    quint16 namespaceIndex;
};

QOpcUa::QQualifiedName::QQualifiedName()
    : data(new QOpcUa::QQualifiedNameData)
{
}

/*!
    Constructs a qualified name from \a rhs.
*/
QOpcUa::QQualifiedName::QQualifiedName(const QOpcUa::QQualifiedName &rhs)
    : data(rhs.data)
{
}

/*!
    Constructs a qualified name with namespace index \a namespaceIndex and the name \a name.
*/
QOpcUa::QQualifiedName::QQualifiedName(quint16 namespaceIndex, const QString &name)
    : data(new QOpcUa::QQualifiedNameData)
{
    data->namespaceIndex = namespaceIndex;
    data->name = name;
}

/*!
    Returns \c true if this qualified name has the same value as \a rhs.
*/
bool QOpcUa::QQualifiedName::operator==(const QOpcUa::QQualifiedName &rhs) const
{
    return data->namespaceIndex == rhs.namespaceIndex() &&
            data->name == rhs.name();
}

/*!
    Converts this qualified name to \l QVariant.
*/
QOpcUa::QQualifiedName::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
    Sets the values from \a rhs in this qualified name.
*/
QOpcUa::QQualifiedName &QOpcUa::QQualifiedName::operator=(const QOpcUa::QQualifiedName &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUa::QQualifiedName::~QQualifiedName()
{
}

/*!
    Returns the namespace index.
*/
quint16 QOpcUa::QQualifiedName::namespaceIndex() const
{
    return data->namespaceIndex;
}

/*!
    Sets the namespace index to \a value.
*/
void QOpcUa::QQualifiedName::setNamespaceIndex(const quint16 &value)
{
    data->namespaceIndex = value;
}

/*!
    Returns the name.
*/
QString QOpcUa::QQualifiedName::name() const
{
    return data->name;
}

/*!
    Sets the name to \a value.
*/
void QOpcUa::QQualifiedName::setName(const QString &value)
{
    data->name = value;
}

/*!
    \class QOpcUa::QLocalizedText
    \inmodule QtOpcUa
    \brief The OPC UA LocalizedText type

    This is the Qt OPC UA representation for the OPC UA LocalizedText type defined in OPC-UA part 3, 8.5.
    A LocalizedText value contains a text string with associated locale information in a second string (e. g. "en" or "en-US").
    The format of the locale information string is <language>[-<country/region>]. Language is usually given as ISO 639 two letter code,
    country/region as ISO 3166 two letter code. Custom codes are also allowed (see OPC-UA part 3, 8.4).
    It can be used to provide multiple text strings in different languages for a value using an array of LocalizedText elements.
*/

class QOpcUa::QLocalizedTextData : public QSharedData
{
public:
    QString locale;
    QString text;
};

QOpcUa::QLocalizedText::QLocalizedText()
    : data(new QOpcUa::QLocalizedTextData)
{
}

/*!
    Constructs a localized text from \a rhs.
*/
QOpcUa::QLocalizedText::QLocalizedText(const QOpcUa::QLocalizedText &rhs)
    : data(rhs.data)
{
}

/*!
    Constructs a localized text with the locale \a locale and the text \a text.
*/
QOpcUa::QLocalizedText::QLocalizedText(const QString &locale, const QString &text)
    : data(new QOpcUa::QLocalizedTextData)
{
    data->locale = locale;
    data->text = text;
}

/*!
    Sets the values from \a rhs in this localized text.
*/
QOpcUa::QLocalizedText &QOpcUa::QLocalizedText::operator=(const QOpcUa::QLocalizedText &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this localized text has the same value as \a rhs.
*/
bool QOpcUa::QLocalizedText::operator==(const QOpcUa::QLocalizedText &rhs) const
{
    return data->locale == rhs.locale() &&
            data->text == rhs.text();
}

/*!
    Converts this localized text to \l QVariant.
*/
QOpcUa::QLocalizedText::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QOpcUa::QLocalizedText::~QLocalizedText()
{
}

/*!
    Returns the text.
*/
QString QOpcUa::QLocalizedText::text() const
{
    return data->text;
}

/*!
    Sets the text to \a value.

*/
void QOpcUa::QLocalizedText::setText(const QString &value)
{
    data->text = value;
}

/*!
    Returns the locale.
*/
QString QOpcUa::QLocalizedText::locale() const
{
    return data->locale;
}

/*!
    Sets the locale to \a value.
*/
void QOpcUa::QLocalizedText::setLocale(const QString &value)
{
    data->locale = value;
}

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
    return QStringLiteral("ns=%1;g=").arg(ns).append(identifier.toString().midRef(1, 36)); // Remove enclosing {...};
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
    \class QOpcUa::QRange
    \inmodule QtOpcUa
    \brief The OPC UA Range type

    This is the Qt OPC UA representation for the OPC UA Range type defined in OPC-UA part 8, 5.6.2.
    It consists of two double values which mark minimum and maximum of the range.
    Ranges are mostly used to store information about acceptable values for a node.
*/

class QOpcUa::QRangeData : public QSharedData
{
public:
    double low{0};
    double high{0};
};

QOpcUa::QRange::QRange()
    : data(new QOpcUa::QRangeData)
{
}

/*!
    Constructs a range from \a rhs.
*/
QOpcUa::QRange::QRange(const QOpcUa::QRange &rhs)
    : data(rhs.data)
{
}

/*!
    Constructs a range with low value \a low and high value \a high.
*/
QOpcUa::QRange::QRange(double low, double high)
    : data(new QOpcUa::QRangeData)
{
    data->low = low;
    data->high = high;
}

/*!
    Sets the values from \a rhs in this range.
*/
QOpcUa::QRange &QOpcUa::QRange::operator=(const QOpcUa::QRange &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this range has the same value as \a rhs.
*/
bool QOpcUa::QRange::operator==(const QOpcUa::QRange &rhs) const
{
    return data->low == rhs.low() &&
            data->high == rhs.high();
}

/*!
    Converts this range to \l QVariant.
*/
QOpcUa::QRange::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QOpcUa::QRange::~QRange()
{
}

/*!
    Returns the high value of the range.
*/
double QOpcUa::QRange::high() const
{
    return data->high;
}

/*!
    Sets the high value of the range to \a value.
*/
void QOpcUa::QRange::setHigh(double value)
{
    data->high = value;
}

/*!
    Returns the low value of the range.
*/
double QOpcUa::QRange::low() const
{
    return data->low;
}

/*!
    Sets the low value of the range to \a value.
*/
void QOpcUa::QRange::setLow(double value)
{
    data->low = value;
}

/*!
    \class QOpcUa::QEUInformation
    \inmodule QtOpcUa
    \brief The OPC UA EURange type

    This is the Qt OPC UA representation for the OPC UA EUInformation type defined in OPC-UA part 8, 5.6.3.
    EUInformation values contain information about units and are mostly used as property of a node with a numeric value attribute.
    The information can e. g. be used to add text and tooltips to GUI elements.
*/

class QOpcUa::QEUInformationData : public QSharedData
{
public:
    QString namespaceUri;
    qint32 unitId{0};
    QOpcUa::QLocalizedText displayName;
    QOpcUa::QLocalizedText description;
};

QOpcUa::QEUInformation::QEUInformation()
    : data(new QOpcUa::QEUInformationData)
{
}

/*!
    Constructs a EUinformation from \a rhs.
*/
QOpcUa::QEUInformation::QEUInformation(const QOpcUa::QEUInformation &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this EUinformation.
*/
QOpcUa::QEUInformation &QOpcUa::QEUInformation::operator=(const QOpcUa::QEUInformation &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUa::QEUInformation::~QEUInformation()
{
}

/*!
    Returns the description of the unit, for example \e {degree Celsius}.
*/
QOpcUa::QLocalizedText QOpcUa::QEUInformation::description() const
{
    return data->description;
}

/*!
    Sets the description if the unit to \a description.
*/
void QOpcUa::QEUInformation::setDescription(const QOpcUa::QLocalizedText &description)
{
    data->description = description;
}

/*!
    Returns the display name of the unit, for example \e {°C}.
*/
QOpcUa::QLocalizedText QOpcUa::QEUInformation::displayName() const
{
    return data->displayName;
}

/*!
    Sets the display name of the unit to \a displayName.
*/
void QOpcUa::QEUInformation::setDisplayName(const QOpcUa::QLocalizedText &displayName)
{
    data->displayName = displayName;
}

/*!
    Returns the machine-readable identifier for the unit.
*/
qint32 QOpcUa::QEUInformation::unitId() const
{
    return data->unitId;
}

/*!
    Sets the machine-readable identifier for the unit to \a unitId.
*/
void QOpcUa::QEUInformation::setUnitId(qint32 unitId)
{
    data->unitId = unitId;
}

/*!
    Returns the namespace URI of the unit.
*/
QString QOpcUa::QEUInformation::namespaceUri() const
{
    return data->namespaceUri;
}

/*!
    Sets the namespace URI of the unit to \a namespaceUri.
*/
void QOpcUa::QEUInformation::setNamespaceUri(const QString &namespaceUri)
{
    data->namespaceUri = namespaceUri;
}

/*!
    Constructs a EUinformation with namespace URI \a namespaceUri, unit id \a unitId,
    display name \a displayName and description \a description.
*/
QOpcUa::QEUInformation::QEUInformation(const QString &namespaceUri, qint32 unitId, const QOpcUa::QLocalizedText &displayName,
                                       const QOpcUa::QLocalizedText &description)
    : data(new QOpcUa::QEUInformationData)
{
    data->namespaceUri = namespaceUri;
    data->unitId = unitId;
    data->displayName = displayName;
    data->description = description;
}

/*!
    Returns \c true if this EUinformation has the same value as \a rhs.
*/
bool QOpcUa::QEUInformation::operator==(const QOpcUa::QEUInformation &rhs) const
{
    return data->namespaceUri == rhs.namespaceUri() &&
            data->unitId == rhs.unitId() &&
            data->displayName == rhs.displayName() &&
            data->description == rhs.description();
}

/*!
    Converts this EUinformation to \l QVariant.
*/
QOpcUa::QEUInformation::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
    \class QOpcUa::QComplexNumber
    \inmodule QtOpcUa
    \brief The OPC UA ComplexNumber type

    The ComplexNumberType defined in OPC-UA part 8, 5.6.4.
    It stores a complex number with float precision.
*/

class QOpcUa::QComplexNumberData : public QSharedData
{
public:
    float real{0};
    float imaginary{0};
};

QOpcUa::QComplexNumber::QComplexNumber()
    : data(new QOpcUa::QComplexNumberData)
{
}

QOpcUa::QComplexNumber::QComplexNumber(const QOpcUa::QComplexNumber &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this complex number.
*/
QOpcUa::QComplexNumber &QOpcUa::QComplexNumber::operator=(const QOpcUa::QComplexNumber &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUa::QComplexNumber::~QComplexNumber()
{

}

/*!
    Returns the imaginary part of the complex number.
*/
float QOpcUa::QComplexNumber::imaginary() const
{
    return data->imaginary;
}

/*!
    Sets the imaginary part of the complex number to \a value.
*/
void QOpcUa::QComplexNumber::setImaginary(float value)
{
    data->imaginary = value;
}

/*!
    Returns the real part of the complex number.
*/
float QOpcUa::QComplexNumber::real() const
{
    return data->real;
}

/*!
    Sets the real part of the complex number to \a value.
*/
void QOpcUa::QComplexNumber::setReal(float value)
{
    data->real = value;
}

/*!
    Constructs a complex number with real part \a real and imaginary part \a imaginary.
*/
QOpcUa::QComplexNumber::QComplexNumber(float real, float imaginary)
    : data(new QOpcUa::QComplexNumberData)
{
    data->real = real;
    data->imaginary = imaginary;
}

/*!
    Returns \c true if this complex number has the same value as \a rhs.
*/
bool QOpcUa::QComplexNumber::operator==(const QOpcUa::QComplexNumber &rhs) const
{
    return qFloatDistance(data->real, rhs.real()) == 0 &&
            qFloatDistance(data->imaginary, rhs.imaginary()) == 0;
}

/*!
    Converts this complex number to \l QVariant.
*/
QOpcUa::QComplexNumber::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
    \class QOpcUa::QDoubleComplexNumber
    \inmodule QtOpcUa
    \brief The OPC UA DoubleComplexNumber type

    The DoubleComplexNumberType defined in OPC-UA part 8, 5.6.5.
    It stores a complex number with double precision.
*/

class QOpcUa::QDoubleComplexNumberData : public QSharedData
{
public:
    double real{0};
    double imaginary{0};
};

QOpcUa::QDoubleComplexNumber::QDoubleComplexNumber()
    : data(new QOpcUa::QDoubleComplexNumberData)
{
}

QOpcUa::QDoubleComplexNumber::QDoubleComplexNumber(const QOpcUa::QDoubleComplexNumber &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this double complex number.
*/
QOpcUa::QDoubleComplexNumber &QOpcUa::QDoubleComplexNumber::operator=(const QOpcUa::QDoubleComplexNumber &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUa::QDoubleComplexNumber::~QDoubleComplexNumber()
{

}

/*!
    Returns the imaginary part of the complex number.
*/
double QOpcUa::QDoubleComplexNumber::imaginary() const
{
    return data->imaginary;
}

/*!
    Sets the imaginary part of the complex number to \a value.
*/
void QOpcUa::QDoubleComplexNumber::setImaginary(double value)
{
    data->imaginary = value;
}

/*!
    Returns the real part of the complex number.
*/
double QOpcUa::QDoubleComplexNumber::real() const
{
    return data->real;
}

/*!
    Sets the real part of the complex number to \a value.
*/
void QOpcUa::QDoubleComplexNumber::setReal(double value)
{
    data->real = value;
}

/*!
    Constructs a double complex number with real part \a real and imaginary part \a imaginary.
*/
QOpcUa::QDoubleComplexNumber::QDoubleComplexNumber(double real, double imaginary)
    : data(new QOpcUa::QDoubleComplexNumberData)
{
    data->real = real;
    data->imaginary = imaginary;
}

/*!
    Returns \c true if this double complex number has the same value as \a rhs.
*/
bool QOpcUa::QDoubleComplexNumber::operator==(const QOpcUa::QDoubleComplexNumber &rhs) const
{
    return qFloatDistance(data->real, rhs.real()) == 0 &&
            qFloatDistance(data->imaginary, rhs.imaginary()) == 0;
}

/*!
    Converts this double complex number to \l QVariant.
*/
QOpcUa::QDoubleComplexNumber::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
    \enum QOpcUa::AxisScale

    The AxisScale enum as defined by OPC-UA part 8, 5.6.7.

    \value Linear
    \value Log
    \value Ln
*/

/*!
    \class QOpcUa::QAxisInformation
    \inmodule QtOpcUa
    \brief The OPC UA AxisInformation type

    This is the Qt OPC UA representation for the OPC UA AxisInformation type defined in OPC-UA part 8, 5.6.6.
    It contains information about an axis which can be used for multiple purposes. A common use case could
    involve the plotting of display data. The engineering units and the title are used for the text on the plot,
    range, axisScaleType and axisSteps provide the scaling and the axis ranges of the plot.
*/

class QOpcUa::QAxisInformationData : public QSharedData
{
public:
    QOpcUa::QEUInformation engineeringUnits;
    QOpcUa::QRange eURange;
    QOpcUa::QLocalizedText title;
    QOpcUa::AxisScale axisScaleType{QOpcUa::AxisScale::Linear};
    QVector<double> axisSteps;
};

QOpcUa::QAxisInformation::QAxisInformation()
    : data(new QOpcUa::QAxisInformationData)
{
}

/*!
    Constructs axis information from \a rhs.
*/
QOpcUa::QAxisInformation::QAxisInformation(const QOpcUa::QAxisInformation &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this axis information.
*/
QOpcUa::QAxisInformation &QOpcUa::QAxisInformation::operator=(const QOpcUa::QAxisInformation &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUa::QAxisInformation::~QAxisInformation()
{
}

/*!
    Returns the lower and upper values of this axis.
*/
QOpcUa::QRange QOpcUa::QAxisInformation::eURange() const
{
    return data->eURange;
}

/*!
    Sets the lower and upper values of this axis to \a eURange.
*/
void QOpcUa::QAxisInformation::setEURange(const QOpcUa::QRange &eURange)
{
    data->eURange = eURange;
}

/*!
    Returns the title of this axis.
*/
QOpcUa::QLocalizedText QOpcUa::QAxisInformation::title() const
{
    return data->title;
}

/*!
    Sets the title to \a title.
*/
void QOpcUa::QAxisInformation::setTitle(const QOpcUa::QLocalizedText &title)
{
    data->title = title;
}

/*!
    Returns the scaling of this axis, defined by \l QOpcUa::AxisScale.
*/
QOpcUa::AxisScale QOpcUa::QAxisInformation::axisScaleType() const
{
    return data->axisScaleType;
}

/*!
    Sets the axis scale type to \a axisScaleType.
*/
void QOpcUa::QAxisInformation::setAxisScaleType(const QOpcUa::AxisScale &axisScaleType)
{
    data->axisScaleType = axisScaleType;
}

/*!
    Returns specific values for each axis step.

    This value is empty if the points are equally distributed and the step size can be
    calculated from the number of steps and the range.
    If the steps are different for each point but constant over a longer time, there is an entry for
    each data point.
*/
QVector<double> QOpcUa::QAxisInformation::axisSteps() const
{
    return data->axisSteps;
}

/*!
    Sets the axis steps to \a axisSteps.
*/
void QOpcUa::QAxisInformation::setAxisSteps(const QVector<double> &axisSteps)
{
    data->axisSteps = axisSteps;
}

/*!
    Returns a reference to the axis steps.
*/
QVector<double> &QOpcUa::QAxisInformation::axisStepsRef()
{
    return data->axisSteps;
}

/*!
    Returns the engineering units of this axis.
*/
QOpcUa::QEUInformation QOpcUa::QAxisInformation::engineeringUnits() const
{
    return data->engineeringUnits;
}

/*!
    Sets the engineering units to \a engineeringUnits.
*/
void QOpcUa::QAxisInformation::setEngineeringUnits(const QOpcUa::QEUInformation &engineeringUnits)
{
    data->engineeringUnits = engineeringUnits;
}

/*!
    Returns \c true if this axis information has the same value as \a rhs.
*/
bool QOpcUa::QAxisInformation::operator==(const QOpcUa::QAxisInformation &rhs) const
{
    return data->axisScaleType == rhs.axisScaleType() &&
            data->axisSteps == rhs.axisSteps() &&
            data->engineeringUnits == rhs.engineeringUnits() &&
            data->eURange == rhs.eURange() &&
            data->title == rhs.title();
}

/*!
    Converts this axis information to \l QVariant.
*/
QOpcUa::QAxisInformation::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
    Constructs axis information with engineering units \a engineeringUnits,
    range \a eURange, title \a title, scaling \a axisScaleType and axis steps \a axisSteps.
*/
QOpcUa::QAxisInformation::QAxisInformation(const QOpcUa::QEUInformation &engineeringUnits, const QOpcUa::QRange &eURange, const QOpcUa::QLocalizedText &title,
                                           const QOpcUa::AxisScale &axisScaleType, const QVector<double> &axisSteps)
    : data (new QOpcUa::QAxisInformationData)
{
    data->engineeringUnits = engineeringUnits;
    data->eURange == eURange;
    data->title = title;
    data->axisScaleType = axisScaleType;
    data->axisSteps = axisSteps;
}


/*!
    \class QOpcUa::QXValue
    \inmodule QtOpcUa
    \brief The OPC UA XVType

    This is the Qt OPC UA representation for the OPC UA XVType type defined in OPC-UA part 8, 5.6.8.
    This type is used to position values of float precision on an axis with double precision.
*/

class QOpcUa::QXValueData : public QSharedData
{
public:
    double x{0};
    float value{0};
};

QOpcUa::QXValue::QXValue()
    : data(new QOpcUa::QXValueData)
{
}

/*!
    Constructs an XValue from \a rhs.
*/
QOpcUa::QXValue::QXValue(const QOpcUa::QXValue &rhs)
    : data(rhs.data)
{
}

/*!
    Constructs an XValue with position \a x and value \a value.
*/
QOpcUa::QXValue::QXValue(double x, float value)
    : data(new QOpcUa::QXValueData)
{
    data->x = x;
    data->value = value;
}

/*!
    Sets the values from \a rhs in this XValue.
*/
QOpcUa::QXValue &QOpcUa::QXValue::operator=(const QOpcUa::QXValue &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this XValue has the same value as \a rhs.
*/
bool QOpcUa::QXValue::operator==(const QOpcUa::QXValue &rhs) const
{
    return qFloatDistance(data->x, rhs.x()) == 0 &&
            qFloatDistance(data->value, rhs.value()) == 0;
}

/*!
    Converts this XValue to \l QVariant.
*/
QOpcUa::QXValue::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QOpcUa::QXValue::~QXValue()
{
}

/*!
    Returns the value for position x.
*/
float QOpcUa::QXValue::value() const
{
    return data->value;
}

/*!
    Sets the value for position x to \a value.
*/
void QOpcUa::QXValue::setValue(float value)
{
    data->value = value;
}

/*!
    Returns the position of the value on the axis.
*/
double QOpcUa::QXValue::x() const
{
    return data->x;
}

/*!
    Sets the position of the value on the axis to \a x.
*/
void QOpcUa::QXValue::setX(double x)
{
    data->x = x;
}

QT_END_NAMESPACE

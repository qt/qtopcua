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

#include <QMetaEnum>
#include <QRegularExpression>
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
    \enum QOpcUa::WriteMaskBit

    This enum contains all possible bits for the WriteMask and UserWriteMask node attributes
    defined in OPC-UA part 3, Tabe 3.

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
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA QualifiedName type.

    This is the Qt OPC UA representation for the OPC UA QualifiedName type defined in OPC-UA part 3, 8.3.
    A QualifiedName is a name qualified by a namespace index. The namespace index corresponds to an entry in the server's namespace array.
    QualifiedName is mainly used to represent the BrowseName attribute of a node.
*/

class QOpcUa::QQualifiedNameData : public QSharedData
{
public:
    QString name;
    quint16 namespaceIndex{0}; //OPC UA part 4, page 116: a string is converted to a qualified name by setting the namespace index to 0.
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
    Sets the namespace index to \a namespaceIndex.
*/
void QOpcUa::QQualifiedName::setNamespaceIndex(quint16 namespaceIndex)
{
    data->namespaceIndex = namespaceIndex;
}

/*!
    Returns the name.
*/
QString QOpcUa::QQualifiedName::name() const
{
    return data->name;
}

/*!
    Sets the name to \a name.
*/
void QOpcUa::QQualifiedName::setName(const QString &name)
{
    data->name = name;
}

/*!
    \class QOpcUa::QLocalizedText
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA LocalizedText type.

    This is the Qt OPC UA representation for the OPC UA LocalizedText type defined in OPC-UA part 3, 8.5.
    A LocalizedText value contains a text string with associated locale information in a second string (e. g. "en" or "en-US").
    The format of the locale information string is <language>[-<country/region>]. Language is usually given as ISO 639 two letter code,
    country/region as ISO 3166 two letter code. Custom codes are also allowed (see OPC-UA part 3, 8.4).
    It can be used to provide multiple text strings in different languages for a value using an array of LocalizedText elements.
*/

/*!
    \qmltype LocalizedText
    \inqmlmodule QtOpcUa
    \brief Contains a text with associated locale.
    \since QtOpcUa 5.12

    The two members of this type contain the actual text and the locale of the text.
*/

/*!
    \qmlproperty string LocalizedText::text

    Textual content.
*/

/*!
    \qmlproperty string LocalizedText::locale

    Locale of the contained text.
    This has to be in a modified ISO standard notation, for example \c en-US.
    See OPC UA specification part 3, 8.4 for details.
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
    Sets the text to \a text.

*/
void QOpcUa::QLocalizedText::setText(const QString &text)
{
    data->text = text;
}

/*!
    Returns the locale.
*/
QString QOpcUa::QLocalizedText::locale() const
{
    return data->locale;
}

/*!
    Sets the locale to \a locale.
*/
void QOpcUa::QLocalizedText::setLocale(const QString &locale)
{
    data->locale = locale;
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
        uint ns = components.at(0).midRef(3).toString().toUInt(&success);
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
        *identifier = components.last().midRef(2).toString();
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
    if (first.startsWith(QLatin1String("ns=0;")) && !second.startsWith(QLatin1String("ns=")))
        return first.midRef(5) == second;
    else if (second.startsWith(QLatin1String("ns=0;")) && !first.startsWith(QLatin1String("ns=")))
        return second.midRef(5) == first;
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

    QStringRef sr = nodeId.midRef(7);

    bool ok = false;
    quint32 identifier = sr.toUInt(&ok);

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
    \class QOpcUa::QRange
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA Range type.

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
    Sets the high value of the range to \a high.
*/
void QOpcUa::QRange::setHigh(double high)
{
    data->high = high;
}

/*!
    Returns the low value of the range.
*/
double QOpcUa::QRange::low() const
{
    return data->low;
}

/*!
    Sets the low value of the range to \a low.
*/
void QOpcUa::QRange::setLow(double low)
{
    data->low = low;
}

/*!
    \class QOpcUa::QEUInformation
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA EURange type.

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
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA ComplexNumber type.

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
    Sets the imaginary part of the complex number to \a imaginary.
*/
void QOpcUa::QComplexNumber::setImaginary(float imaginary)
{
    data->imaginary = imaginary;
}

/*!
    Returns the real part of the complex number.
*/
float QOpcUa::QComplexNumber::real() const
{
    return data->real;
}

/*!
    Sets the real part of the complex number to \a real.
*/
void QOpcUa::QComplexNumber::setReal(float real)
{
    data->real = real;
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
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA DoubleComplexNumber type.

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
    Sets the imaginary part of the complex number to \a imaginary.
*/
void QOpcUa::QDoubleComplexNumber::setImaginary(double imaginary)
{
    data->imaginary = imaginary;
}

/*!
    Returns the real part of the complex number.
*/
double QOpcUa::QDoubleComplexNumber::real() const
{
    return data->real;
}

/*!
    Sets the real part of the complex number to \a real.
*/
void QOpcUa::QDoubleComplexNumber::setReal(double real)
{
    data->real = real;
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
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA AxisInformation type.

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
void QOpcUa::QAxisInformation::setAxisScaleType(QOpcUa::AxisScale axisScaleType)
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
    data->eURange = eURange;
    data->title = title;
    data->axisScaleType = axisScaleType;
    data->axisSteps = axisSteps;
}


/*!
    \class QOpcUa::QXValue
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA XVType.

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

/*!
    \class QOpcUa::QExpandedNodeId
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA ExpandedNodeId.

    An expanded node id is a node id where the namespace index can be given as index or as a string URI.
    A list of namespaces and their indices on the server is provided by \l QOpcUaClient::namespaceArray().
*/

class QOpcUa::QExpandedNodeIdData : public QSharedData
{
public:
    quint32 serverIndex{0};
    QString namespaceUri;
    QString nodeId;
};

QOpcUa::QExpandedNodeId::QExpandedNodeId()
    : data(new QOpcUa::QExpandedNodeIdData)
{
}

/*!
    Constructs an expanded node id from \a rhs.
*/
QOpcUa::QExpandedNodeId::QExpandedNodeId(const QOpcUa::QExpandedNodeId &rhs)
    : data(rhs.data)
{
}

/*!
    Constructs an expanded node id from node id string \a nodeId.
*/
QOpcUa::QExpandedNodeId::QExpandedNodeId(const QString &nodeId)
    : data(new QOpcUa::QExpandedNodeIdData)
{
    data->nodeId = nodeId;
}

/*!
    Constructs an expanded node id from namespace URI \a namespaceUri, node id string \a nodeId
    and server index \a serverIndex.
*/
QOpcUa::QExpandedNodeId::QExpandedNodeId(const QString &namespaceUri, const QString &nodeId, quint32 serverIndex)
    : data(new QOpcUa::QExpandedNodeIdData)
{
    data->namespaceUri = namespaceUri;
    data->nodeId = nodeId;
    data->serverIndex = serverIndex;
}

/*!
    Sets the values from \a rhs in this expanded node id.
*/
QOpcUa::QExpandedNodeId &QOpcUa::QExpandedNodeId::operator=(const QOpcUa::QExpandedNodeId &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this expanded node id has the same value as \a rhs.
*/
bool QOpcUa::QExpandedNodeId::operator==(const QOpcUa::QExpandedNodeId &rhs) const
{
    return data->namespaceUri == rhs.namespaceUri() &&
            nodeIdEquals(data->nodeId, rhs.nodeId()) &&
            data->serverIndex == rhs.serverIndex();
}

/*!
    Converts this expanded node id to \l QVariant.
*/
QOpcUa::QExpandedNodeId::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QOpcUa::QExpandedNodeId::~QExpandedNodeId()
{
}

/*!
    Returns the index of the server containing the node. This index maps to an entry in the server's server table.
*/
quint32 QOpcUa::QExpandedNodeId::serverIndex() const
{
    return data->serverIndex;
}

/*!
    Sets the server index to \a serverIndex.
*/
void QOpcUa::QExpandedNodeId::setServerIndex(quint32 serverIndex)
{
    data->serverIndex = serverIndex;
}

/*!
    Returns the namespace URI of the node id. If this value is specified, the namespace index in
    \l {QOpcUa::QExpandedNodeId::nodeId} {nodeId} is 0 and must be ignored.
*/
QString QOpcUa::QExpandedNodeId::namespaceUri() const
{
    return data->namespaceUri;
}

/*!
    Sets the namespace URI to \a namespaceUri.
*/
void QOpcUa::QExpandedNodeId::setNamespaceUri(const QString &namespaceUri)
{
    data->namespaceUri = namespaceUri;
}

/*!
    Returns the node id. If \l {QOpcUa::QExpandedNodeId::namespaceUri} {namespaceUri} is specified, the namespace index is invalid.
*/
QString QOpcUa::QExpandedNodeId::nodeId() const
{
    return data->nodeId;
}

/*!
    Sets the node id to \a nodeId.
*/
void QOpcUa::QExpandedNodeId::setNodeId(const QString &nodeId)
{
    data->nodeId = nodeId;
}

/*!
    \class QOpcUa::QRelativePathElement
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA RelativePathElement.

    QRelativePathElement defines an element of a relative path on an OPC UA server.
    This is needed for resolution of browse paths to node ids in \l QOpcUaNode::resolveBrowsePath().
*/

class QOpcUa::QRelativePathElementData : public QSharedData
{
public:
    QString referenceTypeId;
    bool isInverse{false};
    bool includeSubtypes{false};
    QOpcUa::QQualifiedName targetName;
};

/*!
    Constructs a relative path element with both flags set to \c false.
*/
QOpcUa::QRelativePathElement::QRelativePathElement()
    : data(new QOpcUa::QRelativePathElementData())
{
}

/*!
    Constructs a relative path element with targetName \a target, reference type node id \a refType and both flags set to \c false.
*/
QOpcUa::QRelativePathElement::QRelativePathElement(const QOpcUa::QQualifiedName &target, const QString &refType)
    : data(new QOpcUa::QRelativePathElementData())
{
    data->referenceTypeId = refType;
    data->targetName = target;
}

/*!
    Constructs a relative path element with targetName \a target, \l QOpcUa::ReferenceTypeId \a refType and both flags set to \c false.
*/
QOpcUa::QRelativePathElement::QRelativePathElement(const QOpcUa::QQualifiedName &target, QOpcUa::ReferenceTypeId refType)
    : data(new QOpcUa::QRelativePathElementData())
{
    data->referenceTypeId = QOpcUa::nodeIdFromReferenceType(refType);
    data->targetName = target;
}

/*!
    Constructs a relative path element from \a rhs.
*/
QOpcUa::QRelativePathElement::QRelativePathElement(const QOpcUa::QRelativePathElement &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values of \a rhs in this relative path element.
*/
QOpcUa::QRelativePathElement &QOpcUa::QRelativePathElement::operator=(const QOpcUa::QRelativePathElement &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this relative path element has the same value as \a rhs.
*/
bool QOpcUa::QRelativePathElement::operator==(const QOpcUa::QRelativePathElement &rhs) const
{
    return data->includeSubtypes == rhs.includeSubtypes() &&
            data->isInverse == rhs.isInverse() &&
            data->referenceTypeId == rhs.referenceTypeId() &&
            data->targetName == rhs.targetName();
}

QOpcUa::QRelativePathElement::~QRelativePathElement()
{
}

/*!
    Returns the qualified name of the reference's target.
*/
QOpcUa::QQualifiedName QOpcUa::QRelativePathElement::targetName() const
{
    return data->targetName;
}

/*!
    Sets the target name to \a targetName, for example QOpcUa::QQualifiedName(0, "DataTypes").
*/
void QOpcUa::QRelativePathElement::setTargetName(const QOpcUa::QQualifiedName &targetName)
{
    data->targetName = targetName;
}

/*!
    Returns the value of the includeSubtypes flag.
*/
bool QOpcUa::QRelativePathElement::includeSubtypes() const
{
    return data->includeSubtypes;
}

/*!
    Sets the includeSubtypes flag to \a includeSubtypes.
    If the flag is \c true, the lookup also follows references with subtypes of \l referenceTypeId().
*/
void QOpcUa::QRelativePathElement::setIncludeSubtypes(bool includeSubtypes)
{
    data->includeSubtypes = includeSubtypes;
}

/*!
    Returns the value of the isInverse flag.
*/
bool QOpcUa::QRelativePathElement::isInverse() const
{
    return data->isInverse;
}

/*!
    Sets the isInverse flag to \a isInverse.
    If the flag is \c true, the lookup follows the reverse reference.
*/
void QOpcUa::QRelativePathElement::setIsInverse(bool isInverse)
{
    data->isInverse = isInverse;
}

/*!
    Returns the type id of the reference connecting this node to the previous node.
*/
QString QOpcUa::QRelativePathElement::referenceTypeId() const
{
    return data->referenceTypeId;
}

/*!
    Sets the reference type id to \a referenceTypeId.
*/
void QOpcUa::QRelativePathElement::setReferenceTypeId(const QString &referenceTypeId)
{
    data->referenceTypeId = referenceTypeId;
}

/*!
    Sets the reference type id to \a referenceTypeId.
*/
void QOpcUa::QRelativePathElement::setReferenceTypeId(QOpcUa::ReferenceTypeId referenceTypeId)
{
    data->referenceTypeId = QOpcUa::nodeIdFromReferenceType(referenceTypeId);
}

/*!
    \class QOpcUa::QBrowsePathTarget
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA BrowsePathTarget.

    A BrowsePathTarget contains a target of a browse path and information about the completeness of the node id resolution.
*/
class QOpcUa::QBrowsePathTargetData : public QSharedData
{
public:
    QOpcUa::QExpandedNodeId targetId;
    quint32 remainingPathIndex{(std::numeric_limits<quint32>::max)()};
};

QOpcUa::QBrowsePathTarget::QBrowsePathTarget()
    : data(new QOpcUa::QBrowsePathTargetData)
{
}

/*!
    Constructs a browse path target from \a rhs.
*/
QOpcUa::QBrowsePathTarget::QBrowsePathTarget(const QOpcUa::QBrowsePathTarget &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values of \a rhs in this browse path target.
*/
QOpcUa::QBrowsePathTarget &QOpcUa::QBrowsePathTarget::operator=(const QOpcUa::QBrowsePathTarget &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this browse path target has the same value as \a rhs.
*/
bool QOpcUa::QBrowsePathTarget::operator==(const QOpcUa::QBrowsePathTarget &rhs) const
{
    return data->targetId == rhs.targetId() &&
            data->remainingPathIndex == rhs.remainingPathIndex();
}

QOpcUa::QBrowsePathTarget::~QBrowsePathTarget()
{
}

/*!
    Returns the index of the first unprocessed element in the browse path.
    If the path was followed to the end, remainingPathIndex has the maximum value of quint32.
    \sa QOpcUa::QBrowsePathTarget::targetId()
*/
quint32 QOpcUa::QBrowsePathTarget::remainingPathIndex() const
{
    return data->remainingPathIndex;
}

/*!
    Sets the remaining path index to \a remainingPathIndex.
*/
void QOpcUa::QBrowsePathTarget::setRemainingPathIndex(quint32 remainingPathIndex)
{
    data->remainingPathIndex = remainingPathIndex;
}

/*!
    Returns \c true if the browse path has been fully resolved.
*/
bool QOpcUa::QBrowsePathTarget::isFullyResolved() const
{
    return (data->remainingPathIndex == (std::numeric_limits<quint32>::max)());
}

/*!
    Returns the target of the last reference the server was able to follow.
    If the reference leads to an external server, \e targetId is the id of the first node on that server.
    \sa QOpcUa::QBrowsePathTarget::remainingPathIndex
*/
QOpcUa::QExpandedNodeId QOpcUa::QBrowsePathTarget::targetId() const
{
    return data->targetId;
}

/*!
    Returns a reference to the target id.
*/
QOpcUa::QExpandedNodeId &QOpcUa::QBrowsePathTarget::targetIdRef()
{
    return data->targetId;
}

/*!
    Sets the node id of the target node to \a targetId.
*/
void QOpcUa::QBrowsePathTarget::setTargetId(const QOpcUa::QExpandedNodeId &targetId)
{
    data->targetId = targetId;
}

/*!
    \class QOpcUa::QContentFilterElement
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA ContentFilterElement.

    A content filter element contains an operator and a list of operands.
    There are four different operator types which contain literal values, references to
    attributes of nodes or to other content filter elements.

    A combination of one or more content filter elements makes a content filter which is used
    by the server to filter data for the criteria defined by the content filter elements.
    For example, the \c where clause of an event filter is a content filter which is used to decide
    if a notification is generated for an event.
*/

/*!
    \enum QContentFilterElement::FilterOperator

    FilterOperator enumerates all possible operators for a ContentFilterElement that are specified in
    OPC-UA part 4, Tables 115 and 116.

    \value Equals
    \value IsNull
    \value GreaterThan
    \value LessThan
    \value GreaterThanOrEqual
    \value LessThanOrEqual
    \value Like
    \value Not
    \value Between
    \value InList
    \value And
    \value Or
    \value Cast
    \value InView
    \value OfType
    \value RelatedTo
    \value BitwiseAnd
    \value BitwiseOr
*/

class QOpcUa::QContentFilterElementData : public QSharedData
{
public:
    QOpcUa::QContentFilterElement::FilterOperator filterOperator;
    QVector<QVariant> filterOperands;
};

QOpcUa::QContentFilterElement::QContentFilterElement()
    : data(new QContentFilterElementData)
{
}

/*!
    Constructs a content filter element from \a rhs.
*/
QOpcUa::QContentFilterElement::QContentFilterElement(const QOpcUa::QContentFilterElement &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this content filter element.
*/
QOpcUa::QContentFilterElement &QOpcUa::QContentFilterElement::operator=(const QOpcUa::QContentFilterElement &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this content filter element has the same value as \a rhs.
*/
bool QOpcUa::QContentFilterElement::operator==(const QOpcUa::QContentFilterElement &rhs) const
{
    return filterOperator() == rhs.filterOperator() && filterOperands() == rhs.filterOperands();
}

/*!
    Converts this content filter element to \l QVariant.
*/
QOpcUa::QContentFilterElement::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
    Returns the operands of the filter element.
*/
QVector<QVariant> QOpcUa::QContentFilterElement::filterOperands() const
{
    return data->filterOperands;
}

/*!
    Returns a reference to the filter operands.

    \sa filterOperands()
*/
QVector<QVariant> &QOpcUa::QContentFilterElement::filterOperandsRef()
{
   return data->filterOperands;
}

/*!
    Sets the filter operands for this content filter element to \a filterOperands.
    Supported classes are \l QOpcUa::QElementOperand, \l QOpcUa::QLiteralOperand,
    \l QOpcUa::QSimpleAttributeOperand and \l QOpcUa::QAttributeOperand.
*/
void QOpcUa::QContentFilterElement::setFilterOperands(const QVector<QVariant> &filterOperands)
{
    data->filterOperands = filterOperands;
}

/*!
    Returns the filter operator.
*/
QOpcUa::QContentFilterElement::FilterOperator QOpcUa::QContentFilterElement::filterOperator() const
{
    return data->filterOperator;
}

/*!
    Sets the operator that is applied to the filter operands to \a filterOperator.
*/
void QOpcUa::QContentFilterElement::setFilterOperator(QOpcUa::QContentFilterElement::FilterOperator filterOperator)
{
    data->filterOperator = filterOperator;
}

QOpcUa::QContentFilterElement::~QContentFilterElement()
{
}

/*!
    Sets filter operator \a op in this content filter element.
    If multiple operators are streamed into one content filter element, only the last operator is used.
    All others are discarded.
*/
QOpcUa::QContentFilterElement &QOpcUa::QContentFilterElement::operator<<(QOpcUa::QContentFilterElement::FilterOperator op)
{
    setFilterOperator(op);
    return *this;
}

/*!
    Adds the simple attribute operand \a op to the operands list of this content filter element.
*/
QOpcUa::QContentFilterElement &QOpcUa::QContentFilterElement::operator<<(const QOpcUa::QSimpleAttributeOperand &op)
{
    filterOperandsRef().append(op);
    return *this;
}

/*!
    Adds the attribute operand \a op to the operands list of this content filter element.
*/
QOpcUa::QContentFilterElement &QOpcUa::QContentFilterElement::operator<<(const QOpcUa::QAttributeOperand &op)
{
    filterOperandsRef().append(op);
    return *this;
}

/*!
    Adds the literal operand \a op to the operands list of this content filter element.
*/
QOpcUa::QContentFilterElement &QOpcUa::QContentFilterElement::operator<<(const QOpcUa::QLiteralOperand &op)
{
    filterOperandsRef().append(op);
    return *this;
}

/*!
    Adds the element operand \a op to the operands list of this content filter element.
*/
QOpcUa::QContentFilterElement &QOpcUa::QContentFilterElement::operator<<(const QOpcUa::QElementOperand &op)
{
    filterOperandsRef().append(op);
    return *this;
}

/*!
    \class QOpcUa::QElementOperand
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA ElementOperand type.

    The ElementOperand is defined in OPC-UA part 4, 7.4.4.2.
    It is used to identify another element in the filter by its index
    (the first element has the index 0).

    This is required to create complex filters, for example to reference
    the two operands of the AND operation in ((Severity > 500) AND (Message == "TestString")).
    The first step is to create content filter elements for the two conditions (Severity > 500)
    and (Message == "TestString"). A third content filter element is required to create an AND
    combination of the two conditions. It consists of the AND operator and two element operands
    with the indices of the two conditions created before:

    \code
    QOpcUaMonitoringParameters::EventFilter filter;
    ...
    // setup select clauses
    ...
    QOpcUa::QContentFilterElement condition1;
    QOpcUa::QContentFilterElement condition2;
    QOpcUa::QContentFilterElement condition3;
    condition1 << QOpcUa::QContentFilterElement::FilterOperator::GreaterThan << QOpcUa::QSimpleAttributeOperand("Severity") <<
                    QOpcUa::QLiteralOperand(quint16(500), QOpcUa::Types::UInt16);
    condition2 << QOpcUa::QContentFilterElement::FilterOperator::Equals << QOpcUa::QSimpleAttributeOperand("Message") <<
                    QOpcUa::QLiteralOperand("TestString", QOpcUa::Types::String);
    condition3 << QOpcUa::QContentFilterElement::FilterOperator::And << QOpcUa::QElementOperand(0) << QOpcUa::QElementOperand(1);
    filter << condition1 << condition2 << condition3;
    \endcode
*/

class QOpcUa::QElementOperandData : public QSharedData
{
public:
    quint32 index {0};
};

QOpcUa::QElementOperand::QElementOperand()
    : data(new QOpcUa::QElementOperandData)
{
}

/*!
    Constructs an element operand from \a rhs.
*/
QOpcUa::QElementOperand::QElementOperand(const QOpcUa::QElementOperand &rhs)
    : data(rhs.data)
{
}

/*!
    Constructs an element operand with index \a index.
*/
QOpcUa::QElementOperand::QElementOperand(quint32 index)
    : data(new QOpcUa::QElementOperandData)
{
    setIndex(index);
}

/*!
    Sets the values from \a rhs in this element operand.
*/
QOpcUa::QElementOperand &QOpcUa::QElementOperand::operator=(const QOpcUa::QElementOperand &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Converts this element operand to \l QVariant.
*/
QOpcUa::QElementOperand::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QOpcUa::QElementOperand::~QElementOperand()
{
}

/*!
    Returns the index of the filter element that is going to be used as operand.
*/
quint32 QOpcUa::QElementOperand::index() const
{
    return data->index;
}

/*!
    Sets the index of the filter element that is going to be used as operand to \a index.
*/
void QOpcUa::QElementOperand::setIndex(quint32 index)
{
    data->index = index;
}

/*!
    \class QOpcUa::QLiteralOperand
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA LiteralOperand type.

    The LiteralOperand is defined in OPC-UA part 4, 7.4.4.3.
    It contains a literal value that is to be used as operand.
*/
class QOpcUa::QLiteralOperandData : public QSharedData
{
public:
    QVariant value;
    QOpcUa::Types type {QOpcUa::Types::Undefined};
};

QOpcUa::QLiteralOperand::QLiteralOperand()
    : data(new QOpcUa::QLiteralOperandData)
{
    data->type = QOpcUa::Types::Undefined;
}

/*!
    Constructs a literal operand from \a rhs.
*/
QOpcUa::QLiteralOperand::QLiteralOperand(const QLiteralOperand &rhs)
    : data(rhs.data)
{
}

/*!
    Constructs a literal operand of value \a value and type \a type.
*/
QOpcUa::QLiteralOperand::QLiteralOperand(const QVariant &value, QOpcUa::Types type)
    : data(new QOpcUa::QLiteralOperandData)
{
    setValue(value);
    setType(type);
}

/*!
    Sets the values from \a rhs in this \l QLiteralOperand.
*/
QOpcUa::QLiteralOperand &QOpcUa::QLiteralOperand::operator=(const QOpcUa::QLiteralOperand &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Converts this literal operand to \l QVariant.
*/
QOpcUa::QLiteralOperand::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QOpcUa::QLiteralOperand::~QLiteralOperand()
{
}

/*!
    Returns the type of the value of the literal operand.
*/
QOpcUa::Types QOpcUa::QLiteralOperand::type() const
{
    return data->type;
}

/*!
    Sets the type of the value of the literal operand to \a type.
*/
void QOpcUa::QLiteralOperand::setType(QOpcUa::Types type)
{
    data->type = type;
}

/*!
    Returns the value of the literal operand.
*/
QVariant QOpcUa::QLiteralOperand::value() const
{
    return data->value;
}

/*!
    Sets the value of the literal operand to \a value.
*/
void QOpcUa::QLiteralOperand::setValue(const QVariant &value)
{
    data->value = value;
}

/*!
    \class QOpcUa::QSimpleAttributeOperand
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA SimpleAttributeOperand type.

    The SimpleAttributeOperand is specified in OPC-UA part 4, 7.4.4.5.
    It is used when a node attribute is required as operand.

    For example, the following simple attribute operand represents the value
    of the "Severity" field of the base event type:
    \code
        QOpcUa::QSimpleAttributeOperand("Severity");
    \endcode
*/
class QOpcUa::QSimpleAttributeOperandData : public QSharedData
{
public:
    QString typeId{QStringLiteral("ns=0;i=2041")}; // BaseEventType
    QVector<QOpcUa::QQualifiedName> browsePath;
    QOpcUa::NodeAttribute attributeId {QOpcUa::NodeAttribute::Value};
    QString indexRange;
};

QOpcUa::QSimpleAttributeOperand::QSimpleAttributeOperand()
    : data(new QSimpleAttributeOperandData)
{
}

/*!
    Constructs a simple attribute operand from \a rhs.
*/
QOpcUa::QSimpleAttributeOperand::QSimpleAttributeOperand(const QOpcUa::QSimpleAttributeOperand &rhs)
    : data(rhs.data)
{
}

/*!
    Constructs a simple attribute operand for attribute \a attributeId of the direct child with the browse name
    \a name in namespace \a namespaceIndex. \a typeId is the node id of a type definition node. The operand will
    be restricted to instances of type \a typeId or a subtype.
*/
QOpcUa::QSimpleAttributeOperand::QSimpleAttributeOperand(const QString &name, quint16 namespaceIndex, const QString &typeId, QOpcUa::NodeAttribute attributeId)
    : data(new QOpcUa::QSimpleAttributeOperandData)
{
    browsePathRef().append(QOpcUa::QQualifiedName(namespaceIndex, name));
    setTypeId(typeId);
    setAttributeId(attributeId);
}

/*!
    Constructs a simple attribute operand for the attribute \a attributeId of an object or variable of type \a typeId.
    This can be used for requesting the ConditionId in an event filter as described in OPC-UA part 9, Table 8.
*/
QOpcUa::QSimpleAttributeOperand::QSimpleAttributeOperand(QOpcUa::NodeAttribute attributeId, const QString &typeId)
    : data(new QOpcUa::QSimpleAttributeOperandData)
{
    setTypeId(typeId);
    setAttributeId(attributeId);
}

/*!
    Sets the values from \a rhs in this simple attribute operand.
*/
QOpcUa::QSimpleAttributeOperand &QOpcUa::QSimpleAttributeOperand::operator=(const QOpcUa::QSimpleAttributeOperand &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this simple attribute operand has the same value as \a rhs.
*/
bool QOpcUa::QSimpleAttributeOperand::operator==(const QOpcUa::QSimpleAttributeOperand &rhs) const
{
    return attributeId() == rhs.attributeId() && browsePath() == rhs.browsePath() &&
            indexRange() == rhs.indexRange() && typeId() == rhs.typeId();
}

/*!
    Converts this simple attribute operand to \l QVariant.
*/
QOpcUa::QSimpleAttributeOperand::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QOpcUa::QSimpleAttributeOperand::~QSimpleAttributeOperand()
{
}

/*!
    Returns the index range string.
*/
QString QOpcUa::QSimpleAttributeOperand::indexRange() const
{
    return data->indexRange;
}

/*!
    Sets the index range string used to identify a single value or subset of the attribute's value to \a indexRange.
*/
void QOpcUa::QSimpleAttributeOperand::setIndexRange(const QString &indexRange)
{
    data->indexRange = indexRange;
}

/*!
    Returns the attribute of the node \l browsePath is pointing to.
*/
QOpcUa::NodeAttribute QOpcUa::QSimpleAttributeOperand::attributeId() const
{
    return data->attributeId;
}

/*!
    Sets the attribute id to \a attributeId.
*/
void QOpcUa::QSimpleAttributeOperand::setAttributeId(QOpcUa::NodeAttribute attributeId)
{
    data->attributeId = attributeId;
}

/*!
    Returns the relative path to a node starting from \l typeId.
*/
QVector<QOpcUa::QQualifiedName> QOpcUa::QSimpleAttributeOperand::browsePath() const
{
    return data->browsePath;
}

/*!
    Returns a reference to the browse path.

    \sa browsePath()
*/
QVector<QOpcUa::QQualifiedName> &QOpcUa::QSimpleAttributeOperand::browsePathRef()
{
    return data->browsePath;
}

/*!
    Sets the browse path to the node holding the attribute to \a browsePath.
*/
void QOpcUa::QSimpleAttributeOperand::setBrowsePath(const QVector<QOpcUa::QQualifiedName> &browsePath)
{
    data->browsePath = browsePath;
}

/*!
    Returns the node id of a type definition node.
*/
QString QOpcUa::QSimpleAttributeOperand::typeId() const
{
    return data->typeId;
}

/*!
    Sets the node id of the type definition node to \a typeId. The operand will be of the type or one of its subtypes.
*/
void QOpcUa::QSimpleAttributeOperand::setTypeId(const QString &typeId)
{
    data->typeId = typeId;
}

/*!
    \class QOpcUa::QAttributeOperand
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA AttributeOperand type.

    The AttributeOperand is defined in OPC-UA part 4, 7.4.4.4.
    It has the same purpose as \l QSimpleAttributeOperand but has more configurable options.
*/

class QOpcUa::QAttributeOperandData : public QSharedData
{
public:
    QString nodeId;
    QString alias;
    QVector<QOpcUa::QRelativePathElement> browsePath;
    QOpcUa::NodeAttribute attributeId {QOpcUa::NodeAttribute::Value};
    QString indexRange;
};

QOpcUa::QAttributeOperand::QAttributeOperand()
    : data(new QAttributeOperandData)
{
}

/*!
    Constructs an attribute operand from \a rhs.
*/
QOpcUa::QAttributeOperand::QAttributeOperand(const QOpcUa::QAttributeOperand &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this attribute operand.
*/
QOpcUa::QAttributeOperand &QOpcUa::QAttributeOperand::operator=(const QOpcUa::QAttributeOperand &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Converts this attribute operand to \l QVariant.
*/
QOpcUa::QAttributeOperand::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QOpcUa::QAttributeOperand::~QAttributeOperand()
{
}

/*!
    Returns the index range string.
*/
QString QOpcUa::QAttributeOperand::indexRange() const
{
    return data->indexRange;
}

/*!
    Sets the index range string used to identify a single value or subset of the attribute's value to \a indexRange.
*/
void QOpcUa::QAttributeOperand::setIndexRange(const QString &indexRange)
{
    data->indexRange = indexRange;
}

/*!
    Returns the attribute id for an attribute of the node \l browsePath is pointing to.
*/
QOpcUa::NodeAttribute QOpcUa::QAttributeOperand::attributeId() const
{
    return data->attributeId;
}

/*!
    Sets the attribute id to \a attributeId.
*/
void QOpcUa::QAttributeOperand::setAttributeId(QOpcUa::NodeAttribute attributeId)
{
    data->attributeId = attributeId;
}

/*!
    Returns the browse path.
*/
QVector<QOpcUa::QRelativePathElement> QOpcUa::QAttributeOperand::browsePath() const
{
    return data->browsePath;
}

/*!
    Returns a reference to the browse path.

    \sa browsePath()
*/
QVector<QOpcUa::QRelativePathElement> &QOpcUa::QAttributeOperand::browsePathRef()
{
    return data->browsePath;
}

/*!
    Sets the relative path to a node starting from \l nodeId() to \a browsePath.
*/
void QOpcUa::QAttributeOperand::setBrowsePath(const QVector<QOpcUa::QRelativePathElement> &browsePath)
{
    data->browsePath = browsePath;
}

/*!
    Returns the alias for this QAttributeOperand.
*/
QString QOpcUa::QAttributeOperand::alias() const
{
    return data->alias;
}

/*!
    Sets the alias to \a alias. This allows using this instance
    as operand for other operations in the filter.
*/
void QOpcUa::QAttributeOperand::setAlias(const QString &alias)
{
    data->alias = alias;
}

/*!
    Returns the node id of the type definition node.
*/
QString QOpcUa::QAttributeOperand::nodeId() const
{
    return data->nodeId;
}

/*!
    Sets the node id of the type definition node to \a nodeId.
*/
void QOpcUa::QAttributeOperand::setNodeId(const QString &nodeId)
{
    data->nodeId = nodeId;
}

/*!
    \class QOpcUa::QContentFilterElementResult
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA ContentFilterElementResult.

    QContentFilterElementResult contains the status code for a
    filter element and all its operands.
*/

class QOpcUa::QContentFilterElementResultData : public QSharedData
{
public:
    QOpcUa::UaStatusCode statusCode {QOpcUa::UaStatusCode::Good};
    QVector<QOpcUa::UaStatusCode> operandStatusCodes;
};

QOpcUa::QContentFilterElementResult::QContentFilterElementResult()
    : data(new QOpcUa::QContentFilterElementResultData)
{
}

/*!
    Constructs a content filter element result from \a rhs.
*/
QOpcUa::QContentFilterElementResult::QContentFilterElementResult(const QOpcUa::QContentFilterElementResult &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this content filter element result.
*/
QOpcUa::QContentFilterElementResult &QOpcUa::QContentFilterElementResult::operator=(const QOpcUa::QContentFilterElementResult &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUa::QContentFilterElementResult::~QContentFilterElementResult()
{
}

/*!
    Returns the status code for the filter element.
*/
QOpcUa::UaStatusCode QOpcUa::QContentFilterElementResult::statusCode() const
{
    return data->statusCode;
}

/*!
    Sets the status code for the filter element to \a statusCode.
*/
void QOpcUa::QContentFilterElementResult::setStatusCode(QOpcUa::UaStatusCode statusCode)
{
    data->statusCode = statusCode;
}

/*!
    Returns the status codes for all filter operands in the order that was used in the filter.
*/
QVector<QOpcUa::UaStatusCode> QOpcUa::QContentFilterElementResult::operandStatusCodes() const
{
    return data->operandStatusCodes;
}

/*!
    Sets the status codes for all filter operands to \a operandStatusCodes.
*/
void QOpcUa::QContentFilterElementResult::setOperandStatusCodes(const QVector<QOpcUa::UaStatusCode> &operandStatusCodes)
{
    data->operandStatusCodes = operandStatusCodes;
}

/*!
    Returns a reference to the operand status codes.

    \sa operandStatusCodes()
*/
QVector<QOpcUa::UaStatusCode> &QOpcUa::QContentFilterElementResult::operandStatusCodesRef()
{
    return data->operandStatusCodes;
}

/*!
    \class QOpcUa::QEventFilterResult
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPCUA EventFilterResult.

    The EventFilterResult contains status codes for all elements of the \c select clauses
    and all elements of the \c where clause.
*/

class QOpcUa::QEventFilterResultData : public QSharedData
{
public:
    QVector<QOpcUa::UaStatusCode> selectClauseResults;
    QVector<QOpcUa::QContentFilterElementResult> whereClauseResults;
};

QOpcUa::QEventFilterResult::QEventFilterResult()
    : data(new QOpcUa::QEventFilterResultData)
{
}

/*!
    Constructs an event filter result from \a rhs.
*/
QOpcUa::QEventFilterResult::QEventFilterResult(const QOpcUa::QEventFilterResult &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this event filter result.
*/
QOpcUa::QEventFilterResult &QOpcUa::QEventFilterResult::operator=(const QOpcUa::QEventFilterResult &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUa::QEventFilterResult::~QEventFilterResult()
{
}

/*!
    Returns \c true if this event filter result is good.
*/
bool QOpcUa::QEventFilterResult::isGood() const
{
    for (auto status : qAsConst(data->selectClauseResults)) {
        if (status != QOpcUa::UaStatusCode::Good)
            return false;
    }
    for (QOpcUa::QContentFilterElementResult element : qAsConst(data->whereClauseResults)) {
        if (element.statusCode() != QOpcUa::UaStatusCode::Good)
            return false;
        for (auto status : qAsConst(element.operandStatusCodesRef())) {
            if (status != QOpcUa::UaStatusCode::Good)
                return false;
        }
    }

    return true;
}

/*!
    Returns the status codes for all elements of the \c where clause in the order that was used in the filter.
*/
QVector<QOpcUa::QContentFilterElementResult> QOpcUa::QEventFilterResult::whereClauseResults() const
{
    return data->whereClauseResults;
}

/*!
    Returns a reference to the \c where clause results.

    \sa whereClauseResults()
*/
QVector<QOpcUa::QContentFilterElementResult> &QOpcUa::QEventFilterResult::whereClauseResultsRef()
{
    return data->whereClauseResults;
}

/*!
    Sets the \c where clause results to \a whereClausesResult.
*/
void QOpcUa::QEventFilterResult::setWhereClauseResults(const QVector<QOpcUa::QContentFilterElementResult> &whereClausesResult)
{
    data->whereClauseResults = whereClausesResult;
}

/*!
    Returns the status codes for all elements of the \c select clauses in the order that was used in the filter.
*/
QVector<QOpcUa::UaStatusCode> QOpcUa::QEventFilterResult::selectClauseResults() const
{
    return data->selectClauseResults;
}

/*!
    Returns a reference to the \c select clause results.

    \sa selectClauseResults()
*/
QVector<QOpcUa::UaStatusCode> &QOpcUa::QEventFilterResult::selectClauseResultsRef()
{
    return data->selectClauseResults;
}

/*!
    Sets the \c select clause results to \a selectClausesResult.
*/
void QOpcUa::QEventFilterResult::setSelectClauseResults(const QVector<QOpcUa::UaStatusCode> &selectClausesResult)
{
    data->selectClauseResults = selectClausesResult;
}

/*!
    \class QOpcUa::QUserTokenPolicy
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA UserTokenPolicy.

    The user token policy contains information about an user token accepted by the server.
*/

/*!
    \enum QOpcUa::QUserTokenPolicy::TokenType

    \value Anonymous No token required.
    \value Username Username and password are required.
    \value Certificate A client certificate is required.
    \value IssuedToken Any Web Services Security (WS-Security) defined token.
*/

class QOpcUa::QUserTokenPolicyData : public QSharedData
{
public:
    QString policyId;
    QOpcUa::QUserTokenPolicy::TokenType tokenType{QOpcUa::QUserTokenPolicy::TokenType::Anonymous};
    QString issuedTokenType;
    QString issuerEndpointUrl;
    QString securityPolicyUri;
};

QOpcUa::QUserTokenPolicy::QUserTokenPolicy()
    : data(new QOpcUa::QUserTokenPolicyData)
{
}

/*!
    Constructs an user token policy from \a rhs.
*/
QOpcUa::QUserTokenPolicy::QUserTokenPolicy(const QUserTokenPolicy &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this user token policy.
*/
QOpcUa::QUserTokenPolicy &QOpcUa::QUserTokenPolicy::operator=(const QOpcUa::QUserTokenPolicy &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUa::QUserTokenPolicy::~QUserTokenPolicy()
{
}

/*!
    Returns the URI of the security policy required when encrypting or signing the token for ActivateSession.
*/
QString QOpcUa::QUserTokenPolicy::securityPolicyUri() const
{
    return data->securityPolicyUri;
}

/*!
    Sets the URI of the security policy to \a securityPolicyUri.
*/
void QOpcUa::QUserTokenPolicy::setSecurityPolicyUri(const QString &securityPolicyUri)
{
    data->securityPolicyUri = securityPolicyUri;
}

/*!
    Returns the URL of a token issuing service.
*/
QString QOpcUa::QUserTokenPolicy::issuerEndpointUrl() const
{
    return data->issuerEndpointUrl;
}

/*!
    Sets the URL of the token issuing service to \a issuerEndpointUrl.
*/
void QOpcUa::QUserTokenPolicy::setIssuerEndpointUrl(const QString &issuerEndpointUrl)
{
    data->issuerEndpointUrl = issuerEndpointUrl;
}

/*!
    Returns the URI for the token type.
*/
QString QOpcUa::QUserTokenPolicy::issuedTokenType() const
{
    return data->issuedTokenType;
}

/*!
    Sets the URI for the token type to \a issuedTokenType.
*/
void QOpcUa::QUserTokenPolicy::setIssuedTokenType(const QString &issuedTokenType)
{
    data->issuedTokenType = issuedTokenType;
}

/*!
    Returns the type of the required user identity token.
*/
QOpcUa::QUserTokenPolicy::TokenType QOpcUa::QUserTokenPolicy::tokenType() const
{
    return data->tokenType;
}

/*!
    Sets the type of the required user identity token to \a tokenType.
*/
void QOpcUa::QUserTokenPolicy::setTokenType(TokenType tokenType)
{
    data->tokenType = tokenType;
}

/*!
    Returns a server assigned identifier for this policy.
*/
QString QOpcUa::QUserTokenPolicy::policyId() const
{
    return data->policyId;
}

/*!
    Sets the identifier for this policy to \a policyId.
*/
void QOpcUa::QUserTokenPolicy::setPolicyId(const QString &policyId)
{
    data->policyId = policyId;
}

/*!
    \class QOpcUa::QApplicationDescription
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA ApplicationDescription.

    The application description contains information about an OPC UA application.
*/

/*!
    \enum QOpcUa::QApplicationDescription::ApplicationType

    \value Server This application is a server.
    \value Client This application is a client.
    \value ClientAndServer This application is a client and a server.
    \value DiscoveryServer This application is a discovery server.
*/

class QOpcUa::QApplicationDescriptionData : public QSharedData
{
public:
    QString applicationUri;
    QString productUri;
    QOpcUa::QLocalizedText applicationName;
    QOpcUa::QApplicationDescription::ApplicationType applicationType{QOpcUa::QApplicationDescription::ApplicationType::Server};
    QString gatewayServerUri;
    QString discoveryProfileUri;
    QVector<QString> discoveryUrls;
};

QOpcUa::QApplicationDescription::QApplicationDescription()
    : data(new QOpcUa::QApplicationDescriptionData)
{
}

/*!
    Constructs an application description from \a other.
*/
QOpcUa::QApplicationDescription::QApplicationDescription(const QOpcUa::QApplicationDescription &other)
    : data(other.data)
{
}

/*!
    Sets the values from \a other in this application description.
*/
QOpcUa::QApplicationDescription &QOpcUa::QApplicationDescription::operator=(const QOpcUa::QApplicationDescription &other)
{
    this->data = other.data;
    return *this;
}

QOpcUa::QApplicationDescription::~QApplicationDescription()
{
}

/*!
    Returns a list of URLs of discovery endpoints.
*/
QVector<QString> QOpcUa::QApplicationDescription::discoveryUrls() const
{
    return data->discoveryUrls;
}

/*!
    Returns a reference to a list of URLs of discovery endpoints.
*/
QVector<QString> &QOpcUa::QApplicationDescription::discoveryUrlsRef()
{
    return data->discoveryUrls;
}

/*!
    Sets the discovery URLs to \a discoveryUrls.
*/
void QOpcUa::QApplicationDescription::setDiscoveryUrls(const QVector<QString> &discoveryUrls)
{
    data->discoveryUrls = discoveryUrls;
}

/*!
    Returns the URI of the supported discovery profile.
*/
QString QOpcUa::QApplicationDescription::discoveryProfileUri() const
{
    return data->discoveryProfileUri;
}

/*!
    Sets the discovery profile URI to \a discoveryProfileUri.
*/
void QOpcUa::QApplicationDescription::setDiscoveryProfileUri(const QString &discoveryProfileUri)
{
    data->discoveryProfileUri = discoveryProfileUri;
}

/*!
    Returns the URI of the gateway server.
*/
QString QOpcUa::QApplicationDescription::gatewayServerUri() const
{
    return data->gatewayServerUri;
}

/*!
    Sets the URI of the gateway server to \a gatewayServerUri.
*/
void QOpcUa::QApplicationDescription::setGatewayServerUri(const QString &gatewayServerUri)
{
    data->gatewayServerUri = gatewayServerUri;
}

/*!
    Returns the application's type (server, client, both, discovery server).
*/
QOpcUa::QApplicationDescription::ApplicationType QOpcUa::QApplicationDescription::applicationType() const
{
    return data->applicationType;
}

/*!
    Sets the application type to \a applicationType.
*/
void QOpcUa::QApplicationDescription::setApplicationType(ApplicationType applicationType)
{
    data->applicationType = applicationType;
}

/*!
    Returns a name describing the application.
*/
QOpcUa::QLocalizedText QOpcUa::QApplicationDescription::applicationName() const
{
    return data->applicationName;
}

/*!
    Sets the application name to \a applicationName.
*/
void QOpcUa::QApplicationDescription::setApplicationName(const QOpcUa::QLocalizedText &applicationName)
{
    data->applicationName = applicationName;
}

/*!
    Returns the globally unique identifier for this product.
*/
QString QOpcUa::QApplicationDescription::productUri() const
{
    return data->productUri;
}

/*!
    Sets the globally unique identifier for this product to \a productUri.
*/
void QOpcUa::QApplicationDescription::setProductUri(const QString &productUri)
{
    data->productUri = productUri;
}

/*!
    Returns the globally unique identifier for this application instance.
*/
QString QOpcUa::QApplicationDescription::applicationUri() const
{
    return data->applicationUri;
}

/*!
    Sets the globally unique identifier for this application instance to \a applicationUri.
*/
void QOpcUa::QApplicationDescription::setApplicationUri(const QString &applicationUri)
{
    data->applicationUri = applicationUri;
}

/*!
    \class QOpcUa::QEndpointDescription
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA EndpointDescription.

    An endpoint description contains information about an endpoint and how to connect to it.
*/

/*!
    \enum QOpcUa::QEndpointDescription::MessageSecurityMode

    \value Invalid The default value, will be rejected by the server.
    \value None No security.
    \value Sign Messages are signed but not encrypted.
    \value SignAndEncrypt Messages are signed and encrypted.
*/

class QOpcUa::QEndpointDescriptionData : public QSharedData
{
public:
    QString endpointUrl;
    QOpcUa::QApplicationDescription server;
    QByteArray serverCertificate;
    QOpcUa::QEndpointDescription::MessageSecurityMode securityMode{QOpcUa::QEndpointDescription::MessageSecurityMode::None};
    QString securityPolicyUri;
    QVector<QOpcUa::QUserTokenPolicy> userIdentityTokens;
    QString transportProfileUri;
    quint8 securityLevel{0};
};

QOpcUa::QEndpointDescription::QEndpointDescription()
    : data(new QOpcUa::QEndpointDescriptionData)
{
}

/*!
    Constructs an endpoint description from \a rhs.
*/
QOpcUa::QEndpointDescription::QEndpointDescription(const QOpcUa::QEndpointDescription &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this endpoint description.
*/
QOpcUa::QEndpointDescription &QOpcUa::QEndpointDescription::operator=(const QOpcUa::QEndpointDescription &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUa::QEndpointDescription::~QEndpointDescription()
{
}

/*!
    Returns a relative index assigned by the server. It describes how secure this
    endpoint is compared to other endpoints of the same server. An endpoint with strong
    security measures has a higher security level than one with weaker or no security
    measures.

    Security level 0 indicates an endpoint for backward compatibility purposes which
    should only be used if the client does not support the security measures required
    by more secure endpoints.
*/
quint8 QOpcUa::QEndpointDescription::securityLevel() const
{
    return data->securityLevel;
}

/*!
    Sets the security level to \a securityLevel.
*/
void QOpcUa::QEndpointDescription::setSecurityLevel(quint8 securityLevel)
{
    data->securityLevel = securityLevel;
}

/*!
    Returns the URI of the transport profile supported by the endpoint.
*/
QString QOpcUa::QEndpointDescription::transportProfileUri() const
{
    return data->transportProfileUri;
}

/*!
    Sets the URI of the transport profile supported by the endpoint to \a transportProfileUri.
*/
void QOpcUa::QEndpointDescription::setTransportProfileUri(const QString &transportProfileUri)
{
    data->transportProfileUri = transportProfileUri;
}

/*!
    Returns a list of user identity tokens the endpoint will accept.
*/
QVector<QOpcUa::QUserTokenPolicy> QOpcUa::QEndpointDescription::userIdentityTokens() const
{
    return data->userIdentityTokens;
}

/*!
    Returns a reference to a list of user identity tokens the endpoint will accept.
*/
QVector<QOpcUa::QUserTokenPolicy> &QOpcUa::QEndpointDescription::userIdentityTokensRef()
{
    return data->userIdentityTokens;
}

/*!
    Sets the user identity tokens to \a userIdentityTokens.
*/
void QOpcUa::QEndpointDescription::setUserIdentityTokens(const QVector<QOpcUa::QUserTokenPolicy> &userIdentityTokens)
{
    data->userIdentityTokens = userIdentityTokens;
}

/*!
    Returns the URI of the security policy.
*/
QString QOpcUa::QEndpointDescription::securityPolicyUri() const
{
    return data->securityPolicyUri;
}

/*!
    Sets the URI of the security policy to \a securityPolicyUri.
*/
void QOpcUa::QEndpointDescription::setSecurityPolicyUri(const QString &securityPolicyUri)
{
    data->securityPolicyUri = securityPolicyUri;
}

/*!
    Returns the security mode supported by this endpoint.
*/
QOpcUa::QEndpointDescription::MessageSecurityMode QOpcUa::QEndpointDescription::securityMode() const
{
    return data->securityMode;
}

/*!
    Sets the security mode supported by this endpoint to \a securityMode.
*/
void QOpcUa::QEndpointDescription::setSecurityMode(MessageSecurityMode securityMode)
{
    data->securityMode = securityMode;
}

/*!
    Returns the application instance certificate of the server.
*/
QByteArray QOpcUa::QEndpointDescription::serverCertificate() const
{
    return data->serverCertificate;
}

/*!
    Sets the application instance certificate of the server to \a serverCertificate.
*/
void QOpcUa::QEndpointDescription::setServerCertificate(const QByteArray &serverCertificate)
{
    data->serverCertificate = serverCertificate;
}

/*!
    Returns the application description of the server.
*/
QOpcUa::QApplicationDescription QOpcUa::QEndpointDescription::server() const
{
    return data->server;
}

/*!
    Returns a reference to the application description of the server.
*/
QOpcUa::QApplicationDescription &QOpcUa::QEndpointDescription::serverRef()
{
    return data->server;
}

/*!
    Sets the application description of the server to \a server.
*/
void QOpcUa::QEndpointDescription::setServer(const QOpcUa::QApplicationDescription &server)
{
    data->server = server;
}

/*!
    Returns the URL for the endpoint.
*/
QString QOpcUa::QEndpointDescription::endpointUrl() const
{
    return data->endpointUrl;
}

/*!
    Sets the URL for the endpoint to \a endpointUrl.
*/
void QOpcUa::QEndpointDescription::setEndpointUrl(const QString &endpointUrl)
{
    data->endpointUrl = endpointUrl;
}

/*!
    \class QOpcUa::QArgument
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief The OPC UA Argument type.

    This is the Qt OPC UA representation for the Argument type defined in OPC-UA part 3, 8.6.

    The Argument type is mainly used for the values of the InputArguments and OutputArguments properties
    which describe the parameters and return values of method nodes.
*/
class QOpcUa::QArgumentData : public QSharedData
{
public:
    QString name;
    QString dataTypeId;
    qint32 valueRank{-2};
    QVector<quint32> arrayDimensions;
    QOpcUa::QLocalizedText description;
};

QOpcUa::QArgument::QArgument()
    : data(new QOpcUa::QArgumentData)
{
}

QOpcUa::QArgument::QArgument(const QOpcUa::QArgument &rhs)
    : data(rhs.data)
{
}

/*!
    Constructs an argument with name \a name, data type id \a dataTypeId, value rank \a valueRank,
    array dimensions \a arrayDimensions and description \a description.
*/
QOpcUa::QArgument::QArgument(const QString &name, const QString &dataTypeId, qint32 valueRank,
                             const QVector<quint32> &arrayDimensions, const QOpcUa::QLocalizedText &description)
    : data(new QOpcUa::QArgumentData)
{
    setName(name);
    setDataTypeId(dataTypeId);
    setValueRank(valueRank);
    setArrayDimensions(arrayDimensions);
    setDescription(description);
}

/*!
    Sets the values from \a rhs in this argument.
*/
QOpcUa::QArgument &QOpcUa::QArgument::operator=(const QOpcUa::QArgument &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns true if this argument has the same value as \a other.
*/
bool QOpcUa::QArgument::operator==(const QOpcUa::QArgument &other) const
{
    return data->arrayDimensions == other.arrayDimensions() &&
            nodeIdEquals(data->dataTypeId, other.dataTypeId()) &&
            data->description == other.description() &&
            data->name == other.name() &&
            data->valueRank == other.valueRank();
}

/*!
    Converts this argument to \l QVariant.
*/
QOpcUa::QArgument::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QOpcUa::QArgument::~QArgument()
{
}

/*!
    Returns the name of the argument.
*/
QString QOpcUa::QArgument::name() const
{
    return data->name;
}

/*!
    Sets the name of the argument to \a name.
*/
void QOpcUa::QArgument::setName(const QString &name)
{
    data->name = name;
}

/*!
    Returns the data type node id of the argument.
*/
QString QOpcUa::QArgument::dataTypeId() const
{
    return data->dataTypeId;
}

/*!
    Sets the data type node id of the argument to \a dataTypeId.
*/
void QOpcUa::QArgument::setDataTypeId(const QString &dataTypeId)
{
    data->dataTypeId = dataTypeId;
}

/*!
    Returns the value rank of the argument.
    The value rank describes the structure of the value.
    \table
        \header
            \li ValueRank
            \li Meaning
        \row
            \li -3
            \li Scalar or one dimensional array
        \row
            \li -2
            \li Scalar or array with any number of dimensions
        \row
            \li -1
            \li Not an array
        \row
            \li 0
            \li Array with one or more dimensions
        \row
            \li 1
            \li One dimensional array
        \row
            \li >1
            \li Array with n dimensions
    \endtable
*/
qint32 QOpcUa::QArgument::valueRank() const
{
    return data->valueRank;
}

/*!
    Sets the value rank of the argument to \a valueRank.
*/
void QOpcUa::QArgument::setValueRank(qint32 valueRank)
{
    data->valueRank = valueRank;
}

/*!
    Returns the array dimensions of the argument.

    The array dimensions describe the length of each array dimension.
*/
QVector<quint32> QOpcUa::QArgument::arrayDimensions() const
{
    return data->arrayDimensions;
}

/*!
    Returns a reference to the array dimensions of the argument.
*/
QVector<quint32> &QOpcUa::QArgument::arrayDimensionsRef()
{
    return data->arrayDimensions;
}

/*!
    Sets the array dimensions of the argument to \a arrayDimensions.
*/
void QOpcUa::QArgument::setArrayDimensions(const QVector<quint32> &arrayDimensions)
{
    data->arrayDimensions = arrayDimensions;
}

/*!
    Returns the description of the argument.
*/
QOpcUa::QLocalizedText QOpcUa::QArgument::description() const
{
    return data->description;
}

/*!
    Sets the description of the argument to \a description.
*/
void QOpcUa::QArgument::setDescription(const QOpcUa::QLocalizedText &description)
{
    data->description = description;
}

/*!
    \class QOpcUa::QExtensionObject
    \inmodule QtOpcUa
    \brief The OPC UA ExtensionObject.

    This is the Qt OPC UA representation for an extension object.
    Extension objects are used as a container in OPC UA whenever a non-builtin type is stored
    in a Variant. It contains information about the type and encoding of the data as well as
    the data itself encoded with one of the encodings specified in OPC-UA part 6.
    Decoders are supposed to decode extension objects if they can handle the type. If the type
    is not supported by the decoder, the extension object is not decoded and decoding is left
    to the user.
*/

/*!
    \enum QOpcUa::QExtensionObject::Encoding

    Enumerates the possible encodings of the body.

    \value NoBody
    \value ByteString
    \value Xml
*/

class QOpcUa::QExtensionObjectData : public QSharedData
{
public:
    QString encodingTypeId;
    QByteArray encodedBody;
    QOpcUa::QExtensionObject::Encoding encoding{QOpcUa::QExtensionObject::Encoding::NoBody};
};

QOpcUa::QExtensionObject::QExtensionObject()
    : data(new QExtensionObjectData)
{
}

/*!
    Constructs an extension object from \a rhs.
*/
QOpcUa::QExtensionObject::QExtensionObject(const QExtensionObject &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this extension object.
*/
QOpcUa::QExtensionObject &QOpcUa::QExtensionObject::operator=(const QOpcUa::QExtensionObject &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this extension object has the same value as \a rhs.
*/
bool QOpcUa::QExtensionObject::operator==(const QOpcUa::QExtensionObject &rhs) const
{
    return data->encoding == rhs.encoding() &&
            nodeIdEquals(data->encodingTypeId, rhs.encodingTypeId()) &&
            data->encodedBody == rhs.encodedBody();
}

/*!
    Converts this extension object to \l QVariant.
*/
QOpcUa::QExtensionObject::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QOpcUa::QExtensionObject::~QExtensionObject()
{
}

/*!
    Returns the \l {QOpcUa::QExtensionObject::Encoding} {encoding} of the body.
*/
QOpcUa::QExtensionObject::Encoding QOpcUa::QExtensionObject::encoding() const
{
    return data->encoding;
}

/*!
    Sets the encoding of the body to \a encoding.
*/
void QOpcUa::QExtensionObject::setEncoding(QOpcUa::QExtensionObject::Encoding encoding)
{
    data->encoding = encoding;
}

/*!
    Returns the body of this extension object. It contains the encoded data.
*/
QByteArray QOpcUa::QExtensionObject::encodedBody() const
{
    return data->encodedBody;
}

/*!
    Returns a reference to the body of this extension object.
*/
QByteArray &QOpcUa::QExtensionObject::encodedBodyRef()
{
    return data->encodedBody;
}
/*!
    Sets the body of this extension object to \a encodedBody.
*/
void QOpcUa::QExtensionObject::setEncodedBody(const QByteArray &encodedBody)
{
    data->encodedBody = encodedBody;
}

/*!
    Returns the node id of the encoding for the type stored by this extension object, for example ns=0;i=886 for
    Range_Encoding_DefaultBinary. All encoding ids are listed in \l {https://opcfoundation.org/UA/schemas/1.03/NodeIds.csv}.
*/
QString QOpcUa::QExtensionObject::encodingTypeId() const
{
    return data->encodingTypeId;
}

/*!
    Sets the node id of the encoding for the type stored by this extension object to \a encodingTypeId.
*/
void QOpcUa::QExtensionObject::setEncodingTypeId(const QString &encodingTypeId)
{
    data->encodingTypeId = encodingTypeId;
}

/*
    This class has been modelled in the style of the Variant encoding
    defined in OPC-UA part 6, 5.2.2.16.

    This solution has been preferred to returning nested QVariantLists
    due to the following reasons:
    - A QVariantList inside a QVariantList is stored as a QVariant which must be converted
      to QVariantList before the elements can be accessed. This makes it impossible to update the
      values in place.
    - The length of the array is encoded as a 32 bit unsigned integer.
      Array dimensions are encoded in an array, so an array can have UINT32_MAX dimensions.
      Depending on the number of dimensions, there could be lots of nested QVariantLists
      which would require a huge effort when calculating the array dimensions for conversions
      between QVariantList and the sdk specific variant type.
*/

/*!
    \class QOpcUa::QMultiDimensionalArray
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuatype.h
    \brief A container class for multidimensional arrays.

    This class manages arrays of Qt OPC UA types with associated array dimensions information.
    It is returned as value when a multidimensional array is received from the server. It can also
    be used as a write value or as parameter for filters and method calls.
*/

class QOpcUa::QMultiDimensionalArrayData : public QSharedData
{
public:
    QVariantList value;
    QVector<quint32> arrayDimensions;
    quint32 expectedArrayLength{0};
};

QOpcUa::QMultiDimensionalArray::QMultiDimensionalArray()
    : data(new QOpcUa::QMultiDimensionalArrayData)
{
}

/*!
    Constructs a multidimensional array from \a other.
*/
QOpcUa::QMultiDimensionalArray::QMultiDimensionalArray(const QOpcUa::QMultiDimensionalArray &other)
    : data(other.data)
{
}

/*!
    Sets the values from \a rhs in the multidimensional array.
*/
QOpcUa::QMultiDimensionalArray &QOpcUa::QMultiDimensionalArray::operator=(const QOpcUa::QMultiDimensionalArray &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Constructs a multidimensional array with value \a value and array dimensions \a arrayDimensions.
*/
QOpcUa::QMultiDimensionalArray::QMultiDimensionalArray(const QVariantList &value, const QVector<quint32> &arrayDimensions)
    : data(new QOpcUa::QMultiDimensionalArrayData)
{
    setValueArray(value);
    setArrayDimensions(arrayDimensions);
}

/*!
    Creates a multidimensional array with preallocated data fitting \a arrayDimensions.
*/
QOpcUa::QMultiDimensionalArray::QMultiDimensionalArray(const QVector<quint32> &arrayDimensions)
    : data(new QOpcUa::QMultiDimensionalArrayData)
{
    setArrayDimensions(arrayDimensions);
    if (data->expectedArrayLength) {
        data->value.reserve(data->expectedArrayLength);
        for (size_t i = 0; i < data->expectedArrayLength; ++i)
            data->value.append(QVariant());
    }
}

QOpcUa::QMultiDimensionalArray::~QMultiDimensionalArray()
{
}

/*!
    Returns the dimensions of the multidimensional array.
    The element at position n contains the length of the n-th dimension.
*/
QVector<quint32> QOpcUa::QMultiDimensionalArray::arrayDimensions() const
{
    return data->arrayDimensions;
}

/*!
    Sets the dimensions of the multidimensional array to \a arrayDimensions.
*/
void QOpcUa::QMultiDimensionalArray::setArrayDimensions(const QVector<quint32> &arrayDimensions)
{
    data->arrayDimensions = arrayDimensions;
    data->expectedArrayLength = std::accumulate(data->arrayDimensions.begin(), data->arrayDimensions.end(),
                                                1, std::multiplies<quint32>());
}

/*!
    Returns \c true if this multidimensional array has the same value as \a other.
*/
bool QOpcUa::QMultiDimensionalArray::operator==(const QOpcUa::QMultiDimensionalArray &other) const
{
    return arrayDimensions() == other.arrayDimensions() &&
            valueArray() == other.valueArray();
}

/*!
    Converts this multidimensional array to \l QVariant.
*/
QOpcUa::QMultiDimensionalArray::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
    Returns the value array of the multidimensional array.
*/
QVariantList QOpcUa::QMultiDimensionalArray::valueArray() const
{
    return data->value;
}

/*!
    Returns a reference to the value array of the multidimensional array.
*/
QVariantList &QOpcUa::QMultiDimensionalArray::valueArrayRef()
{
    return data->value;
}

/*!
    Sets the value array of the multidimensional array to \a value.
*/
void QOpcUa::QMultiDimensionalArray::setValueArray(const QVariantList &value)
{
    data->value = value;
}

/*!
    Returns the array index in \l valueArray() of the element identified by \a indices.
    If \a indices is invalid for the array or if the array's dimensions don't match
    the size of \l valueArray(), the invalid index \c -1 is returned.
*/
int QOpcUa::QMultiDimensionalArray::arrayIndex(const QVector<quint32> &indices) const
{
    // A QList can store INT_MAX values. Depending on the platform, this allows a size > UINT32_MAX
    if (data->expectedArrayLength > static_cast<quint64>((std::numeric_limits<int>::max)()) ||
            static_cast<quint64>(data->value.size()) > (std::numeric_limits<quint32>::max)())
        return -1;

    // Check number of dimensions and data size
    if (indices.size() != data->arrayDimensions.size() ||
            data->expectedArrayLength != static_cast<quint32>(data->value.size()))
        return -1; // Missing array dimensions or array dimensions don't fit the array

    quint32 index = 0;
    quint32 stride = 1;
    // Reverse iteration to avoid repetitions while calculating the stride
    for (int i = data->arrayDimensions.size() - 1; i >= 0; --i) {
        if (indices.at(i) >= data->arrayDimensions.at(i)) // Out of bounds
            return -1;

        // Arrays are encoded in row-major order: [0,0,0], [0,0,1], [0,1,0], [0,1,1], [1,0,0], [1,0,1], [1,1,0], [1,1,1]
        // The stride for dimension i in a n dimensional array is the product of all array dimensions from i+1 to n
        if (i < data->arrayDimensions.size() - 1)
            stride *= data->arrayDimensions.at(i + 1);
        index += stride * indices.at(i);
    }

    return (index <= static_cast<quint64>((std::numeric_limits<int>::max)())) ?
                static_cast<int>(index) : -1;
}

/*!
    Returns the value of the element identified by \a indices.
    If the indices are invalid for the array, an empty \l QVariant is returned.
*/
QVariant QOpcUa::QMultiDimensionalArray::value(const QVector<quint32> &indices) const
{
    int index = arrayIndex(indices);

    if (index < 0)
        return QVariant();

    return data->value.at(index);
}

/*!
    Sets the value at position \a indices to \a value.
    Returns \c true if the value has been successfully set.
*/
bool QOpcUa::QMultiDimensionalArray::setValue(const QVector<quint32> &indices, const QVariant &value)
{
    int index = arrayIndex(indices);

    if (index < 0)
        return false;

    data->value[index] = value;
    return true;
}

/*!
    Returns \c true if the multidimensional array is valid
*/
bool QOpcUa::QMultiDimensionalArray::isValid() const
{
    return static_cast<quint64>(data->value.size()) == data->expectedArrayLength &&
            static_cast<quint64>(data->value.size()) <= (std::numeric_limits<quint32>::max)() &&
            static_cast<quint64>(data->arrayDimensions.size()) <= (std::numeric_limits<quint32>::max)();
}

QT_END_NAMESPACE

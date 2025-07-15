// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuatype.h"

#include <private/qopcuasecuritypolicyuris_p.h>

#include <QMetaEnum>
#include <QRegularExpression>
#include <QUuid>
#include <QString>
#include <QList>

QT_BEGIN_NAMESPACE

using namespace Qt::Literals::StringLiterals;

/*!
    \namespace QOpcUa
    \inmodule QtOpcua

    \brief The QOpcUa namespace contains miscellaneous identifiers used throughout the Qt OPC UA library.
*/

/*!
    \enum QOpcUa::NodeClass

    This enum specifies the class a node belongs to. OPC UA specifies a fixed
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

    This enum contains the reference types specified in OPC UA 1.05 part 3, 7.
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
    \value IsDeprecated The type for a reference to the information model where the starting node was first deprecated
    \value HasStructuredComponent Indicate that a VariableType or Variable also exposes its Structure fields as Variables in the information model
    \value AssociatedWith The type for a reference to indicate an association between two nodes
    \value UsesPriorityMappingTable The type for a reference to indicate usage of a priority mapping table
    \value HasLowerLayerInterface Virtual interfaces shall reference their physical interfaces with this reference type
    \value IsExecutableOn The type for a reference to relate a software component to its execution environment
    \value Controls Relates a controlling component to the components controlled by the controlling component
    \value Utilizes Relates a component to other components that are needed by the component in order to work
    \value Requires Relates a component to other components it requires in order to work
    \value IsPhysicallyConnectedTo Connects two components that are physically connected, without one being a subcomponent of the other
    \value RepresentsSameEntityAs The type for a reference to indicate that two nodes represent the same entity
    \value RepresentsSameHardwareAs The type for a reference to indicate that two nodes represent the same hardware
    \value RepresentsSameFunctionalityAs The type for a reference to indicate that two nodes represent the same functionality
    \value IsHostedBy Relates functionality represented by an OPC UA Node to the hardware and software needed to provide the functionality
    \value HasPhysicalComponent Relates a hardware component to its subcomponent, which is also a hardware component
    \value HasContainedComponent Relates a hardware component to its subcomponent that is inside the component.
    \value HasAttachedComponent Indicates that the subcomponent is attached to the component
    \value IsExecutingOn The type for a reference to relate a software component to its current execution environment
    \value HasPushedSecurityGroup The type for a reference to a pushed security group
    \value [since 6.9] AlarmSuppressionGroupMember Connects alarm instances or bool variables to an alarm group
    \value [since 6.9] HasReferenceDescription Connects a node of any node class to a reference description variable.
*/

/*!
\enum QOpcUa::NodeAttribute

    This enum contains the 22 node attributes defined in OPC UA 1.05 part 3, 5.

    \value None No node attribute.
    \value NodeId Mandatory for all nodes. Contains the node's id in the OPC UA address space.
    \value NodeClass Mandatory for all nodes. Contains the node id describing the node class of the node.
    \value BrowseName Mandatory for all nodes. Contains a non-localized human readable name of the node.
    \value DisplayName Mandatory for all nodes. Contains a localized human readable name for display purposes.
    \value Description Contains a localized human readable description of the node.
    \value WriteMask Contains a bit mask. Each bit corresponds to a writable attribute (OPC UA 1.05 part 3, 8.60).
    \value UserWriteMask Same as WriteMask but for the current user.
    \value IsAbstract True if the node is an abstract type which means that no nodes of this type shall exist.
    \value Symmetric True if a reference's meaning is the same seen from both ends.
    \value InverseName The localized inverse name of a reference (for example "HasSubtype" has the InverseName "SubtypeOf").
    \value ContainsNoLoops True if there is no way to get back to a node following forward references in the current view.
    \value EventNotifier Contains a bit mask used to indicate if subscribing to events and access to historic events is supported (OPC UA 1.05 part 3, 8.59).
    \value Value The value of a Variable node.
    \value DataType The NodeId of the Value attribute's data type (for example "ns=0;i=13" for DateTime, see https://opcfoundation.org/UA/schemas/1.05/NodeIds.csv).
    \value ValueRank Contains information about the structure of the Value attribute (scalar/array) (OPC UA 1.05 part 3, 5.6.2).
    \value ArrayDimensions An array containing the length for each dimension of a multi-dimensional array.
    \value AccessLevel Contains a bit mask. Each bit corresponds to an access capability (OPC UA 1.05 part 3, 5.6.2).
    \value UserAccessLevel Same as AccessLevel, but for the current user.
    \value MinimumSamplingInterval Contains the shortest possible interval in which the server is able to sample the value.
    \value Historizing True if historical data is collected.
    \value Executable True if the node is currently executable. Only relevant for Method nodes.
    \value UserExecutable Same as Executable, but for the current user.
    \value [since 6.7] DataTypeDefinition The data type definition attribute of a data type node.
    \value [since 6.9] RolePermissions Permissions for all roles with access to the node.
    \value [since 6.9] UserRolePermissions Permissions for all roles of the requesting session.
    \value [since 6.9] AccessRestrictions The AccessRestrictions of the node.
    \value [since 6.9] AccessLevelEx Contains a bit mask. Each bit corresponds to an access capability (OPC UA 1.05 part 3, 8.58).
*/

/*!
    \enum QOpcUa::WriteMaskBit

    This enum contains all possible bits for the WriteMask and UserWriteMask node attributes
    defined in OPC UA 1.05 part 3, 8.60.

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
    \value [since 6.9] DataTypeDefinition The DataTypeDefinition attribute is writable.
    \value [since 6.9] RolePermissions The RolePermissions attribute is writable.
    \value [since 6.9] AccessRestrictions The AccessRestrictions attribute is writable.
    \value [since 6.9] AccessLevelEx The AccessLevelEx attribute is writable.
*/

/*!
    \enum QOpcUa::AccessLevelBit

    This enum contains all possible bits for the AccessLevel and UserAccessLevel node attributes
    defined in OPC UA 1.05 part 3, 8.57.

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
    \enum QOpcUa::AccessLevelExBit
    \since 6.9

    This enum contains all possible bits for the AccessLevelEx node attribute
    defined in OPC UA 1.05 part 3, 8.58.

    \value None No read access to the Value attribute is permitted.
    \value CurrentRead The current value can be read.
    \value CurrentWrite The current value can be written.
    \value HistoryRead The history of the value is readable.
    \value HistoryWrite The history of the value is writable.
    \value SemanticChange The property variable generates SemanticChangeEvents.
    \value StatusWrite The status code of the value is writable.
    \value TimestampWrite The SourceTimestamp is writable.
    \value NonatomicRead Indicates if the read is non-atomic.
    \value NonatomicWrite Indicates if the write is non-atomic.
    \value WriteFullArrayOnly Indicates if writing an index range of an array is supported.
    \value NoSubDataTypes Indicates if a write also accepts subtypes of the type.
    \value NonVolatile Indicates if the variable is volatile.
    \value Constant Indicates if the variable's value is constant.
*/

/*!
    \enum QOpcUa::EventNotifierBit

    This enum contains all possible bits for the EventNotifier node attribute
    defined in OPC UA 1.05 part 3, 8.59.

    \value None The node can't be used to interact with events.
    \value SubscribeToEvents A client can subscribe to events.
    \value HistoryRead A client can read the event history.
    \value HistoryWrite A client can write the event history.
*/

/*!
    \enum QOpcUa::TimestampsToReturn
    \since 6.7

    This enum contains the options for timestamps to return for service calls.
    It is specified in OPC UA 1.05, Part 4, 7.40.

    \value Source Return the source timestamp
    \value Server Return the server timestamp
    \value Both Return the source and the server timestamp
    \value Neither Return no timestamps
    \value Invalid The invalid value specified by OPC UA
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
    \value StructureDefinition The OPC UA StructureDefinition type
    \value StructureField The OPC UA StructureField type
    \value EnumDefinition The OPC UA EnumDefinition type
    \value EnumField The OPC UA EnumField type
    \value DiagnosticInfo The OPC UA DiagnosticInfo type
    \value SimpleAttributeOperand The OPC UA SimpleAttributeOperand type.
    \value AttributeOperand The OPC UA AttributeOperand type.
    \value LiteralOperand The OPC UA LiteralOperand type.
    \value ElementOperand The OPC UA ElementOperand type.
    \value RelativePathElement The OPC UA RelativePathElement type.
    \value ContentFilterElement The OPC UA ContentFilterElement type.
    \value EventFilter The OPC UA EventFilter type.
    \value Undefined
*/

/*!
    \enum QOpcUa::UaStatusCode

    Enumerates all status codes from \l {https://opcfoundation.org/UA/schemas/1.05/Opc.Ua.StatusCodes.csv}

    \value Good The operation succeeded.
    \value Uncertain The operation was uncertain.
    \value Bad The operation failed.
    \value BadUnexpectedError An unexpected error occurred.
    \value BadInternalError An internal error occurred as a result of a programming or configuration error.
    \value BadOutOfMemory Not enough memory to complete the operation.
    \value BadResourceUnavailable An operating system resource is not available.
    \value BadCommunicationError A low level communication error occurred.
    \value BadEncodingError Encoding halted because of invalid data in the objects being serialized.
    \value BadDecodingError Decoding halted because of invalid data in the stream.
    \value BadEncodingLimitsExceeded The message encoding/decoding limits imposed by the stack have been exceeded.
    \value BadRequestTooLarge The request message size exceeds limits set by the server.
    \value BadResponseTooLarge The response message size exceeds limits set by the client.
    \value BadUnknownResponse An unrecognized response was received from the server.
    \value BadTimeout The operation timed out.
    \value BadServiceUnsupported The server does not support the requested service.
    \value BadShutdown The operation was cancelled because the application is shutting down.
    \value BadServerNotConnected The operation could not complete because the client is not connected to the server.
    \value BadServerHalted The server has stopped and cannot process any requests.
    \value BadNothingToDo No processing could be done because there was nothing to do.
    \value BadTooManyOperations The request could not be processed because it specified too many operations.
    \value BadTooManyMonitoredItems The request could not be processed because there are too many monitored items in the subscription.
    \value BadDataTypeIdUnknown The extension object cannot be (de)serialized because the data type id is not recognized.
    \value BadCertificateInvalid The certificate provided as a parameter is not valid.
    \value BadSecurityChecksFailed An error occurred verifying security.
    \value BadCertificatePolicyCheckFailed The certificate does not meet the requirements of the security policy.
    \value BadCertificateTimeInvalid The certificate has expired or is not yet valid.
    \value BadCertificateIssuerTimeInvalid An issuer certificate has expired or is not yet valid.
    \value BadCertificateHostNameInvalid The HostName used to connect to a server does not match a HostName in the certificate.
    \value BadCertificateUriInvalid The URI specified in the ApplicationDescription does not match the URI in the certificate.
    \value BadCertificateUseNotAllowed The certificate may not be used for the requested operation.
    \value BadCertificateIssuerUseNotAllowed The issuer certificate may not be used for the requested operation.
    \value BadCertificateUntrusted The certificate is not trusted.
    \value BadCertificateRevocationUnknown It was not possible to determine if the certificate has been revoked.
    \value BadCertificateIssuerRevocationUnknown It was not possible to determine if the issuer certificate has been revoked.
    \value BadCertificateRevoked The certificate has been revoked.
    \value BadCertificateIssuerRevoked The issuer certificate has been revoked.
    \value BadCertificateChainIncomplete The certificate chain is incomplete.
    \value BadUserAccessDenied User does not have permission to perform the requested operation.
    \value BadIdentityTokenInvalid The user identity token is not valid.
    \value BadIdentityTokenRejected The user identity token is valid but the server has rejected it.
    \value BadSecureChannelIdInvalid The specified secure channel is no longer valid.
    \value BadInvalidTimestamp The timestamp is outside the range allowed by the server.
    \value BadNonceInvalid The nonce does appear to be not a random value or it is not the correct length.
    \value BadSessionIdInvalid The session id is not valid.
    \value BadSessionClosed The session was closed by the client.
    \value BadSessionNotActivated The session cannot be used because ActivateSession has not been called.
    \value BadSubscriptionIdInvalid The subscription id is not valid.
    \value BadRequestHeaderInvalid The header for the request is missing or invalid.
    \value BadTimestampsToReturnInvalid The timestamps to return parameter is invalid.
    \value BadRequestCancelledByClient The request was cancelled by the client.
    \value BadTooManyArguments Too many arguments were provided.
    \value BadLicenseExpired The server requires a license to operate in general or to perform a service or operation, but existing license is expired.
    \value BadLicenseLimitsExceeded The server has limits on number of allowed operations / objects, based on installed licenses, and these limits where exceeded.
    \value BadLicenseNotAvailable The server does not have a license which is required to operate in general or to perform a service or operation.
    \value GoodSubscriptionTransferred The subscription was transferred to another session.
    \value GoodCompletesAsynchronously The processing will complete asynchronously.
    \value GoodOverload Sampling has slowed down due to resource limitations.
    \value GoodClamped The value written was accepted but was clamped.
    \value BadNoCommunication Communication with the data source is defined, but not established, and there is no last known value available.
    \value BadWaitingForInitialData Waiting for the server to obtain values from the underlying data source.
    \value BadNodeIdInvalid The syntax of the node id is not valid.
    \value BadNodeIdUnknown The node id refers to a node that does not exist in the server address space.
    \value BadAttributeIdInvalid The attribute is not supported for the specified Node.
    \value BadIndexRangeInvalid The syntax of the index range parameter is invalid.
    \value BadIndexRangeNoData No data exists within the range of indexes specified.
    \value BadDataEncodingInvalid The data encoding is invalid.
    \value BadDataEncodingUnsupported The server does not support the requested data encoding for the node.
    \value BadNotReadable The access level does not allow reading or subscribing to the Node.
    \value BadNotWritable The access level does not allow writing to the Node.
    \value BadOutOfRange The value was out of range.
    \value BadNotSupported The requested operation is not supported.
    \value BadNotFound A requested item was not found or a search operation ended without success.
    \value BadObjectDeleted The object cannot be used because it has been deleted.
    \value BadNotImplemented Requested operation is not implemented.
    \value BadMonitoringModeInvalid The monitoring mode is invalid.
    \value BadMonitoredItemIdInvalid The monitoring item id does not refer to a valid monitored item.
    \value BadMonitoredItemFilterInvalid The monitored item filter parameter is not valid.
    \value BadMonitoredItemFilterUnsupported The server does not support the requested monitored item filter.
    \value BadFilterNotAllowed A monitoring filter cannot be used in combination with the attribute specified.
    \value BadStructureMissing A mandatory structured parameter was missing or null.
    \value BadEventFilterInvalid The event filter is not valid.
    \value BadContentFilterInvalid The content filter is not valid.
    \value BadFilterOperatorInvalid An unrecognized operator was provided in a filter.
    \value BadFilterOperatorUnsupported A valid operator was provided, but the server does not provide support for this filter operator.
    \value BadFilterOperandCountMismatch The number of operands provided for the filter operator was less then expected for the operand provided.
    \value BadFilterOperandInvalid The operand used in a content filter is not valid.
    \value BadFilterElementInvalid The referenced element is not a valid element in the content filter.
    \value BadFilterLiteralInvalid The referenced literal is not a valid value.
    \value BadContinuationPointInvalid The continuation point provide is longer valid.
    \value BadNoContinuationPoints The operation could not be processed because all continuation points have been allocated.
    \value BadReferenceTypeIdInvalid The reference type id does not refer to a valid reference type node.
    \value BadBrowseDirectionInvalid The browse direction is not valid.
    \value BadNodeNotInView The node is not part of the view.
    \value BadNumericOverflow The number was not accepted because of a numeric overflow.
    \value BadServerUriInvalid The ServerUri is not a valid URI.
    \value BadServerNameMissing No ServerName was specified.
    \value BadDiscoveryUrlMissing No DiscoveryUrl was specified.
    \value BadSempahoreFileMissing The semaphore file specified by the client is not valid.
    \value BadRequestTypeInvalid The security token request type is not valid.
    \value BadSecurityModeRejected The security mode does not meet the requirements set by the server.
    \value BadSecurityPolicyRejected The security policy does not meet the requirements set by the server.
    \value BadTooManySessions The server has reached its maximum number of sessions.
    \value BadUserSignatureInvalid The user token signature is missing or invalid.
    \value BadApplicationSignatureInvalid The signature generated with the client certificate is missing or invalid.
    \value BadNoValidCertificates The client did not provide at least one software certificate that is valid and meets the profile requirements for the server.
    \value BadIdentityChangeNotSupported The server does not support changing the user identity assigned to the session.
    \value BadRequestCancelledByRequest The request was cancelled by the client with the Cancel service.
    \value BadParentNodeIdInvalid The parent node id does not to refer to a valid node.
    \value BadReferenceNotAllowed The reference could not be created because it violates constraints imposed by the data model.
    \value BadNodeIdRejected The requested node id was reject because it was either invalid or server does not allow node ids to be specified by the client.
    \value BadNodeIdExists The requested node id is already used by another node.
    \value BadNodeClassInvalid The node class is not valid.
    \value BadBrowseNameInvalid The browse name is invalid.
    \value BadBrowseNameDuplicated The browse name is not unique among nodes that share the same relationship with the parent.
    \value BadNodeAttributesInvalid The node attributes are not valid for the node class.
    \value BadTypeDefinitionInvalid The type definition node id does not reference an appropriate type node.
    \value BadSourceNodeIdInvalid The source node id does not reference a valid node.
    \value BadTargetNodeIdInvalid The target node id does not reference a valid node.
    \value BadDuplicateReferenceNotAllowed The reference type between the nodes is already defined.
    \value BadInvalidSelfReference The server does not allow this type of self reference on this node.
    \value BadReferenceLocalOnly The reference type is not valid for a reference to a remote server.
    \value BadNoDeleteRights The server will not allow the node to be deleted.
    \value UncertainReferenceNotDeleted The server was not able to delete all target references.
    \value BadServerIndexInvalid The server index is not valid.
    \value BadViewIdUnknown The view id does not refer to a valid view node.
    \value BadViewTimestampInvalid The view timestamp is not available or not supported.
    \value BadViewParameterMismatch The view parameters are not consistent with each other.
    \value BadViewVersionInvalid The view version is not available or not supported.
    \value UncertainNotAllNodesAvailable The list of references may not be complete because the underlying system is not available.
    \value GoodResultsMayBeIncomplete The server should have followed a reference to a node in a remote server but did not. The result set may be incomplete.
    \value BadNotTypeDefinition The provided Nodeid was not a type definition nodeid.
    \value UncertainReferenceOutOfServer One of the references to follow in the relative path references to a node in the address space in another server.
    \value BadTooManyMatches The requested operation has too many matches to return.
    \value BadQueryTooComplex The requested operation requires too many resources in the server.
    \value BadNoMatch The requested operation has no match to return.
    \value BadMaxAgeInvalid The max age parameter is invalid.
    \value BadSecurityModeInsufficient The operation is not permitted over the current secure channel.
    \value BadHistoryOperationInvalid The history details parameter is not valid.
    \value BadHistoryOperationUnsupported The server does not support the requested operation.
    \value BadInvalidTimestampArgument The defined timestamp to return was invalid.
    \value BadWriteNotSupported The server does not support writing the combination of value, status and timestamps provided.
    \value BadTypeMismatch The value supplied for the attribute is not of the same type as the attribute's value.
    \value BadMethodInvalid The method id does not refer to a method for the specified object.
    \value BadArgumentsMissing The client did not specify all of the input arguments for the method.
    \value BadNotExecutable The executable attribute does not allow the execution of the method.
    \value BadTooManySubscriptions The server has reached its maximum number of subscriptions.
    \value BadTooManyPublishRequests The server has reached the maximum number of queued publish requests.
    \value BadNoSubscription There is no subscription available for this session.
    \value BadSequenceNumberUnknown The sequence number is unknown to the server.
    \value GoodRetransmissionQueueNotSupported The Server does not support retransmission queue and acknowledgement of sequence numbers is not available.
    \value BadMessageNotAvailable The requested notification message is no longer available.
    \value BadInsufficientClientProfile The client of the current session does not support one or more Profiles that are necessary for the subscription.
    \value BadStateNotActive The sub-state machine is not currently active.
    \value BadAlreadyExists An equivalent rule already exists.
    \value BadTcpServerTooBusy The server cannot process the request because it is too busy.
    \value BadTcpMessageTypeInvalid The type of the message specified in the header invalid.
    \value BadTcpSecureChannelUnknown The SecureChannelId and/or TokenId are not currently in use.
    \value BadTcpMessageTooLarge The size of the message chunk specified in the header is too large.
    \value BadTcpNotEnoughResources There are not enough resources to process the request.
    \value BadTcpInternalError An internal error occurred.
    \value BadTcpEndpointUrlInvalid The server does not recognize the QueryString specified.
    \value BadRequestInterrupted The request could not be sent because of a network interruption.
    \value BadRequestTimeout Timeout occurred while processing the request.
    \value BadSecureChannelClosed The secure channel has been closed.
    \value BadSecureChannelTokenUnknown The token has expired or is not recognized.
    \value BadSequenceNumberInvalid The sequence number is not valid.
    \value BadProtocolVersionUnsupported The applications do not have compatible protocol versions.
    \value BadConfigurationError There is a problem with the configuration that affects the usefulness of the value.
    \value BadNotConnected The variable should receive its value from another variable, but has never been configured to do so.
    \value BadDeviceFailure There has been a failure in the device/data source that generates the value that has affected the value.
    \value BadSensorFailure There has been a failure in the sensor from which the value is derived by the device/data source.
    \value BadOutOfService The source of the data is not operational.
    \value BadDeadbandFilterInvalid The deadband filter is not valid.
    \value UncertainNoCommunicationLastUsableValue Communication to the data source has failed. The variable value is the last value that had a good quality.
    \value UncertainLastUsableValue Whatever was updating this value has stopped doing so.
    \value UncertainSubstituteValue The value is an operational value that was manually overwritten.
    \value UncertainInitialValue The value is an initial value for a variable that normally receives its value from another variable.
    \value UncertainSensorNotAccurate The value is at one of the sensor limits.
    \value UncertainEngineeringUnitsExceeded The value is outside of the range of values defined for this parameter.
    \value UncertainSubNormal The value is derived from multiple sources and has less than the required number of Good sources.
    \value GoodLocalOverride The value has been overridden.
    \value BadRefreshInProgress This Condition refresh failed, a Condition refresh operation is already in progress.
    \value BadConditionAlreadyDisabled This condition has already been disabled.
    \value BadConditionAlreadyEnabled This condition has already been enabled.
    \value BadConditionDisabled Property not available, this condition is disabled.
    \value BadEventIdUnknown The specified event id is not recognized.
    \value BadEventNotAcknowledgeable The event cannot be acknowledged.
    \value BadDialogNotActive The dialog condition is not active.
    \value BadDialogResponseInvalid The response is not valid for the dialog.
    \value BadConditionBranchAlreadyAcked The condition branch has already been acknowledged.
    \value BadConditionBranchAlreadyConfirmed The condition branch has already been confirmed.
    \value BadConditionAlreadyShelved The condition has already been shelved.
    \value BadConditionNotShelved The condition is not currently shelved.
    \value BadShelvingTimeOutOfRange The shelving time not within an acceptable range.
    \value BadNoData No data exists for the requested time range or event filter.
    \value BadBoundNotFound No data found to provide upper or lower bound value.
    \value BadBoundNotSupported The server cannot retrieve a bound for the variable.
    \value BadDataLost Data is missing due to collection started/stopped/lost.
    \value BadDataUnavailable Expected data is unavailable for the requested time range due to an un-mounted volume, an off-line archive or tape, or similar reason for temporary unavailability.
    \value BadEntryExists The data or event was not successfully inserted because a matching entry exists.
    \value BadNoEntryExists The data or event was not successfully updated because no matching entry exists.
    \value BadTimestampNotSupported The client requested history using a timestamp format the server does not support (i.e requested ServerTimestamp when server only supports SourceTimestamp).
    \value GoodEntryInserted The data or event was successfully inserted into the historical database.
    \value GoodEntryReplaced The data or event field was successfully replaced in the historical database.
    \value UncertainDataSubNormal The value is derived from multiple values and has less than the required number of Good values.
    \value GoodNoData No data exists for the requested time range or event filter.
    \value GoodMoreData More data is available in the time range beyond the number of values requested.
    \value BadAggregateListMismatch The requested number of Aggregates does not match the requested number of NodeIds.
    \value BadAggregateNotSupported The requested Aggregate is not support by the server.
    \value BadAggregateInvalidInputs The aggregate value could not be derived due to invalid data inputs.
    \value BadAggregateConfigurationRejected The aggregate configuration is not valid for specified node.
    \value GoodDataIgnored The request specifies fields which are not valid for the EventType or cannot be saved by the historian.
    \value BadRequestNotAllowed The request was rejected by the server because it did not meet the criteria set by the server.
    \value BadRequestNotComplete The request has not been processed by the server yet.
    \value BadTransactionPending The operation is not allowed because a transaction is in progress.
    \value BadTicketRequired The device identity needs a ticket before it can be accepted.
    \value BadTicketInvalid The device identity needs a ticket before it can be accepted.
    \value BadLocked The requested operation is not allowed, because the Node is locked by a different application.
    \value GoodEdited The value does not come from the real source and has been edited by the server.
    \value GoodPostActionFailed There was an error in execution of these post-actions.
    \value UncertainDominantValueChanged The related EngineeringUnit has been changed but the Variable Value is still provided based on the previous unit.
    \value GoodDependentValueChanged A dependent value has been changed but the change has not been applied to the device.
    \value BadDominantValueChanged The related EngineeringUnit has been changed but this change has not been applied to the device. The Variable Value is still dependent on the previous unit but its status is currently Bad.
    \value UncertainDependentValueChanged A dependent value has been changed but the change has not been applied to the device. The quality of the dominant variable is uncertain.
    \value BadDependentValueChanged A dependent value has been changed but the change has not been applied to the device. The quality of the dominant variable is Bad.
    \value GoodEdited_DependentValueChanged It is delivered with a dominant Variable value when a dependent Variable has changed but the change has not been applied.
    \value GoodEdited_DominantValueChanged It is delivered with a dependent Variable value when a dominant Variable has changed but the change has not been applied.
    \value GoodEdited_DominantValueChanged_DependentValueChanged It is delivered with a dependent Variable value when a dominant or dependent Variable has changed but change has not been applied.
    \value BadEdited_OutOfRange It is delivered with a Variable value when Variable has changed but the value is not legal.
    \value BadInitialValue_OutOfRange It is delivered with a Variable value when a source Variable has changed but the value is not legal.
    \value BadOutOfRange_DominantValueChanged It is delivered with a dependent Variable value when a dominant Variable has changed and the value is not legal.
    \value BadEdited_OutOfRange_DominantValueChanged It is delivered with a dependent Variable value when a dominant Variable has changed, the value is not legal and the change has not been applied.
    \value BadOutOfRange_DominantValueChanged_DependentValueChanged It is delivered with a dependent Variable value when a dominant or dependent Variable has changed and the value is not legal.
    \value BadEdited_OutOfRange_DominantValueChanged_DependentValueChanged It is delivered with a dependent Variable value when a dominant or dependent Variable has changed, the value is not legal and the change has not been applied.
    \value GoodCommunicationEvent The communication layer has raised an event.
    \value GoodShutdownEvent The system is shutting down.
    \value GoodCallAgain The operation is not finished and needs to be called again.
    \value GoodNonCriticalTimeout A non-critical timeout occurred.
    \value BadInvalidArgument One or more arguments are invalid.
    \value BadConnectionRejected Could not establish a network connection to remote server.
    \value BadDisconnect The server has disconnected from the client.
    \value BadConnectionClosed The network connection has been closed.
    \value BadInvalidState The operation cannot be completed because the object is closed, uninitialized or in some other invalid state.
    \value BadEndOfStream Cannot move beyond end of the stream.
    \value BadNoDataAvailable No data is currently available for reading from a non-blocking stream.
    \value BadWaitingForResponse The asynchronous operation is waiting for a response.
    \value BadOperationAbandoned The asynchronous operation was abandoned by the caller.
    \value BadExpectedStreamToBlock The stream did not return all data requested (possibly because it is a non-blocking stream).
    \value BadWouldBlock Non blocking behaviour is required and the operation would block.
    \value BadSyntaxError A value had an invalid syntax.
    \value BadMaxConnectionsReached The operation could not be finished because all available connections are in use.
    \value UncertainTransducerInManual The value may not be accurate because the transducer is in manual mode.
    \value UncertainSimulatedValue The value is simulated.
    \value UncertainSensorCalibration The value may not be accurate due to a sensor calibration fault.
    \value UncertainConfigurationError The value may not be accurate due to a configuration issue.
    \value GoodCascadeInitializationAcknowledged The value source supports cascade handshaking and the value has been Initialized based on an initialization request from a cascade secondary.
    \value GoodCascadeInitializationRequest The value source supports cascade handshaking and is requesting initialization of a cascade primary.
    \value GoodCascadeNotInvited The value source supports cascade handshaking, however, the source’s current state does not allow for cascade.
    \value GoodCascadeNotSelected The value source supports cascade handshaking, however, the source has not selected the corresponding cascade primary for use.
    \value GoodFaultStateActive There is a fault state condition active in the value source.
    \value GoodInitiateFaultState A fault state condition is being requested of the destination.
    \value GoodCascade The value is accurate, and the signal source supports cascade handshaking.
    \value BadDataSetIdInvalid The DataSet specified for the DataSetWriter creation is invalid.
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
    The meaning of the status codes for the different services is documented in OPC UA part 4.

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
    return u"ns=%1;s=%2"_s.arg(ns).arg(identifier);
}

/*!
    Creates a node id string from the namespace index \a ns and the byte string \a identifier.
    \sa QOpcUaNode
*/
QString QOpcUa::nodeIdFromByteString(quint16 ns, const QByteArray &identifier)
{
    return u"ns=%1;b=%2"_s.arg(ns).arg(QString::fromUtf8(identifier.toBase64()));
}

/*!
    Creates a node id string from the namespace index \a ns and the GUID \a identifier.
    \sa QOpcUaNode
*/
QString QOpcUa::nodeIdFromGuid(quint16 ns, const QUuid &identifier)
{
    return u"ns=%1;g="_s.arg(ns).append(QStringView(identifier.toString()).mid(1, 36)); // Remove enclosing {...};
}

/*!
    Creates a node id string from the namespace index \a ns and the integer \a identifier.
    \sa QOpcUaNode
*/
QString QOpcUa::nodeIdFromInteger(quint16 ns, quint32 identifier)
{
    return u"ns=%1;i=%2"_s.arg(ns).arg(identifier);
}

/*!
    Creates a node id string for the reference type id \a referenceType.
*/
QString QOpcUa::nodeIdFromReferenceType(QOpcUa::ReferenceTypeId referenceType)
{
    return u"ns=0;i=%1"_s.arg(static_cast<quint32>(referenceType));
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

    QStringList components = nodeIdString.split(';'_L1);

    if (components.size() > 2)
        return false;

    static const QRegularExpression namespaceRegex(u"^ns=[0-9]+"_s);
    if (components.size() == 2 && components.at(0).contains(namespaceRegex)) {
        bool success = false;
        uint ns = QStringView(components.at(0)).mid(3).toUInt(&success);
        if (!success || ns > (std::numeric_limits<quint16>::max)())
            return false;
        namespaceIndex = ns;
    }

    if (components.last().size() < 3)
        return false;

    static const QRegularExpression identifierRegex(u"^[isgb]="_s);
    if (!components.last().contains(identifierRegex))
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
    if (first.startsWith("ns=0;"_L1) && !second.startsWith("ns="_L1))
        return fView.mid(5) == sView;
    else if (second.startsWith("ns=0;"_L1) && !first.startsWith("ns="_L1))
        return sView.mid(5) == fView;
    else
        return first == second;
}

/*!
    Returns a node id string for the namespace 0 identifier \a id.
*/
QString QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0 id)
{
    return u"ns=0;i=%1"_s.arg(quint32(id));
}

/*!
    Returns the enum value from \l QOpcUa::NodeIds::Namespace0 for \a nodeId.

    If the node id is not in namespace 0 or doesn't have a numeric
    identifier which is part of the OPC Foundation's NodeIds.csv file,
    \l {QOpcUa::NodeIds::Namespace0} {Unknown} is returned.

    If Qt OPC UA has been configured with FEATURE_ns0idnames=OFF,
    the check if the numeric identifier is part of the NodeIds.csv
    file is omitted. If the node id is in namespace 0 and has a
    numeric identifier, the identifier is returned regardless if it
    is part of the \l QOpcUa::NodeIds::Namespace0 enum.
*/
QOpcUa::NodeIds::Namespace0 QOpcUa::namespace0IdFromNodeId(const QString &nodeId)
{
    if (!nodeId.startsWith("ns=0;i="_L1))
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

    If \a id is unknown or Qt OPC UA has been configured with FEATURE_ns0idnames=OFF,
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

    The AxisScale enum as defined by OPC UA 1.05 part 8, 5.6.7.

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
        return u"Invalid enum value for UaStatusCode"_s;
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
    if (type == "ns=0;i=1"_L1)
        return QOpcUa::Boolean;
    else if (type == "ns=0;i=3"_L1)
        return QOpcUa::Byte;
    else if (type == "ns=0;i=2"_L1)
        return QOpcUa::SByte;
    else if (type == "ns=0;i=5"_L1)
        return QOpcUa::UInt16;
    else if (type == "ns=0;i=4"_L1)
        return QOpcUa::Int16;
    else if (type == "ns=0;i=6"_L1)
        return QOpcUa::Int32;
    else if (type == "ns=0;i=7"_L1)
        return QOpcUa::UInt32;
    else if (type == "ns=0;i=9"_L1)
        return QOpcUa::UInt64;
    else if (type == "ns=0;i=8"_L1)
        return QOpcUa::Int64;
    else if (type == "ns=0;i=11"_L1)
        return QOpcUa::Double;
    else if (type == "ns=0;i=10"_L1)
        return QOpcUa::Float;
    else if (type == "ns=0;i=12"_L1)
        return QOpcUa::String;
    else if (type == "ns=0;i=13"_L1)
        return QOpcUa::DateTime;
    else if (type == "ns=0;i=15"_L1)
        return QOpcUa::ByteString;
    else if (type == "ns=0;i=14"_L1)
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
    return securityPolicy == Basic128Rsa15Policy ||
           securityPolicy == Basic256Policy ||
           securityPolicy == Basic256Sha256Policy ||
           securityPolicy == Aes128Sha256RsaOaepPolicy ||
           securityPolicy == Aes256Sha256RsaPssPolicy;
}

QT_END_NAMESPACE


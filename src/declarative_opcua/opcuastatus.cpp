// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/opcuastatus_p.h>
#include <private/opcuaconnection_p.h>

QT_BEGIN_NAMESPACE

/*!
    \qmltype Status
    \inqmlmodule QtOpcUa
    \brief Status code of an OPC UA function.
    \since QtOpcUa 5.13

    This QML element contains information about the result status of an OPC UA action.
    The most common use case is to check if a call was successful
    \code
    if (methodNode.resultCode.isGood) {
        // do something
    }
     \endcode
*/

/*!
    \qmlproperty bool Status::isGood

    Returns whether the result status is a success.
*/

/*!
    \qmlproperty bool Status::isBad

    Returns whether the result status is a failure.
*/

/*!
    \qmlproperty Status.Status Status::status

    Returns the exact status code. This allows to handle the status specifically.

    \code
    if (resultStatus.status == QtOpcUa.Status.BadTimeout) {
        // try again
    }
    \endcode
*/

/*!
   \qmlproperty enumeration Status::Status

   The names of this enumeration are matching the names from the standard, but
   the values are different.

   Attributes of a status

   \value Status.Good Everything is fine.
   \value Status.BadUnexpectedError An unexpected error occurred.
   \value Status.BadInternalError An internal error occurred as a result of a programming or configuration error.
   \value Status.BadOutOfMemory Not enough memory to complete the operation.
   \value Status.BadResourceUnavailable An operating system resource is not available.
   \value Status.BadCommunicationError A low level communication error occurred.
   \value Status.BadEncodingError Encoding halted because of invalid data in the objects being serialized.
   \value Status.BadDecodingError Decoding halted because of invalid data in the stream.
   \value Status.BadEncodingLimitsExceeded The message encoding/decoding limits imposed by the stack have been exceeded.
   \value Status.BadRequestTooLarge The request message size exceeds limits set by the server.
   \value Status.BadResponseTooLarge The response message size exceeds limits set by the client.
   \value Status.BadUnknownResponse An unrecognized response was received from the server.
   \value Status.BadTimeout The operation timed out.
   \value Status.BadServiceUnsupported The server does not support the requested service.
   \value Status.BadShutdown The operation was cancelled because the application is shutting down.
   \value Status.BadServerNotConnected The operation could not complete because the client is not connected to the server.
   \value Status.BadServerHalted The server has stopped and cannot process any requests.
   \value Status.BadNothingToDo There was nothing to do because the client passed a list of operations with no elements.
   \value Status.BadTooManyOperations The request could not be processed because it specified too many operations.
   \value Status.BadTooManyMonitoredItems The request could not be processed because there are too many monitored items in the subscription.
   \value Status.BadDataTypeIdUnknown The extension object cannot be (de)serialized because the data type id is not recognized.
   \value Status.BadCertificateInvalid The certificate provided as a parameter is not valid.
   \value Status.BadSecurityChecksFailed An error occurred verifying security.
   \value Status.BadCertificateTimeInvalid The Certificate has expired or is not yet valid.
   \value Status.BadCertificateIssuerTimeInvalid An Issuer Certificate has expired or is not yet valid.
   \value Status.BadCertificateHostNameInvalid The HostName used to connect to a Server does not match a HostName in the Certificate.
   \value Status.BadCertificateUriInvalid The URI specified in the ApplicationDescription does not match the URI in the Certificate.
   \value Status.BadCertificateUseNotAllowed The Certificate may not be used for the requested operation.
   \value Status.BadCertificateIssuerUseNotAllowed The Issuer Certificate may not be used for the requested operation.
   \value Status.BadCertificateUntrusted The Certificate is not trusted.
   \value Status.BadCertificateRevocationUnknown It was not possible to determine if the Certificate has been revoked.
   \value Status.BadCertificateIssuerRevocationUnknown It was not possible to determine if the Issuer Certificate has been revoked.
   \value Status.BadCertificateRevoked The certificate has been revoked.
   \value Status.BadCertificateIssuerRevoked The issuer certificate has been revoked.
   \value Status.BadCertificateChainIncomplete The certificate chain is incomplete.
   \value Status.BadUserAccessDenied User does not have permission to perform the requested operation.
   \value Status.BadIdentityTokenInvalid The user identity token is not valid.
   \value Status.BadIdentityTokenRejected The user identity token is valid but the server has rejected it.
   \value Status.BadSecureChannelIdInvalid The specified secure channel is no longer valid.
   \value Status.BadInvalidTimestamp The timestamp is outside the range allowed by the server.
   \value Status.BadNonceInvalid The nonce does appear to be not a random value or it is not the correct length.
   \value Status.BadSessionIdInvalid The session id is not valid.
   \value Status.BadSessionClosed The session was closed by the client.
   \value Status.BadSessionNotActivated The session cannot be used because ActivateSession has not been called.
   \value Status.BadSubscriptionIdInvalid The subscription id is not valid.
   \value Status.BadRequestHeaderInvalid The header for the request is missing or invalid.
   \value Status.BadTimestampsToReturnInvalid The timestamps to return parameter is invalid.
   \value Status.BadRequestCancelledByClient The request was cancelled by the client.
   \value Status.BadTooManyArguments Too many arguments were provided.
   \value Status.GoodSubscriptionTransferred The subscription was transferred to another session.
   \value Status.GoodCompletesAsynchronously The processing will complete asynchronously.
   \value Status.GoodOverload Sampling has slowed down due to resource limitations.
   \value Status.GoodClamped The value written was accepted but was clamped.
   \value Status.BadNoCommunication Communication with the data source is defined, but not established, and there is no last known value available.
   \value Status.BadWaitingForInitialData Waiting for the server to obtain values from the underlying data source.
   \value Status.BadNodeIdInvalid The syntax of the node id is not valid.
   \value Status.BadNodeIdUnknown The node id refers to a node that does not exist in the server address space.
   \value Status.BadAttributeIdInvalid The attribute is not supported for the specified Node.
   \value Status.BadIndexRangeInvalid The syntax of the index range parameter is invalid.
   \value Status.BadIndexRangeNoData No data exists within the range of indexes specified.
   \value Status.BadDataEncodingInvalid The data encoding is invalid.
   \value Status.BadDataEncodingUnsupported The server does not support the requested data encoding for the node.
   \value Status.BadNotReadable The access level does not allow reading or subscribing to the Node.
   \value Status.BadNotWritable The access level does not allow writing to the Node.
   \value Status.BadOutOfRange The value was out of range.
   \value Status.BadNotSupported The requested operation is not supported.
   \value Status.BadNotFound A requested item was not found or a search operation ended without success.
   \value Status.BadObjectDeleted The object cannot be used because it has been deleted.
   \value Status.BadNotImplemented Requested operation is not implemented.
   \value Status.BadMonitoringModeInvalid The monitoring mode is invalid.
   \value Status.BadMonitoredItemIdInvalid The monitoring item id does not refer to a valid monitored item.
   \value Status.BadMonitoredItemFilterInvalid The monitored item filter parameter is not valid.
   \value Status.BadMonitoredItemFilterUnsupported The server does not support the requested monitored item filter.
   \value Status.BadFilterNotAllowed A monitoring filter cannot be used in combination with the attribute specified.
   \value Status.BadStructureMissing A mandatory structured parameter was missing or null.
   \value Status.BadEventFilterInvalid The event filter is not valid.
   \value Status.BadContentFilterInvalid The content filter is not valid.
   \value Status.BadFilterOperatorInvalid An unregognized operator was provided in a filter.
   \value Status.BadFilterOperatorUnsupported A valid operator was provided, but the server does not provide support for this filter operator.
   \value Status.BadFilterOperandCountMismatch The number of operands provided for the filter operator was less then expected for the operand provided.
   \value Status.BadFilterOperandInvalid The operand used in a content filter is not valid.
   \value Status.BadFilterElementInvalid The referenced element is not a valid element in the content filter.
   \value Status.BadFilterLiteralInvalid The referenced literal is not a valid value.
   \value Status.BadContinuationPointInvalid The continuation point provide is longer valid.
   \value Status.BadNoContinuationPoints The operation could not be processed because all continuation points have been allocated.
   \value Status.BadReferenceTypeIdInvalid The operation could not be processed because all continuation points have been allocated.
   \value Status.BadBrowseDirectionInvalid The browse direction is not valid.
   \value Status.BadNodeNotInView The node is not part of the view.
   \value Status.BadServerUriInvalid The ServerUri is not a valid URI.
   \value Status.BadServerNameMissing No ServerName was specified.
   \value Status.BadDiscoveryUrlMissing No DiscoveryUrl was specified.
   \value Status.BadSempahoreFileMissing The semaphore file specified by the client is not valid.
   \value Status.BadRequestTypeInvalid The security token request type is not valid.
   \value Status.BadSecurityModeRejected The security mode does not meet the requirements set by the Server.
   \value Status.BadSecurityPolicyRejected The security policy does not meet the requirements set by the Server.
   \value Status.BadTooManySessions The server has reached its maximum number of sessions.
   \value Status.BadUserSignatureInvalid The user token signature is missing or invalid.
   \value Status.BadApplicationSignatureInvalid The signature generated with the client certificate is missing or invalid.
   \value Status.BadNoValidCertificates The client did not provide at least one software certificate that is valid and meets the profile requirements for the server.
   \value Status.BadIdentityChangeNotSupported The Server does not support changing the user identity assigned to the session.
   \value Status.BadRequestCancelledByRequest The request was cancelled by the client with the Cancel service.
   \value Status.BadParentNodeIdInvalid The parent node id does not to refer to a valid node.
   \value Status.BadReferenceNotAllowed The reference could not be created because it violates constraints imposed by the data model.
   \value Status.BadNodeIdRejected The requested node id was reject because it was either invalid or server does not allow node ids to be specified by the client.
   \value Status.BadNodeIdExists The requested node id is already used by another node.
   \value Status.BadNodeClassInvalid The node class is not valid.
   \value Status.BadBrowseNameInvalid The browse name is invalid.
   \value Status.BadBrowseNameDuplicated The browse name is not unique among nodes that share the same relationship with the parent.
   \value Status.BadNodeAttributesInvalid The node attributes are not valid for the node class.
   \value Status.BadTypeDefinitionInvalid The type definition node id does not reference an appropriate type node.
   \value Status.BadSourceNodeIdInvalid The source node id does not reference a valid node.
   \value Status.BadTargetNodeIdInvalid The target node id does not reference a valid node.
   \value Status.BadDuplicateReferenceNotAllowed he reference type between the nodes is already defined.
   \value Status.BadInvalidSelfReference The server does not allow this type of self reference on this node.
   \value Status.BadReferenceLocalOnly The reference type is not valid for a reference to a remote server.
   \value Status.BadNoDeleteRights The server will not allow the node to be deleted.
   \value Status.UncertainReferenceNotDeleted The server was not able to delete all target references.
   \value Status.BadServerIndexInvalid The server index is not valid.
   \value Status.BadViewIdUnknown The view id does not refer to a valid view node.
   \value Status.BadViewTimestampInvalid The view timestamp is not available or not supported.
   \value Status.BadViewParameterMismatch The view parameters are not consistent with each other.
   \value Status.BadViewVersionInvalid The view version is not available or not supported.
   \value Status.UncertainNotAllNodesAvailable The list of references may not be complete because the underlying system is not available.
   \value Status.GoodResultsMayBeIncomplete The server should have followed a reference to a node in a remote server but did not. The result set may be incomplete.
   \value Status.BadNotTypeDefinition The provided Nodeid was not a type definition nodeid.
   \value Status.UncertainReferenceOutOfServer One of the references to follow in the relative path references to a node in the address space in another server.
   \value Status.BadTooManyMatches The requested operation has too many matches to return.
   \value Status.BadQueryTooComplex The requested operation requires too many resources in the server.
   \value Status.BadNoMatch The requested operation has no match to return.
   \value Status.BadMaxAgeInvalid The max age parameter is invalid.
   \value Status.BadSecurityModeInsufficient The operation is not permitted over the current secure channel.
   \value Status.BadHistoryOperationInvalid The history details parameter is not valid.
   \value Status.BadHistoryOperationUnsupported The server does not support the requested operation.
   \value Status.BadInvalidTimestampArgument The defined timestamp to return was invalid.
   \value Status.BadWriteNotSupported The server not does support writing the combination of value, status and timestamps provided.
   \value Status.BadTypeMismatch The value supplied for the attribute is not of the same type as the attribute's value.
   \value Status.BadMethodInvalid The method id does not refer to a method for the specified object.
   \value Status.BadArgumentsMissing The client did not specify all of the input arguments for the method.
   \value Status.BadTooManySubscriptions The server has reached its  maximum number of subscriptions.
   \value Status.BadTooManyPublishRequests The server has reached the maximum number of queued publish requests.
   \value Status.BadNoSubscription There is no subscription available for this session.
   \value Status.BadSequenceNumberUnknown The sequence number is unknown to the server.
   \value Status.BadMessageNotAvailable The requested notification message is no longer available.
   \value Status.BadInsufficientClientProfile The Client of the current Session does not support one or more Profiles that are necessary for the Subscription.
   \value Status.BadStateNotActive The sub-state machine is not currently active.
   \value Status.BadTcpServerTooBusy The server cannot process the request because it is too busy.
   \value Status.BadTcpMessageTypeInvalid The type of the message specified in the header invalid.
   \value Status.BadTcpSecureChannelUnknown The SecureChannelId and/or TokenId are not currently in use.
   \value Status.BadTcpMessageTooLarge The size of the message specified in the header is too large.
   \value Status.BadTcpNotEnoughResources There are not enough resources to process the request.
   \value Status.BadTcpInternalError An internal error occurred.
   \value Status.BadTcpEndpointUrlInvalid The Server does not recognize the QueryString specified.
   \value Status.BadRequestInterrupted The request could not be sent because of a network interruption.
   \value Status.BadRequestTimeout Timeout occurred while processing the request.
   \value Status.BadSecureChannelClosed The secure channel has been closed.
   \value Status.BadSecureChannelTokenUnknown The token has expired or is not recognized.
   \value Status.BadSequenceNumberInvalid The sequence number is not valid.
   \value Status.BadProtocolVersionUnsupported The applications do not have compatible protocol versions.
   \value Status.BadConfigurationError There is a problem with the configuration that affects the usefulness of the value.
   \value Status.BadNotConnected The variable should receive its value from another variable, but has never been configured to do so.
   \value Status.BadDeviceFailure There has been a failure in the device/data source that generates the value that has affected the value.
   \value Status.BadSensorFailure There has been a failure in the sensor from which the value is derived by the device/data source.
   \value Status.BadOutOfService The source of the data is not operational.
   \value Status.BadDeadbandFilterInvalid The deadband filter is not valid.
   \value Status.UncertainNoCommunicationLastUsableValue Communication to the data source has failed. The variable value is the last value that had a good quality.
   \value Status.UncertainLastUsableValue Whatever was updating this value has stopped doing so.
   \value Status.UncertainSubstituteValue The value is an operational value that was manually overwritten.
   \value Status.UncertainInitialValue The value is an initial value for a variable that normally receives its value from another variable.
   \value Status.UncertainSensorNotAccurate The value is at one of the sensor limits.
   \value Status.UncertainEngineeringUnitsExceeded The value is outside of the range of values defined for this parameter.
   \value Status.UncertainSubNormal The value is derived from multiple sources and has less than the required number of Good sources.
   \value Status.GoodLocalOverride The value has been overridden.
   \value Status.BadRefreshInProgress This Condition refresh failed, a Condition refresh operation is already in progress.
   \value Status.BadConditionAlreadyDisabled This condition has already been disabled.
   \value Status.BadConditionAlreadyEnabled This condition has already been enabled.
   \value Status.BadConditionDisabled Property not available, this condition is disabled.
   \value Status.BadEventIdUnknown The specified event id is not recognized.
   \value Status.BadEventNotAcknowledgeable The event cannot be acknowledged.
   \value Status.BadDialogNotActive The dialog condition is not active.
   \value Status.BadDialogResponseInvalid The response is not valid for the dialog.
   \value Status.BadConditionBranchAlreadyAcked The condition branch has already been acknowledged.
   \value Status.BadConditionBranchAlreadyConfirmed The condition branch has already been confirmed.
   \value Status.BadConditionAlreadyShelved The condition has already been shelved.
   \value Status.BadConditionNotShelved The condition is not currently shelved.
   \value Status.BadShelvingTimeOutOfRange The shelving time not within an acceptable range.
   \value Status.BadNoData No data exists for the requested time range or event filter.
   \value Status.BadBoundNotFoundNo data found to provide upper or lower bound value.
   \value Status.BadBoundNotSupported The server cannot retrieve a bound for the variable.
   \value Status.BadDataLost Data is missing due to collection started/stopped/lost.
   \value Status.BadDataUnavailable Expected data is unavailable for the requested time range due to an un-mounted volume, an off-line archive or tape, or similar reason for temporary unavailability.
   \value Status.BadEntryExists The data or event was not successfully inserted because a matching entry exists.
   \value Status.BadNoEntryExists The data or event was not successfully updated because no matching entry exists.
   \value Status.BadTimestampNotSupported The client requested history using a timestamp format the server does not support (i.e requested ServerTimestamp when server only supports SourceTimestamp).
   \value Status.GoodEntryInserted The data or event was successfully inserted into the historical database.
   \value Status.GoodEntryReplaced The data or event field was successfully replaced in the historical database.
   \value Status.UncertainDataSubNormal The value is derived from multiple values and has less than the required number of Good values.
   \value Status.GoodNoData No data exists for the requested time range or event filter.
   \value Status.GoodMoreData The data or event field was successfully replaced in the historical database.
   \value Status.BadAggregateListMismatch The requested number of Aggregates does not match the requested number of NodeIds.
   \value Status.BadAggregateNotSupported The requested Aggregate is not support by the server.
   \value Status.BadAggregateInvalidInputs The aggregate value could not be derived due to invalid data inputs.
   \value Status.BadAggregateConfigurationRejected The aggregate configuration is not valid for specified node.
   \value Status.GoodDataIgnored The request pecifies fields which are not valid for the EventType or cannot be saved by the historian
   \value Status.BadRequestNotAllowed The request was rejected by the server because it did not meet the criteria set by the server.
   \value Status.GoodEdited The value does not come from the real source and has been edited by the server.
   \value Status.GoodPostActionFailed There was an error in execution of these post-actions.
   \value Status.UncertainDominantValueChanged The related EngineeringUnit has been changed but the Variable Value is still provided based on the previous unit.
   \value Status.GoodDependentValueChanged A dependent value has been changed but the change has not been applied to the device.
   \value Status.BadDominantValueChanged The related EngineeringUnit has been changed but this change has not been applied to the device. The Variable Value is still dependent on the previous unit but its status is currently Bad.
   \value Status.UncertainDependentValueChanged A dependent value has been changed but the change has not been applied to the device. The quality of the dominant variable is uncertain.
   \value Status.BadDependentValueChanged A dependent value has been changed but the change has not been applied to the device. The quality of the dominant variable is Bad.
   \value Status.GoodCommunicationEvent The communication layer has raised an event.
   \value Status.GoodShutdownEvent The system is shutting down.
   \value Status.GoodCallAgain The operation is not finished and needs to be called again.
   \value Status.GoodNonCriticalTimeout A non-critical timeout occurred.
   \value Status.BadInvalidArgument One or more arguments are invalid.
   \value Status.BadConnectionRejected Could not establish a network connection to remote server.
   \value Status.BadDisconnect The server has disconnected from the client.
   \value Status.BadConnectionClosed The network connection has been closed.
   \value Status.BadInvalidState The operation cannot be completed because the object is closed, uninitialized or in some other invalid state.
   \value Status.BadEndOfStream Cannot move beyond end of the stream.
   \value Status.BadNoDataAvailable No data is currently available for reading from a non-blocking stream.
   \value Status.BadWaitingForResponse The asynchronous operation is waiting for a response.
   \value Status.BadOperationAbandoned The asynchronous operation was abandoned by the caller.
   \value Status.BadExpectedStreamToBlock  The stream did not return all data requested (possibly because it is a non-blocking stream).
   \value Status.BadWouldBlock Non blocking behavior is required and the operation would block.
   \value Status.BadSyntaxError A value had an invalid syntax.
   \value Status.BadMaxConnectionsReached The operation could not be finished because all available connections are in use.
*/

OpcUaStatus::OpcUaStatus() = default;

OpcUaStatus::OpcUaStatus(OpcUaStatus::Status status)
{
    m_statusCode = static_cast<quint32>(status) << 16;
}

OpcUaStatus::~OpcUaStatus() = default;

bool OpcUaStatus::isBad() const
{
    return m_statusCode & 0x80000000;
}

bool OpcUaStatus::isGood() const
{
    return !isBad();
}

OpcUaStatus::OpcUaStatus(QOpcUa::UaStatusCode uaStatusCode)
{
    m_statusCode = static_cast<quint32>(uaStatusCode);
}

OpcUaStatus::Status OpcUaStatus::status() const
{
    switch (static_cast<QOpcUa::UaStatusCode>(m_statusCode)) {
    case QOpcUa::Good:
        return Status::Good;
    case QOpcUa::BadUnexpectedError:
        return Status::BadUnexpectedError;
    case QOpcUa::BadInternalError:
        return Status::BadInternalError;
    case QOpcUa::BadOutOfMemory:
        return Status::BadOutOfMemory;
    case QOpcUa::BadResourceUnavailable:
        return Status::BadResourceUnavailable;
    case QOpcUa::BadCommunicationError:
        return Status::BadCommunicationError;
    case QOpcUa::BadEncodingError:
        return Status::BadEncodingError;
    case QOpcUa::BadDecodingError:
        return Status::BadDecodingError;
    case QOpcUa::BadEncodingLimitsExceeded:
        return Status::BadEncodingLimitsExceeded;
    case QOpcUa::BadRequestTooLarge:
        return Status::BadRequestTooLarge;
    case QOpcUa::BadResponseTooLarge:
        return Status::BadResponseTooLarge;
    case QOpcUa::BadUnknownResponse:
        return Status::BadUnknownResponse;
    case QOpcUa::BadTimeout:
        return Status::BadTimeout;
    case QOpcUa::BadServiceUnsupported:
        return Status::BadServiceUnsupported;
    case QOpcUa::BadShutdown:
        return Status::BadShutdown;
    case QOpcUa::BadServerNotConnected:
        return Status::BadServerNotConnected;
    case QOpcUa::BadServerHalted:
        return Status::BadServerHalted;
    case QOpcUa::BadNothingToDo:
        return Status::BadNothingToDo;
    case QOpcUa::BadTooManyOperations:
        return Status::BadTooManyOperations;
    case QOpcUa::BadTooManyMonitoredItems:
        return Status::BadTooManyMonitoredItems;
    case QOpcUa::BadDataTypeIdUnknown:
        return Status::BadDataTypeIdUnknown;
    case QOpcUa::BadCertificateInvalid:
        return Status::BadCertificateInvalid;
    case QOpcUa::BadSecurityChecksFailed:
        return Status::BadSecurityChecksFailed;
    case QOpcUa::BadCertificateTimeInvalid:
        return Status::BadCertificateTimeInvalid;
    case QOpcUa::BadCertificateIssuerTimeInvalid:
        return Status::BadCertificateIssuerTimeInvalid;
    case QOpcUa::BadCertificateHostNameInvalid:
        return Status::BadCertificateHostNameInvalid;
    case QOpcUa::BadCertificateUriInvalid:
        return Status::BadCertificateUriInvalid;
    case QOpcUa::BadCertificateUseNotAllowed:
        return Status::BadCertificateUseNotAllowed;
    case QOpcUa::BadCertificateIssuerUseNotAllowed:
        return Status::BadCertificateIssuerUseNotAllowed;
    case QOpcUa::BadCertificateUntrusted:
        return Status::BadCertificateUntrusted;
    case QOpcUa::BadCertificateRevocationUnknown:
        return Status::BadCertificateRevocationUnknown;
    case QOpcUa::BadCertificateIssuerRevocationUnknown:
        return Status::BadCertificateIssuerRevocationUnknown;
    case QOpcUa::BadCertificateRevoked:
        return Status::BadCertificateRevoked;
    case QOpcUa::BadCertificateIssuerRevoked:
        return Status::BadCertificateIssuerRevoked;
    case QOpcUa::BadCertificateChainIncomplete:
        return Status::BadCertificateChainIncomplete;
    case QOpcUa::BadUserAccessDenied:
        return Status::BadUserAccessDenied;
    case QOpcUa::BadIdentityTokenInvalid:
        return Status::BadIdentityTokenInvalid;
    case QOpcUa::BadIdentityTokenRejected:
        return Status::BadIdentityTokenRejected;
    case QOpcUa::BadSecureChannelIdInvalid:
        return Status::BadSecureChannelIdInvalid;
    case QOpcUa::BadInvalidTimestamp:
        return Status::BadInvalidTimestamp;
    case QOpcUa::BadNonceInvalid:
        return Status::BadNonceInvalid;
    case QOpcUa::BadSessionIdInvalid:
        return Status::BadSessionIdInvalid;
    case QOpcUa::BadSessionClosed:
        return Status::BadSessionClosed;
    case QOpcUa::BadSessionNotActivated:
        return Status::BadSessionNotActivated;
    case QOpcUa::BadSubscriptionIdInvalid:
        return Status::BadSubscriptionIdInvalid;
    case QOpcUa::BadRequestHeaderInvalid:
        return Status::BadRequestHeaderInvalid;
    case QOpcUa::BadTimestampsToReturnInvalid:
        return Status::BadTimestampsToReturnInvalid;
    case QOpcUa::BadRequestCancelledByClient:
        return Status::BadRequestCancelledByClient;
    case QOpcUa::BadTooManyArguments:
        return Status::BadTooManyArguments;
    case QOpcUa::GoodSubscriptionTransferred:
        return Status::GoodSubscriptionTransferred;
    case QOpcUa::GoodCompletesAsynchronously:
        return Status::GoodCompletesAsynchronously;
    case QOpcUa::GoodOverload:
        return Status::GoodOverload;
    case QOpcUa::GoodClamped:
        return Status::GoodClamped;
    case QOpcUa::BadNoCommunication:
        return Status::BadNoCommunication;
    case QOpcUa::BadWaitingForInitialData:
        return Status::BadWaitingForInitialData;
    case QOpcUa::BadNodeIdInvalid:
        return Status::BadNodeIdInvalid;
    case QOpcUa::BadNodeIdUnknown:
        return Status::BadNodeIdUnknown;
    case QOpcUa::BadAttributeIdInvalid:
        return Status::BadAttributeIdInvalid;
    case QOpcUa::BadIndexRangeInvalid:
        return Status::BadIndexRangeInvalid;
    case QOpcUa::BadIndexRangeNoData:
        return Status::BadIndexRangeNoData;
    case QOpcUa::BadDataEncodingInvalid:
        return Status::BadDataEncodingInvalid;
    case QOpcUa::BadDataEncodingUnsupported:
        return Status::BadDataEncodingUnsupported;
    case QOpcUa::BadNotReadable:
        return Status::BadNotReadable;
    case QOpcUa::BadNotWritable:
        return Status::BadNotWritable;
    case QOpcUa::BadOutOfRange:
        return Status::BadOutOfRange;
    case QOpcUa::BadNotSupported:
        return Status::BadNotSupported;
    case QOpcUa::BadNotFound:
        return Status::BadNotFound;
    case QOpcUa::BadObjectDeleted:
        return Status::BadObjectDeleted;
    case QOpcUa::BadNotImplemented:
        return Status::BadNotImplemented;
    case QOpcUa::BadMonitoringModeInvalid:
        return Status::BadMonitoringModeInvalid;
    case QOpcUa::BadMonitoredItemIdInvalid:
        return Status::BadMonitoredItemIdInvalid;
    case QOpcUa::BadMonitoredItemFilterInvalid:
        return Status::BadMonitoredItemFilterInvalid;
    case QOpcUa::BadMonitoredItemFilterUnsupported:
        return Status::BadMonitoredItemFilterUnsupported;
    case QOpcUa::BadFilterNotAllowed:
        return Status::BadFilterNotAllowed;
    case QOpcUa::BadStructureMissing:
        return Status::BadStructureMissing;
    case QOpcUa::BadEventFilterInvalid:
        return Status::BadEventFilterInvalid;
    case QOpcUa::BadContentFilterInvalid:
        return Status::BadContentFilterInvalid;
    case QOpcUa::BadFilterOperatorInvalid:
        return Status::BadFilterOperatorInvalid;
    case QOpcUa::BadFilterOperatorUnsupported:
        return Status::BadFilterOperatorUnsupported;
    case QOpcUa::BadFilterOperandCountMismatch:
        return Status::BadFilterOperandCountMismatch;
    case QOpcUa::BadFilterOperandInvalid:
        return Status::BadFilterOperandInvalid;
    case QOpcUa::BadFilterElementInvalid:
        return Status::BadFilterElementInvalid;
    case QOpcUa::BadFilterLiteralInvalid:
        return Status::BadFilterLiteralInvalid;
    case QOpcUa::BadContinuationPointInvalid:
        return Status::BadContinuationPointInvalid;
    case QOpcUa::BadNoContinuationPoints:
        return Status::BadNoContinuationPoints;
    case QOpcUa::BadReferenceTypeIdInvalid:
        return Status::BadReferenceTypeIdInvalid;
    case QOpcUa::BadBrowseDirectionInvalid:
        return Status::BadBrowseDirectionInvalid;
    case QOpcUa::BadNodeNotInView:
        return Status::BadNodeNotInView;
    case QOpcUa::BadServerUriInvalid:
        return Status::BadServerUriInvalid;
    case QOpcUa::BadServerNameMissing:
        return Status::BadServerNameMissing;
    case QOpcUa::BadDiscoveryUrlMissing:
        return Status::BadDiscoveryUrlMissing;
    case QOpcUa::BadSempahoreFileMissing:
        return Status::BadSempahoreFileMissing;
    case QOpcUa::BadRequestTypeInvalid:
        return Status::BadRequestTypeInvalid;
    case QOpcUa::BadSecurityModeRejected:
        return Status::BadSecurityModeRejected;
    case QOpcUa::BadSecurityPolicyRejected:
        return Status::BadSecurityPolicyRejected;
    case QOpcUa::BadTooManySessions:
        return Status::BadTooManySessions;
    case QOpcUa::BadUserSignatureInvalid:
        return Status::BadUserSignatureInvalid;
    case QOpcUa::BadApplicationSignatureInvalid:
        return Status::BadApplicationSignatureInvalid;
    case QOpcUa::BadNoValidCertificates:
        return Status::BadNoValidCertificates;
    case QOpcUa::BadIdentityChangeNotSupported:
        return Status::BadIdentityChangeNotSupported;
    case QOpcUa::BadRequestCancelledByRequest:
        return Status::BadRequestCancelledByRequest;
    case QOpcUa::BadParentNodeIdInvalid:
        return Status::BadParentNodeIdInvalid;
    case QOpcUa::BadReferenceNotAllowed:
        return Status::BadReferenceNotAllowed;
    case QOpcUa::BadNodeIdRejected:
        return Status::BadNodeIdRejected;
    case QOpcUa::BadNodeIdExists:
        return Status::BadNodeIdExists;
    case QOpcUa::BadNodeClassInvalid:
        return Status::BadNodeClassInvalid;
    case QOpcUa::BadBrowseNameInvalid:
        return Status::BadBrowseNameInvalid;
    case QOpcUa::BadBrowseNameDuplicated:
        return Status::BadBrowseNameDuplicated;
    case QOpcUa::BadNodeAttributesInvalid:
        return Status::BadNodeAttributesInvalid;
    case QOpcUa::BadTypeDefinitionInvalid:
        return Status::BadTypeDefinitionInvalid;
    case QOpcUa::BadSourceNodeIdInvalid:
        return Status::BadSourceNodeIdInvalid;
    case QOpcUa::BadTargetNodeIdInvalid:
        return Status::BadTargetNodeIdInvalid;
    case QOpcUa::BadDuplicateReferenceNotAllowed:
        return Status::BadDuplicateReferenceNotAllowed;
    case QOpcUa::BadInvalidSelfReference:
        return Status::BadInvalidSelfReference;
    case QOpcUa::BadReferenceLocalOnly:
        return Status::BadReferenceLocalOnly;
    case QOpcUa::BadNoDeleteRights:
        return Status::BadNoDeleteRights;
    case QOpcUa::UncertainReferenceNotDeleted:
        return Status::UncertainReferenceNotDeleted;
    case QOpcUa::BadServerIndexInvalid:
        return Status::BadServerIndexInvalid;
    case QOpcUa::BadViewIdUnknown:
        return Status::BadViewIdUnknown;
    case QOpcUa::BadViewTimestampInvalid:
        return Status::BadViewTimestampInvalid;
    case QOpcUa::BadViewParameterMismatch:
        return Status::BadViewParameterMismatch;
    case QOpcUa::BadViewVersionInvalid:
        return Status::BadViewVersionInvalid;
    case QOpcUa::UncertainNotAllNodesAvailable:
        return Status::UncertainNotAllNodesAvailable;
    case QOpcUa::GoodResultsMayBeIncomplete:
        return Status::GoodResultsMayBeIncomplete;
    case QOpcUa::BadNotTypeDefinition:
        return Status::BadNotTypeDefinition;
    case QOpcUa::UncertainReferenceOutOfServer:
        return Status::UncertainReferenceOutOfServer;
    case QOpcUa::BadTooManyMatches:
        return Status::BadTooManyMatches;
    case QOpcUa::BadQueryTooComplex:
        return Status::BadQueryTooComplex;
    case QOpcUa::BadNoMatch:
        return Status::BadNoMatch;
    case QOpcUa::BadMaxAgeInvalid:
        return Status::BadMaxAgeInvalid;
    case QOpcUa::BadSecurityModeInsufficient:
        return Status::BadSecurityModeInsufficient;
    case QOpcUa::BadHistoryOperationInvalid:
        return Status::BadHistoryOperationInvalid;
    case QOpcUa::BadHistoryOperationUnsupported:
        return Status::BadHistoryOperationUnsupported;
    case QOpcUa::BadInvalidTimestampArgument:
        return Status::BadInvalidTimestampArgument;
    case QOpcUa::BadWriteNotSupported:
        return Status::BadWriteNotSupported;
    case QOpcUa::BadTypeMismatch:
        return Status::BadTypeMismatch;
    case QOpcUa::BadMethodInvalid:
        return Status::BadMethodInvalid;
    case QOpcUa::BadArgumentsMissing:
        return Status::BadArgumentsMissing;
    case QOpcUa::BadTooManySubscriptions:
        return Status::BadTooManySubscriptions;
    case QOpcUa::BadTooManyPublishRequests:
        return Status::BadTooManyPublishRequests;
    case QOpcUa::BadNoSubscription:
        return Status::BadNoSubscription;
    case QOpcUa::BadSequenceNumberUnknown:
        return Status::BadSequenceNumberUnknown;
    case QOpcUa::BadMessageNotAvailable:
        return Status::BadMessageNotAvailable;
    case QOpcUa::BadInsufficientClientProfile:
        return Status::BadInsufficientClientProfile;
    case QOpcUa::BadStateNotActive:
        return Status::BadStateNotActive;
    case QOpcUa::BadTcpServerTooBusy:
        return Status::BadTcpServerTooBusy;
    case QOpcUa::BadTcpMessageTypeInvalid:
        return Status::BadTcpMessageTypeInvalid;
    case QOpcUa::BadTcpSecureChannelUnknown:
        return Status::BadTcpSecureChannelUnknown;
    case QOpcUa::BadTcpMessageTooLarge:
        return Status::BadTcpMessageTooLarge;
    case QOpcUa::BadTcpNotEnoughResources:
        return Status::BadTcpNotEnoughResources;
    case QOpcUa::BadTcpInternalError:
        return Status::BadTcpInternalError;
    case QOpcUa::BadTcpEndpointUrlInvalid:
        return Status::BadTcpEndpointUrlInvalid;
    case QOpcUa::BadRequestInterrupted:
        return Status::BadRequestInterrupted;
    case QOpcUa::BadRequestTimeout:
        return Status::BadRequestTimeout;
    case QOpcUa::BadSecureChannelClosed:
        return Status::BadSecureChannelClosed;
    case QOpcUa::BadSecureChannelTokenUnknown:
        return Status::BadSecureChannelTokenUnknown;
    case QOpcUa::BadSequenceNumberInvalid:
        return Status::BadSequenceNumberInvalid;
    case QOpcUa::BadProtocolVersionUnsupported:
        return Status::BadProtocolVersionUnsupported;
    case QOpcUa::BadConfigurationError:
        return Status::BadConfigurationError;
    case QOpcUa::BadNotConnected:
        return Status::BadNotConnected;
    case QOpcUa::BadDeviceFailure:
        return Status::BadDeviceFailure;
    case QOpcUa::BadSensorFailure:
        return Status::BadSensorFailure;
    case QOpcUa::BadOutOfService:
        return Status::BadOutOfService;
    case QOpcUa::BadDeadbandFilterInvalid:
        return Status::BadDeadbandFilterInvalid;
    case QOpcUa::UncertainNoCommunicationLastUsableValue:
        return Status::UncertainNoCommunicationLastUsableValue;
    case QOpcUa::UncertainLastUsableValue:
        return Status::UncertainLastUsableValue;
    case QOpcUa::UncertainSubstituteValue:
        return Status::UncertainSubstituteValue;
    case QOpcUa::UncertainInitialValue:
        return Status::UncertainInitialValue;
    case QOpcUa::UncertainSensorNotAccurate:
        return Status::UncertainSensorNotAccurate;
    case QOpcUa::UncertainEngineeringUnitsExceeded:
        return Status::UncertainEngineeringUnitsExceeded;
    case QOpcUa::UncertainSubNormal:
        return Status::UncertainSubNormal;
    case QOpcUa::GoodLocalOverride:
        return Status::GoodLocalOverride;
    case QOpcUa::BadRefreshInProgress:
        return Status::BadRefreshInProgress;
    case QOpcUa::BadConditionAlreadyDisabled:
        return Status::BadConditionAlreadyDisabled;
    case QOpcUa::BadConditionAlreadyEnabled:
        return Status::BadConditionAlreadyEnabled;
    case QOpcUa::BadConditionDisabled:
        return Status::BadConditionDisabled;
    case QOpcUa::BadEventIdUnknown:
        return Status::BadEventIdUnknown;
    case QOpcUa::BadEventNotAcknowledgeable:
        return Status::BadEventNotAcknowledgeable;
    case QOpcUa::BadDialogNotActive:
        return Status::BadDialogNotActive;
    case QOpcUa::BadDialogResponseInvalid:
        return Status::BadDialogResponseInvalid;
    case QOpcUa::BadConditionBranchAlreadyAcked:
        return Status::BadConditionBranchAlreadyAcked;
    case QOpcUa::BadConditionBranchAlreadyConfirmed:
        return Status::BadConditionBranchAlreadyConfirmed;
    case QOpcUa::BadConditionAlreadyShelved:
        return Status::BadConditionAlreadyShelved;
    case QOpcUa::BadConditionNotShelved:
        return Status::BadConditionNotShelved;
    case QOpcUa::BadShelvingTimeOutOfRange:
        return Status::BadShelvingTimeOutOfRange;
    case QOpcUa::BadNoData:
        return Status::BadNoData;
    case QOpcUa::BadBoundNotFound:
        return Status::BadBoundNotFound;
    case QOpcUa::BadBoundNotSupported:
        return Status::BadBoundNotSupported;
    case QOpcUa::BadDataLost:
        return Status::BadDataLost;
    case QOpcUa::BadDataUnavailable:
        return Status::BadDataUnavailable;
    case QOpcUa::BadEntryExists:
        return Status::BadEntryExists;
    case QOpcUa::BadNoEntryExists:
        return Status::BadNoEntryExists;
    case QOpcUa::BadTimestampNotSupported:
        return Status::BadTimestampNotSupported;
    case QOpcUa::GoodEntryInserted:
        return Status::GoodEntryInserted;
    case QOpcUa::GoodEntryReplaced:
        return Status::GoodEntryReplaced;
    case QOpcUa::UncertainDataSubNormal:
        return Status::UncertainDataSubNormal;
    case QOpcUa::GoodNoData:
        return Status::GoodNoData;
    case QOpcUa::GoodMoreData:
        return Status::GoodMoreData;
    case QOpcUa::BadAggregateListMismatch:
        return Status::BadAggregateListMismatch;
    case QOpcUa::BadAggregateNotSupported:
        return Status::BadAggregateNotSupported;
    case QOpcUa::BadAggregateInvalidInputs:
        return Status::BadAggregateInvalidInputs;
    case QOpcUa::BadAggregateConfigurationRejected:
        return Status::BadAggregateConfigurationRejected;
    case QOpcUa::GoodDataIgnored:
        return Status::GoodDataIgnored;
    case QOpcUa::BadRequestNotAllowed:
        return Status::BadRequestNotAllowed;
    case QOpcUa::GoodEdited:
        return Status::GoodEdited;
    case QOpcUa::GoodPostActionFailed:
        return Status::GoodPostActionFailed;
    case QOpcUa::UncertainDominantValueChanged:
        return Status::UncertainDominantValueChanged;
    case QOpcUa::GoodDependentValueChanged:
        return Status::GoodDependentValueChanged;
    case QOpcUa::BadDominantValueChanged:
        return Status::BadDominantValueChanged;
    case QOpcUa::UncertainDependentValueChanged:
        return Status::UncertainDependentValueChanged;
    case QOpcUa::BadDependentValueChanged:
        return Status::BadDependentValueChanged;
    case QOpcUa::GoodCommunicationEvent:
        return Status::GoodCommunicationEvent;
    case QOpcUa::GoodShutdownEvent:
        return Status::GoodShutdownEvent;
    case QOpcUa::GoodCallAgain:
        return Status::GoodCallAgain;
    case QOpcUa::GoodNonCriticalTimeout:
        return Status::GoodNonCriticalTimeout;
    case QOpcUa::BadInvalidArgument:
        return Status::BadInvalidArgument;
    case QOpcUa::BadConnectionRejected:
        return Status::BadConnectionRejected;
    case QOpcUa::BadDisconnect:
        return Status::BadDisconnect;
    case QOpcUa::BadConnectionClosed:
        return Status::BadConnectionClosed;
    case QOpcUa::BadInvalidState:
        return Status::BadInvalidState;
    case QOpcUa::BadEndOfStream:
        return Status::BadEndOfStream;
    case QOpcUa::BadNoDataAvailable:
        return Status::BadNoDataAvailable;
    case QOpcUa::BadWaitingForResponse:
        return Status::BadWaitingForResponse;
    case QOpcUa::BadOperationAbandoned:
        return Status::BadOperationAbandoned;
    case QOpcUa::BadExpectedStreamToBlock:
        return Status::BadExpectedStreamToBlock;
    case QOpcUa::BadWouldBlock:
        return Status::BadWouldBlock;
    case QOpcUa::BadSyntaxError:
        return Status::BadSyntaxError;
    case QOpcUa::BadMaxConnectionsReached:
        return Status::BadMaxConnectionsReached;
    }
    return Status::BadUnexpectedError;
}

QT_END_NAMESPACE


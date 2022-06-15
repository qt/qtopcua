// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuaclient.h"
#include "qopcuanode.h"
#include <private/qopcuaclient_p.h>
#include <private/qopcuaclientimpl_p.h>
#include <private/qopcuanode_p.h>
#include <private/qopcuanodeimpl_p.h>

#include "qopcuarelativepathelement.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaNode
    \inmodule QtOpcUa

    \brief QOpcUaNode allows interaction with an OPC UA node.


    The node is the basic building block of the OPC UA address space.
    It has attributes like browse name, value, associated properties and can have
    references to other nodes in the address space.
    Nodes are organized in namespaces and have IDs which can e.g. be numeric,
    a string, a namespace-specific format (opaque) or a globally unique identifier.
    A node is identified by the namespace ID and the node ID.
    This identifier is usually given as a string:
    The identifier of a node residing in namespace 0 and having the numeric
    identifier 42 results in the string \c ns=0;i=42. A node with a string
    identifier can be addressed via \c ns=0;s=myStringIdentifier.

    Objects of this type are owned by the user and must be deleted when they are
    no longer needed. They are valid as long as the \l QOpcUaClient which created them exists.

    \section1 Reading and writing of attributes

    The node attributes are read from the server when \l readAttributes() or \l readAttributeRange()
    is called. The results are cached locally and can be retrieved using \l attribute()
    after the \l attributeRead signal has been received.

    Attributes can be written using \l writeAttribute(), \l writeAttributes() and \l writeAttributeRange()
    if the user has the necessary rights.
    Success of the write operation is reported using the \l attributeWritten signal.

    \l attributeError() contains a status code associated with the last read or write operation
    on the attribute. This is the low level status code returned by the OPC UA service.
    This status code can be simplified by converting it to a \l QOpcUa::ErrorCategory using
    \l QOpcUa::errorCategory().

    \section1 Subscriptions and monitored items
    Subscriptions are a concept in OPC UA which allows receiving of notifications for changes in data
    or in case of events instead of continuously polling a node for changes.
    Monitored items define how attributes of a node are watched for changes. They are added to a
    subscription and any notifications they generate are forwarded to the user via the subscription.
    The interval of the updates as well as many other options of the monitored items and subscriptions
    can be configured by the user.

    \l QOpcUaNode offers an abstraction to interact with subscriptions and monitored items.
    \l enableMonitoring() enables data change notifications for one or more attributes.
    The \l dataChangeOccurred signal contains new values and the local cache is updated.
    \l disableMonitoring() disables the data change notifications.
    The \l monitoringStatusChanged signal notifies about changes of the monitoring status, e. g. after
    manual enable and disable or a status change on the server.

    Event monitoring uses the same API for setup and life cycle management.
    The \l {QOpcUa::NodeAttribute} {EventNotifier} attribute
    must be monitored using an \l {QOpcUaMonitoringParameters::EventFilter} {EventFilter} which selects
    the required event fields and filters the reported events by user defined criteria. The events are
    reported in the \l eventOccurred() signal as a \l QVariantList which contains the values of the selected
    event fields.

    Settings of the subscription and monitored item can be modified at runtime using \l modifyMonitoring().

    \section1 Browsing the address space
    The OPC UA address space consists of nodes connected by references.
    \l browseChildren follows these references in forward direction and returns attributes from all
    nodes connected to the node behind an instance of \l QOpcUaNode in the \l browseFinished signal.
    \l browse() is similar to \l browseChildren() but offers more options to configure the browse call.

    \section1 Method calls
    OPC UA specifies methods on the server which can be called by the user.
    \l QOpcUaNode supports this via \l callMethod which takes parameters and returns the results of
    the call in the \l methodCallFinished signal.

    \section1 Resolving browse paths
    To support programming against a type description, OPC UA supports the resolution of a path of browse names
    starting from a certain node to obtain the node id of the target node. The \l resolveBrowsePath() method
    follows a path starting from the node it was called on and returns the result in the
    \l resolveBrowsePathFinished() signal.

    \section1 Example
    For connecting the client to a server and getting a \l QOpcUaNode object, see \l QOpcUaClient.

    After the node has been successfully created, the BrowseName of the root node is read from the server:

    \code
    QOpcUaNode *rootNode; // Created before, see QOpcUaClient documentation.
    // Connect to the attributeRead signal. Compatible slots of QObjects can be used instead of a lambda.
    QObject::connect(rootNode, &QOpcUaNode::attributeRead, [rootNode, client](QOpcUa::NodeAttributes attr) {
        qDebug() << "Signal for attributes:" << attr;
        if (rootNode->attributeError(QOpcUa::NodeAttribute::BrowseName) != QOpcUa::UaStatusCode::Good) {
            qDebug() << "Failed to read attribute:" << rootNode->attributeError(QOpcUa::NodeAttribute::BrowseName);
            client->disconnectFromEndpoint();
        }
        qDebug() << "Browse name:" << rootNode->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>().name();
    });
    rootNode->readAttributes(QOpcUa::NodeAttribute::BrowseName); // Start a read operation for the node's BrowseName attribute.
    \endcode
*/

/*!
    \typedef QOpcUaNode::AttributeMap

    This type is used by \l writeAttributes() to write more than one attribute at a time.
    QVariant values must be assigned to the attributes to be written.
*/

/*!
    \fn void QOpcUaNode::attributeRead(QOpcUa::NodeAttributes attributes)

    This signal is emitted after a \l readAttributes() or \l readAttributeRange() operation has finished.
    The receiver has to check the status code for the attributes contained in \a attributes.
*/

/*!
    \fn void QOpcUaNode::attributeWritten(QOpcUa::NodeAttribute attribute, QOpcUa::UaStatusCode statusCode)

    This signal is emitted after a \l writeAttribute(), \l writeAttributes() or \l writeAttributeRange()
    operation has finished.

    Before this signal is emitted, the attribute cache is updated in case of a successful write.
    For \l writeAttributes() a signal is emitted for each attribute in the write call.
    \a statusCode contains the success information for the write operation on \a attribute.
*/

/*!
    \fn void QOpcUaNode::dataChangeOccurred(QOpcUa::NodeAttribute attr, QVariant value)

    This signal is emitted after a data change notification has been received. \a value contains the
    new value for the node attribute \a attr.

    \sa attribute() serverTimestamp() sourceTimestamp()
*/

/*!
    \fn void QOpcUaNode::attributeUpdated(QOpcUa::NodeAttribute attr, QVariant value)

    This signal is emitted after the value in the attribute cache has been updated by a
    data change notification from the server, a read or a write operation. \a value contains the
    new value for the node attribute \a attr.

    \sa attribute() attributeError() serverTimestamp() sourceTimestamp()
*/

/*!
    \fn void QOpcUaNode::enableMonitoringFinished(QOpcUa::NodeAttribute attr, QOpcUa::UaStatusCode statusCode)

    This signal is emitted after an asynchronous call to \l enableMonitoring() has finished.
    After this signal has been emitted, \l monitoringStatus() returns valid information for \a attr.
    \a statusCode contains the status code for the operation.
*/

/*!
    \fn void QOpcUaNode::disableMonitoringFinished(QOpcUa::NodeAttribute attr, QOpcUa::UaStatusCode statusCode)

    This signal is emitted after an asynchronous call to \l disableMonitoring() has finished. \a statusCode contains
    the status code generated by the operation.
    After this signal has been emitted, monitoringStatus returns a default constructed value with
    status code BadMonitoredItemIdIinvalid for \a attr.
*/

/*!
    \fn void QOpcUaNode::monitoringStatusChanged(QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameters items, QOpcUa::UaStatusCode statusCode);

    This signal is emitted after an asynchronous call to \l modifyMonitoring() has finished.
    The node attribute for which the operation was requested is returned in \a attr. \a items contains the parameters that have been modified.
    \a statusCode contains the result of the modify operation on the server.
*/

/*!
    \fn void QOpcUaNode::methodCallFinished(QString methodNodeId, QVariant result, QOpcUa::UaStatusCode statusCode)

    This signal is emitted after a method call for \a methodNodeId has finished on the server.
    \a statusCode contains the status code from the method call, \a result contains the output
    arguments of the method. \a result is empty if the method has no output arguments or \a statusCode
    is not \l {QOpcUa::UaStatusCode} {Good}.
    The \a result variant is either a single value if there is only one output argument or it contains a list of
    variants in case the called function returned multiple output arguments.
    \code
        if (result.canConvert<QVariantList>()) {
            // handle list type
        } else {
            // handle value type
        }
    \endcode
*/

/*!
    \fn void QOpcUaNode::browseFinished(QList<QOpcUaReferenceDescription> children, QOpcUa::UaStatusCode statusCode)

    This signal is emitted after a \l browseChildren() or \l browse() operation has finished.

    \a children contains information about all nodes which matched the criteria in \l browseChildren().
    \a statusCode contains the service result of the browse operation. If \a statusCode is not \l {QOpcUa::UaStatusCode} {Good},
    the passed \a children vector is empty.
    \sa QOpcUaReferenceDescription
*/

/*!
    \fn void QOpcUaNode::resolveBrowsePathFinished(QList<QOpcUaBrowsePathTarget> targets, QList<QOpcUaRelativePathElement> path, QOpcUa::UaStatusCode statusCode)

    This signal is emitted after a \l resolveBrowsePath() call has finished.

    \l QOpcUaBrowsePathTarget \a targets contains the matches, \a statusCode is the status code of the operation.
    If \a statusCode is not \l {QOpcUa::UaStatusCode} {Good}, \a targets is empty.
    The browse path \a path is the browse path from the request. It can be used to associate results with requests.
*/

/*!
    \fn void QOpcUaNode::eventOccurred(QVariantList eventFields)

    This signal is emitted after a new event has been received.

    \a eventFields contains the values of the event fields in the order specified in the \c select clause of the event filter.
*/

/*!
    \fn QOpcUa::NodeAttributes QOpcUaNode::mandatoryBaseAttributes()

    Contains all mandatory attributes of the OPC UA base node class.
*/

/*!
    \fn QOpcUa::NodeAttributes QOpcUaNode::allBaseAttributes()

    Contains all attributes of the OPC UA base node class.
*/

/*!
    \internal QOpcUaNodeImpl is an opaque type (as seen from the public API).
    This prevents users of the public API to use this constructor (even though
    it is public).
*/
QOpcUaNode::QOpcUaNode(QOpcUaNodeImpl *impl, QOpcUaClient *client, QObject *parent)
    : QObject(*new QOpcUaNodePrivate(impl, client), parent)
{
}

QOpcUaNode::~QOpcUaNode()
{
}

/*!
    Starts an asynchronous read operation for the node attribute \a attribute.
    \a indexRange is a string which can be used to select a part of an array. It is defined in OPC-UA part 4, 7.22.
    The first element in an array is 0, "1" returns the second element, "0:9" returns the first 10 elements,
    "0,1" returns the second element of the first row in a two-dimensional array.

    Returns \c true if the asynchronous call has been successfully dispatched.

    Attribute values only contain valid information after the \l attributeRead signal has been emitted.
*/
bool QOpcUaNode::readAttributeRange(QOpcUa::NodeAttribute attribute, const QString &indexRange)
{
    Q_D(QOpcUaNode);
    if (d->m_client.isNull() || d->m_client->state() != QOpcUaClient::Connected)
        return false;

    return d->m_impl->readAttributes(QOpcUa::NodeAttributes() | attribute, indexRange);
}

/*!
    Starts an asynchronous read operation for the node's Value attribute.

    Returns \c true if the asynchronous call has been successfully dispatched.

    \sa readAttributes()
*/
bool QOpcUaNode::readValueAttribute()
{
    return readAttributes(QOpcUa::NodeAttribute::Value);
}

/*!
    Starts an asynchronous read operation for the node attributes in \a attributes.

    Returns \c true if the asynchronous call has been successfully dispatched.

    Attribute values only contain valid information after the \l attributeRead signal has been emitted.
*/
bool QOpcUaNode::readAttributes(QOpcUa::NodeAttributes attributes)
{
    Q_D(QOpcUaNode);
    if (d->m_client.isNull() || d->m_client->state() != QOpcUaClient::Connected)
        return false;

    return d->m_impl->readAttributes(attributes, QString());
}

/*!
    Returns the value of the attribute given in \a attribute.

    The value is only valid after the \l attributeRead signal has been emitted.
    An empty QVariant is returned if there is no cached value for the attribute.
*/
QVariant QOpcUaNode::attribute(QOpcUa::NodeAttribute attribute) const
{
    Q_D(const QOpcUaNode);
    auto it = d->m_nodeAttributes.constFind(attribute);
    if (it == d->m_nodeAttributes.constEnd())
        return QVariant();

    return it->value();
}

/*!
    Returns the value of the node's Value attribute.

    The returned value is only valid after the Value attribute has been successfully read or written
    or after a data change from a monitoring has updated the attribute cache.
    This is indicated by a \l attributeRead() or \l attributeWritten() signal with status code
    \l {QOpcUa::UaStatusCode} {Good} or a \l dataChangeOccurred() signal for the Value attribute.

    If there is no value in the attribute cache, an invalid \l QVariant is returned.

    \sa readValueAttribute()  writeValueAttribute() valueAttributeError()
*/
QVariant QOpcUaNode::valueAttribute() const
{
    return attribute(QOpcUa::NodeAttribute::Value);
}

/*!
    Returns the error code for the attribute given in \a attribute.

    The error code is only valid after the \l attributeRead or \l attributeWritten signal has been emitted.

    If there is no entry in the attribute cache, \l {QOpcUa::UaStatusCode} {BadNoEntryExists} is returned.

    \sa QOpcUa::errorCategory
 */
QOpcUa::UaStatusCode QOpcUaNode::attributeError(QOpcUa::NodeAttribute attribute) const
{
    Q_D(const QOpcUaNode);
    auto it = d->m_nodeAttributes.constFind(attribute);
    if (it == d->m_nodeAttributes.constEnd())
        return QOpcUa::UaStatusCode::BadNoEntryExists;

    return it->statusCode();
}

/*!
    Returns the error code for the node's Value attribute.
    The status code \l {QOpcUa::UaStatusCode} {Good} indicates a valid return value for \l valueAttribute().
    If there is no entry in the attribute cache, \l {QOpcUa::UaStatusCode} {BadNoEntryExists} is returned.
*/
QOpcUa::UaStatusCode QOpcUaNode::valueAttributeError() const
{
    return attributeError(QOpcUa::NodeAttribute::Value);
}

/*!
    Returns the source timestamp from the last read or data change of \a attribute.
    Before at least one \l attributeRead or \l dataChangeOccurred signal has been emitted,
    a null datetime is returned.

*/
QDateTime QOpcUaNode::sourceTimestamp(QOpcUa::NodeAttribute attribute) const
{
    Q_D(const QOpcUaNode);
    auto it = d->m_nodeAttributes.constFind(attribute);
    if (it == d->m_nodeAttributes.constEnd())
        return QDateTime();

    return it->sourceTimestamp();
}

/*!
    Returns the server timestamp from the last read or data change of \a attribute.
    Before at least one \l attributeRead or \l dataChangeOccurred signal has been emitted,
    a null datetime is returned.
*/
QDateTime QOpcUaNode::serverTimestamp(QOpcUa::NodeAttribute attribute) const
{
    Q_D(const QOpcUaNode);
    auto it = d->m_nodeAttributes.constFind(attribute);
    if (it == d->m_nodeAttributes.constEnd())
        return QDateTime();

    return it->serverTimestamp();
}

/*!
    This method creates a monitored item for each of the attributes given in \a attr.
    The settings from \a settings are used in the creation of the monitored items and the subscription.

    Returns \c true if the asynchronous call has been successfully dispatched.

    On completion of the call, the \l enableMonitoringFinished signal is emitted.
    There are multiple error cases in which a bad status code is generated: A subscription with the subscription id specified in \a settings does not exist,
    the node does not exist on the server, the node does not have the requested attribute or the maximum number of monitored items for
    the server is reached.

    The same method is used to enable event monitoring. Events are special objects in the OPC UA address space which contain information
    about an event that has occurred. If an event is triggered on the server, an event monitored item collects selected values of
    node attributes of the event object and its child nodes.
    Every node that has an event source can be monitored for events.
    To monitor a node for events, the attribute \l {QOpcUa::NodeAttribute} {EventNotifier} must be monitored using an EventFilter which contains the event fields
    the user needs and optionally a where clause which is used to filter events by criteria (for more details, see \l QOpcUaMonitoringParameters::EventFilter).

 */
bool QOpcUaNode::enableMonitoring(QOpcUa::NodeAttributes attr, const QOpcUaMonitoringParameters &settings)
{
    Q_D(QOpcUaNode);
    if (d->m_client.isNull() || d->m_client->state() != QOpcUaClient::Connected)
        return false;

    return d->m_impl->enableMonitoring(attr, settings);
}

/*!
    This method modifies settings of the monitored item or the subscription.
    The parameter \a item of the monitored item or subscription associated with \a attr is attempted to set to \a value.

    Returns \c true if the asynchronous call has been successfully dispatched.

    After the call has finished, the \l monitoringStatusChanged signal is emitted. This signal contains the modified parameters and the status code.
    A bad status code is generated if there is no monitored item associated with the requested attribute, modifying the requested
    parameter is not implemented or if the server has rejected the requested value.
*/
bool QOpcUaNode::modifyMonitoring(QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, const QVariant &value)
{
    Q_D(QOpcUaNode);
    if (d->m_client.isNull() || d->m_client->state() != QOpcUaClient::Connected)
        return false;

    return d->m_impl->modifyMonitoring(attr, item, value);
}

/*!
    Returns the monitoring parameters associated with the attribute \a attr. This can be used to check the success of \l enableMonitoring()
    or if parameters have been revised.
    The returned values are only valid after \l enableMonitoringFinished or \l monitoringStatusChanged have been emitted
    for \a attr. If the status is queried before a signal has been emitted, \l QOpcUaMonitoringParameters::statusCode()
    returns \l {QOpcUa::UaStatusCode} {BadNoEntryExists}.
*/
QOpcUaMonitoringParameters QOpcUaNode::monitoringStatus(QOpcUa::NodeAttribute attr)
{
    Q_D(QOpcUaNode);
    auto it = d->m_monitoringStatus.constFind(attr);
    if (it == d->m_monitoringStatus.constEnd()) {
        QOpcUaMonitoringParameters p;
        p.setStatusCode(QOpcUa::UaStatusCode::BadNoEntryExists);
        return p;
    }

    return *it;
}

/*!
    Modifies an existing event monitoring to use \a eventFilter as event filter.

    Returns \c true if the filter modification request has been successfully dispatched to the backend.

    \l monitoringStatusChanged for \l {QOpcUa::NodeAttribute} {EventNotifier} is emitted after the operation has finished.
*/
bool QOpcUaNode::modifyEventFilter(const QOpcUaMonitoringParameters::EventFilter &eventFilter)
{
    return modifyMonitoring(QOpcUa::NodeAttribute::EventNotifier, QOpcUaMonitoringParameters::Parameter::Filter, QVariant::fromValue(eventFilter));
}

/*!
    Modifies an existing data change monitoring to use \a filter as data change filter.

    Returns \c true if the filter modification request has been successfully dispatched to the backend.

    \l monitoringStatusChanged for \a attr is emitted after the operation has finished.
*/
bool QOpcUaNode::modifyDataChangeFilter(QOpcUa::NodeAttribute attr, const QOpcUaMonitoringParameters::DataChangeFilter &filter)
{
    return modifyMonitoring(attr, QOpcUaMonitoringParameters::Parameter::Filter, QVariant::fromValue(filter));
}

/*!
    Writes \a value to the attribute given in \a attribute using the type information from \a type.
    Returns \c true if the asynchronous call has been successfully dispatched.

    If the \a type parameter is omitted, the backend tries to find the correct type. The following default types are assumed:
    \table
        \header
            \li Qt MetaType
            \li OPC UA type
        \row
            \li Bool
            \li Boolean
        \row
            \li UChar
            \li Byte
        \row
            \li Char
            \li SByte
        \row
            \li UShort
            \li UInt16
        \row
            \li Short
            \li Int16
        \row
            \li Int
            \li Int32
        \row
            \li UInt
            \li UInt32
        \row
            \li ULongLong
            \li UInt64
        \row
            \li LongLong
            \li Int64
        \row
            \li Double
            \li Double
        \row
            \li Float
            \li Float
        \row
            \li QString
            \li String
        \row
            \li QDateTime
            \li DateTime
        \row
            \li QByteArray
            \li ByteString
        \row
            \li QUuid
            \li Guid
    \endtable
*/

bool QOpcUaNode::writeAttribute(QOpcUa::NodeAttribute attribute, const QVariant &value, QOpcUa::Types type)
{
    Q_D(QOpcUaNode);
    if (d->m_client.isNull() || d->m_client->state() != QOpcUaClient::Connected)
        return false;

    return d->m_impl->writeAttribute(attribute, value, type, QString());
}

/*!
    Writes \a value to the attribute given in \a attribute using the type information from \a type.
    For \a indexRange, see \l readAttributeRange().

    Returns \c true if the asynchronous call has been successfully dispatched.
*/
bool QOpcUaNode::writeAttributeRange(QOpcUa::NodeAttribute attribute, const QVariant &value, const QString &indexRange, QOpcUa::Types type)
{
    Q_D(QOpcUaNode);
    if (d->m_client.isNull() || d->m_client->state() != QOpcUaClient::Connected)
        return false;

    return d->m_impl->writeAttribute(attribute, value, type, indexRange);
}

/*!
    Executes a write operation for the attributes and values specified in \a toWrite.

    Returns \c true if the asynchronous call has been successfully dispatched.

    The \a valueAttributeType parameter can be used to supply type information for the value attribute.
    All other attributes have known types.
    \sa writeAttribute()
*/
bool QOpcUaNode::writeAttributes(const AttributeMap &toWrite, QOpcUa::Types valueAttributeType)
{
    Q_D(QOpcUaNode);
    if (d->m_client.isNull() || d->m_client->state() != QOpcUaClient::Connected)
        return false;

    return d->m_impl->writeAttributes(toWrite, valueAttributeType);
}

/*!
    Writes \a value to the node's Value attribute using the type information from \a type.

    Returns \c true if the asynchronous call has been successfully dispatched.

    \sa writeAttribute()
*/
bool QOpcUaNode::writeValueAttribute(const QVariant &value, QOpcUa::Types type)
{
    return writeAttribute(QOpcUa::NodeAttribute::Value, value, type);
}

/*!
    This method disables monitoring for the attributes given in \a attr.

    Returns \c true if the asynchronous call has been successfully dispatched.

    After the call is finished, the \l disableMonitoringFinished signal is emitted and monitoringStatus returns a default constructed value with
    status code BadMonitoredItemIdIinvalid for \a attr.
*/
bool QOpcUaNode::disableMonitoring(QOpcUa::NodeAttributes attr)
{
  Q_D(QOpcUaNode);
  if (d->m_client.isNull() || d->m_client->state() != QOpcUaClient::Connected)
      return false;

  return d->m_impl->disableMonitoring(attr);
}

/*!
    Executes a forward browse call starting from the node this method is called on.
    The browse operation collects information about child nodes connected to the node
    and delivers the results in the \l browseFinished() signal.

    Returns \c true if the asynchronous call has been successfully dispatched.

    To request only children connected to the node by a certain type of reference, \a referenceType must be set to that reference type.
    For example, this can be  used to get all properties of a node by passing \l {QOpcUa::ReferenceTypeId} {HasProperty} in \a referenceType.
    The results can be filtered to contain only nodes with certain node classes by setting them in \a nodeClassMask.
*/
bool QOpcUaNode::browseChildren(QOpcUa::ReferenceTypeId referenceType, QOpcUa::NodeClasses nodeClassMask)
{
    Q_D(QOpcUaNode);
    if (d->m_client.isNull() || d->m_client->state() != QOpcUaClient::Connected)
        return false;

    QOpcUaBrowseRequest request;
    request.setReferenceTypeId(referenceType);
    request.setNodeClassMask(nodeClassMask);
    request.setBrowseDirection(QOpcUaBrowseRequest::BrowseDirection::Forward);
    request.setIncludeSubtypes(true);
    return d->m_impl->browse(request);
}

/*!
    Returns the ID of the OPC UA node.
*/
QString QOpcUaNode::nodeId() const
{
    Q_D(const QOpcUaNode);
    if (d->m_client.isNull() || d->m_client->state() != QOpcUaClient::Connected)
        return QString();

    return d->m_impl->nodeId();
}

/*!
    Returns a pointer to the client that has created this node.
*/
QOpcUaClient *QOpcUaNode::client() const
{
    Q_D(const QOpcUaNode);
    return d->m_client.data();
}

/*!
    Calls the OPC UA method \a methodNodeId with the parameters given via \a args. The result is
    returned in the \l methodCallFinished signal.

    Returns \c true if the asynchronous call has been successfully dispatched.
*/
bool QOpcUaNode::callMethod(const QString &methodNodeId, const QList<QOpcUa::TypedVariant> &args)
{
    Q_D(QOpcUaNode);
    if (d->m_client.isNull() || d->m_client->state() != QOpcUaClient::Connected)
        return false;

    return d->m_impl->callMethod(methodNodeId, args);
}

/*!
    Resolves the browse path \a path to one or more node ids starting from this node
    using the TranslateBrowsePathsToNodeIds service specified in OPC-UA part 4, 5.8.4.

    Returns \c true if the asynchronous call has been successfully dispatched.

    TranslateBrowsePathsToNodeIds is mainly used to program against type definitions instead of a concrete set of
    nodes in the OPC UA address space.
    For example, a type definition for a machine model could consist of a starting node with browse name "Machine"
    which has a component with browse name "Fan". Fan has a component with browse name "RPM" which is a Variable node
    holding the current RPM value of the fan. There are multiple machines of that type and each of these machines is
    mapped into the OPC UA address space as an object of the machine type.
    For each of these machine objects, the path from the machine node to the "RPM" node is the same. If a client wants
    to read the current RPM value, it needs to call \l resolveBrowsePath() with the machine node as starting node
    and the browse path from the machine to the "RPM" node:

    \code
    QScopedPointer<QOpcUaNode> node(opcuaClient->node("ns=1;s=machine1"));

    QList<QOpcUaRelativePathElement> path;
    path.append(QOpcUaRelativePathElement(QOpcUaQualifiedName(1, "Fan"), QOpcUa::ReferenceTypeId::HasComponent));
    path.append(QOpcUaRelativePathElement(QOpcUaQualifiedName(1, "RPM"), QOpcUa::ReferenceTypeId::HasComponent));
    node->resolveBrowsePath(path);
    \endcode

    The result returned in \l resolveBrowsePathFinished() contains the node id of the "RPM" node which can be
    used to access the node's attributes:

    \code
    if (!results.size()) {
        qWarning() << "Browse path resolution failed";
        return;
    }

    if (results.at(0).isFullyResolved()) {
       QOpcUaNode *rpmNode = client->node(results.at(0).targetId());
       if (!rpmNode) {
           qWarning() << "Failed to create node";
           return;
       }
       // Connect slots, call methods
    } else {
        qWarning() << "Browse path could not be fully resolved, the target node is on another server";
        return;
    }
    \endcode
*/
bool QOpcUaNode::resolveBrowsePath(const QList<QOpcUaRelativePathElement> &path)
{
    Q_D(QOpcUaNode);
    if (d->m_client.isNull() || d->m_client->state() != QOpcUaClient::Connected)
        return 0;

    return d->m_impl->resolveBrowsePath(path);
}

/*!
    Starts a browse call from this node.

    Returns \c true if the asynchronous call has been successfully dispatched.

    All references matching the criteria specified in \a request are returned in the \l browseFinished() signal.

    For example, an inverse browse call can be used to find the parent node of a property node:
    \code
    QOpcUaBrowseRequest request;
    request.setBrowseDirection(QOpcUaBrowseRequest::BrowseDirection::Inverse);
    request.setReferenceTypeId(QOpcUa::ReferenceTypeId::HasProperty);
    propertyNode->browse(request);
    \endcode
*/
bool QOpcUaNode::browse(const QOpcUaBrowseRequest &request)
{
  Q_D(QOpcUaNode);
  if (d->m_client.isNull() || d->m_client->state() != QOpcUaClient::Connected)
      return false;

  return d->m_impl->browse(request);
}

/*!
    Starts a read history request for this node. This is the Qt OPC UA representation for the OPC UA
    ReadHistory service for reading raw historical data defined in
    \l {https://reference.opcfoundation.org/v104/Core/docs/Part4/5.10.3/} {OPC-UA part 4, 5.10.3}.
    It reads the history based on the parementers, \a startTime, \a endTime,
    \a numValues, and \a returnBounds.

    Returns a \l QOpcUaHistoryReadResponse which contains the state of the request if the asynchronous
    request has been successfully dispatched. The results are returned in the
    \l QOpcUaHistoryReadResponse::readHistoryDataFinished(const QList<QOpcUaHistoryData> &results, QOpcUa::UaStatusCode serviceResult)
    signal.

    In the following example, the historic data from the last two days of a node are requested and printed.
    The result is limited to ten values per node.

    \code
        QOpcUaHistoryReadResponse *response = node->readHistoryRaw(QDateTime::currentDateTime(),
                                                                   QDateTime::currentDateTime().addDays(-2),
                                                                   10,
                                                                   true);
        if (response) {
            QObject::connect(response123, &QOpcUaHistoryReadResponse::readHistoryDataFinished,
                             [] (QList<QOpcUaHistoryData> results, QOpcUa::UaStatusCode serviceResult) {
                if (serviceResult != QOpcUa::UaStatusCode::Good) {
                    qWarning() << "Fetching historical data failed with:" << serviceResult;
                } else {
                    for (const auto& result : results) {
                        qInfo() << "NodeId:" << result.nodeId();
                        for (const auto &dataValue : result.result())
                            qInfo() << "Value:" << dataValue.value();
                    }
                }
            });
        }
    \endcode

    \since 6.3
*/
QOpcUaHistoryReadResponse *QOpcUaNode::readHistoryRaw(const QDateTime &startTime, const QDateTime &endTime, quint32 numValues, bool returnBounds)
{
    Q_D(QOpcUaNode);
    if (d->m_client.isNull() || d->m_client->state() != QOpcUaClient::Connected)
        return nullptr;

    return d->m_impl->readHistoryRaw(startTime, endTime, numValues, returnBounds);
}

QDebug operator<<(QDebug dbg, const QOpcUaNode &node)
{
    dbg << "QOpcUaNode {"
        << "DisplayName:" << node.attribute(QOpcUa::NodeAttribute::DisplayName)
        << "Id:" << node.attribute(QOpcUa::NodeAttribute::NodeId)
        << "Class:" << node.attribute(QOpcUa::NodeAttribute::NodeClass)
        << "}";
    return dbg;
}

QT_END_NAMESPACE

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

#include "qopcuaclient.h"
#include "qopcuanode.h"
#include <private/qopcuaclient_p.h>
#include <private/qopcuaclientimpl_p.h>
#include <private/qopcuanode_p.h>
#include <private/qopcuanodeimpl_p.h>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaNode
    \inmodule QtOpcUa

    \brief QOpcUaNode is a base class for plugin specific node objects which\
    allows access to attributes of an OPC UA node.

    The node is the basic building block of the OPC UA address space.
    It has attributes like browse name, value, associated properties and can have
    references to other nodes in the address space.
    Nodes are organized in namespaces and have IDs which can e.g. be numeric,
    a string, a namespace specific format (/opaque) or a globally unique identifier.
    A node is identified by the namespace id and the node ID.
    This identifier is usually given as a string:
    The identifier of a node residing in namespace 0 and having the numeric
    identifier 42, the string is \c ns=0;i=42, a node with a string
    identifier can be addressed via \c ns=0;s=myStringIdentifier.

    The node attributes are read from the server when the getter methods are
    called.

    Objects of this type are owned by the user and must be deleted when they are
    no longer needed.

    \section1 Working with Subscriptions
    Subscriptions are a concept in OPC UA to enable updating values by data
    changes or events instead of polling.
    The subscription has an interval in which it checks for value changes and
    sends updates to the client. For an event monitored item, there is no interval.

    Monitored Items are used to assign the node and the node's attribute or the
    event we want to keep up with to the subscription.

    \image subscriptions.png
*/

/*!
    \typedef QOpcUaNode::AttributeMap

    This type is used by \l writeAttributes() to write more than one attribute at a time.
    QVariant values must be assigned to the attributes to be written.
*/

/*!
    \fn void QOpcUaNode::readFinished(QOpcUa::NodeAttributes attributes)

    This signal is emitted after a \l readAttributes() or \l readAttributeRange() operation has finished.
    The receiver has to check the status code for the attributes contained in \a attributes.
*/

/*!
    \fn QOpcUaNode::attributeWritten(QOpcUa::NodeAttribute attribute, QOpcUa::UaStatusCode statusCode)

    This signal is emitted after a \l writeAttribute(), \l writeAttributes() or \l writeAttributeRange()
    operation has finished.

    Before this signal is emitted, the attribute cache is updated in case of a successful write.
    For \l writeAttributes() a signal is emitted for each attribute in the write call.
*/

/*!
    \fn void enableMonitoringFinished(QOpcUa::NodeAttribute attr, QOpcUa::UaStatusCode statusCode)

    This signal is emitted after an asynchronous call to \l enableMonitoring() has finished.
    After this signal has been emitted, \l monitoringStatus() returns valid information for \a attr.
*/

/*!
    \fn void disableMonitoringFinished(QOpcUa::NodeAttribute attr, QOpcUa::UaStatusCode statusCode)

    This signal is emitted after an asynchronous call to \l disableMonitoring() has finished.
    After this signal has been emitted, monitoringStatus returns a default constructed value with
    status code BadMonitoredItemIdIinvalid for \a attr.
*/

/*!
    \fn void QOpcUaNode::monitoringStatusChanged(QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, QVariant value, QOpcUa::UaStatusCode statusCode);

    This signal is emitted after an asynchronous call to \l modifyMonitoring() has finished.
    The node attribute for which the operation was requested is returned in \a attr. \a item contains the parameters that have been modified.
    \a statusCode contains the result of the modify operation on the server.
*/

/*!
    \fn void methodCallFinished(QString methodNodeId, QVariant result, QOpcUa::UaStatusCode statusCode)

    This signal is emitted after a method call has finished on the server.
    \a statusCode contains the status code from the method call, \a result contains the output
    arguments of the method. \a result is empty if the method has no output arguments or \a statusCode
    is not OpcUa::UaStatusCode::Good.
*/

/*!
    \fn void browseFinished(QVector<QOpcUaReferenceDescription> children, QOpcUa::UaStatusCode statusCode)

    This signal is emitted after a \l browseChildren() operation has finished.

    \a children contains information about all nodes which matched the criteria in \l browseChildren().
    \a statusCode contains the service result of the browse operation. If \a statusCode is not QOpcUa::UaAttributeId::Good,
    \a children is empty.
    \sa QOpcUaReferenceDescription
*/

/*!
    \internal QOpcUaNodeImpl is an opaque type (as seen from the public API).
    This prevents users of the public API to use this constructor (eventhough
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

    Returns true if the asynchronous call has been successfully dispatched.

    Attribute values only contain valid information after the \l readFinished signal has been emitted.
*/
bool QOpcUaNode::readAttributeRange(QOpcUa::NodeAttribute attribute, const QString &indexRange)
{
    if (d_func()->m_client.isNull() || d_func()->m_client->state() != QOpcUaClient::Connected)
        return false;

    return d_func()->m_impl->readAttributes(QOpcUa::NodeAttributes() | attribute, indexRange);
}

/*!
    Starts an asynchronous read operation for the node attributes in \a attributes.
    Returns true if the asynchronous call has been successfully dispatched.

    Attribute values only contain valid information after the \l readFinished signal has been emitted.
*/
bool QOpcUaNode::readAttributes(QOpcUa::NodeAttributes attributes)
{
    if (d_func()->m_client.isNull() || d_func()->m_client->state() != QOpcUaClient::Connected)
        return false;

    return d_func()->m_impl->readAttributes(attributes, QString());
}

/*!
    Returns the value of the attribute given in \a attribute.

    The value is only valid after the \l readFinished signal has been emitted.
    An empty QVariant is returned if there is no cached value for the attribute.
 */
QVariant QOpcUaNode::attribute(QOpcUa::NodeAttribute attribute) const
{
    auto it = d_func()->m_nodeAttributes.constFind(attribute);
    if (it == d_func()->m_nodeAttributes.constEnd())
        return QVariant();

    return it->attribute;
}

/*!
    Returns the error code for the attribute given in \a attribute.

    The error code is only valid after the \l readFinished signal has been emitted.

    \sa QOpcUa::errorCategory
 */
QOpcUa::UaStatusCode QOpcUaNode::attributeError(QOpcUa::NodeAttribute attribute) const
{
    auto it = d_func()->m_nodeAttributes.constFind(attribute);
    if (it == d_func()->m_nodeAttributes.constEnd())
        return QOpcUa::UaStatusCode::BadNotFound;

    return it->statusCode;
}

/*!
    Writes \a value to the attribute given in \a attribute using the type information from \a type.
    Returns true if the asynchronous call has been successfully dispatched.

    If the \a type parameter is omitted, the backend tries to find the correct type. Following default types are assumed:
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

/*!
    This method creates a monitored item for each of the attributes given in \a attr.
    The settings from \a settings are used in the creation of the monitored items and the subscription.

    The \a shared field of \a settings controls how the monitored items are assigned to a subscription. If shared in \a settings is set to Shared,
    the monitored item will be added to a subscription with a matching \a interval. If shared in \a settings is set to Exclusive,
    either a new subscription is created, or if a subscription is provided via the subscriptionId field in \a settings, an existing subscription is used.
    This can be used to group monitored items on a subscription manually by creating the first monitored item with \a shared=false and
    using the subscriptionId returned in \a subscriptionId of monitoringStatus(\a attr) in the following requests.

    Returns true if the asynchronous call has been successfully dispatched.

    On the completion of the call, the \l enableMonitoringFinished signal is emitted.
    There are multiple error cases in which a Bad status code is generated: A subscription with \a subscriptionId does not exist,
    the node does not exist on the server, the node does not have the requested attribute or the maximum number of monitored items for
    the server is reached.
 */
bool QOpcUaNode::enableMonitoring(QOpcUa::NodeAttributes attr, const QOpcUaMonitoringParameters &settings)
{
    if (d_func()->m_client.isNull() || d_func()->m_client->state() != QOpcUaClient::Connected)
        return false;

    return d_func()->m_impl->enableMonitoring(attr, settings);
}

/*!
    This method modifies settings of the monitored item or the subscription.
    The parameter \a item of the monitored item or subscription associated with \a attr is attempted to set to \a value.

    Returns true if the asynchronous call has been successfully dispatched.

    After the call has finished, the monitoringStatusChanged signal is emitted. This signal contains the modified parameters and the status code.
    A Bad status code is generated if there is no monitored item associated with the requested attribute, revising the requested
    parameter is not implemented or if the server has rejected the requested value.
*/
bool QOpcUaNode::modifyMonitoring(QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, const QVariant &value)
{
    if (d_func()->m_client.isNull() || d_func()->m_client->state() != QOpcUaClient::Connected)
        return false;

    return d_func()->m_impl->modifyMonitoring(attr, item, value);
}

/*!
    Returns the monitoring parameters associated with the attribute \a attr. This can be used to check the success of \l enableMonitoring()
    or if parameters have been revised.
    The returned values are only valid after \l enableMonitoringFinished or \l monitoringStatusChanged have been emitted for \a attr.
*/
QOpcUaMonitoringParameters QOpcUaNode::monitoringStatus(QOpcUa::NodeAttribute attr)
{
    auto it = d_func()->m_monitoringStatus.constFind(attr);
    if (it == d_func()->m_monitoringStatus.constEnd()) {
        QOpcUaMonitoringParameters p;
        p.setStatusCode(QOpcUa::UaStatusCode::BadAttributeIdInvalid);
        return p;
    }

    return *it;
}

/*!
    Writes \value to the attribute given in \a attribute using the type information from \a type.

    Returns true if the write operation has been dispatched successfully.
 */
bool QOpcUaNode::writeAttribute(QOpcUa::NodeAttribute attribute, const QVariant &value, QOpcUa::Types type)
{
    if (d_func()->m_client.isNull() || d_func()->m_client->state() != QOpcUaClient::Connected)
        return false;

    return d_func()->m_impl->writeAttribute(attribute, value, type, QString());
}

/*!
    Writes \a value to the attribute given in \a attribute using the type information from \a type.
    For \a indexRange, see \l readAttributeRange().

    Returns true if the asynchronous call has been successfully dispatched.
*/
bool QOpcUaNode::writeAttributeRange(QOpcUa::NodeAttribute attribute, const QVariant &value, const QString &indexRange, QOpcUa::Types type)
{
    if (d_func()->m_client.isNull() || d_func()->m_client->state() != QOpcUaClient::Connected)
        return false;

    return d_func()->m_impl->writeAttribute(attribute, value, type, indexRange);
}

/*!
    Executes a write operation for the attributes and values specified in \a toWrite.
    Returns true if the asynchronous call has been successfully dispatched.

    The \a valueAttributeType parameter can be used to supply type information for the value attribute.
    All other attributes have known types.
    \sa writeAttribute()
*/
bool QOpcUaNode::writeAttributes(const AttributeMap &toWrite, QOpcUa::Types valueAttributeType)
{
    if (d_func()->m_client.isNull() || d_func()->m_client->state() != QOpcUaClient::Connected)
        return false;

    return d_func()->m_impl->writeAttributes(toWrite, valueAttributeType);
}

/*!
    This method disables monitoring for the attributes given in \a attr.

    Returns true if the asynchronous call has been successfully dispatched.

    After the call is finished, the \l disableMonitoringFinished signal is emitted and monitoringStatus returns a default constructed value with
    status code BadMonitoredItemIdIinvalid for \a attr.
*/
bool QOpcUaNode::disableMonitoring(QOpcUa::NodeAttributes attr)
{
  if (d_func()->m_client.isNull() || d_func()->m_client->state() != QOpcUaClient::Connected)
      return false;

  return d_func()->m_impl->disableMonitoring(attr);
}

/*!
    Executes a forward browse call starting from the node this method is called on.
    The browse operation collects information about child nodes connected to the node
    and delivers the results in the \l browseFinished() signal.

    Returns \c true if the asynchronous call has been successfully dispatched.

    To request only children connected to the node by a certain type of reference, \a referenceType must be set to that reference type.
    For example, this can be  used to get all properties of a node by passing QOpcUa::ReferenceTypeId::HasProperty in \a referenceType.
    The results can be filtered to contain only nodes with certain node classes by setting them in \a nodeClassMask.
*/
bool QOpcUaNode::browseChildren(QOpcUa::ReferenceTypeId referenceType, QOpcUa::NodeClasses nodeClassMask)
{
    if (d_func()->m_client.isNull() || d_func()->m_client->state() != QOpcUaClient::Connected)
        return false;

    return d_func()->m_impl->browseChildren(referenceType, nodeClassMask);
}

/*!
    The ID of the OPC UA node.
*/
QString QOpcUaNode::nodeId() const
{
    if (d_func()->m_client.isNull() || d_func()->m_client->state() != QOpcUaClient::Connected)
        return QString();

    return d_func()->m_impl->nodeId();
}

/*!
    Reads value range from the OPC UA node and
    returns it as a pair of doubles containing the lower and upper limit.

    NaN values are returned if the read has failed or if the plugin does not
    support reading EURanges from the server.

    \warning Currently not supported by the FreeOPCUA backend
*/
QPair<double, double> QOpcUaNode::readEuRange() const
{
    if (d_func()->m_client.isNull() || d_func()->m_client->state() != QOpcUaClient::Connected)
        return QPair<double, double>(qQNaN(), qQNaN());

    return d_func()->m_impl->readEuRange();
}

/*!
    Reads Engineering unit information from the OPC UA node and returns it
    as a pair of strings containing the unit and its description.

    \warning Currently not supported by the FreeOPCUA backend
*/
QPair<QString, QString> QOpcUaNode::readEui() const
{
    if (d_func()->m_client.isNull() || d_func()->m_client->state() != QOpcUaClient::Connected)
        return QPair<QString, QString>();

    return d_func()->m_impl->readEui();
}

/*!
    Calls the OPC UA method \a methodNodeId with the parameters given via \a args. The result is
    returned in the \l methodCallFinished signal.

    Returns true if the asynchronous call has been successfully dispatched.
*/
bool QOpcUaNode::callMethod(const QString &methodNodeId, const QVector<QOpcUa::TypedVariant> &args)
{
    Q_D(QOpcUaNode);
    if (d->m_client.isNull() || d_func()->m_client->state() != QOpcUaClient::Connected)
        return false;

    return d->m_impl->callMethod(methodNodeId, args);
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

// Copyright (C) 2017 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuamonitoringparameters.h"
#include "private/qopcuamonitoringparameters_p.h"
#include <QtOpcUa/qopcuaeventfilterresult.h>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaMonitoringParameters
    \inmodule QtOpcUa

    \brief QOpcUaMonitoringParameters provides a way to set and retrieve parameters for subscriptions and monitored items.

    This class is used in \l QOpcUaNode::enableMonitoring() requests
    and as return value for \l QOpcUaNode::monitoringStatus() in which case it contains
    the revised values from the server.

    \section1 Usage
    For most use cases, only the publishing interval is required.
    \code
    QOpcUaMonitoringParameters p(100); // Set a publishing interval of 100ms and share the subscription.
    node->enableMonitoring(QOpcUa::NodeAttribute::Value, p);
    \endcode
    If an \l {QOpcUaMonitoringParameters::SubscriptionType} {exclusive} subscription is required, use the second parameter.
    \code
    QOpcUaMonitoringParameters p(100, QOpcUaMonitoringParameters::SubscriptionType::Exclusive); // Create a new subscription
    \endcode
    To add additional items to an existing subscription, use the third parameter for the next calls to QOpcUaNode::enableMonitoring().
    \code
    quint32 subscriptionId = node->monitoringStatus(QOpcUa::NodeAttribute::Value).subscriptionId();
    QOpcUaMonitoringParameters p(100, QOpcUaMonitoringParameters::SubscriptionType::Shared, subscriptionId); // Add the monitored item to this subscription
    \endcode

    If other parameters are required, they must be set using the setter methods.
*/

/*!
    \enum QOpcUaMonitoringParameters::MonitoringMode

    This enum is used to set the monitoring mode for a monitored item.

    \value Disabled Sampling is disabled and no notifications are being generated.
    \value Sampling Sampling is enabled and notifications are generated and queued, but reporting is disabled.
    \value Reporting Sampling is enabled, notifications are generated and queued, reporting is enabled.
*/

/*!
    \enum QOpcUaMonitoringParameters::SubscriptionType

    This enum is used to determine if the monitored item can be added to a shared subscription
    or if a new subscription must be created.

    \value Shared Share subscription with other monitored items with the same interval
    \value Exclusive Request a new subscription for this attribute
*/

/*!
    \enum QOpcUaMonitoringParameters::Parameter

    Enumerates parameters that can be modified at runtime using \l QOpcUaNode::modifyMonitoring().
    Not all values are guaranteed to be supported by all plugins. Lack of support will be reported
    in the \l QOpcUaNode::monitoringStatusChanged signal.

    \value PublishingEnabled
    \value PublishingInterval
    \value LifetimeCount
    \value MaxKeepAliveCount
    \value MaxNotificationsPerPublish
    \value Priority
    \value SamplingInterval
    \value Filter
    \value QueueSize
    \value DiscardOldest
    \value MonitoringMode
    \value [since 6.7] TriggeredItemIds
*/

/*!
    The default constructor for QOpcUaMonitoringParameters.
*/
QOpcUaMonitoringParameters::QOpcUaMonitoringParameters()
    : d_ptr(new QOpcUaMonitoringParametersPrivate())
{}

/*!
    The destructor for QOpcUaMonitoringParameters.
*/
QOpcUaMonitoringParameters::~QOpcUaMonitoringParameters()
{}

/*!
    This is the constructor which covers most use cases for the Qt OPC UA user.
    \a publishingInterval must be supplied, \a shared and \a subscriptionId are optional.
*/
QOpcUaMonitoringParameters::QOpcUaMonitoringParameters(double publishingInterval, QOpcUaMonitoringParameters::SubscriptionType shared, quint32 subscriptionId)
    : d_ptr(new QOpcUaMonitoringParametersPrivate)
{
    d_ptr->publishingInterval = publishingInterval;
    d_ptr->shared = shared;
    d_ptr->subscriptionId = subscriptionId;

}

/*!
    Constructs a QOpcuaMonitoringParameters object from the value of \a other.
*/
QOpcUaMonitoringParameters::QOpcUaMonitoringParameters(const QOpcUaMonitoringParameters &other)
    : d_ptr(other.d_ptr)
{}

/*!
    Assigns the value of \a other to this object.
*/
QOpcUaMonitoringParameters &QOpcUaMonitoringParameters::operator=(const QOpcUaMonitoringParameters &other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
    Returns the subscription type.
*/
QOpcUaMonitoringParameters::SubscriptionType QOpcUaMonitoringParameters::subscriptionType() const
{
    return d_ptr->shared;
}

/*!
    Request \a shared as subscription type for the subscription.
*/
void QOpcUaMonitoringParameters::setSubscriptionType(SubscriptionType shared)
{
    d_ptr->shared = shared;
}

/*!
    Returns the index range for the monitored item.
*/
QString QOpcUaMonitoringParameters::indexRange() const
{
    return d_ptr->indexRange;
}

/*!
    Requests \a indexRange as index range for the monitored item.
    For details on the index range string, see QOpcUaNode::readAttributeRange().
*/
void QOpcUaMonitoringParameters::setIndexRange(const QString &indexRange)
{
    d_ptr->indexRange = indexRange;
}

/*!
    \since 6.7

    Returns the ids of the monitored items triggerd by this monitored item.
*/
QSet<quint32> QOpcUaMonitoringParameters::triggeredItemIds() const
{
    return d_ptr->triggeredItemIds;
}

/*!
    \since 6.7

    Adds triggering links to all monitored items in \a ids as described in OPC UA 1.05, 5.12.1.6.

    The values in \a ids must be the monitored item ids of other monitored item on the same subscription.
    If the monitoring mode of these items is set to Sampling, their data change notifications will be
    delivered to the client whenever this monitoring detects a data change.

    Any ids that could not be added will not be included in the monitoring status but will instead show
    up in \l failedTriggeredItemsStatus().

    Modifying this setting to an empty set will remove all triggering links.
*/
void QOpcUaMonitoringParameters::setTriggeredItemIds(const QSet<quint32> &ids)
{
    d_ptr->triggeredItemIds = ids;
}

/*!
   \since 6.7

    Returns the status codes for all triggered items from \l setTriggeredItemIds() that could not
    be successfully added.
*/
QHash<quint32, QOpcUa::UaStatusCode> QOpcUaMonitoringParameters::failedTriggeredItemsStatus() const
{
    return d_ptr->addTriggeredItemStatus;
}

/*!
    \since 6.7

    Sets the status codes for all triggered items that could not be successfully added to \a status.
    Setting this value as a client has no effect.
*/
void QOpcUaMonitoringParameters::setFailedTriggeredItemsStatus(const QHash<quint32, QOpcUa::UaStatusCode> &status)
{
    d_ptr->addTriggeredItemStatus = status;
}

/*!
    Returns the status code of the monitored item creation.
*/
QOpcUa::UaStatusCode QOpcUaMonitoringParameters::statusCode() const
{
    return d_ptr->statusCode;
}

/*!
    Set the status code to \a statusCode.
*/
void QOpcUaMonitoringParameters::setStatusCode(QOpcUa::UaStatusCode statusCode)
{
    d_ptr->statusCode = statusCode;
}

/*!
    Returns the publishing mode for the subscription.
*/
bool QOpcUaMonitoringParameters::isPublishingEnabled() const
{
    return d_ptr->publishingEnabled;
}

/*!
    Set \a publishingEnabled as publishing mode for the subscription.
*/
void QOpcUaMonitoringParameters::setPublishingEnabled(bool publishingEnabled)
{
    d_ptr->publishingEnabled = publishingEnabled;
}

/*!
    Returns the priority of the subscription used for the monitored item.
*/
quint8 QOpcUaMonitoringParameters::priority() const
{
    return d_ptr->priority;
}

/*!
    Set \a priority as priority for the subscription.
*/
void QOpcUaMonitoringParameters::setPriority(quint8 priority)
{
    d_ptr->priority = priority;
}

/*!
    Returns the maximum notifications per publish value of the subscription.
*/
quint32 QOpcUaMonitoringParameters::maxNotificationsPerPublish() const
{
    return d_ptr->maxNotificationsPerPublish;
}

/*!
    Set \a maxNotificationsPerPublish as maximum notifications per publish value for the subscription.
*/
void QOpcUaMonitoringParameters::setMaxNotificationsPerPublish(quint32 maxNotificationsPerPublish)
{
    d_ptr->maxNotificationsPerPublish = maxNotificationsPerPublish;
}

/*!
    Returns the maximum keepalive count of the subscription.
*/
quint32 QOpcUaMonitoringParameters::maxKeepAliveCount() const
{
    return d_ptr->maxKeepAliveCount;
}

/*!
    Request \a maxKeepAliveCount as maximum keepalive count for the subscription.
*/
void QOpcUaMonitoringParameters::setMaxKeepAliveCount(quint32 maxKeepAliveCount)
{
    d_ptr->maxKeepAliveCount = maxKeepAliveCount;
}

/*!
    Returns the lifetime count of the subscription.
*/
quint32 QOpcUaMonitoringParameters::lifetimeCount() const
{
    return d_ptr->lifetimeCount;
}

/*!
    Request \a lifetimeCount as lifetime count for the subscription.
*/
void QOpcUaMonitoringParameters::setLifetimeCount(quint32 lifetimeCount)
{
    d_ptr->lifetimeCount = lifetimeCount;
}

/*!
    Returns the publishing interval of the subscription.
    The interval is expressed in milliseconds.
*/
double QOpcUaMonitoringParameters::publishingInterval() const
{
    return d_ptr->publishingInterval;
}

/*!
    Request \a publishingInterval as publishing interval for the subscription.
    The interval is expressed in milliseconds.
*/
void QOpcUaMonitoringParameters::setPublishingInterval(double publishingInterval)
{
    d_ptr->publishingInterval = publishingInterval;
}

/*!
    Returns the assigned subscription id.
*/
quint32 QOpcUaMonitoringParameters::subscriptionId() const
{
    return d_ptr->subscriptionId;
}

/*!
    Request the monitored items to be created on a known subscription with \a subscriptionId.
*/
void QOpcUaMonitoringParameters::setSubscriptionId(quint32 subscriptionId)
{
    d_ptr->subscriptionId = subscriptionId;
}

/*!
    Returns the monitored item id assigned by the server.
    If the monitored item id is 0, the monitored item could
    not be successfully created.
*/
quint32 QOpcUaMonitoringParameters::monitoredItemId() const
{
    return d_ptr->monitoredItemId;
}

/*!
    Sets the monitored item id to \a monitoredItemId.

    Setting this value as a client has no effect.
*/
void QOpcUaMonitoringParameters::setMonitoredItemId(quint32 monitoredItemId)
{
    d_ptr->monitoredItemId = monitoredItemId;
}

/*!
    Returns the monitoring mode for the monitored item.
*/
QOpcUaMonitoringParameters::MonitoringMode QOpcUaMonitoringParameters::monitoringMode() const
{
    return d_ptr->monitoringMode;
}

/*!
    Set \a monitoringMode as monitoring mode for the monitored item.
*/
void QOpcUaMonitoringParameters::setMonitoringMode(MonitoringMode monitoringMode)
{
    d_ptr->monitoringMode = monitoringMode;
}

/*!
    Returns the discardOldest setting of the monitored item.
*/
bool QOpcUaMonitoringParameters::discardOldest() const
{
    return d_ptr->discardOldest;
}

/*!
    Set \a discardOldest as discardOldest setting for the monitored item.
*/
void QOpcUaMonitoringParameters::setDiscardOldest(bool discardOldest)
{
    d_ptr->discardOldest = discardOldest;
}

/*!
    Returns the queue size of the monitored item.
*/
quint32 QOpcUaMonitoringParameters::queueSize() const
{
    return d_ptr->queueSize;
}

/*!
    Request \a queueSize as queue size for the monitored item.
*/
void QOpcUaMonitoringParameters::setQueueSize(quint32 queueSize)
{
    d_ptr->queueSize = queueSize;
}

/*!
    Returns the current filter.
    \sa setFilter()
*/
QVariant QOpcUaMonitoringParameters::filter() const
{
    return d_ptr->filter;
}

/*!
    Sets \l DataChangeFilter \a filter as filter for the monitored item.
    If another data change filter or an event filter is present, it will be replaced.

    If the server does not accept the filter, this will be indicated by the
    status code after the \l QOpcUaNode::enableMonitoring() request has finished.

    \sa filter()
*/
void QOpcUaMonitoringParameters::setFilter(const QOpcUaMonitoringParameters::DataChangeFilter &filter)
{
    d_ptr->filter = QVariant::fromValue(filter);
}

/*!
    Request \a eventFilter as filter for the monitored item.
    If another event filter or a data change filter is present, it will be replaced.
    If the server does not accept the filter, this will be indicated by the
    status code and the event filter result after the \l QOpcUaNode::enableMonitoring()
    request has finished.

    \sa filter()
*/
void QOpcUaMonitoringParameters::setFilter(const EventFilter &eventFilter)
{
    d_ptr->filter = QVariant::fromValue(eventFilter);
}

/*!
    Removes the current filter from the monitoring parameters.

    \sa filter() setFilter()
*/
void QOpcUaMonitoringParameters::clearFilter()
{
    d_ptr->filter.clear();
}

/*!
    Returns the filter result.

    This value is empty for an attribute monitoring. In case of an event monitoring,
    the filter result can be empty if the server did not detect any errors in the filter.
*/
QVariant QOpcUaMonitoringParameters::filterResult() const
{
    return d_ptr->filterResult;
}

/*!
    Sets the event filter result to \a eventFilterResult.

    This method must only be used by the backend, setting an event filter result as a user
    does not have any effect.

    \sa filterResult()
*/
void QOpcUaMonitoringParameters::setFilterResult(const QOpcUaEventFilterResult &eventFilterResult)
{
    d_ptr->filterResult = QVariant::fromValue(eventFilterResult);
}

/*!
    Removes the current filter result from the monitoring parameters.

    \sa filterResult() setFilterResult()
*/
void QOpcUaMonitoringParameters::clearFilterResult()
{
    d_ptr->filterResult.clear();
}

/*!
    Returns the revised sampling interval of the monitored item.
    The interval is expressed in milliseconds.
*/
double QOpcUaMonitoringParameters::samplingInterval() const
{
    return d_ptr->samplingInterval;
}

/*!
    Request \a samplingInterval as sampling interval for the monitored item.
    The interval is expressed in milliseconds.
*/
void QOpcUaMonitoringParameters::setSamplingInterval(double samplingInterval)
{
    d_ptr->samplingInterval = samplingInterval;
}

/*!
    \class QOpcUaMonitoringParameters::DataChangeFilter
    \inmodule QtOpcUa
    \inheaderfile QOpcUaMonitoringParameters
    \brief Defines a DataChangeFilter for a monitored item.

    This class is used to set up filtering for a DataChange monitored item.
    It is defined in OPC UA 1.05 part 4, 7.22.2.
*/

/*!
    \enum QOpcUaMonitoringParameters::DataChangeFilter::DataChangeTrigger

    Enumerates the possible triggers for a \l DataChangeFilter.

    \value Status Triggers if the value's status code changes.
    \value StatusOrValue Triggers if the value's status code or the value itself changes.
    \value StatusOrValueOrTimestamp Triggers if the value's status code, the value itself or the source timestamp changes.
*/

/*!
    \enum QOpcUaMonitoringParameters::DataChangeFilter::DeadbandType

    Enumerates the possible deadband types for a \l DataChangeFilter.

    \value None No deadband filtering.
    \value Absolute A notification is generated if the absolute value of the difference between the last cached value
           and the current value is greater than the deadband value.
    \value Percent Only valid for AnalogItems with an EURange property. A notification is generated if the absolute value
           of the difference between the last cached value and the current value is greater than value percent of the EURange.
*/

class QOpcUaMonitoringParameters::DataChangeFilterData : public QSharedData
{
public:
    DataChangeFilterData()
        : trigger(DataChangeFilter::DataChangeTrigger::Status)
        , deadbandType(DataChangeFilter::DeadbandType::None)
        , deadbandValue(0)
    {}

    DataChangeFilter::DataChangeTrigger trigger;
    DataChangeFilter::DeadbandType deadbandType;
    double deadbandValue;
};

/*!
    Constructs a data change filter with trigger on \c status, deadband type \c none and deadbandValue \c 0.
*/
QOpcUaMonitoringParameters::DataChangeFilter::DataChangeFilter()
    : data(new QOpcUaMonitoringParameters::DataChangeFilterData)
{
}

/*!
    Constructs a data change filter from \a rhs.
*/
QOpcUaMonitoringParameters::DataChangeFilter::DataChangeFilter(const DataChangeFilter &rhs)
    : data(rhs.data)
{
}

/*!
    Constructs a data change filter with trigger \a trigger, deadband type \a deadbandType and deadband value \a deadbandValue.
*/
QOpcUaMonitoringParameters::DataChangeFilter::DataChangeFilter(DataChangeFilter::DataChangeTrigger trigger,
                                                               DataChangeFilter::DeadbandType deadbandType, double deadbandValue)
    : data(new QOpcUaMonitoringParameters::DataChangeFilterData)
{
    data->trigger = trigger;
    data->deadbandType = deadbandType;
    data->deadbandValue = deadbandValue;
}

/*!
    Sets the values from \a rhs in this data change filter.
*/
QOpcUaMonitoringParameters::DataChangeFilter &QOpcUaMonitoringParameters::DataChangeFilter::operator=(const DataChangeFilter &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this data change filter has the same value as \a rhs.
*/
bool QOpcUaMonitoringParameters::DataChangeFilter::operator==(const QOpcUaMonitoringParameters::DataChangeFilter &rhs) const
{
    return data->deadbandType == rhs.deadbandType() &&
            data->trigger == rhs.trigger() &&
            data->deadbandValue == rhs.deadbandValue();
}

QOpcUaMonitoringParameters::DataChangeFilter::~DataChangeFilter()
{
}

/*!
    Returns the deadband value.
*/
double QOpcUaMonitoringParameters::DataChangeFilter::deadbandValue() const
{
    return data->deadbandValue;
}

/*!
    Sets the deadband value to \a deadbandValue.
*/
void QOpcUaMonitoringParameters::DataChangeFilter::setDeadbandValue(double deadbandValue)
{
    data->deadbandValue = deadbandValue;
}

/*!
    Returns the deadband type.
*/
QOpcUaMonitoringParameters::DataChangeFilter::DeadbandType QOpcUaMonitoringParameters::DataChangeFilter::deadbandType() const
{
    return data->deadbandType;
}

/*!
    Sets the deadband type to \a deadbandType.
*/
void QOpcUaMonitoringParameters::DataChangeFilter::setDeadbandType(DeadbandType deadbandType)
{
    data->deadbandType = deadbandType;
}

/*!
    Returns the trigger.
*/
QOpcUaMonitoringParameters::DataChangeFilter::DataChangeTrigger QOpcUaMonitoringParameters::DataChangeFilter::trigger() const
{
    return data->trigger;
}

/*!
    Sets the trigger to \a trigger.
*/
void QOpcUaMonitoringParameters::DataChangeFilter::setTrigger(DataChangeTrigger trigger)
{
    data->trigger = trigger;
}

/*!
    Converts this data change filter to \l QVariant.
*/
QOpcUaMonitoringParameters::DataChangeFilter::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
    \class QOpcUaMonitoringParameters::EventFilter
    \inmodule QtOpcUa
    \inheaderfile QOpcUaMonitoringParameters
    \brief Defines an EventFilter for a monitored item.

    An event filter is required for monitoring events on the server.
    It consists of \c select clauses and a \c where clause.

    The \c select clauses are used to specify the data the user wants to receive when an event occurs.
    It consists of \l {QOpcUaSimpleAttributeOperand} simple attribute operands which select
    attributes of child nodes of an event type, for example the value attribute of the "Message"
    property of BaseEventType.

    The \c where clause is used to restrict the reported events by matching against certain criteria.
    Several operators and four different operand types allow filtering based on the values of the
    attributes of the child nodes of an event type.

    Filters can be constructed using the setter or the streaming operator. Streaming a \l QOpcUaSimpleAttributeOperand
    into an event filter adds a new \c select clause to the filter, a \l QOpcUaContentFilterElement is appended to the \c where clause.
    A content filter element can be constructed by streaming operands of the types \l QOpcUaLiteralOperand,
    \l QOpcUaElementOperand, \l QOpcUaAttributeOperand and \l QOpcUaSimpleAttributeOperand and an operator into a content
    filter element. Only the last operator is used, previous operators will be discarded.

    The following EventFilter tells the server to report the value of the "Message" field for events that have a "Severity" field with value >= 500:

    \code
        QOpcUaMonitoringParameters::EventFilter filter;
        filter << QOpcUaSimpleAttributeOperand("Message"); // Select clause of the filter

        QOpcUaContentFilterElement condition;
        condition << QOpcUaContentFilterElement::FilterOperator::GreaterThanOrEqual;
        condition << QOpcUaSimpleAttributeOperand("Severity");
        condition << QOpcUaLiteralOperand(500, QOpcUa::Types::UInt16);
        filter << condition; // Where clause of the filter
    \endcode

    For a more complex example with two conditions, see \l QOpcUaElementOperand.
*/

class QOpcUaMonitoringParameters::EventFilterData : public QSharedData
{
public:
    QList<QOpcUaSimpleAttributeOperand> selectClauses;
    QList<QOpcUaContentFilterElement> whereClause;
};

QOpcUaMonitoringParameters::EventFilter::EventFilter()
    : data(new EventFilterData)
{
}

/*!
    Constructs an event filter from \a rhs.
*/
QOpcUaMonitoringParameters::EventFilter::EventFilter(const QOpcUaMonitoringParameters::EventFilter &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this event filter.
*/
QOpcUaMonitoringParameters::EventFilter &QOpcUaMonitoringParameters::EventFilter::operator=(const QOpcUaMonitoringParameters::EventFilter &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this event filter has the same value as \a rhs.
*/
bool QOpcUaMonitoringParameters::EventFilter::operator==(const QOpcUaMonitoringParameters::EventFilter &rhs) const
{
    return selectClauses() == rhs.selectClauses() && whereClause() == rhs.whereClause();
}

/*!
    Adds the content filter element \a whereClauseElement to the where clause of this event filter.
*/
QOpcUaMonitoringParameters::EventFilter &QOpcUaMonitoringParameters::EventFilter::operator<<(const QOpcUaContentFilterElement &whereClauseElement)
{
    whereClauseRef().append(whereClauseElement);
    return *this;
}

/*!
    Adds the simple attribute operand \a selectClauseElement to the select clause of this content filter element.
*/
QOpcUaMonitoringParameters::EventFilter &QOpcUaMonitoringParameters::EventFilter::operator<<(const QOpcUaSimpleAttributeOperand &selectClauseElement)
{
    selectClausesRef().append(selectClauseElement);
    return *this;
}

/*!
    Converts this event filter to \l QVariant.
*/
QOpcUaMonitoringParameters::EventFilter::operator const QVariant()
{
    return QVariant::fromValue(*this);
}

QOpcUaMonitoringParameters::EventFilter::~EventFilter()
{
}

/*!
    Returns the content filter used to restrict the reported events to events matching certain criteria.
*/
QList<QOpcUaContentFilterElement> QOpcUaMonitoringParameters::EventFilter::whereClause() const
{
    return data->whereClause;
}

/*!
    Returns a reference to the where clause.

    \sa whereClause()
*/
QList<QOpcUaContentFilterElement> &QOpcUaMonitoringParameters::EventFilter::whereClauseRef()
{
    return data->whereClause;
}

/*!
    Sets the where clause to \a whereClause.
*/
void QOpcUaMonitoringParameters::EventFilter::setWhereClause(const QList<QOpcUaContentFilterElement> &whereClause)
{
    data->whereClause = whereClause;
}

/*!
    Returns the selected event fields that shall be included when a new event is reported.
*/
QList<QOpcUaSimpleAttributeOperand> QOpcUaMonitoringParameters::EventFilter::selectClauses() const
{
    return data->selectClauses;
}

/*!
    Returns a reference to the select clauses.
*/
QList<QOpcUaSimpleAttributeOperand> &QOpcUaMonitoringParameters::EventFilter::selectClausesRef()
{
    return data->selectClauses;
}

/*!
    Sets the select clauses to \a selectClauses.
*/
void QOpcUaMonitoringParameters::EventFilter::setSelectClauses(const QList<QOpcUaSimpleAttributeOperand> &selectClauses)
{
    data->selectClauses = selectClauses;
}

QT_END_NAMESPACE

// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuahistoryevent.h"

#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaHistoryEvent
    \inmodule QtOpcUa
    \brief This class stores historical events from a node.
    \since 6.7

    When a request to read history events is being handled, instances of this class
    are used to store information about which node has been read, its events and
    the status code.
    The finished signal of a history read response will return a list of QOpcUaHistoryEvent
    objects which can be parsed to review the results of the request.
*/

class QOpcUaHistoryEventData : public QSharedData
{
public:
    QList<QVariantList> events;
    QOpcUa::UaStatusCode statusCode;
    QString nodeId;
};

QT_DEFINE_QESDP_SPECIALIZATION_DTOR(QOpcUaHistoryEventData)

/*!
    Constructs an invalid  history event item.
 */
QOpcUaHistoryEvent::QOpcUaHistoryEvent()
    : data(new QOpcUaHistoryEventData)
{
    data->statusCode = QOpcUa::UaStatusCode::Good;
}

/*!
    Constructs a history event item and stores which node it corresponds to.
*/
QOpcUaHistoryEvent::QOpcUaHistoryEvent(const QString &nodeId)
    : data(new QOpcUaHistoryEventData)
{
    data->statusCode = QOpcUa::UaStatusCode::Good;
    data->nodeId = nodeId;
}

/*!
    Constructs a history event item from \a other.
*/
QOpcUaHistoryEvent::QOpcUaHistoryEvent(const QOpcUaHistoryEvent &other)
    : data(other.data)
{
}

/*!
    Destroys the history event item.
 */
QOpcUaHistoryEvent::~QOpcUaHistoryEvent()
{
}

/*!
    \fn QOpcUaHistoryEvent::QOpcUaHistoryEvent(QOpcUaHistoryEvent &&other)

    Move-constructs a new history event object from \a other.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn QOpcUaHistoryEvent &QOpcUaHistoryEvent::operator=(QOpcUaHistoryEvent &&other)

    Move-assigns \a other to this QOpcUaHistoryEvent instance.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn void QOpcUaHistoryEvent::swap(QOpcUaHistoryEvent &other)

    Swaps history event object \a other with this history event
    object. This operation is very fast and never fails.
*/

/*!
    Returns the status code which indicates if an error occurred while fetching the history events.
*/
QOpcUa::UaStatusCode QOpcUaHistoryEvent::statusCode() const
{
    return data->statusCode;
}

/*!
    Sets the status code to \a statusCode.
*/
void QOpcUaHistoryEvent::setStatusCode(QOpcUa::UaStatusCode statusCode)
{
    if (statusCode != data->statusCode) {
        data.detach();
        data->statusCode = statusCode;
    }
}

/*!
    Returns the list of \l QVariantList objects which contain the results of the history read request.
*/
QList<QVariantList> QOpcUaHistoryEvent::events() const
{
    return data->events;
}

/*!
    Returns the number of available events.
*/
int QOpcUaHistoryEvent::count() const
{
    return data->events.size();
}

/*!
    Adds an event field list given by \a value.
*/
void QOpcUaHistoryEvent::addEvent(const QVariantList &value)
{
    data.detach();
    data->events.append(value);
}

/*!
    Returns the nodeId of the node whose events have been stored.
*/
QString QOpcUaHistoryEvent::nodeId() const
{
    return data->nodeId;
}

/*!
    Sets the nodeId to \a nodeId.
*/
void QOpcUaHistoryEvent::setNodeId(const QString &nodeId)
{
    if (nodeId != data->nodeId) {
        data.detach();
        data->nodeId = nodeId;
    }
}

/*!
    Sets the values from \a other in this history event item.
*/
QOpcUaHistoryEvent &QOpcUaHistoryEvent::operator=(const QOpcUaHistoryEvent &other)
{
    if (this != &other)
        data.operator=(other.data);
    return *this;
}

/*!
    \fn bool QOpcUaHistoryEvent::operator!=(const QOpcUaHistoryEvent &lhs, const QOpcUaHistoryEvent &rhs)

    Returns \c true if \a lhs is not equal to \a rhs.
*/

/*!
    \fn bool QOpcUaHistoryEvent::operator==(const QOpcUaHistoryEvent &lhs, const QOpcUaHistoryEvent &rhs)

    Returns \c true if \a rhs and \a lhs contain the same values.
*/
bool comparesEqual(const QOpcUaHistoryEvent &lhs, const QOpcUaHistoryEvent &rhs) noexcept
{
    return lhs.events() == rhs.events() && lhs.statusCode() == rhs.statusCode() && lhs.nodeId() == rhs.nodeId();
}

QT_END_NAMESPACE

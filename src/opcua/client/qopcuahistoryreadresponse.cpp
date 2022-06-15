// Copyright (C) 2021 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuahistoryreadresponse.h"

#include "private/qopcuahistoryreadresponse_p.h"
#include "private/qopcuahistoryreadresponseimpl_p.h"

QT_BEGIN_NAMESPACE
/*!
    \class QOpcUaHistoryReadResponse
    \inmodule QtOpcUa
    \brief This class is used for requesting historical data and storing the results.
    \since 6.3

    A historical data request to an OPC UA server can be specified by a \l QOpcUaHistoryReadRawRequest.

    Objects of this class and the statuscode of the request are returned in the \l QOpcUaHistoryReadResponse::readHistoryDataFinished(const QList<QOpcUaHistoryData> &results, QOpcUa::UaStatusCode serviceResult)
    signal and contain the result of a request.

*/

/*!
    \enum QOpcUaHistoryReadResponse::State

    This enum specifies the state the response is in.

    \value Unknown
    \value Reading
    \value Finished
    \value MoreDataAvailable
    \value Error
*/

/*!
    \fn QOpcUaHistoryReadResponse::readHistoryDataFinished(const QList<QOpcUaHistoryData> &results, QOpcUa::UaStatusCode serviceResult);

    This signal is emitted when a historical data request is finished. It adds
    to \a results and sets \a serviceResult to indicate the state of the result.

    \sa data(), serviceResult()
*/

/*!
    \fn QOpcUaHistoryReadResponse::stateChanged(State state)

    This signal is emitted when the of a historical data request is changed.
    It sets \a state to indicate the state of the change.
*/
QOpcUaHistoryReadResponse::QOpcUaHistoryReadResponse(QOpcUaHistoryReadResponseImpl *impl)
    : QObject(*new QOpcUaHistoryReadResponsePrivate(impl), nullptr)
{}

/*!
    The destructor for QOpcUaHistoryReadResponse
 */
QOpcUaHistoryReadResponse::~QOpcUaHistoryReadResponse()
{
}

/*!
    Returns \c true if there are more values available from the historic data request.
*/
bool QOpcUaHistoryReadResponse::hasMoreData() const
{
    return d_func()->m_impl->hasMoreData();
}

/*!
    Returns \c true if a read request for more historic values is successfully dispatched.
*/
bool QOpcUaHistoryReadResponse::readMoreData()
{
    return d_func()->m_impl->readMoreData();
}

/*!
    Returns the current state of historic data request.
*/
QOpcUaHistoryReadResponse::State QOpcUaHistoryReadResponse::state() const
{
    return d_func()->m_impl->state();
}

/*!
    Releases the continuation points and sets the request as finished.
    Returns \c true if the pending request has been successfully finished; otherwise returns false.
*/
bool QOpcUaHistoryReadResponse::releaseContinuationPoints()
{
    return d_func()->m_impl->releaseContinuationPoints();
}

/*!
    Returns a list which contains the requested historic data.
*/
QList<QOpcUaHistoryData> QOpcUaHistoryReadResponse::data() const
{
    return d_func()->m_impl->data();
}

/*!
    Returns the serviceresult of the historic data request.
*/
QOpcUa::UaStatusCode QOpcUaHistoryReadResponse::serviceResult() const
{
    return d_func()->m_impl->serviceResult();
}

QT_END_NAMESPACE

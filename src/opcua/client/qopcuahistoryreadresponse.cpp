/****************************************************************************
**
** Copyright (C) 2021 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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

    Objects of this class and the statuscode of the request are returned in the \l QOpcUaHistoryReadResponse::readHistoryDataFinished(QList<QOpcUaHistoryData> results, QOpcUa::UaStatusCode serviceResult)
    signal and contain the result of a request.

*/

/*!
    \fn QOpcUaHistoryReadResponse::readHistoryDataFinished(QList<QOpcUaHistoryData> results, QOpcUa::UaStatusCode serviceResult);

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
    The states are
    \list
    \li Unknown
    \li Reading
    \li Finished
    \li MoreDataAvailable
    \li Error
    \endlist
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

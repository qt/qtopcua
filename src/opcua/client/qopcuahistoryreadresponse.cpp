/****************************************************************************
**
** Copyright (C) 2021 basysKom GmbH, opensource@basyskom.com
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

#include "qopcuahistoryreadresponse.h"

#include "private/qopcuahistoryreadresponse_p.h"
#include "private/qopcuahistoryreadresponseimpl_p.h"

QT_BEGIN_NAMESPACE
/*!
    \class QOpcUaHistoryReadResponse
    \inmodule QtOpcUa
    \brief This class is used for requesting historical data and storing the results.

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
    Returns \c true if there are more values available from the historic data request.
*/
bool QOpcUaHistoryReadResponse::hasMoreData() const
{
    return d_func()->m_impl->hasMoreData();
}

/*!
    Returns \c true if read request for more historic values is succesfull dispatched.
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
    Returns a list which contains the requestet historic data.
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

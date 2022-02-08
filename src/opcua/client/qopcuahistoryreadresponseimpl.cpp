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

#include "qopcuahistoryreadresponseimpl_p.h"

QT_BEGIN_NAMESPACE

quint64 QOpcUaHistoryReadResponseImpl::m_currentHandle = 0;

QOpcUaHistoryReadResponseImpl::QOpcUaHistoryReadResponseImpl(const QOpcUaHistoryReadRawRequest &request)
    : m_requestType(RequestType::ReadRaw)
    , m_readRawRequest(request)
    , m_handle(++m_currentHandle)
{
}

QOpcUaHistoryReadResponseImpl::~QOpcUaHistoryReadResponseImpl()
{
    releaseContinuationPoints();
}

bool QOpcUaHistoryReadResponseImpl::hasMoreData() const
{
    return m_state == QOpcUaHistoryReadResponse::State::MoreDataAvailable;
}

bool QOpcUaHistoryReadResponseImpl::readMoreData()
{
    if (!hasMoreData())
        return false;

    if (m_requestType == RequestType::ReadRaw) {
        QOpcUaHistoryReadRawRequest request;
        request.setStartTimestamp(m_readRawRequest.startTimestamp());
        request.setEndTimestamp(m_readRawRequest.endTimestamp());
        request.setNumValuesPerNode(m_readRawRequest.numValuesPerNode());
        request.setReturnBounds(m_readRawRequest.returnBounds());

        int arrayIndex = 0;
        QList<int> newDataMapping;
        QList<QByteArray> newContinuationPoints;

        for (const auto &continuationPoint : m_continuationPoints) {
            int mappingIndex = 0;
            if (m_dataMapping.empty())
                mappingIndex = arrayIndex;
            else
                mappingIndex = m_dataMapping.at(arrayIndex);

            if (!continuationPoint.isEmpty()) {
                newDataMapping.push_back(mappingIndex);
                newContinuationPoints.push_back(continuationPoint);
                request.addNodeToRead(m_readRawRequest.nodesToRead().at(mappingIndex));
            }

            ++arrayIndex;
        }

        m_dataMapping = newDataMapping;

        m_continuationPoints = newContinuationPoints;
        emit historyReadRawRequested(request, m_continuationPoints, false, handle());
        return true;
    }

    return false;
}

QOpcUaHistoryReadResponse::State QOpcUaHistoryReadResponseImpl::state() const
{
    return m_state;
}

bool QOpcUaHistoryReadResponseImpl::releaseContinuationPoints()
{
    if (m_requestType == RequestType::ReadRaw) {
        emit historyReadRawRequested(m_readRawRequest, m_continuationPoints, true, handle());
        setState(QOpcUaHistoryReadResponse::State::Finished);
    };

    return true;
}

QList<QOpcUaHistoryData> QOpcUaHistoryReadResponseImpl::data() const
{
    return m_data;
}

QOpcUa::UaStatusCode QOpcUaHistoryReadResponseImpl::serviceResult() const
{
    return m_serviceResult;
}

void QOpcUaHistoryReadResponseImpl::handleDataAvailable(const QList<QOpcUaHistoryData> &data, const QList<QByteArray> &continuationPoints,
                                                           QOpcUa::UaStatusCode serviceResult, quint64 responseHandle)
{
    if (responseHandle != handle())
        return;

    m_serviceResult = serviceResult;
    m_continuationPoints = continuationPoints;

    if (m_data.empty()) {
        m_data = data;
    } else {
        int index = 0;
        for (const auto &result : data) {
            auto &target = m_data[m_dataMapping.at(index++)];
            target.setStatusCode(result.statusCode());
            for (const auto &value : result.result()) {
                target.addValue(value);
            }
        }
    }

    bool found = false;
    for (const auto &continuationPoint : m_continuationPoints) {
        if (!continuationPoint.isEmpty()) {
            setState(QOpcUaHistoryReadResponse::State::MoreDataAvailable);
            found = true;
            break;
        }
    }

    if (!found)
        setState(QOpcUaHistoryReadResponse::State::Finished);

    emit readHistoryDataFinished(m_data, m_serviceResult);
}

void QOpcUaHistoryReadResponseImpl::handleRequestError(quint64 requestHandle)
{
    if (requestHandle == handle())
        setState(QOpcUaHistoryReadResponse::State::Error);
}

quint64 QOpcUaHistoryReadResponseImpl::handle() const
{
    return m_handle;
}

void QOpcUaHistoryReadResponseImpl::setState(QOpcUaHistoryReadResponse::State state)
{
    if (m_state != state) {
        m_state = state;
        emit stateChanged(state);
    }
}

QT_END_NAMESPACE

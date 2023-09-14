// Copyright (C) 2021 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
        const auto request = createReadRawRequestWithContinuationPoints();
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
        const auto request = createReadRawRequestWithContinuationPoints();

        if (!request.nodesToRead().isEmpty())
            emit historyReadRawRequested(request, m_continuationPoints, true, handle());

        m_continuationPoints.clear();

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

QOpcUaHistoryReadRawRequest QOpcUaHistoryReadResponseImpl::createReadRawRequestWithContinuationPoints()
{
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

    return request;
}

QT_END_NAMESPACE

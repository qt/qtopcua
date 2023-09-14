// Copyright (C) 2021 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QOPCUAHISTORYREADRESPONSEIMPL_H
#define QOPCUAHISTORYREADRESPONSEIMPL_H

#include <QtOpcUa/qopcuahistoryreadresponse.h>
#include <QtOpcUa/qopcuahistoryreadrawrequest.h>

#include <private/qobject_p.h>
#include <QObject>

QT_BEGIN_NAMESPACE

class Q_OPCUA_EXPORT QOpcUaHistoryReadResponseImpl : public QObject {
    Q_OBJECT

public:
    QOpcUaHistoryReadResponseImpl(const QOpcUaHistoryReadRawRequest &request);
    ~QOpcUaHistoryReadResponseImpl();

    bool hasMoreData() const;
    bool readMoreData();
    QOpcUaHistoryReadResponse::State state() const;

    bool releaseContinuationPoints();

    QList<QOpcUaHistoryData> data() const;
    QOpcUa::UaStatusCode serviceResult() const;

    Q_INVOKABLE void handleDataAvailable(const QList<QOpcUaHistoryData> &data, const QList<QByteArray> &continuationPoints,
                                         QOpcUa::UaStatusCode serviceResult, quint64 responseHandle);
    Q_INVOKABLE void handleRequestError(quint64 requestHandle);

    quint64 handle() const;

Q_SIGNALS:
    void historyReadRawRequested(QOpcUaHistoryReadRawRequest request, QList<QByteArray> continuationPoints, bool releaseContinuationPoints, quint64 handle);
    void readHistoryDataFinished(QList<QOpcUaHistoryData> results, QOpcUa::UaStatusCode serviceResult);
    void stateChanged(QOpcUaHistoryReadResponse::State state);

protected:
    void setState(QOpcUaHistoryReadResponse::State state);

    QOpcUaHistoryReadRawRequest createReadRawRequestWithContinuationPoints();

private:
    enum class RequestType {
        Unknown,
        ReadRaw
    };

    QOpcUaHistoryReadResponse::State m_state = QOpcUaHistoryReadResponse::State::Reading;
    QList<QByteArray> m_continuationPoints;

    RequestType m_requestType = RequestType::Unknown;
    QOpcUaHistoryReadRawRequest m_readRawRequest;
    QList<QOpcUaHistoryData> m_data;
    QOpcUa::UaStatusCode m_serviceResult = QOpcUa::UaStatusCode::Good;
    QList<int> m_dataMapping;

    static quint64 m_currentHandle;

    quint64 m_handle = 0;
};

QT_END_NAMESPACE

#endif // QOPCUAHISTORYREADRESPONSEIMPL_H

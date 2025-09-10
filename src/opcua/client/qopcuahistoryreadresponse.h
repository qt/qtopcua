// Copyright (C) 2021 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAHISTORYREADRESPONSE_H
#define QOPCUAHISTORYREADRESPONSE_H

#include <QtOpcUa/qopcuahistorydata.h>
#include <QtOpcUa/qopcuahistoryevent.h>
#include <QtOpcUa/qopcuaglobal.h>

#include <QtOpcUa/qopcuahistoryreadrawrequest.h>

#include <QtCore/qpointer.h>

QT_BEGIN_NAMESPACE

class QOpcUaHistoryReadResponseImpl;

class QOpcUaHistoryReadResponsePrivate;

class Q_OPCUA_EXPORT QOpcUaHistoryReadResponse : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(QOpcUaHistoryReadResponse)
public:
    explicit QOpcUaHistoryReadResponse(QOpcUaHistoryReadResponseImpl *impl);
    ~QOpcUaHistoryReadResponse();

    enum class State : quint32 {
        Unknown,
        Reading,
        Finished,
        MoreDataAvailable,
        Error,
    };
    Q_ENUM(State)

    bool hasMoreData() const;
    bool readMoreData();
    State state() const;

    bool releaseContinuationPoints();

    QList<QOpcUaHistoryData> data() const;
    QList<QOpcUaHistoryEvent> events() const;
    QOpcUa::UaStatusCode serviceResult() const;

Q_SIGNALS:
    void readHistoryDataFinished(const QList<QOpcUaHistoryData> &results, QOpcUa::UaStatusCode serviceResult);
    void readHistoryEventsFinished(const QList<QOpcUaHistoryEvent> &results, QOpcUa::UaStatusCode serviceResult);
    void stateChanged(State state);
};

QT_END_NAMESPACE

#endif // QOPCUAHISTORYREADRESPONSE_H

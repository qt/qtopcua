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

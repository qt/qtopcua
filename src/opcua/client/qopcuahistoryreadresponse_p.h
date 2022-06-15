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

#ifndef QOPCUAHISTORYREADRESPONSEPRIVATE_H
#define QOPCUAHISTORYREADRESPONSEPRIVATE_H

#include <QtOpcUa/qopcuahistoryreadresponse.h>
#include <QtOpcUa/qopcuahistoryreadrawrequest.h>

#include "private/qopcuahistoryreadresponseimpl_p.h"

#include <private/qobject_p.h>
#include <QObject>

#include <QUuid>

QT_BEGIN_NAMESPACE

class Q_OPCUA_EXPORT QOpcUaHistoryReadResponsePrivate : public QObjectPrivate {
    Q_DECLARE_PUBLIC(QOpcUaHistoryReadResponse)

public:
    QOpcUaHistoryReadResponsePrivate(QOpcUaHistoryReadResponseImpl *impl)
        : m_impl(impl)
    {
        QObject::connect(impl, &QOpcUaHistoryReadResponseImpl::readHistoryDataFinished, impl,
                         [this](const QList<QOpcUaHistoryData> &data, QOpcUa::UaStatusCode serviceResult) {
            if (q_func())
                emit q_func()->readHistoryDataFinished(data, serviceResult);
        });

        QObject::connect(impl, &QOpcUaHistoryReadResponseImpl::stateChanged, impl,
                         [this](QOpcUaHistoryReadResponse::State state) {
            if (q_func())
                emit q_func()->stateChanged(state);
        });
    }

    ~QOpcUaHistoryReadResponsePrivate() = default;

    QScopedPointer<QOpcUaHistoryReadResponseImpl> m_impl;
};

QT_END_NAMESPACE

#endif // QOPCUAHISTORYREADRESPONSEPRIVATE_H

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

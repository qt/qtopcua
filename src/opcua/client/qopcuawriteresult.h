// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAWRITERESULT_H
#define QOPCUAWRITERESULT_H

#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaWriteResultData;
class Q_OPCUA_EXPORT QOpcUaWriteResult
{
public:
    QOpcUaWriteResult();
    QOpcUaWriteResult(const QOpcUaWriteResult &other);
    QOpcUaWriteResult &operator=(const QOpcUaWriteResult &rhs);
    ~QOpcUaWriteResult();

    QString nodeId() const;
    void setNodeId(const QString &nodeId);

    QOpcUa::NodeAttribute attribute() const;
    void setAttribute(QOpcUa::NodeAttribute attribute);

    QString indexRange() const;
    void setIndexRange(const QString &indexRange);

    QOpcUa::UaStatusCode statusCode() const;
    void setStatusCode(QOpcUa::UaStatusCode statusCode);

private:
    QSharedDataPointer<QOpcUaWriteResultData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaWriteResult)

#endif // QOPCUAWRITERESULT_H

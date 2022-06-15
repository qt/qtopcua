// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAREADRESULT_H
#define QOPCUAREADRESULT_H

#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qdatetime.h>

QT_BEGIN_NAMESPACE

class QOpcUaReadResultData;
class Q_OPCUA_EXPORT QOpcUaReadResult
{
public:
    QOpcUaReadResult();
    QOpcUaReadResult(const QOpcUaReadResult &other);
    QOpcUaReadResult &operator=(const QOpcUaReadResult &rhs);
    ~QOpcUaReadResult();

    QDateTime serverTimestamp() const;
    void setServerTimestamp(const QDateTime &serverTimestamp);

    QDateTime sourceTimestamp() const;
    void setSourceTimestamp(const QDateTime &sourceTimestamp);

    QOpcUa::UaStatusCode statusCode() const;
    void setStatusCode(QOpcUa::UaStatusCode statusCode);

    QString nodeId() const;
    void setNodeId(const QString &nodeId);

    QOpcUa::NodeAttribute attribute() const;
    void setAttribute(QOpcUa::NodeAttribute attribute);

    QString indexRange() const;
    void setIndexRange(const QString &indexRange);

    QVariant value() const;
    void setValue(const QVariant &value);

private:
    QSharedDataPointer<QOpcUaReadResultData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaReadResult)

#endif // QOPCUAREADRESULT_H

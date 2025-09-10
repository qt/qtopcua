// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAWRITEITEM_H
#define QOPCUAWRITEITEM_H

#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qdatetime.h>

QT_BEGIN_NAMESPACE

class QOpcUaWriteItemData;
class Q_OPCUA_EXPORT QOpcUaWriteItem
{
public:
    QOpcUaWriteItem();
    QOpcUaWriteItem(const QOpcUaWriteItem &other);
    QOpcUaWriteItem(const QString &nodeId, QOpcUa::NodeAttribute attribute, const QVariant &value,
                    QOpcUa::Types type = QOpcUa::Types::Undefined, const QString &indexRange = QString());
    QOpcUaWriteItem &operator=(const QOpcUaWriteItem &rhs);
    ~QOpcUaWriteItem();

    QString nodeId() const;
    void setNodeId(const QString &nodeId);

    QOpcUa::NodeAttribute attribute() const;
    void setAttribute(QOpcUa::NodeAttribute attribute);

    QString indexRange() const;
    void setIndexRange(const QString &indexRange);

    QVariant value() const;
    void setValue(const QVariant &value);
    void setValue(const QVariant &value, QOpcUa::Types type);

    QOpcUa::Types type() const;
    void setType(QOpcUa::Types type);

    QDateTime sourceTimestamp() const;
    void setSourceTimestamp(const QDateTime &sourceTimestamp);

    QDateTime serverTimestamp() const;
    void setServerTimestamp(const QDateTime &serverTimestamp);

    QOpcUa::UaStatusCode statusCode() const;
    bool hasStatusCode() const;
    void setStatusCode(QOpcUa::UaStatusCode statusCode);

private:
    QSharedDataPointer<QOpcUaWriteItemData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaWriteItem)

#endif // QOPCUAWRITEITEM_H

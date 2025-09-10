// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAREADITEM_H
#define QOPCUAREADITEM_H

#include <QtOpcUa/qopcuatype.h>
#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaReadItemData;
class Q_OPCUA_EXPORT QOpcUaReadItem
{
public:
    QOpcUaReadItem();
    QOpcUaReadItem(const QOpcUaReadItem &other);
    QOpcUaReadItem(const QString &nodeId, QOpcUa::NodeAttribute attr = QOpcUa::NodeAttribute::Value,
                   const QString &indexRange = QString());
    QOpcUaReadItem &operator=(const QOpcUaReadItem &rhs);
    ~QOpcUaReadItem();

    QString nodeId() const;
    void setNodeId(const QString &nodeId);

    QOpcUa::NodeAttribute attribute() const;
    void setAttribute(QOpcUa::NodeAttribute attribute);

    QString indexRange() const;
    void setIndexRange(const QString &indexRange);

private:
    QSharedDataPointer<QOpcUaReadItemData> data;

    friend Q_OPCUA_EXPORT bool operator==(const QOpcUaReadItem &lhs,
                                          const QOpcUaReadItem &rhs) noexcept;
    friend inline bool operator!=(const QOpcUaReadItem &lhs, const QOpcUaReadItem &rhs) noexcept
    {
        return !(lhs == rhs);
    }
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaReadItem)

#endif // QOPCUAREADITEM_H

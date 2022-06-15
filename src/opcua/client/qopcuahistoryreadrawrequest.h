// Copyright (C) 2019 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QHISTORYREADRAWREQUEST_H
#define QHISTORYREADRAWREQUEST_H

#include <QtCore/qobject.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qshareddata.h>

#include <QtOpcUa/qopcuareaditem.h>
#include <QtOpcUa/qopcuahistorydata.h>

#include <QtOpcUa/qopcuaglobal.h>

QT_BEGIN_NAMESPACE

class QOpcUaHistoryReadRawRequestData;
class Q_OPCUA_EXPORT QOpcUaHistoryReadRawRequest
{
public:
    QOpcUaHistoryReadRawRequest();
    explicit QOpcUaHistoryReadRawRequest(const QList<QOpcUaReadItem> &nodesToRead,
                                         const QDateTime &startTimestamp,
                                         const QDateTime &endTimestamp,
                                         quint32 numValuesPerNode = 0,
                                         bool returnBounds = false);

    QOpcUaHistoryReadRawRequest(const QOpcUaHistoryReadRawRequest &other);
    ~QOpcUaHistoryReadRawRequest();

    void swap(QOpcUaHistoryReadRawRequest &other) noexcept
    { data.swap(other.data); }

    QDateTime startTimestamp() const;
    void setStartTimestamp(const QDateTime &startTimestamp);

    QDateTime endTimestamp() const;
    void setEndTimestamp(const QDateTime &endTimestamp);

    quint32 numValuesPerNode() const;
    void setNumValuesPerNode(quint32 numValuesPerNode);

    bool returnBounds() const;
    void setReturnBounds(bool returnBounds);

    QList<QOpcUaReadItem> nodesToRead() const;
    void setNodesToRead(const QList<QOpcUaReadItem> &nodesToRead);

    void addNodeToRead(const QOpcUaReadItem &nodeToRead);

    QOpcUaHistoryReadRawRequest &operator=(const QOpcUaHistoryReadRawRequest &other);

private:
    QSharedDataPointer<QOpcUaHistoryReadRawRequestData> data;

    friend Q_OPCUA_EXPORT bool operator==(const QOpcUaHistoryReadRawRequest &lhs,
                                          const QOpcUaHistoryReadRawRequest &rhs) noexcept;
    friend inline bool operator!=(const QOpcUaHistoryReadRawRequest &lhs,
                                  const QOpcUaHistoryReadRawRequest &rhs) noexcept
    {
        return !(lhs == rhs);
    }
};

Q_DECLARE_SHARED(QOpcUaHistoryReadRawRequest)

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaHistoryReadRawRequest)

#endif // QHISTORYREADRAWREQUEST_H

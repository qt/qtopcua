// Copyright (C) 2019 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAHISTORYDATA_H
#define QOPCUAHISTORYDATA_H

#include <QtOpcUa/qopcuatype.h>
#include <QtOpcUa/qopcuadatavalue.h>

#include <QtOpcUa/qopcuaglobal.h>
#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

class QOpcUaHistoryDataData;
class Q_OPCUA_EXPORT QOpcUaHistoryData
{
public:
    QOpcUaHistoryData();
    explicit QOpcUaHistoryData(const QString &nodeId);
    QOpcUaHistoryData(const QOpcUaHistoryData &other);
    ~QOpcUaHistoryData();

    void swap(QOpcUaHistoryData &other) noexcept
    { data.swap(other.data); }

    QOpcUa::UaStatusCode statusCode() const;
    void setStatusCode(QOpcUa::UaStatusCode statusCode);
    QList<QOpcUaDataValue> result() const;
    int count() const;
    void addValue(const QOpcUaDataValue &value);
    QString nodeId() const;
    void setNodeId(const QString &nodeId);

    QOpcUaHistoryData &operator=(const QOpcUaHistoryData &other);
private:
    QExplicitlySharedDataPointer<QOpcUaHistoryDataData> data;
};

Q_DECLARE_SHARED(QOpcUaHistoryData)

QT_END_NAMESPACE

#endif // QOPCUAHISTORYDATA_H

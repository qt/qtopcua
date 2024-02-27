// Copyright (C) 2021 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUADATAVALUE_H
#define QOPCUADATAVALUE_H

#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qdatetime.h>

QT_BEGIN_NAMESPACE

class QOpcUaDataValueData;
class Q_OPCUA_EXPORT QOpcUaDataValue
{
public:
    QOpcUaDataValue();
    QOpcUaDataValue(const QOpcUaDataValue &other);
    QOpcUaDataValue &operator=(const QOpcUaDataValue &other);
    ~QOpcUaDataValue();

    void swap(QOpcUaDataValue &other) noexcept
    { data.swap(other.data); }

    QDateTime serverTimestamp() const;
    void setServerTimestamp(const QDateTime &serverTimestamp);

    quint16 serverPicoseconds() const;
    void setServerPicoseconds(quint16 serverPicoseconds);

    QDateTime sourceTimestamp() const;
    void setSourceTimestamp(const QDateTime &sourceTimestamp);

    quint16 sourcePicoseconds() const;
    void setSourcePicoseconds(quint16 sourcePicoseconds);

    QOpcUa::UaStatusCode statusCode() const;
    void setStatusCode(QOpcUa::UaStatusCode statusCode);

    QVariant value() const;
    void setValue(const QVariant &value);

    operator QVariant() const;

private:
    QExplicitlySharedDataPointer<QOpcUaDataValueData> data;

    friend Q_OPCUA_EXPORT bool comparesEqual(const QOpcUaDataValue &lhs,
                                             const QOpcUaDataValue &rhs) noexcept;
    friend bool operator==(const QOpcUaDataValue &lhs, const QOpcUaDataValue &rhs) noexcept
    { return comparesEqual(lhs, rhs); }
    friend bool operator!=(const QOpcUaDataValue &lhs, const QOpcUaDataValue &rhs) noexcept
    {
        return !(lhs == rhs);
    }
};

Q_DECLARE_SHARED(QOpcUaDataValue)

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaDataValue)

#endif // QOPCUADATAVALUE_H

// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAVARIANT_H
#define QOPCUAVARIANT_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qcontainerfwd.h>
#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QVariant;

class QOpcUaVariantData;

QT_DECLARE_QESDP_SPECIALIZATION_DTOR_WITH_EXPORT(QOpcUaVariantData, Q_OPCUA_EXPORT)

class QOpcUaVariant
{
public:
    enum class ValueType {
        Unknown = 0,
        Boolean = 1,
        SByte = 2,
        Byte = 3,
        Int16 = 4,
        UInt16 = 5,
        Int32 = 6,
        UInt32 = 7,
        Int64 = 8,
        UInt64 = 9,
        Float = 10,
        Double = 11,
        String = 12,
        DateTime = 13,
        Guid = 14,
        ByteString = 15,
        XmlElement = 16,
        NodeId = 17,
        ExpandedNodeId = 18,
        StatusCode = 19,
        QualifiedName = 20,
        LocalizedText = 21,
        ExtensionObject = 22,
        DataValue = 23,
        Variant = 24,
        DiagnosticInfo = 25,
    };

    Q_OPCUA_EXPORT QOpcUaVariant();
    Q_OPCUA_EXPORT QOpcUaVariant(ValueType type, const QVariant &value);
    Q_OPCUA_EXPORT QOpcUaVariant(ValueType type, const QVariant &value,
                                                  const QList<qint32> arrayDimensions);
    Q_OPCUA_EXPORT QOpcUaVariant(const QOpcUaVariant &other);
    Q_OPCUA_EXPORT ~QOpcUaVariant();
    void swap(QOpcUaVariant &other) noexcept
    { data.swap(other.data); }
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QOpcUaVariant)
    QOpcUaVariant(QOpcUaVariant &&other) noexcept = default;
    Q_OPCUA_EXPORT QOpcUaVariant &operator=(const QOpcUaVariant &rhs);

    Q_OPCUA_EXPORT QVariant value() const;
    Q_OPCUA_EXPORT void setValue(ValueType type, const QVariant &value);
    Q_OPCUA_EXPORT void setValue(ValueType type, const QVariant &value,
                                      const QList<qint32> &arrayDimensions);

    Q_OPCUA_EXPORT ValueType type() const;
    Q_OPCUA_EXPORT bool isArray() const;

    Q_OPCUA_EXPORT QList<qint32> arrayDimensions() const;
    Q_OPCUA_EXPORT void setArrayDimensions(const QList<qint32> &arrayDimensions);

    Q_OPCUA_EXPORT operator QVariant() const;

private:
    QExplicitlySharedDataPointer<QOpcUaVariantData> data;

    friend Q_OPCUA_EXPORT bool comparesEqual(const QOpcUaVariant &lhs,
                                             const QOpcUaVariant &rhs) noexcept;
    friend bool operator==(const QOpcUaVariant &lhs, const QOpcUaVariant &rhs) noexcept
    { return comparesEqual(lhs, rhs); }
    friend bool operator!=(const QOpcUaVariant &lhs, const QOpcUaVariant &rhs) noexcept
    {
        return !(lhs == rhs);
    }
};

Q_DECLARE_SHARED(QOpcUaVariant)

QT_END_NAMESPACE

#endif // QOPCUAVARIANT_H

// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUASTRUCTUREDEFINITION_H
#define QOPCUASTRUCTUREDEFINITION_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qcontainerfwd.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qstringfwd.h>

QT_BEGIN_NAMESPACE

class QOpcUaStructureDefinitionData;
class QOpcUaStructureField;

QT_DECLARE_QESDP_SPECIALIZATION_DTOR_WITH_EXPORT(QOpcUaStructureDefinitionData, Q_OPCUA_EXPORT)

class QOpcUaStructureDefinition
{
public:
    Q_OPCUA_EXPORT QOpcUaStructureDefinition();
    Q_OPCUA_EXPORT QOpcUaStructureDefinition(const QOpcUaStructureDefinition &);
    void swap(QOpcUaStructureDefinition &other) noexcept
    { data.swap(other.data); }
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QOpcUaStructureDefinition)
    QOpcUaStructureDefinition(QOpcUaStructureDefinition &&other) noexcept = default;
    Q_OPCUA_EXPORT QOpcUaStructureDefinition &operator=(const QOpcUaStructureDefinition &);
    Q_OPCUA_EXPORT operator QVariant() const;
    Q_OPCUA_EXPORT ~QOpcUaStructureDefinition();

    enum class StructureType {
        Structure = 0,
        StructureWithOptionalFields = 1,
        Union = 2,
    };

    Q_OPCUA_EXPORT QString defaultEncodingId() const;
    Q_OPCUA_EXPORT void setDefaultEncodingId(const QString &defaultEncodingId);

    Q_OPCUA_EXPORT QString baseDataType() const;
    Q_OPCUA_EXPORT void setBaseDataType(const QString &baseDataType);

    Q_OPCUA_EXPORT QOpcUaStructureDefinition::StructureType structureType() const;
    Q_OPCUA_EXPORT void setStructureType(const QOpcUaStructureDefinition::StructureType &structureType);

    Q_OPCUA_EXPORT QList<QOpcUaStructureField> fields() const;
    Q_OPCUA_EXPORT void setFields(const QList<QOpcUaStructureField> &fields);

private:
    QExplicitlySharedDataPointer<QOpcUaStructureDefinitionData> data;

    friend Q_OPCUA_EXPORT bool comparesEqual(const QOpcUaStructureDefinition &lhs,
                                             const QOpcUaStructureDefinition &rhs) noexcept;
    friend bool operator==(const QOpcUaStructureDefinition &lhs,
                           const QOpcUaStructureDefinition &rhs) noexcept
    { return comparesEqual(lhs, rhs); }
    friend bool operator!=(const QOpcUaStructureDefinition &lhs,
                           const QOpcUaStructureDefinition &rhs) noexcept
    {
        return !(lhs == rhs);
    }
};

Q_DECLARE_SHARED(QOpcUaStructureDefinition)

QT_END_NAMESPACE

#endif // QOPCUASTRUCTUREDEFINITION_H

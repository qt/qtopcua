// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUASTRUCTUREFIELD_H
#define QOPCUASTRUCTUREFIELD_H

#include <QtOpcUa/qopcuaglobal.h>
#include <QtOpcUa/qopcualocalizedtext.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QOpcUaStructureFieldData;

QT_DECLARE_QSDP_SPECIALIZATION_DTOR_WITH_EXPORT(QOpcUaStructureFieldData, Q_OPCUA_EXPORT)

class QOpcUaStructureField
{
public:
    Q_OPCUA_EXPORT QOpcUaStructureField();
    Q_OPCUA_EXPORT QOpcUaStructureField(const QOpcUaStructureField &other);
    void swap(QOpcUaStructureField &other) noexcept
    { data.swap(other.data); }
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QOpcUaStructureField)
    QOpcUaStructureField(QOpcUaStructureField &&other) noexcept = default;
    Q_OPCUA_EXPORT QOpcUaStructureField &operator=(const QOpcUaStructureField &rhs);
    friend Q_OPCUA_EXPORT bool operator==(const QOpcUaStructureField &lhs, const QOpcUaStructureField &rhs) noexcept;
    friend inline bool operator!=(const QOpcUaStructureField &lhs, const QOpcUaStructureField &rhs) noexcept
    {
        return !(lhs == rhs);
    }
    Q_OPCUA_EXPORT operator QVariant() const;
    Q_OPCUA_EXPORT ~QOpcUaStructureField();

    Q_OPCUA_EXPORT QString name() const;
    Q_OPCUA_EXPORT void setName(const QString &name);

    Q_OPCUA_EXPORT QOpcUaLocalizedText description() const;
    Q_OPCUA_EXPORT void setDescription(const QOpcUaLocalizedText &description);

    Q_OPCUA_EXPORT QString dataType() const;
    Q_OPCUA_EXPORT void setDataType(const QString &dataTypeId);

    Q_OPCUA_EXPORT qint32 valueRank() const;
    Q_OPCUA_EXPORT void setValueRank(qint32 valueRank);

    Q_OPCUA_EXPORT QList<quint32> arrayDimensions() const;
    Q_OPCUA_EXPORT void setArrayDimensions(const QList<quint32> &arrayDimensions);

    Q_OPCUA_EXPORT quint32 maxStringLength() const;
    Q_OPCUA_EXPORT void setMaxStringLength(quint32 maxStringLength);

    Q_OPCUA_EXPORT bool isOptional() const;
    Q_OPCUA_EXPORT void setIsOptional(bool isOptional);

private:
    QSharedDataPointer<QOpcUaStructureFieldData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaStructureField)

#endif // QOPCUASTRUCTUREFIELD_H

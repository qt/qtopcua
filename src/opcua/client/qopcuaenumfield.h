// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAENUMFIELD_H
#define QOPCUAENUMFIELD_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qstringfwd.h>

QT_BEGIN_NAMESPACE

class QOpcUaEnumFieldData;
class QOpcUaLocalizedText;

class QVariant;

QT_DECLARE_QESDP_SPECIALIZATION_DTOR_WITH_EXPORT(QOpcUaEnumFieldData, Q_OPCUA_EXPORT)

class QOpcUaEnumField
{
public:
    Q_OPCUA_EXPORT QOpcUaEnumField();
    Q_OPCUA_EXPORT QOpcUaEnumField(const QOpcUaEnumField &other);
    void swap(QOpcUaEnumField &other) noexcept
    { data.swap(other.data); }
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QOpcUaEnumField)
    QOpcUaEnumField(QOpcUaEnumField &&other) noexcept = default;
    Q_OPCUA_EXPORT QOpcUaEnumField &operator=(const QOpcUaEnumField &rhs);
    Q_OPCUA_EXPORT operator QVariant() const;
    Q_OPCUA_EXPORT ~QOpcUaEnumField();


    Q_OPCUA_EXPORT qint64 value() const;
    Q_OPCUA_EXPORT void setValue(qint64 value);

    Q_OPCUA_EXPORT QOpcUaLocalizedText displayName() const;
    Q_OPCUA_EXPORT void setDisplayName(const QOpcUaLocalizedText &displayName);

    Q_OPCUA_EXPORT QOpcUaLocalizedText description() const;
    Q_OPCUA_EXPORT void setDescription(const QOpcUaLocalizedText &description);

    Q_OPCUA_EXPORT QString name() const;
    Q_OPCUA_EXPORT void setName(const QString &name);

private:
    QExplicitlySharedDataPointer<QOpcUaEnumFieldData> data;

    friend Q_OPCUA_EXPORT bool comparesEqual(const QOpcUaEnumField &lhs,
                                             const QOpcUaEnumField &rhs) noexcept;
    friend bool operator==(const QOpcUaEnumField &lhs, const QOpcUaEnumField &rhs) noexcept
    { return comparesEqual(lhs, rhs); }
    friend bool operator!=(const QOpcUaEnumField &lhs, const QOpcUaEnumField &rhs) noexcept
    {
        return !(lhs == rhs);
    }
};

Q_DECLARE_SHARED(QOpcUaEnumField)

QT_END_NAMESPACE

#endif // QOPCUAENUMFIELD_H

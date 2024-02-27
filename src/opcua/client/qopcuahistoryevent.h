// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAHISTORYEVENT_H
#define QOPCUAHISTORYEVENT_H

#include <QtOpcUa/qopcuatype.h>

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qcontainerfwd.h>
#include <QtCore/qstringfwd.h>
#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaHistoryEventData;
QT_DECLARE_QESDP_SPECIALIZATION_DTOR_WITH_EXPORT(QOpcUaHistoryEventData, Q_OPCUA_EXPORT)
class QOpcUaHistoryEvent
{
public:
    Q_OPCUA_EXPORT QOpcUaHistoryEvent();
    Q_OPCUA_EXPORT explicit QOpcUaHistoryEvent(const QString &nodeId);
    Q_OPCUA_EXPORT QOpcUaHistoryEvent(const QOpcUaHistoryEvent &other);
    Q_OPCUA_EXPORT QOpcUaHistoryEvent &operator=(const QOpcUaHistoryEvent &other);
    QOpcUaHistoryEvent(QOpcUaHistoryEvent &&other) noexcept = default;
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QOpcUaHistoryEvent)
    Q_OPCUA_EXPORT ~QOpcUaHistoryEvent();

    void swap(QOpcUaHistoryEvent &other) noexcept
    { data.swap(other.data); }

    Q_OPCUA_EXPORT QOpcUa::UaStatusCode statusCode() const;
    Q_OPCUA_EXPORT void setStatusCode(QOpcUa::UaStatusCode statusCode);
    Q_OPCUA_EXPORT QList<QVariantList> events() const;
    Q_OPCUA_EXPORT int count() const;
    Q_OPCUA_EXPORT void addEvent(const QVariantList &value);
    Q_OPCUA_EXPORT QString nodeId() const;
    Q_OPCUA_EXPORT void setNodeId(const QString &nodeId);

private:
    friend Q_OPCUA_EXPORT bool comparesEqual(const QOpcUaHistoryEvent &lhs,
                                             const QOpcUaHistoryEvent &rhs) noexcept;
    friend bool operator==(const QOpcUaHistoryEvent &lhs, const QOpcUaHistoryEvent &rhs) noexcept
    { return comparesEqual(lhs, rhs); }
    friend bool operator!=(const QOpcUaHistoryEvent &lhs, const QOpcUaHistoryEvent &rhs) noexcept
    {
        return !(lhs == rhs);
    }

    QExplicitlySharedDataPointer<QOpcUaHistoryEventData> data;
};

Q_DECLARE_SHARED(QOpcUaHistoryEvent)

QT_END_NAMESPACE

#endif // QOPCUAHISTORYEVENT_H

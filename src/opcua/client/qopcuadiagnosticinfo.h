// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qstringfwd.h>

#ifndef QOPCUADIAGNOSTICINFO_H
#define QOPCUADIAGNOSTICINFO_H

QT_BEGIN_NAMESPACE

class QVariant;

class QOpcUaDiagnosticInfoData;
QT_DECLARE_QESDP_SPECIALIZATION_DTOR_WITH_EXPORT(QOpcUaDiagnosticInfoData, Q_OPCUA_EXPORT)
class QOpcUaDiagnosticInfo {
public:
    Q_OPCUA_EXPORT QOpcUaDiagnosticInfo();
    Q_OPCUA_EXPORT ~QOpcUaDiagnosticInfo();
    Q_OPCUA_EXPORT QOpcUaDiagnosticInfo(const QOpcUaDiagnosticInfo &other);
    QOpcUaDiagnosticInfo(QOpcUaDiagnosticInfo &&other) noexcept = default;
    Q_OPCUA_EXPORT QOpcUaDiagnosticInfo &operator=(const QOpcUaDiagnosticInfo &rhs);
    void swap(QOpcUaDiagnosticInfo &other) noexcept
    { data.swap(other.data); }
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QOpcUaDiagnosticInfo)

    Q_OPCUA_EXPORT operator QVariant() const;

    Q_OPCUA_EXPORT qint32 symbolicId() const;
    Q_OPCUA_EXPORT void setSymbolicId(qint32 newSymbolicId);

    Q_OPCUA_EXPORT bool hasSymbolicId() const;
    Q_OPCUA_EXPORT void setHasSymbolicId(bool newHasSymbolicId);

    Q_OPCUA_EXPORT qint32 namespaceUri() const;
    Q_OPCUA_EXPORT void setNamespaceUri(qint32 newNamespaceUri);

    Q_OPCUA_EXPORT bool hasNamespaceUri() const;
    Q_OPCUA_EXPORT void setHasNamespaceUri(bool newHasNamespaceUri);

    Q_OPCUA_EXPORT qint32 locale() const;
    Q_OPCUA_EXPORT void setLocale(qint32 newLocale);

    Q_OPCUA_EXPORT bool hasLocale() const;
    Q_OPCUA_EXPORT void setHasLocale(bool newHasLocale);

    Q_OPCUA_EXPORT qint32 localizedText() const;
    Q_OPCUA_EXPORT void setLocalizedText(qint32 newLocalizedText);

    Q_OPCUA_EXPORT bool hasLocalizedText() const;
    Q_OPCUA_EXPORT void setHasLocalizedText(bool newHasLocalizedText);

    Q_OPCUA_EXPORT QString additionalInfo() const;
    Q_OPCUA_EXPORT void setAdditionalInfo(const QString &newAdditionalInfo);

    Q_OPCUA_EXPORT bool hasAdditionalInfo() const;
    Q_OPCUA_EXPORT void setHasAdditionalInfo(bool newHasAdditionalInfo);

    Q_OPCUA_EXPORT QOpcUa::UaStatusCode innerStatusCode() const;
    Q_OPCUA_EXPORT void setInnerStatusCode(QOpcUa::UaStatusCode newInnerStatusCode);

    Q_OPCUA_EXPORT bool hasInnerStatusCode() const;
    Q_OPCUA_EXPORT void setHasInnerStatusCode(bool newHasInnerStatusCode);

    Q_OPCUA_EXPORT QOpcUaDiagnosticInfo innerDiagnosticInfo() const;
    Q_OPCUA_EXPORT QOpcUaDiagnosticInfo &innerDiagnosticInfoRef();
    Q_OPCUA_EXPORT void setInnerDiagnosticInfo(const QOpcUaDiagnosticInfo &newInnerDiagnosticInfo);

    Q_OPCUA_EXPORT bool hasInnerDiagnosticInfo() const;
    Q_OPCUA_EXPORT void setHasInnerDiagnosticInfo(bool newHasInnerDiagnosticInfo);

private:
    QExplicitlySharedDataPointer<QOpcUaDiagnosticInfoData> data;

    friend Q_OPCUA_EXPORT bool comparesEqual(const QOpcUaDiagnosticInfo &lhs,
                                             const QOpcUaDiagnosticInfo &rhs) noexcept;
    friend bool operator==(const QOpcUaDiagnosticInfo &lhs,
                           const QOpcUaDiagnosticInfo &rhs) noexcept
    { return comparesEqual(lhs, rhs); }
    friend bool operator!=(const QOpcUaDiagnosticInfo &lhs,
                           const QOpcUaDiagnosticInfo &rhs) noexcept
    {
        return !(lhs == rhs);
    }
};

Q_DECLARE_SHARED(QOpcUaDiagnosticInfo)

QT_END_NAMESPACE

#endif // QOPCUADIAGNOSTICINFO_H

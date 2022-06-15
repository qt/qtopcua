// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QUACPPVALUECONVERTER_H
#define QUACPPVALUECONVERTER_H

#include "qopcuanode.h"
#include "qopcuatype.h"
#include "qopcuaargument.h"

#include <QtCore/QVariant>

#include <uanodeid.h>
#include <uaarraytemplates.h> // for UaStringArray

class UaLocalizedText;
class UaArgument;

QT_BEGIN_NAMESPACE

namespace QUACppValueConverter {
    QOpcUa::Types qvariantTypeToQOpcUaType(QVariant::Type type);

    /*constexpr*/ OpcUa_UInt32 toUaAttributeId(QOpcUa::NodeAttribute attr);

    OpcUa_Variant toUACppVariant(const QVariant&, QOpcUa::Types);
    QVariant toQVariant(const OpcUa_Variant&);
    OpcUa_BuiltInType toDataType(QOpcUa::Types valueType);

    template<typename TARGETTYPE, typename UATYPE>
    QVariant scalarToQVariant(UATYPE *data, QMetaType::Type type = QMetaType::UnknownType);

    template<typename TARGETTYPE, typename UATYPE>
    QVariant arrayToQVariant(const OpcUa_Variant &var, QMetaType::Type type = QMetaType::UnknownType);

    template<typename TARGETTYPE, typename QTTYPE>
    void scalarFromQVariant(const QVariant &var, TARGETTYPE *ptr);

    template<typename TARGETTYPE, typename QTTYPE>
    OpcUa_Variant arrayFromQVariant(const QVariant &var, const OpcUa_BuiltInType type);

    QDateTime toQDateTime(const OpcUa_DateTime *dt);
    OpcUa_DateTime toUACppDateTime(const QDateTime &qtDateTime);

    UaStringArray toUaStringArray(const QStringList &value);
    QOpcUaLocalizedText toQOpcUaLocalizedText(UaLocalizedText *data);

    OpcUa_ExpandedNodeId toUACppExpandedNodeId(const QOpcUaExpandedNodeId &qtExpandedNodeId);
    OpcUa_LocalizedText toUACppLocalizedText(const QOpcUaLocalizedText &qtLocalizedText);
    OpcUa_QualifiedName toUACppQualifiedName(const QOpcUaQualifiedName& qtQualifiedName);

    QOpcUaArgument toQArgument(const UaArgument *data);
}

QT_END_NAMESPACE

#endif // QUACPPVALUECONVERTER_H

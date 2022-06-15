// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUANODECREATIONATTRIBUTES_P_H
#define QOPCUANODECREATIONATTRIBUTES_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qopcuatype.h"

#include <QSharedData>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

class QOpcUaNodeCreationAttributesPrivate : public QSharedData
{
public:
    QOpcUaNodeCreationAttributesPrivate()
        : mask(0)
    {}

    // Bit positions from OPC-UA part 4, 7.19.1
    enum class BitMask : quint32 {
        AccessLevel = 0,
        ArrayDimensions = 1,
        ContainsNoLoops = 3,
        DataType = 4,
        Description = 5,
        DisplayName = 6,
        EventNotifier = 7,
        Executable = 8,
        Historizing = 9,
        InverseName = 10,
        IsAbstract = 11,
        MinimumSamplingInterval = 12,
        Symmetric = 15,
        UserAccessLevel = 16,
        UserExecutable = 17,
        UserWriteMask = 18,
        ValueRank = 19,
        WriteMask = 20,
        Value = 21
    };

    void setAttributeInMask(QOpcUaNodeCreationAttributesPrivate::BitMask attribute)
    {
        const quint32 position = static_cast<quint32>(attribute);
        mask |= (1 << position);
    }

    bool attributeSet(QOpcUaNodeCreationAttributesPrivate::BitMask attribute) const
    {
        const quint32 position = static_cast<quint32>(attribute);
        return mask & (1 << position);
    }

    quint32 mask {0};
    QOpcUa::AccessLevel accessLevel {QOpcUa::AccessLevelBit::None};
    QList<quint32> arrayDimensions;
    bool containsNoLoops {false};
    QString dataTypeId;
    QOpcUaLocalizedText description;
    QOpcUaLocalizedText displayName;
    QOpcUa::EventNotifier eventNotifier {QOpcUa::EventNotifierBit::None};
    bool executable {false};
    bool historizing {false};
    QOpcUaLocalizedText inverseName;
    bool isAbstract {false};
    double minimumSamplingInterval {0};
    bool symmetric {false};
    QOpcUa::AccessLevel userAccessLevel {QOpcUa::AccessLevelBit::None};
    bool userExecutable {false};
    QOpcUa::WriteMask userWriteMask {QOpcUa::WriteMaskBit::None};
    qint32 valueRank {0};
    QOpcUa::WriteMask writeMask {QOpcUa::WriteMaskBit::None};
    QVariant value;
    QOpcUa::Types valueType {QOpcUa::Types::Undefined};
};

QT_END_NAMESPACE

#endif // QOPCUANODECREATIONATTRIBUTES_P_H

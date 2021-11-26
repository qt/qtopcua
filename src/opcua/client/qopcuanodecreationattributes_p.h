/****************************************************************************
**
** Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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

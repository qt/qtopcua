/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QOPEN62541VALUECONVERTER_H
#define QOPEN62541VALUECONVERTER_H

#include "qopen62541.h"
#include <QtOpcUa/qopcuanode.h>
#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

namespace QOpen62541ValueConverter {
    QOpcUa::Types qvariantTypeToQOpcUaType(QVariant::Type type);

    constexpr UA_AttributeId toUaAttributeId(QOpcUaNode::NodeAttribute attr)
    {
        return static_cast<UA_AttributeId>(std::log2(static_cast<std::underlying_type<QOpcUaNode::NodeAttribute>::type>(attr)) + 1);
    }

    UA_Variant toOpen62541Variant(const QVariant&, QOpcUa::Types);
    QVariant toQVariant(const UA_Variant&);
    QOpcUa::Types toQOpcUaVariantType(quint8 typeIndex);
    const UA_DataType *toDataType(QOpcUa::Types valueType);
    QOpcUa::Types qvariantTypeToQOpcUaType(QMetaType::Type type);

    QString toQString(UA_String value);

    template<typename TARGETTYPE, typename UATYPE>
    QVariant scalarToQVariant(UATYPE *data, QMetaType::Type type = QMetaType::UnknownType);

    template<typename TARGETTYPE, typename UATYPE>
    QVariant arrayToQVariant(const UA_Variant &var, QMetaType::Type type = QMetaType::UnknownType);

    template<typename TARGETTYPE, typename QTTYPE>
    void scalarFromQVariant(const QVariant &var, TARGETTYPE *ptr);

    template<typename TARGETTYPE, typename QTTYPE>
    UA_Variant arrayFromQVariant(const QVariant &var, const UA_DataType *type);
}

QT_END_NAMESPACE

#endif // QOPEN62541VALUECONVERTER_H

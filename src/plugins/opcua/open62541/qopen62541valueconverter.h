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
#include <private/qopcuabinarydataencoding_p.h>

#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

namespace QOpen62541ValueConverter {
    QOpcUa::Types qvariantTypeToQOpcUaType(QVariant::Type type);

    inline UA_AttributeId toUaAttributeId(QOpcUa::NodeAttribute attr)
    {
        const int attributeIdUsedBits = 22;
        for (int i = 0; i < attributeIdUsedBits; ++i)
            if (static_cast<int>(attr) == (1 << i))
                return static_cast<UA_AttributeId>(i + 1);

        return static_cast<UA_AttributeId>(0);
    }

    UA_Variant toOpen62541Variant(const QVariant&, QOpcUa::Types);
    QVariant toQVariant(const UA_Variant&);
    const UA_DataType *toDataType(QOpcUa::Types valueType);
    QOpcUa::Types qvariantTypeToQOpcUaType(QMetaType::Type type);

    template<typename TARGETTYPE, typename UATYPE>
    TARGETTYPE scalarToQt(UATYPE *data);

    template<typename TARGETTYPE, typename UATYPE>
    QVariant arrayToQVariant(const UA_Variant &var, QMetaType::Type type = QMetaType::UnknownType);

    template<typename TARGETTYPE, typename QTTYPE>
    void scalarFromQt(const QTTYPE &var, TARGETTYPE *ptr);

    template<typename TARGETTYPE, typename QTTYPE>
    UA_Variant arrayFromQVariant(const QVariant &var, const UA_DataType *type);

    void createExtensionObject(QByteArray &data, QOpcUaBinaryDataEncoding::TypeEncodingId id, UA_ExtensionObject *ptr);
}

QT_END_NAMESPACE

#endif // QOPEN62541VALUECONVERTER_H

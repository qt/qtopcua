/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#ifndef QOPEN62541VALUECONVERTER_H
#define QOPEN62541VALUECONVERTER_H

#include "qopen62541.h"
#include <QtOpcUa/qopcuanode.h>
#include <QtOpcUa/qopcuatype.h>
#include <QtOpcUa/qopcuabinarydataencoding.h>

#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

namespace QOpen62541ValueConverter {

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
    TARGETTYPE scalarToQt(const UATYPE *data);

    template<typename TARGETTYPE, typename UATYPE>
    QVariant arrayToQVariant(const UA_Variant &var, QMetaType::Type type = QMetaType::UnknownType);

    template<typename TARGETTYPE, typename QTTYPE>
    void scalarFromQt(const QTTYPE &var, TARGETTYPE *ptr);

    template<typename TARGETTYPE, typename QTTYPE>
    UA_Variant arrayFromQVariant(const QVariant &var, const UA_DataType *type);

    void createExtensionObject(QByteArray &data, const UA_NodeId &typeEncodingId, UA_ExtensionObject *ptr,
                               QOpcUaExtensionObject::Encoding encoding = QOpcUaExtensionObject::Encoding::ByteString);
}

QT_END_NAMESPACE

#endif // QOPEN62541VALUECONVERTER_H

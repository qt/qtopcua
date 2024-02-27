// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPEN62541VALUECONVERTER_H
#define QOPEN62541VALUECONVERTER_H

#include "qopen62541.h"
#include <QtOpcUa/qopcuanode.h>
#include <QtOpcUa/qopcuatype.h>
#include <QtOpcUa/qopcuabinarydataencoding.h>
#include <QtOpcUa/qopcuaextensionobject.h>

#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

namespace QOpen62541ValueConverter {

    inline UA_AttributeId toUaAttributeId(QOpcUa::NodeAttribute attr)
    {
        const int attributeIdUsedBits = 23;
        for (int i = 0; i < attributeIdUsedBits; ++i)
            if (static_cast<int>(attr) == (1 << i))
                return static_cast<UA_AttributeId>(i + 1);

        return static_cast<UA_AttributeId>(0);
    }

    inline QOpcUa::NodeAttribute toQtAttributeId(UA_AttributeId attr)
    {
        if (attr > 0 && attr <= UA_ATTRIBUTEID_DATATYPEDEFINITION)
            return QOpcUa::NodeAttribute(1 << (attr - 1));

        return QOpcUa::NodeAttribute::None;
    }

    UA_Variant toOpen62541Variant(const QVariant&, QOpcUa::Types);
    QVariant toQVariant(const UA_Variant&);
    const UA_DataType *toDataType(QOpcUa::Types valueType);
    QOpcUa::Types toQtDataType(const UA_DataType *type);
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

    QVariant uaVariantToQtExtensionObject(const UA_Variant &var);
    QOpcUaExtensionObject encodeAsBinaryExtensionObject(const void *data, const UA_DataType *type, bool *success = nullptr);
}

QT_END_NAMESPACE

#endif // QOPEN62541VALUECONVERTER_H

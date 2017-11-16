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

#include "qopen62541.h"
#include "qopen62541utils.h"
#include "qopen62541valueconverter.h"

#include <QtCore/qdatetime.h>
#include <QtCore/qloggingcategory.h>
#include <QtCore/quuid.h>

#include <cstring>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_OPEN62541)

namespace QOpen62541ValueConverter {

QOpcUa::Types qvariantTypeToQOpcUaType(QMetaType::Type type)
{
    switch (type) {
    case QMetaType::Bool:
        return QOpcUa::Boolean;
    case QMetaType::UChar:
        return QOpcUa::Byte;
    case QMetaType::Char:
        return QOpcUa::SByte;
    case QMetaType::UShort:
        return QOpcUa::UInt16;
    case QMetaType::Short:
        return QOpcUa::Int16;
    case QMetaType::Int:
        return QOpcUa::Int32;
    case QMetaType::UInt:
        return QOpcUa::UInt32;
    case QMetaType::ULongLong:
        return QOpcUa::UInt64;
    case QMetaType::LongLong:
        return QOpcUa::Int64;
    case QMetaType::Double:
        return QOpcUa::Double;
    case QMetaType::Float:
        return QOpcUa::Float;
    case QMetaType::QString:
        return QOpcUa::String;
    //return QOpcUa::LocalizedText;  // TODO: unclear
    case QMetaType::QDateTime:
        return QOpcUa::DateTime;
    case QMetaType::QByteArray:
        return QOpcUa::ByteString;
    case QMetaType::QUuid:
        return QOpcUa::Guid;
    //return QOpcUa::XmlElement;
    //return QOpcUa::NodeId;
    default:
        break;
    }

    return QOpcUa::Undefined;
}

UA_Variant toOpen62541Variant(const QVariant &value, QOpcUa::Types type)
{
    UA_Variant open62541value;
    UA_Variant_init(&open62541value);


    if (value.type() == QVariant::List && value.toList().size() == 0)
        return open62541value;

    QVariant temp = (value.type() == QVariant::List) ? value.toList().at(0) : value;
    QOpcUa::Types valueType = type == QOpcUa::Undefined ?
                qvariantTypeToQOpcUaType(static_cast<QMetaType::Type>(temp.type())) : type;

    const UA_DataType *dt = toDataType(valueType);

    switch (valueType) {
    case QOpcUa::Boolean:
        return arrayFromQVariant<UA_Boolean, bool>(value, dt);
    case QOpcUa::SByte:
        return arrayFromQVariant<UA_SByte, char>(value, dt);
    case QOpcUa::Byte:
        return arrayFromQVariant<UA_Byte, uchar>(value, dt);
    case QOpcUa::Int16:
        return arrayFromQVariant<UA_Int16, qint16>(value, dt);
    case QOpcUa::UInt16:
        return arrayFromQVariant<UA_UInt16, quint16>(value, dt);
    case QOpcUa::Int32:
        return arrayFromQVariant<UA_Int32, qint32>(value, dt);
    case QOpcUa::UInt32:
        return arrayFromQVariant<UA_UInt32, quint32>(value, dt);
    case QOpcUa::Int64:
        return arrayFromQVariant<UA_Int64, int64_t>(value, dt);
    case QOpcUa::UInt64:
        return arrayFromQVariant<UA_UInt64, uint64_t>(value, dt);
    case QOpcUa::Float:
        return arrayFromQVariant<UA_Float, float>(value, dt);
    case QOpcUa::Double:
        return arrayFromQVariant<UA_Double, double>(value, dt);
    case QOpcUa::DateTime:
        return arrayFromQVariant<UA_DateTime, QDateTime>(value, dt);
    case QOpcUa::String:
        return arrayFromQVariant<UA_String, QString>(value, dt);
    case QOpcUa::LocalizedText:
        return arrayFromQVariant<UA_LocalizedText, QString>(value, dt);
    case QOpcUa::ByteString:
        return arrayFromQVariant<UA_ByteString, QByteArray>(value, dt);
    case QOpcUa::NodeId:
        return arrayFromQVariant<UA_NodeId, QString>(value, dt);
    case QOpcUa::Guid:
        return arrayFromQVariant<UA_Guid, QUuid>(value, dt);
    case QOpcUa::XmlElement:
        return arrayFromQVariant<UA_XmlElement, QString>(value, dt);
    case QOpcUa::QualifiedName:
        return arrayFromQVariant<UA_QualifiedName, QOpcUa::QQualifiedName>(value, dt);
    default:
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Variant conversion to Open62541 for typeIndex" << type << " not implemented";
    }

    return open62541value;
}

QVariant toQVariant(const UA_Variant &value)
{
    switch (value.type->typeIndex) {
    case UA_TYPES_BOOLEAN:
        return arrayToQVariant<bool, UA_Boolean>(value, QMetaType::Bool);
    case UA_TYPES_SBYTE:
        return arrayToQVariant<signed char, UA_SByte>(value, QMetaType::SChar);
    case UA_TYPES_BYTE:
        return arrayToQVariant<uchar, UA_Byte>(value, QMetaType::UChar);
    case UA_TYPES_INT16:
        return arrayToQVariant<qint16, UA_Int16>(value, QMetaType::Short);
    case UA_TYPES_UINT16:
        return arrayToQVariant<quint16, UA_UInt16>(value, QMetaType::UShort);
    case UA_TYPES_INT32:
        return arrayToQVariant<qint32, UA_Int32>(value, QMetaType::Int);
    case UA_TYPES_UINT32:
        return arrayToQVariant<quint32, UA_UInt32>(value, QMetaType::UInt);
    case UA_TYPES_INT64:
        return arrayToQVariant<int64_t, UA_Int64>(value, QMetaType::LongLong);
    case UA_TYPES_UINT64:
        return arrayToQVariant<uint64_t, UA_UInt64>(value, QMetaType::ULongLong);
    case UA_TYPES_FLOAT:
        return arrayToQVariant<float, UA_Float>(value, QMetaType::Float);
    case UA_TYPES_DOUBLE:
        return arrayToQVariant<double, UA_Double>(value, QMetaType::Double);
    case UA_TYPES_STRING:
        return arrayToQVariant<QString, UA_String>(value, QMetaType::QString);
    case UA_TYPES_BYTESTRING:
        return arrayToQVariant<QByteArray, UA_ByteString>(value, QMetaType::QByteArray);
    case UA_TYPES_LOCALIZEDTEXT:
        return arrayToQVariant<QString, UA_LocalizedText>(value, QMetaType::QString);
    case UA_TYPES_NODEID:
        return arrayToQVariant<QString, UA_NodeId>(value, QMetaType::QString);
    case UA_TYPES_DATETIME:
        return arrayToQVariant<QDateTime, UA_DateTime>(value, QMetaType::QDateTime);
    case UA_TYPES_GUID:
        return arrayToQVariant<QUuid, UA_Guid>(value, QMetaType::QUuid);
    case UA_TYPES_XMLELEMENT:
        return arrayToQVariant<QString, UA_XmlElement>(value, QMetaType::QString);
    case UA_TYPES_QUALIFIEDNAME:
        return arrayToQVariant<QOpcUa::QQualifiedName, UA_QualifiedName>(value);
    default:
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Variant conversion from Open62541 for typeIndex" << value.type->typeIndex << " not implemented";
        return QVariant();
    }
    return QVariant();
}

QOpcUa::Types toQOpcUaVariantType(quint8 typeIndex)
{
    switch (typeIndex) {
    case UA_TYPES_BOOLEAN:
        return QOpcUa::Boolean;
    case UA_TYPES_SBYTE:
        return QOpcUa::SByte;
    case UA_TYPES_BYTE:
        return QOpcUa::Byte;
    case UA_TYPES_INT16:
        return QOpcUa::Int16;
    case UA_TYPES_UINT16:
        return QOpcUa::UInt16;
    case UA_TYPES_INT32:
        return QOpcUa::Int32;
    case UA_TYPES_UINT32:
        return QOpcUa::UInt32;
    case UA_TYPES_INT64:
        return QOpcUa::Int64;
    case UA_TYPES_UINT64:
        return QOpcUa::UInt64;
    default:
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Variant conversion to Qt type " << typeIndex << " not implemented";
        return QOpcUa::Undefined;
    }
    return QOpcUa::Undefined;
}

const UA_DataType *toDataType(QOpcUa::Types valueType)
{
    switch (valueType) {
    case QOpcUa::Boolean:
        return &UA_TYPES[UA_TYPES_BOOLEAN];
    case QOpcUa::Int32:
        return &UA_TYPES[UA_TYPES_INT32];
    case QOpcUa::UInt32:
        return &UA_TYPES[UA_TYPES_UINT32];
    case QOpcUa::Double:
        return &UA_TYPES[UA_TYPES_DOUBLE];
    case QOpcUa::Float:
        return &UA_TYPES[UA_TYPES_FLOAT];
    case QOpcUa::String:
        return &UA_TYPES[UA_TYPES_STRING];
    case QOpcUa::LocalizedText:
        return &UA_TYPES[UA_TYPES_LOCALIZEDTEXT];
    case QOpcUa::DateTime:
        return &UA_TYPES[UA_TYPES_DATETIME];
    case QOpcUa::UInt16:
        return &UA_TYPES[UA_TYPES_UINT16];
    case QOpcUa::Int16:
        return &UA_TYPES[UA_TYPES_INT16];
    case QOpcUa::UInt64:
        return &UA_TYPES[UA_TYPES_UINT64];
    case QOpcUa::Int64:
        return &UA_TYPES[UA_TYPES_INT64];
    case QOpcUa::Byte:
        return &UA_TYPES[UA_TYPES_BYTE];
    case QOpcUa::SByte:
        return &UA_TYPES[UA_TYPES_SBYTE];
    case QOpcUa::ByteString:
        return &UA_TYPES[UA_TYPES_BYTESTRING];
    case QOpcUa::XmlElement:
        return &UA_TYPES[UA_TYPES_XMLELEMENT];
    case QOpcUa::NodeId:
        return &UA_TYPES[UA_TYPES_NODEID];
    case QOpcUa::Guid:
        return &UA_TYPES[UA_TYPES_GUID];
    case QOpcUa::QualifiedName:
        return &UA_TYPES[UA_TYPES_QUALIFIEDNAME];
    default:
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Trying to convert undefined type:" << valueType;
        return nullptr;
    }
}

QString toQString(UA_String value)
{
    return QString::fromUtf8((const char *)value.data, value.length);
}

template<typename TARGETTYPE, typename UATYPE>
QVariant scalarToQVariant(UATYPE *data, QMetaType::Type type)
{
    return QVariant(type, reinterpret_cast<TARGETTYPE *>(data));
}

template<>
QVariant scalarToQVariant<QString, UA_String>(UA_String *data, QMetaType::Type type)
{
    Q_UNUSED(type)
    UA_String *uaStr = static_cast<UA_String *>(data);
    return QVariant(QString::fromUtf8(reinterpret_cast<char *>(uaStr->data), uaStr->length));
}

template<>
QVariant scalarToQVariant<QByteArray, UA_ByteString>(UA_ByteString *data, QMetaType::Type type)
{
    Q_UNUSED(type)
    UA_ByteString *uaBs = static_cast<UA_ByteString *>(data);
    return QVariant(QByteArray(reinterpret_cast<char *>(uaBs->data), uaBs->length));
}

template<>
QVariant scalarToQVariant<QString, UA_LocalizedText>(UA_LocalizedText *data, QMetaType::Type type)
{
    Q_UNUSED(type)
    UA_LocalizedText *uaLt = static_cast<UA_LocalizedText *>(data);
    return QVariant(QString::fromUtf8(reinterpret_cast<char *>(uaLt->text.data),
                                      uaLt->text.length));
}

template<>
QVariant scalarToQVariant<QString, UA_NodeId>(UA_NodeId *data, QMetaType::Type type)
{
    Q_UNUSED(type)
    UA_NodeId *uan = static_cast<UA_NodeId *>(data);
    return Open62541Utils::nodeIdToQString(*uan);
}

template<>
QVariant scalarToQVariant<QDateTime, UA_DateTime>(UA_DateTime *data, QMetaType::Type type)
{
    Q_UNUSED(type)
    return QVariant(QDateTime::fromMSecsSinceEpoch(*static_cast<UA_DateTime *>(data) * UA_DATETIME_TO_MSEC));
}

template<>
QVariant scalarToQVariant<QUuid, UA_Guid>(UA_Guid *data, QMetaType::Type type)
{
    Q_UNUSED(type)
    return QUuid(data->data1, data->data2, data->data3, data->data4[0], data->data4[1], data->data4[2],
            data->data4[3], data->data4[4], data->data4[5], data->data4[6], data->data4[7]);
}

template<>
QVariant scalarToQVariant<QOpcUa::QQualifiedName, UA_QualifiedName>(UA_QualifiedName *data, QMetaType::Type type)
{
    Q_UNUSED(type);
    QOpcUa::QQualifiedName temp;
    temp.namespaceIndex = data->namespaceIndex;
    temp.name = scalarToQVariant<QString, UA_String>(&(data->name), QMetaType::Type::QString).toString();
    return QVariant::fromValue(temp);
}

template<typename TARGETTYPE, typename UATYPE>
QVariant arrayToQVariant(const UA_Variant &var, QMetaType::Type type)
{
    if (var.arrayLength > 1) {
        QVariantList list;
        for (size_t i = 0; i < var.arrayLength; ++i) {
            UATYPE *temp = static_cast<UATYPE *>(var.data);
            list.append(scalarToQVariant<TARGETTYPE, UATYPE>(&temp[i], type));
        }
        return list;
    }
    UATYPE *temp = static_cast<UATYPE *>(var.data);
    return scalarToQVariant<TARGETTYPE, UATYPE>(temp, type);
}

template<typename TARGETTYPE, typename QTTYPE>
void scalarFromQVariant(const QVariant &var, TARGETTYPE *ptr)
{
    *ptr = static_cast<TARGETTYPE>(var.value<QTTYPE>());
}

template<>
void scalarFromQVariant<UA_DateTime, QDateTime>(const QVariant &var, UA_DateTime *ptr)
{
    *ptr = UA_MSEC_TO_DATETIME * var.toDateTime().toMSecsSinceEpoch();
}

template<>
void scalarFromQVariant<UA_String, QString>(const QVariant &var, UA_String *ptr)
{
    UA_String tmpValue = UA_String_fromChars(var.toString().toUtf8().constData());
    UA_String_copy(&tmpValue, ptr);
    UA_String_deleteMembers(&tmpValue);
}

template<>
void scalarFromQVariant<UA_LocalizedText, QString>(const QVariant &var, UA_LocalizedText *ptr)
{
    UA_LocalizedText tmpValue;
    UA_String_init(&tmpValue.locale);
    tmpValue.text = UA_String_fromChars(var.toString().toUtf8().constData());
    UA_LocalizedText_copy(&tmpValue, ptr);
    UA_LocalizedText_deleteMembers(&tmpValue);
}

template<>
void scalarFromQVariant<UA_ByteString, QByteArray>(const QVariant &var, UA_ByteString *ptr)
{
    QByteArray arr = var.toByteArray();
    UA_ByteString tmpValue;
    UA_ByteString_init(&tmpValue);
    tmpValue.length = arr.length();
    tmpValue.data = reinterpret_cast<UA_Byte *>(arr.data());
    UA_ByteString_copy(&tmpValue, ptr);
}

template<>
void scalarFromQVariant<UA_NodeId, QString>(const QVariant &var, UA_NodeId *ptr)
{
    UA_NodeId tmpValue = Open62541Utils::nodeIdFromQString(var.toString());
    UA_NodeId_copy(&tmpValue, ptr);
    UA_NodeId_deleteMembers(&tmpValue);
}

template<>
void scalarFromQVariant<UA_QualifiedName, QOpcUa::QQualifiedName>(const QVariant &var, UA_QualifiedName *ptr)
{
    QOpcUa::QQualifiedName temp = var.value<QOpcUa::QQualifiedName>();
    ptr->namespaceIndex = temp.namespaceIndex;
    scalarFromQVariant<UA_String, QString>(temp.name, &(ptr->name));
}

template<>
void scalarFromQVariant<UA_Guid, QUuid>(const QVariant &var, UA_Guid *ptr)
{
    const QUuid uuid = var.toUuid();
    ptr->data1 = uuid.data1;
    ptr->data2 = uuid.data2;
    ptr->data3 = uuid.data3;
    std::memcpy(ptr->data4, uuid.data4, sizeof(uuid.data4));
}

template<typename TARGETTYPE, typename QTTYPE>
UA_Variant arrayFromQVariant(const QVariant &var, const UA_DataType *type)
{
    UA_Variant open62541value;
    UA_Variant_init(&open62541value);

    if (var.type() == QVariant::List) {
        const QVariantList list = var.toList();
        if (list.isEmpty())
            return open62541value;

        TARGETTYPE *arr = static_cast<TARGETTYPE *>(UA_Array_new(list.size(), type));

        for (int i = 0; i < list.size(); ++i)
            scalarFromQVariant<TARGETTYPE, QTTYPE>(list[i], &arr[i]);

        UA_Variant_setArray(&open62541value, arr, list.size(), type);
        return open62541value;
    }

    TARGETTYPE *temp = static_cast<TARGETTYPE *>(UA_new(type));
    scalarFromQVariant<TARGETTYPE, QTTYPE>(var, temp);
    UA_Variant_setScalar(&open62541value, temp, type);
    return open62541value;
}

}

QT_END_NAMESPACE

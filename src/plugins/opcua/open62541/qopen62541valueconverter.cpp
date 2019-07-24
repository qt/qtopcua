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

using namespace QOpcUa::NodeIds;

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
    case QMetaType::QDateTime:
        return QOpcUa::DateTime;
    case QMetaType::QByteArray:
        return QOpcUa::ByteString;
    case QMetaType::QUuid:
        return QOpcUa::Guid;
    default:
        break;
    }

    return QOpcUa::Undefined;
}

UA_Variant toOpen62541Variant(const QVariant &value, QOpcUa::Types type)
{
    UA_Variant open62541value;
    UA_Variant_init(&open62541value);

    if (value.canConvert<QOpcUa::QMultiDimensionalArray>()) {
        QOpcUa::QMultiDimensionalArray data = value.value<QOpcUa::QMultiDimensionalArray>();
        UA_Variant result = toOpen62541Variant(data.valueArray(), type);

        if (!data.arrayDimensions().isEmpty()) {
            // Ensure that the array dimensions size is < UINT32_MAX
            if (static_cast<quint64>(data.arrayDimensions().size()) > (std::numeric_limits<quint32>::max)())
                return open62541value;
            result.arrayDimensionsSize = data.arrayDimensions().size();
            result.arrayDimensions = static_cast<UA_UInt32 *>(UA_Array_new(result.arrayDimensionsSize, &UA_TYPES[UA_TYPES_UINT32]));
            std::copy(data.arrayDimensions().constBegin(), data.arrayDimensions().constEnd(), result.arrayDimensions);
        }
        return result;
    }

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
        return arrayFromQVariant<UA_LocalizedText, QOpcUa::QLocalizedText>(value, dt);
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
    case QOpcUa::StatusCode:
        return arrayFromQVariant<UA_StatusCode, QOpcUa::UaStatusCode>(value, dt);
    case QOpcUa::Range:
        return arrayFromQVariant<UA_ExtensionObject, QOpcUa::QRange>(value, dt);
    case QOpcUa::EUInformation:
        return arrayFromQVariant<UA_ExtensionObject, QOpcUa::QEUInformation>(value, dt);
    case QOpcUa::ComplexNumber:
        return arrayFromQVariant<UA_ExtensionObject, QOpcUa::QComplexNumber>(value, dt);
    case QOpcUa::DoubleComplexNumber:
        return arrayFromQVariant<UA_ExtensionObject, QOpcUa::QDoubleComplexNumber>(value, dt);
    case QOpcUa::AxisInformation:
        return arrayFromQVariant<UA_ExtensionObject, QOpcUa::QAxisInformation>(value, dt);
    case QOpcUa::XV:
        return arrayFromQVariant<UA_ExtensionObject, QOpcUa::QXValue>(value, dt);
    case QOpcUa::ExpandedNodeId:
        return arrayFromQVariant<UA_ExpandedNodeId, QOpcUa::QExpandedNodeId>(value, dt);
    case QOpcUa::Argument:
        return arrayFromQVariant<UA_Argument, QOpcUa::QArgument>(value, dt);
    case QOpcUa::ExtensionObject:
        return arrayFromQVariant<UA_ExtensionObject, QOpcUa::QExtensionObject>(value, dt);
    default:
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Variant conversion to Open62541 for typeIndex" << type << " not implemented";
    }

    return open62541value;
}

QVariant toQVariant(const UA_Variant &value)
{
    if (value.type == nullptr) {
        return QVariant();
    }

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
        return arrayToQVariant<QOpcUa::QLocalizedText, UA_LocalizedText>(value);
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
    case UA_TYPES_STATUSCODE:
        return arrayToQVariant<QOpcUa::UaStatusCode, UA_StatusCode>(value, QMetaType::UInt);
    case UA_TYPES_EXTENSIONOBJECT:
        return arrayToQVariant<QVariant, UA_ExtensionObject>(value);
    case UA_TYPES_EXPANDEDNODEID:
        return arrayToQVariant<QOpcUa::QExpandedNodeId, UA_ExpandedNodeId>(value);
    case UA_TYPES_ARGUMENT:
        return arrayToQVariant<QOpcUa::QArgument, UA_Argument>(value);
    default:
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Variant conversion from Open62541 for typeIndex" << value.type->typeIndex << " not implemented";
        return QVariant();
    }
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
    case QOpcUa::StatusCode:
        return &UA_TYPES[UA_TYPES_STATUSCODE];
    case QOpcUa::Range:
    case QOpcUa::EUInformation:
    case QOpcUa::ComplexNumber:
    case QOpcUa::DoubleComplexNumber:
    case QOpcUa::AxisInformation:
    case QOpcUa::XV:
    case QOpcUa::ExtensionObject:
        return &UA_TYPES[UA_TYPES_EXTENSIONOBJECT];
    case QOpcUa::ExpandedNodeId:
        return &UA_TYPES[UA_TYPES_EXPANDEDNODEID];
    case QOpcUa::Argument:
        return &UA_TYPES[UA_TYPES_ARGUMENT];
    default:
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Trying to convert undefined type:" << valueType;
        return nullptr;
    }
}

template<typename TARGETTYPE, typename UATYPE>
TARGETTYPE scalarToQt(const UATYPE *data)
{
    return *reinterpret_cast<const TARGETTYPE *>(data);
}

template<>
QString scalarToQt<QString, UA_String>(const UA_String *data)
{
    return QString::fromUtf8(reinterpret_cast<const char *>(data->data), data->length);
}

template<>
QByteArray scalarToQt<QByteArray, UA_ByteString>(const UA_ByteString *data)
{
    return QByteArray(reinterpret_cast<const char *>(data->data), data->length);
}

template<>
QOpcUa::QLocalizedText scalarToQt<QOpcUa::QLocalizedText, UA_LocalizedText>(const UA_LocalizedText *data)
{
    QOpcUa::QLocalizedText lt;
    lt.setLocale(scalarToQt<QString, UA_String>(&(data->locale)));
    lt.setText(scalarToQt<QString, UA_String>(&(data->text)));
    return lt;
}

template<>
QString scalarToQt<QString, UA_NodeId>(const UA_NodeId *data)
{
    return Open62541Utils::nodeIdToQString(*data);
}

template<>
QDateTime scalarToQt<QDateTime, UA_DateTime>(const UA_DateTime *data)
{
    // OPC-UA part 3, Table C.9
    const QDateTime epochStart(QDate(1601, 1, 1), QTime(0, 0), Qt::UTC);
    return epochStart.addMSecs(*data / UA_DATETIME_MSEC).toLocalTime();
}

template<>
QUuid scalarToQt<QUuid, UA_Guid>(const UA_Guid *data)
{
    return QUuid(data->data1, data->data2, data->data3, data->data4[0], data->data4[1], data->data4[2],
            data->data4[3], data->data4[4], data->data4[5], data->data4[6], data->data4[7]);
}

template<>
QOpcUa::QQualifiedName scalarToQt<QOpcUa::QQualifiedName, UA_QualifiedName>(const UA_QualifiedName *data)
{
    QOpcUa::QQualifiedName temp;
    temp.setNamespaceIndex(data->namespaceIndex);
    temp.setName(scalarToQt<QString, UA_String>(&(data->name)));
    return temp;
}

template<>
QOpcUa::QArgument scalarToQt<QOpcUa::QArgument, UA_Argument>(const UA_Argument *data)
{
    QOpcUa::QArgument temp;
    temp.setValueRank(data->valueRank);
    temp.setDataTypeId(Open62541Utils::nodeIdToQString(data->dataType));
    temp.setName(scalarToQt<QString, UA_String>(&data->name));
    temp.setDescription(scalarToQt<QOpcUa::QLocalizedText, UA_LocalizedText>(&data->description));
    for (size_t i = 0; i < data->arrayDimensionsSize; ++i)
        temp.arrayDimensionsRef().append(data->arrayDimensions[i]);
    return temp;
}

template <>
QVariant scalarToQt<QVariant, UA_ExtensionObject>(const UA_ExtensionObject *data)
{
    // OPC-UA part 6, Table 13 states that an extension object can have no body, a ByteString encoded body
    // or an XML encoded body.

    // Handle extension object without body
    if (data->encoding == UA_EXTENSIONOBJECT_ENCODED_NOBODY) {
        QOpcUa::QExtensionObject obj;
        obj.setEncoding(QOpcUa::QExtensionObject::Encoding::NoBody);
        return QVariant::fromValue(obj);
    }

    // Some types are automatically decoded by open62541. In this case, the encoding is UA_EXTENSIONOBJECT_DECODED
    if (data->encoding != UA_EXTENSIONOBJECT_ENCODED_XML && data->encoding != UA_EXTENSIONOBJECT_ENCODED_BYTESTRING) {

        if (data->content.decoded.type == &UA_TYPES[UA_TYPES_ARGUMENT] && data->content.decoded.data != nullptr) {
            return scalarToQt<QOpcUa::QArgument, UA_Argument>(reinterpret_cast<UA_Argument *>(data->content.decoded.data));
        }

        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Unsupported decoded extension object type, unable to convert";
        return QVariant();
    }

    QByteArray buffer = QByteArray::fromRawData(reinterpret_cast<const char *>(data->content.encoded.body.data),
                                                data->content.encoded.body.length);

    // Decode recognized types, as required by OPC-UA part 4, 5.2.2.15
    if (data->content.encoded.typeId.identifierType == UA_NODEIDTYPE_NUMERIC &&
            data->content.encoded.typeId.namespaceIndex == 0 &&
            data->encoding == UA_EXTENSIONOBJECT_ENCODED_BYTESTRING) {

        QOpcUaBinaryDataEncoding decoder(&buffer);

        bool success = false;
        QVariant result;
        Namespace0 objType = Namespace0(data->content.encoded.typeId.identifier.numeric);
        switch (objType) {
        case Namespace0::EUInformation_Encoding_DefaultBinary:
            result = decoder.decode<QOpcUa::QEUInformation>(success);
            break;
        case Namespace0::Range_Encoding_DefaultBinary:
            result = decoder.decode<QOpcUa::QRange>(success);
            break;
        case Namespace0::ComplexNumberType_Encoding_DefaultBinary:
            result = decoder.decode<QOpcUa::QComplexNumber>(success);
            break;
        case Namespace0::DoubleComplexNumberType_Encoding_DefaultBinary:
            result = decoder.decode<QOpcUa::QDoubleComplexNumber>(success);
            break;
        case Namespace0::AxisInformation_Encoding_DefaultBinary:
            result = decoder.decode<QOpcUa::QAxisInformation>(success);
            break;
        case Namespace0::XVType_Encoding_DefaultBinary:
            result = decoder.decode<QOpcUa::QXValue>(success);
            break;
        default:
            break;
        }
        if (success)
            return result;
    }

    // Return extension objects with binary or XML body as QOpcUa::QExtensionObject
    QOpcUa::QExtensionObject obj;
    obj.setEncoding(static_cast<QOpcUa::QExtensionObject::Encoding>(data->encoding));
    obj.setEncodingTypeId(Open62541Utils::nodeIdToQString(data->content.encoded.typeId));
    obj.setEncodedBody(QByteArray(buffer.constData(), buffer.size()));
    return obj;
}

template<>
QOpcUa::QExpandedNodeId scalarToQt<QOpcUa::QExpandedNodeId, UA_ExpandedNodeId>(const UA_ExpandedNodeId *data)
{
    QOpcUa::QExpandedNodeId temp;
    temp.setServerIndex(data->serverIndex);
    temp.setNodeId(Open62541Utils::nodeIdToQString(data->nodeId));
    temp.setNamespaceUri(scalarToQt<QString, UA_String>(&data->namespaceUri));
    return temp;
}

template<typename TARGETTYPE, typename UATYPE>
QVariant arrayToQVariant(const UA_Variant &var, QMetaType::Type type)
{
    UATYPE *temp = static_cast<UATYPE *>(var.data);

    if (var.arrayLength > 0) {
        QVariantList list;
        for (size_t i = 0; i < var.arrayLength; ++i) {
            QVariant tempVar = QVariant::fromValue(scalarToQt<TARGETTYPE, UATYPE>(&temp[i]));
            if (type != QMetaType::UnknownType && type != static_cast<QMetaType::Type>(tempVar.type()))
                tempVar.convert(type);
            list.append(tempVar);
        }

        if (var.arrayDimensionsSize > 0) {
            // Ensure that the array dimensions fit in a QVector
            if (var.arrayDimensionsSize > static_cast<quint64>((std::numeric_limits<int>::max)()))
                return QOpcUa::QMultiDimensionalArray();
            QVector<quint32> arrayDimensions;
            std::copy(var.arrayDimensions, var.arrayDimensions+var.arrayDimensionsSize, std::back_inserter(arrayDimensions));
            return QOpcUa::QMultiDimensionalArray(list, arrayDimensions);
        }

        if (list.size() == 1)
            return list.at(0);
        else
            return list;
    } else if (UA_Variant_isScalar(&var)) {
        QVariant tempVar = QVariant::fromValue(scalarToQt<TARGETTYPE, UATYPE>(temp));
        if (type != QMetaType::UnknownType && type != static_cast<QMetaType::Type>(tempVar.type()))
            tempVar.convert(type);
        return tempVar;
    } else if (var.arrayLength == 0 && var.data == UA_EMPTY_ARRAY_SENTINEL) {
        return QVariantList(); // Return empty QVariantList for empty array
    }

    return QVariant(); // Return empty QVariant for empty scalar variant
}

template<typename TARGETTYPE, typename QTTYPE>
void scalarFromQt(const QTTYPE &value, TARGETTYPE *ptr)
{
    *ptr = static_cast<TARGETTYPE>(value);
}

template<>
void scalarFromQt<UA_DateTime, QDateTime>(const QDateTime &value, UA_DateTime *ptr)
{
    // OPC-UA part 3, Table C.9
    const QDateTime uaEpochStart(QDate(1601, 1, 1), QTime(0, 0), Qt::UTC);

    *ptr = UA_DATETIME_MSEC * (value.toMSecsSinceEpoch() - uaEpochStart.toMSecsSinceEpoch());
}

template<>
void scalarFromQt<UA_String, QString>(const QString &value, UA_String *ptr)
{
    *ptr = UA_STRING_ALLOC(value.toUtf8().constData());
}

template<>
void scalarFromQt<UA_LocalizedText, QOpcUa::QLocalizedText>(const QOpcUa::QLocalizedText &value, UA_LocalizedText *ptr)
{
    scalarFromQt<UA_String, QString>(value.locale(), &(ptr->locale));
    scalarFromQt<UA_String, QString>(value.text(), &(ptr->text));
}

template<>
void scalarFromQt<UA_ByteString, QByteArray>(const QByteArray &value, UA_ByteString *ptr)
{
    ptr->length = value.length();
    UA_StatusCode success = UA_Array_copy(reinterpret_cast<const UA_Byte *>(value.constData()),
                                          value.length(), reinterpret_cast<void **>(&ptr->data), &UA_TYPES[UA_TYPES_BYTE]);
    if (success != UA_STATUSCODE_GOOD) {
        ptr->length = 0;
        ptr->data = nullptr;
    }
}

template<>
void scalarFromQt<UA_NodeId, QString>(const QString &value, UA_NodeId *ptr)
{
    *ptr = Open62541Utils::nodeIdFromQString(value);
}

template<>
void scalarFromQt<UA_QualifiedName, QOpcUa::QQualifiedName>(const QOpcUa::QQualifiedName &value, UA_QualifiedName *ptr)
{
    ptr->namespaceIndex = value.namespaceIndex();
    scalarFromQt<UA_String, QString>(value.name(), &(ptr->name));
}

template<>
void scalarFromQt<UA_Guid, QUuid>(const QUuid &value, UA_Guid *ptr)
{
    ptr->data1 = value.data1;
    ptr->data2 = value.data2;
    ptr->data3 = value.data3;
    std::memcpy(ptr->data4, value.data4, sizeof(value.data4));
}

template<>
void scalarFromQt<UA_ExtensionObject, QOpcUa::QRange>(const QOpcUa::QRange &value, UA_ExtensionObject *ptr)
{
    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    encoder.encode<QOpcUa::QRange>(value);
    return createExtensionObject(temp,
                                 UA_NODEID_NUMERIC(0,static_cast<UA_UInt32>(Namespace0::Range_Encoding_DefaultBinary)), ptr);
}

template<>
void scalarFromQt<UA_ExtensionObject, QOpcUa::QEUInformation>(const QOpcUa::QEUInformation &value, UA_ExtensionObject *ptr)
{
    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    encoder.encode<QOpcUa::QEUInformation>(value);
    return createExtensionObject(temp,
                                 UA_NODEID_NUMERIC(0, static_cast<UA_UInt32>(Namespace0::EUInformation_Encoding_DefaultBinary)), ptr);
}

template<>
void scalarFromQt<UA_ExtensionObject, QOpcUa::QComplexNumber>(const QOpcUa::QComplexNumber &value, UA_ExtensionObject *ptr)
{
    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    encoder.encode<QOpcUa::QComplexNumber>(value);
    return createExtensionObject(temp,
                                 UA_NODEID_NUMERIC(0, static_cast<UA_UInt32>(Namespace0::ComplexNumberType_Encoding_DefaultBinary)), ptr);
}

template<>
void scalarFromQt<UA_ExtensionObject, QOpcUa::QDoubleComplexNumber>(const QOpcUa::QDoubleComplexNumber &value, UA_ExtensionObject *ptr)
{
    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    encoder.encode<QOpcUa::QDoubleComplexNumber>(value);
    return createExtensionObject(temp,
                                 UA_NODEID_NUMERIC(0, static_cast<UA_UInt32>(Namespace0::DoubleComplexNumberType_Encoding_DefaultBinary)), ptr);
}

template<>
void scalarFromQt<UA_ExtensionObject, QOpcUa::QAxisInformation>(const QOpcUa::QAxisInformation &value, UA_ExtensionObject *ptr)
{
    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    encoder.encode<QOpcUa::QAxisInformation>(value);
    return createExtensionObject(temp,
                                 UA_NODEID_NUMERIC(0, static_cast<UA_UInt32>(Namespace0::AxisInformation_Encoding_DefaultBinary)), ptr);
}

template<>
void scalarFromQt<UA_ExtensionObject, QOpcUa::QXValue>(const QOpcUa::QXValue &value, UA_ExtensionObject *ptr)
{
    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    encoder.encode<QOpcUa::QXValue>(value);
    return createExtensionObject(temp,
                                 UA_NODEID_NUMERIC(0, static_cast<UA_UInt32>(Namespace0::XVType_Encoding_DefaultBinary)), ptr);
}

template<>
void scalarFromQt<UA_ExtensionObject, QOpcUa::QExtensionObject>(const QOpcUa::QExtensionObject &obj, UA_ExtensionObject *ptr)
{
    QByteArray temp = obj.encodedBody();
    UA_NodeId encodingId = Open62541Utils::nodeIdFromQString(obj.encodingTypeId());
    UaDeleter<UA_NodeId> nodeIdDeleter(&encodingId, UA_NodeId_deleteMembers);
    createExtensionObject(temp, encodingId, ptr, obj.encoding());
}

template<>
void scalarFromQt<UA_ExpandedNodeId, QOpcUa::QExpandedNodeId>(const QOpcUa::QExpandedNodeId &value, UA_ExpandedNodeId *ptr)
{
    ptr->serverIndex = value.serverIndex();
    scalarFromQt<UA_String, QString>(value.namespaceUri(), &ptr->namespaceUri);
    ptr->nodeId = Open62541Utils::nodeIdFromQString(value.nodeId());
}

template<>
void scalarFromQt<UA_Argument, QOpcUa::QArgument>(const QOpcUa::QArgument &value, UA_Argument *ptr)
{
    ptr->valueRank = value.valueRank();
    scalarFromQt<UA_LocalizedText, QOpcUa::QLocalizedText>(value.description(), &ptr->description);
    scalarFromQt<UA_String, QString>(value.name(), &ptr->name);
    ptr->dataType = Open62541Utils::nodeIdFromQString(value.dataTypeId());
    ptr->arrayDimensionsSize = value.arrayDimensions().size();
    UA_StatusCode res = UA_Array_copy(value.arrayDimensions().constData(), ptr->arrayDimensionsSize,
                  reinterpret_cast<void **>(&ptr->arrayDimensions), &UA_TYPES[UA_TYPES_UINT32]);
    if (res != UA_STATUSCODE_GOOD)
        ptr->arrayDimensionsSize = 0;
}

template<typename TARGETTYPE, typename QTTYPE>
UA_Variant arrayFromQVariant(const QVariant &var, const UA_DataType *type)
{
    UA_Variant open62541value;
    UA_Variant_init(&open62541value);

    if (type == nullptr) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Unable to convert QVariant to UA_Variant, unknown type";
        return open62541value;
    }

    if (var.type() == QVariant::List) {
        const QVariantList list = var.toList();
        if (list.isEmpty())
            return open62541value;

        for (auto it : qAsConst(list)) {
            if (!it.canConvert<QTTYPE>()) {
                qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Value type" << var.typeName() <<
                                                         "in the QVariant does not match type parameter" << type->typeName;
                return open62541value;
            }
        }

        TARGETTYPE *arr = static_cast<TARGETTYPE *>(UA_Array_new(list.size(), type));

        for (int i = 0; i < list.size(); ++i)
            scalarFromQt<TARGETTYPE, QTTYPE>(list[i].value<QTTYPE>(), &arr[i]);

        UA_Variant_setArray(&open62541value, arr, list.size(), type);
        return open62541value;
    }

    if (!var.canConvert<QTTYPE>()) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Value type" << var.typeName() <<
                                                 "in the QVariant does not match type parameter" << type->typeName;
        return open62541value;
    }

    TARGETTYPE *temp = static_cast<TARGETTYPE *>(UA_new(type));
    scalarFromQt<TARGETTYPE, QTTYPE>(var.value<QTTYPE>(), temp);
    UA_Variant_setScalar(&open62541value, temp, type);
    return open62541value;
}

void createExtensionObject(QByteArray &data, const UA_NodeId &typeEncodingId, UA_ExtensionObject *ptr, QOpcUa::QExtensionObject::Encoding encoding)
{
    UA_ExtensionObject obj;
    UA_ExtensionObject_init(&obj);

    if (!data.isEmpty() && encoding == QOpcUa::QExtensionObject::Encoding::NoBody)
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Data for extension object provided but will not be encoded because encoding format is set to skip the body";

    if (encoding != QOpcUa::QExtensionObject::Encoding::NoBody) {
        obj.encoding = static_cast<UA_ExtensionObjectEncoding>(encoding);
        obj.content.encoded.body.data = reinterpret_cast<UA_Byte *>(data.data());
        obj.content.encoded.body.length = data.length();
    }
    obj.content.encoded.typeId = typeEncodingId;
    UA_ExtensionObject_copy(&obj, ptr);
}

}

QT_END_NAMESPACE

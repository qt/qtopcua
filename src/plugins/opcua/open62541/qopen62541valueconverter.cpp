// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopen62541.h"
#include "qopen62541utils.h"
#include "qopen62541valueconverter.h"

#include "qopcuadatavalue.h"
#include "qopcuamultidimensionalarray.h"

#include <QtCore/qdatetime.h>
#include <QtCore/qloggingcategory.h>
#include <QtCore/qtimezone.h>
#include <QtCore/quuid.h>

#include <cstring>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_OPEN62541)

using namespace QOpcUa::NodeIds;

namespace QOpen62541ValueConverter {

UA_Variant toOpen62541Variant(const QVariant &value, QOpcUa::Types type)
{
    UA_Variant open62541value;
    UA_Variant_init(&open62541value);

    if (value.canConvert<QOpcUaMultiDimensionalArray>()) {
        QOpcUaMultiDimensionalArray data = value.value<QOpcUaMultiDimensionalArray>();
        UA_Variant result = toOpen62541Variant(data.valueArray(), type);

        const auto &arrayDimensions = data.arrayDimensions();

        if (!arrayDimensions.isEmpty()) {
            // Ensure that the array dimensions size is < UINT32_MAX
            if (static_cast<quint64>(arrayDimensions.size()) > (std::numeric_limits<quint32>::max)())
                return open62541value;
            result.arrayDimensionsSize = arrayDimensions.size();
            result.arrayDimensions = static_cast<UA_UInt32 *>(UA_Array_new(result.arrayDimensionsSize, &UA_TYPES[UA_TYPES_UINT32]));
            std::copy(arrayDimensions.constBegin(), arrayDimensions.constEnd(), result.arrayDimensions);
        }
        return result;
    }

    if (value.metaType().id() == QMetaType::QVariantList && value.toList().size() == 0)
        return open62541value;

    QVariant temp = (value.metaType().id() == QMetaType::QVariantList) ? value.toList().at(0) : value;
    QOpcUa::Types valueType = type == QOpcUa::Undefined ?
                QOpcUa::metaTypeToQOpcUaType(static_cast<QMetaType::Type>(temp.metaType().id())) : type;

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
        return arrayFromQVariant<UA_LocalizedText, QOpcUaLocalizedText>(value, dt);
    case QOpcUa::ByteString:
        return arrayFromQVariant<UA_ByteString, QByteArray>(value, dt);
    case QOpcUa::NodeId:
        return arrayFromQVariant<UA_NodeId, QString>(value, dt);
    case QOpcUa::Guid:
        return arrayFromQVariant<UA_Guid, QUuid>(value, dt);
    case QOpcUa::XmlElement:
        return arrayFromQVariant<UA_XmlElement, QString>(value, dt);
    case QOpcUa::QualifiedName:
        return arrayFromQVariant<UA_QualifiedName, QOpcUaQualifiedName>(value, dt);
    case QOpcUa::StatusCode:
        return arrayFromQVariant<UA_StatusCode, QOpcUa::UaStatusCode>(value, dt);
    case QOpcUa::Range:
        return arrayFromQVariant<UA_Range, QOpcUaRange>(value, dt);
    case QOpcUa::EUInformation:
        return arrayFromQVariant<UA_EUInformation, QOpcUaEUInformation>(value, dt);
    case QOpcUa::ComplexNumber:
        return arrayFromQVariant<UA_ComplexNumberType, QOpcUaComplexNumber>(value, dt);
    case QOpcUa::DoubleComplexNumber:
        return arrayFromQVariant<UA_DoubleComplexNumberType, QOpcUaDoubleComplexNumber>(value, dt);
    case QOpcUa::AxisInformation:
        return arrayFromQVariant<UA_AxisInformation, QOpcUaAxisInformation>(value, dt);
    case QOpcUa::XV:
        return arrayFromQVariant<UA_XVType, QOpcUaXValue>(value, dt);
    case QOpcUa::ExpandedNodeId:
        return arrayFromQVariant<UA_ExpandedNodeId, QOpcUaExpandedNodeId>(value, dt);
    case QOpcUa::Argument:
        return arrayFromQVariant<UA_Argument, QOpcUaArgument>(value, dt);
    case QOpcUa::ExtensionObject:
        return arrayFromQVariant<UA_ExtensionObject, QOpcUaExtensionObject>(value, dt);
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

    if (value.type == &UA_TYPES[UA_TYPES_BOOLEAN])
        return arrayToQVariant<bool, UA_Boolean>(value, QMetaType::Bool);
    else if (value.type == &UA_TYPES[UA_TYPES_SBYTE])
        return arrayToQVariant<signed char, UA_SByte>(value, QMetaType::SChar);
    else if (value.type == &UA_TYPES[UA_TYPES_BYTE])
        return arrayToQVariant<uchar, UA_Byte>(value, QMetaType::UChar);
    else if (value.type == &UA_TYPES[UA_TYPES_INT16])
        return arrayToQVariant<qint16, UA_Int16>(value, QMetaType::Short);
    else if (value.type == &UA_TYPES[UA_TYPES_UINT16])
        return arrayToQVariant<quint16, UA_UInt16>(value, QMetaType::UShort);
    else if (value.type == &UA_TYPES[UA_TYPES_INT32])
        return arrayToQVariant<qint32, UA_Int32>(value, QMetaType::Int);
    else if (value.type == &UA_TYPES[UA_TYPES_UINT32])
        return arrayToQVariant<quint32, UA_UInt32>(value, QMetaType::UInt);
    else if (value.type == &UA_TYPES[UA_TYPES_INT64])
        return arrayToQVariant<int64_t, UA_Int64>(value, QMetaType::LongLong);
    else if (value.type == &UA_TYPES[UA_TYPES_UINT64])
        return arrayToQVariant<uint64_t, UA_UInt64>(value, QMetaType::ULongLong);
    else if (value.type == &UA_TYPES[UA_TYPES_FLOAT])
        return arrayToQVariant<float, UA_Float>(value, QMetaType::Float);
    else if (value.type == &UA_TYPES[UA_TYPES_DOUBLE])
        return arrayToQVariant<double, UA_Double>(value, QMetaType::Double);
    else if (value.type == &UA_TYPES[UA_TYPES_STRING])
        return arrayToQVariant<QString, UA_String>(value, QMetaType::QString);
    else if (value.type == &UA_TYPES[UA_TYPES_BYTESTRING])
        return arrayToQVariant<QByteArray, UA_ByteString>(value, QMetaType::QByteArray);
    else if (value.type == &UA_TYPES[UA_TYPES_LOCALIZEDTEXT])
        return arrayToQVariant<QOpcUaLocalizedText, UA_LocalizedText>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_NODEID])
        return arrayToQVariant<QString, UA_NodeId>(value, QMetaType::QString);
    else if (value.type == &UA_TYPES[UA_TYPES_DATETIME])
        return arrayToQVariant<QDateTime, UA_DateTime>(value, QMetaType::QDateTime);
    else if (value.type == &UA_TYPES[UA_TYPES_GUID])
        return arrayToQVariant<QUuid, UA_Guid>(value, QMetaType::QUuid);
    else if (value.type == &UA_TYPES[UA_TYPES_XMLELEMENT])
        return arrayToQVariant<QString, UA_XmlElement>(value, QMetaType::QString);
    else if (value.type == &UA_TYPES[UA_TYPES_QUALIFIEDNAME])
        return arrayToQVariant<QOpcUaQualifiedName, UA_QualifiedName>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_STATUSCODE])
        return arrayToQVariant<QOpcUa::UaStatusCode, UA_StatusCode>(value, QMetaType::UInt);
    else if (value.type == &UA_TYPES[UA_TYPES_EXTENSIONOBJECT])
        return arrayToQVariant<QVariant, UA_ExtensionObject>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_EXPANDEDNODEID])
        return arrayToQVariant<QOpcUaExpandedNodeId, UA_ExpandedNodeId>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_ARGUMENT])
        return arrayToQVariant<QOpcUaArgument, UA_Argument>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_RANGE])
        return arrayToQVariant<QOpcUaRange, UA_Range>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_EUINFORMATION])
        return arrayToQVariant<QOpcUaEUInformation, UA_EUInformation>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_AXISINFORMATION])
        return arrayToQVariant<QOpcUaAxisInformation, UA_AxisInformation>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_COMPLEXNUMBERTYPE])
        return arrayToQVariant<QOpcUaComplexNumber, UA_ComplexNumberType>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_DOUBLECOMPLEXNUMBERTYPE])
        return arrayToQVariant<QOpcUaDoubleComplexNumber, UA_DoubleComplexNumberType>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_XVTYPE])
        return arrayToQVariant<QOpcUaXValue, UA_XVType>(value);

    qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Variant conversion from Open62541 for typeName" << value.type->typeName << " not implemented";
    return QVariant();
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
        return &UA_TYPES[UA_TYPES_RANGE];
    case QOpcUa::EUInformation:
        return &UA_TYPES[UA_TYPES_EUINFORMATION];
    case QOpcUa::ComplexNumber:
        return &UA_TYPES[UA_TYPES_COMPLEXNUMBERTYPE];
    case QOpcUa::DoubleComplexNumber:
        return &UA_TYPES[UA_TYPES_DOUBLECOMPLEXNUMBERTYPE];
    case QOpcUa::AxisInformation:
        return &UA_TYPES[UA_TYPES_AXISINFORMATION];
    case QOpcUa::XV:
        return &UA_TYPES[UA_TYPES_XVTYPE];
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
QOpcUaLocalizedText scalarToQt<QOpcUaLocalizedText, UA_LocalizedText>(const UA_LocalizedText *data)
{
    QOpcUaLocalizedText lt;
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
    if (*data == (std::numeric_limits<qint64>::min)() || *data == (std::numeric_limits<qint64>::max)())
        return QDateTime();

    const QDateTime epochStart(QDate(1601, 1, 1), QTime(0, 0), QTimeZone::UTC);
    return epochStart.addMSecs(*data / UA_DATETIME_MSEC).toLocalTime();
}

template<>
QOpcUaDataValue scalarToQt<QOpcUaDataValue, UA_DataValue>(const UA_DataValue *data)
{
    QOpcUaDataValue result;
    if (data->hasSourceTimestamp)
        result.setSourceTimestamp(QOpen62541ValueConverter::scalarToQt<QDateTime, UA_DateTime>(&data->sourceTimestamp));
    if (data->hasServerTimestamp)
        result.setServerTimestamp(QOpen62541ValueConverter::scalarToQt<QDateTime, UA_DateTime>(&data->serverTimestamp));
    if (data->hasValue)
        result.setValue(QOpen62541ValueConverter::toQVariant(data->value));
    if (data->hasStatus) {
        result.setStatusCode(QOpen62541ValueConverter::scalarToQt<QOpcUa::UaStatusCode, UA_StatusCode>(&data->status));
    } else {
        result.setStatusCode(QOpcUa::UaStatusCode::Good);
    }
    return result;
}

template<>
QUuid scalarToQt<QUuid, UA_Guid>(const UA_Guid *data)
{
    return QUuid(data->data1, data->data2, data->data3, data->data4[0], data->data4[1], data->data4[2],
            data->data4[3], data->data4[4], data->data4[5], data->data4[6], data->data4[7]);
}

template<>
QOpcUaQualifiedName scalarToQt<QOpcUaQualifiedName, UA_QualifiedName>(const UA_QualifiedName *data)
{
    QOpcUaQualifiedName temp;
    temp.setNamespaceIndex(data->namespaceIndex);
    temp.setName(scalarToQt<QString, UA_String>(&(data->name)));
    return temp;
}

template<>
QOpcUaArgument scalarToQt<QOpcUaArgument, UA_Argument>(const UA_Argument *data)
{
    QOpcUaArgument temp;
    temp.setValueRank(data->valueRank);
    temp.setDataTypeId(Open62541Utils::nodeIdToQString(data->dataType));
    temp.setName(scalarToQt<QString, UA_String>(&data->name));
    temp.setDescription(scalarToQt<QOpcUaLocalizedText, UA_LocalizedText>(&data->description));
    for (size_t i = 0; i < data->arrayDimensionsSize; ++i)
        temp.arrayDimensionsRef().append(data->arrayDimensions[i]);
    return temp;
}

template<>
QOpcUaRange scalarToQt<QOpcUaRange, UA_Range>(const UA_Range *data)
{
    return QOpcUaRange(data->low, data->high);
}

template<>
QOpcUaEUInformation scalarToQt<QOpcUaEUInformation, UA_EUInformation>(const UA_EUInformation *data)
{
    return QOpcUaEUInformation(scalarToQt<QString, UA_String>(&data->namespaceUri),
                             data->unitId,
                             scalarToQt<QOpcUaLocalizedText, UA_LocalizedText>(&data->displayName),
                             scalarToQt<QOpcUaLocalizedText, UA_LocalizedText>(&data->description));
}

template<>
QOpcUaComplexNumber scalarToQt<QOpcUaComplexNumber, UA_ComplexNumberType>(const UA_ComplexNumberType *data)
{
    return QOpcUaComplexNumber(data->real, data->imaginary);
}

template<>
QOpcUaDoubleComplexNumber scalarToQt<QOpcUaDoubleComplexNumber, UA_DoubleComplexNumberType>(
        const UA_DoubleComplexNumberType *data)
{
    return QOpcUaDoubleComplexNumber(data->real, data->imaginary);
}

template<>
QOpcUaAxisInformation scalarToQt<QOpcUaAxisInformation, UA_AxisInformation>(const UA_AxisInformation *data)
{
    QList<double> axisSteps;

    if (data->axisStepsSize) {
        axisSteps.reserve(data->axisStepsSize);
        std::copy(data->axisSteps, data->axisSteps + data->axisStepsSize, std::back_inserter(axisSteps));
    }

    return QOpcUaAxisInformation(scalarToQt<QOpcUaEUInformation, UA_EUInformation>(&data->engineeringUnits),
                                 scalarToQt<QOpcUaRange, UA_Range>(&data->eURange),
                                 scalarToQt<QOpcUaLocalizedText, UA_LocalizedText>(&data->title),
                                 static_cast<QOpcUa::AxisScale>(data->axisScaleType),
                                 axisSteps);
}

template<>
QOpcUaXValue scalarToQt<QOpcUaXValue, UA_XVType>(const UA_XVType *data)
{
    return QOpcUaXValue(data->x, data->value);
}

template <>
QVariant scalarToQt<QVariant, UA_ExtensionObject>(const UA_ExtensionObject *data)
{
    // OPC-UA part 6, Table 13 states that an extension object can have no body, a ByteString encoded body
    // or an XML encoded body.

    // Handle extension object without body
    if (data->encoding == UA_EXTENSIONOBJECT_ENCODED_NOBODY) {
        QOpcUaExtensionObject obj;
        obj.setEncoding(QOpcUaExtensionObject::Encoding::NoBody);
        return QVariant::fromValue(obj);
    }

    // Some types are automatically decoded by open62541. In this case, the encoding is UA_EXTENSIONOBJECT_DECODED
    if (data->encoding != UA_EXTENSIONOBJECT_ENCODED_XML && data->encoding != UA_EXTENSIONOBJECT_ENCODED_BYTESTRING) {

        if (data->content.decoded.type == &UA_TYPES[UA_TYPES_ARGUMENT] && data->content.decoded.data != nullptr) {
            return scalarToQt<QOpcUaArgument, UA_Argument>(reinterpret_cast<UA_Argument *>(data->content.decoded.data));
        }

        if (data->content.decoded.type == &UA_TYPES[UA_TYPES_RANGE] && data->content.decoded.data != nullptr) {
            return scalarToQt<QOpcUaRange, UA_Range>(reinterpret_cast<UA_Range *>(data->content.decoded.data));
        }

        if (data->content.decoded.type == &UA_TYPES[UA_TYPES_EUINFORMATION] && data->content.decoded.data) {
            return scalarToQt<QOpcUaEUInformation, UA_EUInformation>
                    (reinterpret_cast<UA_EUInformation *>(data->content.decoded.data));
        }

        if (data->content.decoded.type == &UA_TYPES[UA_TYPES_COMPLEXNUMBERTYPE] && data->content.decoded.data) {
            return scalarToQt<QOpcUaComplexNumber, UA_ComplexNumberType>
                    (reinterpret_cast<UA_ComplexNumberType *>(data->content.decoded.data));
        }

        if (data->content.decoded.type == &UA_TYPES[UA_TYPES_DOUBLECOMPLEXNUMBERTYPE] && data->content.decoded.data) {
            return scalarToQt<QOpcUaDoubleComplexNumber, UA_DoubleComplexNumberType>
                    (reinterpret_cast<UA_DoubleComplexNumberType *>(data->content.decoded.data));
        }

        if (data->content.decoded.type == &UA_TYPES[UA_TYPES_AXISINFORMATION] && data->content.decoded.data) {
            return scalarToQt<QOpcUaAxisInformation, UA_AxisInformation>
                    (reinterpret_cast<UA_AxisInformation *>(data->content.decoded.data));
        }

        if (data->content.decoded.type == &UA_TYPES[UA_TYPES_XVTYPE] && data->content.decoded.data) {
            return scalarToQt<QOpcUaXValue, UA_XVType>
                    (reinterpret_cast<UA_XVType *>(data->content.decoded.data));
        }

        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Unsupported decoded extension object type, unable to convert" << data->content.decoded.type->typeName;
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
            result = decoder.decode<QOpcUaEUInformation>(success);
            break;
        case Namespace0::ComplexNumberType_Encoding_DefaultBinary:
            result = decoder.decode<QOpcUaComplexNumber>(success);
            break;
        case Namespace0::DoubleComplexNumberType_Encoding_DefaultBinary:
            result = decoder.decode<QOpcUaDoubleComplexNumber>(success);
            break;
        case Namespace0::AxisInformation_Encoding_DefaultBinary:
            result = decoder.decode<QOpcUaAxisInformation>(success);
            break;
        case Namespace0::XVType_Encoding_DefaultBinary:
            result = decoder.decode<QOpcUaXValue>(success);
            break;
        default:
            break;
        }
        if (success)
            return result;
    }

    // Return extension objects with binary or XML body as QOpcUaExtensionObject
    QOpcUaExtensionObject obj;
    obj.setEncoding(static_cast<QOpcUaExtensionObject::Encoding>(data->encoding));
    obj.setEncodingTypeId(Open62541Utils::nodeIdToQString(data->content.encoded.typeId));
    obj.setEncodedBody(QByteArray(buffer.constData(), buffer.size()));
    return obj;
}

template<>
QOpcUaExpandedNodeId scalarToQt<QOpcUaExpandedNodeId, UA_ExpandedNodeId>(const UA_ExpandedNodeId *data)
{
    QOpcUaExpandedNodeId temp;
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
            if (type != QMetaType::UnknownType && type != static_cast<QMetaType::Type>(tempVar.metaType().id()))
                tempVar.convert(QMetaType(type));
            list.append(tempVar);
        }

        if (var.arrayDimensionsSize > 0) {
            // Ensure that the array dimensions fit in a QList
            if (var.arrayDimensionsSize > static_cast<quint64>((std::numeric_limits<int>::max)()))
                return QOpcUaMultiDimensionalArray();
            QList<quint32> arrayDimensions;
            std::copy(var.arrayDimensions, var.arrayDimensions+var.arrayDimensionsSize, std::back_inserter(arrayDimensions));
            return QOpcUaMultiDimensionalArray(list, arrayDimensions);
        }

        if (list.size() == 1)
            return list.at(0);
        else
            return list;
    } else if (UA_Variant_isScalar(&var)) {
        QVariant tempVar = QVariant::fromValue(scalarToQt<TARGETTYPE, UATYPE>(temp));
        if (type != QMetaType::UnknownType && type != static_cast<QMetaType::Type>(tempVar.metaType().id()))
            tempVar.convert(QMetaType(type));
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
    if (!value.isValid()) {
        *ptr = (std::numeric_limits<qint64>::min)();
        return;
    }

    // OPC-UA part 3, Table C.9
    const QDateTime uaEpochStart(QDate(1601, 1, 1), QTime(0, 0), QTimeZone::UTC);

    *ptr = UA_DATETIME_MSEC * (value.toMSecsSinceEpoch() - uaEpochStart.toMSecsSinceEpoch());
}

template<>
void scalarFromQt<UA_String, QString>(const QString &value, UA_String *ptr)
{
    *ptr = UA_STRING_ALLOC(value.toUtf8().constData());
}

template<>
void scalarFromQt<UA_LocalizedText, QOpcUaLocalizedText>(const QOpcUaLocalizedText &value, UA_LocalizedText *ptr)
{
    scalarFromQt<UA_String, QString>(value.locale(), &(ptr->locale));
    scalarFromQt<UA_String, QString>(value.text(), &(ptr->text));
}

template<>
void scalarFromQt<UA_ByteString, QByteArray>(const QByteArray &value, UA_ByteString *ptr)
{
    ptr->length = value.size();
    UA_StatusCode success = UA_Array_copy(reinterpret_cast<const UA_Byte *>(value.constData()),
                                          value.size(), reinterpret_cast<void **>(&ptr->data), &UA_TYPES[UA_TYPES_BYTE]);
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
void scalarFromQt<UA_QualifiedName, QOpcUaQualifiedName>(const QOpcUaQualifiedName &value, UA_QualifiedName *ptr)
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
void scalarFromQt<UA_Range, QOpcUaRange>(const QOpcUaRange &value, UA_Range *ptr)
{
    ptr->low = value.low();
    ptr->high = value.high();
}

template<>
void scalarFromQt<UA_EUInformation, QOpcUaEUInformation>(const QOpcUaEUInformation &value, UA_EUInformation *ptr)
{
    scalarFromQt<UA_String, QString>(value.namespaceUri(), &ptr->namespaceUri);
    scalarFromQt<UA_LocalizedText, QOpcUaLocalizedText>(value.description(), &ptr->description);
    scalarFromQt<UA_LocalizedText, QOpcUaLocalizedText>(value.displayName(), &ptr->displayName);
    ptr->unitId = value.unitId();
}

template<>
void scalarFromQt<UA_ComplexNumberType, QOpcUaComplexNumber>(const QOpcUaComplexNumber &value, UA_ComplexNumberType *ptr)
{
    ptr->real = value.real();
    ptr->imaginary = value.imaginary();
}

template<>
void scalarFromQt<UA_DoubleComplexNumberType, QOpcUaDoubleComplexNumber>(const QOpcUaDoubleComplexNumber &value, UA_DoubleComplexNumberType *ptr)
{
    ptr->real = value.real();
    ptr->imaginary = value.imaginary();
}

template<>
void scalarFromQt<UA_AxisInformation, QOpcUaAxisInformation>(const QOpcUaAxisInformation &value, UA_AxisInformation *ptr)
{
    scalarFromQt<UA_LocalizedText, QOpcUaLocalizedText>(value.title(), &ptr->title);
    scalarFromQt<UA_EUInformation, QOpcUaEUInformation>(value.engineeringUnits(), &ptr->engineeringUnits);
    scalarFromQt<UA_Range, QOpcUaRange>(value.eURange(), &ptr->eURange);
    ptr->axisScaleType = static_cast<UA_AxisScaleEnumeration>(value.axisScaleType());
    ptr->axisStepsSize = value.axisSteps().size();
    if (ptr->axisStepsSize) {
        auto res = UA_Array_copy(value.axisSteps().constData(), ptr->axisStepsSize, reinterpret_cast<void **>(&ptr->axisSteps),
                      &UA_TYPES[UA_TYPES_DOUBLE]);

        if (res != UA_STATUSCODE_GOOD)
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to copy axis steps";
    } else {
        ptr->axisSteps = nullptr;
    }
}

template<>
void scalarFromQt<UA_XVType, QOpcUaXValue>(const QOpcUaXValue &value, UA_XVType *ptr)
{
    ptr->x = value.x();
    ptr->value = value.value();
}

template<>
void scalarFromQt<UA_ExtensionObject, QOpcUaExtensionObject>(const QOpcUaExtensionObject &obj, UA_ExtensionObject *ptr)
{
    QByteArray temp = obj.encodedBody();
    UA_NodeId encodingId = Open62541Utils::nodeIdFromQString(obj.encodingTypeId());
    UaDeleter<UA_NodeId> nodeIdDeleter(&encodingId, UA_NodeId_clear);
    createExtensionObject(temp, encodingId, ptr, obj.encoding());
}

template<>
void scalarFromQt<UA_ExpandedNodeId, QOpcUaExpandedNodeId>(const QOpcUaExpandedNodeId &value, UA_ExpandedNodeId *ptr)
{
    ptr->serverIndex = value.serverIndex();
    scalarFromQt<UA_String, QString>(value.namespaceUri(), &ptr->namespaceUri);
    ptr->nodeId = Open62541Utils::nodeIdFromQString(value.nodeId());
}

template<>
void scalarFromQt<UA_Argument, QOpcUaArgument>(const QOpcUaArgument &value, UA_Argument *ptr)
{
    ptr->valueRank = value.valueRank();
    scalarFromQt<UA_LocalizedText, QOpcUaLocalizedText>(value.description(), &ptr->description);
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

    if (var.metaType().id() == QMetaType::QVariantList) {
        const QVariantList list = var.toList();
        if (list.isEmpty())
            return open62541value;

        for (const auto &it : std::as_const(list)) {
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

void createExtensionObject(QByteArray &data, const UA_NodeId &typeEncodingId, UA_ExtensionObject *ptr, QOpcUaExtensionObject::Encoding encoding)
{
    UA_ExtensionObject obj;
    UA_ExtensionObject_init(&obj);

    if (!data.isEmpty() && encoding == QOpcUaExtensionObject::Encoding::NoBody)
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Data for extension object provided but will not be encoded because encoding format is set to skip the body";

    if (encoding != QOpcUaExtensionObject::Encoding::NoBody) {
        obj.encoding = static_cast<UA_ExtensionObjectEncoding>(encoding);
        obj.content.encoded.body.data = reinterpret_cast<UA_Byte *>(data.data());
        obj.content.encoded.body.length = data.size();
    }
    obj.content.encoded.typeId = typeEncodingId;
    UA_ExtensionObject_copy(&obj, ptr);
}

}

QT_END_NAMESPACE

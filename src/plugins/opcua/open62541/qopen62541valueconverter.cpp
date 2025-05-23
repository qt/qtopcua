// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifdef USE_SYSTEM_OPEN62541
#include <open62541/types.h>
#else
#include "qopen62541.h"
#endif

#include "qopen62541utils.h"
#include "qopen62541valueconverter.h"

#include "qopcuadatavalue.h"
#include "qopcuamultidimensionalarray.h"
#include "qopcuastructuredefinition.h"
#include "qopcuaenumdefinition.h"

#include <QtOpcUa/qopcuaargument.h>
#include <QtOpcUa/qopcuaattributeoperand.h>
#include <QtOpcUa/qopcuaaxisinformation.h>
#include <QtOpcUa/qopcuacomplexnumber.h>
#include <QtOpcUa/qopcuadiagnosticinfo.h>
#include <QtOpcUa/qopcuadoublecomplexnumber.h>
#include <QtOpcUa/qopcuaelementoperand.h>
#include <QtOpcUa/qopcuaenumfield.h>
#include <QtOpcUa/qopcuaeuinformation.h>
#include <QtOpcUa/qopcualiteraloperand.h>
#include <QtOpcUa/qopcualocalizedtext.h>
#include <QtOpcUa/qopcuaqualifiedname.h>
#include <QtOpcUa/qopcuarange.h>
#include <QtOpcUa/qopcuastructurefield.h>
#include <QtOpcUa/qopcuaxvalue.h>

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
    case QOpcUa::SimpleAttributeOperand:
        return arrayFromQVariant<UA_SimpleAttributeOperand, QOpcUaSimpleAttributeOperand>(value, dt);
    case QOpcUa::AttributeOperand:
        return arrayFromQVariant<UA_AttributeOperand, QOpcUaAttributeOperand>(value, dt);
    case QOpcUa::LiteralOperand:
        return arrayFromQVariant<UA_LiteralOperand, QOpcUaLiteralOperand>(value, dt);
    case QOpcUa::ElementOperand:
        return arrayFromQVariant<UA_SimpleAttributeOperand, QOpcUaSimpleAttributeOperand>(value, dt);
    case QOpcUa::RelativePathElement:
        return arrayFromQVariant<UA_RelativePathElement, QOpcUaRelativePathElement>(value, dt);
    case QOpcUa::ContentFilterElement:
        return arrayFromQVariant<UA_ContentFilterElement, QOpcUaContentFilterElement>(value, dt);
    case QOpcUa::EventFilter:
        return arrayFromQVariant<UA_EventFilter, QOpcUaMonitoringParameters::EventFilter>(value, dt);
    case QOpcUa::ExtensionObject:
        return arrayFromQVariant<UA_ExtensionObject, QOpcUaExtensionObject>(value, dt);
    case QOpcUa::StructureDefinition:
        return arrayFromQVariant<UA_StructureDefinition, QOpcUaStructureDefinition>(value, dt);
    case QOpcUa::StructureField:
        return arrayFromQVariant<UA_StructureField, QOpcUaStructureField>(value, dt);
    case QOpcUa::EnumDefinition:
        return arrayFromQVariant<UA_EnumDefinition, QOpcUaEnumDefinition>(value, dt);
    case QOpcUa::EnumField:
        return arrayFromQVariant<UA_EnumField, QOpcUaEnumField>(value, dt);
    case QOpcUa::DiagnosticInfo:
        return arrayFromQVariant<UA_DiagnosticInfo, QOpcUaDiagnosticInfo>(value, dt);
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
    else if (value.type == &UA_TYPES[UA_TYPES_STRUCTUREDEFINITION])
        return arrayToQVariant<QOpcUaStructureDefinition, UA_StructureDefinition>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_STRUCTUREFIELD])
        return arrayToQVariant<QOpcUaStructureField, UA_StructureField>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_ENUMDEFINITION])
        return arrayToQVariant<QOpcUaEnumDefinition, UA_EnumDefinition>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_ENUMFIELD])
        return arrayToQVariant<QOpcUaEnumField, UA_EnumField>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_DIAGNOSTICINFO])
        return arrayToQVariant<QOpcUaDiagnosticInfo, UA_DiagnosticInfo>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_SIMPLEATTRIBUTEOPERAND])
        return arrayToQVariant<QOpcUaSimpleAttributeOperand, UA_SimpleAttributeOperand>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_ATTRIBUTEOPERAND])
        return arrayToQVariant<QOpcUaSimpleAttributeOperand, UA_SimpleAttributeOperand>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_LITERALOPERAND])
        return arrayToQVariant<QOpcUaLiteralOperand, UA_LiteralOperand>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_ELEMENTOPERAND])
        return arrayToQVariant<QOpcUaElementOperand, UA_ElementOperand>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_RELATIVEPATHELEMENT])
        return arrayToQVariant<QOpcUaRelativePathElement, UA_RelativePathElement>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_CONTENTFILTERELEMENT])
        return arrayToQVariant<QOpcUaContentFilterElement, UA_ContentFilterElement>(value);
    else if (value.type == &UA_TYPES[UA_TYPES_EVENTFILTER])
        return arrayToQVariant<QOpcUaMonitoringParameters::EventFilter, UA_EventFilter>(value);

    return uaVariantToQtExtensionObject(value);
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
    case QOpcUa::StructureDefinition:
        return &UA_TYPES[UA_TYPES_STRUCTUREDEFINITION];
    case QOpcUa::StructureField:
        return &UA_TYPES[UA_TYPES_STRUCTUREFIELD];
    case QOpcUa::EnumDefinition:
        return &UA_TYPES[UA_TYPES_ENUMDEFINITION];
    case QOpcUa::EnumField:
        return &UA_TYPES[UA_TYPES_ENUMFIELD];
    case QOpcUa::DiagnosticInfo:
        return &UA_TYPES[UA_TYPES_DIAGNOSTICINFO];
    case QOpcUa::SimpleAttributeOperand:
        return &UA_TYPES[UA_TYPES_SIMPLEATTRIBUTEOPERAND];
    case QOpcUa::AttributeOperand:
        return &UA_TYPES[UA_TYPES_ATTRIBUTEOPERAND];
    case QOpcUa::LiteralOperand:
        return &UA_TYPES[UA_TYPES_LITERALOPERAND];
    case QOpcUa::ElementOperand:
        return &UA_TYPES[UA_TYPES_ELEMENTOPERAND];
    case QOpcUa::RelativePathElement:
        return &UA_TYPES[UA_TYPES_RELATIVEPATHELEMENT];
    case QOpcUa::ContentFilterElement:
        return &UA_TYPES[UA_TYPES_CONTENTFILTERELEMENT];
    case QOpcUa::EventFilter:
        return &UA_TYPES[UA_TYPES_EVENTFILTER];
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
    // OPC UA 1.05 part 6, 5.1.4
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
    if (data->hasServerPicoseconds)
        result.setServerPicoseconds(data->serverPicoseconds);
    if (data->hasSourcePicoseconds)
        result.setSourcePicoseconds(data->sourcePicoseconds);

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

template<>
QOpcUaStructureField scalarToQt<QOpcUaStructureField, UA_StructureField>(const UA_StructureField *data)
{
    QOpcUaStructureField temp;
    temp.setName(scalarToQt<QString, UA_String>(&data->name));
    temp.setDescription(scalarToQt<QOpcUaLocalizedText, UA_LocalizedText>(&data->description));
    temp.setDataType(scalarToQt<QString, UA_NodeId>(&data->dataType));
    temp.setIsOptional(data->isOptional);
    temp.setMaxStringLength(scalarToQt<quint32, UA_UInt32>(&data->maxStringLength));
    temp.setValueRank(scalarToQt<qint32, UA_Int32>(&data->valueRank));

    QList<quint32> dimensions;
    for (size_t i = 0; i < data->arrayDimensionsSize; ++i)
        dimensions.append(data->arrayDimensions[i]);
    temp.setArrayDimensions(dimensions);
    return temp;
}

template<>
QOpcUaStructureDefinition scalarToQt<QOpcUaStructureDefinition, UA_StructureDefinition>(const UA_StructureDefinition *data)
{
    QOpcUaStructureDefinition temp;
    temp.setBaseDataType(scalarToQt<QString, UA_NodeId>(&data->baseDataType));
    temp.setDefaultEncodingId(scalarToQt<QString, UA_NodeId>(&data->defaultEncodingId));
    temp.setStructureType(static_cast<QOpcUaStructureDefinition::StructureType>(data->structureType));

    QList<QOpcUaStructureField> fields;
    for (size_t i = 0; i < data->fieldsSize; ++i)
        fields.append(scalarToQt<QOpcUaStructureField, UA_StructureField>(&data->fields[i]));
    temp.setFields(fields);
    return temp;
}

template<>
QOpcUaEnumField scalarToQt<QOpcUaEnumField, UA_EnumField>(const UA_EnumField *data)
{
    QOpcUaEnumField temp;
    temp.setName(scalarToQt<QString, UA_String>(&data->name));
    temp.setDisplayName(scalarToQt<QOpcUaLocalizedText, UA_LocalizedText>(&data->displayName));
    temp.setValue(data->value);
    temp.setDescription(scalarToQt<QOpcUaLocalizedText, UA_LocalizedText>(&data->description));

    return temp;
}

template<>
QOpcUaEnumDefinition scalarToQt<QOpcUaEnumDefinition, UA_EnumDefinition>(const UA_EnumDefinition *data)
{
    QOpcUaEnumDefinition temp;

    QList<QOpcUaEnumField> fields;
    for (size_t i = 0; i < data->fieldsSize; ++i)
        fields.push_back(scalarToQt<QOpcUaEnumField, UA_EnumField>(&data->fields[i]));
    temp.setFields(fields);

    return temp;
}

template<>
QOpcUaDiagnosticInfo scalarToQt<QOpcUaDiagnosticInfo, UA_DiagnosticInfo>(const UA_DiagnosticInfo *data)
{
    QOpcUaDiagnosticInfo temp;

    if (data->hasSymbolicId) {
        temp.setHasSymbolicId(true);
        temp.setSymbolicId(data->symbolicId);
    }

    if (data->hasNamespaceUri) {
        temp.setHasNamespaceUri(true);
        temp.setNamespaceUri(data->namespaceUri);
    }

    if (data->hasLocale) {
        temp.setHasLocale(true);
        temp.setLocale(data->locale);
    }

    if (data->hasLocalizedText) {
        temp.setHasLocalizedText(true);
        temp.setLocalizedText(data->localizedText);
    }

    if (data->hasAdditionalInfo) {
        temp.setHasAdditionalInfo(true);
        temp.setAdditionalInfo(scalarToQt<QString, UA_String>(&data->additionalInfo));
    }

    if (data->hasInnerStatusCode) {
        temp.setHasInnerStatusCode(true);
        temp.setInnerStatusCode(scalarToQt<QOpcUa::UaStatusCode, UA_StatusCode>(&data->innerStatusCode));
    }

    if (data->hasInnerDiagnosticInfo) {
        temp.setHasInnerDiagnosticInfo(true);
        if (data->innerDiagnosticInfo)
            temp.setInnerDiagnosticInfo(scalarToQt<QOpcUaDiagnosticInfo, UA_DiagnosticInfo>(data->innerDiagnosticInfo));
    }

    return temp;
}

template<>
QOpcUaSimpleAttributeOperand scalarToQt<QOpcUaSimpleAttributeOperand, UA_SimpleAttributeOperand>(const UA_SimpleAttributeOperand *data)
{
    QOpcUaSimpleAttributeOperand result;

    result.setAttributeId(toQtAttributeId(static_cast<UA_AttributeId>(data->attributeId)));
    result.setIndexRange(scalarToQt<QString, UA_String>(&data->indexRange));
    result.setTypeId(scalarToQt<QString, UA_NodeId>(&data->typeDefinitionId));

    QList<QOpcUaQualifiedName> browsePath;
    for (size_t i = 0; i < data->browsePathSize; ++i)
        browsePath.push_back(scalarToQt<QOpcUaQualifiedName, UA_QualifiedName>(&data->browsePath[i]));

    result.setBrowsePath(browsePath);

    return result;
}

template<>
QOpcUaLiteralOperand scalarToQt<QOpcUaLiteralOperand, UA_LiteralOperand>(const UA_LiteralOperand *data)
{
    QOpcUaLiteralOperand result;
    result.setValue(toQVariant(data->value));
    result.setType(toQtDataType(data->value.type));

    return result;
}

template<>
QOpcUaElementOperand scalarToQt<QOpcUaElementOperand, UA_ElementOperand>(const UA_ElementOperand *data)
{
    QOpcUaElementOperand result;
    result.setIndex(data->index);

    return result;
}

template<>
QOpcUaRelativePathElement scalarToQt<QOpcUaRelativePathElement, UA_RelativePathElement>(const UA_RelativePathElement *data)
{
    QOpcUaRelativePathElement result;
    result.setIncludeSubtypes(data->includeSubtypes);
    result.setIsInverse(data->isInverse);
    result.setReferenceTypeId(scalarToQt<QString, UA_NodeId>(&data->referenceTypeId));
    result.setTargetName(scalarToQt<QOpcUaQualifiedName, UA_QualifiedName>(&data->targetName));

    return result;
}

template<>
QOpcUaAttributeOperand scalarToQt<QOpcUaAttributeOperand, UA_AttributeOperand>(const UA_AttributeOperand *data)
{
    QOpcUaAttributeOperand result;
    result.setAttributeId(toQtAttributeId(static_cast<UA_AttributeId>(data->attributeId)));
    result.setNodeId(scalarToQt<QString, UA_NodeId>(&data->nodeId));
    result.setAlias(scalarToQt<QString, UA_String>(&data->alias));
    result.setIndexRange(scalarToQt<QString, UA_String>(&data->indexRange));

    QList<QOpcUaRelativePathElement> browsePath;

    for (size_t i = 0; i < data->browsePath.elementsSize; ++i)
        browsePath.push_back(scalarToQt<QOpcUaRelativePathElement, UA_RelativePathElement>(&data->browsePath.elements[i]));

    result.setBrowsePath(browsePath);

    return result;
}

template<>
QOpcUaContentFilterElement scalarToQt<QOpcUaContentFilterElement, UA_ContentFilterElement>(const UA_ContentFilterElement *data)
{
    QOpcUaContentFilterElement result;

    result.setFilterOperator(QOpcUaContentFilterElement::FilterOperator(data->filterOperator));
    QVariantList filterOperands;

    for (size_t i = 0; i < data->filterOperandsSize; ++i) {
        if (data->filterOperands[i].encoding < UA_EXTENSIONOBJECT_DECODED) {
            filterOperands.push_back(scalarToQt<QOpcUaExtensionObject, UA_ExtensionObject>(&data->filterOperands[i]));
        } else if (data->filterOperands[i].content.decoded.type == &UA_TYPES[UA_TYPES_LITERALOPERAND]) {
            filterOperands.push_back(scalarToQt<QOpcUaLiteralOperand, UA_LiteralOperand>(
                static_cast<UA_LiteralOperand *>(data->filterOperands[i].content.decoded.data)));
        } else if (data->filterOperands[i].content.decoded.type == &UA_TYPES[UA_TYPES_ELEMENTOPERAND]) {
            filterOperands.push_back(scalarToQt<QOpcUaElementOperand, UA_ElementOperand>(
                static_cast<UA_ElementOperand *>(data->filterOperands[i].content.decoded.data)));
        } else if (data->filterOperands[i].content.decoded.type == &UA_TYPES[UA_TYPES_ATTRIBUTEOPERAND]) {
            filterOperands.push_back(scalarToQt<QOpcUaAttributeOperand, UA_AttributeOperand>(
                static_cast<UA_AttributeOperand *>(data->filterOperands[i].content.decoded.data)));
        } else if (data->filterOperands[i].content.decoded.type == &UA_TYPES[UA_TYPES_SIMPLEATTRIBUTEOPERAND]) {
            filterOperands.push_back(scalarToQt<QOpcUaSimpleAttributeOperand, UA_SimpleAttributeOperand>(
                static_cast<UA_SimpleAttributeOperand *>(data->filterOperands[i].content.decoded.data)));
        } else {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Unknown operand in content filter element, unable to convert";
            return {};
        }
    }

    result.setFilterOperands(filterOperands);

    return result;
}

template<>
QOpcUaMonitoringParameters::EventFilter scalarToQt<QOpcUaMonitoringParameters::EventFilter, UA_EventFilter>(const UA_EventFilter *data)
{
    QOpcUaMonitoringParameters::EventFilter eventFilter;

    for (size_t i = 0; i < data->selectClausesSize; ++i) {
        eventFilter << scalarToQt<QOpcUaSimpleAttributeOperand, UA_SimpleAttributeOperand>(&data->selectClauses[i]);
    }

    for (size_t i = 0; i < data->whereClause.elementsSize; ++i) {
        eventFilter << scalarToQt<QOpcUaContentFilterElement, UA_ContentFilterElement>(&data->whereClause.elements[i]);
    }

    return eventFilter;
}

template <>
QVariant scalarToQt<QVariant, UA_ExtensionObject>(const UA_ExtensionObject *data)
{
    // OPC UA 1.05 part 6, 5.2.2.15 states that an extension object can have no body, a ByteString encoded body
    // or an XML encoded body.

    // Handle extension object without body
    if (data->encoding == UA_EXTENSIONOBJECT_ENCODED_NOBODY) {
        QOpcUaExtensionObject obj;
        obj.setEncoding(QOpcUaExtensionObject::Encoding::NoBody);
        return QVariant::fromValue(obj);
    }

    // Some types are automatically decoded by open62541. In this case, the encoding is UA_EXTENSIONOBJECT_DECODED
    if (data->encoding != UA_EXTENSIONOBJECT_ENCODED_XML && data->encoding != UA_EXTENSIONOBJECT_ENCODED_BYTESTRING && data->content.decoded.data) {

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

        if (data->content.decoded.type == &UA_TYPES[UA_TYPES_STRUCTUREDEFINITION]) {
            return scalarToQt<QOpcUaStructureDefinition, UA_StructureDefinition>
                    (reinterpret_cast<UA_StructureDefinition *>(data->content.decoded.data));
        }

        if (data->content.decoded.type == &UA_TYPES[UA_TYPES_STRUCTUREFIELD]) {
            return scalarToQt<QOpcUaStructureField, UA_StructureField>
                    (reinterpret_cast<UA_StructureField *>(data->content.decoded.data));
        }

        if (data->content.decoded.type == &UA_TYPES[UA_TYPES_ENUMDEFINITION]) {
            return scalarToQt<QOpcUaEnumDefinition, UA_EnumDefinition>
                    (reinterpret_cast<UA_EnumDefinition *>(data->content.decoded.data));
        }

        if (data->content.decoded.type == &UA_TYPES[UA_TYPES_ENUMFIELD]) {
            return scalarToQt<QOpcUaEnumField, UA_EnumField>
                    (reinterpret_cast<UA_EnumField *>(data->content.decoded.data));
        }

        if (data->content.decoded.type == &UA_TYPES[UA_TYPES_SIMPLEATTRIBUTEOPERAND] && data->content.decoded.data) {
            return scalarToQt<QOpcUaSimpleAttributeOperand, UA_SimpleAttributeOperand>
                (reinterpret_cast<UA_SimpleAttributeOperand *>(data->content.decoded.data));
        }

        if (data->content.decoded.type == &UA_TYPES[UA_TYPES_ATTRIBUTEOPERAND] && data->content.decoded.data) {
            return scalarToQt<QOpcUaAttributeOperand, UA_AttributeOperand>
                (reinterpret_cast<UA_AttributeOperand *>(data->content.decoded.data));
        }

        if (data->content.decoded.type == &UA_TYPES[UA_TYPES_LITERALOPERAND] && data->content.decoded.data) {
            return scalarToQt<QOpcUaLiteralOperand, UA_LiteralOperand>
                (reinterpret_cast<UA_LiteralOperand *>(data->content.decoded.data));
        }

        if (data->content.decoded.type == &UA_TYPES[UA_TYPES_ELEMENTOPERAND] && data->content.decoded.data) {
            return scalarToQt<QOpcUaElementOperand, UA_ElementOperand>
                (reinterpret_cast<UA_ElementOperand *>(data->content.decoded.data));
        }

        if (data->content.decoded.type == &UA_TYPES[UA_TYPES_RELATIVEPATHELEMENT] && data->content.decoded.data) {
            return scalarToQt<QOpcUaRelativePathElement, UA_RelativePathElement>
                (reinterpret_cast<UA_RelativePathElement *>(data->content.decoded.data));
        }

        if (data->content.decoded.type == &UA_TYPES[UA_TYPES_CONTENTFILTERELEMENT] && data->content.decoded.data) {
            return scalarToQt<QOpcUaContentFilterElement, UA_ContentFilterElement>
                (reinterpret_cast<UA_ContentFilterElement *>(data->content.decoded.data));
        }

        if (data->content.decoded.type == &UA_TYPES[UA_TYPES_EVENTFILTER] && data->content.decoded.data) {
            return scalarToQt<QOpcUaMonitoringParameters::EventFilter, UA_EventFilter>
                (reinterpret_cast<UA_EventFilter *>(data->content.decoded.data));
        }

        bool success = false;
        const auto result = encodeAsBinaryExtensionObject(data->content.decoded.data, data->content.decoded.type, &success);
        if (success)
            return result;

        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to re-encode decoded extension object, unable to convert" << data->content.decoded.type->typeName;
        return {};
    }

    QByteArray buffer = QByteArray::fromRawData(reinterpret_cast<const char *>(data->content.encoded.body.data),
                                                data->content.encoded.body.length);

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
        QVariantList list(var.arrayLength);
        for (size_t i = 0; i < var.arrayLength; ++i) {
            QVariant tempVar = QVariant::fromValue(scalarToQt<TARGETTYPE, UATYPE>(&temp[i]));
            if (type != QMetaType::UnknownType && type != static_cast<QMetaType::Type>(tempVar.metaType().id()))
                tempVar.convert(QMetaType(type));
            list[i] = tempVar;
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

    // OPC UA 1.05 part 6, 5.1.4
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
void scalarFromQt<UA_StructureField, QOpcUaStructureField>(const QOpcUaStructureField &value, UA_StructureField *ptr)
{
    scalarFromQt<UA_NodeId, QString>(value.dataType(), &ptr->dataType);
    ptr->maxStringLength = value.maxStringLength();
    ptr->isOptional = value.isOptional();
    ptr->valueRank = value.valueRank();
    scalarFromQt<UA_String, QString>(value.name(), &ptr->name);
    scalarFromQt<UA_LocalizedText, QOpcUaLocalizedText>(value.description(), &ptr->description);

    ptr->arrayDimensionsSize = value.arrayDimensions().size();
    if (ptr->arrayDimensionsSize) {
        ptr->arrayDimensions = static_cast<quint32 *>(UA_Array_new(ptr->arrayDimensionsSize, &UA_TYPES[UA_TYPES_UINT32]));

        for (int i = 0; i < value.arrayDimensions().size(); ++i)
            ptr->arrayDimensions[i] = value.arrayDimensions().at(i);
    }
}

template<>
void scalarFromQt<UA_StructureDefinition, QOpcUaStructureDefinition>(const QOpcUaStructureDefinition &value, UA_StructureDefinition *ptr)
{
    scalarFromQt<UA_NodeId, QString>(value.baseDataType(), &ptr->baseDataType);
    scalarFromQt<UA_NodeId, QString>(value.defaultEncodingId(), &ptr->defaultEncodingId);
    ptr->structureType = static_cast<UA_StructureType>(value.structureType());

    if (!value.fields().isEmpty()) {
        ptr->fieldsSize = value.fields().size();
        ptr->fields = static_cast<UA_StructureField *>(UA_Array_new(ptr->fieldsSize, &UA_TYPES[UA_TYPES_STRUCTUREFIELD]));

        for (int i = 0; i < value.fields().size(); ++i)
            scalarFromQt<UA_StructureField, QOpcUaStructureField>(value.fields().at(i), &ptr->fields[i]);
    }
}

template<>
void scalarFromQt<UA_EnumField, QOpcUaEnumField>(const QOpcUaEnumField &value, UA_EnumField *ptr)
{
    scalarFromQt<UA_String, QString>(value.name(), &ptr->name);
    scalarFromQt<UA_LocalizedText, QOpcUaLocalizedText>(value.description(), &ptr->description);
    scalarFromQt<UA_LocalizedText, QOpcUaLocalizedText>(value.displayName(), &ptr->displayName);
    ptr->value = value.value();
}

template<>
void scalarFromQt<UA_EnumDefinition, QOpcUaEnumDefinition>(const QOpcUaEnumDefinition &value, UA_EnumDefinition *ptr)
{
    if (!value.fields().isEmpty()) {
        ptr->fieldsSize = value.fields().size();
        ptr->fields = static_cast<UA_EnumField *>(UA_Array_new(ptr->fieldsSize, &UA_TYPES[UA_TYPES_ENUMFIELD]));

        for (int i = 0; i < value.fields().size(); ++i)
            scalarFromQt<UA_EnumField, QOpcUaEnumField>(value.fields().at(i), &ptr->fields[i]);
    }
}

template<>
void scalarFromQt<UA_DiagnosticInfo, QOpcUaDiagnosticInfo>(const QOpcUaDiagnosticInfo &value, UA_DiagnosticInfo *ptr)
{
    if (value.hasSymbolicId()) {
        ptr->hasSymbolicId = true;
        ptr->symbolicId = value.symbolicId();
    }

    if (value.hasNamespaceUri()) {
        ptr->hasNamespaceUri = true;
        ptr->namespaceUri = value.namespaceUri();
    }

    if (value.hasLocale()) {
        ptr->hasLocale = true;
        ptr->locale = value.locale();
    }

    if (value.hasLocalizedText()) {
        ptr->hasLocalizedText = true;
        ptr->localizedText = value.localizedText();
    }

    if (value.hasAdditionalInfo()) {
        ptr->hasAdditionalInfo = true;
        scalarFromQt<UA_String, QString>(value.additionalInfo(), &ptr->additionalInfo);
    }

    if (value.hasInnerStatusCode()) {
        ptr->hasInnerStatusCode = true;
        ptr->innerStatusCode = value.innerStatusCode();
    }

    if (value.hasInnerDiagnosticInfo()) {
        ptr->hasInnerDiagnosticInfo = true;
        ptr->innerDiagnosticInfo = UA_DiagnosticInfo_new();
        scalarFromQt<UA_DiagnosticInfo, QOpcUaDiagnosticInfo>(value.innerDiagnosticInfo(), ptr->innerDiagnosticInfo);
    }
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

template<>
void scalarFromQt<UA_SimpleAttributeOperand, QOpcUaSimpleAttributeOperand>(const QOpcUaSimpleAttributeOperand &value, UA_SimpleAttributeOperand *ptr)
{
    ptr->attributeId = toUaAttributeId(value.attributeId());
    if (!value.indexRange().isEmpty())
        scalarFromQt<UA_String, QString>(value.indexRange(), &ptr->indexRange);
    scalarFromQt<UA_NodeId, QString>(value.typeId(), &ptr->typeDefinitionId);
    ptr->browsePathSize = value.browsePath().size();
    if (!value.browsePath().isEmpty()) {
        ptr->browsePath = static_cast<UA_QualifiedName *>(UA_Array_new(value.browsePath().size(), &UA_TYPES[UA_TYPES_QUALIFIEDNAME]));
        for (size_t i = 0; i < ptr->browsePathSize; ++i)
            scalarFromQt<UA_QualifiedName, QOpcUaQualifiedName>(value.browsePath().at(i), &ptr->browsePath[i]);
    } else {
        ptr->browsePath = static_cast<UA_QualifiedName *>(UA_EMPTY_ARRAY_SENTINEL);
    }
}

template<>
void scalarFromQt<UA_LiteralOperand, QOpcUaLiteralOperand>(const QOpcUaLiteralOperand &value, UA_LiteralOperand *ptr)
{
    ptr->value = toOpen62541Variant(value.value(), value.type());
}

template<>
void scalarFromQt<UA_ElementOperand, QOpcUaElementOperand>(const QOpcUaElementOperand &value, UA_ElementOperand *ptr)
{
    ptr->index = value.index();
}

template<>
void scalarFromQt<UA_RelativePathElement, QOpcUaRelativePathElement>(const QOpcUaRelativePathElement &value, UA_RelativePathElement *ptr)
{
    ptr->includeSubtypes = value.includeSubtypes();
    ptr->isInverse = value.isInverse();
    scalarFromQt<UA_NodeId, QString>(value.referenceTypeId(), &ptr->referenceTypeId);
    scalarFromQt<UA_QualifiedName, QOpcUaQualifiedName>(value.targetName(), &ptr->targetName);
}

template<>
void scalarFromQt<UA_AttributeOperand, QOpcUaAttributeOperand>(const QOpcUaAttributeOperand &value, UA_AttributeOperand *ptr)
{
    ptr->attributeId = toUaAttributeId(value.attributeId());
    scalarFromQt<UA_String, QString>(value.alias(), &ptr->alias);
    if (!value.indexRange().isEmpty())
        scalarFromQt<UA_String, QString>(value.indexRange(), &ptr->indexRange);
    scalarFromQt<UA_NodeId, QString>(value.nodeId(), &ptr->nodeId);
    ptr->browsePath.elementsSize = value.browsePath().size();
    if (ptr->browsePath.elementsSize) {
        ptr->browsePath.elements = static_cast<UA_RelativePathElement *>(UA_Array_new(ptr->browsePath.elementsSize,
                                                                                      &UA_TYPES[UA_TYPES_RELATIVEPATHELEMENT]));
        for (size_t i = 0; i < ptr->browsePath.elementsSize; ++i)
            scalarFromQt<UA_RelativePathElement, QOpcUaRelativePathElement>(value.browsePath().at(i),
                                                                            &ptr->browsePath.elements[i]);
    }
}

template<>
void scalarFromQt<UA_ContentFilterElement, QOpcUaContentFilterElement>(const QOpcUaContentFilterElement &value, UA_ContentFilterElement *ptr)
{
    ptr->filterOperator = static_cast<UA_FilterOperator>(value.filterOperator());
    ptr->filterOperandsSize = value.filterOperands().size();
    if (ptr->filterOperandsSize) {
        ptr->filterOperands = static_cast<UA_ExtensionObject *>(UA_Array_new(ptr->filterOperandsSize, &UA_TYPES[UA_TYPES_EXTENSIONOBJECT]));

        for (size_t i = 0; i < ptr->filterOperandsSize; ++i) {
            if (value.filterOperands().at(i).canConvert<QOpcUaElementOperand>()) {
                const auto operand = UA_ElementOperand_new();
                scalarFromQt<UA_ElementOperand, QOpcUaElementOperand>(value.filterOperands().at(i).value<QOpcUaElementOperand>(), operand);
                ptr->filterOperands[i].encoding = UA_EXTENSIONOBJECT_DECODED;
                ptr->filterOperands[i].content.decoded.data = operand;
                ptr->filterOperands[i].content.decoded.type = &UA_TYPES[UA_TYPES_ELEMENTOPERAND];
            } else if (value.filterOperands().at(i).canConvert<QOpcUaLiteralOperand>()) {
                const auto operand = UA_LiteralOperand_new();
                scalarFromQt<UA_LiteralOperand, QOpcUaLiteralOperand>(value.filterOperands().at(i).value<QOpcUaLiteralOperand>(), operand);
                ptr->filterOperands[i].encoding = UA_EXTENSIONOBJECT_DECODED;
                ptr->filterOperands[i].content.decoded.data = operand;
                ptr->filterOperands[i].content.decoded.type = &UA_TYPES[UA_TYPES_LITERALOPERAND];
            } else if (value.filterOperands().at(i).canConvert<QOpcUaAttributeOperand>()) {
                const auto operand = UA_AttributeOperand_new();
                scalarFromQt<UA_AttributeOperand, QOpcUaAttributeOperand>(value.filterOperands().at(i).value<QOpcUaAttributeOperand>(), operand);
                ptr->filterOperands[i].encoding = UA_EXTENSIONOBJECT_DECODED;
                ptr->filterOperands[i].content.decoded.data = operand;
                ptr->filterOperands[i].content.decoded.type = &UA_TYPES[UA_TYPES_ATTRIBUTEOPERAND];
            } else if (value.filterOperands().at(i).canConvert<QOpcUaSimpleAttributeOperand>()) {
                const auto operand = UA_SimpleAttributeOperand_new();
                scalarFromQt<UA_SimpleAttributeOperand, QOpcUaSimpleAttributeOperand>(value.filterOperands().at(i).value<QOpcUaSimpleAttributeOperand>(), operand);
                ptr->filterOperands[i].encoding = UA_EXTENSIONOBJECT_DECODED;
                ptr->filterOperands[i].content.decoded.data = operand;
                ptr->filterOperands[i].content.decoded.type = &UA_TYPES[UA_TYPES_SIMPLEATTRIBUTEOPERAND];
            } else {
                qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Unsupported operand type in content filter element, unable to convert";
                UA_ContentFilterElement_clear(ptr);
                return;
            }
        }
    }
}

template<>
void scalarFromQt<UA_EventFilter, QOpcUaMonitoringParameters::EventFilter>(const QOpcUaMonitoringParameters::EventFilter &value, UA_EventFilter *ptr)
{
    ptr->selectClausesSize = value.selectClauses().size();
    if (ptr->selectClausesSize) {
        ptr->selectClauses = static_cast<UA_SimpleAttributeOperand *>(
            UA_Array_new(ptr->selectClausesSize, &UA_TYPES[UA_TYPES_SIMPLEATTRIBUTEOPERAND]));

        for (size_t i = 0; i < ptr->selectClausesSize; ++i)
            scalarFromQt<UA_SimpleAttributeOperand, QOpcUaSimpleAttributeOperand>(value.selectClauses().at(i),
                                                                                  &ptr->selectClauses[i]);
    }

    ptr->whereClause.elementsSize = value.whereClause().size();
    if (ptr->whereClause.elementsSize) {
        ptr->whereClause.elements = static_cast<UA_ContentFilterElement *>(
            UA_Array_new(ptr->whereClause.elementsSize, &UA_TYPES[UA_TYPES_CONTENTFILTERELEMENT]));

        for (size_t i = 0; i < ptr->whereClause.elementsSize; ++i)
            scalarFromQt<UA_ContentFilterElement, QOpcUaContentFilterElement>(value.whereClause().at(i),
                                                                              &ptr->whereClause.elements[i]);
    }
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

QVariant uaVariantToQtExtensionObject(const UA_Variant &var)
{
    if (UA_Variant_isScalar(&var)) {
        bool success = false;
        const auto result = encodeAsBinaryExtensionObject(var.data, var.type, &success);
        if (success)
            return result;

        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to re-encode decoded extension object, unable to convert" << var.type->typeName;
        return {};
    } else if (var.arrayLength == 0 && var.data == UA_EMPTY_ARRAY_SENTINEL) {
        return QVariantList(); // Return empty QVariantList for empty array
    } else if (var.arrayLength > 0) {
        QVariantList values;
        values.reserve(var.arrayLength);
        for (size_t i = 0; i < var.arrayLength; ++i) {
            bool success = false;
            values.push_back(encodeAsBinaryExtensionObject(static_cast<quint8 *>(var.data) + (i * var.type->memSize), var.type, &success));

            if (!success)
                return {};
        }

        if (var.arrayDimensionsSize > 0) {
            QOpcUaMultiDimensionalArray arr;
            arr.setValueArray(values);
            QList<quint32> arrayDimensions(var.arrayDimensionsSize);
            for (size_t i = 0; i < var.arrayDimensionsSize; ++i)
                arrayDimensions[i] = var.arrayDimensions[i];
            arr.setArrayDimensions(arrayDimensions);
            return arr;
        }

        return values;
    }

    return QVariant();
}

QOpcUaExtensionObject encodeAsBinaryExtensionObject(const void *data, const UA_DataType *type, bool *success)
{
    if (!data || !type) {
        if (success)
            *success = false;
        return {};
    }

    UA_ByteString encodedData;
    UA_ByteString_init(&encodedData);
    const auto encodeResult = UA_encodeBinary(data, type, &encodedData);

    if (encodeResult != UA_STATUSCODE_GOOD) {
        if (success)
            *success = false;

        return {};
    }

    QOpcUaExtensionObject result;
    result.setBinaryEncodedBody(scalarToQt<QByteArray, UA_ByteString>(&encodedData), scalarToQt<QString, UA_NodeId>(&type->typeId));
    UA_ByteString_clear(&encodedData);
    result.setEncodingTypeId(scalarToQt<QString, UA_NodeId>(&type->binaryEncodingId));

    if (success)
        *success = true;

    return result;
}

QOpcUa::Types toQtDataType(const UA_DataType *type)
{
    if (type == &UA_TYPES[UA_TYPES_BOOLEAN])
        return QOpcUa::Types::Boolean;
    if (type == &UA_TYPES[UA_TYPES_INT32])
        return QOpcUa::Types::Int32;
    if (type == &UA_TYPES[UA_TYPES_UINT32])
        return QOpcUa::Types::UInt32;
    if (type == &UA_TYPES[UA_TYPES_DOUBLE])
        return QOpcUa::Types::Double;
    if (type == &UA_TYPES[UA_TYPES_FLOAT])
        return QOpcUa::Types::Float;
    if (type == &UA_TYPES[UA_TYPES_STRING])
        return QOpcUa::Types::String;
    if (type == &UA_TYPES[UA_TYPES_LOCALIZEDTEXT])
        return QOpcUa::Types::LocalizedText;
    if (type == &UA_TYPES[UA_TYPES_DATETIME])
        return QOpcUa::Types::DateTime;
    if (type == &UA_TYPES[UA_TYPES_UINT16])
        return QOpcUa::Types::UInt16;
    if (type == &UA_TYPES[UA_TYPES_INT16])
        return QOpcUa::Types::Int16;
    if (type == &UA_TYPES[UA_TYPES_UINT64])
        return QOpcUa::Types::UInt64;
    if (type == &UA_TYPES[UA_TYPES_INT64])
        return QOpcUa::Types::Int64;
    if (type == &UA_TYPES[UA_TYPES_BYTE])
        return QOpcUa::Types::Byte;
    if (type == &UA_TYPES[UA_TYPES_SBYTE])
        return QOpcUa::Types::SByte;
    if (type == &UA_TYPES[UA_TYPES_BYTESTRING])
        return QOpcUa::Types::ByteString;
    if (type == &UA_TYPES[UA_TYPES_XMLELEMENT])
        return QOpcUa::Types::XmlElement;
    if (type == &UA_TYPES[UA_TYPES_NODEID])
        return QOpcUa::Types::NodeId;
    if (type == &UA_TYPES[UA_TYPES_GUID])
        return QOpcUa::Types::Guid;
    if (type == &UA_TYPES[UA_TYPES_QUALIFIEDNAME])
        return QOpcUa::Types::QualifiedName;
    if (type == &UA_TYPES[UA_TYPES_STATUSCODE])
        return QOpcUa::Types::StatusCode;
    if (type == &UA_TYPES[UA_TYPES_RANGE])
        return QOpcUa::Types::Range;
    if (type == &UA_TYPES[UA_TYPES_EUINFORMATION])
        return QOpcUa::Types::EUInformation;
    if (type == &UA_TYPES[UA_TYPES_COMPLEXNUMBERTYPE])
        return QOpcUa::Types::ComplexNumber;
    if (type == &UA_TYPES[UA_TYPES_DOUBLECOMPLEXNUMBERTYPE])
        return QOpcUa::Types::DoubleComplexNumber;
    if (type == &UA_TYPES[UA_TYPES_AXISINFORMATION])
        return QOpcUa::Types::AxisInformation;
    if (type == &UA_TYPES[UA_TYPES_XVTYPE])
        return QOpcUa::Types::XV;
    if (type == &UA_TYPES[UA_TYPES_EXTENSIONOBJECT])
        return QOpcUa::Types::ExtensionObject;
    if (type == &UA_TYPES[UA_TYPES_EXPANDEDNODEID])
        return QOpcUa::Types::ExpandedNodeId;
    if (type == &UA_TYPES[UA_TYPES_ARGUMENT])
        return QOpcUa::Types::Argument;
    if (type == &UA_TYPES[UA_TYPES_STRUCTUREDEFINITION])
        return QOpcUa::Types::StructureDefinition;
    if (type == &UA_TYPES[UA_TYPES_STRUCTUREFIELD])
        return QOpcUa::Types::StructureField;
    if (type == &UA_TYPES[UA_TYPES_ENUMDEFINITION])
        return QOpcUa::Types::EnumDefinition;
    if (type == &UA_TYPES[UA_TYPES_ENUMFIELD])
        return QOpcUa::Types::EnumField;
    if (type == &UA_TYPES[UA_TYPES_DIAGNOSTICINFO])
        return QOpcUa::Types::DiagnosticInfo;
    if (type == &UA_TYPES[UA_TYPES_SIMPLEATTRIBUTEOPERAND])
        return QOpcUa::Types::SimpleAttributeOperand;
    if (type == &UA_TYPES[UA_TYPES_ATTRIBUTEOPERAND])
        return QOpcUa::Types::AttributeOperand;
    if (type == &UA_TYPES[UA_TYPES_LITERALOPERAND])
        return QOpcUa::Types::LiteralOperand;
    if (type == &UA_TYPES[UA_TYPES_ELEMENTOPERAND])
        return QOpcUa::Types::ElementOperand;
    if (type == &UA_TYPES[UA_TYPES_RELATIVEPATHELEMENT])
        return QOpcUa::Types::RelativePathElement;
    if (type == &UA_TYPES[UA_TYPES_CONTENTFILTERELEMENT])
        return QOpcUa::Types::ContentFilterElement;
    if (type == &UA_TYPES[UA_TYPES_EVENTFILTER])
        return QOpcUa::Types::EventFilter;

    qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Trying to convert unhandled type:" << (type ? type->typeName : "Unknown");
    return QOpcUa::Types::Undefined;
}
}

QT_END_NAMESPACE

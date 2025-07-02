// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "quacppvalueconverter.h"
#include "quacpputils.h"

#include <QtOpcUa/qopcuabinarydataencoding.h>
#include <QtOpcUa/qopcuamultidimensionalarray.h>

#include <QtCore/QDateTime>
#include <QtCore/QLoggingCategory>
#include <QtCore/QTimeZone>
#include <QtCore/QUuid>

#include <uastring.h>
#include <ualocalizedtext.h>
#include <uadatetime.h>
#include <uaguid.h>
#include <uavariant.h>
#include <uaextensionobject.h>
#include <uaeuinformation.h>
#include <uaaxisinformation.h>
#include <uaargument.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_UACPP)

using namespace QOpcUa::NodeIds;

namespace QUACppValueConverter {

OpcUa_BuiltInType toDataType(QOpcUa::Types valueType)
{
    switch (valueType) {
    case QOpcUa::Boolean:
        return OpcUa_BuiltInType::OpcUaType_Boolean;
    case QOpcUa::Int32:
        return OpcUa_BuiltInType::OpcUaType_Int32;
    case QOpcUa::UInt32:
        return OpcUa_BuiltInType::OpcUaType_UInt32;
    case QOpcUa::Double:
        return OpcUa_BuiltInType::OpcUaType_Double;
    case QOpcUa::Float:
        return OpcUa_BuiltInType::OpcUaType_Float;
    case QOpcUa::String:
        return OpcUa_BuiltInType::OpcUaType_String;
    case QOpcUa::LocalizedText:
        return OpcUa_BuiltInType::OpcUaType_LocalizedText;
    case QOpcUa::DateTime:
        return OpcUa_BuiltInType::OpcUaType_DateTime;
    case QOpcUa::UInt16:
        return OpcUa_BuiltInType::OpcUaType_UInt16;
    case QOpcUa::Int16:
        return OpcUa_BuiltInType::OpcUaType_Int16;
    case QOpcUa::UInt64:
        return OpcUa_BuiltInType::OpcUaType_UInt64;
    case QOpcUa::Int64:
        return OpcUa_BuiltInType::OpcUaType_Int64;
    case QOpcUa::Byte:
        return OpcUa_BuiltInType::OpcUaType_Byte;
    case QOpcUa::SByte:
        return OpcUa_BuiltInType::OpcUaType_SByte;
    case QOpcUa::ByteString:
        return OpcUa_BuiltInType::OpcUaType_ByteString;
    case QOpcUa::XmlElement:
        return OpcUa_BuiltInType::OpcUaType_XmlElement;
    case QOpcUa::NodeId:
        return OpcUa_BuiltInType::OpcUaType_NodeId;
    case QOpcUa::Guid:
        return OpcUa_BuiltInType::OpcUaType_Guid;
    case QOpcUa::QualifiedName:
        return OpcUa_BuiltInType::OpcUaType_QualifiedName;
    case QOpcUa::StatusCode:
        return OpcUa_BuiltInType::OpcUaType_StatusCode;
    case QOpcUa::Range:
    case QOpcUa::EUInformation:
    case QOpcUa::ComplexNumber:
    case QOpcUa::DoubleComplexNumber:
    case QOpcUa::AxisInformation:
    case QOpcUa::XV:
    case QOpcUa::Argument:
    case QOpcUa::ExtensionObject:
        return OpcUa_BuiltInType::OpcUaType_ExtensionObject;
    case QOpcUa::ExpandedNodeId:
        return OpcUa_BuiltInType::OpcUaType_ExpandedNodeId;
    default:
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Trying to convert undefined type:" << valueType;
        return OpcUa_BuiltInType::OpcUaType_Null;
    }
}

template<typename TARGETTYPE, typename UATYPE>
QVariant scalarToQVariant(UATYPE *data, QMetaType::Type type)
{
    return QVariant(QMetaType(type), reinterpret_cast<const TARGETTYPE *>(data));
}

template<>
QVariant scalarToQVariant<QString, OpcUa_String>(OpcUa_String *data, QMetaType::Type type)
{
    Q_UNUSED(type)
    const UaString str(data);
    return QVariant(QString::fromUtf8(str.toUtf8(), str.size()));
}

template<>
QVariant scalarToQVariant<QString, OpcUa_XmlElement>(OpcUa_XmlElement *data, QMetaType::Type type)
{
    Q_UNUSED(type);
    return QVariant(QString::fromUtf8((char*)data->Data, data->Length));
}

template<>
QVariant scalarToQVariant<QByteArray, OpcUa_ByteString>(OpcUa_ByteString *data, QMetaType::Type type)
{
    Q_UNUSED(type)
    UaByteArray ba(*data);
    return QVariant(QByteArray(ba.data(), ba.size()));
}

template<>
QVariant scalarToQVariant<QString, OpcUa_LocalizedText>(OpcUa_LocalizedText *data, QMetaType::Type type)
{
    Q_UNUSED(type)
    UaLocalizedText uaLocalizedText(*data);
    return QVariant::fromValue(toQOpcUaLocalizedText(&uaLocalizedText));
}

template<>
QVariant scalarToQVariant<QOpcUaLocalizedText, OpcUa_LocalizedText>(OpcUa_LocalizedText *data, QMetaType::Type type)
{
    Q_UNUSED(type)
    UaLocalizedText ualt(*data);
    const UaString ualtLocal(ualt.locale());
    const UaString ualtText(ualt.text());

    QOpcUaLocalizedText lt;

    lt.setLocale(QString::fromUtf8(ualtLocal.toUtf8(), ualtLocal.size()));
    lt.setText(QString::fromUtf8(ualtText.toUtf8(), ualtText.size()));
    return QVariant::fromValue(lt);
}

template<>
QVariant scalarToQVariant<QString, OpcUa_NodeId>(OpcUa_NodeId *data, QMetaType::Type type)
{
    Q_UNUSED(type)
    const UaNodeId id(*data);
    return QVariant(UACppUtils::nodeIdToQString(id));
}

template<>
QVariant scalarToQVariant<QDateTime, OpcUa_DateTime>(OpcUa_DateTime *data, QMetaType::Type type)
{
    Q_UNUSED(type)
    return QVariant(toQDateTime(data));
}

template<>
QVariant scalarToQVariant<QUuid, OpcUa_Guid>(OpcUa_Guid *data, QMetaType::Type type)
{
    Q_UNUSED(type)
    return QUuid(data->Data1,    data->Data2,    data->Data3,
                 data->Data4[0], data->Data4[1], data->Data4[2], data->Data4[3],
                 data->Data4[4], data->Data4[5], data->Data4[6], data->Data4[7]);
}

template<>
QVariant scalarToQVariant<QOpcUaExpandedNodeId, OpcUa_ExpandedNodeId>(OpcUa_ExpandedNodeId *data, QMetaType::Type type)
{
    Q_UNUSED(type)
    QOpcUaExpandedNodeId temp;
    temp.setServerIndex(data->ServerIndex);
    temp.setNodeId(UACppUtils::nodeIdToQString(data->NodeId));
    temp.setNamespaceUri(QString::fromUtf8(UaString(data->NamespaceUri).toUtf8(),
                                           UaString(data->NamespaceUri).size()));
    return temp;
}

template<>
QVariant scalarToQVariant<QOpcUaQualifiedName, OpcUa_QualifiedName>(OpcUa_QualifiedName *data, QMetaType::Type type)
{
    Q_UNUSED(type);
    QOpcUaQualifiedName temp;
    temp.setNamespaceIndex(data->NamespaceIndex);
    temp.setName(scalarToQVariant<QString, OpcUa_String>(&data->Name, QMetaType::Type::QString).toString());
    return QVariant::fromValue(temp);
}

inline QOpcUaEUInformation UaEUInformationToQOpcUaEUInformation(const UaEUInformation &info)
{
    const UaLocalizedText desc = info.getDescription();
    const UaLocalizedText dispName = info.getDisplayName();
    const UaString namespaceUri = info.getNamespaceUri();

    quint32 qunitId = info.getUnitId();
    QOpcUaLocalizedText qDesc = scalarToQVariant<QOpcUaLocalizedText, OpcUa_LocalizedText>(const_cast<OpcUa_LocalizedText *>(&*desc)).value<QOpcUaLocalizedText>();
    QOpcUaLocalizedText qDisp = scalarToQVariant<QOpcUaLocalizedText, OpcUa_LocalizedText>(const_cast<OpcUa_LocalizedText *>(&*dispName)).value<QOpcUaLocalizedText>();
    QString qNamespaceUri = QString::fromUtf8(namespaceUri.toUtf8(), namespaceUri.size());
    QOpcUaEUInformation euinfo(qNamespaceUri, qunitId, qDisp, qDesc);
    return euinfo;
}

template<>
QVariant scalarToQVariant<QVariant, OpcUa_ExtensionObject>(OpcUa_ExtensionObject *data, QMetaType::Type type)
{
    Q_UNUSED(type);

    if (data->Encoding == OpcUa_ExtensionObjectEncoding_Binary) {
        QOpcUaExtensionObject obj;
        obj.setEncoding(static_cast<QOpcUaExtensionObject::Encoding>(data->Encoding));
        const UaExpandedNodeId uaExpandedNodeId(data->TypeId);
        const UaNodeId uaTypeId(uaExpandedNodeId.nodeId());
        obj.setEncodingTypeId(UACppUtils::nodeIdToQString(uaTypeId));
        // Copy data for later use
        obj.setEncodedBody(QByteArray(reinterpret_cast<char*>(data->Body.Binary.Data), data->Body.Binary.Length));
        return obj;
    }

    if (data->TypeId.NodeId.IdentifierType != OpcUa_IdentifierType_Numeric ||
            data->TypeId.NodeId.NamespaceIndex != 0 ||
            data->Encoding != OpcUa_ExtensionObjectEncoding_EncodeableObject)
    {
        qCWarning(QT_OPCUA_PLUGINS_UACPP, "Unsupported extension object type");
        return QVariant();
    }

    QByteArray buffer = QByteArray::fromRawData(reinterpret_cast<const char *>(data->Body.EncodeableObject.Object),
                                                      data->Body.EncodeableObject.Type->AllocationSize);
    QOpcUaBinaryDataEncoding decoder(&buffer);

    bool success = true;
    QVariant result;
    Namespace0 objType = Namespace0(data->TypeId.NodeId.Identifier.Numeric);
    switch (objType) {
    case Namespace0::EUInformation_Encoding_DefaultBinary: {
        // ### TODO: Check for non ns=0 cases. Apparently there is an auto-conversion happening in the
        // underlying OpcUA SDK for ns=0 nodes, hence we do not receive a ByteStream, but rather an
        // already decoded object. For non ns=0, we might be able to use the generic decoder from the module.
        // Same for AxisInformation
        UaEUInformation info(*data);
        QOpcUaEUInformation euinfo = UaEUInformationToQOpcUaEUInformation(info);
        result = QVariant::fromValue(euinfo);
        break;
    }
    case Namespace0::Range_Encoding_DefaultBinary:
        result = QVariant::fromValue(decoder.decode<QOpcUaRange>(success));
        break;
    case Namespace0::ComplexNumberType_Encoding_DefaultBinary:
        result = QVariant::fromValue(decoder.decode<QOpcUaComplexNumber>(success));
        break;
    case Namespace0::DoubleComplexNumberType_Encoding_DefaultBinary:
        result = QVariant::fromValue(decoder.decode<QOpcUaDoubleComplexNumber>(success));
        break;
    case Namespace0::AxisInformation_Encoding_DefaultBinary: {
        UaAxisInformation info(*data);
        const UaRange uaRange = info.getEURange();
        const UaLocalizedText uaTitle = info.getTitle();
        UaDoubleArray uaDoubleArray;
        info.getAxisSteps(uaDoubleArray);

        const QOpcUaEUInformation qEuInfo = UaEUInformationToQOpcUaEUInformation(info.getEngineeringUnits());
        const QOpcUaRange qRange(uaRange.getLow(), uaRange.getHigh());
        const QOpcUaLocalizedText qTitle = scalarToQVariant<QOpcUaLocalizedText, OpcUa_LocalizedText>(const_cast<OpcUa_LocalizedText *>(&*uaTitle)).value<QOpcUaLocalizedText>();;
        const QOpcUa::AxisScale qScale = static_cast<QOpcUa::AxisScale>(info.getAxisScaleType());
        QList<double> qAxisSteps;
        for (OpcUa_UInt32 i = 0; i < uaDoubleArray.length(); ++i)
            qAxisSteps.append(uaDoubleArray[i]);

        QOpcUaAxisInformation qAxisInfo(qEuInfo, qRange, qTitle, qScale, qAxisSteps);
        result = QVariant::fromValue(qAxisInfo);
        break;
    }
    case Namespace0::XVType_Encoding_DefaultBinary:
        result = QVariant::fromValue(decoder.decode<QOpcUaXValue>(success));
        break;
    case Namespace0::Argument_Encoding_DefaultBinary: {
        // This type is decoded transparently and must not be decoded binary
        UaArgument uaArgument(*data);
        QOpcUaArgument argument = toQArgument(&uaArgument);
        result = QVariant::fromValue(argument);
        break;
    }
    default:
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Unknown extension object type, returning raw data:" << UACppUtils::nodeIdToQString(data->TypeId.NodeId);
        result = QByteArray(buffer.constData(), buffer.size());
        success = true;
    }

    if (success)
        return result;
    else
        return QVariant();
}

template<typename TARGETTYPE, typename UATYPE>
QVariant arrayToQVariant(const OpcUa_Variant &var, QMetaType::Type type)
{
    if (var.ArrayType == OpcUa_VariantArrayType_Matrix) {
        UaVariant variant(var);

        // Ensure that the array dimensions fit in a QList
        if (variant.dimensionSize() > static_cast<OpcUa_Int32>((std::numeric_limits<int>::max)())) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP) << "The array dimensions do not fit in a QList.";
            return QOpcUaMultiDimensionalArray();
        }

        QVariantList list;
        for (OpcUa_Int32 i = 0; i < variant.noOfMatrixElements(); ++i) {
            UATYPE *temp = static_cast<UATYPE *>(var.Value.Matrix.Value.Array);
            list.append(scalarToQVariant<TARGETTYPE, UATYPE>(&temp[i], type));
        }

        QList<quint32> arrayDimensions;
        for (qint32 i = 0; i < variant.dimensionSize(); ++i)
            arrayDimensions.append(var.Value.Matrix.Dimensions[i]);
        return QOpcUaMultiDimensionalArray(list, arrayDimensions);
    }

    if (var.ArrayType == OpcUa_VariantArrayType_Array) {
        QVariantList list;
        for (OpcUa_Int32 i = 0; i < var.Value.Array.Length; ++i) {
            UATYPE *temp = (UATYPE *)var.Value.Array.Value.Array;
            list.append(scalarToQVariant<TARGETTYPE, UATYPE>(&temp[i], type));
        }
        if (list.size() == 1)
            return list.at(0);
        else
            return list;
    }
    UATYPE *temp = (UATYPE *)&var.Value;
    return scalarToQVariant<TARGETTYPE, UATYPE>(temp, type);
}

// We need specializations for OpcUa_Guid, OpcUa_QualifiedName, OpcUa_NodeId, OpcUa_LocalizedText
// and OpcUA_ExtensionObject as the scalar versions contain pointers to data, all the others
// contain the data itself. Hence, there is a difference between the array and scalar version
template<>
QVariant arrayToQVariant<QString, OpcUa_LocalizedText *>(const OpcUa_Variant &var, QMetaType::Type type)
{
    if (var.ArrayType == OpcUa_VariantArrayType_Array) {
        QVariantList list;
        for (OpcUa_Int32 i = 0; i < var.Value.Array.Length; ++i) {
            OpcUa_LocalizedText *temp = var.Value.Array.Value.LocalizedTextArray;
            list.append(scalarToQVariant<QString, OpcUa_LocalizedText>(&temp[i], type));
        }
        if (list.size() == 1)
            return list.at(0);
        else
            return list;
    }
    OpcUa_LocalizedText *temp = var.Value.LocalizedText;
    return scalarToQVariant<QString, OpcUa_LocalizedText>(temp, type);
}

template<>
QVariant arrayToQVariant<QString, OpcUa_NodeId *>(const OpcUa_Variant &var, QMetaType::Type type)
{
    if (var.ArrayType == OpcUa_VariantArrayType_Array) {
        QVariantList list;
        for (OpcUa_Int32 i = 0; i < var.Value.Array.Length; ++i) {
            OpcUa_NodeId *temp = var.Value.Array.Value.NodeIdArray;
            list.append(scalarToQVariant<QString, OpcUa_NodeId>(&temp[i], type));
        }
        if (list.size() == 1)
            return list.at(0);
        else
            return list;
    }
    OpcUa_NodeId *temp = var.Value.NodeId;
    return scalarToQVariant<QString, OpcUa_NodeId>(temp, type);
}

template<>
QVariant arrayToQVariant<QUuid, OpcUa_Guid *>(const OpcUa_Variant &var, QMetaType::Type type)
{
    if (var.ArrayType == OpcUa_VariantArrayType_Array) {
        QVariantList list;
        for (OpcUa_Int32 i = 0; i < var.Value.Array.Length; ++i) {
            OpcUa_Guid *temp = var.Value.Array.Value.GuidArray;
            list.append(scalarToQVariant<QUuid, OpcUa_Guid>(&temp[i], type));
        }
        if (list.size() == 1)
            return list.at(0);
        else
            return list;
    }
    OpcUa_Guid *temp = var.Value.Guid;
    return scalarToQVariant<QUuid, OpcUa_Guid>(temp, type);
}

template<>
QVariant arrayToQVariant<QOpcUaQualifiedName, OpcUa_QualifiedName *>(const OpcUa_Variant &var, QMetaType::Type type)
{
    if (var.ArrayType == OpcUa_VariantArrayType_Array) {
        QVariantList list;
        for (OpcUa_Int32 i = 0; i < var.Value.Array.Length; ++i) {
            OpcUa_QualifiedName *temp = var.Value.Array.Value.QualifiedNameArray;
            list.append(scalarToQVariant<QOpcUaQualifiedName, OpcUa_QualifiedName>(&temp[i], type));
        }
        if (list.size() == 1)
            return list.at(0);
        else
            return list;
    }
    OpcUa_QualifiedName *temp = var.Value.QualifiedName;
    return scalarToQVariant<QOpcUaQualifiedName, OpcUa_QualifiedName>(temp, type);
}

template<>
QVariant arrayToQVariant<QVariant, OpcUa_ExtensionObject *>(const OpcUa_Variant &var, QMetaType::Type type)
{
    if (var.ArrayType == OpcUa_VariantArrayType_Array) {
        QVariantList list;
        for (OpcUa_Int32 i = 0; i < var.Value.Array.Length; ++i) {
            OpcUa_ExtensionObject *temp = var.Value.Array.Value.ExtensionObjectArray;
            list.append(scalarToQVariant<QVariant, OpcUa_ExtensionObject>(&temp[i], type));
        }
        if (list.size() == 1)
            return list.at(0);
        else
            return list;
    }
    OpcUa_ExtensionObject *temp = var.Value.ExtensionObject;
    return scalarToQVariant<QVariant, OpcUa_ExtensionObject>(temp, type);
}

template<>
QVariant arrayToQVariant<QOpcUaExpandedNodeId, OpcUa_ExpandedNodeId *>(const OpcUa_Variant &var, QMetaType::Type type)
{
    if (var.ArrayType == OpcUa_VariantArrayType_Array) {
        QVariantList list;
        for (OpcUa_Int32 i = 0; i < var.Value.Array.Length; ++i) {
            OpcUa_ExpandedNodeId *temp = var.Value.Array.Value.ExpandedNodeIdArray;
            list.append(scalarToQVariant<QOpcUaExpandedNodeId, OpcUa_ExpandedNodeId>(&temp[i], type));
        }
        if (list.size() == 1)
            return list.at(0);
        else
            return list;
    }
    OpcUa_ExpandedNodeId *temp = var.Value.ExpandedNodeId;
    return scalarToQVariant<QOpcUaExpandedNodeId, OpcUa_ExpandedNodeId>(temp, type);
}

template<typename TARGETTYPE, typename QTTYPE>
void scalarFromQVariant(const QVariant &var, TARGETTYPE *ptr)
{
    *ptr = static_cast<TARGETTYPE>(var.value<QTTYPE>());
}

template<>
void scalarFromQVariant<OpcUa_DateTime, QDateTime>(const QVariant &var, OpcUa_DateTime *ptr)
{
    *ptr = toUACppDateTime(var.toDateTime());
}

template<>
void scalarFromQVariant<OpcUa_String, QString>(const QVariant &var, OpcUa_String *ptr)
{
    UaString str(var.toString().toUtf8().constData());
    str.copyTo(ptr);
}

template<>
void scalarFromQVariant<OpcUa_LocalizedText, QString>(const QVariant &var, OpcUa_LocalizedText *ptr)
{
    QOpcUaLocalizedText lt = var.canConvert<QOpcUaLocalizedText>()
            ? var.value<QOpcUaLocalizedText>()
            : QOpcUaLocalizedText(QLatin1String(""), var.value<QString>());

    *ptr = toUACppLocalizedText(lt);
}

template<>
void scalarFromQVariant<OpcUa_ByteString, QByteArray>(const QVariant &var, OpcUa_ByteString *ptr)
{
    QByteArray arr = var.toByteArray();
    UaByteString str(arr.length(), (OpcUa_Byte*)arr.data());
    str.copyTo(ptr);
}

template<>
void scalarFromQVariant<OpcUa_XmlElement, QString>(const QVariant &var, OpcUa_XmlElement *ptr)
{
    scalarFromQVariant<OpcUa_ByteString, QByteArray>(var, ptr);
}

template<>
void scalarFromQVariant<OpcUa_NodeId, QString>(const QVariant &var, OpcUa_NodeId *ptr)
{
    UaNodeId id = UaNodeId::fromXmlString(UaString(var.toString().toUtf8().constData()));
    id.copyTo(ptr);
}

template<>
void scalarFromQVariant<OpcUa_QualifiedName, QOpcUaQualifiedName>(const QVariant &var, OpcUa_QualifiedName *ptr)
{
    *ptr = toUACppQualifiedName(var.value<QOpcUaQualifiedName>());
}

template<>
void scalarFromQVariant<OpcUa_Guid, QUuid>(const QVariant &var, OpcUa_Guid *ptr)
{
    const QUuid uuid = var.toUuid();
    ptr->Data1 = uuid.data1;
    ptr->Data2 = uuid.data2;
    ptr->Data3 = uuid.data3;
    memcpy(ptr->Data4, uuid.data4, sizeof(uuid.data4));
}

void createExtensionObject(const QByteArray &data, const UaNodeId &typeId, OpcUa_ExtensionObject *ptr)
{
    OpcUa_ExtensionObject_Initialize(ptr);
    UaByteArray arr(data.data(), data.length());
    arr.copyTo(&ptr->Body.Binary);
    ptr->Encoding = OpcUa_ExtensionObjectEncoding_Binary;
    ptr->BodySize = data.length();
    typeId.copyTo(&ptr->TypeId.NodeId);
}

void createExtensionObject(const QByteArray &data, Namespace0 id, OpcUa_ExtensionObject *ptr)
{
    const UaNodeId temp(static_cast<OpcUa_UInt32>(id));
    createExtensionObject(data, temp, ptr);
}

template<>
void scalarFromQVariant<OpcUa_ExtensionObject, QOpcUaRange>(const QVariant &var, OpcUa_ExtensionObject *ptr)
{
    const QOpcUaRange range = var.value<QOpcUaRange>();
    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    encoder.encode<QOpcUaRange>(range);
    return createExtensionObject(temp, Namespace0::Range_Encoding_DefaultBinary, ptr);
}

template<>
void scalarFromQVariant<OpcUa_ExtensionObject, QOpcUaEUInformation>(const QVariant &var, OpcUa_ExtensionObject *ptr)
{
    const QOpcUaEUInformation info = var.value<QOpcUaEUInformation>();
    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    encoder.encode<QOpcUaEUInformation>(info);
    return createExtensionObject(temp, Namespace0::EUInformation_Encoding_DefaultBinary, ptr);
}

template<>
void scalarFromQVariant<OpcUa_ExtensionObject, QOpcUaComplexNumber>(const QVariant &var, OpcUa_ExtensionObject *ptr)
{
    const QOpcUaComplexNumber num = var.value<QOpcUaComplexNumber>();
    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    encoder.encode<QOpcUaComplexNumber>(num);
    return createExtensionObject(temp, Namespace0::ComplexNumberType_Encoding_DefaultBinary, ptr);
}

template<>
void scalarFromQVariant<OpcUa_ExtensionObject, QOpcUaDoubleComplexNumber>(const QVariant &var, OpcUa_ExtensionObject *ptr)
{
    const QOpcUaDoubleComplexNumber num = var.value<QOpcUaDoubleComplexNumber>();
    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    encoder.encode<QOpcUaDoubleComplexNumber>(num);
    return createExtensionObject(temp, Namespace0::DoubleComplexNumberType_Encoding_DefaultBinary, ptr);
}

template<>
void scalarFromQVariant<OpcUa_ExtensionObject, QOpcUaAxisInformation>(const QVariant &var, OpcUa_ExtensionObject *ptr)
{
    const QOpcUaAxisInformation num = var.value<QOpcUaAxisInformation>();
    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    encoder.encode<QOpcUaAxisInformation>(num);
    return createExtensionObject(temp, Namespace0::AxisInformation_Encoding_DefaultBinary, ptr);
}

template<>
void scalarFromQVariant<OpcUa_ExtensionObject, QOpcUaXValue>(const QVariant &var, OpcUa_ExtensionObject *ptr)
{
    const QOpcUaXValue num = var.value<QOpcUaXValue>();
    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    encoder.encode<QOpcUaXValue>(num);
    return createExtensionObject(temp, Namespace0::XVType_Encoding_DefaultBinary, ptr);
}

template<>
void scalarFromQVariant<OpcUa_ExpandedNodeId, QOpcUaExpandedNodeId>(const QVariant &var, OpcUa_ExpandedNodeId *ptr)
{
    *ptr = toUACppExpandedNodeId(var.value<QOpcUaExpandedNodeId>());
}

template<typename TARGETTYPE, typename QTTYPE>
OpcUa_Variant arrayFromQVariant(const QVariant &var, const OpcUa_BuiltInType type)
{
    OpcUa_Variant opcuavariant;
    OpcUa_Variant_Initialize(&opcuavariant);

    if (var.type() == QVariant::List) {
        const QVariantList list = var.toList();
        if (list.isEmpty())
            return opcuavariant;

        opcuavariant.Datatype = type;
        opcuavariant.ArrayType = OpcUa_VariantArrayType_Array;
        opcuavariant.Value.Array.Length = list.size();
        TARGETTYPE *arr = static_cast<TARGETTYPE *>(OpcUa_Alloc(list.size() * sizeof(TARGETTYPE)));
        opcuavariant.Value.Array.Value.Array = arr;

        for (int i = 0; i < list.size(); ++i)
            scalarFromQVariant<TARGETTYPE, QTTYPE>(list[i], &arr[i]);

        return opcuavariant;
    }

    TARGETTYPE *temp = reinterpret_cast<TARGETTYPE *>(&opcuavariant.Value);
    scalarFromQVariant<TARGETTYPE, QTTYPE>(var, temp);
    opcuavariant.Datatype = type;
    return opcuavariant;
}

// We need specializations for OpcUa_Guid, OpcUa_QualifiedName, OpcUa_NodeId, OpcUa_LocalizedText
// and OpcUa_ExtensionObject as the scalar versions contain pointers to data, all the others
// contain the data itself. Hence, there is a difference between the array and scalar version
template<typename TARGETTYPE, typename QTTYPE>
OpcUa_Variant arrayFromQVariantPointer(const QVariant &var, const OpcUa_BuiltInType type)
{
    OpcUa_Variant opcuavariant;
    OpcUa_Variant_Initialize(&opcuavariant);

    if (var.type() == QVariant::List) {
        const QVariantList list = var.toList();
        if (list.isEmpty())
            return opcuavariant;

        opcuavariant.Datatype = type;
        opcuavariant.ArrayType = OpcUa_VariantArrayType_Array;
        opcuavariant.Value.Array.Length = list.size();
        TARGETTYPE *arr = static_cast<TARGETTYPE *>(OpcUa_Alloc(list.size() * sizeof(TARGETTYPE)));

        opcuavariant.Value.Array.Value.Array = arr;

        for (int i = 0; i < list.size(); ++i)
            scalarFromQVariant<TARGETTYPE, QTTYPE>(list[i], &arr[i]);

        return opcuavariant;
    }

    // Taking one pointer for all as it is union
    TARGETTYPE **temp = reinterpret_cast<TARGETTYPE **>(&opcuavariant.Value.Guid);
    // We have to allocate, otherwise copyTo() will not do any action
    *temp = static_cast<TARGETTYPE *>(OpcUa_Alloc(sizeof(TARGETTYPE)));
    scalarFromQVariant<TARGETTYPE, QTTYPE>(var, *temp);
    opcuavariant.Datatype = type;
    return opcuavariant;
}

template<>
OpcUa_Variant arrayFromQVariant<OpcUa_Guid, QUuid>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_Guid, QUuid>(var, type);
}

template<>
OpcUa_Variant arrayFromQVariant<OpcUa_QualifiedName, QOpcUaQualifiedName>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_QualifiedName, QOpcUaQualifiedName>(var, type);
}

template<>
OpcUa_Variant arrayFromQVariant<OpcUa_NodeId, QString>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_NodeId, QString>(var, type);
}

template<>
OpcUa_Variant arrayFromQVariant<OpcUa_LocalizedText, QString>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_LocalizedText, QString>(var, type);
}

template<>
OpcUa_Variant arrayFromQVariant<OpcUa_ExtensionObject, QOpcUaRange>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_ExtensionObject, QOpcUaRange>(var, type);
}

template<>
OpcUa_Variant arrayFromQVariant<OpcUa_ExtensionObject, QOpcUaEUInformation>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_ExtensionObject, QOpcUaEUInformation>(var, type);
}

template<>
OpcUa_Variant arrayFromQVariant<OpcUa_ExtensionObject, QOpcUaComplexNumber>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_ExtensionObject, QOpcUaComplexNumber>(var, type);
}

template<>
OpcUa_Variant arrayFromQVariant<OpcUa_ExtensionObject, QOpcUaDoubleComplexNumber>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_ExtensionObject, QOpcUaDoubleComplexNumber>(var, type);
}

template<>
OpcUa_Variant arrayFromQVariant<OpcUa_ExtensionObject, QOpcUaAxisInformation>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_ExtensionObject, QOpcUaAxisInformation>(var, type);
}

template<>
OpcUa_Variant arrayFromQVariant<OpcUa_ExtensionObject, QOpcUaXValue>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_ExtensionObject, QOpcUaXValue>(var, type);
}

template<>
OpcUa_Variant arrayFromQVariant<OpcUa_ExpandedNodeId, QOpcUaExpandedNodeId>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_ExpandedNodeId, QOpcUaExpandedNodeId>(var, type);
}

template<>
OpcUa_Variant arrayFromQVariant<OpcUa_ExtensionObject, QOpcUaArgument>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_ExtensionObject, QOpcUaArgument>(var, type);
}

template<>
OpcUa_Variant arrayFromQVariant<OpcUa_ExtensionObject, QOpcUaExtensionObject>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_ExtensionObject, QOpcUaExtensionObject>(var, type);
}

QVariant toQVariant(const OpcUa_Variant &value)
{
    switch (value.Datatype) {
    case OpcUa_BuiltInType::OpcUaType_Boolean:
        return arrayToQVariant<bool, OpcUa_Boolean>(value, QMetaType::Bool);
    case OpcUa_BuiltInType::OpcUaType_SByte:
        return arrayToQVariant<signed char, OpcUa_SByte>(value, QMetaType::SChar);
    case OpcUa_BuiltInType::OpcUaType_Byte:
        return arrayToQVariant<uchar, OpcUa_Byte>(value, QMetaType::UChar);
    case OpcUa_BuiltInType::OpcUaType_Int16:
        return arrayToQVariant<qint16, OpcUa_Int16>(value, QMetaType::Short);
    case OpcUa_BuiltInType::OpcUaType_UInt16:
        return arrayToQVariant<quint16, OpcUa_UInt16>(value, QMetaType::UShort);
    case OpcUa_BuiltInType::OpcUaType_Int32:
        return arrayToQVariant<qint32, OpcUa_Int32>(value, QMetaType::Int);
    case OpcUa_BuiltInType::OpcUaType_UInt32:
        return arrayToQVariant<quint32, OpcUa_UInt32>(value, QMetaType::UInt);
    case OpcUa_BuiltInType::OpcUaType_Int64:
        return arrayToQVariant<int64_t, OpcUa_Int64>(value, QMetaType::LongLong);
    case OpcUa_BuiltInType::OpcUaType_UInt64:
        return arrayToQVariant<uint64_t, OpcUa_UInt64>(value, QMetaType::ULongLong);
    case OpcUa_BuiltInType::OpcUaType_Float:
        return arrayToQVariant<float, OpcUa_Float>(value, QMetaType::Float);
    case OpcUa_BuiltInType::OpcUaType_Double:
        return arrayToQVariant<double, OpcUa_Double>(value, QMetaType::Double);
    case OpcUa_BuiltInType::OpcUaType_String:
        return arrayToQVariant<QString, OpcUa_String>(value, QMetaType::QString);
    case OpcUa_BuiltInType::OpcUaType_ByteString:
        return arrayToQVariant<QByteArray, OpcUa_ByteString>(value, QMetaType::QByteArray);
    case OpcUa_BuiltInType::OpcUaType_LocalizedText:
        return arrayToQVariant<QString, OpcUa_LocalizedText *>(value, QMetaType::QString);
    case OpcUa_BuiltInType::OpcUaType_NodeId:
        return arrayToQVariant<QString, OpcUa_NodeId *>(value, QMetaType::QString);
    case OpcUa_BuiltInType::OpcUaType_DateTime:
        return arrayToQVariant<QDateTime, OpcUa_DateTime>(value, QMetaType::QDateTime);
    case OpcUa_BuiltInType::OpcUaType_Guid:
        return arrayToQVariant<QUuid, OpcUa_Guid *>(value, QMetaType::QUuid);
    case OpcUa_BuiltInType::OpcUaType_XmlElement:
        return arrayToQVariant<QString, OpcUa_XmlElement>(value, QMetaType::QString);
    case OpcUa_BuiltInType::OpcUaType_QualifiedName:
        return arrayToQVariant<QOpcUaQualifiedName, OpcUa_QualifiedName *>(value);
    case OpcUa_BuiltInType::OpcUaType_StatusCode:
        return arrayToQVariant<QOpcUa::UaStatusCode, OpcUa_StatusCode>(value, QMetaType::UInt);
    case OpcUa_BuiltInType::OpcUaType_ExtensionObject:
        return arrayToQVariant<QVariant, OpcUa_ExtensionObject *>(value);
    case OpcUa_BuiltInType::OpcUaType_ExpandedNodeId:
        return arrayToQVariant<QOpcUaExpandedNodeId, OpcUa_ExpandedNodeId *>(value);
    default:
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Variant conversion from UACpp for typeIndex" << value.Datatype << " not implemented";
        return QVariant();
    }
}

/*constexpr*/ OpcUa_UInt32 toUaAttributeId(QOpcUa::NodeAttribute attr)
{
    switch (attr) {
    case QOpcUa::NodeAttribute::None:
        return 0;
    case QOpcUa::NodeAttribute::NodeId:
        return OpcUa_Attributes_NodeId;
    case QOpcUa::NodeAttribute::NodeClass:
        return OpcUa_Attributes_NodeClass;
    case QOpcUa::NodeAttribute::BrowseName:
        return OpcUa_Attributes_BrowseName;
    case QOpcUa::NodeAttribute::DisplayName:
        return OpcUa_Attributes_DisplayName;
    case QOpcUa::NodeAttribute::Description:
        return OpcUa_Attributes_Description;
    case QOpcUa::NodeAttribute::WriteMask:
        return OpcUa_Attributes_WriteMask;
    case QOpcUa::NodeAttribute::UserWriteMask:
        return OpcUa_Attributes_UserWriteMask;
    case QOpcUa::NodeAttribute::IsAbstract:
        return OpcUa_Attributes_IsAbstract;
    case QOpcUa::NodeAttribute::Symmetric:
        return OpcUa_Attributes_Symmetric;
    case QOpcUa::NodeAttribute::InverseName:
        return OpcUa_Attributes_InverseName;
    case QOpcUa::NodeAttribute::ContainsNoLoops:
        return OpcUa_Attributes_ContainsNoLoops;
    case QOpcUa::NodeAttribute::EventNotifier:
        return OpcUa_Attributes_EventNotifier;
    // Objects also add the EventNotifier attribute, see part 4, 5.5.1
    // ObjectType also add the IsAbstract attribute, see part 4, 5.5.2
    case QOpcUa::NodeAttribute::Value:
        return OpcUa_Attributes_Value;
    case QOpcUa::NodeAttribute::DataType:
        return OpcUa_Attributes_DataType;
    case QOpcUa::NodeAttribute::ValueRank:
        return OpcUa_Attributes_ValueRank;
    case QOpcUa::NodeAttribute::ArrayDimensions:
        return OpcUa_Attributes_ArrayDimensions;
    case QOpcUa::NodeAttribute::AccessLevel:
        return OpcUa_Attributes_AccessLevel;
    case QOpcUa::NodeAttribute::UserAccessLevel:
        return OpcUa_Attributes_UserAccessLevel;
    case QOpcUa::NodeAttribute::MinimumSamplingInterval:
        return OpcUa_Attributes_MinimumSamplingInterval;
    case QOpcUa::NodeAttribute::Historizing:
        return OpcUa_Attributes_Historizing;
    // VariableType also adds the Value, DataType, ValueRank, ArrayDimensions
    // and isAbstract attributes, see part 4, 5.6.5
    case QOpcUa::NodeAttribute::Executable:
        return OpcUa_Attributes_Executable;
    case QOpcUa::NodeAttribute::UserExecutable:
        return OpcUa_Attributes_UserExecutable;
    default:
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Unknown NodeAttribute to convert:" << attr;
        break;
    }
    return 0;
}

OpcUa_Variant toUACppVariant(const QVariant &value, QOpcUa::Types type)
{
    OpcUa_Variant uacppvalue;
    OpcUa_Variant_Initialize(&uacppvalue);

    if (value.canConvert<QOpcUaMultiDimensionalArray>()) {
        const QOpcUaMultiDimensionalArray data = value.value<QOpcUaMultiDimensionalArray>();
        OpcUa_Variant result = toUACppVariant(data.valueArray(), type);

        if (!data.arrayDimensions().isEmpty()) {
            // Ensure that the array dimensions size is < UINT32_MAX
            if (static_cast<quint64>(data.arrayDimensions().size()) > (std::numeric_limits<qint32>::max)()){
                qCWarning(QT_OPCUA_PLUGINS_UACPP) << "The array dimensions do not fit in a QList.";
                return uacppvalue;
            }
            result.ArrayType = OpcUa_VariantArrayType_Matrix;
            // Reuse the allocated array from toUaCppVariant().
            result.Value.Matrix.Value = result.Value.Array.Value;
            // Set and copy dimension information
            result.Value.Matrix.NoOfDimensions = static_cast<OpcUa_Int32>(data.arrayDimensions().size());
            result.Value.Matrix.Dimensions = static_cast<OpcUa_Int32 *>(OpcUa_Alloc(result.Value.Matrix.NoOfDimensions * sizeof(OpcUa_Int32)));
            std::copy(data.arrayDimensions().constBegin(), data.arrayDimensions().constEnd(), result.Value.Matrix.Dimensions);
        }
        return result;
    }

    if (value.type() == QVariant::List && value.toList().size() == 0)
        return uacppvalue;

    QVariant temp = (value.type() == QVariant::List) ? value.toList().at(0) : value;
    QOpcUa::Types valueType = type == QOpcUa::Undefined ?
                QOpcUa::metaTypeToQOpcUaType(static_cast<QMetaType::Type>(temp.type())) : type;

    const OpcUa_BuiltInType dt = toDataType(valueType);

    switch (valueType) {
    case QOpcUa::Boolean:
        return arrayFromQVariant<OpcUa_Boolean, bool>(value, dt);
    case QOpcUa::SByte:
        return arrayFromQVariant<OpcUa_SByte, char>(value, dt);
    case QOpcUa::Byte:
        return arrayFromQVariant<OpcUa_Byte, uchar>(value, dt);
    case QOpcUa::Int16:
        return arrayFromQVariant<OpcUa_Int16, qint16>(value, dt);
    case QOpcUa::UInt16:
        return arrayFromQVariant<OpcUa_UInt16, quint16>(value, dt);
    case QOpcUa::Int32:
        return arrayFromQVariant<OpcUa_Int32, qint32>(value, dt);
    case QOpcUa::UInt32:
        return arrayFromQVariant<OpcUa_UInt32, quint32>(value, dt);
    case QOpcUa::Int64:
        return arrayFromQVariant<OpcUa_Int64, int64_t>(value, dt);
    case QOpcUa::UInt64:
        return arrayFromQVariant<OpcUa_UInt64, uint64_t>(value, dt);
    case QOpcUa::Float:
        return arrayFromQVariant<OpcUa_Float, float>(value, dt);
    case QOpcUa::Double:
        return arrayFromQVariant<OpcUa_Double, double>(value, dt);
    case QOpcUa::DateTime:
        return arrayFromQVariant<OpcUa_DateTime, QDateTime>(value, dt);
    case QOpcUa::String:
        return arrayFromQVariant<OpcUa_String, QString>(value, dt);
    case QOpcUa::LocalizedText:
        return arrayFromQVariant<OpcUa_LocalizedText, QString>(value, dt);
    case QOpcUa::ByteString:
        return arrayFromQVariant<OpcUa_ByteString, QByteArray>(value, dt);
    case QOpcUa::NodeId:
        return arrayFromQVariant<OpcUa_NodeId, QString>(value, dt);
    case QOpcUa::Guid:
        return arrayFromQVariant<OpcUa_Guid, QUuid>(value, dt);
    case QOpcUa::XmlElement:
        return arrayFromQVariant<OpcUa_XmlElement, QString>(value, dt);
    case QOpcUa::QualifiedName:
        return arrayFromQVariant<OpcUa_QualifiedName, QOpcUaQualifiedName>(value, dt);
    case QOpcUa::StatusCode:
        return arrayFromQVariant<OpcUa_StatusCode, QOpcUa::UaStatusCode>(value, dt);
    case QOpcUa::Range:
        return arrayFromQVariant<OpcUa_ExtensionObject, QOpcUaRange>(value, dt);
    case QOpcUa::EUInformation:
        return arrayFromQVariant<OpcUa_ExtensionObject, QOpcUaEUInformation>(value, dt);
    case QOpcUa::ComplexNumber:
        return arrayFromQVariant<OpcUa_ExtensionObject, QOpcUaComplexNumber>(value, dt);
    case QOpcUa::DoubleComplexNumber:
        return arrayFromQVariant<OpcUa_ExtensionObject, QOpcUaDoubleComplexNumber>(value, dt);
    case QOpcUa::AxisInformation:
        return arrayFromQVariant<OpcUa_ExtensionObject, QOpcUaAxisInformation>(value, dt);
    case QOpcUa::XV:
        return arrayFromQVariant<OpcUa_ExtensionObject, QOpcUaXValue>(value, dt);
    case QOpcUa::ExpandedNodeId:
        return arrayFromQVariant<OpcUa_ExpandedNodeId, QOpcUaExpandedNodeId>(value, dt);
    case QOpcUa::Argument:
        return arrayFromQVariant<OpcUa_ExtensionObject, QOpcUaArgument>(value, dt);
    case QOpcUa::ExtensionObject:
        return arrayFromQVariant<OpcUa_ExtensionObject, QOpcUaExtensionObject>(value, dt);
    default:
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Variant conversion of" << value << "to UACpp for typeIndex" << type << " not implemented";
    }

    return uacppvalue;
}

QDateTime toQDateTime(const OpcUa_DateTime *dt)
{
    // Create an invalid timestamp if both values are zero
    if (dt->dwHighDateTime == 0 && dt->dwLowDateTime == 0)
        return QDateTime();

    // OPC-UA part 3, Table C.9
    const QDateTime uaEpochStart(QDate(1601, 1, 1), QTime(0, 0), QTimeZone::UTC);
    const UaDateTime temp(*dt);

    // OpcUa time is defined in part 6, 5.2.2.5 in 100ns which need to be converted to milliseconds.
    return uaEpochStart.addMSecs(((quint64)temp) / 10000).toLocalTime();
}

OpcUa_DateTime toUACppDateTime(const QDateTime &qtDateTime)
{
    // OPC-UA part 3, Table C.9
    const QDateTime uaEpochStart(QDate(1601, 1, 1), QTime(0, 0), QTimeZone::UTC);
    // OpcUa time is defined in part 6, 5.2.2.5 in 100ns which need to be converted to milliseconds.
    UaDateTime tmp((qtDateTime.toMSecsSinceEpoch() - uaEpochStart.toMSecsSinceEpoch()) * 10000);
    OpcUa_DateTime returnValue;
    tmp.copyTo(&returnValue);
    return returnValue;
}

QOpcUaLocalizedText toQOpcUaLocalizedText(UaLocalizedText *data)
{
    const UaString ualtLocal(data->locale());
    const UaString ualtText(data->text());

    QOpcUaLocalizedText lt;

    lt.setLocale(QString::fromUtf8(ualtLocal.toUtf8(), ualtLocal.size()));
    lt.setText(QString::fromUtf8(ualtText.toUtf8(), ualtText.size()));
    return lt;
}

UaStringArray toUaStringArray(const QStringList &value)
{
    UaStringArray ret;
    ret.create(value.size());

    for (int i = 0; i < value.size(); ++i) {
        OpcUa_String str;
        const QByteArray utf8Data = value[i].toUtf8();

        // Use QByteArray data buffer directly to form a OpcUa_String.
        OpcUa_String_AttachReadOnly(&str, utf8Data.constData());

        OpcUa_String_StrnCpy(&ret[i], &str, OPCUA_STRING_LENDONTCARE);
    }
    return ret;
}

OpcUa_ExpandedNodeId toUACppExpandedNodeId(const QOpcUaExpandedNodeId &qtExpandedNodeId)
{
    OpcUa_ExpandedNodeId returnValue;

    returnValue.ServerIndex = qtExpandedNodeId.serverIndex();
    UACppUtils::nodeIdFromQString(qtExpandedNodeId.nodeId()).copyTo(&returnValue.NodeId);
    UaString namespc(qtExpandedNodeId.namespaceUri().toUtf8().constData());
    namespc.detach(&returnValue.NamespaceUri);
    return returnValue;
}

OpcUa_QualifiedName toUACppQualifiedName(const QOpcUaQualifiedName& qtQualifiedName)
{
    OpcUa_QualifiedName returnValue;

    returnValue.NamespaceIndex = qtQualifiedName.namespaceIndex();
    UaString name(qtQualifiedName.name().toUtf8().constData());
    name.detach(&returnValue.Name);
    return returnValue;
}

OpcUa_LocalizedText toUACppLocalizedText(const QOpcUaLocalizedText &qtLocalizedText)
{
    OpcUa_LocalizedText uaLocalizedText;
    UaLocalizedText ualt;
    if (qtLocalizedText.locale().size())
        ualt.setLocale(UaString(qtLocalizedText.locale().toUtf8().constData()));
    if (qtLocalizedText.text().size())
        ualt.setText(UaString(qtLocalizedText.text().toUtf8().constData()));
    ualt.copyTo(&uaLocalizedText);
    return uaLocalizedText;
}

template<>
void scalarFromQVariant<OpcUa_ExtensionObject, QOpcUaArgument>(const QVariant &var, OpcUa_ExtensionObject *ptr)
{
    const auto value = var.value<QOpcUaArgument>();
    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    encoder.encode<QOpcUaArgument>(value);
    createExtensionObject(temp, Namespace0::Argument_Encoding_DefaultBinary, ptr);
}

QOpcUaArgument toQArgument(const UaArgument *data)
{
    QList<quint32> arrayDimensions;
    UaUInt32Array dataArrayDimensions;
    data->getArrayDimensions(dataArrayDimensions);
    for (quint32 i = 0; i < dataArrayDimensions.length(); ++i)
        arrayDimensions.append(dataArrayDimensions[i]);

    QOpcUaArgument argument;
    argument.setName(QString::fromUtf8(data->getName().toUtf8()));
    argument.setDataTypeId(UACppUtils::nodeIdToQString(data->getDataType()));
    argument.setValueRank(data->getValueRank());
    argument.setArrayDimensions(arrayDimensions);

    UaLocalizedText localizedText = data->getDescription();
    argument.setDescription(toQOpcUaLocalizedText(&localizedText));
    return argument;
}

template<>
void scalarFromQVariant<OpcUa_ExtensionObject, QOpcUaExtensionObject>(const QVariant &var, OpcUa_ExtensionObject *ptr)
{
    const auto value = var.value<QOpcUaExtensionObject>();
    const QByteArray temp = value.encodedBody();
    createExtensionObject(temp, UACppUtils::nodeIdFromQString(value.encodingTypeId()), ptr);
}

}

QT_END_NAMESPACE

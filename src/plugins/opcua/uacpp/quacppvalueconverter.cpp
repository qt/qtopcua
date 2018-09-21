/******************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
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
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#include "quacppvalueconverter.h"
#include "quacpputils.h"

#include <QtOpcUa/qopcuabinarydataencoding.h>

#include <QtCore/QDateTime>
#include <QtCore/QLoggingCategory>
#include <QtCore/QUuid>

#include <uabase/uastring.h>
#include <uabase/ualocalizedtext.h>
#include <uabase/uadatetime.h>
#include <uabase/uaguid.h>
#include <uabase/uavariant.h>
#include <uabase/uaextensionobject.h>
#include <uabase/uaeuinformation.h>
#include <uabase/uaaxisinformation.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_UACPP)

using namespace QOpcUa::NodeIds;

namespace QUACppValueConverter {

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
    return QVariant(type, reinterpret_cast<const TARGETTYPE *>(data));
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
    UaLocalizedText ualt(*data);
    const UaString ualtLocal(ualt.locale());
    const UaString ualtText(ualt.text());

    QOpcUa::QLocalizedText lt;

    lt.setLocale(QString::fromUtf8(ualtLocal.toUtf8(), ualtLocal.size()));
    lt.setText(QString::fromUtf8(ualtText.toUtf8(), ualtText.size()));
    return QVariant::fromValue(lt);
}

template<>
QVariant scalarToQVariant<QOpcUa::QLocalizedText, OpcUa_LocalizedText>(OpcUa_LocalizedText *data, QMetaType::Type type)
{
    Q_UNUSED(type)
    UaLocalizedText ualt(*data);
    const UaString ualtLocal(ualt.locale());
    const UaString ualtText(ualt.text());

    QOpcUa::QLocalizedText lt;

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
QVariant scalarToQVariant<QOpcUa::QExpandedNodeId, OpcUa_ExpandedNodeId>(OpcUa_ExpandedNodeId *data, QMetaType::Type type)
{
    Q_UNUSED(type)
    QOpcUa::QExpandedNodeId temp;
    temp.setServerIndex(data->ServerIndex);
    temp.setNodeId(UACppUtils::nodeIdToQString(data->NodeId));
    temp.setNamespaceUri(QString::fromUtf8(UaString(data->NamespaceUri).toUtf8(),
                                           UaString(data->NamespaceUri).size()));
    return temp;
}

template<>
QVariant scalarToQVariant<QOpcUa::QQualifiedName, OpcUa_QualifiedName>(OpcUa_QualifiedName *data, QMetaType::Type type)
{
    Q_UNUSED(type);
    QOpcUa::QQualifiedName temp;
    temp.setNamespaceIndex(data->NamespaceIndex);
    temp.setName(scalarToQVariant<QString, OpcUa_String>(&data->Name, QMetaType::Type::QString).toString());
    return QVariant::fromValue(temp);
}

inline QOpcUa::QEUInformation UaEUInformationToQEUInformation(const UaEUInformation &info)
{
    const UaLocalizedText desc = info.getDescription();
    const UaLocalizedText dispName = info.getDisplayName();
    const UaString namespaceUri = info.getNamespaceUri();

    quint32 qunitId = info.getUnitId();
    QOpcUa::QLocalizedText qDesc = scalarToQVariant<QOpcUa::QLocalizedText, OpcUa_LocalizedText>(const_cast<OpcUa_LocalizedText *>(&*desc)).value<QOpcUa::QLocalizedText>();
    QOpcUa::QLocalizedText qDisp = scalarToQVariant<QOpcUa::QLocalizedText, OpcUa_LocalizedText>(const_cast<OpcUa_LocalizedText *>(&*dispName)).value<QOpcUa::QLocalizedText>();
    QString qNamespaceUri = QString::fromUtf8(namespaceUri.toUtf8(), namespaceUri.size());
    QOpcUa::QEUInformation euinfo(qNamespaceUri, qunitId, qDisp, qDesc);
    return euinfo;
}

template<>
QVariant scalarToQVariant<QVariant, OpcUa_ExtensionObject>(OpcUa_ExtensionObject *data, QMetaType::Type type)
{
    Q_UNUSED(type);
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
        // underlaying OpcUA SDK for ns=0 nodes, hence we do not receive a ByteStream, but rather an
        // already decoded object. For non ns=0, we might be able to use the generic decoder from the module.
        // Same for AxisInformation
        UaEUInformation info(*data);
        QOpcUa::QEUInformation euinfo = UaEUInformationToQEUInformation(info);
        result = QVariant::fromValue(euinfo);
        break;
    }
    case Namespace0::Range_Encoding_DefaultBinary:
        result = QVariant::fromValue(decoder.decode<QOpcUa::QRange>(success));
        break;
    case Namespace0::ComplexNumberType_Encoding_DefaultBinary:
        result = QVariant::fromValue(decoder.decode<QOpcUa::QComplexNumber>(success));
        break;
    case Namespace0::DoubleComplexNumberType_Encoding_DefaultBinary:
        result = QVariant::fromValue(decoder.decode<QOpcUa::QDoubleComplexNumber>(success));
        break;
    case Namespace0::AxisInformation_Encoding_DefaultBinary: {
        UaAxisInformation info(*data);
        const UaRange uaRange = info.getEURange();
        const UaLocalizedText uaTitle = info.getTitle();
        UaDoubleArray uaDoubleArray;
        info.getAxisSteps(uaDoubleArray);

        const QOpcUa::QEUInformation qEuInfo = UaEUInformationToQEUInformation(info.getEngineeringUnits());
        const QOpcUa::QRange qRange(uaRange.getLow(), uaRange.getHigh());
        const QOpcUa::QLocalizedText qTitle = scalarToQVariant<QOpcUa::QLocalizedText, OpcUa_LocalizedText>(const_cast<OpcUa_LocalizedText *>(&*uaTitle)).value<QOpcUa::QLocalizedText>();;
        const QOpcUa::AxisScale qScale = static_cast<QOpcUa::AxisScale>(info.getAxisScaleType());
        QVector<double> qAxisSteps;
        for (OpcUa_UInt32 i = 0; i < uaDoubleArray.length(); ++i)
            qAxisSteps.append(uaDoubleArray[i]);

        QOpcUa::QAxisInformation qAxisInfo(qEuInfo, qRange, qTitle, qScale, qAxisSteps);
        result = QVariant::fromValue(qAxisInfo);
        break;
    }
    case Namespace0::XVType_Encoding_DefaultBinary:
        result = QVariant::fromValue(decoder.decode<QOpcUa::QXValue>(success));
        break;
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
QVariant arrayToQVariant<QOpcUa::QQualifiedName, OpcUa_QualifiedName *>(const OpcUa_Variant &var, QMetaType::Type type)
{
    if (var.ArrayType == OpcUa_VariantArrayType_Array) {
        QVariantList list;
        for (OpcUa_Int32 i = 0; i < var.Value.Array.Length; ++i) {
            OpcUa_QualifiedName *temp = var.Value.Array.Value.QualifiedNameArray;
            list.append(scalarToQVariant<QOpcUa::QQualifiedName, OpcUa_QualifiedName>(&temp[i], type));
        }
        if (list.size() == 1)
            return list.at(0);
        else
            return list;
    }
    OpcUa_QualifiedName *temp = var.Value.QualifiedName;
    return scalarToQVariant<QOpcUa::QQualifiedName, OpcUa_QualifiedName>(temp, type);
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
QVariant arrayToQVariant<QOpcUa::QExpandedNodeId, OpcUa_ExpandedNodeId *>(const OpcUa_Variant &var, QMetaType::Type type)
{
    if (var.ArrayType == OpcUa_VariantArrayType_Array) {
        QVariantList list;
        for (OpcUa_Int32 i = 0; i < var.Value.Array.Length; ++i) {
            OpcUa_ExpandedNodeId *temp = var.Value.Array.Value.ExpandedNodeIdArray;
            list.append(scalarToQVariant<QOpcUa::QExpandedNodeId, OpcUa_ExpandedNodeId>(&temp[i], type));
        }
        if (list.size() == 1)
            return list.at(0);
        else
            return list;
    }
    OpcUa_ExpandedNodeId *temp = var.Value.ExpandedNodeId;
    return scalarToQVariant<QOpcUa::QExpandedNodeId, OpcUa_ExpandedNodeId>(temp, type);
}

template<typename TARGETTYPE, typename QTTYPE>
void scalarFromQVariant(const QVariant &var, TARGETTYPE *ptr)
{
    *ptr = static_cast<TARGETTYPE>(var.value<QTTYPE>());
}

template<>
void scalarFromQVariant<OpcUa_DateTime, QDateTime>(const QVariant &var, OpcUa_DateTime *ptr)
{
    // OPC-UA part 3, Table C.9
    const QDateTime uaEpochStart(QDate(1601, 1, 1), QTime(0, 0), Qt::UTC);
    const UaDateTime dt = UaDateTime(var.toDateTime().toMSecsSinceEpoch() - uaEpochStart.toMSecsSinceEpoch());
    *ptr = dt;
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
    QOpcUa::QLocalizedText lt = var.canConvert<QOpcUa::QLocalizedText>()
            ? var.value<QOpcUa::QLocalizedText>()
            : QOpcUa::QLocalizedText(QLatin1String(""), var.value<QString>());

    UaLocalizedText ualt;
    if (lt.locale().size())
        ualt.setLocale(UaString(lt.locale().toUtf8().constData()));
    if (lt.text().size())
        ualt.setText(UaString(lt.text().toUtf8().constData()));
    ualt.copyTo(ptr);
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
void scalarFromQVariant<OpcUa_QualifiedName, QOpcUa::QQualifiedName>(const QVariant &var, OpcUa_QualifiedName *ptr)
{
    QOpcUa::QQualifiedName temp = var.value<QOpcUa::QQualifiedName>();
    ptr->NamespaceIndex = temp.namespaceIndex();
    const UaString name(temp.name().toUtf8().constData());
    name.copyTo(&ptr->Name);
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

void createExtensionObject(QByteArray &data, Namespace0 id, OpcUa_ExtensionObject *ptr)
{
    OpcUa_ExtensionObject_Initialize(ptr);
    UaByteArray arr(data.data(), data.length());
    arr.copyTo(&ptr->Body.Binary);
    ptr->Encoding = OpcUa_ExtensionObjectEncoding_Binary;
    ptr->BodySize = data.length();
    const UaNodeId temp(static_cast<OpcUa_UInt32>(id));
    temp.copyTo(&ptr->TypeId.NodeId);
}

template<>
void scalarFromQVariant<OpcUa_ExtensionObject, QOpcUa::QRange>(const QVariant &var, OpcUa_ExtensionObject *ptr)
{
    const QOpcUa::QRange range = var.value<QOpcUa::QRange>();
    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    encoder.encode<QOpcUa::QRange>(range);
    return createExtensionObject(temp, Namespace0::Range_Encoding_DefaultBinary, ptr);
}

template<>
void scalarFromQVariant<OpcUa_ExtensionObject, QOpcUa::QEUInformation>(const QVariant &var, OpcUa_ExtensionObject *ptr)
{
    const QOpcUa::QEUInformation info = var.value<QOpcUa::QEUInformation>();
    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    encoder.encode<QOpcUa::QEUInformation>(info);
    return createExtensionObject(temp, Namespace0::EUInformation_Encoding_DefaultBinary, ptr);
}

template<>
void scalarFromQVariant<OpcUa_ExtensionObject, QOpcUa::QComplexNumber>(const QVariant &var, OpcUa_ExtensionObject *ptr)
{
    const QOpcUa::QComplexNumber num = var.value<QOpcUa::QComplexNumber>();
    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    encoder.encode<QOpcUa::QComplexNumber>(num);
    return createExtensionObject(temp, Namespace0::ComplexNumberType_Encoding_DefaultBinary, ptr);
}

template<>
void scalarFromQVariant<OpcUa_ExtensionObject, QOpcUa::QDoubleComplexNumber>(const QVariant &var, OpcUa_ExtensionObject *ptr)
{
    const QOpcUa::QDoubleComplexNumber num = var.value<QOpcUa::QDoubleComplexNumber>();
    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    encoder.encode<QOpcUa::QDoubleComplexNumber>(num);
    return createExtensionObject(temp, Namespace0::DoubleComplexNumberType_Encoding_DefaultBinary, ptr);
}

template<>
void scalarFromQVariant<OpcUa_ExtensionObject, QOpcUa::QAxisInformation>(const QVariant &var, OpcUa_ExtensionObject *ptr)
{
    const QOpcUa::QAxisInformation num = var.value<QOpcUa::QAxisInformation>();
    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    encoder.encode<QOpcUa::QAxisInformation>(num);
    return createExtensionObject(temp, Namespace0::AxisInformation_Encoding_DefaultBinary, ptr);
}

template<>
void scalarFromQVariant<OpcUa_ExtensionObject, QOpcUa::QXValue>(const QVariant &var, OpcUa_ExtensionObject *ptr)
{
    const QOpcUa::QXValue num = var.value<QOpcUa::QXValue>();
    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    encoder.encode<QOpcUa::QXValue>(num);
    return createExtensionObject(temp, Namespace0::XVType_Encoding_DefaultBinary, ptr);
}

template<>
void scalarFromQVariant<OpcUa_ExpandedNodeId, QOpcUa::QExpandedNodeId>(const QVariant &var, OpcUa_ExpandedNodeId *ptr)
{
    const QOpcUa::QExpandedNodeId temp = var.value<QOpcUa::QExpandedNodeId>();
    ptr->ServerIndex = temp.serverIndex();
    UACppUtils::nodeIdFromQString(temp.nodeId()).copyTo(&ptr->NodeId);
    UaString(temp.namespaceUri().toUtf8().constData()).copyTo(&ptr->NamespaceUri);
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
        opcuavariant.ArrayType = OpcUa_True;
        opcuavariant.Value.Array.Length = list.size();
        // Use calloc() instead of new because the OPC UA stack uses free() internally when clearing the data
        TARGETTYPE *arr = static_cast<TARGETTYPE *>(calloc(list.size(), sizeof(TARGETTYPE)));
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
        opcuavariant.ArrayType = OpcUa_True;
        opcuavariant.Value.Array.Length = list.size();
        // Use calloc() instead of new because the OPC UA stack uses free() internally when clearing the data
        TARGETTYPE *arr = static_cast<TARGETTYPE *>(calloc(list.size(), sizeof(TARGETTYPE)));

        opcuavariant.Value.Array.Value.Array = arr;

        for (int i = 0; i < list.size(); ++i)
            scalarFromQVariant<TARGETTYPE, QTTYPE>(list[i], &arr[i]);

        return opcuavariant;
    }

    // Taking one pointer for all as it is union
    TARGETTYPE **temp = reinterpret_cast<TARGETTYPE **>(&opcuavariant.Value.Guid);
    // We have to allocate, otherwise copyTo() will not do any action
    // Use calloc() instead of new because the OPC UA stack uses free() internally when clearing the data
    *temp = static_cast<TARGETTYPE *>(calloc(1, sizeof(TARGETTYPE)));
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
OpcUa_Variant arrayFromQVariant<OpcUa_QualifiedName, QOpcUa::QQualifiedName>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_QualifiedName, QOpcUa::QQualifiedName>(var, type);
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
OpcUa_Variant arrayFromQVariant<OpcUa_ExtensionObject, QOpcUa::QRange>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_ExtensionObject, QOpcUa::QRange>(var, type);
}

template<>
OpcUa_Variant arrayFromQVariant<OpcUa_ExtensionObject, QOpcUa::QEUInformation>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_ExtensionObject, QOpcUa::QEUInformation>(var, type);
}

template<>
OpcUa_Variant arrayFromQVariant<OpcUa_ExtensionObject, QOpcUa::QComplexNumber>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_ExtensionObject, QOpcUa::QComplexNumber>(var, type);
}

template<>
OpcUa_Variant arrayFromQVariant<OpcUa_ExtensionObject, QOpcUa::QDoubleComplexNumber>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_ExtensionObject, QOpcUa::QDoubleComplexNumber>(var, type);
}

template<>
OpcUa_Variant arrayFromQVariant<OpcUa_ExtensionObject, QOpcUa::QAxisInformation>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_ExtensionObject, QOpcUa::QAxisInformation>(var, type);
}

template<>
OpcUa_Variant arrayFromQVariant<OpcUa_ExtensionObject, QOpcUa::QXValue>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_ExtensionObject, QOpcUa::QXValue>(var, type);
}

template<>
OpcUa_Variant arrayFromQVariant<OpcUa_ExpandedNodeId, QOpcUa::QExpandedNodeId>(const QVariant &var, const OpcUa_BuiltInType type)
{
    return arrayFromQVariantPointer<OpcUa_ExpandedNodeId, QOpcUa::QExpandedNodeId>(var, type);
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
        return arrayToQVariant<QOpcUa::QQualifiedName, OpcUa_QualifiedName *>(value);
    case OpcUa_BuiltInType::OpcUaType_StatusCode:
        return arrayToQVariant<QOpcUa::UaStatusCode, OpcUa_StatusCode>(value, QMetaType::UInt);
    case OpcUa_BuiltInType::OpcUaType_ExtensionObject:
        return arrayToQVariant<QVariant, OpcUa_ExtensionObject *>(value);
    case OpcUa_BuiltInType::OpcUaType_ExpandedNodeId:
        return arrayToQVariant<QOpcUa::QExpandedNodeId, OpcUa_ExpandedNodeId *>(value);
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

    if (value.type() == QVariant::List && value.toList().size() == 0)
        return uacppvalue;

    QVariant temp = (value.type() == QVariant::List) ? value.toList().at(0) : value;
    QOpcUa::Types valueType = type == QOpcUa::Undefined ?
                qvariantTypeToQOpcUaType(static_cast<QMetaType::Type>(temp.type())) : type;

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
        return arrayFromQVariant<OpcUa_QualifiedName, QOpcUa::QQualifiedName>(value, dt);
    case QOpcUa::StatusCode:
        return arrayFromQVariant<OpcUa_StatusCode, QOpcUa::UaStatusCode>(value, dt);
    case QOpcUa::Range:
        return arrayFromQVariant<OpcUa_ExtensionObject, QOpcUa::QRange>(value, dt);
    case QOpcUa::EUInformation:
        return arrayFromQVariant<OpcUa_ExtensionObject, QOpcUa::QEUInformation>(value, dt);
    case QOpcUa::ComplexNumber:
        return arrayFromQVariant<OpcUa_ExtensionObject, QOpcUa::QComplexNumber>(value, dt);
    case QOpcUa::DoubleComplexNumber:
        return arrayFromQVariant<OpcUa_ExtensionObject, QOpcUa::QDoubleComplexNumber>(value, dt);
    case QOpcUa::AxisInformation:
        return arrayFromQVariant<OpcUa_ExtensionObject, QOpcUa::QAxisInformation>(value, dt);
    case QOpcUa::XV:
        return arrayFromQVariant<OpcUa_ExtensionObject, QOpcUa::QXValue>(value, dt);
    case QOpcUa::ExpandedNodeId:
        return arrayFromQVariant<OpcUa_ExpandedNodeId, QOpcUa::QExpandedNodeId>(value, dt);
    default:
        qCWarning(QT_OPCUA_PLUGINS_UACPP) << "Variant conversion to UACpp for typeIndex" << type << " not implemented";
    }

    return uacppvalue;
}

QDateTime toQDateTime(const OpcUa_DateTime *dt)
{
    // OPC-UA part 3, Table C.9
    const QDateTime uaEpochStart(QDate(1601, 1, 1), QTime(0, 0), Qt::UTC);
    const UaDateTime temp(*dt);
    return uaEpochStart.addMSecs(temp).toLocalTime();
}

}

QT_END_NAMESPACE

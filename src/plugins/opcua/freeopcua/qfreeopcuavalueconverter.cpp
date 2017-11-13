/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
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

#include "qfreeopcuavalueconverter.h"
#include <QDateTime>
#include <QLoggingCategory>
#include <QUuid>
#include <vector>

#include <opc/ua/protocol/string_utils.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_FREEOPCUA)

namespace QFreeOpcUaValueConverter {

QVariant toQVariant(const OpcUa::Variant &variant)
{
    // Null variant, return empty QVariant
    if (!variant.IsScalar() && !variant.IsArray()) {
        return QVariant();
    }

    switch (variant.Type()) {
    case OpcUa::VariantType::NUL:
        return QVariant::fromValue(static_cast<QObject *>(0));

    case OpcUa::VariantType::BOOLEAN:
        return arrayToQVariant<bool, bool>(variant);

    case OpcUa::VariantType::SBYTE:
        return arrayToQVariant<qint8, qint8>(variant, QMetaType::SChar);

    case OpcUa::VariantType::BYTE:
        return arrayToQVariant<quint8, quint8>(variant, QMetaType::UChar);

    case OpcUa::VariantType::INT16:
        return arrayToQVariant<qint16, qint16>(variant, QMetaType::Short);

    case OpcUa::VariantType::UINT16:
        return arrayToQVariant<quint16, quint16>(variant, QMetaType::UShort);

    case OpcUa::VariantType::INT32:
        return arrayToQVariant<qint32, qint32>(variant, QMetaType::Int);

    case OpcUa::VariantType::UINT32:
        return arrayToQVariant<quint32, quint32>(variant, QMetaType::UInt);

    case OpcUa::VariantType::INT64:
        return arrayToQVariant<int64_t, int64_t>(variant, QMetaType::LongLong);

    case OpcUa::VariantType::UINT64:
        return arrayToQVariant<uint64_t, uint64_t>(variant, QMetaType::ULongLong);

    case OpcUa::VariantType::FLOAT:
        return arrayToQVariant<float, float>(variant, QMetaType::Float);

    case OpcUa::VariantType::DOUBLE:
        return arrayToQVariant<double, double>(variant, QMetaType::Double);

    case OpcUa::VariantType::STRING:
        return arrayToQVariant<QString, std::string>(variant, QMetaType::QString);

    case OpcUa::VariantType::DATE_TIME:
        return arrayToQVariant<QDateTime, OpcUa::DateTime>(variant, QMetaType::QDateTime);

    case OpcUa::VariantType::BYTE_STRING:
        return arrayToQVariant<QByteArray, OpcUa::ByteString>(variant, QMetaType::QByteArray);

    case OpcUa::VariantType::LOCALIZED_TEXT:
        return arrayToQVariant<QString, OpcUa::LocalizedText>(variant, QMetaType::QString);

    case OpcUa::VariantType::NODE_Id:
        return arrayToQVariant<QString, OpcUa::NodeId>(variant, QMetaType::QString);

    case OpcUa::VariantType::XML_ELEMENT:
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Type XMLElement is not yet supported in FreeOPCUA");
        return QVariant();

    case OpcUa::VariantType::GUId:
        return arrayToQVariant<QUuid, OpcUa::Guid>(variant, QMetaType::QUuid);

    default:
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Variant type is not yet supported: %d", static_cast<int>(variant.Type()));
        return QVariant();
    }
}

OpcUa::Variant toTypedVariant(const QVariant &variant, QOpcUa::Types type)
{
    switch (type) {
    case QOpcUa::Boolean:
        return arrayFromQVariant<bool>(variant);

    case QOpcUa::Byte:
        return arrayFromQVariant<uint8_t>(variant);

    case QOpcUa::DateTime:
        return arrayFromQVariant<OpcUa::DateTime>(variant);
    case QOpcUa::Double:
        return arrayFromQVariant<double>(variant);

    case QOpcUa::Float:
        return arrayFromQVariant<float>(variant);

    case QOpcUa::Int16:
        return arrayFromQVariant<int16_t>(variant);

    case QOpcUa::Int32:
        return arrayFromQVariant<int32_t>(variant);

    case QOpcUa::Int64:
        return arrayFromQVariant<int64_t>(variant);

    case QOpcUa::SByte:
        return arrayFromQVariant<int8_t>(variant);

    case QOpcUa::String:
        return arrayFromQVariant<std::string>(variant);

    case QOpcUa::UInt16:
        return arrayFromQVariant<uint16_t>(variant);

    case QOpcUa::UInt32:
        return arrayFromQVariant<uint32_t>(variant);

    case QOpcUa::UInt64:
        return arrayFromQVariant<uint64_t>(variant);

    case QOpcUa::LocalizedText:
        return arrayFromQVariant<OpcUa::LocalizedText>(variant);

    case QOpcUa::ByteString:
        return arrayFromQVariant<OpcUa::ByteString>(variant);

    case QOpcUa::NodeId:
        return arrayFromQVariant<OpcUa::NodeId>(variant);

    case QOpcUa::XmlElement:
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Type XMLElement is not yet supported in FreeOPCUA");
        return OpcUa::Variant();

    case QOpcUa::Guid:
        return arrayFromQVariant<OpcUa::Guid>(variant);

    default:
        return toVariant(variant);
    }
}

QString nodeIdToString(const OpcUa::NodeId &id)
{
    QString nodeId = QStringLiteral("ns=%1;").arg(id.GetNamespaceIndex());
    if (id.IsInteger()) {
        nodeId += QStringLiteral("i=%1").arg(id.GetIntegerIdentifier());
    } else if (id.IsString()) {
        nodeId += QStringLiteral("s=%1").arg(id.GetStringIdentifier().c_str());
    } else if (id.IsGuid()) {
        const OpcUa::Guid tempId = id.GetGuidIdentifier();
        const QUuid uuid(tempId.Data1, tempId.Data2, tempId.Data3, tempId.Data4[0], tempId.Data4[1], tempId.Data4[2],
                tempId.Data4[3], tempId.Data4[4], tempId.Data4[5], tempId.Data4[6], tempId.Data4[7]);
        nodeId += QStringLiteral("g=").append(uuid.toString().midRef(1, 36)); // Remove enclosing {...}
    } else if (id.IsBinary()) {
        const QString base64String = QByteArray(reinterpret_cast<const char *>(id.GetBinaryIdentifier().data()),
                                          id.GetBinaryIdentifier().size()).toBase64();
        nodeId += QStringLiteral("b=").append(base64String);
    } else {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Unknown nodeId type!");
        nodeId = QString();
    }
    return nodeId;
}

OpcUa::Variant toVariant(const QVariant &variant)
{
    switch (variant.type()) {
    case QMetaType::Bool:
        return arrayFromQVariant<bool>(variant);
    case QMetaType::Int:
        return arrayFromQVariant<qint32>(variant);
    case QMetaType::UInt:
        return arrayFromQVariant<quint32>(variant);
    case QMetaType::Double:
        return arrayFromQVariant<double>(variant);
    case QMetaType::QString:
        return arrayFromQVariant<std::string>(variant);
    default:
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Variant type is not yet supported: %d", variant.type());
        return OpcUa::Variant();
    }
}

template<typename UATYPE, typename QTTYPE=UATYPE>
OpcUa::Variant arrayFromQVariant(const QVariant &var)
{
    if (var.type() == QVariant::List) {
        QVariantList list = var.toList();
        std::vector<UATYPE> temp;
        for (int i = 0; i < list.size(); ++i)
            temp.push_back(scalarFromQVariant<UATYPE, QTTYPE>(list[i]));
        return OpcUa::Variant(temp);
    }
    return OpcUa::Variant(scalarFromQVariant<UATYPE, QTTYPE>(var));
}

template<typename UATYPE, typename QTTYPE=UATYPE>
UATYPE scalarFromQVariant(const QVariant &var)
{
    return static_cast<UATYPE>(var.value<QTTYPE>());
}

template<>
std::string scalarFromQVariant(const QVariant &var)
{
    return var.toString().toStdString();
}

template<>
OpcUa::DateTime scalarFromQVariant(const QVariant &var)
{
    return OpcUa::DateTime::FromTimeT(var.value<QDateTime>().toTime_t());
}

template<>
OpcUa::ByteString scalarFromQVariant(const QVariant &var)
{
    const QByteArray arr = var.toByteArray();
    const char *start = arr.data();
    const char *end = start + arr.length();
    std::vector<uint8_t> temp(start, end);
    return OpcUa::ByteString(temp);
}

template<>
OpcUa::LocalizedText scalarFromQVariant(const QVariant &var)
{
    return OpcUa::LocalizedText(var.toString().toStdString(), std::string("en"));
}

template<>
OpcUa::NodeId scalarFromQVariant(const QVariant &var)
{
    try {
        return OpcUa::ToNodeId(var.toString().toStdString());
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "Failed to parse node id:" << ex.what();
        return OpcUa::NodeId();
    }
}

template<>
OpcUa::Guid scalarFromQVariant(const QVariant &var)
{
    OpcUa::Guid temp;
    const QUuid uuid = var.toUuid();
    temp.Data1 = uuid.data1;
    temp.Data2 = uuid.data2;
    temp.Data3 = uuid.data3;
    std::memcpy(temp.Data4, uuid.data4, sizeof(uuid.data4));
    return temp;
}

template<typename QTTYPE, typename UATYPE>
QVariant arrayToQVariant(const OpcUa::Variant &var, QMetaType::Type type)
{
    if (var.IsArray()) {
        QVariantList list;
        std::vector<UATYPE> temp = var.As<std::vector<UATYPE>>();

        for (unsigned int i = 0; i < temp.size(); ++i) {
            QTTYPE data = scalarUaToQt<QTTYPE, UATYPE>(temp[i]);
            if (type == QMetaType::UnknownType)
                list.append(QVariant::fromValue(data));
            else
                list.append(QVariant(type, &data));
        }
        return list;

    } else if (var.IsScalar()) {
        QTTYPE data = scalarUaToQt<QTTYPE, UATYPE>(var.As<UATYPE>());
        if (type == QMetaType::UnknownType)
            return QVariant::fromValue(data);
        return QVariant(type, &data);
    }
    return QVariant();
}

template<typename QTTYPE, typename UATYPE>
QTTYPE scalarUaToQt(const UATYPE &data)
{
    return static_cast<QTTYPE>(data);
}

template<>
QString scalarUaToQt<QString, std::string>(const std::string &data)
{
    return QString::fromStdString(data);
}

template<>
QDateTime scalarUaToQt<QDateTime, OpcUa::DateTime>(const OpcUa::DateTime &data)
{
    return QDateTime::fromTime_t( OpcUa::DateTime::ToTimeT(data));
}

template<>
QByteArray scalarUaToQt<QByteArray, OpcUa::ByteString>(const OpcUa::ByteString &data)
{
    return QByteArray(reinterpret_cast<const char *>(data.Data.data()), data.Data.size());
}

template<>
QString scalarUaToQt<QString, OpcUa::LocalizedText>(const OpcUa::LocalizedText &data)
{
    return QString::fromStdString(data.Text);
}

template<>
QString scalarUaToQt<QString, OpcUa::NodeId>(const OpcUa::NodeId &data)
{
    return nodeIdToString(data);
}

template<>
QUuid scalarUaToQt<QUuid, OpcUa::Guid>(const OpcUa::Guid &data)
{
    return QUuid(data.Data1, data.Data2, data.Data3, data.Data4[0],
            data.Data4[1], data.Data4[2], data.Data4[3], data.Data4[4],
            data.Data4[5], data.Data4[6], data.Data4[7]);
}

}

QT_END_NAMESPACE

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
#include <vector>

#include <opc/ua/protocol/string_utils.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_FREEOPCUA)

namespace QFreeOpcUaValueConverter {

QVariant toQVariant(const OpcUa::Variant &variant)
{
    if (!variant.IsScalar() && !variant.IsArray()) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Matrix values are not yet supported by FreeOPCUA");
        return QVariant();
    }

    OpcUa::ByteString bs;
    switch (variant.Type()) {
    case OpcUa::VariantType::NUL:
        return QVariant::fromValue(static_cast<QObject *>(0));

    case OpcUa::VariantType::BOOLEAN:
        if (variant.IsScalar())
            return QVariant(variant.As<bool>());
        return getArray<bool>(variant.As<std::vector<bool>>());

    case OpcUa::VariantType::SBYTE:
        if (variant.IsScalar()) {
            qint8 value = variant.As<qint8>();
            return QVariant(QMetaType::SChar, &value);
        }
        return getArray<qint8>(variant.As<std::vector<qint8>>(), QMetaType::SChar);

    case OpcUa::VariantType::BYTE:
        if (variant.IsScalar()) {
            quint8 value = variant.As<quint8>();
            return QVariant(QMetaType::UChar, &value);
        }
        return getArray<quint8>(variant.As<std::vector<quint8>>(), QMetaType::UChar);

    case OpcUa::VariantType::INT16:
        if (variant.IsScalar()) {
            QVariant var(QMetaType::Int, 0);
            var.setValue(variant.As<qint16>());
            return var;
        }
        return getArray<qint16>(variant.As<std::vector<qint16>>(), QMetaType::Int);

    case OpcUa::VariantType::UINT16:
        if (variant.IsScalar()) {
            QVariant var(QMetaType::UInt, 0);
            var.setValue(variant.As<quint16>());
            return var;
        }
        return getArray<quint16>(variant.As<std::vector<quint16>>(), QMetaType::UInt);

    case OpcUa::VariantType::INT32:
        if (variant.IsScalar())
            return QVariant(variant.As<int32_t>());
        return getArray<qint32>(variant.As<std::vector<int32_t>>());

    case OpcUa::VariantType::UINT32:
        if (variant.IsScalar())
            return QVariant(variant.As<uint32_t>());
        return getArray<quint32>(variant.As<std::vector<uint32_t>>());

    case OpcUa::VariantType::INT64: {
        if (variant.IsScalar())
            return QVariant(static_cast<qint64>(variant.As<int64_t>()));
        std::vector<int64_t> temp = variant.As<std::vector<int64_t>>();
        QVariantList ret;
        for (auto it : temp)
            ret.append(QVariant(static_cast<qint64>(it)));
        return ret;
    }

    case OpcUa::VariantType::UINT64: {
        if (variant.IsScalar())
            return QVariant(static_cast<quint64>(variant.As<uint64_t>()));
        std::vector<uint64_t> temp = variant.As<std::vector<uint64_t>>();
        QVariantList ret;
        for (auto it : temp)
            ret.append(QVariant(static_cast<quint64>(it)));
        return ret;
    }
    case OpcUa::VariantType::FLOAT:
        if (variant.IsScalar())
            return QVariant(variant.As<float>());
        return getArray<float>(variant.As<std::vector<float>>());

    case OpcUa::VariantType::DOUBLE:
        if (variant.IsScalar())
            return QVariant(variant.As<double>());
        return getArray<double>(variant.As<std::vector<double>>());

    case OpcUa::VariantType::STRING:
        if (variant.IsScalar()) {
            return QVariant(QString::fromStdString(variant.As<std::string>()));
        } else {
            std::vector<std::string> temp = variant.As<std::vector<std::string>>();
            QVariantList ret;
            for (uint i = 0; i < temp.size(); i++) {
                ret.append(QString::fromStdString(temp[i]));
            }
            return ret;
        }

    case OpcUa::VariantType::DATE_TIME:
        if (variant.IsScalar()) {
            return QVariant(QDateTime::fromTime_t( OpcUa::DateTime::ToTimeT(variant.As<OpcUa::DateTime>()) ));
        } else {
            std::vector<OpcUa::DateTime> temp = variant.As<std::vector<OpcUa::DateTime>>();
            QVariantList ret;
            for (uint i = 0; i < temp.size(); i++) {
                ret.append(QDateTime::fromTime_t(OpcUa::DateTime::ToTimeT(temp[i])));
            }
            return ret;
        }

    case OpcUa::VariantType::BYTE_STRING:
        if (variant.IsScalar()) {
            bs = variant.As<OpcUa::ByteString>();
            return QVariant(QByteArray((char *) bs.Data.data(), bs.Data.size()));
        } else {
            std::vector<OpcUa::ByteString> temp = variant.As<std::vector<OpcUa::ByteString>>();
            QVariantList ret;
            for (uint i = 0; i < temp.size(); i++) {
                ret.append(QByteArray((char *) temp[i].Data.data(), temp[i].Data.size()));
            }
            return ret;
        }

    case OpcUa::VariantType::LOCALIZED_TEXT:
        if (variant.IsScalar()) {
            return QVariant(QString::fromStdString(variant.As<OpcUa::LocalizedText>().Text));
        } else {
            std::vector<OpcUa::LocalizedText> temp = variant.As<std::vector<OpcUa::LocalizedText>>();
            QVariantList ret;
            for (uint i = 0; i < temp.size(); i++) {
                ret.append(QString::fromStdString(temp[i].Text));
            }
            return ret;
        }

    case OpcUa::VariantType::NODE_Id:
        if (variant.IsScalar()) {
            return QFreeOpcUaValueConverter::nodeIdToString(variant.As<OpcUa::NodeId>());
        } else {
            std::vector<OpcUa::NodeId> temp = variant.As<std::vector<OpcUa::NodeId>>();
            QVariantList ret;
            for (uint i = 0; i < temp.size(); i++) {
                ret.append(QFreeOpcUaValueConverter::nodeIdToString(temp[i]));
            }
            return ret;
        }

    case OpcUa::VariantType::XML_ELEMENT:
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Type XMLElement is not yet supported in FreeOPCUA");
        return QVariant();

    default:
        return QVariant();
    }
}

OpcUa::Variant toTypedVariant(const QVariant &variant, QOpcUa::Types type)
{
    OpcUa::DateTime dt;
    std::vector<uint8_t> data;
    OpcUa::Variant var;
    const bool isList = variant.type() == QVariant::Type::List;

    switch (type) {
    case QOpcUa::Boolean:
        if (isList)
            return QFreeOpcUaValueConverter::getArray<bool>(variant);
        return OpcUa::Variant(variant.value<bool>());

    case QOpcUa::Byte:
        if (isList)
            return QFreeOpcUaValueConverter::getArray<uint8_t>(variant);
        return OpcUa::Variant(variant.value<uint8_t>());

    case QOpcUa::DateTime:
        if (isList) {
            QVariantList list = variant.toList();
            std::vector<OpcUa::DateTime> vec;
            for (int i = 0; i < list.size(); i++)
                vec.push_back(OpcUa::DateTime::FromTimeT(list[i].value<QDateTime>().toTime_t()));

            return OpcUa::Variant(vec);
        }
        dt = OpcUa::DateTime::FromTimeT(variant.value<QDateTime>().toTime_t());
        return OpcUa::Variant(dt);
    case QOpcUa::Double:
        if (isList)
            return QFreeOpcUaValueConverter::getArray<double>(variant);
        return OpcUa::Variant(variant.value<double>());

    case QOpcUa::Float:
        if (isList)
            return QFreeOpcUaValueConverter::getArray<float>(variant);
        return OpcUa::Variant(variant.value<float>());

    case QOpcUa::Int16:
        if (isList)
            return QFreeOpcUaValueConverter::getArray<int16_t>(variant);
        return OpcUa::Variant(variant.value<int16_t>());

    case QOpcUa::Int32:
        if (isList)
            return QFreeOpcUaValueConverter::getArray<int32_t>(variant);
        return OpcUa::Variant(variant.value<int32_t>());

    case QOpcUa::Int64:
        if (isList)
            return QFreeOpcUaValueConverter::getArray<int64_t>(variant);
        return OpcUa::Variant(variant.value<int64_t>());

    case QOpcUa::SByte:
        if (isList)
            return QFreeOpcUaValueConverter::getArray<int8_t>(variant);
        return OpcUa::Variant(variant.value<int8_t>());

    case QOpcUa::String:
        if (isList) {
            QVariantList list = variant.toList();
            std::vector<std::string> vec;
            for (int i = 0; i < list.size(); i++)
                vec.push_back(list[i].value<QString>().toStdString());

            return OpcUa::Variant(vec);
        }
        return OpcUa::Variant(variant.toString().toStdString());

    case QOpcUa::UInt16:
        if (isList)
            return QFreeOpcUaValueConverter::getArray<uint16_t>(variant);
        return OpcUa::Variant(variant.value<uint16_t>());

    case QOpcUa::UInt32:
        if (isList)
            return QFreeOpcUaValueConverter::getArray<uint32_t>(variant);
        return OpcUa::Variant(variant.value<uint32_t>());

    case QOpcUa::UInt64:
        if (isList)
            return QFreeOpcUaValueConverter::getArray<uint64_t>(variant);
        return OpcUa::Variant(variant.value<uint64_t>());

    case QOpcUa::LocalizedText:
        if (isList) {
            QVariantList list = variant.toList();
            std::vector<OpcUa::LocalizedText> vec;
            for (int i = 0; i < list.size(); i++)
                vec.push_back(OpcUa::LocalizedText(list[i].toString().toStdString(), std::string("en")));

            return OpcUa::Variant(vec);
        }
        return OpcUa::Variant(OpcUa::LocalizedText(variant.toString().toStdString(), std::string("en")));

    case QOpcUa::ByteString: {
        char *bufferStart;
        char *bufferEnd;
        if (isList) {
            QVariantList list = variant.toList();
            std::vector<OpcUa::ByteString> vec;
            for (int i = 0; i < list.size(); i++) {
                QByteArray rawData = list[i].toByteArray();

                bufferStart = rawData.data();
                bufferEnd = bufferStart + rawData.length();
                data = std::vector<uint8_t>(bufferStart, bufferEnd);
                vec.push_back(OpcUa::ByteString(data));
            }
            var = OpcUa::Variant(vec);
            return var;
        }
        if (variant.type() == QVariant::Type::ByteArray) {
            QByteArray rawData = variant.toByteArray();

            bufferStart = rawData.data();
            bufferEnd = bufferStart + rawData.length();
            data = std::vector<uint8_t>(bufferStart, bufferEnd);
            return OpcUa::Variant(OpcUa::ByteString(data));
        }
        return OpcUa::Variant();
    }

    case QOpcUa::NodeId: {
        try {
            if (isList) {
                std::vector<OpcUa::NodeId> vec;
                QVariantList list = variant.toList();
                for (int i=0; i<list.size(); i++) {
                    vec.push_back(OpcUa::ToNodeId(list[i].toString().toStdString()));
                }
                return OpcUa::Variant(vec);
            } else {
                return OpcUa::Variant(OpcUa::ToNodeId(variant.toString().toStdString()));
            }
        } catch (const std::exception &ex) {
            qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Failed to parse node id: %s", ex.what());
            return OpcUa::Variant();
        }
    }

    case QOpcUa::XmlElement:
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Type XMLElement is not yet supported in FreeOPCUA");
        return OpcUa::Variant();

    default:
        return toVariant(variant);
    }
}

QString nodeIdToString(const OpcUa::NodeId &id)
{
    QString nodeId = QString("ns=%1;").arg(id.GetNamespaceIndex());
    if (id.IsInteger()) {
        nodeId += QString("i=%1").arg(id.GetIntegerIdentifier());
    } else if (id.IsString()) {
        nodeId += QString("s=%1").arg(id.GetStringIdentifier().c_str());
    } else if (id.IsGuid()) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Guid nodeIds are not supported");
        nodeId = QString();
    } else if (id.IsBinary()) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Opaque nodeIds are not supported");
        nodeId = QString();
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
        return OpcUa::Variant(variant.value<bool>());
    case QMetaType::Int:
        return OpcUa::Variant(variant.value<qint32>());
    case QMetaType::UInt:
        return OpcUa::Variant(variant.value<quint32>());
    case QMetaType::Double:
        return OpcUa::Variant(variant.value<double>());
    case QMetaType::QString:
        return OpcUa::Variant(variant.value<QString>().toStdString());
    default:
        return OpcUa::Variant();
    }
}
}

QT_END_NAMESPACE

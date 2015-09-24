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
#include <vector>

QT_BEGIN_NAMESPACE

namespace QFreeOpcUaValueConverter {

QVariant toQVariant(const OpcUa::Variant &variant)
{
    OpcUa::ByteString bs;
    switch (variant.Type()) {
    case OpcUa::VariantType::NUL:
        return QVariant::fromValue(static_cast<QObject *>(0));

    case OpcUa::VariantType::BOOLEAN:
        if (variant.IsScalar())
            return QVariant(variant.As<bool>());
        if (variant.IsArray())
            return getArray<bool>(variant.As<std::vector<bool>>());
        Q_ASSERT(0 && "tbd");

    case OpcUa::VariantType::SBYTE:
        if (variant.IsScalar())
            return QVariant(variant.As<qint8>());
        if (variant.IsArray())
            return getArray<qint8>(variant.As<std::vector<qint8>>());
        Q_ASSERT(0 && "tbd");

    case OpcUa::VariantType::BYTE:
        if (variant.IsScalar())
            return QVariant(variant.As<quint8>());
        if (variant.IsArray())
            return getArray<quint8>(variant.As<std::vector<quint8>>());
        Q_ASSERT(0 && "tbd");

    case OpcUa::VariantType::INT16:
        if (variant.IsScalar())
            return QVariant(variant.As<qint16>());
        if (variant.IsArray())
            return getArray<qint16>(variant.As<std::vector<qint16>>());
        Q_ASSERT(0 && "tbd");

    case OpcUa::VariantType::UINT16:
        if (variant.IsScalar())
            return QVariant(variant.As<quint16>());
        if (variant.IsArray())
            return getArray<quint16>(variant.As<std::vector<quint16>>());
        Q_ASSERT(0 && "tbd");

    case OpcUa::VariantType::INT32:
        if (variant.IsScalar())
            return QVariant(variant.As<int32_t>());
        if (variant.IsArray())
            return getArray<qint32>(variant.As<std::vector<int32_t>>());
        Q_ASSERT(0 && "tbd");

    case OpcUa::VariantType::UINT32:
        if (variant.IsScalar())
            return QVariant(variant.As<uint32_t>());
        if (variant.IsArray())
            return getArray<quint32>(variant.As<std::vector<uint32_t>>());
        Q_ASSERT(0 && "tbd");

    case OpcUa::VariantType::INT64:
        if (variant.IsScalar())
            return QVariant(variant.As<qint64>());
        if (variant.IsArray())
            return getArray<qint64>(variant.As<std::vector<qint64>>());
        Q_ASSERT(0 && "tbd");

    case OpcUa::VariantType::UINT64:
        if (variant.IsScalar())
            return QVariant(variant.As<quint64>());
        if (variant.IsArray())
            return getArray<quint64>(variant.As<std::vector<quint64>>());
        Q_ASSERT(0 && "tbd");

    case OpcUa::VariantType::FLOAT:
        if (variant.IsScalar())
            return QVariant(variant.As<float>());
        if (variant.IsArray())
            return getArray<float>(variant.As<std::vector<float>>());
        Q_ASSERT(0 && "tbd");

    case OpcUa::VariantType::DOUBLE:
        if (variant.IsScalar())
            return QVariant(variant.As<double>());
        if (variant.IsArray())
            return getArray<double>(variant.As<std::vector<double>>());
        Q_ASSERT(0 && "tbd");

    case OpcUa::VariantType::STRING:
        if (variant.IsScalar())
            return QVariant(QString::fromStdString(variant.As<std::string>()));
        if (variant.IsArray()) {
            std::vector<std::string> temp = variant.As<std::vector<std::string>>();
            QVariantList ret;
            for (uint i = 0; i < temp.size(); i++) {
                ret.append(QString::fromStdString(temp[i]));
            }
            return ret;
        }
        Q_ASSERT(0 && "tbd");

    case OpcUa::VariantType::DATE_TIME:
        if (variant.IsScalar())
            return QVariant(QDateTime::fromTime_t( OpcUa::DateTime::ToTimeT(variant.As<OpcUa::DateTime>()) ));
        if (variant.IsArray()) {
            std::vector<OpcUa::DateTime> temp = variant.As<std::vector<OpcUa::DateTime>>();
            QVariantList ret;
            for (uint i = 0; i < temp.size(); i++) {
                ret.append(QDateTime::fromTime_t(OpcUa::DateTime::ToTimeT(temp[i])));
            }
            return ret;
        }
        Q_ASSERT(0 && "tbd");

    case OpcUa::VariantType::BYTE_STRING:
        if (variant.IsScalar()) {
            bs = variant.As<OpcUa::ByteString>();
            return QVariant(QByteArray((char *) bs.Data.data(), bs.Data.size()));
        }
        if (variant.IsArray()) {
            std::vector<OpcUa::ByteString> temp = variant.As<std::vector<OpcUa::ByteString>>();
            QVariantList ret;
            for (uint i = 0; i < temp.size(); i++) {
                ret.append(QByteArray((char *) temp[i].Data.data(), temp[i].Data.size()));
            }
            return ret;
        }
        Q_ASSERT(0 && "tbd");

    case OpcUa::VariantType::LOCALIZED_TEXT:
        if (variant.IsScalar())
            return QVariant(QString::fromStdString(variant.As<OpcUa::LocalizedText>().Text));
        if (variant.IsArray()) {
            std::vector<OpcUa::LocalizedText> temp = variant.As<std::vector<OpcUa::LocalizedText>>();
            QVariantList ret;
            for (uint i = 0; i < temp.size(); i++) {
                ret.append(QString::fromStdString(temp[i].Text));
            }
            return ret;
        }
        Q_ASSERT(0 && "tbd");

    case OpcUa::VariantType::NODE_Id:
        if (variant.IsScalar())
            return QFreeOpcUaValueConverter::nodeIdToString(variant.As<OpcUa::NodeId>());
        if (variant.IsArray()) {
            std::vector<OpcUa::NodeId> temp = variant.As<std::vector<OpcUa::NodeId>>();
            QVariantList ret;
            for (uint i = 0; i < temp.size(); i++) {
                ret.append(QFreeOpcUaValueConverter::nodeIdToString(temp[i]));
            }
            return ret;
        }
        Q_ASSERT(0 && "tbd");

    case OpcUa::VariantType::XML_ELEMENT:
        Q_ASSERT(0 && "Not yet supported in freeopcua");

    default:
        return QVariant();
    }
}

OpcUa::Variant toTypedVariant(const QVariant &variant, QOpcUa::Types type)
{
    OpcUa::DateTime dt;
    std::vector<uint8_t> data;
    OpcUa::Variant var;

    switch (type) {
    case QOpcUa::Boolean:
        if (variant.type() == QVariant::Type::List)
            return QFreeOpcUaValueConverter::getArray<bool>(variant);
        return OpcUa::Variant(variant.value<bool>());

    case QOpcUa::Byte:
        if (variant.type() == QVariant::Type::List)
            return QFreeOpcUaValueConverter::getArray<uint8_t>(variant);
        return OpcUa::Variant(variant.value<uint8_t>());

    case QOpcUa::DateTime:
        if (variant.type() == QVariant::Type::List) {
            QVariantList list = variant.toList();
            std::vector<OpcUa::DateTime> vec;
            for (int i = 0; i < list.size(); i++) {
                vec.push_back(OpcUa::DateTime::FromTimeT(list[i].value<QDateTime>().toTime_t()));
            }
            var = OpcUa::Variant(vec);
            return var;
        }
        dt = OpcUa::DateTime::FromTimeT(variant.value<QDateTime>().toTime_t());
        return OpcUa::Variant(dt);
    case QOpcUa::Double:
        if (variant.type() == QVariant::Type::List)
            return QFreeOpcUaValueConverter::getArray<double>(variant);
        return OpcUa::Variant(variant.value<double>());

    case QOpcUa::Float:
        if (variant.type() == QVariant::Type::List)
            return QFreeOpcUaValueConverter::getArray<float>(variant);
        return OpcUa::Variant(variant.value<float>());

    case QOpcUa::Int16:
        if (variant.type() == QVariant::Type::List)
            return QFreeOpcUaValueConverter::getArray<int16_t>(variant);
        return OpcUa::Variant(variant.value<int16_t>());

    case QOpcUa::Int32:
        if (variant.type() == QVariant::Type::List)
            return QFreeOpcUaValueConverter::getArray<int32_t>(variant);
        return OpcUa::Variant(variant.value<int32_t>());

    case QOpcUa::Int64:
        if (variant.type() == QVariant::Type::List)
            return QFreeOpcUaValueConverter::getArray<int64_t>(variant);
        return OpcUa::Variant(variant.value<int64_t>());

    case QOpcUa::SByte:
        if (variant.type() == QVariant::Type::List)
            return QFreeOpcUaValueConverter::getArray<int8_t>(variant);
        return OpcUa::Variant(variant.value<int8_t>());

    case QOpcUa::String:
        if (variant.type() == QVariant::Type::List) {
            QVariantList list = variant.toList();
            std::vector<std::string> vec;
            for (int i = 0; i < list.size(); i++) {
                vec.push_back(list[i].value<QString>().toStdString());
            }
            var = OpcUa::Variant(vec);
            return var;
        }
        return OpcUa::Variant(variant.toString().toStdString());

    case QOpcUa::UInt16:
        if (variant.type() == QVariant::Type::List)
            return QFreeOpcUaValueConverter::getArray<uint16_t>(variant);
        return OpcUa::Variant(variant.value<uint16_t>());

    case QOpcUa::UInt32:
        if (variant.type() == QVariant::Type::List)
            return QFreeOpcUaValueConverter::getArray<uint32_t>(variant);
        return OpcUa::Variant(variant.value<uint32_t>());

    case QOpcUa::UInt64:
        if (variant.type() == QVariant::Type::List)
            return QFreeOpcUaValueConverter::getArray<uint64_t>(variant);
        return OpcUa::Variant(variant.value<uint64_t>());

    case QOpcUa::LocalizedText:
        if (variant.type() == QVariant::Type::List) {
            QVariantList list = variant.toList();
            std::vector<OpcUa::LocalizedText> vec;
            for (int i = 0; i < list.size(); i++) {
                vec.push_back(OpcUa::LocalizedText(list[i].toString().toStdString(), std::string("en")));
            }
            var = OpcUa::Variant(vec);
            return var;
        }
        return OpcUa::Variant(OpcUa::LocalizedText(variant.toString().toStdString(), std::string("en")));

    case QOpcUa::ByteString: {
        char *bufferStart;
        char *bufferEnd;
        if (variant.type() == QVariant::Type::List) {
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
    case QOpcUa::NodeId:
        Q_ASSERT(0 && "There is no parse function in freeopcua!");
    case QOpcUa::XmlElement:
        Q_ASSERT(0 && "Not yet supported in freeopcua");
    default:
        return toVariant(variant);
    }
}

QString nodeIdToString(const OpcUa::NodeId &id)
{
    QString xmlNodeId = QString("ns=%1;").arg(id.GetNamespaceIndex());
    if (id.IsInteger()) {
        xmlNodeId += QString("i=%1").arg(id.GetIntegerIdentifier());
    } else if (id.IsString()) {
        xmlNodeId += QString("s=%1").arg(id.GetStringIdentifier().c_str());
    }
    return xmlNodeId;
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

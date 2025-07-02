// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "quacpputils.h"

#include <QtOpcUa/qopcuatype.h>

#include <QtCore/QLoggingCategory>
#include <QtCore/QString>
#include <QtCore/QUuid>

#include <uaguid.h>
#include <cstring>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_UACPP)

namespace UACppUtils {

UaNodeId nodeIdFromQString(const QString &name)
{
    quint16 index = 0;
    char identifierType = 0;
    QString identifierString;

    bool success = QOpcUa::nodeIdStringSplit(name, &index, &identifierString, &identifierType);
    if (!success) {
        qCWarning(QT_OPCUA_PLUGINS_UACPP, "Unable to split node id string: %s", qUtf8Printable(name));
        return UaNodeId();
    }

    switch (identifierType) {
    case 'i': {
        bool isNumber;
        OpcUa_UInt32 identifier = static_cast<OpcUa_UInt32>(identifierString.toUInt(&isNumber));
        if (isNumber)
            return UaNodeId(identifier, index);
        else
            qCWarning(QT_OPCUA_PLUGINS_UACPP, "%s does not contain a valid numeric identifier", qUtf8Printable(name));
        break;
    }
    case 's': {
        if (identifierString.length() > 0)
            return UaNodeId(UaString(identifierString.toUtf8().constData()), index);
        else
            qCWarning(QT_OPCUA_PLUGINS_UACPP, "%s does not contain a valid string identifier", qUtf8Printable(name));
        break;
    }
    case 'g': {
        QUuid uuid(identifierString);

        if (uuid.isNull()) {
            qCWarning(QT_OPCUA_PLUGINS_UACPP, "%s does not contain a valid guid identifier", qUtf8Printable(name));
        }

        OpcUa_Guid guid;
        guid.Data1 = uuid.data1;
        guid.Data2 = uuid.data2;
        guid.Data3 = uuid.data3;
        std::memcpy(guid.Data4, uuid.data4, sizeof(uuid.data4));
        return UaNodeId(guid, index);
    }
    case 'b': {
        QByteArray temp = QByteArray::fromBase64(identifierString.toLatin1());
        UaByteString bstr((OpcUa_Int32)temp.size(), reinterpret_cast<OpcUa_Byte *>(temp.data()));
        if (temp.size() > 0) {
            return UaNodeId(bstr, index);
        }
        else
            qCWarning(QT_OPCUA_PLUGINS_UACPP, "%s does not contain a valid byte string identifier", qUtf8Printable(name));
        break;
    }
    default:
        qCWarning(QT_OPCUA_PLUGINS_UACPP, "Could not parse node id: %s", qUtf8Printable(name));
    }
    return UaNodeId();
}

// We only need this for template<> QVariant scalarToQVariant<QString, OpcUa_NodeId>(OpcUa_NodeId *data, QMetaType::Type type)
// And also only because our unit tests assume that ns=0 has to be included in the string. Even though ns=0
// can be assumed implicitly
QString nodeIdToQString(const UaNodeId &id)
{
    QString result = QString::fromLatin1("ns=%1;").arg(id.namespaceIndex());

    switch (id.identifierType()) {
    case OpcUa_IdentifierType_Numeric:
        result.append(QString::fromLatin1("i=%1").arg(id.identifierNumeric()));
        break;
    case OpcUa_IdentifierType_String: {
        result.append(QLatin1String("s="));
        const UaString identifier(id.identifierString());
        result.append(QString::fromUtf8(identifier.toUtf8(), identifier.size()));
        break;
    }
    case OpcUa_IdentifierType_Guid: {
        OpcUa_Guid *uaguid = (*id).Identifier.Guid;
        const QUuid uuid(uaguid->Data1, uaguid->Data2, uaguid->Data3,
                         uaguid->Data4[0], uaguid->Data4[1], uaguid->Data4[2], uaguid->Data4[3],
                         uaguid->Data4[4], uaguid->Data4[5], uaguid->Data4[6], uaguid->Data4[7]);
        result.append(QStringLiteral("g=")).append(QStringView(uuid.toString()).mid(1, 36));
        break;
    }
    case OpcUa_IdentifierType_Opaque: {
        const OpcUa_ByteString &uabs = (*id).Identifier.ByteString;
        const QByteArray temp(reinterpret_cast<char *>(uabs.Data), uabs.Length);
        result.append(QStringLiteral("b=")).append(temp.toBase64());
        break;
    }
    default:
        qCWarning(QT_OPCUA_PLUGINS_UACPP, "UACpp Utils: Could not convert UA_NodeId to QString");
        result.clear();
    }
    return result;
}

} // namespace UaCppUtils

QT_END_NAMESPACE

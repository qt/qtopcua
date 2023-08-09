// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopen62541utils.h"
#include <qopcuatype.h>

#include <QtCore/qloggingcategory.h>
#include <QtCore/qstringlist.h>
#include <QtCore/quuid.h>

#ifdef UA_ENABLE_ENCRYPTION
#include <openssl/evp.h>
#include <openssl/rsa.h>
#endif

#include <cstring>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_OPEN62541)

UA_NodeId Open62541Utils::nodeIdFromQString(const QString &name)
{
    quint16 namespaceIndex;
    QString identifierString;
    char identifierType;
    bool success = QOpcUa::nodeIdStringSplit(name, &namespaceIndex, &identifierString, &identifierType);

    if (!success) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Failed to split node id string:" << name;
        return UA_NODEID_NULL;
    }

    switch (identifierType) {
    case 'i': {
        bool isNumber;
        uint identifier = identifierString.toUInt(&isNumber);
        if (isNumber && identifier <= ((std::numeric_limits<quint32>::max)()))
            return UA_NODEID_NUMERIC(namespaceIndex, static_cast<UA_UInt32>(identifier));
        else
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << name << "does not contain a valid numeric identifier";
        break;
    }
    case 's': {
        if (identifierString.size() > 0)
            return UA_NODEID_STRING_ALLOC(namespaceIndex, identifierString.toUtf8().constData());
        else
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << name << "does not contain a valid string identifier";
        break;
    }
    case 'g': {
        QUuid uuid(identifierString);

        if (uuid.isNull()) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << name << "does not contain a valid guid identifier";
            break;
        }

        UA_Guid guid;
        guid.data1 = uuid.data1;
        guid.data2 = uuid.data2;
        guid.data3 = uuid.data3;
        std::memcpy(guid.data4, uuid.data4, sizeof(uuid.data4));
        return UA_NODEID_GUID(namespaceIndex, guid);
    }
    case 'b': {
        const QByteArray temp = QByteArray::fromBase64(identifierString.toLatin1());
        if (temp.size() > 0) {
            return UA_NODEID_BYTESTRING_ALLOC(namespaceIndex, temp.constData());
        }
        else
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << name << "does not contain a valid byte string identifier";
        break;
    }
    default:
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Could not parse node id:" << name;
    }
    return UA_NODEID_NULL;
}

QString Open62541Utils::nodeIdToQString(UA_NodeId id)
{
    QString result = QString::fromLatin1("ns=%1;").arg(id.namespaceIndex);

    switch (id.identifierType) {
    case UA_NODEIDTYPE_NUMERIC:
        result.append(QString::fromLatin1("i=%1").arg(id.identifier.numeric));
        break;
    case UA_NODEIDTYPE_STRING:
        result.append(QLatin1String("s="));
        result.append(QString::fromUtf8(reinterpret_cast<char *>(id.identifier.string.data),
                                             id.identifier.string.length));
        break;
    case UA_NODEIDTYPE_GUID: {
        const UA_Guid &src = id.identifier.guid;
        const QUuid uuid(src.data1, src.data2, src.data3, src.data4[0], src.data4[1], src.data4[2],
                src.data4[3], src.data4[4], src.data4[5], src.data4[6], src.data4[7]);
        result.append(QStringLiteral("g=")).append(QStringView(uuid.toString()).mid(1, 36)); // Remove enclosing {...}
        break;
    }
    case UA_NODEIDTYPE_BYTESTRING: {
        const QByteArray temp(reinterpret_cast<char *>(id.identifier.byteString.data), id.identifier.byteString.length);
        result.append(QStringLiteral("b=")).append(temp.toBase64());
        break;
    }
    default:
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Open62541 Utils: Could not convert UA_NodeId to QString";
        result.clear();
    }
    return result;
}

#ifdef UA_ENABLE_ENCRYPTION
bool Open62541Utils::checkSha1SignatureSupport()
{
    auto mdCtx = EVP_MD_CTX_create ();

    if (!mdCtx)
        return false;

    auto pkCtx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);

    if (!pkCtx) {
        EVP_MD_CTX_destroy(mdCtx);
        return false;
    }

    auto ret = EVP_PKEY_keygen_init(pkCtx);

    if (ret != 1) {
        EVP_PKEY_CTX_free(pkCtx);
        EVP_MD_CTX_destroy(mdCtx);
        return false;
    }

    ret = EVP_PKEY_CTX_set_rsa_keygen_bits(pkCtx, 2048);

    if (ret != 1) {
        EVP_PKEY_CTX_free(pkCtx);
        EVP_MD_CTX_destroy(mdCtx);
        return false;
    }

    EVP_PKEY *pKey = nullptr;
    ret = EVP_PKEY_keygen(pkCtx, &pKey);

    if (ret != 1) {
        EVP_PKEY_CTX_free(pkCtx);
        EVP_MD_CTX_destroy(mdCtx);
        EVP_PKEY_free(pKey);
        return false;
    }

    bool hasSupport = false;

    ret = EVP_DigestSignInit (mdCtx, NULL, EVP_sha1(), NULL, pKey);
    if (ret == 1)
        hasSupport = true;

    EVP_PKEY_CTX_free(pkCtx);
    EVP_MD_CTX_destroy(mdCtx);
    EVP_PKEY_free(pKey);

    return hasSupport;
}
#endif

QT_END_NAMESPACE

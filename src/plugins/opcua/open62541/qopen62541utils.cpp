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

#include "qopen62541utils.h"

#include <QtCore/qloggingcategory.h>
#include <QtCore/qstringlist.h>
#include <QtCore/quuid.h>

#include <cstring>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_OPEN62541)

UA_NodeId Open62541Utils::nodeIdFromQString(const QString &name)
{
    const int semicolonIndex = name.indexOf(';');

    if (semicolonIndex <= 0) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Unable to split node id string: %s", qUtf8Printable(name));
        return UA_NODEID_NULL;
    }

    QStringRef namespaceString = name.leftRef(semicolonIndex);
    if (namespaceString.length() <= 3 || !namespaceString.startsWith(QLatin1String("ns="))) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Not a valid index string in node id string: %s", qUtf8Printable(name));
        return UA_NODEID_NULL;
    }
    namespaceString = namespaceString.mid(3); // Remove "ns="

    QStringRef identifierString = name.midRef(semicolonIndex + 1);

    if (identifierString.length() <= 2) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541,"There is no identifier in node id string: %s", qUtf8Printable(name));
        return UA_NODEID_NULL;
    }

    char identifierType;
    if (identifierString.startsWith(QLatin1String("s=")))
        identifierType = 's';
    else if (identifierString.startsWith(QLatin1String("i=")))
        identifierType = 'i';
    else if (identifierString.startsWith(QLatin1String("g=")))
        identifierType = 'g';
    else if (identifierString.startsWith(QLatin1String("b=")))
        identifierType = 'b';
    else {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "There is no valid identifier type in node id string: %s", qUtf8Printable(name));
        return UA_NODEID_NULL;
    }
    identifierString = identifierString.mid(2); // Remove identifier type

    UA_NodeId uaNodeId;
    UA_NodeId_init(&uaNodeId);

    bool ok = false;
    UA_UInt16 index = static_cast<UA_UInt16>(namespaceString.toUInt(&ok));

    if (!ok) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Not a valid namespace index in node id string: %s", qUtf8Printable(name));
        return UA_NODEID_NULL;
    }

    switch (identifierType) {
    case 'i': {
        bool isNumber;
        UA_UInt32 identifier = static_cast<UA_UInt32>(identifierString.toUInt(&isNumber));
        if (isNumber)
            return UA_NODEID_NUMERIC(index, identifier);
        else
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "%s does not contain a valid numeric identifier", qUtf8Printable(name));
        break;
    }
    case 's': {
        if (identifierString.length() > 0)
            return UA_NODEID_STRING_ALLOC(index, identifierString.toUtf8().constData());
        else
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "%s does not contain a valid string identifier", qUtf8Printable(name));
        break;
    }
    case 'g': {
        QUuid uuid(identifierString.toString());

        if (uuid.isNull()) {
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "%s does not contain a valid guid identifier", qUtf8Printable(name));
        }

        UA_Guid guid;
        guid.data1 = uuid.data1;
        guid.data2 = uuid.data2;
        guid.data3 = uuid.data3;
        std::memcpy(guid.data4, uuid.data4, sizeof(uuid.data4));
        return UA_NODEID_GUID(index, guid);
    }
    case 'b': {
        const QByteArray temp = QByteArray::fromBase64(identifierString.toLocal8Bit());
        if (temp.size() > 0) {
            return UA_NODEID_BYTESTRING_ALLOC(index, temp.data());
        }
        else
            qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "%s does not contain a valid byte string identifier", qUtf8Printable(name));
        break;
    }
    default:
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Could not parse node id: %s", qUtf8Printable(name));
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
        result.append(QString::fromLocal8Bit(reinterpret_cast<char *>(id.identifier.string.data),
                                             id.identifier.string.length));
        break;
    case UA_NODEIDTYPE_GUID: {
        const UA_Guid &src = id.identifier.guid;
        const QUuid uuid(src.data1, src.data2, src.data3, src.data4[0], src.data4[1], src.data4[2],
                src.data4[3], src.data4[4], src.data4[5], src.data4[6], src.data4[7]);
        result.append(QStringLiteral("g=")).append(uuid.toString().midRef(1, 36)); // Remove enclosing {...}
        break;
    }
    case UA_NODEIDTYPE_BYTESTRING: {
        const QByteArray temp(reinterpret_cast<char *>(id.identifier.byteString.data), id.identifier.byteString.length);
        result.append(QStringLiteral("b=")).append(temp.toBase64());
        break;
    }
    default:
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Open62541 Utils: Could not convert UA_NodeId to QString");
        result.clear();
    }
    return result;
}

QT_END_NAMESPACE

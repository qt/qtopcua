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

#include <QtCore/QStringList>

UA_NodeId Open62541Utils::nodeIdFromQString(const QString &name)
{
    // FIXME: proper type recognition
    QStringList splittedNodeId = name.split(";", QString::SkipEmptyParts);
    QString indexString = splittedNodeId.at(0);
    indexString = indexString.replace("ns=", "");
    QString identifierString = splittedNodeId.at(1);
    identifierString = identifierString.replace("s=", "");
    identifierString = identifierString.replace("i=", "");

    bool isNumber = false;
    UA_NodeId uaNodeId;
    UA_NodeId_init(&uaNodeId);

    UA_UInt16 index = (UA_UInt16) indexString.toUInt();
    UA_UInt32 identifier = (UA_UInt32) identifierString.toUInt(&isNumber);

    if (isNumber) {
        uaNodeId = UA_NODEID_NUMERIC((quint16) index, identifier);
    } else {
        // ToDo: Guid
        // TODO: Who cleans up alloc?
        // uaNodeId = UA_NODEID_STRING((quint16) index, identifierString.toUtf8().data());
        uaNodeId = UA_NODEID_STRING_ALLOC((quint16) index, identifierString.toUtf8().data());
    }
    return uaNodeId;
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
    case UA_NODEIDTYPE_GUID:
    case UA_NODEIDTYPE_BYTESTRING:
    default:
        qWarning("Open62541 Utils: Could not convert UA_NodeId to QString");
        result.clear();
    }
    return result;
}

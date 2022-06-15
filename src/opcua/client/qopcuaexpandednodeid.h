// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAEXPANDEDNODEID_H
#define QOPCUAEXPANDEDNODEID_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QOpcUaExpandedNodeIdData;
class Q_OPCUA_EXPORT QOpcUaExpandedNodeId
{
public:
    QOpcUaExpandedNodeId();
    QOpcUaExpandedNodeId(const QOpcUaExpandedNodeId &);
    QOpcUaExpandedNodeId(const QString &nodeId);
    QOpcUaExpandedNodeId(const QString &namespaceUri, const QString &nodeId, quint32 serverIndex = 0);
    QOpcUaExpandedNodeId &operator=(const QOpcUaExpandedNodeId &);
    bool operator==(const QOpcUaExpandedNodeId &) const;
    operator QVariant() const;
    ~QOpcUaExpandedNodeId();

    quint32 serverIndex() const;
    void setServerIndex(quint32 serverIndex);

    QString namespaceUri() const;
    void setNamespaceUri(const QString &namespaceUri);

    QString nodeId() const;
    void setNodeId(const QString &nodeId);

private:
    QSharedDataPointer<QOpcUaExpandedNodeIdData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaExpandedNodeId)

#endif // QOPCUAEXPANDEDNODEID_H

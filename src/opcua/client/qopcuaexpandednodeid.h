/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
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
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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

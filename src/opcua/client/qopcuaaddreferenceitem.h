// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAADDREFERENCEITEM_H
#define QOPCUAADDREFERENCEITEM_H

#include <QtOpcUa/qopcuanodecreationattributes.h>
#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaExpandedNodeId;

class QOpcUaAddReferenceItemData;
class Q_OPCUA_EXPORT QOpcUaAddReferenceItem
{
public:
    QOpcUaAddReferenceItem();
    QOpcUaAddReferenceItem(const QOpcUaAddReferenceItem &other);
    QOpcUaAddReferenceItem &operator=(const QOpcUaAddReferenceItem &rhs);
    ~QOpcUaAddReferenceItem();

    QString sourceNodeId() const;
    void setSourceNodeId(const QString &sourceNodeId);

    QString referenceTypeId() const;
    void setReferenceTypeId(const QString &referenceTypeId);

    bool isForwardReference() const;
    void setIsForwardReference(bool isForwardReference);

    QOpcUaExpandedNodeId targetNodeId() const;
    void setTargetNodeId(const QOpcUaExpandedNodeId &targetNodeId);

    QOpcUa::NodeClass targetNodeClass() const;
    void setTargetNodeClass(QOpcUa::NodeClass targetNodeClass);

    QString targetServerUri() const;
    void setTargetServerUri(const QString &targetServerUri);

private:
    QSharedDataPointer<QOpcUaAddReferenceItemData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaAddReferenceItem)

#endif // QOPCUAADDREFERENCEITEM_H

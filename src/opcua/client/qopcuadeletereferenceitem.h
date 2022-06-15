// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUADELETEREFERENCEITEM_H
#define QOPCUADELETEREFERENCEITEM_H

#include <QtOpcUa/qopcuanodecreationattributes.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaExpandedNodeId;

class QOpcUaDeleteReferenceItemData;
class Q_OPCUA_EXPORT QOpcUaDeleteReferenceItem
{
public:
    QOpcUaDeleteReferenceItem();
    QOpcUaDeleteReferenceItem(const QOpcUaDeleteReferenceItem &other);
    QOpcUaDeleteReferenceItem &operator=(const QOpcUaDeleteReferenceItem &rhs);
    ~QOpcUaDeleteReferenceItem();

    QString sourceNodeId() const;
    void setSourceNodeId(const QString &sourceNodeId);

    QString referenceTypeId() const;
    void setReferenceTypeId(const QString &referenceTypeId);

    bool isForwardReference() const;
    void setIsForwardReference(bool isForwardReference);

    QOpcUaExpandedNodeId targetNodeId() const;
    void setTargetNodeId(const QOpcUaExpandedNodeId &targetNodeId);

    bool deleteBidirectional() const;
    void setDeleteBidirectional(bool deleteBidirectional);

private:
    QSharedDataPointer<QOpcUaDeleteReferenceItemData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaDeleteReferenceItem)

#endif // QOPCUADELETEREFERENCEITEM_H

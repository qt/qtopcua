// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuanodecreationattributes.h"
#include "qopcuadeletereferenceitem.h"
#include "qopcuaexpandednodeid.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaDeleteReferenceItem
    \inmodule QtOpcUa
    \brief This class stores the necessary information to delete a reference from the server.

    \sa QOpcUaClient::deleteReference()
*/

class QOpcUaDeleteReferenceItemData : public QSharedData
{
public:
    QString sourceNodeId;
    QString referenceTypeId;
    bool isForwardReference {true};
    QOpcUaExpandedNodeId targetNodeId;
    bool deleteBidirectional {true};
};

/*!
    Default constructs a delete reference item with no parameters set.
*/
QOpcUaDeleteReferenceItem::QOpcUaDeleteReferenceItem()
    : data(new QOpcUaDeleteReferenceItemData)
{
}

/*!
    Constructs a delete reference item from \a other.
*/
QOpcUaDeleteReferenceItem::QOpcUaDeleteReferenceItem(const QOpcUaDeleteReferenceItem &other)
    : data(other.data)
{
}

/*!
    Sets the values from \a rhs in this delete reference item.
*/
QOpcUaDeleteReferenceItem &QOpcUaDeleteReferenceItem::operator=(const QOpcUaDeleteReferenceItem &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUaDeleteReferenceItem::~QOpcUaDeleteReferenceItem()
{
}

/*!
    Returns the deleteBidirectional Flag.
*/
bool QOpcUaDeleteReferenceItem::deleteBidirectional() const
{
    return data->deleteBidirectional;
}

/*!
    Sets the deleteBidirectional flag to \a deleteBidirectional.
    If this flag is false, only this reference will be deleted.
    Else, the opposite reference from the target node is deleted too if accessible by the server.
*/
void QOpcUaDeleteReferenceItem::setDeleteBidirectional(bool deleteBidirectional)
{
    data->deleteBidirectional = deleteBidirectional;
}

/*!
    Returns the target node id.
*/
QOpcUaExpandedNodeId QOpcUaDeleteReferenceItem::targetNodeId() const
{
    return data->targetNodeId;
}

/*!
    Sets the node id of the target node to \a targetNodeId.
*/
void QOpcUaDeleteReferenceItem::setTargetNodeId(const QOpcUaExpandedNodeId &targetNodeId)
{
    data->targetNodeId = targetNodeId;
}

/*!
    Returns the isForwardReference flag.
*/
bool QOpcUaDeleteReferenceItem::isForwardReference() const
{
    return data->isForwardReference;
}

/*!
    Sets the isForwardReference flag to \a isForwardReference.
*/
void QOpcUaDeleteReferenceItem::setIsForwardReference(bool isForwardReference)
{
    data->isForwardReference = isForwardReference;
}

/*!
    Returns the reference type id.
*/
QString QOpcUaDeleteReferenceItem::referenceTypeId() const
{
    return data->referenceTypeId;
}

/*!
    Sets the reference type id to \a referenceTypeId.
*/
void QOpcUaDeleteReferenceItem::setReferenceTypeId(const QString &referenceTypeId)
{
    data->referenceTypeId = referenceTypeId;
}

/*!
    Returns the source node id.
*/
QString QOpcUaDeleteReferenceItem::sourceNodeId() const
{
    return data->sourceNodeId;
}

/*!
    Sets the node id of the source node to \a sourceNodeId.
*/
void QOpcUaDeleteReferenceItem::setSourceNodeId(const QString &sourceNodeId)
{
    data->sourceNodeId = sourceNodeId;
}

QT_END_NAMESPACE

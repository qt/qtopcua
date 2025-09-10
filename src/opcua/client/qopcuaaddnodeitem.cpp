// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuanodecreationattributes.h"
#include "qopcuaaddnodeitem.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaAddNodeItem
    \inmodule QtOpcUa
    \brief This class stores the necessary information to create a new node on the server.

    \sa QOpcUaClient::addNode()
*/

class QOpcUaAddNodeItemData : public QSharedData
{
public:
    QOpcUaExpandedNodeId parentNodeId;
    QString referenceTypeId;
    QOpcUaExpandedNodeId requestedNewNodeId;
    QOpcUaQualifiedName browseName;
    QOpcUa::NodeClass nodeClass {QOpcUa::NodeClass::Object};
    QOpcUaNodeCreationAttributes nodeAttributes;
    QOpcUaExpandedNodeId typeDefinition;
};

/*!
    Default constructs an add node item with no parameters set.
*/
QOpcUaAddNodeItem::QOpcUaAddNodeItem()
    : data(new QOpcUaAddNodeItemData)
{
}

/*!
    Constructs a add node item from \a other.
*/
QOpcUaAddNodeItem::QOpcUaAddNodeItem(const QOpcUaAddNodeItem &other)
    : data(other.data)
{
}

/*!
    Assigns the values from \a rhs to this add node item.
*/
QOpcUaAddNodeItem &QOpcUaAddNodeItem::operator=(const QOpcUaAddNodeItem &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUaAddNodeItem::~QOpcUaAddNodeItem()
{
}

/*!
    Returns the node id of the type definition node.
*/
QOpcUaExpandedNodeId QOpcUaAddNodeItem::typeDefinition() const
{
    return data->typeDefinition;
}

/*!
    Sets the node id of the type definition node to \a typeDefinition. This value shall be set if the node class
    is not Object or Variable.
*/
void QOpcUaAddNodeItem::setTypeDefinition(const QOpcUaExpandedNodeId &typeDefinition)
{
    data->typeDefinition = typeDefinition;
}

/*!
    Returns the values for the node attributes of the new node.
*/
QOpcUaNodeCreationAttributes QOpcUaAddNodeItem::nodeAttributes() const
{
    return data->nodeAttributes;
}

/*!
    Returns a reference to the values for the node attributes of the new node.
*/
QOpcUaNodeCreationAttributes &QOpcUaAddNodeItem::nodeAttributesRef()
{
    return data->nodeAttributes;
}

/*!
    Sets the values for the node attributes of the new node to \a nodeAttributes.
    Only the attributes supported by the node class of the new node will be used.
*/
void QOpcUaAddNodeItem::setNodeAttributes(const QOpcUaNodeCreationAttributes &nodeAttributes)
{
    data->nodeAttributes = nodeAttributes;
}

/*!
    Returns the node class of the new node.
*/
QOpcUa::NodeClass QOpcUaAddNodeItem::nodeClass() const
{
    return data->nodeClass;
}

/*!
    Sets the node class of the new node to \a nodeClass.
*/
void QOpcUaAddNodeItem::setNodeClass(const QOpcUa::NodeClass &nodeClass)
{
    data->nodeClass = nodeClass;
}

/*!
    Returns the browse name of the new node.
*/
QOpcUaQualifiedName QOpcUaAddNodeItem::browseName() const
{
    return data->browseName;
}

/*!
    Sets the browse name of the new node to \a browseName.
*/
void QOpcUaAddNodeItem::setBrowseName(const QOpcUaQualifiedName &browseName)
{
    data->browseName = browseName;
}

/*!
    Returns the requested new node id.
*/
QOpcUaExpandedNodeId QOpcUaAddNodeItem::requestedNewNodeId() const
{
    return data->requestedNewNodeId;
}

/*!
    Sets the requested new node id to \a requestedNewNodeId.
*/
void QOpcUaAddNodeItem::setRequestedNewNodeId(const QOpcUaExpandedNodeId &requestedNewNodeId)
{
    data->requestedNewNodeId = requestedNewNodeId;
}

/*!
    Returns the reference type id.
*/
QString QOpcUaAddNodeItem::referenceTypeId() const
{
    return data->referenceTypeId;
}

/*!
    Sets the reference type id to \a referenceTypeId. A reference of this type will be used to connect
    the node to the parent node.
*/
void QOpcUaAddNodeItem::setReferenceTypeId(const QString &referenceTypeId)
{
    data->referenceTypeId = referenceTypeId;
}

/*!
    Returns the parent node id.
*/
QOpcUaExpandedNodeId QOpcUaAddNodeItem::parentNodeId() const
{
    return data->parentNodeId;
}

/*!
    Sets the parent node id to \a parentNodeId. A reference of the type set in \l setReferenceTypeId()
    from this node to the newly added node will be created.

    \sa setReferenceTypeId()
*/
void QOpcUaAddNodeItem::setParentNodeId(const QOpcUaExpandedNodeId &parentNodeId)
{
    data->parentNodeId = parentNodeId;
}

QT_END_NAMESPACE

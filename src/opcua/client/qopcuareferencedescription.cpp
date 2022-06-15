// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuareferencedescription.h"
#include <QtOpcUa/qopcuaexpandednodeid.h>
#include <QtOpcUa/qopcuaqualifiedname.h>
#include <QtOpcUa/qopcualocalizedtext.h>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaReferenceDescription
    \inmodule QtOpcUa
    \brief Contains information about a node.

    This class is used to return the results of a call to \l QOpcUaNode::browse() or
    \l QOpcUaNode::browseChildren().

    It contains the type and direction of the reference used to connect the node
    to the starting node of the browse operation, the node id of the target's type
    definition node and the values of the following \l {QOpcUa::NodeAttribute} {node attributes}:
    \table
    \header
    \li Attribute
    \row
    \li NodeId
    \row
    \li BrowseName
    \row
    \li DisplayName
    \row
    \li NodeClass
    \endtable
*/

class QOpcUaReferenceDescriptionPrivate : public QSharedData
{
public:
    QString refTypeId;
    QOpcUaExpandedNodeId targetNodeId;
    QOpcUaExpandedNodeId typeDefinition;
    QOpcUaQualifiedName browseName;
    QOpcUaLocalizedText displayName;
    QOpcUa::NodeClass nodeClass {QOpcUa::NodeClass::Object};
    bool isForwardReference {true};
};

/*!
    Creates an empty QOpcUaReferenceDescription object
*/
QOpcUaReferenceDescription::QOpcUaReferenceDescription()
    : d_ptr(new QOpcUaReferenceDescriptionPrivate())
{}

/*!
    Creates a copy of the QOpcUaReferenceDescription object \a other.
*/
QOpcUaReferenceDescription::QOpcUaReferenceDescription(const QOpcUaReferenceDescription &other)
    : d_ptr(other.d_ptr)
{}

/*!
    Assigns the value of \a other to this object.
*/
QOpcUaReferenceDescription &QOpcUaReferenceDescription::operator=(const QOpcUaReferenceDescription &other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
    Destructor for QOpcUaReferenceDescription
*/
QOpcUaReferenceDescription::~QOpcUaReferenceDescription()
{}

/*!
    Returns the node class of the node.
*/
QOpcUa::NodeClass QOpcUaReferenceDescription::nodeClass() const
{
    return d_ptr->nodeClass;
}

/*!
    Sets the node class of the node to \a nodeClass.
*/
void QOpcUaReferenceDescription::setNodeClass(QOpcUa::NodeClass nodeClass)
{
    d_ptr->nodeClass = nodeClass;
}

/*!
    Sets \a isForwardReference as isForwardReference information for the reference.
*/
void QOpcUaReferenceDescription::setIsForwardReference(bool isForwardReference)
{
    d_ptr->isForwardReference = isForwardReference;
}

/*!
    Returns \c true if the reference is forward.
*/
bool QOpcUaReferenceDescription::isForwardReference() const
{
    return d_ptr->isForwardReference;
}

/*!
    Sets \a typeDefinition as id of the type definition.
*/
void QOpcUaReferenceDescription::setTypeDefinition(const QOpcUaExpandedNodeId &typeDefinition)
{
    d_ptr->typeDefinition = typeDefinition;
}

/*!
    Returns the type definition id.
*/
QOpcUaExpandedNodeId QOpcUaReferenceDescription::typeDefinition() const
{
    return d_ptr->typeDefinition;
}

/*!
    Returns the display name of the node.
*/
QOpcUaLocalizedText QOpcUaReferenceDescription::displayName() const
{
    return d_ptr->displayName;
}

/*!
    Sets the display name of the node to \a displayName.
*/
void QOpcUaReferenceDescription::setDisplayName(const QOpcUaLocalizedText &displayName)
{
    d_ptr->displayName = displayName;
}

/*!
    Returns the browse name of the node.
*/
QOpcUaQualifiedName QOpcUaReferenceDescription::browseName() const
{
    return d_ptr->browseName;
}

/*!
    Sets the browse name of the node to \a browseName.
*/
void QOpcUaReferenceDescription::setBrowseName(const QOpcUaQualifiedName &browseName)
{
    d_ptr->browseName = browseName;
}

/*!
    Returns the node id of the node.
*/
QOpcUaExpandedNodeId QOpcUaReferenceDescription::targetNodeId() const
{
    return d_ptr->targetNodeId;
}

/*!
    Sets the node id of the node to \a nodeId.
*/
void QOpcUaReferenceDescription::setTargetNodeId(const QOpcUaExpandedNodeId &nodeId)
{
    d_ptr->targetNodeId = nodeId;
}

/*!
    Returns the reference type id of the node.
*/
QString QOpcUaReferenceDescription::refTypeId() const
{
    return d_ptr->refTypeId;
}

/*!
    Sets the reference type id of the node to \a refTypeId.

    \sa QOpcUa::nodeIdFromReferenceType()
*/
void QOpcUaReferenceDescription::setRefTypeId(const QString &refTypeId)
{
    d_ptr->refTypeId = refTypeId;
}

QT_END_NAMESPACE

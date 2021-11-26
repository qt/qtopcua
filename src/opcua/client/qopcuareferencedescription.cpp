/****************************************************************************
**
** Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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

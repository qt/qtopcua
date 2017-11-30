/****************************************************************************
**
** Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
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

#include "qopcuareferencedescription.h"
#include "qopcuareferencedescription_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaReferenceDescription
    This class is used to return the results of a call to \l QOpcUaNode::browseChildren().

    It contains the type of the reference used to connect the child node to the parent
    and the values of the following attributes of the node:
    NodeClass, DisplayName, BrowseName and NodeId (see \l QOpcUa::NodeAttribute).
*/

/*!
    Creates an empty QOpcUaReferenceDescription object
*/
QOpcUaReferenceDescription::QOpcUaReferenceDescription()
    : d_ptr(new QOpcUaReferenceDescriptionPrivate())
{}

/*!
    Creates a copy of another QOpcUaReferenceDescription object
*/
QOpcUaReferenceDescription::QOpcUaReferenceDescription(const QOpcUaReferenceDescription &other)
    : d_ptr(other.d_ptr)
{}

/*!
    Compares this object to another QOpcUaReferenceDescription object
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
    Sets the node class of the node.
*/
void QOpcUaReferenceDescription::setNodeClass(QOpcUa::NodeClass nodeClass)
{
    d_ptr->nodeClass = nodeClass;
}

/*!
    Returns the display name of the node.
*/

QOpcUa::QLocalizedText QOpcUaReferenceDescription::displayName() const
{
    return d_ptr->displayName;
}

/*!
    Sets the display name of the node.
*/
void QOpcUaReferenceDescription::setDisplayName(const QOpcUa::QLocalizedText &displayName)
{
    d_ptr->displayName = displayName;
}

/*!
    Returns the browse name of the node.
*/
QOpcUa::QQualifiedName QOpcUaReferenceDescription::browseName() const
{
    return d_ptr->browseName;
}

/*!
    Sets the browse name of the node.
*/
void QOpcUaReferenceDescription::setBrowseName(const QOpcUa::QQualifiedName &browseName)
{
    d_ptr->browseName = browseName;
}

/*!
    Returns the node id of the node.
*/
QString QOpcUaReferenceDescription::nodeId() const
{
    return d_ptr->nodeId;
}

/*!
    Sets the node id of the node.
*/
void QOpcUaReferenceDescription::setNodeId(const QString &nodeId)
{
    d_ptr->nodeId = nodeId;
}

/*!
    Returns the reference type of the node.
*/
QOpcUa::ReferenceTypeId QOpcUaReferenceDescription::refType() const
{
    return d_ptr->refType;
}

/*!
    Sets the reference type of the node.
*/
void QOpcUaReferenceDescription::setRefType(QOpcUa::ReferenceTypeId refType)
{
    d_ptr->refType = refType;
}

QT_END_NAMESPACE

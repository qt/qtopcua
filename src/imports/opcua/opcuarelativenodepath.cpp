/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#include "opcuarelativenodepath.h"
#include "qopcuatype.h"
#include "qopcuaclient.h"

QT_BEGIN_NAMESPACE

/*!
    \qmltype RelativeNodePath
    \inqmlmodule QtOpcUa
    \brief Specifies a relative node path element.
    \since QtOpcUa 5.12

    \code
    import QtOpcUa 5.12 as QtOpcUa

    QtOpcUa.RelativeNodePath {
        ns: "Test Namespace"
        browseName: "SomeName"
    }
    \endcode

    \sa Node, NodeId, RelativeNodeId
*/

/*!
    \qmlproperty NodeId RelativeNodePath::ns

    Namespace name of this path element.
    The identifier can be the index as a number or the name as string.
    A string which can be converted to an integer is considered a namespace index.
*/

/*!
    \qmlproperty NodeId RelativeNodePath::browseName

    Browse name of this path element.
*/

/*!
    \qmlproperty bool RelativeNodePath::includeSubtypes

    Whether subtypes are included when matching this path element.
    The default value of this property is \c true.
*/

/*!
    \qmlproperty bool RelativeNodePath::isInverse

    Whether the reference to follow is inverse.
    The default value of this property is \c false.
*/

/*!
    \qmlproperty QOpcUa::ReferenceTypeId RelativeNodePath::referenceType

    Type of reference when mathing this path element.
    The default value of this property is \c QOpcUa::ReferenceTypeId::References.
*/

OpcUaRelativeNodePath::OpcUaRelativeNodePath(QObject *parent) : QObject(parent)
{
    connect(&m_browseNode, &UniversalNode::namespaceNameChanged, this, &OpcUaRelativeNodePath::nodeNamespaceChanged);
    connect(&m_browseNode, &UniversalNode::nodeIdentifierChanged, this, &OpcUaRelativeNodePath::browseNameChanged);
}

const QString &OpcUaRelativeNodePath::nodeNamespace() const
{
    return m_browseNode.namespaceName();
}

const QString &OpcUaRelativeNodePath::browseName() const
{
    return m_browseNode.nodeIdentifier();
}

QOpcUa::ReferenceTypeId OpcUaRelativeNodePath::referenceType() const
{
    return m_referenceType;
}

bool OpcUaRelativeNodePath::includeSubtypes() const
{
    return m_includeSubtypes;
}

void OpcUaRelativeNodePath::setNodeNamespace(QString ns)
{
    m_browseNode.setNamespace(ns);
}

void OpcUaRelativeNodePath::setBrowseName(QString browseName)
{
    m_browseNode.setNodeIdentifier(browseName);
}

void OpcUaRelativeNodePath::setReferenceType(QOpcUa::ReferenceTypeId referenceType)
{
    if (m_referenceType == referenceType)
        return;

    m_referenceType = referenceType;
    emit referenceTypeChanged(m_referenceType);
}

void OpcUaRelativeNodePath::setIncludeSubtypes(bool includeSubtypes)
{
    if (m_includeSubtypes == includeSubtypes)
        return;

    m_includeSubtypes = includeSubtypes;
    emit includeSubtypesChanged(m_includeSubtypes);
}

bool OpcUaRelativeNodePath::isInverse() const
{
    return m_isInverse;
}

QOpcUa::QRelativePathElement OpcUaRelativeNodePath::toRelativePathElement(QOpcUaClient *client) const
{
    m_browseNode.resolveNamespaceNameToIndex(client);

    QOpcUa::QRelativePathElement x;
    x.setIsInverse(isInverse());
    x.setIncludeSubtypes(includeSubtypes());
    x.setTargetName(m_browseNode.toQualifiedName());
    x.setReferenceTypeId(referenceType());
    return x;
}

void OpcUaRelativeNodePath::setIsInverse(bool isInverse)
{
    if (m_isInverse == isInverse)
        return;

    m_isInverse = isInverse;
    emit isInverseChanged(m_isInverse);
}

QT_END_NAMESPACE

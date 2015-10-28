/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
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

#include "qopcuanode.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaNode
    \inmodule QtOpcUa

    \brief QOpcUaNode is a base class for plugin specific node objects which\
    allows access to attributes of an OPC UA node.

    The node is the basic building block of the OPC UA address space.
    It has attributes like browse name, value, associated properties and can have
    references to other nodes in the address space.
    Nodes are organized in namespaces and have IDs which can e.g. be numeric,
    a string, a namespace specific format (/opaque) or a globally unique identifier.
    A node is identified by the namespace id and the node ID.
    This identifier is usually given as a string:
    The identifier of a node residing in namespace 0 and having the numeric
    identifier 42, the XML string is \c ns=0;i=42, a node with a string
    identifier can be addressed via \c ns=0;s=myStringIdentifier.

    The node attributes are read from the server when the getter methods are
    called.

    Objects of this type are owned by QOpcUaClient and will be destroyed when
    the QOpcUaClient object is destroyed.
*/

/*!
    Constructs a QOpcUaNode object. The value of \a parent is passed to the
    QObject constructor.
*/
QOpcUaNode::QOpcUaNode(QObject *parent)
    : QObject(parent)
{
}

/*!
    \property QOpcUaNode::name
    \brief the DisplayName attribute of the OPC UA node.

    The getter method must be implemented in the concrete plugin implementation.
*/
/*!
    \property QOpcUaNode::type
    \brief the type attribute of the OPC UA node.

    The getter method must be implemented in the concrete plugin implementation.
 */
/*!
    \property QOpcUaNode::value
    \brief the value attribute of the OPC UA node.

    The getter method must be implemented in the concrete plugin implementation.
*/

/*!
    \property QOpcUaNode::id
    \brief the numeric ID of the OPC UA node.

    The getter method must be implemented in the concrete plugin implementation.
*/

/*!
    \property QOpcUaNode::childCount
    \brief the child count of the OPC UA node.

    The getter method must be implemented in the concrete plugin implementation.
*/

/*!
    \fn void QOpcUaNode::children()

    Returns a QList filled with children objects of the OPC UA node.
    must be reimplemented in the concrete plugin implementation.
*/

/*!
   \property QOpcUaNode::childIds
   \brief QStringList filled with the node IDs of all child nodes of the OPC UA node.

    The getter method must be reimplemented in the concrete plugin
    implementation.
*/

/*!
    \property QOpcUaNode::xmlNodeId
    \brief the XML ID of the OPC UA node.

    The getter method must be implemented in the concrete plugin implementation.
*/

/*!
   \property QOpcUaNode::nodeClass
   \brief the node class of the OPC UA node.

    The getter method must be reimplemented in the concrete plugin
    implementation.
*/

/*!
    \brief Returns the value as a base64 encoded string if the type is "ByteArray".
    If the value has an other type, the value is returned unencoded.
    This comes in handy when the value is an image or icon which can be
    displayed in QML applications via \l {http://dataurl.net/#about} {Data URL}.
*/
QVariant QOpcUaNode::encodedValue() const
{
    QVariant val = value();
    if (val.type() == QVariant::ByteArray)
        return QString::fromUtf8(val.toByteArray().toBase64());

    return val;
}

QT_END_NAMESPACE

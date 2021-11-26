/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#include <opcuareaditem_p.h>

QT_BEGIN_NAMESPACE

/*!
    \qmltype ReadItem
    \inqmlmodule QtOpcUa
    \brief Specifies an item to be read from the server.
    \since QtOpcUa 5.13

    This type is used to specify items to be read from the server using the function
    \l Connection::readNodeAttributes.
*/

/*!
    \qmlproperty Constants.NodeAttribute ReadItem::attribute

    Determines the attribute of the node to be read.
*/

/*!
    \qmlproperty string ReadItem::indexRange

    Determines the index range of the attribute to be read.
    If not needed, leave this property empty.
*/

/*!
    \qmlproperty string ReadItem::nodeId

    Determines the node id of the node to be read.
*/

/*!
    \qmlproperty variant ReadItem::ns

    Determines the namespace of the node to be read.
    The namespace can be given by name or index.
    If this property is given, any namespace in the node id will be
    ignored.
*/

class OpcUaReadItemData : public QSharedData
{
public:
    QOpcUa::NodeAttribute attribute;
    QString indexRange;
    QString nodeId;
    QVariant namespaceIdentifier;
};

OpcUaReadItem::OpcUaReadItem()
    : data(new OpcUaReadItemData)
{
    data->attribute = QOpcUa::NodeAttribute::Value;
}

OpcUaReadItem::OpcUaReadItem(const OpcUaReadItem &other)
    : data(other.data)
{
}

OpcUaReadItem &OpcUaReadItem::operator=(const OpcUaReadItem &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

OpcUaReadItem::~OpcUaReadItem() = default;

const QString &OpcUaReadItem::indexRange() const
{
    return data->indexRange;
}

void OpcUaReadItem::setIndexRange(const QString &indexRange)
{
    data->indexRange = indexRange;
}

const QString &OpcUaReadItem::nodeId() const
{
    return data->nodeId;
}

void OpcUaReadItem::setNodeId(const QString &nodeId)
{
    data->nodeId = nodeId;
}

QOpcUa::NodeAttribute OpcUaReadItem::attribute() const
{
    return data->attribute;
}

void OpcUaReadItem::setAttribute(QOpcUa::NodeAttribute attribute)
{
    data->attribute = attribute;
}

const QVariant &OpcUaReadItem::namespaceIdentifier() const
{
    return data->namespaceIdentifier;
}

void OpcUaReadItem::setNamespaceIdentifier(const QVariant &namespaceIdentifier)
{
    data->namespaceIdentifier = namespaceIdentifier;
}

OpcUaReadItem OpcUaReadItemFactory::create()
{
    return OpcUaReadItem();
}

QT_END_NAMESPACE


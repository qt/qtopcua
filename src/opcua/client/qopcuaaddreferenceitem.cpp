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

#include "qopcuanodecreationattributes.h"
#include "qopcuaaddreferenceitem.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaAddReferenceItem
    \inmodule QtOpcUa
    \brief This class stores the necessary information to add a new reference on the server.

    \sa QOpcUaClient::addReference()
*/
class QOpcUaAddReferenceItemData : public QSharedData
{
public:
    QString sourceNodeId;
    QString referenceTypeId;
    bool isForward {true};
    QOpcUa::QExpandedNodeId targetNodeId;
    QOpcUa::NodeClass targetNodeClass {QOpcUa::NodeClass::Undefined};
    QString targetServerUri;
};

QOpcUaAddReferenceItem::QOpcUaAddReferenceItem()
    : data(new QOpcUaAddReferenceItemData)
{
}

/*!
    Constructs an add reference item from \a other.
*/
QOpcUaAddReferenceItem::QOpcUaAddReferenceItem(const QOpcUaAddReferenceItem &other)
    : data(other.data)
{

}

/*!
    Sets the values from \a rhs in this add reference item.
*/
QOpcUaAddReferenceItem &QOpcUaAddReferenceItem::operator=(const QOpcUaAddReferenceItem &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUaAddReferenceItem::~QOpcUaAddReferenceItem()
{
}

/*!
    Returns the target node class.
*/
QOpcUa::NodeClass QOpcUaAddReferenceItem::targetNodeClass() const
{
    return data->targetNodeClass;
}

/*!
    Sets the node class of the target node to \a targetNodeClass.
    The target node class shall be specified because the target node might be located on another server.
*/
void QOpcUaAddReferenceItem::setTargetNodeClass(QOpcUa::NodeClass targetNodeClass)
{
    data->targetNodeClass = targetNodeClass;
}

/*!
    Returns the target server URI.
*/
QString QOpcUaAddReferenceItem::targetServerUri() const
{
    return data->targetServerUri;
}

/*!
    Sets the URI of the target server to \a targetServerUri.
    If this value is set, it will override the server URI from \l setTargetNodeId().
*/
void QOpcUaAddReferenceItem::setTargetServerUri(const QString &targetServerUri)
{
    data->targetServerUri = targetServerUri;
}

/*!
    Returns the target node id.
*/
QOpcUa::QExpandedNodeId QOpcUaAddReferenceItem::targetNodeId() const
{
    return data->targetNodeId;
}

/*!
    Sets the node id of the target node to \a targetNodeId.
*/
void QOpcUaAddReferenceItem::setTargetNodeId(const QOpcUa::QExpandedNodeId &targetNodeId)
{
    data->targetNodeId = targetNodeId;
}

/*!
    Returns the isForward flag.
*/
bool QOpcUaAddReferenceItem::isForward() const
{
    return data->isForward;
}

/*!
    Sets the isForward flag to \a isForward.
    If the flag is set, a forward reference is created.
    Otherwise, an inverse reference is created.
*/
void QOpcUaAddReferenceItem::setIsForward(bool isForward)
{
    data->isForward = isForward;
}

/*!
    Returns the reference type id.
*/
QString QOpcUaAddReferenceItem::referenceTypeId() const
{
    return data->referenceTypeId;
}

/*!
    Sets the reference type id to \a referenceTypeId.
    A reference of this type will be created on the server.
*/
void QOpcUaAddReferenceItem::setReferenceTypeId(const QString &referenceTypeId)
{
    data->referenceTypeId = referenceTypeId;
}

/*!
    Returns the source node id.
*/
QString QOpcUaAddReferenceItem::sourceNodeId() const
{
    return data->sourceNodeId;
}

/*!
    Sets the node id of the source node to \a sourceNodeId.
*/
void QOpcUaAddReferenceItem::setSourceNodeId(const QString &sourceNodeId)
{
    data->sourceNodeId = sourceNodeId;
}

QT_END_NAMESPACE

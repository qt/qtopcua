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

#include "qopcuabrowserequest.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaBrowseRequest
    \inmodule QtOpcUa
    \brief Contains parameters for a call to the OPC UA browse service.

    \sa QOpcUaNode::browse()
*/

/*!
    \enum QOpcUaBrowseRequest::BrowseDirection

    This enum specifies the possible browse directions supported by a browse call.

    \value Forward Follow references in the forward direction.
    \value Inverse Follow references in the inverse direction.
    \value Both Follow references in both directions.
*/

class QOpcUaBrowseRequestData : public QSharedData
{
public:
    QOpcUaBrowseRequest::BrowseDirection browseDirection {QOpcUaBrowseRequest::BrowseDirection::Forward};
    QString referenceTypeId;
    bool includeSubtypes {false};
    QOpcUa::NodeClasses nodeClassMask;
};

QOpcUaBrowseRequest::QOpcUaBrowseRequest()
    : data(new QOpcUaBrowseRequestData)
{
}

/*!
    Creates a browse request from \a other.
*/
QOpcUaBrowseRequest::QOpcUaBrowseRequest(const QOpcUaBrowseRequest &other)
    : data(other.data)
{
}

/*!
    Sets the values from \a rhs in this browse request.
*/
QOpcUaBrowseRequest &QOpcUaBrowseRequest::operator=(const QOpcUaBrowseRequest &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUaBrowseRequest::~QOpcUaBrowseRequest()
{
}

/*!
    Returns the browse direction.
*/
QOpcUaBrowseRequest::BrowseDirection QOpcUaBrowseRequest::browseDirection() const
{
    return data->browseDirection;
}

/*!
    Sets the browse direction to \a browseDirection.
*/
void QOpcUaBrowseRequest::setBrowseDirection(const QOpcUaBrowseRequest::BrowseDirection &browseDirection)
{
    data->browseDirection = browseDirection;
}

/*!
    Returns the reference type id.
*/
QString QOpcUaBrowseRequest::referenceTypeId() const
{
    return data->referenceTypeId;
}

/*!
    Sets the reference type id to \a referenceTypeId.
*/
void QOpcUaBrowseRequest::setReferenceTypeId(const QString &referenceTypeId)
{
    data->referenceTypeId = referenceTypeId;
}

/*!
    Sets the reference type id to \a referenceTypeId.
*/
void QOpcUaBrowseRequest::setReferenceTypeId(QOpcUa::ReferenceTypeId referenceTypeId)
{
    data->referenceTypeId = QOpcUa::nodeIdFromReferenceType(referenceTypeId);
}

/*!
    Returns true if subtypes of the reference type will be retrieved too.
*/
bool QOpcUaBrowseRequest::includeSubtypes() const
{
    return data->includeSubtypes;
}

/*!
    Sets the inclusion of subtypes of the reference type to \a includeSubtypes.
*/
void QOpcUaBrowseRequest::setIncludeSubtypes(bool includeSubtypes)
{
    data->includeSubtypes = includeSubtypes;
}

/*!
    Returns the node class mask.
*/
QOpcUa::NodeClasses QOpcUaBrowseRequest::nodeClassMask() const
{
    return data->nodeClassMask;
}

/*!
    Sets the node class mask to \a nodeClassMask.
    Nodes of all classes included into the node class mask will be returned
    by the browse operation.
*/
void QOpcUaBrowseRequest::setNodeClassMask(const QOpcUa::NodeClasses &nodeClassMask)
{
    data->nodeClassMask = nodeClassMask;
}

QT_END_NAMESPACE

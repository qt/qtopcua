// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

/*!
    Default constructs a browse request with no parameters set.
*/
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

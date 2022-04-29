/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#include "qopcuabrowsepathtarget.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaBrowsePathTarget
    \inmodule QtOpcUa
    \brief The OPC UA BrowsePathTarget.

    A BrowsePathTarget contains a target of a browse path and information about the completeness of the node id resolution.
*/
class QOpcUaBrowsePathTargetData : public QSharedData
{
public:
    QOpcUaExpandedNodeId targetId;
    quint32 remainingPathIndex{(std::numeric_limits<quint32>::max)()};
};

QOpcUaBrowsePathTarget::QOpcUaBrowsePathTarget()
    : data(new QOpcUaBrowsePathTargetData)
{
}

/*!
    Constructs a browse path target from \a rhs.
*/
QOpcUaBrowsePathTarget::QOpcUaBrowsePathTarget(const QOpcUaBrowsePathTarget &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values of \a rhs in this browse path target.
*/
QOpcUaBrowsePathTarget &QOpcUaBrowsePathTarget::operator=(const QOpcUaBrowsePathTarget &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this browse path target has the same value as \a rhs.
*/
bool QOpcUaBrowsePathTarget::operator==(const QOpcUaBrowsePathTarget &rhs) const
{
    return data->targetId == rhs.targetId() &&
            data->remainingPathIndex == rhs.remainingPathIndex();
}

QOpcUaBrowsePathTarget::~QOpcUaBrowsePathTarget()
{
}

/*!
    Returns the index of the first unprocessed element in the browse path.
    If the path was followed to the end, remainingPathIndex has the maximum value of quint32.
    \sa QOpcUaBrowsePathTarget::targetId()
*/
quint32 QOpcUaBrowsePathTarget::remainingPathIndex() const
{
    return data->remainingPathIndex;
}

/*!
    Sets the remaining path index to \a remainingPathIndex.
*/
void QOpcUaBrowsePathTarget::setRemainingPathIndex(quint32 remainingPathIndex)
{
    data->remainingPathIndex = remainingPathIndex;
}

/*!
    Returns \c true if the browse path has been fully resolved.
*/
bool QOpcUaBrowsePathTarget::isFullyResolved() const
{
    return (data->remainingPathIndex == (std::numeric_limits<quint32>::max)());
}

/*!
    Returns the target of the last reference the server was able to follow.
    If the reference leads to an external server, \e targetId is the id of the first node on that server.
    \sa QOpcUaBrowsePathTarget::remainingPathIndex
*/
QOpcUaExpandedNodeId QOpcUaBrowsePathTarget::targetId() const
{
    return data->targetId;
}

/*!
    Returns a reference to the target id.
*/
QOpcUaExpandedNodeId &QOpcUaBrowsePathTarget::targetIdRef()
{
    return data->targetId;
}

/*!
    Sets the node id of the target node to \a targetId.
*/
void QOpcUaBrowsePathTarget::setTargetId(const QOpcUaExpandedNodeId &targetId)
{
    data->targetId = targetId;
}

QT_END_NAMESPACE

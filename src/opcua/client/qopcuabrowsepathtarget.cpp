// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

/*!
    Default constructs a browse path target with no parameters set.
*/
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

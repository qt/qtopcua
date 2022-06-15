// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/opcuawriteresult_p.h>
#include <private/universalnode_p.h>

#include <QOpcUaWriteResult>
#include <QOpcUaClient>
#include <qopcuatype.h>

QT_BEGIN_NAMESPACE

/*!
    \qmltype WriteResult
    \inqmlmodule QtOpcUa
    \brief Contains result data after writing to the server.
    \since QtOpcUa 5.13

    This type is used to pass the results after writing to the server using the function
    \l Connection::writeNodeAttributes.

    \sa WriteItem
*/

/*!
    \qmlproperty Constants.NodeAttribute WriteResult::attribute
    \readonly

    The node attribute of data that was written.
*/

/*!
    \qmlproperty string WriteResult::indexRange
    \readonly

    The index range of the data that was written.
*/

/*!
    \qmlproperty string WriteResult::nodeId
    \readonly

    The node id of the node that was written.
*/

/*!
    \qmlproperty string WriteResult::namespaceName
    \readonly

    The namespace name of the node that was written.
*/

/*!
    \qmlproperty Status WriteResult::status
    \readonly

    Result status of this WriteResult.
    If the write request was successful the status is \l {Status::Status}
    {Status.Good}.
*/

class OpcUaWriteResultData : public QSharedData
{
public:
    OpcUaStatus status;
    QOpcUa::NodeAttribute attribute;
    QString indexRange;
    QString nodeId;
    QString namespaceName;
};

OpcUaWriteResult::OpcUaWriteResult()
    : data(new OpcUaWriteResultData)
{
    data->attribute = QOpcUa::NodeAttribute::None;
}

OpcUaWriteResult::OpcUaWriteResult(const OpcUaWriteResult &other)
    : data(other.data)
{
}

OpcUaWriteResult::OpcUaWriteResult(const QOpcUaWriteResult &other, const QOpcUaClient *client)
    : data(new OpcUaWriteResultData)
{
    data->status = OpcUaStatus(other.statusCode());
    data->attribute = other.attribute();
    data->indexRange = other.indexRange();

    int namespaceIndex = -1;
    UniversalNode::splitNodeIdAndNamespace(other.nodeId(), &namespaceIndex, &data->nodeId);
    data->namespaceName = client->namespaceArray().at(namespaceIndex);
}

OpcUaWriteResult &OpcUaWriteResult::operator=(const OpcUaWriteResult &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

OpcUaWriteResult::~OpcUaWriteResult() = default;

const QString &OpcUaWriteResult::indexRange() const
{
    return data->indexRange;
}

const QString &OpcUaWriteResult::nodeId() const
{
    return data->nodeId;
}

QOpcUa::NodeAttribute OpcUaWriteResult::attribute() const
{
    return data->attribute;
}

const QString &OpcUaWriteResult::namespaceName() const
{
    return data->namespaceName;
}

OpcUaStatus OpcUaWriteResult::status() const
{
    return data->status;
}

QT_END_NAMESPACE


// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/opcuareadresult_p.h>
#include <private/universalnode_p.h>

#include <QOpcUaReadResult>
#include <QOpcUaClient>
#include <qopcuatype.h>

QT_BEGIN_NAMESPACE

/*!
    \qmltype ReadResult
    \inqmlmodule QtOpcUa
    \brief Contains result data after reading from the server.
    \since QtOpcUa 5.13

    This type is used to pass the read data after reading from the server using the function
    \l Connection::readNodeAttributes.
*/

/*!
    \qmlproperty Constants.NodeAttribute ReadResult::attribute
    \readonly

    The node attribute of data that was read.
*/

/*!
    \qmlproperty string ReadResult::indexRange
    \readonly

    The index range of the data that was read.
*/

/*!
    \qmlproperty string ReadResult::nodeId
    \readonly

    The node id of the node that was read.
*/

/*!
    \qmlproperty string ReadResult::namespaceName
    \readonly

    The namespace name of the node that was read.
*/

/*!
    \qmlproperty datetime ReadResult::serverTimestamp
    \readonly

    The server timestamp of the data that was read.
*/

/*!
    \qmlproperty datetime ReadResult::sourceTimestamp
    \readonly

    The source timestamp of the data that was read.
*/

/*!
    \qmlproperty variant ReadResult::value
    \readonly

    Actual data that was requested to be read.
*/

/*!
    \qmlproperty Status ReadResult::status
    \readonly

    Result status of this ReadResult.
    Before using any value of this ReadResult, the status
    should be checked for \l {Status::Status}{Status.isGood}. To make sure
    the server has provided valid data.
*/

class OpcUaReadResultData : public QSharedData
{
public:
    OpcUaStatus status;
    QOpcUa::NodeAttribute attribute;
    QString indexRange;
    QString nodeId;
    QString namespaceName;
    QDateTime serverTimestamp;
    QDateTime sourceTimestamp;
    QVariant value;
};

OpcUaReadResult::OpcUaReadResult()
    : data(new OpcUaReadResultData)
{
    data->attribute = QOpcUa::NodeAttribute::None;
}

OpcUaReadResult::OpcUaReadResult(const OpcUaReadResult &other)
    : data(other.data)
{
}

OpcUaReadResult::OpcUaReadResult(const QOpcUaReadResult &other, const QOpcUaClient *client)
    : data(new OpcUaReadResultData)
{
    data->status = OpcUaStatus(other.statusCode());
    data->attribute = other.attribute();
    data->indexRange = other.indexRange();
    data->serverTimestamp = other.serverTimestamp();
    data->sourceTimestamp = other.sourceTimestamp();
    data->value = other.value();

    int namespaceIndex = -1;
    UniversalNode::splitNodeIdAndNamespace(other.nodeId(), &namespaceIndex, &data->nodeId);
    data->namespaceName = client->namespaceArray().at(namespaceIndex);
}

OpcUaReadResult &OpcUaReadResult::operator=(const OpcUaReadResult &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

OpcUaReadResult::~OpcUaReadResult() = default;

const QString &OpcUaReadResult::indexRange() const
{
    return data->indexRange;
}

const QString &OpcUaReadResult::nodeId() const
{
    return data->nodeId;
}

QOpcUa::NodeAttribute OpcUaReadResult::attribute() const
{
    return data->attribute;
}

const QString &OpcUaReadResult::namespaceName() const
{
    return data->namespaceName;
}

const QDateTime &OpcUaReadResult::serverTimestamp() const
{
    return data->serverTimestamp;
}

const QDateTime &OpcUaReadResult::sourceTimestamp() const
{
    return data->sourceTimestamp;
}

const QVariant &OpcUaReadResult::value() const
{
    return data->value;
}

OpcUaStatus OpcUaReadResult::status() const
{
    return data->status;
}

QT_END_NAMESPACE


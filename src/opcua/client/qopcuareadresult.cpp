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

#include "qopcuareadresult.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaReadResult
    \inmodule QtOpcUa
    \brief This class stores the result of a read operation.

    A read operation on an OPC UA server returns the value and timestamps which describe when a value was generated
    by the source and when the server obtained it. It also returns a status code which describes if the value could
    be read and if not, for what reason the read has failed.

    In addition to the data returned by the server, this class also contains the node id, the attribute and the index
    range from the request to enable a client to match the result with a request.

    Objects of this class are returned in the \l QOpcUaClient::readNodeAttributesFinished()
    signal and contain the result of a read operation that was part of a \l QOpcUaClient::readNodeAttributes()
    request.

    \sa QOpcUaClient::readNodeAttributes() QOpcUaClient::readNodeAttributesFinished() QOpcUaReadItem
*/
class QOpcUaReadResultData : public QSharedData
{
public:
    QDateTime serverTimestamp;
    QDateTime sourceTimestamp;
    QOpcUa::UaStatusCode statusCode {QOpcUa::UaStatusCode::Good};
    QString nodeId;
    QOpcUa::NodeAttribute attribute {QOpcUa::NodeAttribute::Value};
    QString indexRange;
    QVariant value;
};

QOpcUaReadResult::QOpcUaReadResult()
    : data(new QOpcUaReadResultData)
{
}

/*!
    Constructs a read result from \a other.
*/
QOpcUaReadResult::QOpcUaReadResult(const QOpcUaReadResult &other)
    : data(other.data)
{
}

/*!
    Sets the values from \a rhs in this read result.
*/
QOpcUaReadResult &QOpcUaReadResult::operator=(const QOpcUaReadResult &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUaReadResult::~QOpcUaReadResult()
{
}

/*!
    Returns the value.
*/
QVariant QOpcUaReadResult::value() const
{
    return data->value;
}

/*!
    Sets the value to \a value.
*/
void QOpcUaReadResult::setValue(const QVariant &value)
{
    data->value = value;
}

/*!
    Returns the attribute id.
*/
QOpcUa::NodeAttribute QOpcUaReadResult::attribute() const
{
    return data->attribute;
}

/*!
    Sets the attribute id to \a attribute.
*/
void QOpcUaReadResult::setAttribute(QOpcUa::NodeAttribute attribute)
{
    data->attribute = attribute;
}

/*!
    Returns the index range.
*/
QString QOpcUaReadResult::indexRange() const
{
    return data->indexRange;
}

/*!
    Sets the index range to \a indexRange.
*/
void QOpcUaReadResult::setIndexRange(const QString &indexRange)
{
    data->indexRange = indexRange;
}

/*!
    Returns the node id.
*/
QString QOpcUaReadResult::nodeId() const
{
    return data->nodeId;
}

/*!
    Sets the node id to \a nodeId.
*/
void QOpcUaReadResult::setNodeId(const QString &nodeId)
{
    data->nodeId = nodeId;
}

/*!
    Returns the status code for this element. If the status code is not \l {QOpcUa::UaStatusCode} {Good}, the
    value and the timestamps are invalid.
*/
QOpcUa::UaStatusCode QOpcUaReadResult::statusCode() const
{
    return data->statusCode;
}

/*!
    Sets the status code to \a statusCode.
*/
void QOpcUaReadResult::setStatusCode(QOpcUa::UaStatusCode statusCode)
{
    data->statusCode = statusCode;
}

/*!
    Returns the source timestamp for \l value().
*/
QDateTime QOpcUaReadResult::sourceTimestamp() const
{
    return data->sourceTimestamp;
}

/*!
    Sets the source timestamp to \a sourceTimestamp.
*/
void QOpcUaReadResult::setSourceTimestamp(const QDateTime &sourceTimestamp)
{
    data->sourceTimestamp = sourceTimestamp;
}

/*!
    Returns the server timestamp for \l value().
*/
QDateTime QOpcUaReadResult::serverTimestamp() const
{
    return data->serverTimestamp;
}

/*!
    Sets the server timestamp to \a serverTimestamp.
*/
void QOpcUaReadResult::setServerTimestamp(const QDateTime &serverTimestamp)
{
    data->serverTimestamp = serverTimestamp;
}

QT_END_NAMESPACE

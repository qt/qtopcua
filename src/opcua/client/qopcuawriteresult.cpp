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

#include "qopcuawriteresult.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaWriteResult
    \inmodule QtOpcUa
    \brief This class stores the result of a write operation.

    A write operation on an OPC UA server returns a status code which describes if the value could
    be written and if not, for what reason the write has failed.

    In addition to the status code returned by the server, this class also contains the node id, the attribute and the index
    range from the request to enable a client to match the result with a request.

    Objects of this class are returned in the \l QOpcUaClient::writeNodeAttributesFinished()
    signal and contain the result of a write operation that was part of a \l QOpcUaClient::writeNodeAttributes()
    request.

    \sa QOpcUaClient::writeNodeAttributes() QOpcUaClient::writeNodeAttributesFinished() QOpcUaWriteItem
*/
class QOpcUaWriteResultData : public QSharedData
{
public:
    QString nodeId;
    QOpcUa::NodeAttribute attribute {QOpcUa::NodeAttribute::Value};
    QString indexRange;
    QOpcUa::UaStatusCode statusCode {QOpcUa::UaStatusCode::Good};
};

QOpcUaWriteResult::QOpcUaWriteResult()
    : data(new QOpcUaWriteResultData)
{
}

/*!
    Constructs a write result from \a other.
*/
QOpcUaWriteResult::QOpcUaWriteResult(const QOpcUaWriteResult &other)
    : data(other.data)
{
}

/*!
    Sets the values from \a rhs in this write result.
*/
QOpcUaWriteResult &QOpcUaWriteResult::operator=(const QOpcUaWriteResult &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUaWriteResult::~QOpcUaWriteResult()
{
}

/*!
    Returns the node id of the write result.
*/
QString QOpcUaWriteResult::nodeId() const
{
    return data->nodeId;
}

/*!
    Sets the node id of the write result to \a nodeId.
*/
void QOpcUaWriteResult::setNodeId(const QString &nodeId)
{
    data->nodeId = nodeId;
}

/*!
    Returns the attribute of the write result.
*/
QOpcUa::NodeAttribute QOpcUaWriteResult::attribute() const
{
    return data->attribute;
}

/*!
    Sets the attribute of the write result to \a attribute.
*/
void QOpcUaWriteResult::setAttribute(QOpcUa::NodeAttribute attribute)
{
    data->attribute = attribute;
}

/*!
    Returns the index range of the write result.
*/
QString QOpcUaWriteResult::indexRange() const
{
    return data->indexRange;
}

/*!
    Sets the index range of the write result to \a indexRange.
*/
void QOpcUaWriteResult::setIndexRange(const QString &indexRange)
{
    data->indexRange = indexRange;
}

/*!
    Returns the status code of the write result.
*/
QOpcUa::UaStatusCode QOpcUaWriteResult::statusCode() const
{
    return data->statusCode;
}

/*!
    Sets the status code of the write result to \a statusCode.
*/
void QOpcUaWriteResult::setStatusCode(QOpcUa::UaStatusCode statusCode)
{
    data->statusCode = statusCode;
}

QT_END_NAMESPACE

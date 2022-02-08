/****************************************************************************
**
** Copyright (C) 2019 basysKom GmbH, opensource@basyskom.com
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

#include "qopcuahistorydata.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaHistoryData
    \inmodule QtOpcUa
    \brief This class stores historical data values from a node.
    \since 6.3

    When a request to read history data is being handled, instances of this class
    are used to store information about which node has been read, its values and
    the status code.
    The finished signal of a history read request will return a list of QOpcUaHistoryData
    objects which can be parsed to review the results of the request.
*/

class QOpcUaHistoryDataData : public QSharedData
{
public:
    QList<QOpcUaDataValue> result;
    QOpcUa::UaStatusCode statusCode;
    QString nodeId;
};

/*!
    Constructs an invalid  history data item.
 */
QOpcUaHistoryData::QOpcUaHistoryData()
    : data(new QOpcUaHistoryDataData)
{
    data->statusCode = QOpcUa::UaStatusCode::Good;
}

/*!
    Constructs a history data item and stores which node it corresponds to.
*/
QOpcUaHistoryData::QOpcUaHistoryData(const QString &nodeId)
    : data(new QOpcUaHistoryDataData)
{
    data->statusCode = QOpcUa::UaStatusCode::Good;
    data->nodeId = nodeId;
}

/*!
    Constructs a history data item from \a other.
*/
QOpcUaHistoryData::QOpcUaHistoryData(const QOpcUaHistoryData &other)
    : data(other.data)
{
}

/*!
    Destroys the history data item.
 */
QOpcUaHistoryData::~QOpcUaHistoryData()
{
}

/*!
    Returns the status code which indicates if an error occurred while fetching the history data.
*/
QOpcUa::UaStatusCode QOpcUaHistoryData::statusCode() const
{
    return data->statusCode;
}

/*!
    Sets the status code to \a statusCode.
*/
void QOpcUaHistoryData::setStatusCode(QOpcUa::UaStatusCode statusCode)
{
    data.detach();
    data->statusCode = statusCode;
}

/*!
    Returns the list of data value objects which contain the results of the history read request.
*/
QList<QOpcUaDataValue> QOpcUaHistoryData::result() const
{
    return data->result;
}

/*!
    Returns the number of available data value objects.
*/
int QOpcUaHistoryData::count() const
{
    return data->result.count();
}

/*!
    Adds a data value object given by \a value.
*/
void QOpcUaHistoryData::addValue(const QOpcUaDataValue &value)
{
    data.detach();
    data->result.append(value);
}

/*!
    Returns the nodeId of the node whose data has been stored.
*/
QString QOpcUaHistoryData::nodeId() const
{
    return data->nodeId;
}

/*!
    Sets the nodeId to \a nodeId.
*/
void QOpcUaHistoryData::setNodeId(const QString &nodeId)
{
    data.detach();
    data->nodeId = nodeId;
}

/*!
    Sets the values from \a other in this history data item.
*/
QOpcUaHistoryData &QOpcUaHistoryData::operator=(const QOpcUaHistoryData &other)
{
    if (this != &other)
        data.operator=(other.data);
    return *this;
}

QT_END_NAMESPACE

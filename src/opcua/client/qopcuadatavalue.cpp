/****************************************************************************
**
** Copyright (C) 2021 basysKom GmbH, opensource@basyskom.com
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

#include "qopcuadatavalue.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaDataValue
    \inmodule QtOpcUa
    \brief This class stores OPC UA value data and associated metadata.

    This clas corresponds to the OPC UA DataValue type.
*/
class QOpcUaDataValueData : public QSharedData
{
public:
    QDateTime serverTimestamp;
    QDateTime sourceTimestamp;
    QOpcUa::UaStatusCode statusCode {QOpcUa::UaStatusCode::Good};
    QVariant value;
};

QOpcUaDataValue::QOpcUaDataValue()
    : data(new QOpcUaDataValueData)
{
}

/*!
    Constructs a data value from \a other.
*/
QOpcUaDataValue::QOpcUaDataValue(const QOpcUaDataValue &other)
    : data(other.data)
{
}

/*!
    Sets the values from \a rhs in this data value.
*/
QOpcUaDataValue &QOpcUaDataValue::operator=(const QOpcUaDataValue &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUaDataValue::~QOpcUaDataValue()
{
}

/*!
    Returns the value.
*/
QVariant QOpcUaDataValue::value() const
{
    return data->value;
}

/*!
    Sets the value to \a value.
*/
void QOpcUaDataValue::setValue(const QVariant &value)
{
    data->value = value;
}

/*!
    Returns the status code for this data value. If the status code is not \l {QOpcUa::UaStatusCode} {Good}, the
    value and the timestamps are invalid.
*/
QOpcUa::UaStatusCode QOpcUaDataValue::statusCode() const
{
    return data->statusCode;
}

/*!
    Sets the status code to \a statusCode.
*/
void QOpcUaDataValue::setStatusCode(QOpcUa::UaStatusCode statusCode)
{
    data->statusCode = statusCode;
}

/*!
    Returns the source timestamp for \l value().
*/
QDateTime QOpcUaDataValue::sourceTimestamp() const
{
    return data->sourceTimestamp;
}

/*!
    Sets the source timestamp to \a sourceTimestamp.
*/
void QOpcUaDataValue::setSourceTimestamp(const QDateTime &sourceTimestamp)
{
    data->sourceTimestamp = sourceTimestamp;
}

/*!
    Returns the server timestamp for \l value().
*/
QDateTime QOpcUaDataValue::serverTimestamp() const
{
    return data->serverTimestamp;
}

/*!
    Sets the server timestamp to \a serverTimestamp.
*/
void QOpcUaDataValue::setServerTimestamp(const QDateTime &serverTimestamp)
{
    data->serverTimestamp = serverTimestamp;
}

QT_END_NAMESPACE

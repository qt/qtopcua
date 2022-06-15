// Copyright (C) 2021 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuadatavalue.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaDataValue
    \inmodule QtOpcUa
    \brief This class stores OPC UA value data and associated metadata.
    \since 6.3

    This class corresponds to the OPC UA DataValue type.
*/
class QOpcUaDataValueData : public QSharedData
{
public:
    QDateTime serverTimestamp;
    QDateTime sourceTimestamp;
    QOpcUa::UaStatusCode statusCode {QOpcUa::UaStatusCode::Good};
    QVariant value;
};

/*!
    Constructs an invalid data value.
*/
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
    Sets the values from \a other in this data value.
*/
QOpcUaDataValue &QOpcUaDataValue::operator=(const QOpcUaDataValue &other)
{
    if (this != &other)
        data.operator=(other.data);
    return *this;
}

/*!
    Destroys the data value.
*/
QOpcUaDataValue::~QOpcUaDataValue()
{
}

/*!
    \fn QOpcUaDataValue::swap(QOpcUaDataValue &other)

    Swaps this data value instance with \a other. This function is very
    fast and never fails.
*/

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
    data.detach();
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
    data.detach();
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
    data.detach();
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
    data.detach();
    data->serverTimestamp = serverTimestamp;
}

QT_END_NAMESPACE

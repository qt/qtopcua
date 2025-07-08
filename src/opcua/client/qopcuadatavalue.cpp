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
    quint16 serverPicoseconds = 0;
    QDateTime sourceTimestamp;
    quint16 sourcePicoseconds = 0;
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

    If this data value is to be used with \l QOpcUaBinaryDataEncoding or
    \l QOpcUaGenericStructHandler, the value's type must be \l QOpcUaVariant.
*/
void QOpcUaDataValue::setValue(const QVariant &value)
{
    data.detach();
    data->value = value;
}

/*!
    Returns a QVariant containing this data value.

    \since 6.7
*/
QOpcUaDataValue::operator QVariant() const
{
    return QVariant::fromValue(*this);
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
    \since 6.7

    Returns the number of 10 picosecond intervals for the source timestamp.
*/
quint16 QOpcUaDataValue::sourcePicoseconds() const
{
    return data->sourcePicoseconds;
}

/*!
    \since 6.7

    Sets the number of 10 picosecond intervals for the source timestamp to \a sourcePicoseconds.
*/
void QOpcUaDataValue::setSourcePicoseconds(quint16 sourcePicoseconds)
{
    data.detach();
    data->sourcePicoseconds = sourcePicoseconds;
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

/*!
    \since 6.7

    Returns the number of 10 picosecond intervals for the server timestamp.
*/
quint16 QOpcUaDataValue::serverPicoseconds() const
{
    return data->serverPicoseconds;
}

/*!
    \since 6.7

    Sets the number of 10 picosecond intervals for the server timestamp to \a serverPicoseconds.
*/
void QOpcUaDataValue::setServerPicoseconds(quint16 serverPicoseconds)
{
    data.detach();
    data->serverPicoseconds = serverPicoseconds;
}

/*!
    \fn bool QOpcUaDataValue::operator!=(const QOpcUaDataValue &lhs, const QOpcUaDataValue &rhs) noexcept
    \since 6.7

    Returns \c true if \a lhs is not equal to \a rhs.
*/

/*!
    \fn bool QOpcUaDataValue::operator==(const QOpcUaDataValue &lhs, const QOpcUaDataValue &rhs) noexcept
    \since 6.7

    Returns \c true if \a lhs is equal to \a rhs.
*/
bool comparesEqual(const QOpcUaDataValue &lhs, const QOpcUaDataValue &rhs) noexcept
{
    return lhs.data->serverTimestamp == rhs.data->serverTimestamp &&
           lhs.data->serverPicoseconds == rhs.data->serverPicoseconds &&
           lhs.data->sourceTimestamp == rhs.data->sourceTimestamp &&
           lhs.data->sourcePicoseconds == rhs.data->sourcePicoseconds &&
           lhs.data->statusCode == rhs.data->statusCode &&
           lhs.data->value == rhs.data->value;
}

QT_END_NAMESPACE

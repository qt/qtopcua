// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuacomplexnumber.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaComplexNumber
    \inmodule QtOpcUa
    \brief The OPC UA ComplexNumber type.

    The ComplexNumberType defined in OPC UA 1.05 part 8, 5.6.4.
    It stores a complex number with float precision.
*/

class QOpcUaComplexNumberData : public QSharedData
{
public:
    float real{0};
    float imaginary{0};
};

/*!
    Default constructs a complex number with no parameters set.
*/
QOpcUaComplexNumber::QOpcUaComplexNumber()
    : data(new QOpcUaComplexNumberData)
{
}

QOpcUaComplexNumber::QOpcUaComplexNumber(const QOpcUaComplexNumber &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this complex number.
*/
QOpcUaComplexNumber &QOpcUaComplexNumber::operator=(const QOpcUaComplexNumber &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUaComplexNumber::~QOpcUaComplexNumber()
{

}

/*!
    Returns the imaginary part of the complex number.
*/
float QOpcUaComplexNumber::imaginary() const
{
    return data->imaginary;
}

/*!
    Sets the imaginary part of the complex number to \a imaginary.
*/
void QOpcUaComplexNumber::setImaginary(float imaginary)
{
    data->imaginary = imaginary;
}

/*!
    Returns the real part of the complex number.
*/
float QOpcUaComplexNumber::real() const
{
    return data->real;
}

/*!
    Sets the real part of the complex number to \a real.
*/
void QOpcUaComplexNumber::setReal(float real)
{
    data->real = real;
}

/*!
    Constructs a complex number with real part \a real and imaginary part \a imaginary.
*/
QOpcUaComplexNumber::QOpcUaComplexNumber(float real, float imaginary)
    : data(new QOpcUaComplexNumberData)
{
    data->real = real;
    data->imaginary = imaginary;
}

/*!
    Returns \c true if this complex number has the same value as \a rhs.
*/
bool QOpcUaComplexNumber::operator==(const QOpcUaComplexNumber &rhs) const
{
    return qFloatDistance(data->real, rhs.real()) == 0 &&
            qFloatDistance(data->imaginary, rhs.imaginary()) == 0;
}

/*!
    Converts this complex number to \l QVariant.
*/
QOpcUaComplexNumber::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QT_END_NAMESPACE

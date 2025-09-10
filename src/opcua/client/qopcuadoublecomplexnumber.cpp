// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuadoublecomplexnumber.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaDoubleComplexNumber
    \inmodule QtOpcUa
    \brief The OPC UA DoubleComplexNumber type.

    The DoubleComplexNumberType defined in OPC UA 1.05 part 8, 5.6.5.
    It stores a complex number with double precision.
*/

class QOpcUaDoubleComplexNumberData : public QSharedData
{
public:
    double real{0};
    double imaginary{0};
};

/*!
    Default constructs a double complex number with no parameters set.
*/
QOpcUaDoubleComplexNumber::QOpcUaDoubleComplexNumber()
    : data(new QOpcUaDoubleComplexNumberData)
{
}

QOpcUaDoubleComplexNumber::QOpcUaDoubleComplexNumber(const QOpcUaDoubleComplexNumber &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this double complex number.
*/
QOpcUaDoubleComplexNumber &QOpcUaDoubleComplexNumber::operator=(const QOpcUaDoubleComplexNumber &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUaDoubleComplexNumber::~QOpcUaDoubleComplexNumber()
{

}

/*!
    Returns the imaginary part of the complex number.
*/
double QOpcUaDoubleComplexNumber::imaginary() const
{
    return data->imaginary;
}

/*!
    Sets the imaginary part of the complex number to \a imaginary.
*/
void QOpcUaDoubleComplexNumber::setImaginary(double imaginary)
{
    data->imaginary = imaginary;
}

/*!
    Returns the real part of the complex number.
*/
double QOpcUaDoubleComplexNumber::real() const
{
    return data->real;
}

/*!
    Sets the real part of the complex number to \a real.
*/
void QOpcUaDoubleComplexNumber::setReal(double real)
{
    data->real = real;
}

/*!
    Constructs a double complex number with real part \a real and imaginary part \a imaginary.
*/
QOpcUaDoubleComplexNumber::QOpcUaDoubleComplexNumber(double real, double imaginary)
    : data(new QOpcUaDoubleComplexNumberData)
{
    data->real = real;
    data->imaginary = imaginary;
}

/*!
    Returns \c true if this double complex number has the same value as \a rhs.
*/
bool QOpcUaDoubleComplexNumber::operator==(const QOpcUaDoubleComplexNumber &rhs) const
{
    return qFloatDistance(data->real, rhs.real()) == 0 &&
            qFloatDistance(data->imaginary, rhs.imaginary()) == 0;
}

/*!
    Converts this double complex number to \l QVariant.
*/
QOpcUaDoubleComplexNumber::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QT_END_NAMESPACE

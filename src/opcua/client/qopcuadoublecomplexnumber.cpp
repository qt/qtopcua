/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
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

#include "qopcuadoublecomplexnumber.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaDoubleComplexNumber
    \inmodule QtOpcUa
    \brief The OPC UA DoubleComplexNumber type.

    The DoubleComplexNumberType defined in OPC-UA part 8, 5.6.5.
    It stores a complex number with double precision.
*/

class QOpcUaDoubleComplexNumberData : public QSharedData
{
public:
    double real{0};
    double imaginary{0};
};

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

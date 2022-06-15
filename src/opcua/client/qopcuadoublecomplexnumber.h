// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUADOUBLECOMPLEXNUMBER_H
#define QOPCUADOUBLECOMPLEXNUMBER_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QOpcUaDoubleComplexNumberData;
class Q_OPCUA_EXPORT QOpcUaDoubleComplexNumber
{
public:
    QOpcUaDoubleComplexNumber();
    QOpcUaDoubleComplexNumber(const QOpcUaDoubleComplexNumber &);
    QOpcUaDoubleComplexNumber(double real, double imaginary);
    QOpcUaDoubleComplexNumber &operator=(const QOpcUaDoubleComplexNumber &);
    bool operator==(const QOpcUaDoubleComplexNumber &rhs) const;
    operator QVariant() const;
    ~QOpcUaDoubleComplexNumber();

    double real() const;
    void setReal(double real);

    double imaginary() const;
    void setImaginary(double imaginary);

private:
    QSharedDataPointer<QOpcUaDoubleComplexNumberData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaDoubleComplexNumber)

#endif // QOPCUADOUBLECOMPLEXNUMBER_H

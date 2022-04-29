/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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

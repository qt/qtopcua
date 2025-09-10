// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUACOMPLEXNUMBER_H
#define QOPCUACOMPLEXNUMBER_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QOpcUaComplexNumberData;
class Q_OPCUA_EXPORT QOpcUaComplexNumber
{
public:
    QOpcUaComplexNumber();
    QOpcUaComplexNumber(const QOpcUaComplexNumber &);
    QOpcUaComplexNumber(float real, float imaginary);
    QOpcUaComplexNumber &operator=(const QOpcUaComplexNumber &);
    bool operator==(const QOpcUaComplexNumber &rhs) const;
    operator QVariant() const;
    ~QOpcUaComplexNumber();

    float real() const;
    void setReal(float real);

    float imaginary() const;
    void setImaginary(float imaginary);

private:
    QSharedDataPointer<QOpcUaComplexNumberData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaComplexNumber)

#endif // QOPCUACOMPLEXNUMBER_H

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

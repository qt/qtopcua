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

#ifndef QOPCUAXVALUE_H
#define QOPCUAXVALUE_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QOpcUaXValueData;
class Q_OPCUA_EXPORT QOpcUaXValue
{
public:
    QOpcUaXValue();
    QOpcUaXValue(const QOpcUaXValue &);
    QOpcUaXValue(double x, float value);
    QOpcUaXValue &operator=(const QOpcUaXValue &);
    bool operator==(const QOpcUaXValue &rhs) const;
    operator QVariant() const;
    ~QOpcUaXValue();

    double x() const;
    void setX(double x);

    float value() const;
    void setValue(float value);

private:
    QSharedDataPointer<QOpcUaXValueData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaXValue)

#endif // QOPCUAXVALUE_H

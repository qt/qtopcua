// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

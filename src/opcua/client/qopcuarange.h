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

#ifndef QOPCUARANGE_H
#define QOPCUARANGE_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QOpcUaRangeData;
class Q_OPCUA_EXPORT QOpcUaRange
{
public:
    QOpcUaRange();
    QOpcUaRange(const QOpcUaRange &);
    QOpcUaRange(double low, double high);
    QOpcUaRange &operator=(const QOpcUaRange &);
    bool operator==(const QOpcUaRange &rhs) const;
    operator QVariant() const;
    ~QOpcUaRange();

    double low() const;
    void setLow(double low);

    double high() const;
    void setHigh(double high);

private:
    QSharedDataPointer<QOpcUaRangeData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaRange)

#endif // QOPCUARANGE_H

// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

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

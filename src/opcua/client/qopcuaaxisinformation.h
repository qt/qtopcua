// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAAXISINFORMATION_H
#define QOPCUAAXISINFORMATION_H

#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaEUInformation;
class QOpcUaRange;
class QOpcUaLocalizedText;

class QOpcUaAxisInformationData;
class Q_OPCUA_EXPORT QOpcUaAxisInformation
{
public:
    QOpcUaAxisInformation();
    QOpcUaAxisInformation(const QOpcUaAxisInformation &);
    QOpcUaAxisInformation(const QOpcUaEUInformation &engineeringUnits, const QOpcUaRange &eURange, const QOpcUaLocalizedText &title,
                          const QOpcUa::AxisScale &axisScaleType, const QList<double> &axisSteps);
    QOpcUaAxisInformation &operator=(const QOpcUaAxisInformation &);
    bool operator==(const QOpcUaAxisInformation &rhs) const;
    operator QVariant() const;
    ~QOpcUaAxisInformation();

    QOpcUaEUInformation engineeringUnits() const;
    void setEngineeringUnits(const QOpcUaEUInformation &engineeringUnits);

    QOpcUaRange eURange() const;
    void setEURange(const QOpcUaRange &eURange);

    QOpcUaLocalizedText title() const;
    void setTitle(const QOpcUaLocalizedText &title);

    QOpcUa::AxisScale axisScaleType() const;
    void setAxisScaleType(QOpcUa::AxisScale axisScaleType);

    QList<double> axisSteps() const;
    void setAxisSteps(const QList<double> &axisSteps);
    QList<double> &axisStepsRef();

private:
    QSharedDataPointer<QOpcUaAxisInformationData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaAxisInformation)

#endif // QOPCUAAXISINFORMATION_H

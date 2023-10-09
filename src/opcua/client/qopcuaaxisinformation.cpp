// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuaaxisinformation.h"
#include "qopcuaeuinformation.h"
#include "qopcuarange.h"
#include "qopcualocalizedtext.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaAxisInformation
    \inmodule QtOpcUa
    \brief The OPC UA AxisInformation type.

    This is the Qt OPC UA representation for the OPC UA AxisInformation type defined in OPC-UA part 8, 5.6.6.
    It contains information about an axis which can be used for multiple purposes. A common use case could
    involve the plotting of display data. The engineering units and the title are used for the text on the plot,
    range, axisScaleType and axisSteps provide the scaling and the axis ranges of the plot.
*/

class QOpcUaAxisInformationData : public QSharedData
{
public:
    QOpcUaEUInformation engineeringUnits;
    QOpcUaRange eURange;
    QOpcUaLocalizedText title;
    QOpcUa::AxisScale axisScaleType{QOpcUa::AxisScale::Linear};
    QList<double> axisSteps;
};

/*!
    Default constructs an axis information with no parameters set.
*/
QOpcUaAxisInformation::QOpcUaAxisInformation()
    : data(new QOpcUaAxisInformationData)
{
}

/*!
    Constructs axis information from \a rhs.
*/
QOpcUaAxisInformation::QOpcUaAxisInformation(const QOpcUaAxisInformation &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this axis information.
*/
QOpcUaAxisInformation &QOpcUaAxisInformation::operator=(const QOpcUaAxisInformation &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUaAxisInformation::~QOpcUaAxisInformation()
{
}

/*!
    Returns the lower and upper values of this axis.
*/
QOpcUaRange QOpcUaAxisInformation::eURange() const
{
    return data->eURange;
}

/*!
    Sets the lower and upper values of this axis to \a eURange.
*/
void QOpcUaAxisInformation::setEURange(const QOpcUaRange &eURange)
{
    data->eURange = eURange;
}

/*!
    Returns the title of this axis.
*/
QOpcUaLocalizedText QOpcUaAxisInformation::title() const
{
    return data->title;
}

/*!
    Sets the title to \a title.
*/
void QOpcUaAxisInformation::setTitle(const QOpcUaLocalizedText &title)
{
    data->title = title;
}

/*!
    Returns the scaling of this axis, defined by \l QOpcUa::AxisScale.
*/
QOpcUa::AxisScale QOpcUaAxisInformation::axisScaleType() const
{
    return data->axisScaleType;
}

/*!
    Sets the axis scale type to \a axisScaleType.
*/
void QOpcUaAxisInformation::setAxisScaleType(QOpcUa::AxisScale axisScaleType)
{
    data->axisScaleType = axisScaleType;
}

/*!
    Returns specific values for each axis step.

    This value is empty if the points are equally distributed and the step size can be
    calculated from the number of steps and the range.
    If the steps are different for each point but constant over a longer time, there is an entry for
    each data point.
*/
QList<double> QOpcUaAxisInformation::axisSteps() const
{
    return data->axisSteps;
}

/*!
    Sets the axis steps to \a axisSteps.
*/
void QOpcUaAxisInformation::setAxisSteps(const QList<double> &axisSteps)
{
    data->axisSteps = axisSteps;
}

/*!
    Returns a reference to the axis steps.
*/
QList<double> &QOpcUaAxisInformation::axisStepsRef()
{
    return data->axisSteps;
}

/*!
    Returns the engineering units of this axis.
*/
QOpcUaEUInformation QOpcUaAxisInformation::engineeringUnits() const
{
    return data->engineeringUnits;
}

/*!
    Sets the engineering units to \a engineeringUnits.
*/
void QOpcUaAxisInformation::setEngineeringUnits(const QOpcUaEUInformation &engineeringUnits)
{
    data->engineeringUnits = engineeringUnits;
}

/*!
    Returns \c true if this axis information has the same value as \a rhs.
*/
bool QOpcUaAxisInformation::operator==(const QOpcUaAxisInformation &rhs) const
{
    return data->axisScaleType == rhs.axisScaleType() &&
            data->axisSteps == rhs.axisSteps() &&
            data->engineeringUnits == rhs.engineeringUnits() &&
            data->eURange == rhs.eURange() &&
            data->title == rhs.title();
}

/*!
    Converts this axis information to \l QVariant.
*/
QOpcUaAxisInformation::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
    Constructs axis information with engineering units \a engineeringUnits,
    range \a eURange, title \a title, scaling \a axisScaleType and axis steps \a axisSteps.
*/
QOpcUaAxisInformation::QOpcUaAxisInformation(const QOpcUaEUInformation &engineeringUnits, const QOpcUaRange &eURange, const QOpcUaLocalizedText &title,
                                             const QOpcUa::AxisScale &axisScaleType, const QList<double> &axisSteps)
    : data (new QOpcUaAxisInformationData)
{
    data->engineeringUnits = engineeringUnits;
    data->eURange = eURange;
    data->title = title;
    data->axisScaleType = axisScaleType;
    data->axisSteps = axisSteps;
}

QT_END_NAMESPACE

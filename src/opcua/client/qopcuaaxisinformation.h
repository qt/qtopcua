/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
                     const QOpcUa::AxisScale &axisScaleType, const QVector<double> &axisSteps);
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

    QVector<double> axisSteps() const;
    void setAxisSteps(const QVector<double> &axisSteps);
    QVector<double> &axisStepsRef();

private:
    QSharedDataPointer<QOpcUaAxisInformationData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaAxisInformation)

#endif // QOPCUAAXISINFORMATION_H

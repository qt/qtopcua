/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
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

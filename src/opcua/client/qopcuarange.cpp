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

#include "qopcuarange.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaRange
    \inmodule QtOpcUa
    \brief The OPC UA Range type.

    This is the Qt OPC UA representation for the OPC UA Range type defined in OPC-UA part 8, 5.6.2.
    It consists of two double values which mark minimum and maximum of the range.
    Ranges are mostly used to store information about acceptable values for a node.
*/

class QOpcUaRangeData : public QSharedData
{
public:
    double low{0};
    double high{0};
};

QOpcUaRange::QOpcUaRange()
    : data(new QOpcUaRangeData)
{
}

/*!
    Constructs a range from \a rhs.
*/
QOpcUaRange::QOpcUaRange(const QOpcUaRange &rhs)
    : data(rhs.data)
{
}

/*!
    Constructs a range with low value \a low and high value \a high.
*/
QOpcUaRange::QOpcUaRange(double low, double high)
    : data(new QOpcUaRangeData)
{
    data->low = low;
    data->high = high;
}

/*!
    Sets the values from \a rhs in this range.
*/
QOpcUaRange &QOpcUaRange::operator=(const QOpcUaRange &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this range has the same value as \a rhs.
*/
bool QOpcUaRange::operator==(const QOpcUaRange &rhs) const
{
    return data->low == rhs.low() &&
            data->high == rhs.high();
}

/*!
    Converts this range to \l QVariant.
*/
QOpcUaRange::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QOpcUaRange::~QOpcUaRange()
{
}

/*!
    Returns the high value of the range.
*/
double QOpcUaRange::high() const
{
    return data->high;
}

/*!
    Sets the high value of the range to \a high.
*/
void QOpcUaRange::setHigh(double high)
{
    data->high = high;
}

/*!
    Returns the low value of the range.
*/
double QOpcUaRange::low() const
{
    return data->low;
}

/*!
    Sets the low value of the range to \a low.
*/
void QOpcUaRange::setLow(double low)
{
    data->low = low;
}

QT_END_NAMESPACE

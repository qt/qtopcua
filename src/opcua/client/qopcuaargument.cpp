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

#include "qopcuaargument.h"
#include "qopcuatype.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaArgument
    \inmodule QtOpcUa
    \brief The OPC UA Argument type.

    This is the Qt OPC UA representation for the Argument type defined in OPC-UA part 3, 8.6.

    The Argument type is mainly used for the values of the InputArguments and OutputArguments properties
    which describe the parameters and return values of method nodes.
*/
class QOpcUaArgumentData : public QSharedData
{
public:
    QString name;
    QString dataTypeId;
    qint32 valueRank{-2};
    QList<quint32> arrayDimensions;
    QOpcUaLocalizedText description;
};

QOpcUaArgument::QOpcUaArgument()
    : data(new QOpcUaArgumentData)
{
}

QOpcUaArgument::QOpcUaArgument(const QOpcUaArgument &rhs)
    : data(rhs.data)
{
}

/*!
    Constructs an argument with name \a name, data type id \a dataTypeId, value rank \a valueRank,
    array dimensions \a arrayDimensions and description \a description.
*/
QOpcUaArgument::QOpcUaArgument(const QString &name, const QString &dataTypeId, qint32 valueRank,
                               const QList<quint32> &arrayDimensions, const QOpcUaLocalizedText &description)
    : data(new QOpcUaArgumentData)
{
    setName(name);
    setDataTypeId(dataTypeId);
    setValueRank(valueRank);
    setArrayDimensions(arrayDimensions);
    setDescription(description);
}

/*!
    Sets the values from \a rhs in this argument.
*/
QOpcUaArgument &QOpcUaArgument::operator=(const QOpcUaArgument &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns true if this argument has the same value as \a other.
*/
bool QOpcUaArgument::operator==(const QOpcUaArgument &other) const
{
    return data->arrayDimensions == other.arrayDimensions() &&
            QOpcUa::nodeIdEquals(data->dataTypeId, other.dataTypeId()) &&
            data->description == other.description() &&
            data->name == other.name() &&
            data->valueRank == other.valueRank();
}

/*!
    Converts this argument to \l QVariant.
*/
QOpcUaArgument::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QOpcUaArgument::~QOpcUaArgument()
{
}

/*!
    Returns the name of the argument.
*/
QString QOpcUaArgument::name() const
{
    return data->name;
}

/*!
    Sets the name of the argument to \a name.
*/
void QOpcUaArgument::setName(const QString &name)
{
    data->name = name;
}

/*!
    Returns the data type node id of the argument.
*/
QString QOpcUaArgument::dataTypeId() const
{
    return data->dataTypeId;
}

/*!
    Sets the data type node id of the argument to \a dataTypeId.
*/
void QOpcUaArgument::setDataTypeId(const QString &dataTypeId)
{
    data->dataTypeId = dataTypeId;
}

/*!
    Returns the value rank of the argument.
    The value rank describes the structure of the value.
    \table
        \header
            \li ValueRank
            \li Meaning
        \row
            \li -3
            \li Scalar or one dimensional array
        \row
            \li -2
            \li Scalar or array with any number of dimensions
        \row
            \li -1
            \li Not an array
        \row
            \li 0
            \li Array with one or more dimensions
        \row
            \li 1
            \li One dimensional array
        \row
            \li >1
            \li Array with n dimensions
    \endtable
*/
qint32 QOpcUaArgument::valueRank() const
{
    return data->valueRank;
}

/*!
    Sets the value rank of the argument to \a valueRank.
*/
void QOpcUaArgument::setValueRank(qint32 valueRank)
{
    data->valueRank = valueRank;
}

/*!
    Returns the array dimensions of the argument.

    The array dimensions describe the length of each array dimension.
*/
QList<quint32> QOpcUaArgument::arrayDimensions() const
{
    return data->arrayDimensions;
}

/*!
    Returns a reference to the array dimensions of the argument.
*/
QList<quint32> &QOpcUaArgument::arrayDimensionsRef()
{
    return data->arrayDimensions;
}

/*!
    Sets the array dimensions of the argument to \a arrayDimensions.
*/
void QOpcUaArgument::setArrayDimensions(const QList<quint32> &arrayDimensions)
{
    data->arrayDimensions = arrayDimensions;
}

/*!
    Returns the description of the argument.
*/
QOpcUaLocalizedText QOpcUaArgument::description() const
{
    return data->description;
}

/*!
    Sets the description of the argument to \a description.
*/
void QOpcUaArgument::setDescription(const QOpcUaLocalizedText &description)
{
    data->description = description;
}

QT_END_NAMESPACE

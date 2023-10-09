// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

/*!
    Default constructs an argument with no parameters set.
*/
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

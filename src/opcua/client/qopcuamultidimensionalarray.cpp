// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuamultidimensionalarray.h"

QT_BEGIN_NAMESPACE

/*
    This class has been modelled in the style of the Variant encoding
    defined in OPC UA 1.05 part 6, 5.2.2.16.

    This solution has been preferred to returning nested QVariantLists
    due to the following reasons:
    - A QVariantList inside a QVariantList is stored as a QVariant which must be converted
      to QVariantList before the elements can be accessed. This makes it impossible to update the
      values in place.
    - The length of the array is encoded as a 32 bit unsigned integer.
      Array dimensions are encoded in an array, so an array can have UINT32_MAX dimensions.
      Depending on the number of dimensions, there could be lots of nested QVariantLists
      which would require a huge effort when calculating the array dimensions for conversions
      between QVariantList and the sdk specific variant type.
*/

/*!
    \class QOpcUaMultiDimensionalArray
    \inmodule QtOpcUa
    \brief A container class for multidimensional arrays.

    This class manages arrays of Qt OPC UA types with associated array dimensions information.
    It is returned as value when a multidimensional array is received from the server. It can also
    be used as a write value or as parameter for filters and method calls.
*/

class QOpcUaMultiDimensionalArrayData : public QSharedData
{
public:
    QVariantList value;
    QList<quint32> arrayDimensions;
    quint32 expectedArrayLength{0};
};

/*!
    Default constructs a multi dimensional array with no parameters set.
*/
QOpcUaMultiDimensionalArray::QOpcUaMultiDimensionalArray()
    : data(new QOpcUaMultiDimensionalArrayData)
{
}

/*!
    Constructs a multidimensional array from \a other.
*/
QOpcUaMultiDimensionalArray::QOpcUaMultiDimensionalArray(const QOpcUaMultiDimensionalArray &other)
    : data(other.data)
{
}

/*!
    Sets the values from \a rhs in the multidimensional array.
*/
QOpcUaMultiDimensionalArray &QOpcUaMultiDimensionalArray::operator=(const QOpcUaMultiDimensionalArray &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Constructs a multidimensional array with value \a value and array dimensions \a arrayDimensions.
*/
QOpcUaMultiDimensionalArray::QOpcUaMultiDimensionalArray(const QVariantList &value, const QList<quint32> &arrayDimensions)
    : data(new QOpcUaMultiDimensionalArrayData)
{
    setValueArray(value);
    setArrayDimensions(arrayDimensions);
}

/*!
    Creates a multidimensional array with preallocated data fitting \a arrayDimensions.
*/
QOpcUaMultiDimensionalArray::QOpcUaMultiDimensionalArray(const QList<quint32> &arrayDimensions)
    : data(new QOpcUaMultiDimensionalArrayData)
{
    setArrayDimensions(arrayDimensions);
    if (data->expectedArrayLength) {
        data->value.reserve(data->expectedArrayLength);
        for (size_t i = 0; i < data->expectedArrayLength; ++i)
            data->value.append(QVariant());
    }
}

QOpcUaMultiDimensionalArray::~QOpcUaMultiDimensionalArray()
{
}

/*!
    Returns the dimensions of the multidimensional array.
    The element at position n contains the length of the n-th dimension.
*/
QList<quint32> QOpcUaMultiDimensionalArray::arrayDimensions() const
{
    return data->arrayDimensions;
}

/*!
    Sets the dimensions of the multidimensional array to \a arrayDimensions.
*/
void QOpcUaMultiDimensionalArray::setArrayDimensions(const QList<quint32> &arrayDimensions)
{
    data->arrayDimensions = arrayDimensions;
    data->expectedArrayLength = std::accumulate(data->arrayDimensions.begin(), data->arrayDimensions.end(),
                                                1, std::multiplies<quint32>());
}

/*!
    Returns \c true if this multidimensional array has the same value as \a other.
*/
bool QOpcUaMultiDimensionalArray::operator==(const QOpcUaMultiDimensionalArray &other) const
{
    return arrayDimensions() == other.arrayDimensions() &&
            valueArray() == other.valueArray();
}

/*!
    Converts this multidimensional array to \l QVariant.
*/
QOpcUaMultiDimensionalArray::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
    Returns the value array of the multidimensional array.
*/
QVariantList QOpcUaMultiDimensionalArray::valueArray() const
{
    return data->value;
}

/*!
    Returns a reference to the value array of the multidimensional array.
*/
QVariantList &QOpcUaMultiDimensionalArray::valueArrayRef()
{
    return data->value;
}

/*!
    Sets the value array of the multidimensional array to \a value.
*/
void QOpcUaMultiDimensionalArray::setValueArray(const QVariantList &value)
{
    data->value = value;
}

/*!
    Returns the array index in \l valueArray() of the element identified by \a indices.
    If \a indices is invalid for the array or if the array's dimensions don't match
    the size of \l valueArray(), the invalid index \c -1 is returned.
*/
int QOpcUaMultiDimensionalArray::arrayIndex(const QList<quint32> &indices) const
{
    // A QList can store INT_MAX values. Depending on the platform, this allows a size > UINT32_MAX
    if (data->expectedArrayLength > static_cast<quint64>((std::numeric_limits<int>::max)()) ||
            static_cast<quint64>(data->value.size()) > (std::numeric_limits<quint32>::max)())
        return -1;

    // Check number of dimensions and data size
    if (indices.size() != data->arrayDimensions.size() ||
            data->expectedArrayLength != static_cast<quint32>(data->value.size()))
        return -1; // Missing array dimensions or array dimensions don't fit the array

    quint32 index = 0;
    quint32 stride = 1;
    // Reverse iteration to avoid repetitions while calculating the stride
    for (int i = data->arrayDimensions.size() - 1; i >= 0; --i) {
        if (indices.at(i) >= data->arrayDimensions.at(i)) // Out of bounds
            return -1;

        // Arrays are encoded in row-major order: [0,0,0], [0,0,1], [0,1,0], [0,1,1], [1,0,0], [1,0,1], [1,1,0], [1,1,1]
        // The stride for dimension i in a n dimensional array is the product of all array dimensions from i+1 to n
        if (i < data->arrayDimensions.size() - 1)
            stride *= data->arrayDimensions.at(i + 1);
        index += stride * indices.at(i);
    }

    return (index <= static_cast<quint64>((std::numeric_limits<int>::max)())) ?
                static_cast<int>(index) : -1;
}

/*!
    Returns the value of the element identified by \a indices.
    If the indices are invalid for the array, an empty \l QVariant is returned.
*/
QVariant QOpcUaMultiDimensionalArray::value(const QList<quint32> &indices) const
{
    int index = arrayIndex(indices);

    if (index < 0)
        return QVariant();

    return data->value.at(index);
}

/*!
    Sets the value at position \a indices to \a value.
    Returns \c true if the value has been successfully set.
*/
bool QOpcUaMultiDimensionalArray::setValue(const QList<quint32> &indices, const QVariant &value)
{
    int index = arrayIndex(indices);

    if (index < 0)
        return false;

    data->value[index] = value;
    return true;
}

/*!
    Returns \c true if the multidimensional array is valid
*/
bool QOpcUaMultiDimensionalArray::isValid() const
{
    return static_cast<quint64>(data->value.size()) == data->expectedArrayLength &&
            static_cast<quint64>(data->value.size()) <= (std::numeric_limits<quint32>::max)() &&
            static_cast<quint64>(data->arrayDimensions.size()) <= (std::numeric_limits<quint32>::max)();
}

QT_END_NAMESPACE

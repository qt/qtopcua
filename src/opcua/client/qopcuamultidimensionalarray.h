// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAMULTIDIMENSIONALARRAY_H
#define QOPCUAMULTIDIMENSIONALARRAY_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>
#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

class QOpcUaMultiDimensionalArrayData;
class Q_OPCUA_EXPORT QOpcUaMultiDimensionalArray
{
public:
    QOpcUaMultiDimensionalArray();
    QOpcUaMultiDimensionalArray(const QOpcUaMultiDimensionalArray &other);
    QOpcUaMultiDimensionalArray &operator=(const QOpcUaMultiDimensionalArray &rhs);
    QOpcUaMultiDimensionalArray(const QVariantList &valueArray, const QList<quint32> &arrayDimensions);
    QOpcUaMultiDimensionalArray(const QList<quint32> &arrayDimensions);
    ~QOpcUaMultiDimensionalArray();

    QVariantList valueArray() const;
    QVariantList &valueArrayRef();
    void setValueArray(const QVariantList &valueArray);

    int arrayIndex(const QList<quint32> &indices) const;
    QVariant value(const QList<quint32> &indices) const;
    bool setValue(const QList<quint32> &indices, const QVariant &value);

    bool isValid() const;

    QList<quint32> arrayDimensions() const;
    void setArrayDimensions(const QList<quint32> &arrayDimensions);

    bool operator==(const QOpcUaMultiDimensionalArray &other) const;

    operator QVariant() const;

private:
    QSharedDataPointer<QOpcUaMultiDimensionalArrayData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaMultiDimensionalArray)

#endif // QOPCUAMULTIDIMENSIONALARRAY_H

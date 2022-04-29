/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#ifndef QOPCUAMULTIDIMENSIONALARRAY_H
#define QOPCUAMULTIDIMENSIONALARRAY_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

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

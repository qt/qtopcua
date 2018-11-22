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
    QOpcUaMultiDimensionalArray(const QVariantList &valueArray, const QVector<quint32> &arrayDimensions);
    QOpcUaMultiDimensionalArray(const QVector<quint32> &arrayDimensions);
    ~QOpcUaMultiDimensionalArray();

    QVariantList valueArray() const;
    QVariantList &valueArrayRef();
    void setValueArray(const QVariantList &valueArray);

    int arrayIndex(const QVector<quint32> &indices) const;
    QVariant value(const QVector<quint32> &indices) const;
    bool setValue(const QVector<quint32> &indices, const QVariant &value);

    bool isValid() const;

    QVector<quint32> arrayDimensions() const;
    void setArrayDimensions(const QVector<quint32> &arrayDimensions);

    bool operator==(const QOpcUaMultiDimensionalArray &other) const;

    operator QVariant() const;

private:
    QSharedDataPointer<QOpcUaMultiDimensionalArrayData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaMultiDimensionalArray)

#endif // QOPCUAMULTIDIMENSIONALARRAY_H

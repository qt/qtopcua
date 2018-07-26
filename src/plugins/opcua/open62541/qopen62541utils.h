/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
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

#ifndef QOPEN62541UTILS_H
#define QOPEN62541UTILS_H

#include "qopen62541.h"

#include <QtCore/qstring.h>

#include <functional>

QT_BEGIN_NAMESPACE

template <typename T>
class UaDeleter
{
public:
    UaDeleter(T *data, std::function<void(T *value)> f)
        : m_data(data)
        , m_function(f)
    {
    }
    ~UaDeleter()
    {
        if (m_data)
            m_function(m_data);
    }
private:
    T *m_data {nullptr};
    std::function<void(T *attribute)> m_function;
};

template <uint TYPEINDEX>
class UaArrayDeleter
{
public:
    UaArrayDeleter(void *data, size_t arrayLength)
        : m_data(data)
        , m_arrayLength(arrayLength)
    {
        static_assert (TYPEINDEX < UA_TYPES_COUNT, "Invalid index outside the UA_TYPES array.");
    }
    ~UaArrayDeleter()
    {
        if (m_data && m_arrayLength > 0)
            UA_Array_delete(m_data, m_arrayLength, &UA_TYPES[TYPEINDEX]);
    }
private:
    void *m_data {nullptr};
    size_t m_arrayLength {0};
};

namespace Open62541Utils {
    UA_NodeId nodeIdFromQString(const QString &name);
    QString nodeIdToQString(UA_NodeId id);
}

QT_END_NAMESPACE

#endif // QOPEN62541UTILS_H

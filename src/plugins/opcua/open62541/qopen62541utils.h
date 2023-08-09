// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPEN62541UTILS_H
#define QOPEN62541UTILS_H

#include "qopen62541.h"

#include <QString>

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
    void release()
    {
        m_data = nullptr;
        m_function = nullptr;
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
    void release() {
        m_data = nullptr;
        m_arrayLength = 0;
    }
private:
    void *m_data {nullptr};
    size_t m_arrayLength {0};
};

namespace Open62541Utils {
    UA_NodeId nodeIdFromQString(const QString &name);
    QString nodeIdToQString(UA_NodeId id);

#ifdef UA_ENABLE_ENCRYPTION
    bool checkSha1SignatureSupport();
#endif
}

QT_END_NAMESPACE

#endif // QOPEN62541UTILS_H

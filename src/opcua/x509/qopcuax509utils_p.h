// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QOPCUAX509UTILS_H
#define QOPCUAX509UTILS_H

#include <functional>
#include <QString>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

template <typename T>
class Deleter
{
public:
    Deleter(T *data, std::function<void(T *value)> f)
        : m_data(data)
        , m_function(f)
    {
    }
    ~Deleter()
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

QString getOpenSslError();

QT_END_NAMESPACE

#endif // QOPCUAX509UTILS_H

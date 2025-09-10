// Copyright (C) 2019 The Qt Company Ltd.
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

#include "qopcuax509utils_p.h"
#include "openssl_symbols_p.h"

QT_BEGIN_NAMESPACE

QString getOpenSslError()
{
    char errorBuf[1024];
    errorBuf[0] = 0;
    q_ERR_error_string_n(q_ERR_get_error(), errorBuf, sizeof(errorBuf));
    return QString::fromLatin1(errorBuf);
}

QT_END_NAMESPACE


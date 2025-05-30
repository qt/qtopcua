// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QUACPPUTILS_H
#define QUACPPUTILS_H

#include <QtCore/qstring.h>

#include <uanodeid.h>

QT_BEGIN_NAMESPACE

namespace UACppUtils {
    UaNodeId nodeIdFromQString(const QString &name);
    QString nodeIdToQString(const UaNodeId &id);
}

QT_END_NAMESPACE

#endif // QUACPPUTILS_H

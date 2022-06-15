// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAX509EXTENSION_P_H
#define QOPCUAX509EXTENSION_P_H

#include <QtCore/qshareddata.h>
#include <QtCore/private/qglobal_p.h>

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

QT_BEGIN_NAMESPACE

class QOpcUaX509ExtensionData : public QSharedData
{
public:
    virtual ~QOpcUaX509ExtensionData() = default;
    bool critical = false;
};

QT_END_NAMESPACE

#endif // QOPCUAX509EXTENSION_P_H

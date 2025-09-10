// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma once

#include <QtCore/qstring.h>

namespace Util
{
    QString indent(int level);
    QString lineBreak(int n = 1);

    QString removeNamespace(const QString &typeName);
    QString lowerFirstLetter(const QString &temp);
}

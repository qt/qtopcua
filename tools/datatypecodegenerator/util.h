// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#pragma once

#include <QtCore/qstring.h>

namespace Util
{
    QString indent(int level);
    QString lineBreak(int n = 1);

    QString removeNamespace(const QString &typeName);
    QString lowerFirstLetter(const QString &temp);
}

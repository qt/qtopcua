// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "util.h"

#include <QtCore/qstringlist.h>

QString Util::indent(int level)
{
    return QStringLiteral("%1").arg(" ", level * 4, ' ');
}

QString Util::lineBreak(int n)
{
    return QStringLiteral("%1").arg("\n", n, '\n');
}

QString Util::removeNamespace(const QString &typeName)
{
    return typeName.split(":").value(1, QString());
}

QString Util::lowerFirstLetter(const QString &temp)
{
    if (temp.isEmpty())
        return temp;

    auto result = temp;
    result.front() = result.at(0).toLower();
    return result;
}

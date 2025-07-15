// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "util.h"

#include <QtCore/qstringlist.h>

using namespace Qt::Literals::StringLiterals;

QString Util::indent(int level)
{
    return u"%1"_s.arg(' '_L1, level * 4, ' '_L1);
}

QString Util::lineBreak(int n)
{
    return u"%1"_s.arg('\n'_L1, n, '\n'_L1);
}

QString Util::removeNamespace(const QString &typeName)
{
    return typeName.split(':'_L1).value(1, QString());
}

QString Util::lowerFirstLetter(const QString &temp)
{
    if (temp.isEmpty())
        return temp;

    auto result = temp;
    result.front() = result.at(0).toLower();
    return result;
}

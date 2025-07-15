// Copyright (C) 2024 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "nodeidgenerator.h"

#include <util.h>

#include <QDebug>
#include <QDir>
#include <QFile>

using namespace Qt::Literals::StringLiterals;

bool NodeIdGenerator::parseNodeIds(const QString &name, const QString &path)
{
    if (m_nodeIds.contains(name)) {
        qWarning() << "Duplicate NodeId map name:" << name;
        return false;
    }

    QList<NodeId> result;

    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open" << path << "for reading";
        return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        const auto line = in.readLine();
        const auto split = line.split(QLatin1Char(','));

        if (split.length() != 3 || split.at(0).isEmpty() || split.at(1).isEmpty()) {
            qWarning() << "Skip line" << line << "in file" << path;
            continue;
        }

        result.push_back({ split.at(0), split.at(1), split.at(2) });
    }

    if (result.empty()) {
        qWarning() << "No NodeIds in file" << path;
        return false;
    }

    m_nodeIds[name] = result;

    return true;
}

bool NodeIdGenerator::hasNodeIds() const
{
    return !m_nodeIds.empty();
}

bool NodeIdGenerator::generateNodeIdsHeader(const QString &prefix, const QString &path, const QString &header)
{
    if (!hasNodeIds()) {
        qWarning() << "Unable to generate node id files, no known node ids";
        return false;
    }

    const auto name = u"%1nodeids.h"_s.arg(prefix.toLower());

    QDir dir(path);
    if (!dir.mkpath(u"."_s)) {
        qWarning() << "Failed to create directory" << path << "for writing";
        return false;
    }

    QFile file(dir.absoluteFilePath(name));

    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        qWarning() << "Failed to open file for writing:" << path << file.errorString();
        return false;
    }

    QTextStream out(&file);

    out << header << Util::lineBreak(2);

    out << "#pragma once" << Util::lineBreak(2);

    for (auto it = m_nodeIds.constBegin(); it != m_nodeIds.constEnd();) {
        const auto enumName = u"%1NodeId"_s.arg(it.key());
        out << "enum class " << enumName << " {" << Util::lineBreak();

        out << Util::indent(1) << "Unknown = 0," << Util::lineBreak();

        for (const auto &id : it.value()) {
            out << Util::indent(1) << id.name << " = " << id.value << ",";
            if (!id.type.isEmpty())
                out << " // " << id.type;
            out << Util::lineBreak();
        }

        out << "};" << Util::lineBreak(++it == m_nodeIds.constEnd() ? 1 : 2 );
    }

    return true;
}

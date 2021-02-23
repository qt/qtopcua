// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "import.h"
#include "visitor.h"

#include <QtCore/qdebug.h>

Import::Import(const QString &nameSpace, const QString &location)
    : m_location(location)
    , m_nameSpace(nameSpace)
{}

QString Import::nameSpace() const
{
    return m_nameSpace;
}

void Import::setNameSpace(const QString &nameSpace)
{
    m_nameSpace = nameSpace;
}

QString Import::location() const
{
    return m_location;
}

void Import::setLocation(const QString &location)
{
    m_location = location;
}

void Import::print() const
{
    qDebug() << "Namespace:" << m_nameSpace << "\tLocation:" << m_location;
}

void Import::accept(Visitor *visitor)
{
    visitor->visit(this);
}

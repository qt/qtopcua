// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include "xmlelement.h"

#include "util.h"

#include <QtCore/qdebug.h>

XmlElement::XmlElement(const QString &name)
    : m_name(name)
{}

void XmlElement::setName(const QString &name)
{
    m_name = name;
}

QString XmlElement::name() const
{
    return m_name;
}

QString XmlElement::lowerFirstName() const
{
    return Util::lowerFirstLetter(m_name);
}

void XmlElement::print() const
{
    qDebug() << "name: " << m_name;
}

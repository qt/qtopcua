// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "enumeratedvalue.h"
#include "visitor.h"

#include <QtCore/qdebug.h>

EnumeratedValue::EnumeratedValue(const QString &name, qint32 value)
    : XmlElement(name)
    , m_value(value)
{}

void EnumeratedValue::print() const
{
    XmlElement::print();
    qDebug() << "Value: " << m_value;
}

void EnumeratedValue::accept(Visitor *visitor)
{
    visitor->visit(this);
}

qint32 EnumeratedValue::value() const
{
    return m_value;
}

void EnumeratedValue::setValue(qint32 value)
{
    m_value = value;
}

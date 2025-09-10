// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "enumeratedtype.h"
#include "enumeratedvalue.h"
#include "visitor.h"

#include <QtCore/qdebug.h>

EnumeratedType::EnumeratedType(const QString &name, quint32 lengthInBits)
    : XmlElement(name)
    , m_lengthInBits(lengthInBits)
{}

EnumeratedType::~EnumeratedType()
{
    qDeleteAll(m_values);
}

quint32 EnumeratedType::lengthInBits() const
{
    return m_lengthInBits;
}

void EnumeratedType::setLengthInBits(quint32 lengthInBits)
{
    m_lengthInBits = lengthInBits;
}

void EnumeratedType::addValue(EnumeratedValue *enumeratedValue)
{
    m_values.push_back(enumeratedValue);
}

void EnumeratedType::print() const
{
    XmlElement::print();
    qDebug() << "LengthInBits: " << m_lengthInBits;
    for (int i = 0; i < m_values.size(); i++)
        m_values.at(i)->print();
}

void EnumeratedType::accept(Visitor *visitor)
{
    visitor->visit(this);
    for (auto &enumeratedValues : m_values)
        enumeratedValues->accept(visitor);
}

QList<EnumeratedValue *> EnumeratedType::values() const
{
    return m_values;
}

void EnumeratedType::setValues(const QList<EnumeratedValue *> &values)
{
    m_values = values;
}

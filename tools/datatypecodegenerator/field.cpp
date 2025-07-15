// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "field.h"
#include "visitor.h"

#include <QtCore/qdebug.h>
#include <QtCore/qstringlist.h>

using namespace Qt::Literals::StringLiterals;

Field::Field(const QString &name, const QString &typeName, const QString &lengthField)
    : XmlElement(name)
    , m_lengthField(lengthField)
    , m_typeName(typeName)
{}

QString Field::typeName() const
{
    return m_typeName;
}

QString Field::typeNameSecondPart() const
{
    return m_typeName.split(':'_L1).value(1, QString());
}

void Field::setTypeName(const QString &typeName)
{
    m_typeName = typeName;
}

QString Field::lengthField() const
{
    return m_lengthField;
}

void Field::setLengthField(const QString &lengthField)
{
    m_lengthField = lengthField;
}

bool Field::hasLengthField() const
{
    return m_hasLengthField;
}

void Field::setHasLengthField(bool hasLengthField)
{
    m_hasLengthField = hasLengthField;
}

bool Field::needContainer() const
{
    return m_needContainer;
}

void Field::setNeedContainer(bool needContainer)
{
    m_needContainer = needContainer;
}

bool Field::isInStructuredTypeBitMask() const
{
    return m_isInStructuredTypeBitMask;
}

void Field::setIsInStructuredTypeBitMask(bool isInStructuredTypeBitMask)
{
    m_isInStructuredTypeBitMask = isInStructuredTypeBitMask;
}

int Field::length() const
{
    return m_length;
}

void Field::setLength(int length)
{
    m_length = length;
}

QString Field::switchField() const
{
    return m_switchField;
}

void Field::setSwitchField(const QString &switchField)
{
    m_switchField = switchField;
}

int Field::switchValue() const
{
    return m_switchValue;
}

void Field::setSwitchValue(int switchValue)
{
    m_switchValue = switchValue;
}

bool Field::isUnion() const
{
    return m_isUnion;
}

void Field::setIsUnion(bool isUnion)
{
    m_isUnion = isUnion;
}

bool Field::recursive() const
{
    return m_recursive;
}

void Field::setRecursive(bool recursive)
{
    m_recursive = recursive;
}

bool Field::isEnum() const
{
    return m_isEnum;
}

void Field::setIsEnum(bool newIsEnum)
{
    m_isEnum = newIsEnum;
}

void Field::print() const
{
    XmlElement::print();
    qDebug() << "Type name: " << m_typeName;
    if (!m_lengthField.isEmpty())
        qDebug() << "Length field: " << m_lengthField;
}

void Field::accept(Visitor *visitor)
{
    visitor->visit(this);
}

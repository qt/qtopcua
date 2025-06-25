// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "field.h"
#include "structuredtype.h"
#include "visitor.h"

#include <QtCore/qdebug.h>

StructuredType::StructuredType(const QString &name, const QString &baseType)
    : XmlElement(name)
    , m_baseType(baseType)
{}

StructuredType::~StructuredType()
{
    qDeleteAll(m_fields);
}

QString StructuredType::baseType() const
{
    return m_baseType;
}

void StructuredType::setBaseType(const QString &baseType)
{
    m_baseType = baseType;
}

void StructuredType::addField(Field *field)
{
    m_fields.push_back(field);
}

void StructuredType::print() const
{
    XmlElement::print();
    qDebug() << "BaseType: " << m_baseType;
    for (int i = 0; i < m_fields.size(); i++)
        m_fields.at(i)->print();
}

void StructuredType::accept(Visitor *visitor)
{
    visitor->visit(this);
    for (const auto &field : std::as_const(m_fields))
        field->accept(visitor);
}

QList<Field *> StructuredType::fields() const
{
    return m_fields;
}

void StructuredType::setFields(const QList<Field *> &fields)
{
    m_fields = fields;
}

bool StructuredType::containsBitMask() const
{
    return m_containsBitMask;
}

void StructuredType::setContainsBitMask(bool containsBitMask)
{
    m_containsBitMask = containsBitMask;
}

bool StructuredType::isBuiltInType() const
{
    return m_isBuiltInType;
}

void StructuredType::setIsBuiltInType(bool isBuiltInType)
{
    m_isBuiltInType = isBuiltInType;
}

bool StructuredType::hasSwitchfield() const
{
    return m_hasSwitchfield;
}

void StructuredType::setHasSwitchfield(bool hasSwitchfield)
{
    m_hasSwitchfield = hasSwitchfield;
}

bool StructuredType::hasUnion() const
{
    return m_hasUnion;
}

void StructuredType::setHasUnion(bool hasUnion)
{
    m_hasUnion = hasUnion;
}

QString StructuredType::targetNamespace() const
{
    return m_targetNamespace;
}

void StructuredType::setTargetNamespace(const QString &targetNamespace)
{
    m_targetNamespace = targetNamespace;
}

bool StructuredType::recursive() const
{
    return m_recursive;
}

void StructuredType::setRecursive(bool recursive)
{
    m_recursive = recursive;
}

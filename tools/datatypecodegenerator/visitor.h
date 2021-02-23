// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma once

#include "xmlelement.h"

class EnumeratedType;
class EnumeratedValue;
class Import;
class Field;
class StructuredType;
class TypeDictionary;

class Visitor
{
public:
    virtual ~Visitor() = default;
    virtual void visit(XmlElement *xmlElement) = 0;
    virtual void visit(EnumeratedType *enumteratedType) = 0;
    virtual void visit(EnumeratedValue *enumeratedValue) = 0;
    virtual void visit(Import *import) = 0;
    virtual void visit(Field *field) = 0;
    virtual void visit(StructuredType *structuredType) = 0;
    virtual void visit(TypeDictionary *typeDictionary) = 0;
};

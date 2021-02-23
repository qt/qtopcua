// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma once

#include "visitor.h"

#include <QtCore/qlist.h>

class XmlElement;

class DependencyDataTypeValidator : public Visitor
{
public:
    enum ReadResolveDependencies { ReadDependencies, ResolveDependencies };

    DependencyDataTypeValidator();
    ~DependencyDataTypeValidator() override = default;

    void visit(EnumeratedType *enumeratedType) override;
    void visit(EnumeratedValue *enumeratedValue) override;
    void visit(Field *field) override;
    void visit(Import *import) override;
    void visit(StructuredType *structuredType) override;
    void visit(TypeDictionary *typeDictionary) override;
    void visit(XmlElement *xmlElement) override;

    QStringList unresolvedDependencyStringList() const;

    QList<XmlElement *> resolvedDependencyElementList() const;

    void setReadResolveDependencies(const ReadResolveDependencies &readResolveDependencies);

private:
    ReadResolveDependencies m_readResolveDependencies;
    QList<XmlElement *> m_resolvedDependencyElementList;
    QStringList m_unresolvedDependencyStringList;
};

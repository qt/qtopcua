// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "dependencydatatypevalidator.h"
#include "enumeratedtype.h"
#include "field.h"
#include "stringidentifier.h"
#include "structuredtype.h"

using namespace Qt::Literals::StringLiterals;

DependencyDataTypeValidator::DependencyDataTypeValidator()
    : m_readResolveDependencies(ReadDependencies)
{}

void DependencyDataTypeValidator::visit(EnumeratedType *enumeratedType)
{
    if (m_readResolveDependencies == DependencyDataTypeValidator::ResolveDependencies) {
        if (m_unresolvedDependencyStringList.contains(enumeratedType->name())) {
            m_unresolvedDependencyStringList.removeAll(enumeratedType->name());
            m_resolvedDependencyElementList.push_back(enumeratedType);
        }
    }
}

void DependencyDataTypeValidator::visit(EnumeratedValue *enumeratedValue)
{
    Q_UNUSED(enumeratedValue);
}

void DependencyDataTypeValidator::visit(Field *field)
{
    if (m_readResolveDependencies == DependencyDataTypeValidator::ReadDependencies) {
        if (!field->typeName().contains("opc:"_L1)) {
            const auto typeName = field->typeNameSecondPart();
            for (const auto &precoded : StringIdentifier::opcUaPrecodedTypes) {
                if (precoded.contains(typeName)) {
                    return;
                }
            }
            m_unresolvedDependencyStringList.push_back(typeName);
        }
    }
}

void DependencyDataTypeValidator::visit(Import *import)
{
    Q_UNUSED(import);
}

void DependencyDataTypeValidator::visit(StructuredType *structuredType)
{
    if (m_readResolveDependencies == DependencyDataTypeValidator::ResolveDependencies) {
        if (m_unresolvedDependencyStringList.contains(structuredType->name())) {
            m_unresolvedDependencyStringList.removeAll(structuredType->name());
            m_resolvedDependencyElementList.push_back(structuredType);
            const auto tempFields = structuredType->fields();
            for (const auto &field : tempFields) {
                const auto typeName = field->typeNameSecondPart();

                if (!StringIdentifier::typeNameDataTypeConverter.contains(field->typeName())) {
                    bool isPrecoded = false;
                    for (const auto &precoded : StringIdentifier::opcUaPrecodedTypes) {
                        if (precoded.contains(typeName)) {
                            isPrecoded = true;
                            break;
                        }
                    }
                    if (!isPrecoded && !m_unresolvedDependencyStringList.contains(typeName)) {
                        bool isResolved = false;
                        for (const auto &type : std::as_const(m_resolvedDependencyElementList)) {
                            if (type->name() == typeName) {
                                isResolved = true;
                                break;
                            }
                        }
                        if (!isResolved)
                            m_unresolvedDependencyStringList.push_back(typeName);
                    }
                }
            }
        }
    }
}

void DependencyDataTypeValidator::visit(TypeDictionary *typeDictionary)
{
    Q_UNUSED(typeDictionary);
}

void DependencyDataTypeValidator::visit(XmlElement *xmlElement)
{
    Q_UNUSED(xmlElement);
}

QStringList DependencyDataTypeValidator::unresolvedDependencyStringList() const
{
    return m_unresolvedDependencyStringList;
}

QList<XmlElement *> DependencyDataTypeValidator::resolvedDependencyElementList() const
{
    return m_resolvedDependencyElementList;
}

void DependencyDataTypeValidator::setReadResolveDependencies(
    const ReadResolveDependencies &readResolveDependencies)
{
    m_readResolveDependencies = readResolveDependencies;
}

// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "typedictionary.h"

#include "enumeratedtype.h"
#include "import.h"
#include "structuredtype.h"
#include "visitor.h"

#include <QtCore/qdebug.h>

TypeDictionary::TypeDictionary(bool dependencyTypeDictionary,
                               const QString &defaultByOrder,
                               const QString &targetNamespace,
                               const QMap<QString, QString> &namespaces)
    : m_dependencyTypeDictionary(dependencyTypeDictionary)
    , m_defaultByOrder(defaultByOrder)
    , m_targetNamespace(targetNamespace)
    , m_namespaces(namespaces)
{}

TypeDictionary::~TypeDictionary()
{
    qDeleteAll(m_types);
}

QMap<QString, QString> TypeDictionary::namespaces() const
{
    return m_namespaces;
}

void TypeDictionary::setNamespaces(const QMap<QString, QString> &namespaces)
{
    m_namespaces = namespaces;
}

QString TypeDictionary::defaultByOrder() const
{
    return m_defaultByOrder;
}

void TypeDictionary::setDefaultByOrder(const QString &defaultByOrder)
{
    m_defaultByOrder = defaultByOrder;
}

QString TypeDictionary::targetNamespace() const
{
    return m_targetNamespace;
}

void TypeDictionary::setTargetNamespace(const QString &targetNamespace)
{
    m_targetNamespace = targetNamespace;
}

QList<const Import *> TypeDictionary::imports() const
{
    QList<const Import *> imports;
    for (auto element : m_types) {
        Import *import = dynamic_cast<Import *>(element);
        if (import) {
            imports.push_back(import);
        }
    }
    return imports;
}

bool TypeDictionary::dependencyTypeDictionary() const
{
    return m_dependencyTypeDictionary;
}

QList<XmlElement *> TypeDictionary::types() const
{
    return m_types;
}

void TypeDictionary::addType(XmlElement *type)
{
    m_types.push_back(type);
}

void TypeDictionary::print() const
{
    for (auto it = m_namespaces.constBegin(); it != m_namespaces.constEnd(); ++it) {
        qDebug() << it.key() << ": " << it.value();
    }

    qDebug() << "defaultByOrder:" << m_defaultByOrder << "\tTargetNamespace:" << m_targetNamespace;

    for (auto element : m_types) {
        const auto *enumeratedType = dynamic_cast<EnumeratedType *>(element);
        if (enumeratedType) {
            enumeratedType->print();
        } else {
            const auto *structuredType = dynamic_cast<StructuredType *>(element);
            if (structuredType) {
                structuredType->print();
            } else {
                const auto *import = dynamic_cast<Import *>(element);
                if (import) {
                    import->print();
                }
            }
        }
    }
}

void TypeDictionary::accept(Visitor *visitor)
{
    visitor->visit(this);
    for (const auto &element : std::as_const(m_types))
        element->accept(visitor);
}

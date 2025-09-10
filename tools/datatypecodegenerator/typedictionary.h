// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma once

#include <QtCore/qmap.h>

class Import;
class Visitor;
class XmlElement;

class TypeDictionary
{
public:
    TypeDictionary(bool dependencyTypeDictionary,
                   const QString &defaultByOrder,
                   const QString &targetNamespace,
                   const QMap<QString, QString> &namespaces);
    ~TypeDictionary();

    void addType(XmlElement *type);
    void print() const;
    void accept(Visitor *visitor);

    QMap<QString, QString> namespaces() const;
    void setNamespaces(const QMap<QString, QString> &namespaces);

    QString defaultByOrder() const;
    void setDefaultByOrder(const QString &defaultByOrder);

    QString targetNamespace() const;
    void setTargetNamespace(const QString &targetNamespace);

    QList<const Import *> imports() const;

    bool dependencyTypeDictionary() const;

    QList<XmlElement *> types() const;

private:
    bool m_dependencyTypeDictionary;
    QString m_defaultByOrder;
    QString m_targetNamespace;
    QMap<QString, QString> m_namespaces;
    QList<XmlElement *> m_types;
};

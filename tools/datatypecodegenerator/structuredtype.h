// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma once

#include "xmlelement.h"

#include <QtCore/qlist.h>

class Field;

class StructuredType : public XmlElement
{
public:
    StructuredType(const QString &name, const QString &baseType);
    ~StructuredType();

    QString baseType() const;
    void setBaseType(const QString &baseType);

    void addField(Field *field);

    void print() const override;
    void accept(Visitor *visitor) override;

    QList<Field *> fields() const;
    void setFields(const QList<Field *> &fields);

    bool containsBitMask() const;
    void setContainsBitMask(bool containsBitMask);

    bool isBuiltInType() const;
    void setIsBuiltInType(bool isBuiltInType);

    bool hasSwitchfield() const;
    void setHasSwitchfield(bool hasSwitchfield);

    bool hasUnion() const;
    void setHasUnion(bool hasUnion);

    QString targetNamespace() const;
    void setTargetNamespace(const QString &targetNamespace);

    bool recursive() const;
    void setRecursive(bool recursive);

private:
    bool m_containsBitMask{false};
    bool m_isBuiltInType{false};
    bool m_hasSwitchfield{false};
    bool m_hasUnion{false};
    bool m_recursive{false};
    QString m_baseType;
    QString m_targetNamespace;
    QList<Field *> m_fields;
};

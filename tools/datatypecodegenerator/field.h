// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma once

#include "xmlelement.h"

class Field : public XmlElement
{
public:
    Field(const QString &name, const QString &typeName, const QString &lengthField = QString());
    ~Field() override = default;

    void print() const override;
    virtual void accept(Visitor *visitor) override;

    QString typeName() const;
    QString typeNameSecondPart() const;
    void setTypeName(const QString &typeName);

    QString lengthField() const;
    void setLengthField(const QString &lengthField);

    bool hasLengthField() const;
    void setHasLengthField(bool hasLengthField);

    bool needContainer() const;
    void setNeedContainer(bool needContainer);

    bool isInStructuredTypeBitMask() const;
    void setIsInStructuredTypeBitMask(bool isInStructuredTypeBitMask);

    int length() const;
    void setLength(int length);

    QString switchField() const;
    void setSwitchField(const QString &switchField);

    int switchValue() const;
    void setSwitchValue(int switchValue);

    bool isUnion() const;
    void setIsUnion(bool isUnion);

    bool recursive() const;
    void setRecursive(bool recursive);

    bool isEnum() const;
    void setIsEnum(bool newIsEnum);
private:
    bool m_hasLengthField{false};
    bool m_needContainer{false};
    bool m_isInStructuredTypeBitMask{false};
    bool m_isUnion{false};
    int m_length;
    int m_switchValue{0};
    bool m_recursive{false};
    bool m_isEnum{false};
    QString m_lengthField;
    QString m_switchField;
    QString m_typeName;
};

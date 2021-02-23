// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma once

#include "xmlelement.h"

class Visitor;

class EnumeratedValue : public XmlElement
{
public:
    EnumeratedValue(const QString &name, qint32 value);
    ~EnumeratedValue() override = default;

    void print() const override;
    virtual void accept(Visitor *visitor) override;

    qint32 value() const;
    void setValue(qint32 value);

private:
    qint32 m_value;
};

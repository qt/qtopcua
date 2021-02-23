// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma once

#include "xmlelement.h"

#include <QtCore/qlist.h>

class EnumeratedValue;
class Visitor;

class EnumeratedType : public XmlElement

{
public:
    EnumeratedType(const QString &name, quint32 lengthInBits);
    ~EnumeratedType();

    void addValue(EnumeratedValue *enumeratedValue);
    void print() const override;

    quint32 lengthInBits() const;
    void setLengthInBits(quint32 lengthInBits);

    QList<EnumeratedValue *> values() const;
    void setValues(const QList<EnumeratedValue *> &values);

    virtual void accept(Visitor *visitor) override;

private:
    quint32 m_lengthInBits;
    QList<EnumeratedValue *> m_values;
};

// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma once

#include "xmlelement.h"

class Visitor;

class Import : public XmlElement
{
public:
    Import(const QString &nameSpace, const QString &location);
    ~Import() override = default;

    virtual void print() const override;
    virtual void accept(Visitor *visitor) override;

    QString nameSpace() const;
    void setNameSpace(const QString &nameSpace);

    QString location() const;
    void setLocation(const QString &location);

private:
    QString m_location;
    QString m_nameSpace;
};

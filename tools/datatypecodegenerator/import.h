// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

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

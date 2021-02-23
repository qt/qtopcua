// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma once

#include <QtCore/qstring.h>

class Visitor;

class XmlElement
{
public:
    XmlElement() = default;
    XmlElement(const QString &name);
    virtual ~XmlElement() = default;

    void setName(const QString &name);
    QString name() const;
    QString lowerFirstName() const;

    virtual void print() const;
    virtual void accept(Visitor *visitor) = 0;

private:
    QString m_name;
};

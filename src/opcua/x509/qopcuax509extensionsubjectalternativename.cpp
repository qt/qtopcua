/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#include "qopcuax509extensionsubjectalternativename.h"
#include "qopcuax509extension_p.h"

#include <QList>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaX509ExtensionSubjectAlternativeName
    \inmodule QtOpcUa
    \since 5.14

    \brief Class for an X509 subject alternative name.

    This class is currently available as a Technology Preview, and therefore the API
    and functionality provided by the class may be subject to change at any time without
    prior notice.
*/

/*!
    \enum QOpcUaX509ExtensionSubjectAlternativeName::Type

    Enum with entry types for subject alternative name.

    \value Email
        Entry type for an email address
    \value URI
        Entry type for an URI
    \value DNS
        Entry type for DNS
    \value IP
        Entry type for an IP address
*/

class QOpcUaX509ExtensionSubjectAlternativeNameData : public QOpcUaX509ExtensionData
{
public:
    ~QOpcUaX509ExtensionSubjectAlternativeNameData() override = default;
    QList <QPair<QOpcUaX509ExtensionSubjectAlternativeName::Type, QString>> entries;
};

/*!
    Constructs a X509ExtensionSubjectAlternativeName.
*/
QOpcUaX509ExtensionSubjectAlternativeName::QOpcUaX509ExtensionSubjectAlternativeName()
    : QOpcUaX509Extension(new QOpcUaX509ExtensionSubjectAlternativeNameData)
{
}

/*!
    Constructs a X509ExtensionSubjectAlternativeName from \a rhs.
*/
QOpcUaX509ExtensionSubjectAlternativeName::QOpcUaX509ExtensionSubjectAlternativeName(const QOpcUaX509ExtensionSubjectAlternativeName &rhs)
    : QOpcUaX509Extension(rhs.data)
{
}

/*!
    Returns \c true if this X509ExtensionSubjectAlternativeName has the same value as \a rhs.
*/
bool QOpcUaX509ExtensionSubjectAlternativeName::operator==(const QOpcUaX509ExtensionSubjectAlternativeName &rhs) const
{
    return data->critical == rhs.data->critical;
}

/*!
    Destructs a X509ExtensionSubjectAlternativeName.
*/
QOpcUaX509ExtensionSubjectAlternativeName::~QOpcUaX509ExtensionSubjectAlternativeName()
{
}

/*!
    Sets the values from \a rhs in this X509ExtensionSubjectAlternativeName.
*/
QOpcUaX509ExtensionSubjectAlternativeName &QOpcUaX509ExtensionSubjectAlternativeName::operator=(const QOpcUaX509ExtensionSubjectAlternativeName &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Adds an entry of type \a type with content \a value.
*/
void QOpcUaX509ExtensionSubjectAlternativeName::addEntry(QOpcUaX509ExtensionSubjectAlternativeName::Type type, const QString &value)
{
    QOpcUaX509ExtensionSubjectAlternativeNameData *d = static_cast<QOpcUaX509ExtensionSubjectAlternativeNameData*>(data.data());
    d->entries.append(qMakePair(type, value));
}

/*!
    Returns the vector of entries.
*/
const QList<QPair<QOpcUaX509ExtensionSubjectAlternativeName::Type, QString>> &QOpcUaX509ExtensionSubjectAlternativeName::entries() const
{
    const QOpcUaX509ExtensionSubjectAlternativeNameData *d = static_cast<const QOpcUaX509ExtensionSubjectAlternativeNameData*>(data.data());
    return d->entries;
}

QT_END_NAMESPACE

// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuax509distinguishedname.h"
#include <QtCore/QMap>

QT_BEGIN_NAMESPACE

using namespace Qt::Literals::StringLiterals;

/*!
    \class QOpcUaX509DistinguishedName
    \inmodule QtOpcUa
    \since 5.14

    \brief Information about a distinguished name item.

    This class is currently available as a Technology Preview, and therefore the API
    and functionality provided by the class may be subject to change at any time without
    prior notice.

    \code
        QOpcUaX509DistinguishedName dn;
        dn.setEntry(QOpcUaX509DistinguishedName::Type::CommonName, "QtOpcUaViewer");
        dn.setEntry(QOpcUaX509DistinguishedName::Type::CountryName, "DE");
        dn.setEntry(QOpcUaX509DistinguishedName::Type::LocalityName, "Berlin");
        dn.setEntry(QOpcUaX509DistinguishedName::Type::StateOrProvinceName, "Berlin");
        dn.setEntry(QOpcUaX509DistinguishedName::Type::OrganizationName, "The Qt Company");
    \endcode

    \sa QOpcUaX509CertificateSigningRequest
*/

/*!
    \enum QOpcUaX509DistinguishedName::Type

    Enum with entry types for X509DistinguishedName.

    \value CommonName
        Common name
    \value CountryName
        Country name
    \value LocalityName
        Locality name
    \value StateOrProvinceName
        State or province name
    \value OrganizationName
        Organization name
*/

class QOpcUaX509DistinguishedNameData : public QSharedData
{
public:
    QMap<QOpcUaX509DistinguishedName::Type, QString> entries;
};

/*!
    Constructs an empty X509DistinguishedName.
*/
QOpcUaX509DistinguishedName::QOpcUaX509DistinguishedName()
    : data(new QOpcUaX509DistinguishedNameData)
{
}

/*!
    Destructs a X509DistinguishedName.
*/
QOpcUaX509DistinguishedName::~QOpcUaX509DistinguishedName()
{
}

/*!
    Constructs a X509DistinguishedName from \a rhs.
*/
QOpcUaX509DistinguishedName::QOpcUaX509DistinguishedName(const QOpcUaX509DistinguishedName &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this X509DistinguishedName.
*/
QOpcUaX509DistinguishedName &QOpcUaX509DistinguishedName::operator=(const QOpcUaX509DistinguishedName &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this X509DistinguishedName has the same value as \a rhs.
*/
bool QOpcUaX509DistinguishedName::operator==(const QOpcUaX509DistinguishedName &rhs) const
{
    return data->entries == rhs.data->entries;
}

/*!
    Sets the entry of \a type to \a value.
    Already existing types will be overwritten.
*/
void QOpcUaX509DistinguishedName::setEntry(QOpcUaX509DistinguishedName::Type type, const QString &value)
{
    data->entries.insert(type, value);
}

/*!
    Returns the object id string for \a type.
*/
QString QOpcUaX509DistinguishedName::typeToOid(QOpcUaX509DistinguishedName::Type type)
{
    switch (type) {
    case Type::CommonName:
        return u"2.5.4.3"_s;
    case Type::CountryName:
        return u"2.5.4.6"_s;
    case Type::LocalityName:
        return u"2.5.4.7"_s;
    case Type::StateOrProvinceName:
        return u"2.5.4.8"_s;
    case Type::OrganizationName:
        return u"2.5.4.10"_s;
    default:
        return QString();
    }
}

/*!
    Returns value for a \a type.
*/
QString QOpcUaX509DistinguishedName::entry(QOpcUaX509DistinguishedName::Type type) const
{
    return data->entries.value(type);
}

QT_END_NAMESPACE

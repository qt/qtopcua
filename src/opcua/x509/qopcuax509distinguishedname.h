// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUADISTINGUISHEDNAME_H
#define QOPCUADISTINGUISHEDNAME_H

#include <QtOpcUa/qopcuaglobal.h>
#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaX509DistinguishedNameData;
class Q_OPCUA_EXPORT QOpcUaX509DistinguishedName
{
public:
    enum class Type {
        CommonName, // 2.5.4.3
        CountryName, // 2.5.4.6
        LocalityName, // 2.5.4.7
        StateOrProvinceName, // 2.5.4.8
        OrganizationName, // 2.5.4.10
    };

    QOpcUaX509DistinguishedName();
    QOpcUaX509DistinguishedName(const QOpcUaX509DistinguishedName &);
    QOpcUaX509DistinguishedName &operator=(const QOpcUaX509DistinguishedName &);
    bool operator==(const QOpcUaX509DistinguishedName &rhs) const;
    ~QOpcUaX509DistinguishedName();
    void setEntry(Type type, const QString &value);
    QString entry(Type type) const;
    static QString typeToOid(Type type);

private:
    QSharedDataPointer<QOpcUaX509DistinguishedNameData> data;
};

QT_END_NAMESPACE

#endif // QOPCUADISTINGUISHEDNAME_H

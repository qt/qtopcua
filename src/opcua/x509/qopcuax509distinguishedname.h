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

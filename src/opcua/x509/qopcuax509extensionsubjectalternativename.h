// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAX509EXTENSIONSUBJECTALTERNATIVENAME_H
#define QOPCUAX509EXTENSIONSUBJECTALTERNATIVENAME_H

#include "QtOpcUa/qopcuax509extension.h"
#include <QtOpcUa/qopcuaglobal.h>

QT_BEGIN_NAMESPACE

// OID 2.5.29.17
class Q_OPCUA_EXPORT QOpcUaX509ExtensionSubjectAlternativeName : public QOpcUaX509Extension
{
public:
    enum class Type {
        Email,
        URI,
        DNS,
        IP
    };

    QOpcUaX509ExtensionSubjectAlternativeName();
    QOpcUaX509ExtensionSubjectAlternativeName(const QOpcUaX509ExtensionSubjectAlternativeName &);
    QOpcUaX509ExtensionSubjectAlternativeName &operator=(const QOpcUaX509ExtensionSubjectAlternativeName &);
    bool operator==(const QOpcUaX509ExtensionSubjectAlternativeName &rhs) const;
    ~QOpcUaX509ExtensionSubjectAlternativeName();

    void addEntry(Type type, const QString &value);
    const QList<QPair<Type, QString>> &entries() const;
};

QT_END_NAMESPACE
#endif // QOPCUAX509EXTENSIONSUBJECTALTERNATIVENAME_H

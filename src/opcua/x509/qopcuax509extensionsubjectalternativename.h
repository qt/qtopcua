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

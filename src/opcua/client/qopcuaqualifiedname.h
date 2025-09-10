// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAQUALIFIEDNAMEDATA_H
#define QOPCUAQUALIFIEDNAMEDATA_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QOpcUaQualifiedNameData;
class Q_OPCUA_EXPORT QOpcUaQualifiedName
{
public:
    QOpcUaQualifiedName();
    QOpcUaQualifiedName(const QOpcUaQualifiedName &);
    QOpcUaQualifiedName(quint16 namespaceIndex, const QString &name);
    QOpcUaQualifiedName &operator=(const QOpcUaQualifiedName &);
    bool operator==(const QOpcUaQualifiedName &rhs) const;
    operator QVariant() const;
    ~QOpcUaQualifiedName();

    QString name() const;
    void setName(const QString &name);

    quint16 namespaceIndex() const;
    void setNamespaceIndex(quint16 namespaceIndex);

private:
    QSharedDataPointer<QOpcUaQualifiedNameData> data;

#ifndef QT_NO_DEBUG_STREAM
    friend Q_OPCUA_EXPORT QDebug operator<<(QDebug debug, const QOpcUaQualifiedName &qn);
#endif
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaQualifiedName)

#endif // QOPCUAQUALIFIEDNAMEDATA_H

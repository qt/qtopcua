// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAEUINFORMATION_H
#define QOPCUAEUINFORMATION_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QOpcUaLocalizedText;

class QOpcUaEUInformationData;
class Q_OPCUA_EXPORT QOpcUaEUInformation
{
public:
    QOpcUaEUInformation();
    QOpcUaEUInformation(const QOpcUaEUInformation &);
    QOpcUaEUInformation(const QString &namespaceUri, qint32 unitId,
                   const QOpcUaLocalizedText &displayName, const QOpcUaLocalizedText &description);
    QOpcUaEUInformation &operator=(const QOpcUaEUInformation &);
    bool operator==(const QOpcUaEUInformation &rhs) const;
    operator QVariant() const;
    ~QOpcUaEUInformation();

    QString namespaceUri() const;
    void setNamespaceUri(const QString &namespaceUri);

    qint32 unitId() const;
    void setUnitId(qint32 unitId);

    QOpcUaLocalizedText displayName() const;
    void setDisplayName(const QOpcUaLocalizedText &displayName);

    QOpcUaLocalizedText description() const;
    void setDescription(const QOpcUaLocalizedText &description);

private:
    QSharedDataPointer<QOpcUaEUInformationData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaEUInformation)

#endif // QOPCUAEUINFORMATION_H

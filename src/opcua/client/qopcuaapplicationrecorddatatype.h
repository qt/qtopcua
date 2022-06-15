// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAAPPLICATIONRECORDDATATYPE_H
#define QOPCUAAPPLICATIONRECORDDATATYPE_H

#include <QtOpcUa/qopcuaglobal.h>
#include <QtOpcUa/qopcualocalizedtext.h>
#include <QtOpcUa/qopcuaapplicationdescription.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QOpcUaApplicationRecordDataTypeData;
class Q_OPCUA_EXPORT QOpcUaApplicationRecordDataType
{
public:
    QOpcUaApplicationRecordDataType();
    QOpcUaApplicationRecordDataType(const QOpcUaApplicationRecordDataType &);
    QOpcUaApplicationRecordDataType &operator=(const QOpcUaApplicationRecordDataType &);
    bool operator==(const QOpcUaApplicationRecordDataType &rhs) const;
    operator QVariant() const;
    ~QOpcUaApplicationRecordDataType();

    void setApplicationId(const QString &nodeId);
    const QString &applicationId() const;

    void setApplicationType(QOpcUaApplicationDescription::ApplicationType applicationType);
    QOpcUaApplicationDescription::ApplicationType applicationType() const;

    void setApplicationUri(const QString &applicationUri);
    const QString &applicationUri() const;

    void setApplicationNames(const QList<QOpcUaLocalizedText> &applicationNames);
    const QList<QOpcUaLocalizedText> &applicationNames() const;

    void setProductUri(const QString &productUri);
    const QString &productUri() const;

    void setDiscoveryUrls(const QList<QString> &discoveryUrls);
    const QList<QString> &discoveryUrls() const;

    void setServerCapabilityIdentifiers(const QList<QString> &serverCapabilityIdentifiers);
    const QList<QString> &serverCapabilityIdentifiers() const;

private:
    QSharedDataPointer<QOpcUaApplicationRecordDataTypeData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaApplicationRecordDataType)

#endif // QOPCUAAPPLICATIONRECORDDATATYPE_H

// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtOpcUa/qopcuaclient.h>
#include <QtOpcUa/qopcuaextensionobject.h>
#include <QtOpcUa/qopcuastructuredefinition.h>
#include <QtOpcUa/qopcuabinarydataencoding.h>
#include <QtOpcUa/qopcuagenericstructvalue.h>

#include <QtCore/qobject.h>

#include <optional>

#ifndef QOPCUAGENERICSTRUCTHANDLER_H
#define QOPCUAGENERICSTRUCTHANDLER_H

QT_BEGIN_NAMESPACE

class QOpcUaGenericStructHandlerPrivate;

class Q_OPCUA_EXPORT QOpcUaGenericStructHandler : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(QOpcUaGenericStructHandler)
    Q_DECLARE_PRIVATE(QOpcUaGenericStructHandler)
public:
    QOpcUaGenericStructHandler(QOpcUaClient *client, QObject *parent = nullptr);

    bool initialize();

    std::optional<QOpcUaGenericStructValue> decode(const QOpcUaExtensionObject &extensionObject) const;
    std::optional<QOpcUaExtensionObject> encode(const QOpcUaGenericStructValue &value);

    QOpcUaGenericStructValue createGenericStructValueForTypeId(const QString &typeId);

    QOpcUaStructureDefinition structureDefinitionForBinaryEncodingId(const QString &id) const;
    QOpcUaStructureDefinition structureDefinitionForTypeId(const QString &id) const;
    QOpcUaEnumDefinition enumDefinitionForTypeId(const QString &id) const;
    QString typeNameForBinaryEncodingId(const QString &id) const;
    QString typeNameForTypeId(const QString &id) const;
    bool isAbstractTypeId(const QString &id) const;

    bool addCustomStructureDefinition(const QOpcUaStructureDefinition &definition, const QString &typeId,
                                      const QString &name, bool isAbstract = false);
    bool addCustomEnumDefinition(const QOpcUaEnumDefinition &definition, const QString &typeId,
                                 const QString &name, bool isAbstract = false);

    bool initialized() const;

    enum class DataTypeKind {
        Unknown = 0,
        Struct = 1,
        Enum = 2,
        Other = 3,
    };
    Q_ENUM(DataTypeKind)

    DataTypeKind dataTypeKindForTypeId(const QString &id) const;

    QString typeIdForBinaryEncodingId(const QString &id) const;

Q_SIGNALS:
    void initializedChanged(bool initialized);
};

QT_END_NAMESPACE

#endif // QOPCUAGENERICSTRUCTHANDLER_H

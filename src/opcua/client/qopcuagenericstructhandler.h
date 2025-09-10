// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtOpcUa/qopcuaglobal.h>
#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qobject.h>
#include <QtCore/qstringfwd.h>

#include <optional>

#ifndef QOPCUAGENERICSTRUCTHANDLER_H
#define QOPCUAGENERICSTRUCTHANDLER_H

QT_BEGIN_NAMESPACE

class QOpcUaClient;
class QOpcUaEnumDefinition;
class QOpcUaExtensionObject;
class QOpcUaStructureDefinition;
class QOpcUaGenericStructValue;

class QOpcUaGenericStructHandlerPrivate;

class Q_OPCUA_EXPORT QOpcUaGenericStructHandler : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(QOpcUaGenericStructHandler)
    Q_DECLARE_PRIVATE(QOpcUaGenericStructHandler)
public:
    explicit QOpcUaGenericStructHandler(QOpcUaClient *client, QObject *parent = nullptr);
    ~QOpcUaGenericStructHandler() override;

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
                                      const QString &name, QOpcUa::IsAbstract isAbstract = QOpcUa::IsAbstract::NotAbstract);
    bool addCustomEnumDefinition(const QOpcUaEnumDefinition &definition, const QString &typeId,
                                 const QString &name, QOpcUa::IsAbstract isAbstract = QOpcUa::IsAbstract::NotAbstract);

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

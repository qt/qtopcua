// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtOpcUa/qopcuaenumdefinition.h>
#include <QtOpcUa/qopcuagenericstructhandler.h>
#include <QtOpcUa/qopcuaclient.h>
#include <QtOpcUa/qopcuaextensionobject.h>
#include <QtOpcUa/qopcuastructuredefinition.h>
#include <QtOpcUa/qopcuabinarydataencoding.h>
#include <QtOpcUa/qopcuamultidimensionalarray.h>

#include <private/qobject_p.h>
#include <QLoggingCategory>
#include <QPointer>
#include <QSet>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QOPCUAGENERICSTRUCTHANDLERPRIVATE_H
#define QOPCUAGENERICSTRUCTHANDLERPRIVATE_H

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(lcGenericStructHandler);

class QOpcUaInternalDataTypeNode;

class QOpcUaGenericStructHandlerPrivate : public QObjectPrivate {
    Q_DECLARE_PUBLIC(QOpcUaGenericStructHandler)

    Q_DISABLE_COPY(QOpcUaGenericStructHandlerPrivate)
public:
    QOpcUaGenericStructHandlerPrivate(QOpcUaClient *client);

    bool initialize();

    QOpcUaGenericStructValue decode(const QOpcUaExtensionObject &extensionObject, bool &success) const;
    bool encode(const QOpcUaGenericStructValue &value, QOpcUaExtensionObject &output);

    QOpcUaGenericStructValue createGenericStructValueForTypeId(const QString &typeId);

    QOpcUaStructureDefinition structureDefinitionForBinaryEncodingId(const QString &id) const;
    QOpcUaStructureDefinition structureDefinitionForTypeId(const QString &id) const;
    QOpcUaEnumDefinition enumDefinitionForTypeId(const QString &id) const;
    QString typeNameForBinaryEncodingId(const QString &id) const;
    QString typeNameForTypeId(const QString &id) const;
    QOpcUaGenericStructHandler::DataTypeKind dataTypeKindForTypeId(const QString &id) const;
    QString typeIdForBinaryEncodingId(const QString &id) const;
    bool isAbstractTypeId(const QString &id) const;

Q_SIGNALS:
    void initializeFinished(bool success);

protected:
    QOpcUaGenericStructValue decodeStructInternal(QOpcUaBinaryDataEncoding &decoder, const QString &dataTypeId,
                                                  bool &success, int currentDepth) const;
    QVariant decodeKnownTypesInternal(QOpcUaBinaryDataEncoding &decoder, const QString &dataTypeId, qint32 valueRank,
                                      bool &success, int currentDepth) const;
    bool encodeStructInternal(QOpcUaBinaryDataEncoding &encoder, const QOpcUaGenericStructValue &value);
    bool encodeKnownTypesInternal(QOpcUaBinaryDataEncoding &encoder, const QVariant &value, qint32 valueRank, const QString &dataTypeId);
    void handleFinished(bool success);

    bool addCustomStructureDefinition(const QOpcUaStructureDefinition &definition, const QString &typeId, const QString &name,
                                      QOpcUa::IsAbstract isAbstract);
    bool addCustomEnumDefinition(const QOpcUaEnumDefinition &definition, const QString &typeId, const QString &name,
                                 QOpcUa::IsAbstract isAbstract);

    bool initialized() const;

    template <typename T, QOpcUa::Types OVERLAY = QOpcUa::Types::Undefined>
    QVariant decodeArrayOrScalar(QOpcUaBinaryDataEncoding &decoder, qint32 valueRank, bool &success) const
    {
        if (valueRank > 1) {
            const auto arrayDimensions = decoder.decodeArray<quint32>(success);

            if (!success)
                return QVariant();

            const auto data = decoder.decodeArray<T, OVERLAY>(success);

            if (!success)
                return QVariant();

            QOpcUaMultiDimensionalArray matrix;
            matrix.setArrayDimensions(arrayDimensions);
            matrix.setValueArray({data.constBegin(), data.constEnd()});
            return matrix;
        }

        return valueRank == 1 ? QVariant::fromValue(decoder.decodeArray<T, OVERLAY>(success)) : decoder.decode<T, OVERLAY>(success);
    }

    template <typename T, QOpcUa::Types OVERLAY = QOpcUa::Types::Undefined>
    bool encodeArrayOrScalar(QOpcUaBinaryDataEncoding &encoder, qint32 valueRank, const QVariant &value)
    {
        if ((valueRank == 1 && !value.canConvert<QList<T>>()) || (valueRank <= 0 && !value.canConvert<T>())) {
            qCWarning(lcGenericStructHandler) << "Type mismatch for enum field, unable to encode";
            return false;
        }

        if (valueRank > 1) {
            if (!value.canConvert<QOpcUaMultiDimensionalArray>()) {
                qCWarning(lcGenericStructHandler) << "Type mismatch for enum field, value rank > 1 requires QOpcUaMultiDimensionalArray";
                return false;
            }

            auto array = value.value<QOpcUaMultiDimensionalArray>();
            QList<T> data;
            const auto valueArray = array.valueArray();
            for (const auto &entry : valueArray) {
                if (!entry.canConvert<T>()) {
                    qCWarning(lcGenericStructHandler) << "Invalid type in multi dimensional array";
                }
                data.push_back(entry.value<T>());
            }

            const auto success = encoder.encodeArray<quint32>(array.arrayDimensions());

            if (!success)
                return false;

            return encoder.encodeArray<T, OVERLAY>(data);
        }

        return valueRank == 1 ? encoder.encodeArray<T, OVERLAY>(value.value<QList<T>>()) : encoder.encode<T, OVERLAY>(value.value<T>());
    }

protected:
    void handleInitializeFinished(bool success);
    void processDataTypeRecursive(QOpcUaInternalDataTypeNode *node);
    void processStructRecursive(QOpcUaInternalDataTypeNode *node);
    void processEnumRecursive(QOpcUaInternalDataTypeNode *node);
    void processSubtypeOfKnownTypeRecursive(QOpcUaInternalDataTypeNode *node, const QString &id);

private:
    QPointer<QOpcUaClient> m_client;
    QScopedPointer<QOpcUaInternalDataTypeNode> m_baseDataType;

    int m_finishedCount = 0;
    bool m_hasError = false;

    class StructMapEntry {
    public:
        QString name;
        QString nodeId;
        bool isAbstract = false;
        QOpcUaStructureDefinition structureDefinition;
    };

    class EnumMapEntry {
    public:
        QString name;
        QString nodeId;
        bool isAbstract = false;
        QOpcUaEnumDefinition enumDefinition;
    };

    QHash<QString, StructMapEntry> m_structuresByEncodingId;
    QHash<QString, StructMapEntry> m_structuresByTypeId;
    QHash<QString, EnumMapEntry> m_enumsByTypeId;
    QHash<QString, QString> m_typeNamesByTypeId;
    QHash<QString, QString> m_typeNamesByEncodingId;
    QSet<QString> m_abstractTypeIds;

    QHash<QString, QString> m_knownSubtypes;

    const int m_maxNestingLevel = 500;

    bool m_initialized = false;
};

QT_END_NAMESPACE

#endif // QOPCUAGENERICSTRUCTHANDLERPRIVATE_H

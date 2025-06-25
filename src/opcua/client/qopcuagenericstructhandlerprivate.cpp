// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "private/qopcuagenericstructhandler_p.h"

#include <QtOpcUa/qopcuaargument.h>
#include <QtOpcUa/qopcuaaxisinformation.h>
#include <QtOpcUa/qopcuabinarydataencoding.h>
#include <QtOpcUa/qopcuacomplexnumber.h>
#include <QtOpcUa/qopcuadatavalue.h>
#include <QtOpcUa/qopcuadiagnosticinfo.h>
#include <QtOpcUa/qopcuadoublecomplexnumber.h>
#include <QtOpcUa/qopcuaenumfield.h>
#include <QtOpcUa/qopcuaeuinformation.h>
#include <QtOpcUa/qopcuagenericstructhandler.h>
#include <QtOpcUa/qopcuagenericstructvalue.h>
#include <QtOpcUa/qopcuarange.h>
#include <QtOpcUa/qopcuastructurefield.h>
#include <QtOpcUa/qopcuaxvalue.h>

#include "qopcuainternaldatatypenode_p.h"

#include <QtCore/quuid.h>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(lcGenericStructHandler, "qt.opcuagenericstructhandler")

QOpcUaGenericStructHandlerPrivate::QOpcUaGenericStructHandlerPrivate(QOpcUaClient *client)
    : m_client(client)
{
}

bool QOpcUaGenericStructHandlerPrivate::initialize()
{
    if (!m_client)
        return false;

    m_initialized = false;

    m_baseDataType.reset(new QOpcUaInternalDataTypeNode(m_client));

    QObjectPrivate::connect(m_baseDataType.get(), &QOpcUaInternalDataTypeNode::initializeFinished,
                     this, &QOpcUaGenericStructHandlerPrivate::handleInitializeFinished);

    return m_baseDataType->initialize(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::BaseDataType));
}

QOpcUaGenericStructValue QOpcUaGenericStructHandlerPrivate::decode(const QOpcUaExtensionObject &extensionObject, bool &success) const
{
    const auto entry = m_structuresByEncodingId.constFind(extensionObject.encodingTypeId());
    if (entry == m_structuresByEncodingId.constEnd()) {
        qCWarning(lcGenericStructHandler) << "Failed to find description for" << extensionObject.encodingTypeId();
        success = false;
        return QOpcUaGenericStructValue();
    }

    qCDebug(lcGenericStructHandler) << "Decoding" << entry->name << entry.key();

    auto data = extensionObject.encodedBody();
    QOpcUaBinaryDataEncoding decoder(&data);

    return decodeStructInternal(decoder, entry->nodeId, success, 0);
}

bool QOpcUaGenericStructHandlerPrivate::encode(const QOpcUaGenericStructValue &value, QOpcUaExtensionObject &output)
{
    if (value.structureDefinition().fields().empty()) {
        qCWarning(lcGenericStructHandler) << "The structure doesn't have any fields, unable to encode";
        return false;
    }

    if (value.structureDefinition().defaultEncodingId().isEmpty()) {
        qCWarning(lcGenericStructHandler) << "The struct doesn't have an encoding ID, unable to encode";
        return false;
    }

    output.setEncodingTypeId(value.structureDefinition().defaultEncodingId());
    output.setEncoding(QOpcUaExtensionObject::Encoding::ByteString);

    QOpcUaBinaryDataEncoding encoder(output);
    return encodeStructInternal(encoder, value);
}

QOpcUaGenericStructValue QOpcUaGenericStructHandlerPrivate::createGenericStructValueForTypeId(const QString &typeId)
{
    const auto entry = m_structuresByTypeId.constFind(typeId);

    if (entry != m_structuresByTypeId.constEnd()) {
        QOpcUaGenericStructValue value(entry.value().name, typeId, entry.value().structureDefinition);

        if (entry.value().structureDefinition.structureType() != QOpcUaStructureDefinition::StructureType::Union) {
            const auto tempFields = entry.value().structureDefinition.fields();
            for (const auto &field : tempFields) {
                if (field.isOptional())
                    continue;

                value.fieldsRef().insert(field.name(), QVariant());
            }
        }

        return value;
    }

    qCWarning(lcGenericStructHandler) << "Unable to create a pre-filled generic struct value for" << typeId;
    return {};
}

QOpcUaStructureDefinition QOpcUaGenericStructHandlerPrivate::structureDefinitionForBinaryEncodingId(const QString &id) const
{
    const auto entry = m_structuresByEncodingId.constFind(id);
    return entry != m_structuresByEncodingId.constEnd() ? entry.value().structureDefinition : QOpcUaStructureDefinition();
}

QOpcUaStructureDefinition QOpcUaGenericStructHandlerPrivate::structureDefinitionForTypeId(const QString &id) const
{
    const auto entry = m_structuresByTypeId.constFind(id);
    return entry != m_structuresByTypeId.constEnd() ? entry.value().structureDefinition : QOpcUaStructureDefinition();
}

QOpcUaEnumDefinition QOpcUaGenericStructHandlerPrivate::enumDefinitionForTypeId(const QString &id) const
{
    const auto entry = m_enumsByTypeId.constFind(id);
    return entry != m_enumsByTypeId.constEnd() ? entry.value().enumDefinition : QOpcUaEnumDefinition();
}

QString QOpcUaGenericStructHandlerPrivate::typeNameForBinaryEncodingId(const QString &id) const
{
    const auto entry = m_typeNamesByEncodingId.constFind(id);
    return entry != m_typeNamesByEncodingId.constEnd() ? entry.value() : QString();
}

QString QOpcUaGenericStructHandlerPrivate::typeNameForTypeId(const QString &id) const
{
    const auto entry = m_typeNamesByTypeId.constFind(id);
    return entry != m_typeNamesByTypeId.constEnd() ? entry.value() : QString();
}

QOpcUaGenericStructHandler::DataTypeKind QOpcUaGenericStructHandlerPrivate::dataTypeKindForTypeId(const QString &id) const
{
    const auto isStruct = m_structuresByTypeId.constFind(id);
    if (isStruct != m_structuresByTypeId.constEnd())
        return QOpcUaGenericStructHandler::DataTypeKind::Struct;

    const auto isEnum = m_enumsByTypeId.constFind(id);
    if (isEnum != m_enumsByTypeId.constEnd())
        return QOpcUaGenericStructHandler::DataTypeKind::Enum;

    return typeNameForTypeId(id).isEmpty() ?
               QOpcUaGenericStructHandler::DataTypeKind::Unknown : QOpcUaGenericStructHandler::DataTypeKind::Other;
}

QString QOpcUaGenericStructHandlerPrivate::typeIdForBinaryEncodingId(const QString &id) const
{
    const auto entry = m_structuresByEncodingId.constFind(id);
    return entry != m_structuresByEncodingId.constEnd() ? entry->nodeId : QString();
}

bool QOpcUaGenericStructHandlerPrivate::isAbstractTypeId(const QString &id) const
{
    return m_abstractTypeIds.contains(id);
}

QOpcUaGenericStructValue QOpcUaGenericStructHandlerPrivate::decodeStructInternal(QOpcUaBinaryDataEncoding &decoder,
                                                                                 const QString &dataTypeId, bool &success,
                                                                                 int currentDepth) const
{
    if (currentDepth > m_maxNestingLevel) {
        qCWarning(lcGenericStructHandler) << "Maximum nesting level of" << m_maxNestingLevel << "exceeded";
        success = false;
        return QOpcUaGenericStructValue();
    }

    const auto entry = m_structuresByTypeId.constFind(dataTypeId);
    if (entry == m_structuresByTypeId.constEnd()) {
        qCWarning(lcGenericStructHandler) << "Failed to find description for" << dataTypeId;
        success = false;
        return QOpcUaGenericStructValue();
    }

    if (entry->isAbstract) {
        qCWarning(lcGenericStructHandler) << "Decoding of abstract struct" << entry->name << "requested";
        success = false;
        return QOpcUaGenericStructValue();
    }

    if (entry->structureDefinition.fields().isEmpty()) {
        qCWarning(lcGenericStructHandler) << "Missing fields information for struct" << entry->name;
        success = false;
        return QOpcUaGenericStructValue();
    }

    QOpcUaGenericStructValue result(entry->name, entry->nodeId, entry->structureDefinition);
    auto &fields = result.fieldsRef();

    const auto definitionFields = entry->structureDefinition.fields();

    if (entry->structureDefinition.structureType() == QOpcUaStructureDefinition::StructureType::Structure) {
        for (const auto &field : definitionFields) {
            fields[field.name()] = decodeKnownTypesInternal(decoder, field.dataType(), field.valueRank(), success, currentDepth + 1);
            if (!success) {
                qCWarning(lcGenericStructHandler) << "Failed to decode struct field";
                return QOpcUaGenericStructValue();
            }
        }
    } else if (entry->structureDefinition.structureType() == QOpcUaStructureDefinition::StructureType::Union) {
        auto switchField = decoder.decode<quint32>(success);
        if (!success) {
            qCWarning(lcGenericStructHandler) << "Failed to decode the union switch field";
            return QOpcUaGenericStructValue();
        }

        if (!switchField)
            return result; // Empty union, no need to continue processing

        if (switchField > static_cast<quint32>(definitionFields.size())) {
            qCWarning(lcGenericStructHandler) << "Union switch field out of bounds";
            success = false;
            return QOpcUaGenericStructValue();
        }

        qCDebug(lcGenericStructHandler) << "Decode union field with switch value" << switchField;

        auto field = definitionFields.at(switchField - 1);

        fields[field.name()] = decodeKnownTypesInternal(decoder, field.dataType(), field.valueRank() > 0, success, currentDepth + 1);
        if (!success) {
            qCWarning(lcGenericStructHandler) << "Failed to decode union content";
            return QOpcUaGenericStructValue();
        }
    } else if (entry->structureDefinition.structureType() == QOpcUaStructureDefinition::StructureType::StructureWithOptionalFields) {
        auto mask = decoder.decode<quint32>(success);
        if (!success) {
            qCWarning(lcGenericStructHandler) << "Failed to decode the optional fields mask";
            return QOpcUaGenericStructValue();
        }

        int optionalFieldIndex = 0;
        for (const auto &field : definitionFields) {
            if (field.isOptional() && !(mask & (1 << optionalFieldIndex++)))
                continue;

            fields[field.name()] = decodeKnownTypesInternal(decoder, field.dataType(), field.valueRank() > 0, success, currentDepth + 1);
            if (!success) {
                qCWarning(lcGenericStructHandler) << "Failed to decode struct field";
                return QOpcUaGenericStructValue();
            }
        }
    }

    return result;
}

QVariant QOpcUaGenericStructHandlerPrivate::decodeKnownTypesInternal(QOpcUaBinaryDataEncoding &decoder, const QString &dataTypeId,
                                                                     qint32 valueRank, bool &success, int currentDepth) const
{
    if (currentDepth > m_maxNestingLevel) {
        qCWarning(lcGenericStructHandler) << "Maximum nesting level of" << m_maxNestingLevel << "exceeded";
        success = false;
        return QOpcUaGenericStructValue();
    }

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Boolean))
        return decodeArrayOrScalar<bool>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Byte))
        return decodeArrayOrScalar<quint8>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::SByte))
        return decodeArrayOrScalar<qint8>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::UInt16))
        return decodeArrayOrScalar<quint16>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Int16))
        return decodeArrayOrScalar<qint16>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::UInt32))
        return decodeArrayOrScalar<quint32>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Int32))
        return decodeArrayOrScalar<qint32>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::UInt64))
        return decodeArrayOrScalar<quint64>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Int64))
        return decodeArrayOrScalar<qint64>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Float))
        return decodeArrayOrScalar<float>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Double))
        return decodeArrayOrScalar<double>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::StatusCode))
        return decodeArrayOrScalar<QOpcUa::UaStatusCode>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::DateTime))
        return decodeArrayOrScalar<QDateTime>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::String))
        return decodeArrayOrScalar<QString>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::NodeId))
        return decodeArrayOrScalar<QString, QOpcUa::Types::NodeId>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::ByteString))
        return decodeArrayOrScalar<QByteArray>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::XmlElement))
        return decodeArrayOrScalar<QByteArray>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Guid))
        return decodeArrayOrScalar<QUuid>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::QualifiedName))
        return decodeArrayOrScalar<QOpcUaQualifiedName>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::LocalizedText))
        return decodeArrayOrScalar<QOpcUaLocalizedText>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Range))
        return decodeArrayOrScalar<QOpcUaRange>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::EUInformation))
        return decodeArrayOrScalar<QOpcUaEUInformation>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::ComplexNumberType))
        return decodeArrayOrScalar<QOpcUaComplexNumber>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::DoubleComplexNumberType))
        return decodeArrayOrScalar<QOpcUaDoubleComplexNumber>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::AxisInformation))
        return decodeArrayOrScalar<QOpcUaAxisInformation>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::XVType))
        return decodeArrayOrScalar<QOpcUaXValue>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::ExpandedNodeId))
        return decodeArrayOrScalar<QOpcUaExpandedNodeId>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Argument))
        return decodeArrayOrScalar<QOpcUaArgument>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::StructureDefinition))
        return decodeArrayOrScalar<QOpcUaStructureDefinition>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::StructureField))
        return decodeArrayOrScalar<QOpcUaStructureField>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::EnumDefinition))
        return decodeArrayOrScalar<QOpcUaEnumDefinition>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::EnumField))
        return decodeArrayOrScalar<QOpcUaEnumField>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::DiagnosticInfo)) {
        return decodeArrayOrScalar<QOpcUaDiagnosticInfo>(decoder, valueRank, success);
    }

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::DataValue))
        return decodeArrayOrScalar<QOpcUaDataValue>(decoder, valueRank, success);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::BaseDataType))
        return decodeArrayOrScalar<QOpcUaVariant>(decoder, valueRank, success);

    // Enumeration
    const auto enumType = m_enumsByTypeId.constFind(dataTypeId);
    if (enumType != m_enumsByTypeId.constEnd()) {
        if (enumType->isAbstract) {
            qCWarning(lcGenericStructHandler) << "Decoding abstract enum" << enumType->name << "requested";
            success = false;
            return QVariant();
        }

        const auto enumValue = decodeArrayOrScalar<qint32>(decoder, valueRank, success);
        if (!success) {
            qCWarning(lcGenericStructHandler) << "Failed to decode enum";
            return QVariant();
        }
        return enumValue;
    }

    // Fallback for nested unknown structs
    if (valueRank > 0) {
        QList<quint32> arrayDimensions;
        if (valueRank > 1) {
            arrayDimensions = decoder.decodeArray<quint32>(success);

            if (!success)
                return QVariant();
        }

        const auto arrayLength = decoder.decode<qint32>(success);
        if (!success)
            return QVariant();

        QList<QOpcUaGenericStructValue> result;
        for (int i = 0; i < arrayLength; ++i) {
            result.append(decodeStructInternal(decoder, dataTypeId, success, currentDepth + 1));
            if (!success) {
                qCWarning(lcGenericStructHandler) << "Failed to decode nested struct array";
                return QVariant();
            }
        }

        if (!arrayDimensions.isEmpty()) {
            QVariantList data(result.constBegin(), result.constEnd());
            return QOpcUaMultiDimensionalArray(data, arrayDimensions);
        }

        return QVariant::fromValue(result);
    } else {
        const auto result = decodeStructInternal(decoder, dataTypeId, success, currentDepth + 1);
        if (!success) {
            qCWarning(lcGenericStructHandler) << "Failed to decode nested struct";
            return QVariant();
        }
        return result;
    }

    const auto superType = m_knownSubtypes.constFind(dataTypeId);
    if (superType != m_knownSubtypes.constEnd())
        return decodeKnownTypesInternal(decoder, superType.value(), valueRank, success, currentDepth + 1);

    success = false;
    return QVariant();
}

bool QOpcUaGenericStructHandlerPrivate::encodeStructInternal(QOpcUaBinaryDataEncoding &encoder, const QOpcUaGenericStructValue &value)
{
    const auto entry = m_structuresByTypeId.constFind(value.typeId());

    if (entry == m_structuresByTypeId.constEnd()) {
        qCWarning(lcGenericStructHandler) << "Failed to find description for" << value.typeId();
        return false;
    }

    if (entry->isAbstract) {
        qCWarning(lcGenericStructHandler) << "Decoding of abstract struct" << entry->name << "requested";
        return false;
    }

    if (entry->structureDefinition.fields().isEmpty()) {
        qCWarning(lcGenericStructHandler) << "Missing fields information for struct" << entry->name;
        return false;
    }

    const auto definitionFields = value.structureDefinition().fields();

    if (value.structureDefinition().structureType() == QOpcUaStructureDefinition::StructureType::Structure) {
        for (const auto &field : definitionFields) {
            if (!value.fields().contains(field.name())) {
                qCWarning(lcGenericStructHandler) << "Field" << field.name() << "is missing, unable to encode struct";
                return false;
            }
            const auto success = encodeKnownTypesInternal(encoder, value.fields().value(field.name()), field.valueRank(), field.dataType());
            if (!success) {
                qCWarning(lcGenericStructHandler) << "Failed to encode struct field" << field.name();
                return false;
            }
        }

        return true;
    } else if (value.structureDefinition().structureType() == QOpcUaStructureDefinition::StructureType::StructureWithOptionalFields) {
        quint32 mask = 0;
        quint32 index = 0;
        for (const auto &field : definitionFields) {
            if (!field.isOptional())
                continue;

            if (value.fields().contains(field.name()))
                mask |= (1 << index);

            ++index;
        }

        auto success = encoder.encode<quint32>(mask);

        if (!success) {
            qCWarning(lcGenericStructHandler) << "Failed to encode optional fields mask";
            return false;
        }

        for (const auto &field : definitionFields) {
            if (!field.isOptional() && !value.fields().contains(field.name())) {
                qCWarning(lcGenericStructHandler) << "Field" << field.name() << "is missing, unable to encode struct";
                return false;
            }

            if (value.fields().contains(field.name())) {
                const auto success = encodeKnownTypesInternal(encoder, value.fields().value(field.name()), field.valueRank(), field.dataType());
                if (!success) {
                    qCWarning(lcGenericStructHandler) << "Failed to encode struct field" << field.name();
                    return false;
                }
            }
        }

        return true;
    } else if (value.structureDefinition().structureType() == QOpcUaStructureDefinition::StructureType::Union) {
        if (value.fields().size() > 1) {
            qCWarning(lcGenericStructHandler) << "Multiple union fields were specified, unable to encode";
            return false;
        }

        if (value.fields().size() > 0) {
            for (int i = 0; i < definitionFields.size(); ++i) {
                if (definitionFields.at(i).name() == value.fields().keys().first()) {
                    const auto success = encoder.encode<quint32>(1 << i);

                    if (!success) {
                        qCWarning(lcGenericStructHandler) << "Failed to encode union mask";
                        return false;
                    }

                    return encodeKnownTypesInternal(encoder, value.fields().constKeyValueBegin()->second,
                                                    definitionFields.at(i).valueRank(),
                                                    definitionFields.at(i).dataType());
                }
            }

            qCWarning(lcGenericStructHandler) << "Unknown union field" << value.fields().keys().first();
            return false;
        } else {
            // An empty union consists only of the mask
            return encoder.encode<quint32>(0);
        }
    } else {
        qCWarning(lcGenericStructHandler) << "Encoding failed, unknown struct type encountered";
        return false;
    }

    return false;
}

bool QOpcUaGenericStructHandlerPrivate::encodeKnownTypesInternal(QOpcUaBinaryDataEncoding &encoder, const QVariant &value,
                                                                 qint32 valueRank, const QString &dataTypeId)
{
    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Boolean))
        return encodeArrayOrScalar<bool>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Byte))
        return encodeArrayOrScalar<quint8>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::SByte))
        return encodeArrayOrScalar<qint8>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::UInt16))
        return encodeArrayOrScalar<quint16>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Int16))
        return encodeArrayOrScalar<qint16>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::UInt32))
        return encodeArrayOrScalar<quint32>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Int32))
        return encodeArrayOrScalar<qint32>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::UInt64))
        return encodeArrayOrScalar<quint64>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Int64))
        return encodeArrayOrScalar<qint64>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Float))
        return encodeArrayOrScalar<float>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Double))
        return encodeArrayOrScalar<double>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::StatusCode))
        return encodeArrayOrScalar<QOpcUa::UaStatusCode>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::DateTime))
        return encodeArrayOrScalar<QDateTime>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::String))
        return encodeArrayOrScalar<QString>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::NodeId))
        return encodeArrayOrScalar<QString, QOpcUa::Types::NodeId>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::ByteString))
        return encodeArrayOrScalar<QByteArray>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::XmlElement))
        return encodeArrayOrScalar<QByteArray>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Guid))
        return encodeArrayOrScalar<QUuid>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::QualifiedName))
        return encodeArrayOrScalar<QOpcUaQualifiedName>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::LocalizedText))
        return encodeArrayOrScalar<QOpcUaLocalizedText>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Range))
        return encodeArrayOrScalar<QOpcUaRange>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::EUInformation))
        return encodeArrayOrScalar<QOpcUaEUInformation>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::ComplexNumberType))
        return encodeArrayOrScalar<QOpcUaComplexNumber>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::DoubleComplexNumberType))
        return encodeArrayOrScalar<QOpcUaDoubleComplexNumber>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::AxisInformation))
        return encodeArrayOrScalar<QOpcUaAxisInformation>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::XVType))
        return encodeArrayOrScalar<QOpcUaXValue>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::ExpandedNodeId))
        return encodeArrayOrScalar<QOpcUaExpandedNodeId>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Argument))
        return encodeArrayOrScalar<QOpcUaArgument>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::StructureDefinition))
        return encodeArrayOrScalar<QOpcUaStructureDefinition>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::StructureField))
        return encodeArrayOrScalar<QOpcUaStructureField>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::EnumDefinition))
        return encodeArrayOrScalar<QOpcUaEnumDefinition>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::EnumField))
        return encodeArrayOrScalar<QOpcUaEnumField>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::DiagnosticInfo)) {
        return encodeArrayOrScalar<QOpcUaDiagnosticInfo>(encoder, valueRank, value);
    }

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::DataValue))
        return encodeArrayOrScalar<QOpcUaDataValue>(encoder, valueRank, value);

    if (dataTypeId == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::BaseDataType))
        return encodeArrayOrScalar<QOpcUaVariant>(encoder, valueRank, value);

    const auto enumType = m_enumsByTypeId.constFind(dataTypeId);
    if (enumType != m_enumsByTypeId.constEnd()) {
        if (enumType->isAbstract) {
            qCWarning(lcGenericStructHandler) << "Encoding abstract enum" << enumType->name << "requested";
            return false;
        }

        return encodeArrayOrScalar<qint32>(encoder, valueRank, value);
    }

    if (dataTypeKindForTypeId(dataTypeId) == QOpcUaGenericStructHandler::DataTypeKind::Struct) {
        if (valueRank == -1) {
            if (!value.canConvert<QOpcUaGenericStructValue>()) {
                qCWarning(lcGenericStructHandler) << "Struct value expected for member, unable to encode";
                return false;
            }

            const auto genericStruct = value.value<QOpcUaGenericStructValue>();

            if (genericStruct.typeId() != dataTypeId) {
                qCWarning(lcGenericStructHandler) << "Type mismatch for nested struct value, unable to encode";
                return false;
            }

            return encodeStructInternal(encoder, genericStruct);
        } else if (valueRank == 1) {
            if (!value.canConvert<QList<QOpcUaGenericStructValue>>()) {
                qCWarning(lcGenericStructHandler) << "Struct list value expected for member, unable to encode";
                return false;
            }

            const auto data = value.value<QList<QOpcUaGenericStructValue>>();

            auto success = encoder.encode<qint32>(data.size());

            if (!success) {
                qCWarning(lcGenericStructHandler) << "Failed to encode array length";
                return false;
            }

            for (const auto &entry : data) {
                success = encodeStructInternal(encoder, entry);

                if (!success) {
                    qCWarning(lcGenericStructHandler) << "Failed to encode struct into the array";
                    return false;
                }
            }

            return true;
        } else if (valueRank > 1) {
            if (!value.canConvert<QOpcUaMultiDimensionalArray>()) {
                qCWarning(lcGenericStructHandler) << "QOpcUaMultiDimensionalArray value expected for member, unable to encode";
                return false;
            }

            const auto array = value.value<QOpcUaMultiDimensionalArray>();

            const auto valueArray = array.valueArray();

            for (const auto &entry : valueArray) {
                if (!entry.canConvert<QOpcUaGenericStructValue>()) {
                    qCWarning(lcGenericStructHandler) << "QOpcUaMultiDimensionalArray value is expected to contain"
                                                         << "a generic struct, unable to encode";
                    return false;
                }
            }

            auto success = encoder.encodeArray<quint32>(array.arrayDimensions());

            if (!success) {
                qCWarning(lcGenericStructHandler) << "Failed to encode array dimensions";
                return false;
            }

            success = encoder.encode<qint32>(valueArray.size());

            if (!success) {
                qCWarning(lcGenericStructHandler) << "Failed to encode array length";
                return false;
            }

            for (const auto &entry : valueArray) {
                success = encodeStructInternal(encoder, entry.value<QOpcUaGenericStructValue>());

                if (!success) {
                    qCWarning(lcGenericStructHandler) << "Failed to encode struct into the array";
                    return false;
                }
            }

            return true;
        }

        return false;
    }

    // Maybe this is a supertype of a built-in type
    const auto superType = m_knownSubtypes.constFind(dataTypeId);
    if (superType != m_knownSubtypes.constEnd())
        return encodeKnownTypesInternal(encoder, value, valueRank, superType.value());

    return false;
}

void QOpcUaGenericStructHandlerPrivate::handleFinished(bool success)
{
    if (m_hasError)
        return;

    m_finishedCount++;
    m_hasError = !success;

    Q_Q(QOpcUaGenericStructHandler);

    if (m_finishedCount == 1 || m_hasError) {
        m_initialized = !m_hasError;
        emit q->initializedChanged(m_initialized);
    }
}

bool QOpcUaGenericStructHandlerPrivate::addCustomStructureDefinition(const QOpcUaStructureDefinition &definition,
                                                                     const QString &typeId, const QString &name, QOpcUa::IsAbstract isAbstract)
{
    if (typeId.isEmpty()) {
        qCWarning(lcGenericStructHandler) << "Failed to add custom structure definition, typeId must not be empty";
        return false;
    }

    if (name.isEmpty()) {
        qCWarning(lcGenericStructHandler) << "Failed to add custom structure definition, name must not be empty";
        return false;
    }

    if (definition.defaultEncodingId().isEmpty()) {
        qCWarning(lcGenericStructHandler) << "Failed to add custom structure definition, definition.defaultEncodingId() must not be empty";
        return false;
    }

    StructMapEntry entry;
    entry.isAbstract = isAbstract == QOpcUa::IsAbstract::Abstract;
    entry.name = name;
    entry.nodeId = typeId;
    entry.structureDefinition = definition;

    m_structuresByTypeId[typeId] = entry;
    m_structuresByEncodingId[definition.defaultEncodingId()] = entry;
    m_typeNamesByEncodingId[definition.defaultEncodingId()] = name;
    m_typeNamesByTypeId[typeId] = name;

    if (entry.isAbstract)
        m_abstractTypeIds.insert(typeId);

    return true;
}

bool QOpcUaGenericStructHandlerPrivate::addCustomEnumDefinition(const QOpcUaEnumDefinition &definition,
                                                                const QString &typeId, const QString &name,
                                                                QOpcUa::IsAbstract isAbstract)
{
    if (typeId.isEmpty()) {
        qCWarning(lcGenericStructHandler) << "Failed to add custom enum definition, typeId must not be empty";
        return false;
    }

    if (name.isEmpty()) {
        qCWarning(lcGenericStructHandler) << "Failed to add custom enum definition, name must not be empty";
        return false;
    }

    EnumMapEntry entry;
    entry.isAbstract = isAbstract == QOpcUa::IsAbstract::Abstract;
    entry.name = name;
    entry.nodeId = typeId;
    entry.enumDefinition = definition;

    m_enumsByTypeId[typeId] = entry;
    m_typeNamesByTypeId[typeId] = name;

    return true;
}

bool QOpcUaGenericStructHandlerPrivate::initialized() const
{
    return m_initialized;
}

void QOpcUaGenericStructHandlerPrivate::handleInitializeFinished(bool success)
{
    if (!success) {
        qCWarning(lcGenericStructHandler) << "Failed to read the data type tree";
        handleFinished(false);
        return;
    } else {
        processDataTypeRecursive(m_baseDataType.get());
        handleFinished(true);
    }
}

void QOpcUaGenericStructHandlerPrivate::processDataTypeRecursive(QOpcUaInternalDataTypeNode *node)
{
    qCDebug(lcGenericStructHandler) << "Found DataType:" << node->name();

    m_typeNamesByTypeId[node->nodeId()] = node->name();
    if (node->isAbstract())
        m_abstractTypeIds.insert(node->nodeId());

    for (const auto &child : node->children()) {
        if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Structure))
            processStructRecursive(child.get());
        else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Enumeration))
            processEnumRecursive(child.get());
        else {
            if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Boolean))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Int32))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::UInt32))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Double))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Float))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::String))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::LocalizedText))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::DateTime))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::UInt16))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Int16))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::UInt64))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Int64))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Byte))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::SByte))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::ByteString))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::XmlElement))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::NodeId))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Guid))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::QualifiedName))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else if (child->nodeId() == QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::StatusCode))
                processSubtypeOfKnownTypeRecursive(child.get(), child->nodeId());
            else
                processDataTypeRecursive(child.get());
        }
    }
}

void QOpcUaGenericStructHandlerPrivate::processStructRecursive(QOpcUaInternalDataTypeNode *node)
{
    qCDebug(lcGenericStructHandler) << "Found struct:" << node->name();

    m_typeNamesByTypeId.insert(node->nodeId(), node->name());

    if (node->isAbstract())
        m_abstractTypeIds.insert(node->nodeId());

    const auto structureDefinition = node->definition().value<QOpcUaStructureDefinition>();
    if (!structureDefinition.defaultEncodingId().isEmpty())
        m_typeNamesByEncodingId.insert(structureDefinition.defaultEncodingId(), node->name());

    const StructMapEntry entry{ node->name(), node->nodeId(), node->isAbstract(), structureDefinition };
    m_structuresByEncodingId.insert(structureDefinition.defaultEncodingId(), entry);
    m_structuresByTypeId.insert(node->nodeId(), entry);

    for (const auto &child : node->children())
        processStructRecursive(child.get());
}

void QOpcUaGenericStructHandlerPrivate::processEnumRecursive(QOpcUaInternalDataTypeNode *node)
{
    qCDebug(lcGenericStructHandler) << "Found enum:" << node->name();

    m_typeNamesByTypeId[node->nodeId()] = node->name();

    if (node->isAbstract())
        m_abstractTypeIds.insert(node->nodeId());

    EnumMapEntry entry;
    entry.name = node->name();
    entry.enumDefinition = node->definition().value<QOpcUaEnumDefinition>();
    entry.nodeId = node->nodeId();
    entry.isAbstract = node->isAbstract();
    m_enumsByTypeId.insert(node->nodeId(), entry);

    for (const auto &child : node->children())
        processEnumRecursive(child.get());
}

void QOpcUaGenericStructHandlerPrivate::processSubtypeOfKnownTypeRecursive(QOpcUaInternalDataTypeNode *node, const QString &id)
{
    m_typeNamesByTypeId[node->nodeId()] = node->name();

    if (node->isAbstract())
        m_abstractTypeIds.insert(node->nodeId());

    if (node->nodeId() != id)
        m_knownSubtypes[node->nodeId()] = id;

    for (const auto &child : node->children())
        processSubtypeOfKnownTypeRecursive(child.get(), id);
}

QT_END_NAMESPACE

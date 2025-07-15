// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "util.h"
#include "enumeratedtype.h"
#include "field.h"
#include "mappingfilegenerator.h"
#include "stringidentifier.h"
#include "structuredtype.h"

#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/qmap.h>
#include <QtCore/qset.h>

using namespace Qt::Literals::StringLiterals;

MappingFileGenerator::MappingFileGenerator(const QList<XmlElement *> &generateMapping,
                                           const QString &path,
                                           const QString &prefix,
                                           const QString &header)
    : m_generateMapping(generateMapping)
    , m_path(path)
    , m_prefix(prefix)
    , m_header(header)
{
    for (const auto &type : std::as_const(m_generateMapping)) {
        const auto structuredType = dynamic_cast<StructuredType *>(type);
        if (structuredType) {
            const auto typeFields = structuredType->fields();
            for (const auto &possibleField : typeFields) {
                for (const auto &field : typeFields) {
                    if (possibleField->name() == field->lengthField()
                        && !m_lengthFields.contains(possibleField))
                        m_lengthFields.push_back(possibleField);

                    if (!m_switchFields.contains(possibleField)
                        && possibleField->name() == field->switchField())
                        m_switchFields.push_back(possibleField);
                }
            }
        }
    }
}

void MappingFileGenerator::addGenerateMapping(
    const QList<XmlElement *> &generateMapping)
{
    for (const auto &mapping : generateMapping) {
        if (!m_generateMapping.contains(mapping))
            m_generateMapping.push_back(mapping);
    }
    for (const auto &mapping : std::as_const(m_generateMapping)) {
        const auto structuredType = dynamic_cast<StructuredType *>(mapping);
        if (structuredType) {
            const auto typeFields = structuredType->fields();
            for (const auto &possibleField : typeFields) {
                for (const auto &field : typeFields) {
                    if (possibleField->name() == field->lengthField()
                        && !m_lengthFields.contains(possibleField))
                        m_lengthFields.push_back(possibleField);

                    if (!m_switchFields.contains(possibleField)
                        && possibleField->name() == field->switchField())
                        m_switchFields.push_back(possibleField);
                }
            }
        }
    }
}

MappingFileGenerator::MappingError MappingFileGenerator::generateMapping()
{
    auto error = sortMappings();
    if (error != NoError)
        return error;

    if (generateMappingHeader() == NoError)
        error = generateMappingCpp();
    else
        error = UnanbleToWrite;
    return error;
}

MappingFileGenerator::MappingError MappingFileGenerator::generateMappingHeader()
{
    QFile file;
    const auto fileName = u"%1binarydeencoder%2"_s
                              .arg(m_prefix.toLower(), StringIdentifier::headerIdentifier);
    QDir dir(m_path);
    if (!dir.exists(m_path))
        if (!dir.mkpath(m_path))
            return UnanbleToWrite;
    file.setFileName(dir.absoluteFilePath(fileName));

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return UnanbleToWrite;
    QTextStream output(&file);

    // Print header (if present)
    if (!m_header.isEmpty())
        output << m_header << Util::lineBreak(2);
    output << "#pragma once"
           << Util::lineBreak(2);
    QList<QString> includes;
    for (auto mapping : std::as_const(m_generateMapping)) {
        const auto enumeratedType = dynamic_cast<EnumeratedType *>(mapping);
        if (enumeratedType) {
            if (!includes.contains(
                    u"#include \"%1enumerations.h\"%2"_s.arg(m_prefix.toLower(), Util::lineBreak())))
                includes.push_back(
                    u"#include \"%1enumerations.h\"%2"_s.arg(m_prefix.toLower(), Util::lineBreak()));
        } else {
            const auto structuredType = dynamic_cast<StructuredType *>(mapping);
            if (structuredType)
                if (!includes.contains(
                        u"#include \"%1%2.h\"%3"_s
                            .arg(m_prefix.toLower(), structuredType->name().toLower(), Util::lineBreak())))
                    includes.push_back(
                        u"#include \"%1%2.h\"%3"_s
                            .arg(m_prefix.toLower(), structuredType->name().toLower(), Util::lineBreak()));
        }
    }

    includes.sort();
    for (const auto &include : includes) {
        output << include;
    }
    output << Util::lineBreak();
    output << "#include <QtOpcUa/QOpcUaBinaryDataEncoding>"
           << Util::lineBreak();
    output << "#include <QtOpcUa/QOpcUaExtensionObject>"
           << Util::lineBreak();
    output << Util::lineBreak();

    output << "class " << m_prefix << "BinaryDeEncoder"
           << Util::lineBreak();
    output << "{"
           << Util::lineBreak();
    output << "public:"
           << Util::lineBreak();
    output << Util::lineBreak();
    output << Util::indent(1) << m_prefix << "BinaryDeEncoder(QByteArray *buffer);"
           << Util::lineBreak();
    output << Util::indent(1) << m_prefix << "BinaryDeEncoder(QOpcUaExtensionObject &object);"
           << Util::lineBreak();
    output << Util::lineBreak();
    output << Util::indent(1) << "template <typename T>"
           << Util::lineBreak();
    output << Util::indent(1) << "T decode(bool &success);"
           << Util::lineBreak();
    output << Util::lineBreak();
    output << Util::indent(1) << "template <typename T>"
           << Util::lineBreak();
    output << Util::indent(1) << "QList<T> decodeArray(bool &success);"
           << Util::lineBreak();
    output << Util::lineBreak();
    output << Util::indent(1) << "template <typename T>"
           << Util::lineBreak();
    output << Util::indent(1) << "bool encode(const T &src);"
           << Util::lineBreak();
    output << Util::lineBreak();
    output << Util::indent(1) << "template <typename T>"
           << Util::lineBreak();
    output << Util::indent(1) << "bool encodeArray(const QList<T> &src);"
           << Util::lineBreak();
    output << Util::lineBreak();
    output << Util::indent(1) << "QOpcUaBinaryDataEncoding &binaryDataEncoding();"
           << Util::lineBreak();
    output << Util::lineBreak();

    output << "private:"
           << Util::lineBreak();
    output << Util::indent(1) << "QScopedPointer<QOpcUaBinaryDataEncoding> m_binaryDataEncoding;"
           << Util::lineBreak();

    output << "};"
           << "\n\n";
    generateDeEncodingArray(output);
    generateDeEncoding(output);
    return NoError;
}

MappingFileGenerator::MappingError MappingFileGenerator::generateMappingCpp()
{
    QFile file;
    QDir dir(m_path);
    const auto fileName = u"%1binarydeencoder%2"_s
                              .arg(m_prefix.toLower(), StringIdentifier::cppIdentifier);
    if (!dir.exists(m_path))
        if (!dir.mkpath(m_path))
            return UnanbleToWrite;
    file.setFileName(dir.absoluteFilePath(fileName));

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return UnanbleToWrite;

    QTextStream output(&file);

    // Print header (if present)
    if (!m_header.isEmpty())
        output << m_header << "\n\n";

    output << "#include \"" << m_prefix.toLower() << "binarydeencoder.h\"" << Util::lineBreak();
    output << Util::lineBreak();
    output << m_prefix << "BinaryDeEncoder::" << m_prefix << "BinaryDeEncoder (QByteArray *buffer)"
           << Util::lineBreak();
    output << Util::indent(1) << ": m_binaryDataEncoding(new QOpcUaBinaryDataEncoding(buffer))"
           << Util::lineBreak();
    output << "{"
           << Util::lineBreak();
    output << "}"
           << Util::lineBreak(2);
    output << m_prefix << "BinaryDeEncoder::" << m_prefix
           << "BinaryDeEncoder "
              "(QOpcUaExtensionObject &object) "
           << Util::lineBreak();
    output << Util::indent(1) << ": m_binaryDataEncoding(new "
              "QOpcUaBinaryDataEncoding(&object.encodedBodyRef()))"
           << Util::lineBreak();
    output << "{"
           << Util::lineBreak();
    output << "}"
           << Util::lineBreak(2);
    output << "QOpcUaBinaryDataEncoding &" << m_prefix << "BinaryDeEncoder::binaryDataEncoding()"
           << Util::lineBreak();
    output << "{"
           << Util::lineBreak();
    output << Util::indent(1) << "return *m_binaryDataEncoding.data();"
           << Util::lineBreak();
    output << "}"
           << Util::lineBreak();
    return NoError;
}

void MappingFileGenerator::generateFieldDecoding(QTextStream &output, const StructuredType *structuredType, const Field *field, int level)
{
    output << Util::indent(level) << "temp.set" << field->name() << "(";
    generateScalarOrArrayDecoding(output, structuredType, field);
    output << ");"  << Util::lineBreak();
    output << Util::indent(level) << "if (!success)" << Util::lineBreak();
    output << Util::indent(level + 1) << "return {};" << Util::lineBreak();
}

void MappingFileGenerator::generateScalarOrArrayDecoding(QTextStream &output, const StructuredType *structuredType, const Field *field)
{
    Q_UNUSED(structuredType);
    const auto builtinType = StringIdentifier::typeNameDataTypeConverter.constFind(field->typeName());
    if (builtinType != StringIdentifier::typeNameDataTypeConverter.constEnd()) {
        output << "m_binaryDataEncoding->decode" << (field->lengthField().isEmpty() ? "" : "Array") << "<" << builtinType.value() <<  ">(success)";
        return;
    }

    bool isPrecoded = false;
    for (const auto &precodedType : StringIdentifier::opcUaPrecodedTypes) {
        if (precodedType.contains(Util::removeNamespace(field->typeName()))) {
            output << "m_binaryDataEncoding->decode";
            if (!field->lengthField().isEmpty())
                output << "Array";
            output << "<";
            if (!precodedType.deEncoderName().isEmpty())
                output << precodedType.deEncoderName() << ", QOpcUa::Types::NodeId";
            else
                output << precodedType.className();
            output << ">(success)";
            isPrecoded = true;
            break;
        }
    }
    if (!isPrecoded) {
        output << "decode";
        if (!field->lengthField().isEmpty())
            output << "Array";
        output << "<" << m_prefix << (field->isEnum() ? "::" : "") << Util::removeNamespace(field->typeName()) << ">(success)";
    }
}

void MappingFileGenerator::generateOptionalFieldDecoding(QTextStream &output, const StructuredType *structuredType, const Field *field, int level)
{
    int index = -1;
    QSet<QString> usedSwitchFields;
    const auto typeFields = structuredType->fields();
    for (const auto &current : typeFields) {
        if (!current->switchField().isEmpty() && !usedSwitchFields.contains(current->switchField())) {
            index++;
            usedSwitchFields.insert(current->switchField());
        }

        if (current == field)
            break;
    }

    output << Util::indent(level) << "if (decodingMask & " << u"(1 << %1)"_s.arg(index) << ") {" << Util::lineBreak();
    generateFieldDecoding(output, structuredType, field, level + 1);
    output << Util::indent(level + 1) << "temp.set" << field->switchField() << "(true);" << Util::lineBreak();
    output << Util::indent(level) << "}" << Util::lineBreak();
}

void MappingFileGenerator::generateUnionFieldDecoding(QTextStream &output, const StructuredType *structuredType, const Field *field, int level)
{
    output << Util::indent(level) << u"if (switchField == %1) {"_s.arg(field->switchValue()) << Util::lineBreak();
    generateFieldDecoding(output, structuredType, field, level + 1);
    output << Util::indent(level) << "}" << Util::lineBreak();
}

void MappingFileGenerator::generateFieldEncoding(QTextStream &output, const StructuredType *structuredType, const Field *field, int level)
{
    output << Util::indent(level) << "if (!";
    generateScalarOrArrayEncoding(output, structuredType, field);
    output << ")" << Util::lineBreak();
    output << Util::indent(level + 1) << "return false;" << Util::lineBreak();
}

void MappingFileGenerator::generateScalarOrArrayEncoding(QTextStream &output, const StructuredType *structuredType, const Field *field)
{
    Q_UNUSED(structuredType);
    const auto builtinType = StringIdentifier::typeNameDataTypeConverter.constFind(field->typeName());
    if (builtinType != StringIdentifier::typeNameDataTypeConverter.constEnd()) {
        output << "m_binaryDataEncoding->encode" << (field->lengthField().isEmpty() ? "" : "Array") << "<" << builtinType.value() << ">(src." << field->lowerFirstName() << "())";
        return;
    }

    bool isPrecoded = false;
    for (const auto &precodedType : StringIdentifier::opcUaPrecodedTypes) {
        if (precodedType.contains(Util::removeNamespace(field->typeName()))) {
            output << "m_binaryDataEncoding->encode";
            if (!field->lengthField().isEmpty())
                output << "Array";
            output << "<";
            if (!precodedType.deEncoderName().isEmpty())
                output << precodedType.deEncoderName() << ", QOpcUa::Types::NodeId";
            else
                output << precodedType.className();
            output << ">(src." << field->lowerFirstName() << "())";
            isPrecoded = true;
            break;
        }
    }
    if (!isPrecoded) {
        output << "encode";
        if (!field->lengthField().isEmpty())
            output << "Array";
        output << "<" << m_prefix << (field->isEnum() ? "::" : "") << Util::removeNamespace(field->typeName()) << ">(src." << field->lowerFirstName() << "())";
    }
}

void MappingFileGenerator::generateOptionalFieldEncoding(QTextStream &output, const StructuredType *structuredType, const Field *field, int level)
{
    output << Util::indent(level) << "if (src." << Util::lowerFirstLetter(field->switchField()) << "()) {" << Util::lineBreak();
    generateFieldEncoding(output, structuredType, field, level + 1);
    output << Util::indent(level) << "}" << Util::lineBreak();
}

void MappingFileGenerator::generateUnionFieldEncoding(QTextStream &output, const StructuredType *structuredType, const Field *field, int level)
{
    output << Util::indent(level) << "if (static_cast<quint32>(src." << structuredType->fields().constFirst()->lowerFirstName() << "()) == " << field->switchValue() << ") {" << Util::lineBreak();
    generateFieldEncoding(output, structuredType, field, level + 1);
    output << Util::indent(level) << "}" << Util::lineBreak();
}

void MappingFileGenerator::generateDeEncodingArray(QTextStream &output)
{
    output << "template<typename T>"
           << Util::lineBreak();
    output << "inline QList<T> " << m_prefix << "BinaryDeEncoder::decodeArray(bool &success)"
           << Util::lineBreak();
    output << "{"
           << Util::lineBreak();
    output << Util::indent(1) << "QList<T> temp;"
           << Util::lineBreak(2);
    output << Util::indent(1) << "qint32 size = m_binaryDataEncoding->decode<qint32>(success);"
           << Util::lineBreak();
    output << Util::indent(1) << "if (!success)"
           << Util::lineBreak();
    output << Util::indent(2) << "return {};";
    output << Util::lineBreak(2);
    output << Util::indent(1) << "for (int i = 0; i < size; ++i) {"
           << Util::lineBreak();
    output << Util::indent(2) << "temp.push_back(decode<T>(success));"
           << Util::lineBreak();
    output << Util::indent(2) << "if (!success)"
           << Util::lineBreak();
    output << Util::indent(3) << "return {};"
           << Util::lineBreak();
    output << Util::indent(1) << "}"
           << Util::lineBreak(2);
    output << Util::indent(1) << "return temp;"
           << Util::lineBreak();
    output << "}"
           << Util::lineBreak(2);
    output << "template<typename T>"
           << Util::lineBreak();
    output << "inline bool " << m_prefix << "BinaryDeEncoder::encodeArray(const QList<T> &src)"
           << Util::lineBreak();
    output << "{"
           << Util::lineBreak();
    output << Util::indent(1) << "if (src.size() > (std::numeric_limits<qint32>::max()))"
           << Util::lineBreak();
    output << Util::indent(2) << "return false;"
           << Util::lineBreak(2);
    output << Util::indent(1) << "if (!m_binaryDataEncoding->encode<qint32>(src.size()))"
           << Util::lineBreak();
    output << Util::indent(2) << "return false;"
           << Util::lineBreak();
    output << Util::indent(1) << "for (const auto &element : src) {"
           << Util::lineBreak();
    output << Util::indent(2) << "if (!encode<T>(element))"
           << Util::lineBreak();
    output << Util::indent(3) << "return false;"
           << Util::lineBreak();
    output << Util::indent(1) << "}"
           << Util::lineBreak();
    output << Util::indent(1) << "return true;"
           << Util::lineBreak();
    output << "}"
           << Util::lineBreak(2);
}

void MappingFileGenerator::generateDeEncoding(QTextStream &output)
{
    for (const auto &mapping : std::as_const(m_generateMapping)) {
        const auto enumeratedType = dynamic_cast<EnumeratedType *>(mapping);
        if (enumeratedType) {
            generateDecodingEnumeratedType(output, enumeratedType);
            generateEncodingEnumeratedType(output, enumeratedType);
        } else {
            const auto structuredType = dynamic_cast<StructuredType *>(mapping);
            if (structuredType) {
                generateDecodingStructuredType(output, structuredType);
                generateEncodingStructuredType(output, structuredType);
            }
        }
    }
}

void MappingFileGenerator::generateDecodingEnumeratedType(QTextStream &output,
                                                          const EnumeratedType *enumeratedType)
{
    output << "template<>"
           << Util::lineBreak();
    output << "inline " << m_prefix << "::" << enumeratedType->name() << " " << m_prefix
           << "BinaryDeEncoder::decode(bool &success)"
           << Util::lineBreak();
    output << "{"
           << Util::lineBreak();
    output << Util::indent(1) << "return static_cast<" << m_prefix << "::" << enumeratedType->name() << ">("
           "m_binaryDataEncoding->decode<qint32>(success));"
           << Util::lineBreak();
    output << "}"
           << Util::lineBreak(2);
}

void MappingFileGenerator::generateEncodingEnumeratedType(QTextStream &output,
                                                          const EnumeratedType *enumeratedType)
{
    output << "template<>"
           << Util::lineBreak();
    output << "inline bool " << m_prefix << "BinaryDeEncoder"
           << "::encode<" << m_prefix << "::" << enumeratedType->name() << ">(const " << m_prefix
           << "::" << enumeratedType->name() << " &src)"
           << Util::lineBreak();
    output << "{"
           << Util::lineBreak();
    output << Util::indent(1) << "return m_binaryDataEncoding->encode<qint32>(static_cast<qint32>(src));"
           << Util::lineBreak();
    output << "}"
           << Util::lineBreak(2);
}

void MappingFileGenerator::generateDecodingStructuredType(QTextStream &output,
                                                          const StructuredType *structuredType)
{
    output << "template<>"
           << Util::lineBreak();
    output << "inline " << m_prefix << structuredType->name() << " " << m_prefix
           << "BinaryDeEncoder::decode<" << m_prefix << structuredType->name() << ">(bool &success)" << Util::lineBreak()
           << "{" << Util::lineBreak();
    output << Util::indent(1) << m_prefix << structuredType->name() << " temp;"
           << Util::lineBreak(2);

    if (structuredType->containsBitMask()) {
        output << Util::indent(1) <<"const auto decodingMask = "
               << "m_binaryDataEncoding->decode<quint32>(success);" << Util::lineBreak()
               << Util::indent(1) << "if (!success)" << Util::lineBreak()
               << Util::indent(2) << "return {};" << Util::lineBreak(2);
    } else if (structuredType->hasUnion()) {
        output << Util::indent(1) <<"const auto switchField = "
               << "m_binaryDataEncoding->decode<quint32>(success);" << Util::lineBreak()
               << Util::indent(1) << "if (!success)" << Util::lineBreak()
               << Util::indent(2) << "return {};" << Util::lineBreak(2);
    }

    const auto typeFields = structuredType->fields();
    for (const auto &field : typeFields) {
        if (field->isInStructuredTypeBitMask())
            continue;

        if (m_lengthFields.contains(field))
            continue;

        // The switch field is not a member of the generated data class
        if (structuredType->hasUnion() && field == typeFields.constFirst())
            continue;

        if (!field->switchField().isEmpty() && !field->isUnion())
            generateOptionalFieldDecoding(output, structuredType, field);
        else if (structuredType->hasUnion())
            generateUnionFieldDecoding(output, structuredType, field);
        else
            generateFieldDecoding(output, structuredType, field);

        output << Util::lineBreak();
    }

    output <<   Util::indent(1) << "return temp;" << Util::lineBreak();
    output << "}" << Util::lineBreak(2);
}

void MappingFileGenerator::generateEncodingStructuredType(QTextStream &output,
                                                          const StructuredType *structuredType)
{
    output << "template<>"
           << Util::lineBreak();
    output << "inline bool " << m_prefix << "BinaryDeEncoder"
           << "::encode<" << m_prefix << structuredType->name() << ">(const " << m_prefix
           << structuredType->name() << " &src)"
           << Util::lineBreak();
    output << "{"
           << Util::lineBreak();

    const auto typeFields = structuredType->fields();

    // Create encoding mask for bitmask
    if (structuredType->hasSwitchfield() && !structuredType->hasUnion()) {
        output << Util::indent(1) << "quint32 encodingMask = 0;" << Util::lineBreak();
        int index = 0;
        for (const auto &field : typeFields) {
            if (!m_switchFields.contains(field))
                continue;
            output << Util::indent(1) << "encodingMask |= ((src." << field->lowerFirstName() << "() ? 1 : 0) << " << index++ << ");" << Util::lineBreak();
        }
        output << Util::lineBreak();
        output << Util::indent(1) << "if (!m_binaryDataEncoding->encode<quint32>(encodingMask))" << Util::lineBreak();
        output << Util::indent(2) << "return false;" << Util::lineBreak(2);
    } else if (structuredType->hasUnion()) {
        output << Util::indent(1) << "if (!m_binaryDataEncoding->encode<quint32>(static_cast<quint32>(src." << typeFields.constFirst()->lowerFirstName() << "())))" << Util::lineBreak();
        output << Util::indent(2) << "return false;" << Util::lineBreak(2);
    }

    for (const auto &field : typeFields) {
        if (m_lengthFields.contains(field))
            continue;

        if (m_switchFields.contains(field))
            continue;

        // Ignore reserved fields
        if (field->isInStructuredTypeBitMask())
            continue;

        if (structuredType->hasUnion())
            generateUnionFieldEncoding(output, structuredType, field);
        else if (!field->switchField().isEmpty() && !field->isUnion())
            generateOptionalFieldEncoding(output, structuredType, field);
        else
            generateFieldEncoding(output, structuredType, field);

        output << Util::lineBreak();
    }

    output << Util::indent(1) << "return true;" << Util::lineBreak();
    output << "}" << Util::lineBreak(2);
}

MappingFileGenerator::MappingError MappingFileGenerator::sortMappings()
{
    QList<XmlElement *> baseType;
    QList<XmlElement *> extendedType;
    for (const auto &mapping : std::as_const(m_generateMapping)) {
        const auto structuredType = dynamic_cast<StructuredType *>(mapping);
        if (structuredType) {
            const auto typeFields = structuredType->fields();
            bool isBaseType = true;
            for (const auto &field : typeFields) {
                bool isPreCoded = false;
                for (const auto &precodedType : StringIdentifier::opcUaPrecodedTypes) {
                    if (precodedType.contains(Util::removeNamespace(field->typeName()))) {
                        isPreCoded = true;
                        break;
                    }
                }
                if (!isPreCoded)
                    if (!StringIdentifier::typeNameDataTypeConverter.contains(field->typeName()))
                        if (Util::removeNamespace(field->typeName()) != structuredType->name())
                            isBaseType = false;
            }
            if (isBaseType)
                baseType.push_back(structuredType);
            else
                extendedType.push_back(structuredType);
        } else {
            baseType.push_front(mapping);
        }
    }
    m_generateMapping = baseType;

    while (!extendedType.isEmpty()) {
        QList<XmlElement *> tmpExtendedType = extendedType;
        for (int i = 0; i < extendedType.size(); i++) {
            const auto structuredType = dynamic_cast<StructuredType *>(extendedType.at(i));
            if (structuredType) {
                const auto typeFields = structuredType->fields();
                bool independentExtended = true;
                for (const auto &field : typeFields) {
                    bool isPreCoded = false;
                    for (const auto &precodedType : StringIdentifier::opcUaPrecodedTypes) {
                        if (precodedType.contains(Util::removeNamespace(field->typeName()))) {
                            isPreCoded = true;
                            break;
                        }
                    }
                    bool basicdependecy = false;
                    if (StringIdentifier::typeNameDataTypeConverter.contains(field->typeName()))
                        basicdependecy = true;
                    else if (!isPreCoded
                             && Util::removeNamespace(field->typeName()) != structuredType->name()) {
                        for (const auto &type : std::as_const(m_generateMapping)) {
                            if (type->name() == Util::removeNamespace(field->typeName())) {
                                basicdependecy = true;
                            }
                        }
                    } else if (!isPreCoded
                               && Util::removeNamespace(field->typeName()) == structuredType->name()) {
                        basicdependecy = true;
                    } else if (isPreCoded)
                        basicdependecy = true;
                    independentExtended &= basicdependecy;
                }

                if (independentExtended) {
                    m_generateMapping.append(structuredType);
                    extendedType.removeAt(extendedType.indexOf(structuredType));
                }
            }
        }
        if (extendedType == tmpExtendedType) {
            return MappingFileGenerator::MappingError::UnableToResolve;
        }
    }
    return MappingFileGenerator::MappingError::NoError;
}

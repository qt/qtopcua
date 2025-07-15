// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "util.h"
#include "datatypefilewriter.h"
#include "dependencydatatypevalidator.h"
#include "enumeratedtype.h"
#include "enumeratedvalue.h"
#include "field.h"
#include "import.h"
#include "mappingfilegenerator.h"
#include "recursivedescentparser.h"
#include "stringidentifier.h"
#include "structuredtype.h"
#include "typedictionary.h"

#include <QtCore/qfile.h>

using namespace Qt::Literals::StringLiterals;

RecursiveDescentParser::~RecursiveDescentParser()
{
    qDeleteAll(m_mapTypeDictionary);
}

void RecursiveDescentParser::printInOrder() const
{
    for (const auto &entry : m_mapTypeDictionary) {
        entry->print();
    }
}

RecursiveDescentParser::ParsingError RecursiveDescentParser::parseEnumeratedType(
    QXmlStreamReader &xmlStreamReader, TypeDictionary *typeDictionary)
{
    EnumeratedType *enumeratedType = new EnumeratedType(
        xmlStreamReader.attributes().value(StringIdentifier::nameIdentifier).toString(),
        xmlStreamReader.attributes().value(StringIdentifier::lengthInBitsIdentifier).toInt());
    typeDictionary->addType(enumeratedType);
    xmlStreamReader.readNext();
    ParsingError error = NoError;
    while (!xmlStreamReader.atEnd() && error == NoError) {
        if (xmlStreamReader.isEndElement()) {
            xmlStreamReader.readNext();
            return error;
        }
        if (xmlStreamReader.isStartElement()) {
            if (xmlStreamReader.name() == StringIdentifier::enumeratedValueIdentifier)
                error = parseEnumeratedValue(xmlStreamReader, enumeratedType);
            else {
                if (isKnownElement(xmlStreamReader))
                    error = skipKnownElement(xmlStreamReader);
                else
                    error = InvalidEnumeratedTypeEntry;
            }
        } else
            xmlStreamReader.readNext();
    }
    return error;
}

RecursiveDescentParser::ParsingError RecursiveDescentParser::parseEnumeratedValue(
    QXmlStreamReader &xmlStreamReader, EnumeratedType *enumeratedType)
{
    const auto name = permittedName(xmlStreamReader.attributes().value(StringIdentifier::nameIdentifier).toString());

    if (name.isEmpty())
        return ParsingError::InvalidEnumeratedValueEntry;

    EnumeratedValue *enumeratedValue = new EnumeratedValue(
        name, xmlStreamReader.attributes().value(StringIdentifier::valueIdentifier).toInt());
    enumeratedType->addValue(enumeratedValue);
    ParsingError error = NoError;
    xmlStreamReader.readNext();
    while (!xmlStreamReader.atEnd() && error == NoError) {
        if (xmlStreamReader.isEndElement()) {
            xmlStreamReader.readNext();
            return error;
        } else {
            if (isKnownElement(xmlStreamReader))
                error = skipKnownElement(xmlStreamReader);
            else
                error = InvalidEnumeratedValueEntry;
        }
    }
    return error;
}

RecursiveDescentParser::ParsingError RecursiveDescentParser::parseField(
    QXmlStreamReader &xmlStreamReader, StructuredType *structuredType)
{
    const auto name = permittedName(xmlStreamReader.attributes().value(StringIdentifier::nameIdentifier).toString());

    if (name.isEmpty())
        return ParsingError::InvalidFieldEntry;

    Field *field = new Field(
        name,
        xmlStreamReader.attributes().value(StringIdentifier::typeNameIdentifier).toString(),
        xmlStreamReader.attributes().value(StringIdentifier::lengthFieldIdentifier).toString());
    if (!field->lengthField().isEmpty())
        field->setNeedContainer(true);

    if (field->typeName().endsWith(u":%1"_s.arg(structuredType->name())))
        field->setRecursive(true);

    if (!xmlStreamReader.attributes().value(StringIdentifier::lengthIdentifier).toString().isEmpty()) {
        field->setLength(xmlStreamReader.attributes().value(StringIdentifier::lengthIdentifier).toUInt());
        field->setIsInStructuredTypeBitMask(true);
    }
    if (!xmlStreamReader.attributes()
             .value(StringIdentifier::switchFieldIdentifier)
             .toString()
             .isEmpty()) {
        structuredType->setHasSwitchfield(true);
        field->setSwitchField(
            xmlStreamReader.attributes().value(StringIdentifier::switchFieldIdentifier).toString());
    }
    if (!xmlStreamReader.attributes()
             .value(StringIdentifier::switchValueIdentifier)
             .toString()
             .isEmpty()) {
        structuredType->setHasUnion(true);
        field->setIsUnion(true);
        field->setSwitchValue(
            xmlStreamReader.attributes().value(StringIdentifier::switchValueIdentifier).toInt());
    }
    if (field->typeName() == StringIdentifier::opcBitIdentifier)
        structuredType->setContainsBitMask(true);
    if (field->typeName().contains(StringIdentifier::bitIdentifier)
        && (field->name().contains(StringIdentifier::specifiedIdentifier)
            || field->name().contains(StringIdentifier::reservedIdentifier))
        && structuredType->containsBitMask())
        field->setIsInStructuredTypeBitMask(true);
    structuredType->addField(field);
    ParsingError error = NoError;
    xmlStreamReader.readNext();
    while (!xmlStreamReader.atEnd() && error == NoError) {
        if (xmlStreamReader.isEndElement()) {
            xmlStreamReader.readNext();
            return error;
        } else {
            if (isKnownElement(xmlStreamReader))
                error = skipKnownElement(xmlStreamReader);
            else
                error = InvalidFieldEntry;
        }
    }
    return error;
}

RecursiveDescentParser::ParsingError RecursiveDescentParser::parseFile(
    const QString &fileName, const bool &dependencyTypeDictionary)
{
    ParsingError error = NoError;
    QFile xmlFile(fileName);
    if (!xmlFile.open(QIODevice::ReadOnly))
        return InvalidFileName;
    QXmlStreamReader xmlStreamReader;
    xmlStreamReader.setDevice((&xmlFile));
    while (!xmlStreamReader.atEnd() && error == NoError) {
        if (xmlStreamReader.isStartElement()) {
            if (xmlStreamReader.name() == StringIdentifier::typeDictionaryIdentifier)
                error = parseTypeDictionary(xmlStreamReader, dependencyTypeDictionary);
            else {
                if (isKnownElement(xmlStreamReader))
                    error = skipKnownElement(xmlStreamReader);
                else
                    error = InvalidTypeDictionaryEntry;
            }
        } else
            xmlStreamReader.readNext();
    }
    xmlFile.close();
    return error;
}

RecursiveDescentParser::ParsingError RecursiveDescentParser::parseImport(
    QXmlStreamReader &xmlStreamReader, TypeDictionary *typeDictionary)
{
    Import *import = new Import(
        xmlStreamReader.attributes().value(StringIdentifier::nameSpaceIdentifier).toString(),
        xmlStreamReader.attributes().value(StringIdentifier::locationIdentifier).toString());
    typeDictionary->addType(import);
    xmlStreamReader.readNext();
    ParsingError error = NoError;
    while (!xmlStreamReader.atEnd() && error == NoError) {
        if (xmlStreamReader.isEndElement()) {
            xmlStreamReader.readNext();
            return error;
        } else {
            if (isKnownElement(xmlStreamReader))
                error = skipKnownElement(xmlStreamReader);
            else
                error = InvalidImportEntry;
        }
    }
    return error;
}

RecursiveDescentParser::ParsingError RecursiveDescentParser::parseStructuredType(
    QXmlStreamReader &xmlStreamReader, TypeDictionary *typeDictionary)
{
    StructuredType *structuredType = new StructuredType(
        xmlStreamReader.attributes().value(StringIdentifier::nameIdentifier).toString(),
        xmlStreamReader.attributes().value(StringIdentifier::baseTypeIdentifier).toString());
    if (StringIdentifier::buildInTypesWithBitMask.contains(structuredType->name())) {
        structuredType->setIsBuiltInType(true);
        structuredType->setContainsBitMask(true);
    }
    structuredType->setTargetNamespace(typeDictionary->targetNamespace());
    typeDictionary->addType(structuredType);
    xmlStreamReader.readNext();
    ParsingError error = NoError;
    while (!xmlStreamReader.atEnd() && error == NoError) {
        if (xmlStreamReader.isEndElement()) {
            xmlStreamReader.readNext();

            const auto tempFields = structuredType->fields();
            for (const auto &field : tempFields) {
                if (field->recursive()) {
                    structuredType->setRecursive(true);
                    break;
                }
            }
            return error;
        }
        if (xmlStreamReader.isStartElement()) {
            if (xmlStreamReader.name() == StringIdentifier::fieldIdentifier)
                error = parseField(xmlStreamReader, structuredType);
            else {
                if (isKnownElement(xmlStreamReader))
                    error = skipKnownElement(xmlStreamReader);
                else
                    error = InvalidStructuredTypeEntry;
            }
        } else
            xmlStreamReader.readNext();
    }

    return error;
}

RecursiveDescentParser::ParsingError RecursiveDescentParser::parseTypeDictionary(
    QXmlStreamReader &xmlStreamReader, const bool &dependencyTypeDictionary)
{
    QMap<QString, QString> namespaces;
    for (int i = 0; i < xmlStreamReader.namespaceDeclarations().size(); i++)
        namespaces.insert(xmlStreamReader.namespaceDeclarations().at(i).prefix().toString(),
                          xmlStreamReader.namespaceDeclarations().at(i).namespaceUri().toString());
    m_mapTypeDictionary.insert(
        xmlStreamReader.attributes().value(StringIdentifier::targetNamespaceIdentifier).toString(),
        new TypeDictionary(dependencyTypeDictionary,
                           xmlStreamReader.attributes()
                               .value(StringIdentifier::defaultByteOrderIdentifier)
                               .toString(),
                           xmlStreamReader.attributes()
                               .value(StringIdentifier::targetNamespaceIdentifier)
                               .toString(),
                           namespaces));
    QString targetNamespace
        = xmlStreamReader.attributes().value(StringIdentifier::targetNamespaceIdentifier).toString();
    ParsingError error = NoError;
    if (!dependencyTypeDictionary && targetNamespace == StringIdentifier::namespaceZeroIdentifier)
        return CannotFullyGenerateNamespaceZero;
    xmlStreamReader.readNext();
    while (!xmlStreamReader.atEnd() && error == NoError) {
        if (xmlStreamReader.isEndElement()) {
            xmlStreamReader.readNext();
            return error;
        }
        if (xmlStreamReader.isStartElement()) {
            if (xmlStreamReader.name() == StringIdentifier::enumeratedTypeIdentifier)
                error = parseEnumeratedType(xmlStreamReader,
                                            m_mapTypeDictionary.value(targetNamespace));
            else {
                if (xmlStreamReader.name() == StringIdentifier::structuredTypeIdentifier)
                    error = parseStructuredType(xmlStreamReader,
                                                m_mapTypeDictionary.value(targetNamespace));
                else {
                    if (xmlStreamReader.name() == StringIdentifier::importIdentifier)
                        error = parseImport(xmlStreamReader,
                                            m_mapTypeDictionary.value(targetNamespace));
                    else {
                        if (isKnownElement(xmlStreamReader))
                            error = skipKnownElement(xmlStreamReader);
                        else
                            error = InvalidTypeDictionaryEntry;
                    }
                }
            }
        } else
            xmlStreamReader.readNext();
    }
    return error;
}

RecursiveDescentParser::ParsingError RecursiveDescentParser::skipKnownElement(
    QXmlStreamReader &xmlStreamReader)
{
    ParsingError error = NoError;
    xmlStreamReader.readNext();
    while (!xmlStreamReader.atEnd() && error == NoError) {
        if (xmlStreamReader.isEndElement()) {
            xmlStreamReader.readNext();
            return error;
        }
        if (xmlStreamReader.isStartElement()) {
            if (xmlStreamReader.name() == StringIdentifier::documentationIdentifier
                || xmlStreamReader.name() == StringIdentifier::opaqueTypeIdentifier)
                skipKnownElement(xmlStreamReader);
            else
                error = UnknownEntry;
        } else
            xmlStreamReader.readNext();
    }
    return error;
}

RecursiveDescentParser::ParsingError RecursiveDescentParser::generateInputFiles(
    const QString &path,
    const QString &prefix,
    const QString &header,
    bool generateBundleFiles)
{
    DependencyDataTypeValidator dependencyDataTypeValidator;
    for (auto it = m_mapTypeDictionary.constBegin(); it != m_mapTypeDictionary.constEnd(); ++it) {
        if (!it.value()->dependencyTypeDictionary())
            it.value()->accept(&dependencyDataTypeValidator);
    }

    dependencyDataTypeValidator.setReadResolveDependencies(
        DependencyDataTypeValidator::ResolveDependencies);

    for (auto it = m_mapTypeDictionary.constBegin(); it != m_mapTypeDictionary.constEnd(); ++it)
        it.value()->accept(&dependencyDataTypeValidator);

    for (auto it = m_mapTypeDictionary.constBegin(); it != m_mapTypeDictionary.constEnd(); ++it)
        it.value()->accept(&dependencyDataTypeValidator);

    if (dependencyDataTypeValidator.unresolvedDependencyStringList().isEmpty()) {
        DataTypeFileWriter filewriter(path,
                                      prefix,
                                      header);
        for (auto it = m_mapTypeDictionary.constBegin(); it != m_mapTypeDictionary.constEnd(); ++it) {
            if (!it.value()->dependencyTypeDictionary())
                it.value()->accept(&filewriter);
        };
        if (filewriter.generateMapping().isEmpty()) {
            return UnableToWriteFile;
        }
        MappingFileGenerator mappingFileGenerator(filewriter.generateMapping(), path, prefix, header);
        if (filewriter.generateTypes(dependencyDataTypeValidator.resolvedDependencyElementList())
            != DataTypeFileWriter::GeneratingError::NoError) {
            return UnableToWriteFile;
        }

        mappingFileGenerator.addGenerateMapping(dependencyDataTypeValidator.resolvedDependencyElementList());

        if (mappingFileGenerator.generateMapping() != MappingFileGenerator::MappingError::NoError)
            return UnableToWriteFile;

        if (generateBundleFiles)
            filewriter.writeBundleFiles();
    } else {
        return MissingDependency;
    }

    return NoError;
}

bool RecursiveDescentParser::isKnownElement(const QXmlStreamReader &xmlStreamReader)
{
    return xmlStreamReader.name() == StringIdentifier::documentationIdentifier
           || xmlStreamReader.name() == StringIdentifier::opaqueTypeIdentifier;
}

QString RecursiveDescentParser::permittedName(const QString &name) const
{
    if (name.isEmpty())
        return {};

    auto result = name;

    if (StringIdentifier::illegalNames.contains(name)) {
        result = Util::lowerFirstLetter(name);
        result = u"_%1"_s.arg(name);
    }

    if (result.contains(' '_L1))
        result.replace(' '_L1, '_'_L1);
    if (name.contains('-'_L1))
        result.replace('-'_L1, '_'_L1);

    return result;
}

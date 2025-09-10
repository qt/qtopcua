// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma once

#include <QtCore/qmap.h>
#include <QtCore/qxmlstream.h>

class EnumeratedType;
class StructuredType;
class TypeDictionary;

class RecursiveDescentParser
{
    Q_GADGET
public:
    enum ParsingError {
        NoError,
        InvalidFileName,
        InvalidTypeDictionaryEntry,
        InvalidImportEntry,
        InvalidEnumeratedTypeEntry,
        InvalidStructuredTypeEntry,
        InvalidEnumeratedValueEntry,
        InvalidFieldEntry,
        UnknownEntry,
        MissingDependency,
        CannotFullyGenerateNamespaceZero,
        UnableToWriteFile,
        UnableToResolveDependency,
    };
    Q_ENUM(ParsingError)

    RecursiveDescentParser() = default;
    ~RecursiveDescentParser();

    void printInOrder() const;
    ParsingError parseEnumeratedType(QXmlStreamReader &xmlStreamReader,
                                     TypeDictionary *typeDictionary);

    ParsingError parseEnumeratedValue(QXmlStreamReader &xmlStreamReader,
                                      EnumeratedType *enumeratedType);
    ParsingError parseField(QXmlStreamReader &xmlStreamReader, StructuredType *structuredType);

    ParsingError parseFile(const QString &fileName, const bool &dependencyTypeDictionary);
    ParsingError parseImport(QXmlStreamReader &xmlStreamReader, TypeDictionary *typeDictionary);

    ParsingError parseStructuredType(QXmlStreamReader &xmlStreamReader,
                                     TypeDictionary *typeDictionary);
    ParsingError parseTypeDictionary(QXmlStreamReader &xmlStreamReader,
                                     const bool &dependencyTypeDictionary);

    ParsingError skipKnownElement(QXmlStreamReader &xmlStreamReader);
    ParsingError generateInputFiles(const QString &path,
                                    const QString &prefix,
                                    const QString &header);

    ParsingError dependencyCheck();
    bool isKnownElement(const QXmlStreamReader &xmlStreamReader);

    QString permittedName(const QString &name) const;

private:
    QString m_fileName;
    QMap<QString, TypeDictionary *> m_mapTypeDictionary;
};

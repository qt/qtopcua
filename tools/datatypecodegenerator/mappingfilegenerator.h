// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma once

#include <QtCore/qlist.h>
#include <QtCore/qtextstream.h>

#include <cmath>
#include <iostream>

class EnumeratedType;
class Field;
class StructuredType;
class XmlElement;

class MappingFileGenerator
{
public:
    enum MappingError { NoError, UnanbleToWrite, UnableToResolve };

    MappingFileGenerator(const QList<XmlElement *> &generateMapping,
                         const QString &path,
                         const QString &prefix,
                         const QString &header);
    ~MappingFileGenerator() = default;

    void addGenerateMapping(const QList<XmlElement *> &generateMapping);

    MappingError generateMapping();

private:
    QList<Field *> m_lengthFields;
    QList<Field *> m_switchFields;
    QList<XmlElement *> m_generateMapping;
    QString m_path;
    QString m_prefix;
    QString m_header;

    MappingError generateMappingHeader();
    MappingError generateMappingCpp();

    void generateFieldDecoding(QTextStream &output, const StructuredType *structuredType, const Field *field, int level = 1);
    void generateScalarOrArrayDecoding(QTextStream &output, const StructuredType *structuredType, const Field *field);
    void generateOptionalFieldDecoding(QTextStream &output, const StructuredType *structuredType, const Field *field, int level = 1);
    void generateUnionFieldDecoding(QTextStream &output, const StructuredType *structuredType, const Field *field, int level = 1);

    void generateFieldEncoding(QTextStream &output, const StructuredType *structuredType, const Field *field, int level = 1);
    void generateScalarOrArrayEncoding(QTextStream &output, const StructuredType *structuredType, const Field *field);
    void generateOptionalFieldEncoding(QTextStream &output, const StructuredType *structuredType, const Field *field, int level = 1);
    void generateUnionFieldEncoding(QTextStream &output, const StructuredType *structuredType, const Field *field, int level = 1);

    void generateDeEncodingArray(QTextStream &output);

    void generateDeEncoding(QTextStream &output);

    void generateDecodingEnumeratedType(QTextStream &output, const EnumeratedType *enumeratedType);
    void generateEncodingEnumeratedType(QTextStream &output, const EnumeratedType *enumeratedType);

    void generateDecodingStructuredType(QTextStream &output, const StructuredType *structuredType);
    void generateEncodingStructuredType(QTextStream &output, const StructuredType *structuredType);

    MappingFileGenerator::MappingError sortMappings();
};

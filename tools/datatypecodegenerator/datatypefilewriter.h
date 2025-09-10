// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma once

#include "visitor.h"

#include <QtCore/qlist.h>
#include <QtCore/qtextstream.h>

class DataTypeFileWriter : public Visitor
{
public:
    enum GeneratingError { NoError, UnableToWrite };
    DataTypeFileWriter(const QString &path,
                       const QString &prefix,
                       const QString &header);
    ~DataTypeFileWriter() override = default;

    void visit(EnumeratedType *enumeratedType) override;
    void visit(EnumeratedValue *enumeratedValue) override;
    void visit(Field *field) override;
    void visit(Import *import) override;
    void visit(StructuredType *structuredType) override;
    void visit(TypeDictionary *typeDictionary) override;
    void visit(XmlElement *xmlElement) override;

    GeneratingError generateFile(const XmlElement *type, const QString &fileExtension);
    GeneratingError generateTypes(const QList<XmlElement *> &types);

    QList<XmlElement *> generateMapping() const;
    void setGenerateMapping(const QList<XmlElement *> &generateMapping);

private:
    QString m_path;
    QString m_prefix;
    QString m_header;
    QList<QString> m_generatedEnumeratedTypeFilenames;
    QList<QString> m_generatedStructuredTypeFilenames;
    QList<XmlElement *> m_generateMapping;
    QList<EnumeratedType *> m_enumeratedTypes;

    void writeLicenseHeader(QTextStream &output);

    void writeStructuredTypeCpp(const StructuredType *structuredType, QTextStream &output);
    void writeStructuredTypeCppClassComment(const StructuredType *structuredType,
                                            QTextStream &output);
    void writeStructuredTypeCppDataClass(const StructuredType *structuredType, QTextStream &output);
    void writeStructuredTypeCppDataClassMember(const StructuredType *structuredType,
                                               QTextStream &output);
    void writeStructuredTypeCppConstructors(const StructuredType *structuredType,
                                            QTextStream &output);
    void writeStructuredTypeCppOperatorAssignment(const StructuredType *structuredType,
                                                  QTextStream &output);
    void writeStructuredTypeCppOperatorEquality(const StructuredType *structuredType,
                                                QTextStream &output);
    void writeStructuredTypeCppQVariant(const StructuredType *structuredType, QTextStream &output);
    void writeStructuredTypeCppDestructor(const StructuredType *structuredType, QTextStream &output);
    void writeStructuredTypeCppGetterSetter(const StructuredType *structuredType,
                                            QTextStream &output);
    void writeStructuredTypeCppGetter(const StructuredType *structuredType, QTextStream &output);
    void writeStructuredTypeCppSetter(const Field *field,
                                      const StructuredType *structuredType,
                                      QTextStream &output);

    void writeStructuredTypeHeader(const StructuredType *structuredType, QTextStream &output);
    void writeStructuredTypeHeaderIncludes(const StructuredType *structuredType,
                                           QTextStream &output);
    void writeStructuredTypeHeaderUnion(const StructuredType *structuredType, QTextStream &output);
    void writeStructuredTypeHeaderGetterSetter(const StructuredType *structuredType,
                                               QTextStream &output);

    void writeStructuredTypeHeaderDebug(const StructuredType *structuredType,
                                        QTextStream &output);
    void writeStructuredTypeCppDebug(const StructuredType *structuredType,
                                        QTextStream &output);

    void writeEnumeratedType(const EnumeratedType *enumeratedType, QTextStream &output);
    GeneratingError writeEnumeratedTypes();
};

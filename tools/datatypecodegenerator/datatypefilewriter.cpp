// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "util.h"
#include "datatypefilewriter.h"
#include "enumeratedtype.h"
#include "enumeratedvalue.h"
#include "field.h"
#include "stringidentifier.h"
#include "structuredtype.h"

#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/qset.h>

using namespace Qt::Literals::StringLiterals;

DataTypeFileWriter::DataTypeFileWriter(const QString &path,
                                       const QString &prefix,
                                       const QString &header)
    : m_path(path)
    , m_prefix(prefix)
    , m_header(header)
{}

void DataTypeFileWriter::visit(XmlElement *xmlElement)
{
    Q_UNUSED(xmlElement);
}

void DataTypeFileWriter::visit(EnumeratedType *enumeratedType)
{
    bool isPrecodedType = false;
    for (const auto &precodedType : StringIdentifier::opcUaPrecodedTypes) {
        if (precodedType.contains(enumeratedType->name())) {
            isPrecodedType = true;
            break;
        }
    }
    if (!isPrecodedType) {
        m_generateMapping.append(enumeratedType);
    }
}

void DataTypeFileWriter::visit(EnumeratedValue *enumeratedValue)
{
    Q_UNUSED(enumeratedValue);
}

void DataTypeFileWriter::visit(Field *field)
{
    Q_UNUSED(field);
}

void DataTypeFileWriter::visit(Import *import)
{
    Q_UNUSED(import);
}

void DataTypeFileWriter::visit(StructuredType *structuredType)
{
    if (!structuredType->fields().empty()) {
        bool isPrecodedType = false;
        for (const auto &precodedType : StringIdentifier::opcUaPrecodedTypes) {
            if (precodedType.contains(structuredType->name())) {
                isPrecodedType = true;
                break;
            }
        }
        if (!isPrecodedType) {
            m_generateMapping.append(structuredType);
        }
    }
}

void DataTypeFileWriter::visit(TypeDictionary *typeDictionary)
{
    Q_UNUSED(typeDictionary);
}

void DataTypeFileWriter::writeLicenseHeader(QTextStream &output)
{
    if (!m_header.isEmpty())
        output << m_header << Util::lineBreak(2);
}

void DataTypeFileWriter::writeStructuredTypeCpp(const StructuredType *structuredType,
                                                QTextStream &output)
{
    if (!m_generatedStructuredTypeFilenames.contains(
            u"%1%2"_s.arg(m_prefix, structuredType->name())))
        m_generatedStructuredTypeFilenames.push_back(
            u"%1%2"_s.arg(m_prefix, structuredType->name()));
    output << "#include \"" << m_prefix.toLower() << structuredType->name().toLower() << ".h\"";

    QList<QString> mutualIncludes;
    const auto typeFields = structuredType->fields();
    for (const auto field : typeFields) {
        const auto typeName = field->typeNameSecondPart();
        for (const auto &type : std::as_const(m_generateMapping)) {
            StructuredType *tempStructuredType = dynamic_cast<StructuredType *>(type);
            if (tempStructuredType) {
                if (tempStructuredType == structuredType)
                    continue;
                const auto tempFields = tempStructuredType->fields();
                for (const auto &tempField : tempFields) {
                    if (typeName == tempStructuredType->name()
                        && tempField->typeNameSecondPart() == structuredType->name()) {
                        if (!mutualIncludes.contains(
                                u"#include \"%1%2.h\"\n"_s
                                    .arg(m_prefix.toLower(), tempStructuredType->name().toLower())))
                            mutualIncludes.push_back(
                                u"#include \"%1%2.h\"\n"_s
                                    .arg(m_prefix.toLower(), tempStructuredType->name().toLower()));
                    }
                }
            }
        }
    }
    if (!mutualIncludes.isEmpty()) {
        output << Util::lineBreak();
        std::sort(mutualIncludes.begin(), mutualIncludes.end());
        for (const auto &include : mutualIncludes) {
            output << include;
        }
    }
    if (structuredType->recursive()) {
        output << Util::lineBreak();
        output << "#include <optional>";
        output << Util::lineBreak();
    }
    output << Util::lineBreak(2);

    writeStructuredTypeCppClassComment(structuredType, output);
    writeStructuredTypeCppDataClass(structuredType, output);
    writeStructuredTypeCppConstructors(structuredType, output);
    writeStructuredTypeCppOperatorAssignment(structuredType, output);
    writeStructuredTypeCppOperatorEquality(structuredType, output);
    writeStructuredTypeCppQVariant(structuredType, output);
    writeStructuredTypeCppDestructor(structuredType, output);
    writeStructuredTypeCppGetterSetter(structuredType, output);
    writeStructuredTypeCppDebug(structuredType, output);
}

void DataTypeFileWriter::writeStructuredTypeCppClassComment(const StructuredType *structuredType,
                                                            QTextStream &output)
{
    output << "/*!"
           << Util::lineBreak();
    output << Util::indent(1) << "\\class " << m_prefix << structuredType->name() << Util::lineBreak();
    output << Util::indent(1) << "\\brief the OPC UA " << structuredType->name() << "."
           << Util::lineBreak();
    output << "*/"
           << Util::lineBreak();
}

void DataTypeFileWriter::writeStructuredTypeCppDataClass(const StructuredType *structuredType,
                                                         QTextStream &output)
{
    output << "class " << m_prefix << structuredType->name() << "Data : public QSharedData"
           << Util::lineBreak();
    output << "{"
           << Util::lineBreak();
    output << "public:"
           << Util::lineBreak();

    writeStructuredTypeCppDataClassMember(structuredType, output);

    output << "};"
           << Util::lineBreak(2);
}

void DataTypeFileWriter::writeStructuredTypeCppDataClassMember(const StructuredType *structuredType,
                                                               QTextStream &output)
{
    const auto typeFields = structuredType->fields();
    QList<Field *> unionMember;
    if (structuredType->hasUnion()) {
        for (const auto &possibleUnionMember : typeFields) {
            for (const auto &field : typeFields) {
                if (field->isUnion()) {
                    if (field->switchField() == possibleUnionMember->name()
                        && !unionMember.contains(possibleUnionMember))
                        unionMember.push_back(possibleUnionMember);
                }
            }
        }
    }
    QList<Field *> arrayLengthfields;
    for (const auto &possibleArrayLengthField : typeFields) {
        for (const auto &field : typeFields) {
            if (possibleArrayLengthField->name() == field->lengthField()
                && !arrayLengthfields.contains(possibleArrayLengthField))
                arrayLengthfields.push_back(possibleArrayLengthField);
        }
    }

    QList<Field *> switchFields;
    if (structuredType->hasSwitchfield()) {
        for (const auto &possibleOptionalMember : typeFields) {
            for (const auto &field : typeFields) {
                if (!possibleOptionalMember->switchField().isEmpty() && possibleOptionalMember->switchField() == field->name())
                    switchFields.push_back(field);
            }
        }
    }

    for (const auto &field : typeFields) {
        if (field->isInStructuredTypeBitMask() && !switchFields.contains(field))
            continue;

        bool isEnumeration = false;
        for (const auto &enumeratedType : std::as_const(m_enumeratedTypes)) {
            if (enumeratedType->name() == field->typeName().split(':'_L1).at(1)) {
                isEnumeration = true;
                field->setIsEnum(true);
            }
        }

        bool isPreCoded = false;
        if (!arrayLengthfields.contains(field)) {
            if (!unionMember.contains(field)) {
                output << Util::indent(1);
                if (StringIdentifier::typeNameDataTypeConverter.contains(field->typeName())) {
                    if (!field->needContainer()) {
                        output << StringIdentifier::typeNameDataTypeConverter.value(
                            field->typeName())
                               << " ";
                    } else {
                        output << "QList<"
                               << StringIdentifier::typeNameDataTypeConverter.value(
                                      field->typeName())
                               << "> ";
                    }
                } else {
                    const auto typeName = field->typeNameSecondPart();
                    for (const auto &preCodedType : StringIdentifier::opcUaPrecodedTypes) {
                        if (preCodedType.contains(typeName)) {
                            isPreCoded = true;
                            if (field->needContainer())
                                output << "QList<";
                            if (preCodedType.deEncoderName().isEmpty())
                                output << preCodedType.className();
                            else
                                output << preCodedType.deEncoderName();
                            if (field->needContainer())
                                output << ">";
                            output << " ";
                            break;
                        }
                    }
                    if (!isPreCoded) {
                        if (field->needContainer())
                            output << "QList<" << m_prefix << typeName << "> ";
                        else if (field->recursive())
                            output << "std::optional<" << m_prefix << typeName << "> ";
                        else if (isEnumeration)
                            output << m_prefix << "::" << typeName << " ";
                        else
                            output << m_prefix << typeName << " ";
                    }
                }

                const auto lowerCaseFieldName = field->lowerFirstName();
                output << lowerCaseFieldName;
                if (!field->needContainer()) {
                    if (StringIdentifier::typeNameDataTypeConverter.contains(field->typeName())) {
                        if (field->typeName().contains(StringIdentifier::integerIdentifier)
                            || field->typeName().contains(StringIdentifier::doubleIdentifier)
                            || field->typeName().contains(StringIdentifier::floatIdentifier)
                            || field->typeName().contains(StringIdentifier::byteIdentifier)
                            || field->typeName().contains(StringIdentifier::sbyteIdentifier))
                            output << " {0}";
                        else if (field->typeName().contains(StringIdentifier::booleanIdentifier)
                                 || field->typeName().contains(StringIdentifier::bitIdentifier))
                            output << " {false}";
                    } else if (field->isEnum()) {
                        const auto enumType = std::find_if(m_enumeratedTypes.constBegin(), m_enumeratedTypes.constEnd(),
                                                           [field](EnumeratedType *e) { return e->name() == field->typeNameSecondPart(); });
                        const auto firstValueName = enumType != m_enumeratedTypes.constEnd() && !(*enumType)->values().empty()
                                                        ? (*enumType)->values().first()->name() : u"Unknown"_s;
                        if (!firstValueName.isEmpty())
                            output << " {" << m_prefix << "::" << field->typeNameSecondPart() << "::" << firstValueName << "}";
                        else
                            output << " {}";
                    } else {
                        if (field->typeName().contains(StringIdentifier::uaStatusCodeIdentifier))
                            output << " {QOpcUa::UaStatusCode::Good}";
                    }
                }
                output << ";"
                       << Util::lineBreak();
            } else {
                const auto fieldNameLowerCase = field->lowerFirstName();
                output << Util::indent(1) << m_prefix << structuredType->name() << "::" << field->name()
                       << " " << fieldNameLowerCase << " = " << m_prefix << structuredType->name()
                       << "::" << field->name() << "::"
                       << "None";

                output << ";"
                       << Util::lineBreak();
            }
        }
    }
}

void DataTypeFileWriter::writeStructuredTypeCppConstructors(const StructuredType *structuredType,
                                                            QTextStream &output)
{
    output << m_prefix << structuredType->name() << "::" << m_prefix << structuredType->name()
           << "()"
           << Util::lineBreak();
    output << "   : data(new " << m_prefix << structuredType->name() << "Data)\n";
    output << "{"
           << Util::lineBreak();
    output << "}"
           << Util::lineBreak(2);

    output << "/*!"
           << Util::lineBreak();
    output << "   Constructs a " << structuredType->name() << " from \\a rhs"
           << Util::lineBreak();
    output << "*/"
           << Util::lineBreak();
    output << m_prefix << structuredType->name() << "::" << m_prefix << structuredType->name()
           << "(const " << m_prefix << structuredType->name() << " &rhs)"
           << Util::lineBreak();
    output << "   : data(rhs.data)"
           << Util::lineBreak();
    output << "{"
           << Util::lineBreak();
    output << "}"
           << Util::lineBreak(2);
}

void DataTypeFileWriter::writeStructuredTypeCppOperatorAssignment(
    const StructuredType *structuredType, QTextStream &output)
{
    output << "/*!"
           << Util::lineBreak();
    output << "   Sets the values from \\a rhs in this " << structuredType->name() << Util::lineBreak();
    output << "*/"
           << Util::lineBreak();
    output << m_prefix << structuredType->name() << " &" << m_prefix << structuredType->name()
           << "::operator=(const " << m_prefix << structuredType->name() << " &rhs)"
           << Util::lineBreak();
    output << "{"
           << Util::lineBreak();
    output << Util::indent(1) << "if (this != &rhs)"
           << Util::lineBreak();
    output << Util::indent(2) << "data.operator=(rhs.data);"
           << Util::lineBreak();
    output << Util::indent(1) << "return *this;"
           << Util::lineBreak();
    output << "}"
           << Util::lineBreak(2);
}

void DataTypeFileWriter::writeStructuredTypeCppOperatorEquality(const StructuredType *structuredType,
                                                                QTextStream &output)
{
    output << "/*!"
           << Util::lineBreak();
    output << Util::indent(1) << "Returns \\c true if this " << structuredType->name()
           << " has the same value as \\a rhs"
           << Util::lineBreak();
    output << "*/"
           << Util::lineBreak();

    output << "bool " << m_prefix << structuredType->name() << "::operator==(const " << m_prefix
           << structuredType->name() << " &rhs) const"
           << Util::lineBreak();
    output << "{"
           << Util::lineBreak();
    QList<Field *> unionSwitchfield;
    QList<Field *> arrayLengthfields;

    const auto typeFields = structuredType->fields();

    for (const auto &possibleMember : typeFields) {
        for (const auto &field : typeFields) {
            if (possibleMember->name() == field->lengthField()
                && !arrayLengthfields.contains(possibleMember))
                arrayLengthfields.push_back(possibleMember);
            if (possibleMember->name() == field->switchField()
                && !unionSwitchfield.contains(possibleMember) && field->isUnion())
                unionSwitchfield.push_back(possibleMember);
        }
    }

    if (!unionSwitchfield.isEmpty()) {
        const auto switchField = unionSwitchfield.first()->lowerFirstName();
        output << Util::indent(1) << "if (data->" << switchField << " != " << " rhs.data->" << switchField << ")" << Util::lineBreak();
        output << Util::indent(2) << "return false;" << Util::lineBreak(2);
    }

    if (structuredType->containsBitMask()) {
        for (const auto &field : typeFields) {
            if (!field->switchField().isEmpty() && !arrayLengthfields.contains(field)) {
                const auto switchField = Util::lowerFirstLetter(field->switchField());
                output << Util::indent(1) << "if (data->" << switchField << " != rhs.data->" << switchField << " || (data->" << switchField << " && ";
                output << "!(data->" << field->lowerFirstName() << " == rhs.data->" << field->lowerFirstName() << ")";
                output << "))";
                output << Util::lineBreak() << Util::indent(2) << "return false;" << Util::lineBreak();
            }
        }
        output << Util::lineBreak();
    }

    auto counterGeneratedTypes = 0;
    if (structuredType->hasUnion()) {
        for (const auto &field : typeFields) {
            if (!arrayLengthfields.contains(field) && !unionSwitchfield.contains(field) && !field->isInStructuredTypeBitMask()) {
                if (structuredType->containsBitMask() && !field->switchField().isEmpty())
                    continue;


                output << Util::indent(1) << "if (static_cast<qint32>(data->" << unionSwitchfield.first()->lowerFirstName() << ") == " << field->switchValue() << " && ";
                output << "!(data->" << field->lowerFirstName() << " == rhs.data->" << field->lowerFirstName() << ")";
                output << ")" << Util::lineBreak();
                output << Util::indent(2) << "return false;" << Util::lineBreak(2);
            }
        }

        output << Util::indent(1) << "return true;" << Util::lineBreak();
    } else {
        output << Util::indent(1) << "return ";

        for (const auto &field : typeFields) {
            if (!arrayLengthfields.contains(field) && !unionSwitchfield.contains(field) && !field->isInStructuredTypeBitMask()) {
                if (structuredType->containsBitMask() && !field->switchField().isEmpty())
                    continue;

                const auto memberName = field->lowerFirstName();
                if (counterGeneratedTypes != 0)
                    output << Util::lineBreak() << Util::indent(2) << "   && ";
                output << "data->" << memberName << " == rhs.data->" << memberName;
                counterGeneratedTypes++;
            }
        }

        output << ";" << Util::lineBreak();
    }

    output << "}"
           << Util::lineBreak(2);
}

void DataTypeFileWriter::writeStructuredTypeCppQVariant(const StructuredType *structuredType,
                                                        QTextStream &output)
{
    output << "/*!"
           << Util::lineBreak();
    output << "   Converts this " << structuredType->name() << " to \\l QVariant"
           << Util::lineBreak();
    output << "*/"
           << Util::lineBreak();

    output << m_prefix << structuredType->name() << "::operator QVariant() const"
           << Util::lineBreak();
    output << "{"
           << Util::lineBreak();
    output << Util::indent(1) << "return QVariant::fromValue(*this);"
           << Util::lineBreak();
    output << "}"
           << Util::lineBreak(2);
}

void DataTypeFileWriter::writeStructuredTypeCppDestructor(const StructuredType *structuredType,
                                                          QTextStream &output)
{
    output << m_prefix << structuredType->name() << "::~" << m_prefix << structuredType->name()
           << "()"
           << Util::lineBreak();
    output << "{"
           << Util::lineBreak();
    output << "}"
           << Util::lineBreak(2);
}

void DataTypeFileWriter::writeStructuredTypeCppGetterSetter(const StructuredType *structuredType,
                                                            QTextStream &output)
{
    writeStructuredTypeCppGetter(structuredType, output);
}

void DataTypeFileWriter::writeStructuredTypeCppGetter(const StructuredType *structuredType,
                                                      QTextStream &output)
{
    QList<Field *> unionMember;

    const auto typeFields = structuredType->fields();

    if (structuredType->hasUnion()) {
        for (const auto &possibleUnionMember : typeFields) {
            for (const auto &field : typeFields) {
                if (field->isUnion()) {
                    if (field->switchField() == possibleUnionMember->name()
                        && !unionMember.contains(possibleUnionMember))
                        unionMember.push_back(possibleUnionMember);
                }
            }
        }
    }
    QList<Field *> arrayLengthfields;
    for (const auto &possibleArrayLengthField : typeFields) {
        for (const auto &field : typeFields) {
            if (possibleArrayLengthField->name() == field->lengthField()
                && !arrayLengthfields.contains(possibleArrayLengthField))
                arrayLengthfields.push_back(possibleArrayLengthField);
        }
    }

    QList<Field *> switchFields;
    if (structuredType->hasSwitchfield()) {
        for (const auto &possibleOptionalMember : typeFields) {
            for (const auto &field : typeFields) {
                if (!possibleOptionalMember->switchField().isEmpty() && possibleOptionalMember->switchField() == field->name())
                    switchFields.push_back(field);
            }
        }
    }

    for (const auto &field : typeFields) {
        if (!arrayLengthfields.contains(field) && !(field->isInStructuredTypeBitMask() && !switchFields.contains(field))) {
            const auto tmpFunctionName = field->lowerFirstName();

            output << "/*!\n";
            output << Util::indent(1) << "Returns the field " << field->name() << " of this " << structuredType->name() << " object"
                   << Util::lineBreak();
            output << "*/\n";

            const auto typeName = field->typeNameSecondPart();
            if (!unionMember.contains(field)) {
                if (field->needContainer())
                    output << "QList<";
                if (StringIdentifier::typeNameDataTypeConverter.contains(field->typeName())) {
                    output << StringIdentifier::typeNameDataTypeConverter.value(field->typeName());
                } else {
                    bool isPreCoded = false;
                    for (const auto &preCodedType : StringIdentifier::opcUaPrecodedTypes) {
                        if (preCodedType.contains(typeName)) {
                            isPreCoded = true;
                            if (preCodedType.deEncoderName().isEmpty())
                                output << preCodedType.className();
                            else
                                output << preCodedType.deEncoderName();
                            break;
                        }
                    }
                    if (!isPreCoded) {
                        bool isEnum = false;
                        for (const auto &enumeratedType : std::as_const(m_enumeratedTypes)) {
                            if (enumeratedType->name() == field->typeName().split(':'_L1).at(1))
                                isEnum = true;
                        }
                        if (isEnum)
                            output << m_prefix << "::" << field->typeName().split(':'_L1).at(1);
                        else
                            output << m_prefix << typeName;
                    }
                }
                if (field->needContainer())
                    output << ">";
                output << " ";

            } else {
                output << m_prefix << structuredType->name() << "::" << field->name() << " ";
            }

            output << m_prefix << structuredType->name() << "::" << tmpFunctionName << "() const"
                   << Util::lineBreak();
            output << "{"
                   << Util::lineBreak();
            if (!field->isUnion()) {
                if (field->recursive())
                    if (field->needContainer()) {
                        output << Util::indent(1) << "return data->" << tmpFunctionName << ";"
                               << Util::lineBreak();
                    } else {
                        output << Util::indent(1) << "return data->" << tmpFunctionName << ".value_or(" << m_prefix
                               << field->typeName().split(QChar::fromLatin1(':')).at(1) << "()"
                               << ");"
                               << Util::lineBreak();
                    }
                else
                    output << Util::indent(1) << "return data->" << tmpFunctionName << ";"
                           << Util::lineBreak();
            } else {
                const auto switchfieldToLower = Util::lowerFirstLetter(field->switchField());
                output << Util::indent(1) << "if (data->" << switchfieldToLower << " == " << m_prefix
                       << structuredType->name() << "::" << field->switchField()
                       << "::" << field->name() << ")"
                       << Util::lineBreak();
                output << Util::indent(2) << "return data->" << tmpFunctionName << ";"
                       << Util::lineBreak();
                output << Util::indent(1) << "else"
                       << Util::lineBreak();
                output << Util::indent(2) << "return {};" << Util::lineBreak();
            }
            output << "}"
                   << Util::lineBreak(2);
            if (!unionMember.contains(field))
                writeStructuredTypeCppSetter(field, structuredType, output);
        }
    }
}

void DataTypeFileWriter::writeStructuredTypeCppSetter(const Field *field,
                                                      const StructuredType *structuredType,
                                                      QTextStream &output)
{
    output << "/*!"
           << Util::lineBreak();
    output << Util::indent(1) << "Sets the field " << field->name() << " of this " << structuredType->name() << " object to \\a "
           << field->lowerFirstName() << Util::lineBreak();
    output << "*/"
           << Util::lineBreak();

    output << "void " << m_prefix << structuredType->name() << "::set" << field->name() << "(";
    const auto tmpFunctionName = field->lowerFirstName();
    if (StringIdentifier::typeNameDataTypeConverter.contains(field->typeName())) {
        if (field->typeName().contains(StringIdentifier::booleanIdentifier)
            || field->typeName().contains(StringIdentifier::integerIdentifier)
            || field->typeName().contains(StringIdentifier::floatIdentifier)
            || field->typeName().contains(StringIdentifier::doubleIdentifier)) {
            if (field->needContainer())
                output << "QList<"
                       << StringIdentifier::typeNameDataTypeConverter.value(field->typeName())
                       << "> ";
            else
                output << StringIdentifier::typeNameDataTypeConverter.value(field->typeName())
                       << " ";
        } else {
            output << "const ";
            if (field->needContainer())
                output << "QList<"
                       << StringIdentifier::typeNameDataTypeConverter.value(field->typeName())
                       << "> &";
            else
                output << StringIdentifier::typeNameDataTypeConverter.value(field->typeName())
                       << " &";
        }
    } else {
        const auto typeName = field->typeNameSecondPart();
        bool isPreCoded = false;
        for (const auto &preCodedType : StringIdentifier::opcUaPrecodedTypes) {
            if (preCodedType.contains(typeName)) {
                isPreCoded = true;
                output << "const ";
                if (field->needContainer())
                    output << "QList<";
                if (preCodedType.deEncoderName().isEmpty())
                    output << preCodedType.className();
                else
                    output << preCodedType.deEncoderName();
                if (field->needContainer())
                    output << ">";
                output << " &";
                break;
            }
        }

        if (!isPreCoded) {
            bool isEnum = false;
            for (const auto &enumeratedType : std::as_const(m_enumeratedTypes)) {
                if (enumeratedType->name() == field->typeName().split(':'_L1).at(1))
                    isEnum = true;
            }
            output << "const ";
            if (field->needContainer())
                output << "QList <" << m_prefix << typeName << "> &";
            else if (isEnum)
                output << m_prefix << "::" << field->typeName().split(':'_L1).at(1) << " &";
            else
                output << m_prefix << typeName << " &";
        }
    }
    output << tmpFunctionName << ")"
           << Util::lineBreak();
    output << "{"
           << Util::lineBreak();
    output << Util::indent(1) << "data->" << tmpFunctionName << " = " << tmpFunctionName << ";"
           << Util::lineBreak();
    if (field->isUnion()) {
        const auto switchfieldToLower = Util::lowerFirstLetter(field->switchField());
        output << Util::indent(1) << "data->" << switchfieldToLower << " = " << m_prefix << structuredType->name()
               << "::" << field->switchField() << "::" << field->name() << ";"
               << Util::lineBreak();
    }
    output << "}"
           << Util::lineBreak(2);
}

void DataTypeFileWriter::writeStructuredTypeHeader(const StructuredType *structuredType,
                                                   QTextStream &output)
{
    output << "#pragma once"
           << Util::lineBreak();

    writeStructuredTypeHeaderIncludes(structuredType, output);

    output << "class " << m_prefix << structuredType->name() << Util::lineBreak();
    output << "{"
           << Util::lineBreak();
    output << "public:"
           << Util::lineBreak();

    writeStructuredTypeHeaderUnion(structuredType, output);

    output << Util::indent(1) << m_prefix << structuredType->name() << "();"
           << Util::lineBreak();
    output << Util::indent(1) << m_prefix << structuredType->name() << "(const " << m_prefix
           << structuredType->name() << " &);"
           << Util::lineBreak();
    output << Util::indent(1) << m_prefix << structuredType->name() << " &operator=(const " << m_prefix
           << structuredType->name() << " &rhs);"
           << Util::lineBreak();
    output << Util::indent(1) << "bool operator==(const " << m_prefix << structuredType->name() << " &rhs) const;"
           << Util::lineBreak();
    output << Util::indent(1) << "inline bool operator!=(const " << m_prefix << structuredType->name() << " &rhs) const"
           << Util::lineBreak(1)
           << Util::indent(2) << "{ return !(*this == rhs); }"
           << Util::lineBreak();
    output << Util::indent(1) << "operator QVariant() const;"
           << Util::lineBreak();
    output << Util::indent(1) << "~" << m_prefix << structuredType->name() << "();"
           << Util::lineBreak(2);

    writeStructuredTypeHeaderGetterSetter(structuredType, output);

    writeStructuredTypeHeaderDebug(structuredType, output);

    output << "private:"
           << Util::lineBreak();
    output << Util::indent(1) << "QSharedDataPointer<" << m_prefix << structuredType->name() << "Data> data;"
           << Util::lineBreak();
    output << "};"
           << Util::lineBreak(2);
    output << "Q_DECLARE_METATYPE(" << m_prefix << structuredType->name() << ")"
           << Util::lineBreak();
}

void DataTypeFileWriter::writeStructuredTypeHeaderIncludes(const StructuredType *structuredType,
                                                           QTextStream &output)
{
    QList<QString> qtCoreIncludes{u"#include <QSharedData>\n"_s,
                                  u"#include <QVariant>\n"_s};
    QList<QString> qtOpcUaIncludes{};
    QList<QString> qtClassIncludes;
    QList<QString> qtPredifinedClasses;

    const auto typeFields = structuredType->fields();

    for (const auto &field : typeFields) {
        const auto typeName = field->typeNameSecondPart();
        QString include;
        if (field->typeName().startsWith(u"%1:"_s.arg(StringIdentifier::binaryTypeIdentifier))) {
            if (typeName == StringIdentifier::datetimeIdentifier) {
                include = u"#include <QDateTime>\n"_s;
            } else if (typeName == StringIdentifier::byteStringIdentifier) {
                include = u"#include <QByteArray>\n"_s;
            } else if (typeName == StringIdentifier::charArrayIdentifier) {
                include = u"#include <QString>\n"_s;
            } else if (typeName == StringIdentifier::guidIdentifier) {
                include = u"#include <QUuid>\n"_s;
            }

            if (!qtCoreIncludes.contains(include)) {
                qtCoreIncludes.push_back(include);
            }
        } else {
            bool isPrecoded = false;
            for (const auto &precodedType : StringIdentifier::opcUaPrecodedTypes) {
                if (precodedType.contains(typeName)) {
                    isPrecoded = true;
                    if (!precodedType.filename().isEmpty()) {
                        include = u"#include <QtOpcUa/%1>\n"_s
                                      .arg(precodedType.filename());
                        break;
                    }
                }
            }
            if (typeName != structuredType->name()) {
                if (!isPrecoded) {
                    bool isEnum = false;
                    for (const auto &enumeratedType : std::as_const(m_enumeratedTypes)) {
                        if (enumeratedType->name() == typeName)
                            isEnum = true;
                    }
                    if (isEnum) {
                        if (!qtClassIncludes.contains(
                                u"#include \"%1enumerations.h\"\n"_s
                                    .arg(m_prefix.toLower())))
                            qtClassIncludes.push_back(
                                u"#include \"%1enumerations.h\"\n"_s
                                    .arg(m_prefix.toLower()));

                    } else {
                        bool isMutualInclude = false;
                        for (const auto &type : std::as_const(m_generateMapping)) {
                            StructuredType *tempStructuredType = dynamic_cast<StructuredType *>(
                                type);
                            if (tempStructuredType) {
                                if (tempStructuredType == structuredType)
                                    continue;
                                const auto tempFields = tempStructuredType->fields();
                                for (const auto &tempField : tempFields) {
                                    const auto tempTypeName = tempField->typeNameSecondPart();
                                    if (typeName == tempStructuredType->name()
                                        && tempTypeName == structuredType->name()) {
                                        isMutualInclude = true;
                                        if (!qtPredifinedClasses.contains(
                                                u"%1%2"_s.arg(m_prefix, tempStructuredType->name())))
                                            qtPredifinedClasses.push_back(
                                                u"%1%2"_s.arg(m_prefix, tempStructuredType->name()));
                                    }
                                }
                            }
                        }

                        if (!qtClassIncludes.contains(
                                u"#include \"%1%2.h\"\n"_s
                                    .arg(m_prefix.toLower(), typeName.toLower()))
                            && !isMutualInclude
                            && typeName != StringIdentifier::xmlElementIdentifier)
                            qtClassIncludes.push_back(
                                u"#include \"%1%2.h\"\n"_s
                                    .arg(m_prefix.toLower(), typeName.toLower()));
                    }
                } else {
                    if (!qtOpcUaIncludes.contains(include)) {
                        qtOpcUaIncludes.push_back(include);
                    }
                }
            }
        }
        if (field->needContainer()) {
            include = u"#include <QList>\n"_s;
            if (!qtCoreIncludes.contains(include)) {
                qtCoreIncludes.push_back(include);
            }
        }
    }
    if (!qtClassIncludes.isEmpty()) {
        std::sort(qtClassIncludes.begin(), qtClassIncludes.end());
        for (const auto &include : qtClassIncludes) {
            output << include;
        }
        output << Util::lineBreak();
    }
    std::sort(qtOpcUaIncludes.begin(), qtOpcUaIncludes.end());
    for (const auto &include : qtOpcUaIncludes) {
        output << include;
    }
    output << Util::lineBreak();
    std::sort(qtCoreIncludes.begin(), qtCoreIncludes.end());
    for (const auto &include : qtCoreIncludes) {
        output << include;
    }
    output << Util::lineBreak();

    if (!qtPredifinedClasses.isEmpty()) {
        std::sort(qtPredifinedClasses.begin(), qtPredifinedClasses.end());
        for (const auto &predefinedClass : qtPredifinedClasses) {
            output << "class " << predefinedClass << ";"
                   << Util::lineBreak();
        }
    }

    output << "class " << m_prefix << structuredType->name() << "Data;"
           << Util::lineBreak();
}

void DataTypeFileWriter::writeStructuredTypeHeaderUnion(const StructuredType *structuredType,
                                                        QTextStream &output)
{
    if (structuredType->hasUnion()) {
        QList<Field *> unions;
        QList<Field *> switchFields;

        const auto typeFields = structuredType->fields();

        for (const auto &field : typeFields) {
            if (field->isUnion())
                switchFields.push_back(field);
        }
        for (const auto &switchField : switchFields) {
            for (const auto &structuredTypeField : typeFields) {
                if (switchField->switchField() == structuredTypeField->name()) {
                    if (!unions.contains(structuredTypeField))
                        unions.push_back(structuredTypeField);
                }
            }
        }
        QList<Field *> arrayLengthfields;
        for (const auto &possibleArrayLengthField : typeFields) {
            for (const auto &field : typeFields) {
                if (possibleArrayLengthField->name() == field->lengthField()
                    && !arrayLengthfields.contains(possibleArrayLengthField))
                    arrayLengthfields.push_back(possibleArrayLengthField);
            }
        }
        for (const auto &field : unions) {
            output << Util::indent(1) << "enum class " << field->name() << " {"
                   << Util::lineBreak();
            output << Util::indent(2) << "None = 0,\n";
            for (int i = 0; i < switchFields.size(); i++) {
                const int nextField = i + 1;
                if (!arrayLengthfields.contains(switchFields.at(i))) {
                    if (nextField >= switchFields.size()) {
                        if (switchFields.at(i)->switchField() == field->name()) {
                            output << Util::indent(2) << switchFields.at(i)->name() << " = "
                                   << switchFields.at(i)->switchValue() << Util::lineBreak();
                            output << Util::indent(1) << "};"
                                   << Util::lineBreak(2);
                        }
                    } else {
                        if (field->name() == switchFields.at(nextField)->switchField()) {
                            output << Util::indent(2) << switchFields.at(i)->name() << " = "
                                   << switchFields.at(i)->switchValue() << ","
                                   << Util::lineBreak();
                        } else {
                            output << Util::indent(2) << switchFields.at(i)->name() << " = "
                                   << switchFields.at(i)->switchValue() << Util::lineBreak();
                            output << Util::indent(1) << "};"
                                   << Util::lineBreak(2);
                        }
                    }
                }
            }
        }
    }
}

void DataTypeFileWriter::writeStructuredTypeHeaderGetterSetter(const StructuredType *structuredType,
                                                               QTextStream &output)
{
    QList<Field *> unionMember;
    QList<Field *> arrayLengthfields;
    QList<Field *> switchFields;

    const auto typeFields = structuredType->fields();

    for (const auto &possibleMember : typeFields) {
        for (const auto &field : typeFields) {
            if (possibleMember->name() == field->lengthField()
                && !arrayLengthfields.contains(possibleMember))
                arrayLengthfields.push_back(possibleMember);
            if (field->isUnion() && field->switchField() == possibleMember->name()
                && !unionMember.contains(possibleMember))
                unionMember.push_back(possibleMember);
            if (!possibleMember->switchField().isEmpty() && field->name() == possibleMember->switchField())
                switchFields.push_back(field);
        }
    }
    for (const auto &field : typeFields) {
        if (!arrayLengthfields.contains(field) && !(field->isInStructuredTypeBitMask() && !switchFields.contains(field))) {
            const auto typeName = field->typeNameSecondPart();
            const auto tmpFunctionName = field->lowerFirstName();
            if (!unionMember.contains(field)) {
                if (StringIdentifier::typeNameDataTypeConverter.contains(field->typeName())) {
                    if (field->typeName().contains(StringIdentifier::booleanIdentifier)
                        || field->typeName().contains(StringIdentifier::integerIdentifier)
                        || field->typeName().contains(StringIdentifier::floatIdentifier)
                        || field->typeName().contains(StringIdentifier::doubleIdentifier)) {
                        if (!field->needContainer()) {
                            output << Util::indent(1)
                                   << StringIdentifier::typeNameDataTypeConverter.value(
                                          field->typeName())
                                   << " " << tmpFunctionName << "() const;"
                                   << Util::lineBreak();
                            output << Util::indent(1) << "void set" << field->name() << "("
                                   << StringIdentifier::typeNameDataTypeConverter.value(
                                          field->typeName())
                                   << " " << tmpFunctionName << ");"
                                   << Util::lineBreak(2);
                        } else {
                            output << Util::indent(1) << "QList<"
                                   << StringIdentifier::typeNameDataTypeConverter.value(
                                          field->typeName())
                                   << "> " << tmpFunctionName << "() const;"
                                   << Util::lineBreak();
                            output << Util::indent(1) << "void set" << field->name() << "(QList<"
                                   << StringIdentifier::typeNameDataTypeConverter.value(
                                          field->typeName())
                                   << "> " << tmpFunctionName << ");"
                                   << Util::lineBreak(2);
                        }
                    } else {
                        if (!field->needContainer()) {
                            output << Util::indent(1)
                                   << StringIdentifier::typeNameDataTypeConverter.value(
                                          field->typeName())
                                   << " " << tmpFunctionName << "() const;"
                                   << Util::lineBreak();
                            output << Util::indent(1) << "void set" << field->name() << "(const "
                                   << StringIdentifier::typeNameDataTypeConverter.value(
                                          field->typeName())
                                   << " &" << tmpFunctionName << ");"
                                   << Util::lineBreak(2);
                        } else {
                            output << Util::indent(1) << "QList<"
                                   << StringIdentifier::typeNameDataTypeConverter.value(
                                          field->typeName())
                                   << "> " << tmpFunctionName << "() const;"
                                   << Util::lineBreak();
                            output << Util::indent(1) << "void set" << field->name() << "(const QList<"
                                   << StringIdentifier::typeNameDataTypeConverter.value(
                                          field->typeName())
                                   << "> &" << tmpFunctionName << ");"
                                   << Util::lineBreak(2);
                        }
                    }
                } else {
                    bool isPreCoded = false;
                    for (const auto &preCodedType : StringIdentifier::opcUaPrecodedTypes) {
                        if (preCodedType.contains(typeName)) {
                            isPreCoded = true;
                            if (preCodedType.deEncoderName().isEmpty()) {
                                output << Util::indent(1);
                                if (field->needContainer())
                                    output << "QList<";
                                output << preCodedType.className();
                                if (field->needContainer())
                                    output << ">";
                                output << " " << tmpFunctionName << "() const;"
                                       << Util::lineBreak();
                                output << Util::indent(1) << "void set" << field->name() << "(const ";
                                if (field->needContainer())
                                    output << "QList<";
                                output << preCodedType.className();
                                if (field->needContainer())
                                    output << ">";
                                output << " &" << tmpFunctionName << ");"
                                       << Util::lineBreak(2);
                            } else {
                                output << Util::indent(1);
                                if (field->needContainer())
                                    output << "QList<";
                                output << preCodedType.deEncoderName();
                                if (field->needContainer())
                                    output << ">";
                                output << " " << tmpFunctionName << "() const;"
                                       << Util::lineBreak();
                                output << Util::indent(1) << "void set" << field->name() << "(const ";
                                if (field->needContainer())
                                    output << "QList<";
                                output << preCodedType.deEncoderName();
                                if (field->needContainer())
                                    output << ">";
                                output << " &" << tmpFunctionName << ");"
                                       << Util::lineBreak(2);
                            }
                            break;
                        }
                    }

                    if (!isPreCoded) {
                        if (field->needContainer()) {
                            output << Util::indent(1) << "QList <" << m_prefix << typeName << "> "
                                   << tmpFunctionName << "() const;"
                                   << Util::lineBreak();
                            output << Util::indent(1) << "void set" << field->name() << "(const QList <"
                                   << m_prefix << typeName << "> &" << tmpFunctionName
                                   << ");"
                                   << Util::lineBreak(2);
                        } else {
                            bool isEnum = false;
                            for (const auto &enumeratedType : std::as_const(m_enumeratedTypes)) {
                                if (enumeratedType->name() == field->typeName().split(':'_L1).at(1))
                                    isEnum = true;
                            }
                            if (isEnum) {
                                output << Util::indent(1) << m_prefix << "::" << typeName << " "
                                       << tmpFunctionName << "() const;"
                                       << Util::lineBreak();
                                output << Util::indent(1) << "void set" << field->name() << "(const " << m_prefix
                                       << "::" << typeName << " &" << tmpFunctionName << ");"
                                       << Util::lineBreak(2);
                            } else {
                                output << Util::indent(1) << m_prefix << typeName << " "
                                       << tmpFunctionName << "() const;"
                                       << Util::lineBreak();
                                output << Util::indent(1) << "void set" << field->name() << "(const " << m_prefix
                                       << typeName << " &" << tmpFunctionName << ");"
                                       << Util::lineBreak(2);
                            }
                        }
                    }
                }
            } else { // is union switchfield, create just getter
                output << Util::indent(1) << field->name() << " " << tmpFunctionName << "() const;"
                       << Util::lineBreak();
                output << Util::lineBreak();
            }
        }
    }
}

void DataTypeFileWriter::writeStructuredTypeHeaderDebug(const StructuredType *structuredType, QTextStream &output)
{
    output << Util::indent(1) << "friend QDebug operator<<(QDebug debug, const " << m_prefix << structuredType->name() << " &v);";
    output << Util::lineBreak(2);
}

void DataTypeFileWriter::writeStructuredTypeCppDebug(const StructuredType *structuredType, QTextStream &output)
{
    const auto typeName = u"%1%2"_s.arg(m_prefix, structuredType->name());

    output << "/*!" << Util::lineBreak();
    output << Util::indent(1) << "Prints the field values of object \\a v to \\a debug" << Util::lineBreak();
    output << "*/" << Util::lineBreak();
    output << "QDebug operator<<(QDebug debug, const " << typeName << " &v)" << Util::lineBreak();
    output << "{" << Util::lineBreak();
    output << Util::indent(1) << "QDebugStateSaver saver(debug);" << Util::lineBreak();
    output << Util::indent(1) << "debug.nospace().noquote();" << Util::lineBreak(2);
    output << Util::indent(1) << "debug << \"" << typeName << " (\";" << Util::lineBreak(2);

    if (!structuredType->hasUnion() && structuredType->hasSwitchfield())
        output << Util::indent(1) << "bool firstFieldPrinted = false;" << Util::lineBreak(2);

    bool isFirst = true;

    QSet<Field *> lengthFields;

    const auto typeFields = structuredType->fields();
    for (const auto &field : typeFields) {
        for (const auto &innerField : typeFields) {
            if (!field->lengthField().isEmpty() && field->lengthField() == innerField->name())
                lengthFields.insert(innerField);
        }
    }

    bool parameterUsed = false;

    for (const auto &field : typeFields) {
        if (structuredType->hasUnion() && field == typeFields.constFirst())
            continue;

        if (field->isInStructuredTypeBitMask())
            continue;

        if (lengthFields.contains(field))
            continue;

        if (structuredType->hasUnion())
            output << Util::indent(1) << "if (static_cast<quint32>(v." << Util::lowerFirstLetter(field->switchField()) << "()) == " << field->switchValue() << ") {" << Util::lineBreak();
        else if (!field->switchField().isEmpty())
            output << Util::indent(1) << "if (v." << Util::lowerFirstLetter(field->switchField()) << "()) {" << Util::lineBreak();

        int indentOffset = 0;
        if (structuredType->hasUnion() || !field->switchField().isEmpty())
            indentOffset = 1;

        if (!structuredType->hasUnion() && !isFirst) {
            if (structuredType->hasSwitchfield())
                output << Util::indent(1 + indentOffset) << "if (firstFieldPrinted)" << Util::lineBreak();
            output << Util::indent(1 + (structuredType->hasSwitchfield() ? indentOffset + 1 : 0)) << "debug << \", \";" << Util::lineBreak();
        }

        output << Util::indent(1 + indentOffset) << "debug << \"" << field->name() << ": \";" << Util::lineBreak();

        // Not all types have an operator<< for QDebug...
        const auto isPrecoded = std::find_if(StringIdentifier::opcUaPrecodedTypes.constBegin(), StringIdentifier::opcUaPrecodedTypes.constEnd(),
                                             [&](const auto &entry) { return entry.name() == field->typeNameSecondPart(); });

        if (isPrecoded == StringIdentifier::opcUaPrecodedTypes.constEnd() || StringIdentifier::precodedTypesWithDebugOperator.contains(field->typeNameSecondPart())) {
            output << Util::indent(1 + indentOffset) <<  "debug << v." << field->lowerFirstName() << "();" << Util::lineBreak();
            parameterUsed = true;
        } else {
            if (field->needContainer()) {
                const auto tempListName = u"%1%2"_s.arg(field->lowerFirstName(), u"Strings"_s);
                output << Util::indent(1 + indentOffset) << "QStringList " << tempListName << ";" << Util::lineBreak();
                output << Util::indent(1 + indentOffset) << "using namespace Qt::Literals::StringLiterals;" << Util::lineBreak();
                output << Util::indent(1 + indentOffset) << "for (int i = 0; i < v." << field->lowerFirstName() << "().size(); ++i)" << Util::lineBreak();
                output << Util::indent(2 + indentOffset) << tempListName << ".push_back(u\"" << isPrecoded->className() << "(...)\"_s" << ");" << Util::lineBreak();
                output << Util::indent(1 + indentOffset) << "debug << \"QList(\" << " << tempListName << ".join(u\", \"_s) << \")\";";
                parameterUsed = true;
            } else {
                output << Util::indent(1 + indentOffset) << "debug << \"" << isPrecoded->className() << "(...)\";";
            }
            output << Util::lineBreak();
        }

        if (!structuredType->hasUnion() && structuredType->hasSwitchfield() && field != typeFields.constLast())
            output << Util::indent(1 + indentOffset) << "firstFieldPrinted = true;" << Util::lineBreak();

        if (structuredType->hasUnion() || !field->switchField().isEmpty())
            output << Util::indent(1) << "}" << Util::lineBreak();

        output << Util::lineBreak();

        isFirst = false;
    }

    if (!parameterUsed)
        output << Util::lineBreak() << Util::indent(1) << "Q_UNUSED(v);" << Util::lineBreak(2);

    output << Util::indent(1) << "debug << \")\";" << Util::lineBreak();
    output << Util::indent(1) << "return debug;" << Util::lineBreak();
    output << "}" << Util::lineBreak();
}

DataTypeFileWriter::GeneratingError DataTypeFileWriter::writeEnumeratedTypes()
{
    QFile file;
    const auto fileName = u"%1enumerations.h"_s.arg(m_prefix.toLower());
    QDir dir(m_path);
    if (!dir.exists(m_path))
        if (!dir.mkpath(m_path))
            return UnableToWrite;
    file.setFileName(dir.absoluteFilePath(fileName));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return UnableToWrite;
    QTextStream output(&file);
    writeLicenseHeader(output);

    if (!m_generatedEnumeratedTypeFilenames.contains(
            u"%1enumerations"_s.arg(m_prefix.toLower())))
        m_generatedEnumeratedTypeFilenames.push_back(
            u"%1enumerations"_s.arg(m_prefix.toLower()));
    output << "#pragma once"
           << Util::lineBreak();
    output << "#include <QMetaType>"
           << Util::lineBreak(2);
    output << "namespace " << m_prefix << " {"
           << Util::lineBreak();
    output << "Q_NAMESPACE"
           << Util::lineBreak(2);
    for (const auto &enumeratedType : std::as_const(m_enumeratedTypes)) {
        output << "enum class " << enumeratedType->name() << " {" << Util::lineBreak(1);
        const auto tempValues = enumeratedType->values();
        for (const auto &enumeratedValue : tempValues) {
            output << Util::indent(1) << enumeratedValue->name() << " = " << enumeratedValue->value();
            if (!tempValues.endsWith(enumeratedValue))
                output << ",";
            output << Util::lineBreak();
        }
        output << "};"
               << Util::lineBreak();
        output << "Q_ENUM_NS(" << enumeratedType->name() << ")"
               << Util::lineBreak(2);
    }

    output << Util::lineBreak();
    output << "}"
           << Util::lineBreak();

    m_generatedHeaderFileNames.push_back(fileName);

    return GeneratingError::NoError;
}

DataTypeFileWriter::GeneratingError DataTypeFileWriter::generateFile(const XmlElement *type,
                                                                     const QString &fileExtension)
{
    QFile file;
    const auto fileName = u"%1%2%3"_s.arg(m_prefix.toLower(),
                                          type->name().toLower(),
                                          fileExtension);
    QDir dir(m_path);
    if (!dir.exists(m_path))
        if (!dir.mkpath(m_path))
            return UnableToWrite;
    file.setFileName(dir.absoluteFilePath(fileName));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return UnableToWrite;
    QTextStream output(&file);
    writeLicenseHeader(output);

    auto structuredType = dynamic_cast<const StructuredType *>(type);
    if (structuredType) {
        if (fileExtension == StringIdentifier::headerIdentifier) {
            writeStructuredTypeHeader(structuredType, output);
            if (!m_generatedHeaderFileNames.contains(fileName))
                m_generatedHeaderFileNames.push_back(fileName);
        } else {
            if (fileExtension == StringIdentifier::cppIdentifier) {
                writeStructuredTypeCpp(structuredType, output);
                if (!m_generatedSourceFileNames.contains(fileName))
                    m_generatedSourceFileNames.push_back(fileName);
            }
        }
    }
    file.close();
    return NoError;
}

DataTypeFileWriter::GeneratingError DataTypeFileWriter::generateTypes(
    const QList<XmlElement *> &types)
{
    m_generateMapping.append(types);
    for (const auto &type : std::as_const(m_generateMapping)) {
        const auto enumeratedType = dynamic_cast<EnumeratedType *>(type);
        if (enumeratedType)
            if (!m_enumeratedTypes.contains(enumeratedType))
                m_enumeratedTypes.append(enumeratedType);
    }
    if (writeEnumeratedTypes() != GeneratingError::NoError) {
        return GeneratingError::UnableToWrite;
    }

    for (const auto &type : std::as_const(m_generateMapping)) {
        const auto structuredType = dynamic_cast<StructuredType *>(type);
        if (structuredType) {
            if (generateFile(type, StringIdentifier::headerIdentifier) != GeneratingError::NoError)
                return GeneratingError::UnableToWrite;
            if (generateFile(type, StringIdentifier::cppIdentifier) != GeneratingError::NoError)
                return GeneratingError::UnableToWrite;
        }
    }
    return NoError;
}

QList<XmlElement *> DataTypeFileWriter::generateMapping() const
{
    return m_generateMapping;
}

void DataTypeFileWriter::setGenerateMapping(const QList<XmlElement *> &generateMapping)
{
    m_generateMapping = generateMapping;
}

bool DataTypeFileWriter::writeBundleFiles()
{
    const auto collectionHeaderName = u"%1datatypes%2"_s
    .arg(m_prefix.toLower(), StringIdentifier::headerIdentifier);
    const auto collectionSourceName = u"%1datatypes%2"_s
                                          .arg(m_prefix.toLower(), StringIdentifier::cppIdentifier);

    if (!m_generatedHeaderFileNames.isEmpty()) {
        QFile file(QDir(m_path).absoluteFilePath(collectionHeaderName));
        if (!file.open(QFile::WriteOnly))
            return false;

        QTextStream out(&file);

        writeLicenseHeader(out);

        out << "#pragma once" << Util::lineBreak(2);

        for (const auto &header : std::as_const(m_generatedHeaderFileNames))
            out << "#include \"" << header << "\"" << Util::lineBreak();
    }

    const auto enumHeaderCandidate = u"%1enumerations.h"_s.arg(m_prefix.toLower());
    const bool hasEnums = m_generatedHeaderFileNames.contains(enumHeaderCandidate);

    if (!m_generatedSourceFileNames.isEmpty() || hasEnums) {
        QFile file(QDir(m_path).absoluteFilePath(collectionSourceName));
        if (!file.open(QFile::WriteOnly))
            return false;

        QTextStream out(&file);

        writeLicenseHeader(out);

        if (hasEnums)
            out << "#include \"" << u"moc_%1enumerations.cpp"_s.arg(m_prefix.toLower()) << "\"" << Util::lineBreak();

        for (const auto &source : std::as_const(m_generatedSourceFileNames))
            out << "#include \"" << source << "\"" << Util::lineBreak();
    }

    return true;
}

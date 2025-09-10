// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma once

#include <QtCore/qlist.h>
#include <QtCore/qmap.h>
#include <QtCore/qset.h>
#include <QtCore/qstring.h>

class StringIdentifier
{
public:
    static const QString baseTypeIdentifier;
    static const QString binaryTypeIdentifier;
    static const QString bitIdentifier;
    static const QString booleanIdentifier;
    static const QString byteIdentifier;
    static const QString sbyteIdentifier;
    static const QString byteStringIdentifier;
    static const QString charArrayIdentifier;
    static const QString cppIdentifier;
    static const QString datetimeIdentifier;
    static const QString defaultByteOrderIdentifier;
    static const QString documentationIdentifier;
    static const QString doubleIdentifier;
    static const QString enumeratedTypeIdentifier;
    static const QString enumeratedValueIdentifier;
    static const QString fieldIdentifier;
    static const QString floatIdentifier;
    static const QString guidIdentifier;
    static const QString headerIdentifier;
    static const QString importIdentifier;
    static const QString integerIdentifier;
    static const QString lengthIdentifier;
    static const QString lengthFieldIdentifier;
    static const QString lengthInBitsIdentifier;
    static const QString locationIdentifier;
    static const QString mainIdentifier;
    static const QString makelistIdentifier;
    static const QString nameIdentifier;
    static const QString nameSpaceIdentifier;
    static const QString namespaceZeroIdentifier;
    static const QString opaqueTypeIdentifier;
    static const QString projectIdentifier;
    static const QString qByteArrayIdentifier;
    static const QString qStringIdentifier;
    static const QString uaStatusCodeIdentifier;
    static const QString reservedIdentifier;
    static const QString specifiedIdentifier;
    static const QString structuredTypeIdentifier;
    static const QString switchFieldIdentifier;
    static const QString switchValueIdentifier;
    static const QString targetNamespaceIdentifier;
    static const QString typeDictionaryIdentifier;
    static const QString typeNameIdentifier;
    static const QString valueIdentifier;
    static const QString xmlElementIdentifier;
    static const QString opcBitIdentifier;

    class OpcUaPrecodedType
    {
    public:
        OpcUaPrecodedType(const QString &typeName,
                          const QString &fileName,
                          const QString &className,
                          const QString &deEncoderName = QString())
            : m_name(typeName)
            , m_filename(fileName)
            , m_className(className)
            , m_deEncoderName(deEncoderName)
        {}

        bool contains(QString name) const
        {
            return m_name == name || m_filename == name || m_className == name;
        }

        QString name() const;

        QString filename() const;

        QString className() const;

        QString deEncoderName() const;

    private:
        QString m_name;
        QString m_filename;
        QString m_className;
        QString m_deEncoderName;
    };

    static const QList<OpcUaPrecodedType> opcUaPrecodedTypes;
    static const QList<QString> buildInTypesWithBitMask;
    static const QMap<QString, QString> typeNameDataTypeConverter;

    static const QList<QString> illegalNames;

    static const QSet<QString> precodedTypesWithDebugOperator;
};

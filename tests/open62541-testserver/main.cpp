/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "testserver.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QVariant>
#include <QUuid>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    TestServer server;
    if (!server.init()) {
        qCritical() << "Could not initialize server.";
        return -1;
    }

    server.launch();

    int idx = server.registerNamespace(QLatin1String("http://qt-project.org"));
    if (idx != 2) {
        qWarning() << "Unexpected namespace index for qt-project namespace";
    }
    int ns2 = server.registerNamespace(QLatin1String("Test Namespace"));
    if (ns2 != 3) {
        qWarning() << "Unexpected namespace index for Test namespace";
    }

    const UA_NodeId largeTestFolder = server.addFolder("ns=1;s=Large.Folder", "Large_Folder");

    for (int x = 0; x < 100; ++x)
        server.addObject(largeTestFolder, idx, "");

    const UA_NodeId testFolder = server.addFolder("ns=3;s=TestFolder", "TestFolder");

    server.addVariable(testFolder, "ns=3;s=TestNode.ReadWrite", "TestNode.ReadWrite", 0.1, QOpcUa::Types::Double);

//    // TODO: Create Event

    // Test variables containing arrays of various types
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.Boolean", "BoolArrayTest", false, QOpcUa::Types::Boolean);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.Byte", "ByteArrayTest", 0, QOpcUa::Types::Byte);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.SByte", "SByteArrayTest", 0, QOpcUa::Types::SByte);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.Double", "DoubleArrayTest", 0.0, QOpcUa::Types::Double);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.Float", "FloatArrayTest", 0.0f, QOpcUa::Types::Float);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.Int16", "Int16ArrayTest", 0, QOpcUa::Types::Int16);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.Int32", "Int32ArrayTest", 0, QOpcUa::Types::Int32);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.Int64", "Int64ArrayTest", 0, QOpcUa::Types::Int64);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.UInt16", "UInt16ArrayTest", 0, QOpcUa::Types::UInt16);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.UInt32", "UInt32ArrayTest", 0, QOpcUa::Types::UInt32);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.UInt64", "UInt64ArrayTest", 0, QOpcUa::Types::UInt64);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.String", "StringArrayTest", QStringLiteral("Value 0"), QOpcUa::Types::String);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.LocalizedText", "LocalizedTextArrayTest",
                                                  QVariant::fromValue(QOpcUa::QLocalizedText(QStringLiteral("en_US"), QStringLiteral("Value 0"))), QOpcUa::Types::LocalizedText);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.ByteString", "ByteStringArrayTest", QByteArray("Value 0"), QOpcUa::Types::ByteString);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.DateTime", "DateTimeArrayTest", QDateTime::currentDateTime(), QOpcUa::Types::DateTime);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.NodeId", "NodeIdArrayTest", "ns=0;i=84", QOpcUa::Types::NodeId);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.Guid", "GuidArrayTest", QUuid::createUuid(), QOpcUa::Types::Guid);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.XmlElement", "XmlElementArrayTest", QString(), QOpcUa::Types::XmlElement);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.QualifiedName", "QualifiedNameArrayTest",
                                                  QVariant::fromValue(QOpcUa::QQualifiedName(0, QStringLiteral("Value 0"))), QOpcUa::Types::QualifiedName);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.StatusCode", "StatusCodeArrayTest",
                                               QVariant::fromValue(QOpcUa::UaStatusCode::Good), QOpcUa::Types::StatusCode);

    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.Range", "RangeArrayTest", QVariant::fromValue(QOpcUa::QRange(0, 100)), QOpcUa::Types::Range);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.EUInformation", "EUInformationArrayTest",
                                                    QVariant::fromValue(QOpcUa::QEUInformation()), QOpcUa::Types::EUInformation);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.ComplexNumber", "ComplexNumberArrayTest",
                                                    QVariant::fromValue(QOpcUa::QComplexNumber(1, 2)), QOpcUa::Types::ComplexNumber);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.DoubleComplexNumber", "DoubleComplexNumberArrayTest",
                                                    QVariant::fromValue(QOpcUa::QDoubleComplexNumber(1, 2)), QOpcUa::Types::DoubleComplexNumber);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.AxisInformation", "AxisInformationArrayTest",
                                                    QVariant::fromValue(QOpcUa::QAxisInformation()), QOpcUa::Types::AxisInformation);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Arrays.XV", "XVArrayTest",
                                                    QVariant::fromValue(QOpcUa::QXValue(1, 2)), QOpcUa::Types::XV);

    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.Boolean", "BoolScalarTest", false, QOpcUa::Types::Boolean);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.Byte", "ByteScalarTest", 0, QOpcUa::Types::Byte);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.SByte", "SByteScalarTest", 0, QOpcUa::Types::SByte);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.Double", "DoubleScalarTest", 0.0, QOpcUa::Types::Double);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.Float", "FloatScalarTest", 0.0f, QOpcUa::Types::Float);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.Int16", "Int16ScalarTest", 0, QOpcUa::Types::Int16);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.Int32", "Int32ScalarTest", 0, QOpcUa::Types::Int32);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.Int64", "Int64ScalarTest", 0, QOpcUa::Types::Int64);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.UInt16", "UInt16ScalarTest", 0, QOpcUa::Types::UInt16);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.UInt32", "UInt32ScalarTest", 0, QOpcUa::Types::UInt32);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.UInt64", "UInt64ScalarTest", 0, QOpcUa::Types::UInt64);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.String", "StringScalarTest", QStringLiteral("Value"), QOpcUa::Types::String);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.LocalizedText", "LocalizedTextScalarTest",
                                                  QVariant::fromValue(QOpcUa::QLocalizedText(QStringLiteral("en_US"), QStringLiteral("Value"))), QOpcUa::Types::LocalizedText);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.ByteString", "ByteStringScalarTest", QByteArray("Value"), QOpcUa::Types::ByteString);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.DateTime", "DateTimeScalarTest", QDateTime::currentDateTime(), QOpcUa::Types::DateTime);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.NodeId", "NodeIdScalarTest", "ns=0;i=84", QOpcUa::Types::NodeId);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.Guid", "GuidScalarTest", QUuid::createUuid(), QOpcUa::Types::Guid);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.XmlElement", "XmlElementScalarTest", QString(), QOpcUa::Types::XmlElement);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.QualifiedName", "QualifiedNameScalarTest",
                                                  QVariant::fromValue(QOpcUa::QQualifiedName(0, QStringLiteral("Value"))), QOpcUa::Types::QualifiedName);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.StatusCode", "StatusCodeScalarTest", QVariant::fromValue(QOpcUa::UaStatusCode::Good), QOpcUa::Types::StatusCode);


    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.Range", "RangeScalarTest", QVariant::fromValue(QOpcUa::QRange(0, 100)), QOpcUa::Types::Range);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.EUInformation", "EUInformationScalarTest",
                                                    QVariant::fromValue(QOpcUa::QEUInformation()), QOpcUa::Types::EUInformation);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.ComplexNumber", "ComplexNumberArrayTest",
                                                    QVariant::fromValue(QOpcUa::QComplexNumber(1, 2)), QOpcUa::Types::ComplexNumber);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.DoubleComplexNumber", "DoubleComplexNumberScalarTest",
                                                    QVariant::fromValue(QOpcUa::QDoubleComplexNumber(1, 2)), QOpcUa::Types::DoubleComplexNumber);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.AxisInformation", "AxisInformationScalarTest",
                                                    QVariant::fromValue(QOpcUa::QAxisInformation()), QOpcUa::Types::AxisInformation);
    server.addVariable(testFolder, "ns=2;s=Demo.Static.Scalar.XV", "XVScalarTest",
                                                    QVariant::fromValue(QOpcUa::QXValue(1, 2)), QOpcUa::Types::XV);

    UA_NodeId testStringIdsFolder = server.addFolder("ns=3;s=testStringIdsFolder", "testStringIdsFolder");
    server.addVariable(testStringIdsFolder, "ns=3;s=theStringId", "theStringId", QStringLiteral("Value"), QOpcUa::Types::String);
    UA_NodeId testGuidIdsFolder = server.addFolder("ns=3;s=testGuidIdsFolder", "testGuidIdsFolder");
    server.addVariable(testGuidIdsFolder, "ns=3;g=08081e75-8e5e-319b-954f-f3a7613dc29b", "theGuidId", QStringLiteral("Value"), QOpcUa::Types::String);
    UA_NodeId testOpaqueIdsFolder = server.addFolder("ns=3;s=testOpaqueIdsFolder", "testOpaqueIdsFolder");
    server.addVariable(testOpaqueIdsFolder, "ns=3;b=UXQgZnR3IQ==", "theOpaqueId", QStringLiteral("Value"), QOpcUa::Types::String);

    server.addMethod(testFolder, "ns=3;s=Test.Method.Multiply", "MultiplyDoubles");

    return app.exec();
}

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

    for (int x = 0; x <= 1000; x++)
        server.addObject(largeTestFolder, idx, "");

    const UA_NodeId testFolder = server.addFolder("ns=3;s=TestFolder", "TestFolder");

    server.addVariable<UA_Double, double, UA_TYPES_DOUBLE>(testFolder, "ns=3;s=TestNode.ReadWrite", "ReadWriteTest", 0.1);

//    // TODO: Create Event
//    // TODO: Server side methods

    // Test variables containing arrays of various types
    server.addVariable<UA_Boolean, bool, UA_TYPES_BOOLEAN>(testFolder, "ns=2;s=Demo.Static.Arrays.Boolean", "BoolArrayTest", false);
    server.addVariable<UA_Byte, uchar, UA_TYPES_BYTE>(testFolder, "ns=2;s=Demo.Static.Arrays.Byte", "ByteArrayTest", 0);
    server.addVariable<UA_SByte, char, UA_TYPES_SBYTE>(testFolder, "ns=2;s=Demo.Static.Arrays.SByte", "SByteArrayTest", 0);
    server.addVariable<UA_Double, double, UA_TYPES_DOUBLE>(testFolder, "ns=2;s=Demo.Static.Arrays.Double", "DoubleArrayTest", 0.);
    server.addVariable<UA_Float, float, UA_TYPES_FLOAT>(testFolder, "ns=2;s=Demo.Static.Arrays.Float", "FloatArrayTest", 0.f);
    server.addVariable<UA_Int16, qint16, UA_TYPES_INT16>(testFolder, "ns=2;s=Demo.Static.Arrays.Int16", "Int16ArrayTest", 0);
    server.addVariable<UA_Int32, qint32, UA_TYPES_INT32>(testFolder, "ns=2;s=Demo.Static.Arrays.Int32", "Int32ArrayTest", 0);
    server.addVariable<UA_Int64, qint64, UA_TYPES_INT64>(testFolder, "ns=2;s=Demo.Static.Arrays.Int64", "Int64ArrayTest", 0);
    server.addVariable<UA_UInt16, quint16, UA_TYPES_UINT16>(testFolder, "ns=2;s=Demo.Static.Arrays.UInt16", "UInt16ArrayTest", 0);
    server.addVariable<UA_UInt32, quint32, UA_TYPES_UINT32>(testFolder, "ns=2;s=Demo.Static.Arrays.UInt32", "UInt32ArrayTest", 0);
    server.addVariable<UA_UInt64, quint64, UA_TYPES_UINT64>(testFolder, "ns=2;s=Demo.Static.Arrays.UInt64", "UInt64ArrayTest", 0);
    server.addVariable<UA_String, QString, UA_TYPES_STRING>(testFolder, "ns=2;s=Demo.Static.Arrays.String", "StringArrayTest", QString());
    server.addVariable<UA_LocalizedText, QString, UA_TYPES_LOCALIZEDTEXT>(testFolder, "ns=2;s=Demo.Static.Arrays.LocalizedText", "LocalizedTextArrayTest", QString());
    server.addVariable<UA_ByteString, QByteArray, UA_TYPES_BYTESTRING>(testFolder, "ns=2;s=Demo.Static.Arrays.ByteString", "ByteStringArrayTest", QByteArray());
    server.addVariable<UA_DateTime, QDateTime, UA_TYPES_DATETIME>(testFolder, "ns=2;s=Demo.Static.Arrays.DateTime", "DateTimeArrayTest", QDateTime());
    server.addVariable<UA_NodeId, QString, UA_TYPES_NODEID>(testFolder, "ns=2;s=Demo.Static.Arrays.NodeId", "NodeIdArrayTest", QString());
    server.addVariable<UA_Guid, QUuid, UA_TYPES_GUID>(testFolder, "ns=2;s=Demo.Static.Arrays.Guid", "GuidArrayTest", QUuid());
    server.addVariable<UA_XmlElement, QString, UA_TYPES_XMLELEMENT>(testFolder, "ns=2;s=Demo.Static.Arrays.XmlElement", "XmlElementArrayTest", QString());
    server.addVariable<UA_QualifiedName, QPair<quint16, QString>, UA_TYPES_QUALIFIEDNAME>(testFolder, "ns=2;s=Demo.Static.Arrays.QualifiedName", "QualifiedNameArrayTest", QPair<quint16, QString>());
    server.addVariable<UA_StatusCode, UA_StatusCode, UA_TYPES_STATUSCODE>(testFolder, "ns=2;s=Demo.Static.Arrays.StatusCode", "StatusCodeArrayTest", UA_STATUSCODE_GOOD);

    server.addVariable<UA_Boolean, bool, UA_TYPES_BOOLEAN>(testFolder, "ns=2;s=Demo.Static.Scalar.Boolean", "BoolScalarTest", true);
    server.addVariable<UA_Byte, uchar, UA_TYPES_BYTE>(testFolder, "ns=2;s=Demo.Static.Scalar.Byte", "ByteScalarTest", 0);
    server.addVariable<UA_SByte, char, UA_TYPES_SBYTE>(testFolder, "ns=2;s=Demo.Static.Scalar.SByte", "SByteScalarTest", 0);
    server.addVariable<UA_Int16, qint16, UA_TYPES_INT16>(testFolder, "ns=2;s=Demo.Static.Scalar.Int16", "Int16ScalarTest", 0);
    server.addVariable<UA_Int32, qint32, UA_TYPES_INT32>(testFolder, "ns=2;s=Demo.Static.Scalar.Int32", "Int32ScalarTest", 0);
    server.addVariable<UA_Int64, qint64, UA_TYPES_INT64>(testFolder, "ns=2;s=Demo.Static.Scalar.Int64", "Int64ScalarTest", 0);
    server.addVariable<UA_UInt16, quint16, UA_TYPES_UINT16>(testFolder, "ns=2;s=Demo.Static.Scalar.UInt16", "UInt16ScalarTest", 0);
    server.addVariable<UA_UInt32, quint32, UA_TYPES_UINT32>(testFolder, "ns=2;s=Demo.Static.Scalar.UInt32", "UInt32ScalarTest", 0);
    server.addVariable<UA_UInt64, quint64, UA_TYPES_UINT64>(testFolder, "ns=2;s=Demo.Static.Scalar.UInt64", "UInt64ScalarTest", 0);
    server.addVariable<UA_Double, double, UA_TYPES_DOUBLE>(testFolder, "ns=2;s=Demo.Static.Scalar.Double", "DoubleScalarTest", 0.0);
    server.addVariable<UA_Float, float, UA_TYPES_FLOAT>(testFolder, "ns=2;s=Demo.Static.Scalar.Float", "FloatScalarTest", 0.0f);
    server.addVariable<UA_String, QString, UA_TYPES_STRING>(testFolder, "ns=2;s=Demo.Static.Scalar.String", "StringScalarTest", QString());
    server.addVariable<UA_DateTime, QDateTime, UA_TYPES_DATETIME>(testFolder, "ns=2;s=Demo.Static.Scalar.DateTime", "DateTimeScalarTest", QDateTime());
    server.addVariable<UA_LocalizedText, QString, UA_TYPES_LOCALIZEDTEXT>(testFolder, "ns=2;s=Demo.Static.Scalar.LocalizedText", "LocalizedTextScalarTest", QString());
    server.addVariable<UA_NodeId, QString, UA_TYPES_NODEID>(testFolder, "ns=2;s=Demo.Static.Scalar.NodeId", "NodeIdScalarTest", QString());
    server.addVariable<UA_ByteString, QByteArray, UA_TYPES_BYTESTRING>(testFolder, "ns=2;s=Demo.Static.Scalar.ByteString", "ByteStringScalarTest", QByteArray());
    UA_NodeId testStringIdsFolder = server.addFolder("ns=3;s=testStringIdsFolder", "testStringIdsFolder");
    server.addVariable<UA_String, QString, UA_TYPES_STRING>(testStringIdsFolder, "ns=3;s=theStringId", "theStringId", QString());
    server.addVariable<UA_Guid, QUuid, UA_TYPES_GUID>(testFolder, "ns=2;s=Demo.Static.Scalar.Guid", "GuidScalarTest", QUuid());
    server.addVariable<UA_XmlElement, QString, UA_TYPES_XMLELEMENT>(testFolder, "ns=2;s=Demo.Static.Scalar.XmlElement", "XmlElementScalarTest", QString());
    server.addVariable<UA_QualifiedName, QPair<quint16, QString>, UA_TYPES_QUALIFIEDNAME>(testFolder, "ns=2;s=Demo.Static.Scalar.QualifiedName", "QualifiedNameScalarTest", QPair<quint16, QString>());
    server.addVariable<UA_StatusCode, UA_StatusCode, UA_TYPES_STATUSCODE>(testFolder, "ns=2;s=Demo.Static.Scalar.StatusCode", "StatusCodeScalarTest", UA_STATUSCODE_GOOD);

    UA_NodeId testGuidIdsFolder = server.addFolder("ns=3;s=testGuidIdsFolder", "testGuidIdsFolder");
    server.addVariable<UA_String, QString, UA_TYPES_STRING>(testGuidIdsFolder, "ns=3;g=08081e75-8e5e-319b-954f-f3a7613dc29b", "theGuidId", QString());
    UA_NodeId testOpaqueIdsFolder = server.addFolder("ns=3;s=testOpaqueIdsFolder", "testOpaqueIdsFolder");
    server.addVariable<UA_String, QString, UA_TYPES_STRING>(testOpaqueIdsFolder, "ns=3;b=UXQgZnR3IQ==", "theOpaqueId", QString());

    UA_NodeId demoFolder = server.addFolder("ns=3;s=DemoFolder", "DemoFolder");
    UA_NodeId timeTickerNode = server.addVariable<UA_DateTime, QDateTime, UA_TYPES_DATETIME>(demoFolder, "ns=2;s=DemoFolder.TimeTicker", "TimeTicker", QDateTime());

    QTimer tickTimer;
    tickTimer.setInterval(1000);
    QObject::connect(&tickTimer, &QTimer::timeout, [&server, &timeTickerNode](){
        QDateTime current = QDateTime::currentDateTime();

        UA_Variant val;
        UA_DateTime tmpValue = UA_MSEC_TO_DATETIME * current.toMSecsSinceEpoch();
        UA_Variant_setScalarCopy(&val, &tmpValue, &UA_TYPES[UA_TYPES_DATETIME]);

        UA_StatusCode ret = UA_Server_writeValue(server.m_server, timeTickerNode, val);
        if (ret != UA_STATUSCODE_GOOD) {
            qWarning() << "Open62541 Server: Could not write Current DateTime:" << ret;
        }
    });
    tickTimer.start();


    return app.exec();
}

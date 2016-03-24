/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
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

#include "accontrol.h"

#include <opc/ua/node.h>
#include <opc/ua/subscription.h>
#include <opc/ua/server/server.h>

#include <opc/ua/protocol/node_management.h>
#include <opc/ua/protocol/string_utils.h>

#include <functional>

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QTimer>

using namespace OpcUa;

class SubClient : public SubscriptionHandler
{
    void DataChange(uint32_t handle, const Node& node, const Variant& val, AttributeId attr) override
    {
        Q_UNUSED(handle);
        Q_UNUSED(val);
        Q_UNUSED(attr);
        std::cout << "Received DataChange event for Node " << node << std::endl;
        m_server->TriggerEvent(*m_event);
    }

public:
    UaServer *m_server;
    OpcUa::Event *m_event;
};

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    OpcUa::UaServer server(false /*debug*/);
    server.SetEndpoint("opc.tcp://localhost:43344");
    server.SetServerURI("urn://exampleserver.qt-project.org");
    server.Start();
    uint32_t idx = server.RegisterNamespace("http://qt-project.org");

    Node objects = server.GetObjectsNode();

    Node largeTestFolder = objects.AddFolder("ns=1;s=Large.Folder", "Large_Folder");
    for (int x = 0; x <= 1000; x++)
        largeTestFolder.AddObject(idx, "");

    Node testFolder = objects.AddFolder("ns=3;s=TestFolder", "TestFolder");
    testFolder.AddVariable("ns=3;s=TestNode.ReadWrite", "ReadWriteTest", Variant(0.1));

    //addView(server.GetNode(ObjectId::ViewsFolder), "ns=3;i=5000", "ClassTestObject");

    //Create event
    Node triggerVar = testFolder.AddVariable("ns=3;s=TriggerVariable", "TriggerVariable", Variant(0));
    Node triggerNode = testFolder.AddObject("ns=3;s=TriggerNode", "TriggerNode");
    server.EnableEventNotification();
    Event ev(ObjectId::BaseEventType); //you should create your own type
    ev.Severity = 2;
    ev.SourceNode = triggerNode.GetId();
    ev.SourceName = "Test event";
    ev.Time = DateTime::Current();
    ev.Message = LocalizedText("test event");
    server.TriggerEvent(ev);

    // Workaround for not having server side methods
    SubClient clt;
    clt.m_event = &ev;
    clt.m_server = &server;
    std::unique_ptr<Subscription> sub = server.CreateSubscription(100, clt);
    sub->SubscribeDataChange(triggerVar);

    // Test variables containing arrays of various types
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.Boolean", "BoolArrayTest", std::vector<bool>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.Byte", "ByteArrayTest", std::vector<uint8_t>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.SByte", "SByteArrayTest", std::vector<int8_t>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.DateTime", "DateTimeArrayTest", std::vector<DateTime>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.Double", "DoubleArrayTest", std::vector<double>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.Float", "FloatArrayTest", std::vector<float>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.Int16", "Int16ArrayTest", std::vector<int16_t>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.Int32", "Int32ArrayTest", std::vector<int32_t>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.Int64", "Int64ArrayTest", std::vector<int64_t>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.String", "StringArrayTest", std::vector<std::string>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.UInt16", "UInt16ArrayTest", std::vector<uint16_t>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.UInt32", "UInt32ArrayTest", std::vector<uint32_t>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.UInt64", "UInt64ArrayTest", std::vector<uint64_t>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.NodeId", "NodeIdArrayTest", std::vector<NodeId>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.LocalizedText", "LocalizedTextArrayTest", std::vector<LocalizedText>());
    testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.ByteString", "ByteStringArrayTest", std::vector<ByteString>());
    //  testFolder.AddVariable("ns=2;s=Demo.Static.Arrays.XmlElement", "XmlElementArrayTest", std::vector<XmlElement>());

    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.Boolean", "BoolScalarTest", true);
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.Byte", "ByteScalarTest", uint8_t());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.SByte", "SByteScalarTest", int8_t());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.Int16", "Int16ScalarTest", int16_t());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.Int32", "Int32ScalarTest", int32_t());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.Int64", "Int64ScalarTest", int64_t());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.UInt16", "UInt16ScalarTest", uint16_t());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.UInt32", "UInt32ScalarTest", uint32_t());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.UInt64", "UInt64ScalarTest", uint64_t());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.Double", "DoubleScalarTest", 0.0);
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.Float", "FloatScalarTest", 0.0f);
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.String", "StringScalarTest", std::string());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.DateTime", "DateTimeScalarTest", DateTime());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.LocalizedText", "LocalizedTextScalarTest", LocalizedText());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.NodeId", "NodeIdScalarTest", NodeId());
    testFolder.AddVariable("ns=2;s=Demo.Static.Scalar.ByteString", "ByteStringScalarTest", ByteString());

    Node demoFolder = objects.AddFolder("ns=3;s=DemoFolder", "DemoFolder");
    Node timeTickerNode = demoFolder.AddVariable("ns=2;s=DemoFolder.TimeTicker", "TimeTicker", DateTime());

    QTimer tickTimer;
    tickTimer.setInterval(1000);
    QObject::connect(&tickTimer, &QTimer::timeout, [&timeTickerNode](){
        timeTickerNode.SetValue(DateTime::Current());
    });
    tickTimer.start();

    QObject::connect(qApp, &QCoreApplication::aboutToQuit, [&server](){
        qDebug("Stopping server");
        server.Stop();
    });

    // Initialize ACControl
    ACControl control;
    control.initNodes(server);

    app.exec();
}

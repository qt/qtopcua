// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick
import QtTest
import QtOpcUa as QtOpcUa

Item {
    property string backendName
    property int completedTestCases: 0
    property int availableTestCases: 0
    property bool completed: completedTestCases == availableTestCases
    property bool shouldRun: false

    onShouldRunChanged: {
        if (shouldRun)
            console.log("Running", parent.testName, "with", backendName);
    }

    QtOpcUa.Connection {
        id: connection
        backend: backendName
        defaultConnection: true
    }

    QtOpcUa.ServerDiscovery {
        id: serverDiscovery
        onServersChanged: {
            if (!count)
                return;
            endpointDiscovery.serverUrl = at(0).discoveryUrls[0];
        }
    }

    QtOpcUa.EndpointDiscovery {
        id: endpointDiscovery
        onEndpointsChanged: {
            if (!count)
                return;
            connection.connectToEndpoint(at(0));
        }
    }

    Component.onCompleted: {
        for (var i in children) {
            if (children[i].objectName == "TestCase")
                availableTestCases += 1;
        }
        serverDiscovery.discoveryUrl = OPCUA_DISCOVERY_URL;
    }

    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Reading multiple items"
        when: connection.connected && shouldRun

        SignalSpy {
            id: readNodeAttributesFinishedSpy
            target: connection
            signalName: "readNodeAttributesFinished"
        }

        function test_nodeTest() {
            var readItemList = [];
            var readItem;

            // Item #0
            readItem = QtOpcUa.ReadItem.create();
            readItem.ns = "http://qt-project.org";
            readItem.nodeId = "s=Demo.Static.Scalar.Double";
            readItem.attribute = QtOpcUa.Constants.NodeAttribute.DisplayName;
            readItemList.push(readItem);

            // Item #1
            readItem = QtOpcUa.ReadItem.create();
            readItem.ns = "http://qt-project.org";
            readItem.nodeId = "s=Demo.Static.Scalar.Double";
            // Value is the default attribute to read
            readItemList.push(readItem);

            // Item #2
            readItem = QtOpcUa.ReadItem.create();
            readItem.ns = "http://qt-project.org";
            readItem.nodeId = "s=Demo.Static.Arrays.UInt32";
            // Value is the default attribute to read
            readItem.indexRange = "0:2";
            readItemList.push(readItem);

            verify(connection.readNodeAttributes(readItemList))
            readNodeAttributesFinishedSpy.wait();

            compare(readNodeAttributesFinishedSpy.count, 1);
            compare(readNodeAttributesFinishedSpy.signalArguments[0].length, 1);

            var results = readNodeAttributesFinishedSpy.signalArguments[0][0];
            var now = new Date();

            compare(results.length, readItemList.length);
            for (var i = 0; i < results.length; i++) {
                console.log("Comparing result item #" + i);
                verify(results[i].status.isGood);
                compare(results[i].nodeId, readItemList[i].nodeId);
                compare(results[i].namespaceName, readItemList[i].ns);
                compare(results[i].attribute, readItemList[i].attribute);
                verify((now - results[i].serverTimestamp) < 10000);
            }

            compare(results[0].value.text, "DoubleScalarTest");
            compare(results[1].value, 1.0);
            compare(results[2].value, [1, 2]);
        }
    }

    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Writing multiple items"
        when: connection.connected && shouldRun

        SignalSpy {
            id: writeNodeAttributesFinishedSpy
            target: connection
            signalName: "writeNodeAttributesFinished"
        }

        function test_nodeTest() {
            var writeItemList = [];
            var writeItem;

            // Item #0
            writeItem = QtOpcUa.WriteItem.create();
            writeItem.ns = "http://qt-project.org";
            writeItem.nodeId = "s=Demo.Static.Scalar.String";
            writeItem.attribute = QtOpcUa.Constants.NodeAttribute.Value;
            writeItem.value = "Writing multiple items";
            writeItemList.push(writeItem);

            // Item #1
            writeItem = QtOpcUa.WriteItem.create();
            writeItem.ns = "http://qt-project.org";
            writeItem.nodeId = "s=Demo.Static.Scalar.Double";
            writeItem.value = 15.6;
            writeItemList.push(writeItem);

            // Item #2
            writeItem = QtOpcUa.WriteItem.create();
            writeItem.ns = "http://qt-project.org";
            writeItem.nodeId = "s=Demo.Static.Arrays.UInt32";
            writeItem.indexRange = "0:1";
            writeItem.value = [42, 43];
            writeItem.valueType = QtOpcUa.Constants.UInt32;
            writeItemList.push(writeItem);

            verify(connection.writeNodeAttributes(writeItemList))
            writeNodeAttributesFinishedSpy.wait();

            compare(writeNodeAttributesFinishedSpy.count, 1);
            compare(writeNodeAttributesFinishedSpy.signalArguments[0].length, 1);

            var results = writeNodeAttributesFinishedSpy.signalArguments[0][0];

            compare(results.length, writeItemList.length);
            for (var i = 0; i < results.length; i++) {
                console.log("Comparing result item #" + i);
                verify(results[i].status.isGood);
                compare(results[i].nodeId, writeItemList[i].nodeId);
                compare(results[i].namespaceName, writeItemList[i].ns);
                compare(results[i].attribute, writeItemList[i].attribute);
            }

            // Reset to default values
            //
            writeItemList = [];
            // Item #0
            writeItem = QtOpcUa.WriteItem.create();
            writeItem.ns = "http://qt-project.org";
            writeItem.nodeId = "s=Demo.Static.Scalar.String";
            writeItem.attribute = QtOpcUa.Constants.NodeAttribute.Value;
            writeItem.value = "Value";
            writeItemList.push(writeItem);

            // Item #1
            writeItem = QtOpcUa.WriteItem.create();
            writeItem.ns = "http://qt-project.org";
            writeItem.nodeId = "s=Demo.Static.Scalar.Double";
            writeItem.value = 1.0;
            writeItem.valueType = QtOpcUa.Constants.Double;
            writeItemList.push(writeItem);

            // Item #2
            writeItem = QtOpcUa.WriteItem.create();
            writeItem.ns = "http://qt-project.org";
            writeItem.nodeId = "s=Demo.Static.Arrays.UInt32";
            writeItem.indexRange = "0:1";
            writeItem.value = [1, 2];
            writeItem.valueType = QtOpcUa.Constants.UInt32;
            writeItemList.push(writeItem);

            writeNodeAttributesFinishedSpy.clear();
            verify(connection.writeNodeAttributes(writeItemList))
            writeNodeAttributesFinishedSpy.wait();

            var results = writeNodeAttributesFinishedSpy.signalArguments[0][0];

            compare(results.length, writeItemList.length);
            for (var i = 0; i < results.length; i++) {
                console.log("Comparing result item #" + i);
                verify(results[i].status.isGood);
                compare(results[i].nodeId, writeItemList[i].nodeId);
                compare(results[i].namespaceName, writeItemList[i].ns);
                compare(results[i].attribute, writeItemList[i].attribute);
            }

        }
    }
}

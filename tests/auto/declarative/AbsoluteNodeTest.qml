// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtTest
import QtOpcUa as QtOpcUa

Item {
    property string backendName
    property int completedTestCases: 0
    property int availableTestCases: 0
    property bool completed: completedTestCases == availableTestCases && availableTestCases > 0
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
        name: parent.parent.testName + ": " + backendName + ": Create String Node Id"
        when: node1.readyToUse && shouldRun

        function test_nodeTest() {
            compare(node1.value, "Value", "");
            compare(node1.browseName, "theStringId");
            compare(node1.nodeClass, QtOpcUa.Constants.NodeClass.Variable);
            compare(node1.displayName.text, "theStringId");
            compare(node1.description.text, "Description for ns=3;s=theStringId");
            compare(node1.status, QtOpcUa.Node.Status.Valid);
            tryCompare(node1, "monitored", true);
            compare(node1.publishingInterval, 100.0);

            compare(connection.currentEndpoint.endpointUrl, "opc.tcp://localhost:43344");
            compare(connection.currentEndpoint.securityPolicy, "http://opcfoundation.org/UA/SecurityPolicy#None");
            compare(connection.currentEndpoint.server.applicationUri, "urn:open62541.server.application");
        }

        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "Test Namespace"
                identifier: "s=theStringId"
            }
            id: node1
        }
    }

    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Create GUID Node Id"
        when: node2.readyToUse && shouldRun

        function test_nodeTest() {
            compare(node2.value, "Value", "");
            compare(node2.valueType, QtOpcUa.Constants.String);
        }

        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "Test Namespace"
                identifier: "g=08081e75-8e5e-319b-954f-f3a7613dc29b"
            }
            id: node2
        }
    }

    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Create Opaque Node Id"
        when: node3.readyToUse && shouldRun

        function test_nodeTest() {
            compare(node3.value, "Value", "");
            compare(node3.valueType, QtOpcUa.Constants.String);
        }

        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "Test Namespace"
                identifier: "b=UXQgZnR3IQ=="
            }
            id: node3
        }
    }

    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Create Integer Node Id"
        when: node4.readyToUse && shouldRun

        function test_nodeTest() {
            compare(node4.value, 255, "");
            compare(node4.valueType, QtOpcUa.Constants.Byte);
        }

        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "http://opcfoundation.org/UA/"
                identifier: "i=2267"
            }
            id: node4
        }
    }

    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Write Value Node with Multiple Listeners"
        when: node5.readyToUse && node6.readyToUse && node7.readyToUse && shouldRun

        function test_nodeTest() {
            compare(node5.valueType, QtOpcUa.Constants.Double);
            compare(node6.valueType, QtOpcUa.Constants.Double);
            compare(node7.valueType, QtOpcUa.Constants.Double);
            var oldValue = node5.value;
            node6Spy.clear();
            node7Spy.clear();
            node5.value = oldValue + 1;
            tryCompare(node5, "value", oldValue + 1);
            tryCompare(node6, "value", oldValue + 1);
            tryCompare(node7, "value", oldValue + 1);
            wait(100);
            compare(node6Spy.count, 1);
            compare(node7Spy.count, 1);
        }

        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "Test Namespace"
                identifier: "s=TestNode.ReadWrite"
            }
            id: node5
        }

        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "Test Namespace"
                identifier: "s=TestNode.ReadWrite"
            }
            id: node6
        }

        SignalSpy {
            id: node6Spy
            target: node6
            signalName: "valueChanged"
        }

        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "Test Namespace"
                identifier: "s=TestNode.ReadWrite"
            }
            id: node7
        }

        SignalSpy {
            id: node7Spy
            target: node7
            signalName: "valueChanged"
        }
    }

    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Emitting signals on node changes"
        when: node8.readyToUse && shouldRun

        function test_changeIdentifier() {
            compare(node8.valueType, QtOpcUa.Constants.Double);
            node8NamespaceSpy.clear();
            node8IdentifierSpy.clear();
            node8NodeIdSpy.clear();
            node8NodeChangedSpy.clear();
            node8NodeIdChangedSpy.clear();
            node8ValueSpy.clear();
            verify(node8IdentifierSpy.valid);
            node8.nodeId.identifier = "b=UXQgZnR3IQ==";
            node8ValueSpy.wait();

            // value has to be undefined because when node IDs are changed
            // all attributes become undefined before they get the new values.
            if (node8ValueSpy.count < 2)
                node8ValueSpy.wait();
            verify(!node8ValueSpy.signalArguments[0][0]);
            compare(node8ValueSpy.signalArguments[1][0], "Value");

            compare(node8IdentifierSpy.count, 1);
            compare(node8NodeIdSpy.count, 1);
            compare(node8NamespaceSpy.count, 0);
            verify(node8NodeChangedSpy.count > 0);
            compare(node8NodeIdChangedSpy.count, 1);
            compare(node8ValueSpy.count, 2); // first undefined, then the real value
            compare(node8.value, "Value");
        }

        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "Test Namespace"
                identifier: "s=TestNode.ReadWrite"
            }
            id: node8
        }

        SignalSpy {
            id: node8NamespaceSpy
            target: node8.nodeId
            signalName: "nodeNamespaceChanged"
        }

        SignalSpy {
            id: node8IdentifierSpy
            target: node8.nodeId
            signalName: "identifierChanged"
        }

        SignalSpy {
            id: node8NodeIdSpy
            target: node8.nodeId
            signalName: "nodeChanged"
        }

        SignalSpy {
            id: node8NodeChangedSpy
            target: node8
            signalName: "nodeChanged"
        }

        SignalSpy {
            id: node8NodeIdChangedSpy
            target: node8
            signalName: "nodeIdChanged"
        }

        SignalSpy {
            id: node8ValueSpy
            target: node8
            signalName: "valueChanged"
        }
    }

    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Node with namespace in Id"
        when: node9.readyToUse && shouldRun

        function test_nodeTest() {
            compare(node9.valueType, QtOpcUa.Constants.String);
            compare(node9.value, "Value", "");
        }

        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                identifier: "ns=3;s=theStringId"
            }
            id: node9
        }
    }

    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Standard attributes on variable node"
        when: node10.readyToUse && shouldRun

        SignalSpy {
            id: node10NodeClassSpy
            target: node10
            signalName: "nodeClassChanged"
        }

        SignalSpy {
            id: node10DisplayNameSpy
            target: node10
            signalName: "displayNameChanged"
        }

        SignalSpy {
            id: node10DescriptionSpy
            target: node10
            signalName: "descriptionChanged"
        }

        function test_nodeTest() {
            compare(node10.nodeClass, QtOpcUa.Constants.NodeClass.Variable);
            compare(node10.displayName.text, "FullyWritableTest");
            compare(node10.description.text, "Description for ns=3;s=Demo.Static.Scalar.FullyWritable");
            compare(node10.valueType, QtOpcUa.Constants.Double);

            node10NodeClassSpy.clear();
            node10DisplayNameSpy.clear();
            node10DescriptionSpy.clear();

            // The BrowseName attribute is readonly in open62541 v1.1

            // node class is not supposed to be changed: skipping tests

            node10.displayName.text = "modifiedDisplayName";
            node10DisplayNameSpy.wait();
            compare(node10DisplayNameSpy.count, 1);
            node10.displayName.text = "FullyWritableTest"; // Setting back to default

            node10.description.text = "modifiedDescription";
            node10DescriptionSpy.wait();
            compare(node10DescriptionSpy.count, 1);
            node10DescriptionSpy.clear();
            node10.description.text = "Description for ns=3;s=Demo.Static.Scalar.FullyWritable"; // Setting back to default
            node10DescriptionSpy.wait();
        }

        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "Test Namespace"
                identifier: "s=Demo.Static.Scalar.FullyWritable"
            }
            id: node10
        }
    }

    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Node timestamps"
        when: node11.readyToUse && shouldRun

        SignalSpy {
            id: node11ValueSpy
            target: node11
            signalName: "valueChanged"
        }

        function test_nodeTest() {
            compare(node11.valueType, QtOpcUa.Constants.String);
            node11ValueSpy.wait();

            var now = new Date();
            verify((now - node11.serverTimestamp) < 10000);
            verify((now - node11.sourceTimestamp) < 10000);
        }

        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                identifier: "ns=3;s=theStringId"
            }
            id: node11
        }
    }

    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Assign int value to double node"
        when: node12.readyToUse && shouldRun

        SignalSpy {
            id: node12ValueSpy
            target: node12
            signalName: "valueChanged"
        }

        function test_nodeTest() {
            compare(node12.valueType, QtOpcUa.Constants.Double);
            compare(node12.value, 1)

            // The Value attribute is read and subscribed
            // The initial value from the subscription doesn't generate a changed signal
            // unless the value is modified by setting the value attribute before the update is published.
            wait(1000);

            node12ValueSpy.clear();
            node12.value = 30;
            node12ValueSpy.wait();

            verify(node12ValueSpy.count > 0);
            compare(node12.value, 30);
            compare(node12.value, 30.0);

            // There is a race condition where the local value can be updated after write success
            // before the previous value change from the monitoring arrives.
            wait(1000);

            node12ValueSpy.clear();
            node12.value = 1;
            node12ValueSpy.wait();
            compare(node12.value, 1)
        }

        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "http://qt-project.org"
                identifier: "s=Demo.Static.Scalar.Double"
            }
            id: node12
        }
    }
}

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
        name: parent.parent.testName + ": " + backendName + ": Standard attributes on method node"
        when: node1.readyToUse && shouldRun

        SignalSpy {
            id: node1BrowseNameSpy
            target: node1
            signalName: "browseNameChanged"
        }

        SignalSpy {
            id: node1NodeClassSpy
            target: node1
            signalName: "nodeClassChanged"
        }

        SignalSpy {
            id: node1DisplayNameSpy
            target: node1
            signalName: "displayNameChanged"
        }

        SignalSpy {
            id: node1DescriptionSpy
            target: node1
            signalName: "descriptionChanged"
        }

        SignalSpy {
            id: node1ObjectNodeIdSpy
            target: node1
            signalName: "objectNodeIdChanged"
        }

        function test_nodeTest() {
            compare(node1.browseName, "multiplyArguments");
            compare(node1.nodeClass, QtOpcUa.Constants.NodeClass.Method);
            compare(node1.displayName.text, "ns=3;s=Test.Method.Multiply");
            compare(node1.description.text, "MultiplyDoubles");
            compare(node1.objectNodeId.ns, "Test Namespace");
            compare(node1.objectNodeId.identifier, "s=TestFolder");

            compare(node1BrowseNameSpy.count, 1)
            compare(node1NodeClassSpy.count, 1)
            compare(node1DisplayNameSpy.count, 1)
            compare(node1DescriptionSpy.count, 1)
            compare(node1ObjectNodeIdSpy.count, 0)

            node1.objectNodeId.identifier = "s=foo";
            node1ObjectNodeIdSpy.wait()
            compare(node1ObjectNodeIdSpy.count, 1);
        }

        QtOpcUa.MethodNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "Test Namespace"
                identifier: "s=Test.Method.Multiply"
            }
            objectNodeId: QtOpcUa.NodeId {
                                ns: "Test Namespace"
                                identifier: "s=TestFolder"
                        }
            id: node1
        }
    }

    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Use method arguments"
        when: node2.readyToUse && shouldRun

        SignalSpy {
            id: node2resultStatusSpy
            target: node2
            signalName: "resultStatusChanged"
        }

        function test_nodeTest() {
            compare(node2.browseName, "multiplyArguments");
            compare(node2.nodeClass, QtOpcUa.Constants.NodeClass.Method);
            compare(node2.resultStatus.status, QtOpcUa.Status.Good);
            verify(node2.resultStatus.isGood);

            node2.callMethod();

            node2resultStatusSpy.wait();
            compare(node2resultStatusSpy.count, 1);
            compare(node2.resultStatus.status, QtOpcUa.Status.Good);
            verify(node2.resultStatus.isGood);
            compare(node2.outputArguments.length, 1);
            compare(node2.outputArguments[0], 12);
        }

        QtOpcUa.MethodNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "Test Namespace"
                identifier: "s=Test.Method.Multiply"
            }
            objectNodeId: QtOpcUa.NodeId {
                                ns: "Test Namespace"
                                identifier: "s=TestFolder"
                        }
            inputArguments: [
                QtOpcUa.MethodArgument {
                    value: 3
                    type: QtOpcUa.Constants.Double
                },
                QtOpcUa.MethodArgument {
                    value: 4
                    type: QtOpcUa.Constants.Double
                }
            ]
            id: node2
        }
    }

    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Multiple Output Arguments"
        when: node3.readyToUse && shouldRun

        SignalSpy {
            id: node3resultStatusSpy
            target: node3
            signalName: "resultStatusChanged"
        }

        function test_nodeTest() {
            compare(node3.browseName, "multipleOutputArguments");
            compare(node3.nodeClass, QtOpcUa.Constants.NodeClass.Method);
            compare(node3.resultStatus.status, QtOpcUa.Status.Good);
            verify(node3.resultStatus.isGood);

            node3.callMethod();

            node3resultStatusSpy.wait();
            compare(node3resultStatusSpy.count, 1);
            compare(node3.resultStatus.status, QtOpcUa.Status.Good);
            verify(node3.resultStatus.isGood);
            // Check for two output arguments
            compare(node3.outputArguments.length, 2);
            compare(node3.outputArguments[0], 12);
            compare(node3.outputArguments[1].text, "some text argument");
        }

        QtOpcUa.MethodNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "Test Namespace"
                identifier: "s=Test.Method.MultipleOutputArguments"
            }
            objectNodeId: QtOpcUa.NodeId {
                                ns: "Test Namespace"
                                identifier: "s=TestFolder"
                        }
            inputArguments: [
                QtOpcUa.MethodArgument {
                    value: 3
                    type: QtOpcUa.Constants.Double
                },
                QtOpcUa.MethodArgument {
                    value: 4
                    type: QtOpcUa.Constants.Double
                }
            ]
            id: node3
        }
    }
}

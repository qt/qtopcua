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

        Binding on discoveryUrl {
            when: shouldRun && Component.completed
            value: OPCUA_DISCOVERY_URL
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
    }

    Timer {
        id: timer
        interval: 2000
    }

/*  // In order to run this test you need to compile the example tutorial_server_event from open62541 master branch.
    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Event filter"
        when: node3.readyToUse && shouldRun

        function test_nodeTest() {
            eventSpy.clear();
            node4.callMethod();
            eventSpy.wait();
            compare(eventSpy.count, 1);
            compare(eventSpy.signalArguments[0].length, 1);
            compare(eventSpy.signalArguments[0][0].length, 2);
            compare(eventSpy.signalArguments[0][0][0], 100);
            compare(eventSpy.signalArguments[0][0][1].text, "An event has been generated.");
        }

        QtOpcUa.Node {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "http://opcfoundation.org/UA/"
                identifier: "i=2253" // Server object
            }
            id: node3

            eventFilter: QtOpcUa.EventFilter {
                select: [
                          QtOpcUa.SimpleAttributeOperand {
                            browsePath: [
                                          QtOpcUa.NodeId {
                                              identifier: "Severity"
                                              ns: "http://opcfoundation.org/UA/"
                                          }
                                        ]
                          },
                          QtOpcUa.SimpleAttributeOperand {
                            browsePath: [
                                          QtOpcUa.NodeId {
                                              identifier: "Message"
                                              ns: "http://opcfoundation.org/UA/"
                                          }
                                        ]
                          }
                ]

// Where clause is not supported by server yet
//                where: [
//                    QtOpcUa.FilterElement {
//                        operator: QtOpcUa.FilterElement.GreaterThanOrEqual
//                        firstOperand: QtOpcUa.SimpleAttributeOperand {
//                            browsePath: [
//                                          QtOpcUa.NodeId {
//                                              identifier: "Severity"
//                                              ns: "http://opcfoundation.org/UA/"
//                                          }
//                                        ]
//                        }
//                        secondOperand: QtOpcUa.LiteralOperand {
//                                        value: 700
//                                        type: QtOpcUa.Constants.UInt16
//                         }
//                    }
//                ]
            }
        }

        QtOpcUa.MethodNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "http://opcfoundation.org/UA/"
                identifier: "i=62541"
            }
            objectNodeId: QtOpcUa.NodeId {
                ns: "http://opcfoundation.org/UA/"
                identifier: "i=85"
            }
            id: node4
        }

        SignalSpy {
            id: eventSpy
            target: node3
            signalName: "eventOccurred"
        }
    }
*/
}



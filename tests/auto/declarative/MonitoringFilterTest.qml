/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

import QtQuick 2.3
import QtTest 1.0
import QtOpcUa 5.13 as QtOpcUa

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



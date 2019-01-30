/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
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

    QtOpcUa.Connection {
        id: connection
        backend: availableBackends[0]
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
        serverDiscovery.discoveryUrl = "opc.tcp://127.0.0.1:43344";
    }

    TestCase {
        name: "Standard attributes on method node"
        when: node1.readyToUse

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

    TestCase {
        name: "Use method arguments"
        when: node2.readyToUse

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

    TestCase {
        name: "Multiple Output Arguments"
        when: node3.readyToUse

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

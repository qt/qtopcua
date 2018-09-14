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
import QtOpcUa 5.12 as QtOpcUa

Item {

    QtOpcUa.Connection {
        id: connection
        backend: "open62541"
        defaultConnection: true
    }

    Component.onCompleted: {
        connection.connectToEndpoint("opc.tcp://127.0.0.1:43344");
    }

    TestCase {
        name: "RelativeNodePath"
        when: node1.readyToUse

        function test_nodeRead() {
              tryVerify(function() { return node1.value > 0 });
        }

        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.RelativeNodeId {
                  startNode: QtOpcUa.NodeId {
                                ns: "Test Namespace"
                                identifier: "s=TestFolder"
                             }
                  path: [ QtOpcUa.RelativeNodePath {
                             ns: "Test Namespace"
                             browseName: "TestNode.ReadWrite"
                        }
                        ]
            }
            id: node1
        }
    }

    TestCase {
        name: "CascadedRelativeNodePath"
        when: node2.readyToUse

        function test_nodeRead() {
              tryVerify(function() { return node2.value > 0 });
        }

        QtOpcUa.RelativeNodeId {
              startNode: QtOpcUa.NodeId {
                            ns: "http://opcfoundation.org/UA/"
                            identifier: "i=85"
                         }
              path: [ QtOpcUa.RelativeNodePath {
                         ns: "Test Namespace"
                         browseName: "TestFolder"
                    }
                    ]
              id: secondLevelNode
        }


        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.RelativeNodeId {
                  startNode: secondLevelNode
                  path: [ QtOpcUa.RelativeNodePath {
                             ns: "Test Namespace"
                             browseName: "TestNode.ReadWrite"
                        }
                        ]
            }
            id: node2
        }
    }

    TestCase {
        name: "Emitting signals on node changes"
        when: node3.readyToUse

        function test_changeStartNode() {
            node3NamespaceSpy.clear();
            node3NodeIdSpy.clear();
            node3NodeChangedSpy.clear();
            node3NodeIdChangedSpy.clear();
            node3ValueSpy.clear();
            node3PathSpy.clear();
            node3.nodeId.startNode.identifier = "s=TestFolder2";
            node3ValueSpy.wait();

            // value has to be undefined because when node IDs are changed
            // all attributes become undefined before they get the new values.
            if (node3ValueSpy.count < 2)
                node3ValueSpy.wait();
            verify(!node3ValueSpy.signalArguments[0][0]);
            compare(node3ValueSpy.signalArguments[1][0], 0.1);

            compare(node3NodeIdSpy.count, 1);
            compare(node3NamespaceSpy.count, 0);
            verify(node3NodeChangedSpy.count > 0);
            compare(node3PathSpy.count, 0);
            compare(node3NodeIdChangedSpy.count, 1);
            compare(node3ValueSpy.count, 2); // first undefined, then the real value
            compare(node3.value, 0.1);
        }

        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.RelativeNodeId {
                  startNode: QtOpcUa.NodeId {
                                ns: "Test Namespace"
                                identifier: "s=TestFolder"
                             }
                  path: [ QtOpcUa.RelativeNodePath {
                             ns: "Test Namespace"
                             browseName: "TestNode.ReadWrite"
                        }
                        ]
            }
            id: node3
        }

        SignalSpy {
            id: node3NamespaceSpy
            target: node3.nodeId
            signalName: "nodeNamespaceChanged"
        }

        SignalSpy {
            id: node3NodeIdSpy
            target: node3.nodeId
            signalName: "nodeChanged"
        }

        SignalSpy {
            id: node3NodeChangedSpy
            target: node3
            signalName: "nodeChanged"
        }

        SignalSpy {
            id: node3NodeIdChangedSpy
            target: node3
            signalName: "nodeIdChanged"
        }

        SignalSpy {
            id: node3ValueSpy
            target: node3
            signalName: "valueChanged"
        }

        SignalSpy {
            id: node3PathSpy
            target: node3.nodeId
            signalName: "pathChanged"
        }
    }

    TestCase {
        name: "Emitting signals on cascaded node changes"
        when: node4.readyToUse

        function test_nodeRead() {
            node4NamespaceSpy.clear();
            node4NodeIdSpy.clear();
            node4NodeChangedSpy.clear();
            node4NodeIdChangedSpy.clear();
            node4ValueSpy.clear();
            node4PathSpy.clear();
            node4.nodeId.startNode.path = replacementNode.createObject(parent);
            node4NodeChangedSpy.wait();
            node4ValueSpy.wait();

            // value has to be undefined because when node IDs are changed
            // all attributes become undefined before they get the new values.
            if (node4ValueSpy.count < 2)
                node4ValueSpy.wait();
            verify(!node4ValueSpy.signalArguments[0][0]);
            compare(node4ValueSpy.signalArguments[1][0], 0.1);

            compare(node4NodeIdSpy.count, 2);
            compare(node4NamespaceSpy.count, 0);
            verify(node4NodeChangedSpy.count > 0);
            compare(node4NodeIdChangedSpy.count, 1);
            compare(node4ValueSpy.count, 2); // first undefined, then the real value
            compare(node4.value, 0.1);
            compare(node4PathSpy.count, 2);
        }

        QtOpcUa.RelativeNodeId {
              startNode: QtOpcUa.NodeId {
                            ns: "http://opcfoundation.org/UA/"
                            identifier: "i=85"
                         }
              path: [ QtOpcUa.RelativeNodePath {
                         ns: "Test Namespace"
                         browseName: "TestFolder"
                    }
                    ]
              id: secondLevelNode2
        }


        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.RelativeNodeId {
                  startNode: secondLevelNode2
                  path: [ QtOpcUa.RelativeNodePath {
                             ns: "Test Namespace"
                             browseName: "TestNode.ReadWrite"
                        }
                        ]
            }
            id: node4
        }

        Component {
            id: replacementNode
            QtOpcUa.RelativeNodePath {
                ns: "Test Namespace";
                browseName: "TestFolder2";
            }
        }

        SignalSpy {
            id: node4NamespaceSpy
            target: node4.nodeId
            signalName: "nodeNamespaceChanged"
        }

        SignalSpy {
            id: node4NodeIdSpy
            target: node4.nodeId
            signalName: "nodeChanged"
        }

        SignalSpy {
            id: node4NodeChangedSpy
            target: node4
            signalName: "nodeChanged"
        }

        SignalSpy {
            id: node4NodeIdChangedSpy
            target: node4
            signalName: "nodeIdChanged"
        }

        SignalSpy {
            id: node4ValueSpy
            target: node4
            signalName: "valueChanged"
        }

        SignalSpy {
            id: node4PathSpy
            target: node4.nodeId.startNode
            signalName: "pathChanged"
        }
    }
}

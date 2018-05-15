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
        backend: connection.availableBackends[0]
        defaultConnection: true
    }

    Component.onCompleted: {
        connection.connectToEndpoint("opc.tcp://127.0.0.1:43344");
    }

    TestCase {
        name: "Create String Node Id"
        when: node1.readyToUse

        function test_nodeTest() {
            compare(node1.value, "Value", "");
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

    TestCase {
        name: "Create GUID Node Id"
        when: node2.readyToUse

        function test_nodeTest() {
            compare(node2.value, "Value", "");
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

    TestCase {
        name: "Create Opaque Node Id"
        when: node3.readyToUse

        function test_nodeTest() {
            compare(node3.value, "Value", "");
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

    TestCase {
        name: "Create Integer Node Id"
        when: node4.readyToUse

        function test_nodeTest() {
            compare(node4.value, 255, "");
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

    TestCase {
        name: "Write Value Node with Multiple Listeners"
        when: node5.readyToUse && node6.readyToUse && node7.readyToUse

        function test_nodeTest() {
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

    TestCase {
        name: "Emitting signals on node changes"
        when: node8.readyToUse

        function test_changeIdentifier() {
            node8NamespaceSpy.clear();
            node8IdentifierSpy.clear();
            node8NodeIdSpy.clear();
            node8NodeChangedSpy.clear();
            node8NodeIdChangedSpy.clear();
            node8ValueSpy.clear();
            verify(node8IdentifierSpy.valid);
            node8.nodeId.identifier = "b=UXQgZnR3IQ==";
            node8ValueSpy.wait();
            compare(node8IdentifierSpy.count, 1);
            compare(node8NodeIdSpy.count, 1);
            compare(node8NamespaceSpy.count, 0);
            verify(node8NodeChangedSpy.count > 0);
            compare(node8NodeIdChangedSpy.count, 1);
            compare(node8ValueSpy.count, 1);
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

    TestCase {
        name: "Node with namespace in Id"
        when: node9.readyToUse

        function test_nodeTest() {
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

}

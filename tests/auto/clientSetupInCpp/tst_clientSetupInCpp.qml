// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtTest
import QtOpcUa as QtOpcUa
import App

Item {
    QtOpcUa.Connection {
        id: connection
        connection: myclass.connection
    }

    MyClass {
        id: myclass
    }

    Component.onCompleted: myclass.startConnection()

    TestCase {
        name: "Create String Node Id"
        when: node1.readyToUse

        function test_nodeTest() {
            compare(node1.value, "Value", "");
            compare(node1.browseName, "theStringId");
            compare(node1.nodeClass, QtOpcUa.Constants.NodeClass.Variable);
            compare(node1.displayName.text, "theStringId");
            compare(node1.description.text, "Description for ns=3;s=theStringId");
            compare(node1.status, QtOpcUa.Node.Status.Valid);
            tryCompare(node1, "monitored", true);
            compare(node1.publishingInterval, 100.0);

            compare(connection.currentEndpoint.endpointUrl, "opc.tcp://127.0.0.1:43344");
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
}

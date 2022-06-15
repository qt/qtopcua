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

    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Username authentication"

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

        QtOpcUa.ValueNode {
              connection: connection
              nodeId: QtOpcUa.NodeId {
                  ns: "Test Namespace"
                  identifier: "s=theStringId"
              }
              id: node1
        }

        SignalSpy {
            id: connection1ConnectedSpy
            target: connection
            signalName: "connectedChanged"
        }

        SignalSpy {
            id: node1ValueChangedSpy
            target: node1
            signalName: "valueChanged"
        }

        function test_nodeTest() {
            var authInfo = connection.authenticationInformation;
            authInfo.setUsernameAuthentication("user1", "password");
            connection.authenticationInformation = authInfo;

            serverDiscovery.discoveryUrl = OPCUA_DISCOVERY_URL;

            connection1ConnectedSpy.wait();
            verify(connection.connected);

            node1ValueChangedSpy.wait();
            verify(node1ValueChangedSpy.count > 0);

            compare(node1.value, "Value", "");
            compare(node1.browseName, "theStringId");
        }
    }
}

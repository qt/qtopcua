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

    Component.onCompleted: {
        for (var i in children) {
            if (children[i].objectName == "TestCase")
                availableTestCases += 1;
        }
    }

    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Fetch data from discovery server using default connection"
        when: shouldRun

        QtOpcUa.Connection {
            id: connection1
            backend: backendName
            defaultConnection: true
        }

        QtOpcUa.ServerDiscovery {
            // uses default connection
            id: myServers1
        }

        QtOpcUa.EndpointDiscovery {
            // uses default connection
            id: myEndpoints1
        }

        QtOpcUa.ValueNode {
            connection: connection1
            nodeId: QtOpcUa.NodeId {
                ns: "Test Namespace"
                identifier: "s=theStringId"
            }
            id: node1
        }

        SignalSpy {
            id: serversChangedSpy1
            target: myServers1
            signalName: "serversChanged"
        }

        SignalSpy {
            id: serversCountSpy1
            target: myServers1
            signalName: "countChanged"
        }

        SignalSpy {
            id: serversStatusSpy1
            target: myServers1
            signalName: "statusChanged"
        }

        SignalSpy {
            id: endpointsChangedSpy1
            target: myEndpoints1
            signalName: "endpointsChanged"
        }

        SignalSpy {
            id: endpointsCountSpy1
            target: myEndpoints1
            signalName: "countChanged"
        }

        SignalSpy {
            id: endpointsStatusSpy1
            target: myEndpoints1
            signalName: "statusChanged"
        }

        SignalSpy {
            id: connectedSpy1
            target: node1
            signalName: "readyToUseChanged"
        }

        function test_nodeTest() {
            compare(myServers1.count, 0);

            myServers1.discoveryUrl = OPCUA_DISCOVERY_URL;

            serversStatusSpy1.wait();
            compare(myServers1.status.status, QtOpcUa.Status.GoodCompletesAsynchronously);

            tryVerify(function() { return myServers1.count > 0;});
            compare(serversCountSpy1.count, 1);
            compare(serversChangedSpy1.count, 2);
            compare(myServers1.count, 1);
            verify(myServers1.at(0).discoveryUrls[0].startsWith("opc.tcp://"));

            compare(myEndpoints1.count, 0);

            myEndpoints1.serverUrl = myServers1.at(0).discoveryUrls[0];
            endpointsStatusSpy1.wait();
            compare(myEndpoints1.status.status, QtOpcUa.Status.GoodCompletesAsynchronously);
            compare(endpointsCountSpy1.count, 0);
            if (endpointsStatusSpy1.count == 1)
                endpointsStatusSpy1.wait();
            compare(endpointsStatusSpy1.count, 2);
            compare(endpointsChangedSpy1.count, 2);

            tryVerify(function() { return myEndpoints1.count > 0;});
            if (SERVER_SUPPORTS_SECURITY)
                compare(myEndpoints1.count, connection1.supportedSecurityPolicies.length === 3 ? 5 : 9);
            else
                compare(myEndpoints1.count, 1);
            verify(myEndpoints1.at(0).endpointUrl.startsWith("opc.tcp://"));
            compare(myEndpoints1.at(0).securityPolicy, "http://opcfoundation.org/UA/SecurityPolicy#None");

            connection1.connectToEndpoint(myEndpoints1.at(0));

            connectedSpy1.wait();
            compare(node1.value, "Value", "");
            compare(node1.browseName, "theStringId");
        }
    }

    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Fetch data from discovery server using specified connection"
        when: shouldRun

        QtOpcUa.Connection {
            id: connection2
            backend: backendName
        }

        QtOpcUa.ServerDiscovery {
            connection: connection2
            id: myServers2
        }

        QtOpcUa.EndpointDiscovery {
            id: myEndpoints2
            connection: connection2
        }

        QtOpcUa.ValueNode {
            connection: connection2
            nodeId: QtOpcUa.NodeId {
                ns: "Test Namespace"
                identifier: "s=theStringId"
            }
            id: node2
        }

        SignalSpy {
            id: serversChangedSpy2
            target: myServers2
            signalName: "serversChanged"
        }

        SignalSpy {
            id: serversCountSpy2
            target: myServers2
            signalName: "countChanged"
        }

        SignalSpy {
            id: serversStatusSpy2
            target: myServers2
            signalName: "statusChanged"
        }

        SignalSpy {
            id: endpointsChangedSpy2
            target: myEndpoints2
            signalName: "endpointsChanged"
        }

        SignalSpy {
            id: endpointsCountSpy2
            target: myEndpoints2
            signalName: "countChanged"
        }

        SignalSpy {
            id: endpointsStatusSpy2
            target: myEndpoints2
            signalName: "statusChanged"
        }

        SignalSpy {
            id: connectedSpy2
            target: node2
            signalName: "readyToUseChanged"
        }

        function test_nodeTest() {
            compare(myServers2.count, 0);

            myServers2.discoveryUrl = OPCUA_DISCOVERY_URL;

            serversStatusSpy2.wait();
            compare(myServers2.status.status, QtOpcUa.Status.GoodCompletesAsynchronously);

            tryVerify(function() { return myServers2.count > 0;});
            compare(serversStatusSpy2.count, 2);
            compare(serversChangedSpy2.count, 2);
            compare(myServers2.count, 1);
            verify(myServers2.at(0).discoveryUrls[0].startsWith("opc.tcp://"));

            myEndpoints2.serverUrl = myServers2.at(0).discoveryUrls[0];
            endpointsStatusSpy2.wait();
            compare(myEndpoints2.status.status, QtOpcUa.Status.GoodCompletesAsynchronously);

            tryVerify(function() { return myEndpoints2.count > 0;});
            compare(endpointsCountSpy2.count, 1);
            compare(endpointsStatusSpy2.count, 2);
            compare(endpointsChangedSpy2.count, 2);
            if (SERVER_SUPPORTS_SECURITY)
                compare(myEndpoints2.count, connection2.supportedSecurityPolicies.length === 3 ? 5 : 9);
            else
                compare(myEndpoints2.count, 1);
            verify(myEndpoints2.at(0).endpointUrl.startsWith("opc.tcp://"));
            compare(myEndpoints2.at(0).securityPolicy, "http://opcfoundation.org/UA/SecurityPolicy#None");

            connection2.connectToEndpoint(myEndpoints2.at(0));

            connectedSpy2.wait();
            compare(node2.value, "Value", "");
            compare(node2.browseName, "theStringId");
        }
    }
}

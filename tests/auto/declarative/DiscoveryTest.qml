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
                compare(myEndpoints1.count, 5);
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
                compare(myEndpoints2.count, 5);
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

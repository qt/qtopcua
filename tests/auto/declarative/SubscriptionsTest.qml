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

    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Create String Node Id"
        when: node1.readyToUse && shouldRun

        function test_nodeTest() {
            tryCompare(node1, "monitored", true);
            compare(node1.publishingInterval, 100.0);
            node1ValueSpy.clear();
            verify(node1.value != "foo");
            node2.value = "foo";
            node1ValueSpy.wait(10000);
            compare(node1ValueSpy.count, 1);

            compare(node1.monitored, true);
            node1MonitoredSpy.clear();
            node1.monitored = false;
            node1MonitoredSpy.wait();
            compare(node1MonitoredSpy.count, 1);
            compare(node1IntervalSpy.count, 0);
            compare(node1.monitored, false);

            node1ValueSpy.clear();
            node2.value = "bar";
            timer.running = true;
            timerSpy.wait();
            compare(node1ValueSpy.count, 0);

            node1MonitoredSpy.clear();
            node1.monitored = true;
            node1MonitoredSpy.wait();
            compare(node1MonitoredSpy.count, 1);
            compare(node1IntervalSpy.count, 0);
            compare(node1.monitored, true);

            // Wait for values to settle
            tryCompare(node1, "value", "bar");

            // The value needs to be reset to "Value" for follow up tests to succeed.
            node1ValueSpy.clear();
            node2.value = "Value";
            node1ValueSpy.wait();
            compare(node1ValueSpy.count, 1);

            node1MonitoredSpy.clear();
            node1IntervalSpy.clear();
            node1.publishingInterval = 200.0;
            node1IntervalSpy.wait();
            compare(node1MonitoredSpy.count, 0);
            compare(node1IntervalSpy.count, 1);
            compare(node1.publishingInterval, 200.0);
        }

        SignalSpy {
            id: node1ValueSpy
            target: node1
            signalName: "valueChanged"
        }

        SignalSpy {
            id: node1IntervalSpy
            target: node1
            signalName: "publishingIntervalChanged"
        }

        SignalSpy {
            id: node1MonitoredSpy
            target: node1
            signalName: "monitoredChanged"
        }

        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "Test Namespace"
                identifier: "s=theStringId"
            }
            id: node1
        }

        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "Test Namespace"
                identifier: "s=theStringId"
            }
            id: node2
        }

        Timer {
            id: timer
            interval: 2000
        }

        SignalSpy {
            id: timerSpy
            target: timer
            signalName: "triggered"
        }
    }
}

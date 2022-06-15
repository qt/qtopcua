// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Data Change Filter"
        when: node1.readyToUse && shouldRun

        function test_nodeTest() {
            tryCompare(node1, "monitored", true);
            compare(node2.monitored, false);
            compare(node1.publishingInterval, 100.0);

            compare(node1.filter.deadbandType, 1);
            compare(node1.filter.trigger, 1);
            compare(node1.filter.deadbandValue, 1);
            if (node1ValueSpy.count == 0)
                node1ValueSpy.wait();

            verify(node1.value != 1.5);

            timer.running = true;
            timerSpy.wait();

            node1ValueSpy.clear();
            node2ValueSpy.clear();
            node2.value = 1.5;
            node2ValueSpy.wait(); // wait for value written
            timer.running = true;
            timerSpy.wait();
            compare(node1ValueSpy.count, 0);
            compare(node1.value, 1.0); // still 1.0 because of filtering

            node1ValueSpy.clear();
            node2ValueSpy.clear();
            verify(node1.value != 30);
            node2.value = 30.0;
            verify(node1.value != 30);
            node2ValueSpy.wait(); // wait for value written
            node1ValueSpy.wait();
            compare(node1.value, 30);
            compare(node1ValueSpy.count, 1);

            // Reset values
            node1ValueSpy.clear();
            node2ValueSpy.clear();
            node2.value = 1.0;
            node2ValueSpy.wait(); // wait for value written
            node1ValueSpy.wait();
            compare(node1ValueSpy.count, 1);
            compare(node1.value, 1.0);
        }

        SignalSpy {
            id: node1ValueSpy
            target: node1
            signalName: "valueChanged"
        }

        SignalSpy {
            id: node2ValueSpy
            target: node2
            signalName: "valueChanged"
        }

        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "http://qt-project.org"
                identifier: "s=Demo.Static.Scalar.Double"
            }
            id: node1

            filter: QtOpcUa.DataChangeFilter {
                deadbandValue: 1.0
                deadbandType: QtOpcUa.DataChangeFilter.DeadbandType.Absolute
                trigger: QtOpcUa.DataChangeFilter.DataChangeTrigger.StatusOrValue
            }

        }

        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "http://qt-project.org"
                identifier: "s=Demo.Static.Scalar.Double"
            }
            id: node2
            monitored: false
        }

        SignalSpy {
            id: timerSpy
            target: timer
            signalName: "triggered"
        }
    }
}



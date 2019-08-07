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



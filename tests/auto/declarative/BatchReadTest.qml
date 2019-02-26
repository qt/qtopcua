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
        serverDiscovery.discoveryUrl = "opc.tcp://127.0.0.1:43344";
    }

    CompletionLoggingTestCase {
        name: "Reading multiple items"
        when: connection.connected && shouldRun

        SignalSpy {
            id: readNodeAttributesFinishedSpy
            target: connection
            signalName: "readNodeAttributesFinished"
        }

        function test_nodeTest() {
            var readItemList = [];
            var readItem;

            // Item #0
            readItem = QtOpcUa.ReadItem.create();
            readItem.ns = "http://qt-project.org";
            readItem.nodeId = "s=Demo.Static.Scalar.Double";
            readItem.attribute = QtOpcUa.Constants.NodeAttribute.DisplayName;
            readItemList.push(readItem);

            // Item #1
            readItem = QtOpcUa.ReadItem.create();
            readItem.ns = "http://qt-project.org";
            readItem.nodeId = "s=Demo.Static.Scalar.Double";
            // Value is the default attribute to read
            readItemList.push(readItem);

            // Item #2
            readItem = QtOpcUa.ReadItem.create();
            readItem.ns = "http://qt-project.org";
            readItem.nodeId = "s=Demo.Static.Arrays.UInt32";
            // Value is the default attribute to read
            readItem.indexRange = "0:2";
            readItemList.push(readItem);

            verify(connection.readNodeAttributes(readItemList))
            readNodeAttributesFinishedSpy.wait();

            compare(readNodeAttributesFinishedSpy.count, 1);
            compare(readNodeAttributesFinishedSpy.signalArguments[0].length, 1);

            var results = readNodeAttributesFinishedSpy.signalArguments[0][0];
            var now = new Date();

            compare(results.length, readItemList.length);
            for (var i = 0; i < results.length; i++) {
                console.log("Comparing result item #" + i);
                verify(results[i].status.isGood);
                compare(results[i].nodeId, readItemList[i].nodeId);
                compare(results[i].namespaceName, readItemList[i].ns);
                compare(results[i].attribute, readItemList[i].attribute);
                verify((now - results[i].serverTimestamp) < 10000);
            }

            compare(results[0].value.text, "DoubleScalarTest");
            compare(results[1].value, 1.0);
            compare(results[2].value, [1, 2]);
        }
    }
}

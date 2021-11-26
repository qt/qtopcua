/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
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

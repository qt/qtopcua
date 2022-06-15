// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtOpcUa as QtOpcUa
import "qrc:/machine"

Window {
    visible: true
    width: 800
    height: 600
    minimumWidth: 800
    minimumHeight: 600
    title: "OPC UA Tank Example"

    QtOpcUa.Connection {
        id: connection
        backend: serverControl.backend
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

    Machine {
        id: machine
    }

    ColumnLayout {
        anchors.margins: 10
        anchors.fill: parent
        spacing: 45
        ServerControl {
            id: serverControl
            connection: connection
            serverDiscovery: serverDiscovery
        }
        MachineDisplay {
            Layout.fillHeight: true
            machine: machine
            connection: connection
        }
        Text {
            id: messageText
            font.pointSize: 10
          //  FIXME text: uaBackend.message
        }
    }

    Component.onCompleted: {
        serverControl.resetSimulation.connect(machine.resetMethod.callMethod)
    }
}

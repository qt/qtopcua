// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtOpcUa as QtOpcUa

RowLayout {
    readonly property alias backend: backendSelector.currentText
    property QtOpcUa.Connection connection
    property QtOpcUa.ServerDiscovery serverDiscovery
    signal resetSimulation()
    property alias resetButtonText: resetButton.text

    TextField {
        id: uaUrl
        Layout.fillWidth: true
        text: "opc.tcp://127.0.0.1:43344"
    }
    ComboBox {
        id: backendSelector
        model: connection.availableBackends
    }

    Button {
        text: connection.connected ? "Disconnect" : "Connect"
        enabled: connection.availableBackends.length > 0
        onClicked: {
            if (connection.connected)
                connection.disconnectFromEndpoint()
            else
                serverDiscovery.discoveryUrl = uaUrl.text;
        }
    }
    Button {
        id: resetButton
        text: "Reset simulation"
        enabled: connection.connected
        onClicked: resetSimulation()
    }
}

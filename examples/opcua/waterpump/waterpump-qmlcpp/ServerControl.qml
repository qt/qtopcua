// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    TextField {
        id: uaUrl
        Layout.fillWidth: true
        text: "opc.tcp://127.0.0.1:43344"
    }

    ComboBox {
        id: backendSelector
        model: uaBackend.backends
    }

    Button {
        text: uaBackend.connected ? "Disconnect" : "Connect"
        enabled: uaBackend.backends.length > 0
        onClicked: {
            if (uaBackend.connected)
                uaBackend.disconnectFromEndpoint()
            else
                uaBackend.connectToEndpoint(uaUrl.text, backendSelector.currentIndex)
        }
    }

    Button {
        text: "Reset simulation"
        enabled: uaBackend.connected
        onClicked: uaBackend.resetSimulation();
    }
}

// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

Window {
    visible: true
    width: 800
    height: 600
    minimumWidth: 800
    minimumHeight: 600
    title: "OPC UA Tank Example"

    ColumnLayout {
        anchors.margins: 10
        anchors.fill: parent
        spacing: 45
        ServerControl {}
        TankSimulation {
            Layout.fillHeight: true
        }
        Text {
            id: messageText
            font.pointSize: 10
            text: uaBackend.message
        }
    }
}

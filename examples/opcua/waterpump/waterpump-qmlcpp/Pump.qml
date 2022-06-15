// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import OpcUaMachineBackend

Rectangle {
    id: pump
    width: 40
    height: 40
    radius: width / 2
    color: uaBackend.machineState === OpcUaMachineBackend.MachineState.Pumping ? "#00BFFF" : "lightgrey"
    Rectangle {
        antialiasing: true
        width: height / 4
        height: pump.width * 0.75
        anchors.centerIn: parent
        color: "grey"
    }
    Rectangle {
        antialiasing: true
        width: pump.width * 0.75
        height: width / 4
        anchors.centerIn: parent
        color: "grey"
    }
    Connections {
        target: uaBackend
        onPercentFilledTank2Changed: {
            if (uaBackend.machineState === OpcUaMachineBackend.MachineState.Pumping)
                rotation += 15
        }
    }
}

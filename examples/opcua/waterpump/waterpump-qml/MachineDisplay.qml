// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtOpcUa as QtOpcUa
import "qrc:/machine"

RowLayout {
    property Machine machine
    property QtOpcUa.Connection connection

    opacity: connection.connected ? 1.0 : 0.25
    Tank1Unit {
        startButtonEnabled: connection.connected && machine.state === Machine.MachineState.Idle &&
                            machine.tank1.percentFilled > 0 && machine.tank2.percentFilled < machine.tank2.targetPercent
        stopButtonEnabled: connection.connected && machine.state === Machine.MachineState.Pumping
        percentFilled: machine.tank1.percentFilled
        startButtonText: machine.startMethod.displayName.text
        stopButtonText: machine.stopMethod.displayName.text

        id: tank1unit
        Layout.fillHeight: true
        Layout.fillWidth: true

        Component.onCompleted: {
            tank1unit.startPump.connect(machine.startMethod.callMethod)
            tank1unit.stopPump.connect(machine.stopMethod.callMethod)
        }
    }
    Pump {
        machineIsPumping: machine.state === Machine.MachineState.Pumping

        Component.onCompleted: {
            machine.tank2.onPercentFilledChanged.connect(rotatePump)
        }
    }
    Tank2Unit {
        flushButtonEnabled: connection.connected && machine.state === Machine.MachineState.Idle && machine.tank2.percentFilled > machine.tank2.targetPercent
        percentFilled: machine.tank2.percentFilled
        valveState: machine.tank2valveState
        flushButtonText: machine.flushMethod.displayName.text

        Layout.fillHeight: true
        Layout.fillWidth: true

        id: tank2unit

        Component.onCompleted: {
            tank2unit.flushTank.connect(machine.flushMethod.callMethod)
        }
    }
    Slider {
        id: setpointSlider
        Layout.fillHeight: false
        Layout.preferredHeight: tank1unit.tankHeight
        Layout.alignment: Qt.AlignBottom
        enabled: connection.connected && machine.state === Machine.MachineState.Idle
        from: 0
        to: 100
        value: machine.tank2.targetPercent
        live: false
        stepSize: 1.0
        orientation: Qt.Vertical
        onValueChanged: {
            machine.tank2.targetPercent = value;
        }
    }
    ValueDisplay {
        designation: machine.designation
        percentFilledTank1: machine.tank1.percentFilled
        percentFilledTank2: machine.tank2.percentFilled
        targetPercentTank2: machine.tank2.targetPercent
        machineState: machine.state === Machine.MachineState.Idle ?
                          "Idle" : (machine.state === Machine.MachineState.Pumping ? "Pumping" : "Flushing")
        valveState: machine.tank2valveState
    }
}

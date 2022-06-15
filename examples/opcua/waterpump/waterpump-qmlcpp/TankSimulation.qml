// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import OpcUaMachineBackend

RowLayout {
    opacity: uaBackend.connected ? 1.0 : 0.25
    Tank1Unit {
        id: tank1unit
        Layout.fillHeight: true
        Layout.fillWidth: true
    }
    Pump {}
    Tank2Unit {
        Layout.fillHeight: true
        Layout.fillWidth: true
    }
    Slider {
        id: setpointSlider
        Layout.fillHeight: false
        Layout.preferredHeight: tank1unit.tankHeight
        Layout.alignment: Qt.AlignBottom
        enabled: uaBackend.connected && uaBackend.machineState === OpcUaMachineBackend.MachineState.Idle
        from: 0
        to: 100
        value: uaBackend.tank2TargetPercent
        live: false
        stepSize: 1.0
        orientation: Qt.Vertical
        onValueChanged: {
            uaBackend.machineWriteTank2TargetPercent(value);
        }
    }
    ValueDisplay {}
}

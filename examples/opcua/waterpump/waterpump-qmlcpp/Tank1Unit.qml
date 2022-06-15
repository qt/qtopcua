// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import OpcUaMachineBackend

Item {
    property alias tankHeight: tank1.height

    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            Button {
                id: startButton
                text: "Start"
                enabled: uaBackend.connected && uaBackend.machineState === OpcUaMachineBackend.MachineState.Idle &&
                         uaBackend.percentFilledTank1 > 0 && uaBackend.percentFilledTank2 < uaBackend.tank2TargetPercent
                onClicked: uaBackend.startPump()
            }
            Button {
                id: stopButton
                text: "Stop"
                enabled: uaBackend.connected && uaBackend.machineState === OpcUaMachineBackend.MachineState.Pumping
                onClicked: uaBackend.stopPump()
            }
        }
        Tank {
            id: tank1
            Layout.fillHeight: true
            Layout.fillWidth: true
            percentFilled: uaBackend.percentFilledTank1
        }
    }
}

// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import OpcUaMachineBackend

Item {
    ColumnLayout {
        anchors.fill: parent
        Button {
            id: flushButton
            text: "Flush"
            enabled: uaBackend.connected && uaBackend.machineState === OpcUaMachineBackend.MachineState.Idle && uaBackend.percentFilledTank2 > uaBackend.tank2TargetPercent
            onClicked: {
                uaBackend.flushTank2()
            }
        }
        Layout.fillHeight: true
        Tank {
            id: tank2
            Layout.fillHeight: true
            Layout.fillWidth: true
            percentFilled: uaBackend.percentFilledTank2
            Rectangle {
                color: uaBackend.tank2ValveState ? "#00BFFF" : "lightgrey"
                anchors.top: parent.bottom
                anchors.right: parent.right
                width: tank2.width / 10
                height: 40
            }
        }
    }
}

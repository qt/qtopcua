// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Layouts
import OpcUaMachineBackend

ColumnLayout {
    Text {
        font.pointSize: 12
        text: "<b>Machine Status</b>"
    }
    Text {
        font.pointSize: 10
        text: "<b>Designation:</b> " + uaBackend.machineDesignation
    }
    Text {
        font.pointSize: 10
        text: "<b>State:</b> "
              + (uaBackend.machineState === OpcUaMachineBackend.MachineState.Idle
                ? "Idle"
                : (uaBackend.machineState === OpcUaMachineBackend.MachineState.Pumping ? "Pumping"
                                                                                       : "Flushing"))
    }
    Text {
        font.pointSize: 10
        text: "<b>Tank 1:</b> " + uaBackend.percentFilledTank1 + "%"
    }
    Text {
        font.pointSize: 10
        text: "<b>Tank 2:</b> " + uaBackend.percentFilledTank2 + "%"
    }
    Text {
        font.pointSize: 10
        text: "<b>Setpoint:</b> " + uaBackend.tank2TargetPercent + "%"
    }
    Text {
        font.pointSize: 10
        text: "<b>Valve:</b> " + (uaBackend.tank2ValveState === false ? "Closed" : "Open");
    }
}

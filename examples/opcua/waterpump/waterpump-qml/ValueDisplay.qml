// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    property string designation
    property int percentFilledTank1
    property int percentFilledTank2
    property int targetPercentTank2
    property string machineState
    property bool valveState

    Text {
        font.pointSize: 12
        text: "<b>Machine Status</b>"
    }
    Text {
        font.pointSize: 10
        text: "<b>Designation:</b> " + designation
    }
    Text {
        font.pointSize: 10
        text: "<b>State:</b> " + machineState
    }
    Text {
        font.pointSize: 10
        text: "<b>Tank 1:</b> " + percentFilledTank1 + "%"
    }
    Text {
        font.pointSize: 10
        text: "<b>Tank 2:</b> " + percentFilledTank2 + "%"
    }
    Text {
        font.pointSize: 10
        text: "<b>Setpoint:</b> " + targetPercentTank2 + "%"
    }
    Text {
        font.pointSize: 10
        text: "<b>Valve:</b> " + (valveState ? "Open" : "Closed")
    }
}

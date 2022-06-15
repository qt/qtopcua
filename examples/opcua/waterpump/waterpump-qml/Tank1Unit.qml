// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    property alias tankHeight: tank1.height
    property alias startButtonEnabled: startButton.enabled
    property alias stopButtonEnabled: stopButton.enabled
    property alias percentFilled: tank1.percentFilled
    property alias startButtonText: startButton.text
    property alias stopButtonText: stopButton.text
    signal startPump()
    signal stopPump()

    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            Button {
                id: startButton
                text: "Start"
                onClicked: startPump()
            }
            Button {
                id: stopButton
                text: "Stop"
                onClicked: stopPump()
            }
        }
        TankDisplay {
            id: tank1
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}

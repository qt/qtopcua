// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    property alias flushButtonEnabled: flushButton.enabled
    property alias percentFilled: tank2.percentFilled
    property alias flushButtonText: flushButton.text
    property bool valveState
    signal flushTank()

    ColumnLayout {
        anchors.fill: parent
        Button {
            id: flushButton
            text: "Flush"
            onClicked: flushTank()
        }
        Layout.fillHeight: true
        TankDisplay {
            id: tank2
            Layout.fillHeight: true
            Layout.fillWidth: true
            Rectangle {
                color: valveState ? "#00BFFF" : "lightgrey"
                anchors.top: parent.bottom
                anchors.right: parent.right
                width: tank2.width / 10
                height: 40
            }
        }
    }
}

// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick

Rectangle {
    property bool machineIsPumping;

    function rotatePump() {
        if (machineIsPumping)
            rotation += 15
    }

    id: pump
    width: 40
    height: 40
    radius: width / 2
    color: machineIsPumping ? "#00BFFF" : "lightgrey"
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
}

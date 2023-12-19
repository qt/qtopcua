// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick

Rectangle {
    id: root

    property real percentFilled: 0

    color: "lightgrey"
    width: parent.width
    height: parent.height

    Rectangle {
        y: parent.height - root.percentFilled * (parent.height / 100)
        width: parent.width
        height: root.percentFilled * (parent.height / 100)
        color: "#00BFFF"
    }
}

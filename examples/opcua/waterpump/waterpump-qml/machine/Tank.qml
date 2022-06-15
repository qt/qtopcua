// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtOpcUa as QtOpcUa

Item {
    property QtOpcUa.Node tankNode
    readonly property alias percentFilled: filledState.value

    QtOpcUa.ValueNode {
        nodeId: QtOpcUa.RelativeNodeId {
                      startNode: tankNode.nodeId
                      path:  [
                          QtOpcUa.RelativeNodePath {
                              ns: "Demo Namespace"
                              browseName: "PercentFilled"
                          }
                      ]
                }
        id: filledState
    }
}

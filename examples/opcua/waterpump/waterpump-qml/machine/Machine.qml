// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtOpcUa as QtOpcUa

Item {
    readonly property alias tank1: tank1
    readonly property alias tank2: tank2
    readonly property alias state: machineState.value
    readonly property alias tank2PercentFilled: tank2.percentFilled
    readonly property alias tank2valveState: tank2.valveState
    readonly property alias designation: designation.value
    property alias resetMethod: resetMethod
    property alias startMethod: startMethod
    property alias stopMethod: stopMethod
    property alias flushMethod: flushMethod

    QtOpcUa.NodeId {
        identifier: "s=Machine"
        ns: "Demo Namespace"
        id: machineNodeId
    }

    QtOpcUa.MethodNode {
        nodeId: QtOpcUa.NodeId {
            identifier: "s=Machine.Start"
            ns: "Demo Namespace"
        }
        objectNodeId: machineNodeId
        id: startMethod
    }

    QtOpcUa.MethodNode {
        nodeId: QtOpcUa.NodeId {
            identifier: "s=Machine.Stop"
            ns: "Demo Namespace"
        }
        objectNodeId: machineNodeId
        id: stopMethod
    }

    QtOpcUa.MethodNode {
        nodeId: QtOpcUa.NodeId {
            identifier: "s=Machine.FlushTank2"
            ns: "Demo Namespace"
        }
        objectNodeId: machineNodeId
        id: flushMethod
    }

    QtOpcUa.MethodNode {
        nodeId: QtOpcUa.NodeId {
            identifier: "s=Machine.Reset"
            ns: "Demo Namespace"
        }
        objectNodeId: machineNodeId
        id: resetMethod
    }

    enum MachineState {
        Idle = 0,
        Pumping = 1,
        Flushing = 2
    }

    Tank {
        id: tank1
        tankNode: QtOpcUa.Node {
                      nodeId: QtOpcUa.NodeId {
                        identifier: "s=Machine.Tank1"
                        ns: "Demo Namespace"
                      }
        }
    }

    Tank {
        id: tank2
        tankNode: QtOpcUa.Node {
                      nodeId: QtOpcUa.NodeId {
                            identifier: "s="  + tank2.tankName
                            ns: "Demo Namespace"
                      }
        }
        property alias targetPercent: targetPercent.value
        readonly property string tankName: "Machine.Tank2"
        readonly property alias valveState: valveState.value

        QtOpcUa.ValueNode {
            nodeId: QtOpcUa.NodeId {
                identifier: "s=" + tank2.tankName + ".TargetPercent"
                ns: "Demo Namespace"
            }
            id: targetPercent
        }

        QtOpcUa.ValueNode {
            nodeId: QtOpcUa.NodeId {
                identifier: "s=" + tank2.tankName + ".ValveState"
                ns: "Demo Namespace"
            }
            id: valveState
        }
    }

    QtOpcUa.ValueNode {
        nodeId: QtOpcUa.NodeId {
            identifier: "s=Machine.State"
            ns: "Demo Namespace"
        }
        id: machineState
    }

    QtOpcUa.ValueNode {
        nodeId: QtOpcUa.NodeId {
            identifier: "s=Machine.Designation"
            ns: "Demo Namespace"
        }
        id: designation
    }
}


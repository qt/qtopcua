/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the QtOpcUa module.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

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


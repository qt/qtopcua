/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the QtOpcUa module.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
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

import QtQuick 2.0

Rectangle {
    id: nodeRoot

    x: parent.width/2 - width/2
    y: parent.height/2 - height/2
    z: 1
    radius: 2

    width: Math.max(200, nodeName.contentWidth + 10)
    height: contentColumn.childrenRect.height

    signal openNode(var node)
    signal closeNode(var node)
    signal raiseMe(var node)

    property var node

    property string uniqueId: ""
    property string name: "-"
    property int childCount: -1

    property var childNodes: ([])
    property var childNodeQmlItems: ([])
    property var childOffsets: ({})

    property bool focused: nodeRoot.z > 1

    property string cachedNodeType

    color: focused ? "lightblue" : "aliceblue"

    Rectangle {
        id: shadow
        visible: nodeRoot.focused
        radius: 2

        width: nodeRoot.width
        height: nodeRoot.height

        x: 6
        y: 6
        z: -100

        opacity: 0.2
        color: "black"
    }

    MouseArea {
        id: mainMouseArea
        anchors.fill: parent
        drag.target: parent
        onPressed: nodeRoot.raiseMe(nodeRoot)
    }

    border {
        width: 1
    }

    Component.onDestruction: {
        console.log("QML node going away");
    }

    onNodeChanged: {
        console.log("Node changed:", node)
        if (node) {
            cachedNodeType = nodeRoot.node.type;
            console.log("Node set, updating data points.");
            name = node.name;
            nodeRoot.childNodes = node.childIds;
            childCount = node.childCount

        }
    }

    Column {
        id: contentColumn
        NodeText {
            id: nodeName
            text: "[" + nodeRoot.name + "]"
            horizontalAlignment: Text.AlignHCenter
            width: nodeRoot.width
        }

        Rectangle {
            border.width: 1
            width: nodeRoot.width
            height: nodeType.contentHeight + 4
            visible: nodeType.type
            NodeText {
                id: nodeType
                anchors.centerIn: parent
                property var type: nodeRoot.node ? nodeRoot.node.type : "<no node>"
                text: "Type: " + type.replace("Type", "").trim();
                width: parent.width - 4
                MouseArea {
                    anchors.fill: parent
                    drag.target: nodeRoot
                }
            }
        }

        Rectangle {
            border.width: 1
            height: nodeClass.contentHeight + 4
            width: nodeRoot.width
            NodeText {
                id: nodeClass
                property var value: nodeRoot.node ? nodeRoot.node.nodeClass : "<null node>"
                text: "Class: " + value
                width: parent.width - 4
                anchors.centerIn: parent
                MouseArea {
                    anchors.fill: parent
                    drag.target: nodeRoot
                }
            }
        }

        Rectangle {
            id: valueField
            visible: nodeValueImage.isDefined() === true || nodeValueText.isDefined() === true
            border.width: 1
            height: nodeValueImage.visible ? nodeValueImage.height + 4 : nodeValueText.contentHeight + 4
            width: nodeRoot.width

            Image {
                id: nodeValueImage
                visible: isDefined() === true
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                property var value
                width: nodeRoot.width
                onValueChanged: {
                    if (value !== undefined && nodeRoot.cachedNodeType === "ImagePNG")
                        source = "data:image/png;base64," + value
                }

                function getValue() {
                    if (nodeRoot.node) {
                        value = nodeRoot.node.encodedValue();
                    }
                }

                function isDefined() {
                    return value !== undefined && nodeRoot.cachedNodeType === "ImagePNG"
                }

                Component.onCompleted: {
                    getValue();
                }

                MouseArea {
                    anchors.fill: parent
                    drag.target: nodeRoot
                    onClicked: {
                        console.log("Refresh value");
                        nodeValueImage.getValue();
                    }
                }
            }

            NodeText {
                id: nodeValueText
                property var value
                width: parent.width - 4
                anchors.centerIn: parent
                visible: isDefined() === true
                function getValue() {
                    if (nodeRoot.node) {
                        value = nodeRoot.node.value
                    }
                }

                function isDefined() {
                    return value !== undefined && nodeRoot.cachedNodeType !== "ImagePNG"
                }

                onValueChanged: {
                    if (typeof(value) === "number" && (value % 1 !== 0)) {
                        text = "Value: " + value.toFixed(2);
                    } else if (value !== undefined){
                        text = "Value: " + value
                    } else {
                        text = "Value: <none>"
                    }
                }

                Component.onCompleted: {
                    getValue()
                }

                MouseArea {
                    anchors.fill: parent
                    drag.target: nodeRoot
                    onClicked: {
                        console.log("Refresh value");
                        nodeValueText.getValue();
                        nodeValueImage.getValue();
                    }
                }
            }
        }

        Repeater {
            id: childRpt
            model: nodeRoot.childCount

            Rectangle {
                id: cn
                property var childNode: client.node(nodeRoot.childNodes[index])
                width: nodeRoot.width
                height: childName.contentHeight + 4
                border.width: 1

                property bool expanded: false
                property color baseColor: expanded ? "lightgrey" : "white"

                color: baseColor

                Connections {
                    target: nodeRoot
                    onVisibleChanged: {
                        if (expanded && !nodeRoot.visible) {
                            cn.expanded = false
                            nodeRoot.closeNode(childNode)
                        }
                    }
                }

                MouseArea {
                    id: ma
                    anchors.fill: parent
                    drag.target: nodeRoot
                    acceptedButtons: Qt.RightButton | Qt.LeftButton
                    onClicked: {
                        if (!nodeRoot.focused) {
                            nodeRoot.raiseMe(nodeRoot);
                            return;
                        }

                        if (mouse.button === Qt.LeftButton) {
                            expanded = true;
                            nodeRoot.openNode(childNode);
                        } else if (mouse.button === Qt.RightButton) {
                            console.log("Closing child:", childNode);
                            expanded = false
                            nodeRoot.closeNode(childNode);
                        }
                    }
                }

                NodeText {
                    id: childName
                    property string childNameStr: childNode ? childNode.name : "<no node>"
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left:parent.left
                        right: countText.left
                    }

                    text: " " + childNameStr
                }
                NodeText {
                    id: countText
                    property int childCount: childNode ? childNode.childCount : 0
                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                        margins: 2
                    }
                    text: "(" + childCount + ")"
                }

                onYChanged: {
                    if (childNode) {
                        nodeRoot.childOffsets[childNode.xmlNodeId] = y + height/2
                    }
                }
            }
        }
    }
}

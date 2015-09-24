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

import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0

import org.qtproject.opcua 1.0

Rectangle {
    id: browserRoot
    anchors.fill: parent
    color: "white"

    property var idCache: ({})
    property var lineCache: ([])
    property var lastRaised
    property var client

    function focus(node){
        if (lastRaised)
            lastRaised.z = 1;
        node.z = 100;
        node.visible = true
        browserRoot.lastRaised = node;
    }

    function createNode(node, parent) {
        var xmlNodeId = node.xmlNodeId;

        console.log("Creating node:", node, node.name, xmlNodeId)

        if (idCache.hasOwnProperty(xmlNodeId)) {
            console.log("Already open, raising.");
            focus(idCache[xmlNodeId]);
            return
        }
        var component = Qt.createComponent("OPCNode.qml");
        var n = component.createObject(browserRoot, {x: 0, y: 0, uniqueId: xmlNodeId, node: node});
        if (parent) {
            console.log("Parent:", parent);
            if (idCache.hasOwnProperty(parent)) {
                console.log("Found parent:", parent)
                var p = idCache[parent];
                console.log("Arranging child relative to parent")
                n.x = p.x + p.width + 5
                n.y = p.y + p.childOffsets[xmlNodeId]
                var lineComp = Qt.createComponent("Line.qml");
                var l = lineComp.createObject(browserRoot, {from: p, to: n, highlight: Qt.binding(function(){return n.focused})});
                lineCache.push(l);
                p.childNodeQmlItems.push(n)
            }
        }
        if (n === null) {
            console.log("Error creating node");
        } else {
            idCache[xmlNodeId] = n;
            n.openNode.connect(function(childNode){
                var parentId = xmlNodeId;
                createNode(childNode, parentId);
            });
            n.closeNode.connect(function(childNode){
                if (idCache.hasOwnProperty(childNode.xmlNodeId)) {
                    idCache[childNode.xmlNodeId].visible = false;
                }
            });
            n.raiseMe.connect(focus);
            focus(n)
        }
    }

    Component.onCompleted: {
        // FIXME: add a QML specific initializer to handle this stuff?
        // Check, how other plugins do that...
        if (!client) {
            client = opcua.createClient();
        }

        // endpoint is set by the C++ runner
        client.url = endpoint
        if (autoconnect) {
            client.connectToEndpoint();
        }
    }

    QOpcUaProvider {
        id: opcua
    }

    Connections {
        ignoreUnknownSignals: true
        target: client
        onConnectedChanged: {
            if (connected) {
                console.log("Connected, fetching root node");
                var root = client.node("ns=0;i=84")
                createNode(root);
            } else {
                console.log("Disconnected, clearing cache.");
                for (var x1 in browserRoot.lineCache) {
                    lineCache[x1].destroy()
                }
                for (var x2 in browserRoot.idCache) {
                    idCache[x2].destroy();
                }
                browserRoot.lineCache = [];
                browserRoot.idCache = {}
            }
        }
    }

    RowLayout {
        width: parent.width

        anchors {
            bottom:  parent.bottom
        }

        spacing: 6


        Text {
            text: "Endpoint:"
        }

        TextField {
            id: endpointField
            text: client.url

            Layout.fillWidth: true
            onAccepted: {
                if (client.connected) {
                    client.disconnectFromEndpoint();
                }
                client.connectToEndpoint(endpointField.text);
            }
        }

        Button {
            id: connectButton
            text: client.connected ? "Disconnect" : "Connect"
            onClicked: {
                if (client.connected) {
                    client.disconnectFromEndpoint();
                } else {
                    console.log("Connecting to", endpointField.text)
                    client.connectToEndpoint(endpointField.text);
                }
            }
        }
    }
}

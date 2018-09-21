/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.3
import QtTest 1.0
import QtOpcUa 5.12 as QtOpcUa

Item {

    QtOpcUa.Connection {
        id: connection
        backend: connection.availableBackends[0]
        defaultConnection: true
    }

    Component.onCompleted: {
        connection.connectToEndpoint("opc.tcp://127.0.0.1:43344");
    }

    TestCase {
        name: "Standard attributes on method node"
        when: node1.readyToUse

        SignalSpy {
            id: node1BrowseNameSpy
            target: node1
            signalName: "browseNameChanged"
        }

        SignalSpy {
            id: node1NodeClassSpy
            target: node1
            signalName: "nodeClassChanged"
        }

        SignalSpy {
            id: node1DisplayNameSpy
            target: node1
            signalName: "displayNameChanged"
        }

        SignalSpy {
            id: node1DescriptionSpy
            target: node1
            signalName: "descriptionChanged"
        }

        SignalSpy {
            id: node1ObjectNodeIdSpy
            target: node1
            signalName: "objectNodeIdChanged"
        }

        function test_nodeTest() {
            compare(node1.browseName, "multiplyArguments");
            compare(node1.nodeClass, QtOpcUa.Constants.NodeClass.Method);
            compare(node1.displayName.text, "ns=3;s=Test.Method.Multiply");
            compare(node1.description.text, "MultiplyDoubles");
            compare(node1.objectNodeId.ns, "Test Namespace");
            compare(node1.objectNodeId.identifier, "s=TestFolder");

            compare(node1BrowseNameSpy.count, 1)
            compare(node1NodeClassSpy.count, 1)
            compare(node1DisplayNameSpy.count, 1)
            compare(node1DescriptionSpy.count, 1)
            compare(node1ObjectNodeIdSpy.count, 0)

            node1.objectNodeId.identifier = "s=foo";
            node1ObjectNodeIdSpy.wait()
            compare(node1ObjectNodeIdSpy.count, 1);
        }

        QtOpcUa.MethodNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "Test Namespace"
                identifier: "s=Test.Method.Multiply"
            }
            objectNodeId: QtOpcUa.NodeId {
                                ns: "Test Namespace"
                                identifier: "s=TestFolder"
                        }
            id: node1
        }
    }
}

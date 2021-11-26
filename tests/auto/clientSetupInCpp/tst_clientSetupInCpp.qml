/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.3
import QtTest 1.0
import QtOpcUa 5.13 as QtOpcUa
import App 1.0

Item {
    QtOpcUa.Connection {
        id: connection
        connection: myclass.connection
    }

    MyClass {
        id: myclass
    }

    Component.onCompleted: myclass.startConnection()

    TestCase {
        name: "Create String Node Id"
        when: node1.readyToUse

        function test_nodeTest() {
            compare(node1.value, "Value", "");
            compare(node1.browseName, "theStringId");
            compare(node1.nodeClass, QtOpcUa.Constants.NodeClass.Variable);
            compare(node1.displayName.text, "theStringId");
            compare(node1.description.text, "Description for ns=3;s=theStringId");
            compare(node1.status, QtOpcUa.Node.Status.Valid);
            tryCompare(node1, "monitored", true);
            compare(node1.publishingInterval, 100.0);

            compare(connection.currentEndpoint.endpointUrl, "opc.tcp://127.0.0.1:43344");
            compare(connection.currentEndpoint.securityPolicy, "http://opcfoundation.org/UA/SecurityPolicy#None");
            compare(connection.currentEndpoint.server.applicationUri, "urn:open62541.server.application");
        }

        QtOpcUa.ValueNode {
            connection: connection
            nodeId: QtOpcUa.NodeId {
                ns: "Test Namespace"
                identifier: "s=theStringId"
            }
            id: node1
        }
    }
}

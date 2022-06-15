// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick
import QtTest
import QtOpcUa as QtOpcUa

Item {
    TestCase {
        name: "Enum exports to QML"

        function test_enumExports() {
            compare(QtOpcUa.Constants.NodeClass.Method, 4);
            compare(QtOpcUa.Constants.NodeAttribute.DisplayName, 8);
            compare(QtOpcUa.Node.Status.Valid, 0);
            compare(QtOpcUa.Status.Good, 0);
            compare(QtOpcUa.Status.BadUnexpectedError, 0x8001);
            compare(QtOpcUa.Constants.Double, 3);
            compare(QtOpcUa.Constants.Certificate, 2);
            compare(QtOpcUa.DataChangeFilter.DeadbandType.Absolute, 1);
            compare(QtOpcUa.DataChangeFilter.DataChangeTrigger.StatusOrValueOrTimestamp, 2);

            // Test return value of undefined node
            compare(node1.nodeClass, QtOpcUa.Constants.NodeClass.Undefined);
        }

        QtOpcUa.Node {
            id: node1
        }
    }
}

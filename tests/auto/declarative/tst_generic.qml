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
import QtOpcUa 5.13 as QtOpcUa

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

            // Test return value of undefined node
            compare(node1.nodeClass, QtOpcUa.Constants.NodeClass.Undefined);
        }

        QtOpcUa.Node {
            id: node1
        }
    }
}

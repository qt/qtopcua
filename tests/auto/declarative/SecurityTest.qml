/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
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
    property string backendName
    property int completedTestCases: 0
    property int availableTestCases: 0
    property bool completed: completedTestCases == availableTestCases
    property bool shouldRun: false

    onShouldRunChanged: {
        if (shouldRun)
            console.log("Running", parent.testName, "with", backendName);
    }

    CompletionLoggingTestCase {
        name: parent.parent.testName + ": " + backendName + ": Check supported security features"
        when: shouldRun

        QtOpcUa.Connection {
            id: connection2
            backend: backendName
        }

        function test_nodeTest() {
            verify(connection2.supportedSecurityPolicies.length > 0);
            if (backendName == "uacpp") {
                compare(connection2.supportedSecurityPolicies.length, 6);
                compare(connection2.supportedUserTokenTypes.length, 3);
            } else if (backendName === "open62541") {
                if (SERVER_SUPPORTS_SECURITY)
                    compare(connection2.supportedSecurityPolicies.length, 4);
                else
                    compare(connection2.supportedSecurityPolicies.length, 1);
                compare(connection2.supportedUserTokenTypes.length, 2);
            } else {
                fail(backendName, "is not support by this test case");
            }
        }
    }
}

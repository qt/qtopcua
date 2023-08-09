// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick
import QtTest
import QtOpcUa as QtOpcUa

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
                    compare(connection2.supportedSecurityPolicies.length,
                            connection2.supportedSecurityPolicies.includes("http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15")
                            ? 5 : 3);
                else
                    compare(connection2.supportedSecurityPolicies.length, 1);
                compare(connection2.supportedUserTokenTypes.length, 2);
            } else {
                fail(backendName, "is not support by this test case");
            }
        }
    }
}

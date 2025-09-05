// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtTest

TestCase {
    onCompletedChanged: parent.completedTestCases += (completed ? 1 : -1)
    objectName: "TestCase"

    function initTestCase() {
        if (SKIP_TESTS)
            skip("The tests fails on macOS 26 in CI: QTBUG-139354");
    }
}

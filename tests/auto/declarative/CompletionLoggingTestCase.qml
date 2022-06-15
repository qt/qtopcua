// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtTest

TestCase {
    onCompletedChanged: parent.completedTestCases += (completed ? 1 : -1)
    objectName: "TestCase"
}

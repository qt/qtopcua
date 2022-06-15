// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick
import QtOpcUa as QtOpcUa

Item {
    property int currentTest: 0
    property string testName

    QtOpcUa.Connection {
        id: connection
    }

    Component.onCompleted: {
        var component = Qt.createComponent(testName + ".qml")
        if (component.status != Component.Ready) {
            console.log("Failed to load component " + testName, component.errorString());
            return;
        }

        for (var backendIndex in connection.availableBackends) {
            var backend = connection.availableBackends[backendIndex];
            console.log("Setting up", testName, "for", backend);
            var child = component.createObject(this, { "backendName": backend });
            if (child == null) {
                console.log("Error creating object", testName);
                return;
            }
            child.completedChanged.connect(incrementTest);
            children.push(child);
        }
        children[currentTest].shouldRun = true;
    }

    function incrementTest() {
        currentTest += 1;
        if (currentTest < children.length)
            children[currentTest].shouldRun = true;
    }
}


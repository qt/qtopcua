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
import QtOpcUa 5.13 as QtOpcUa

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


/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the QtOpcUa module.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0

Rectangle {
    id: l

    property var from
    property var to

    property real x1: from.x + from.width
    property real y1: from.y + from.childOffsets[to.uniqueId]

    property real x2: to.x + to.width / 2
    property real y2: to.y

    property bool highlight: false

    x: x1
    y: y1
    z: Math.max(from.z, to.z) - 1

    visible: to.visible

    color: highlight ? "darkolivegreen": "black"
    height: 2
    antialiasing: true

    transformOrigin: Item.TopLeft;

    width: getWidth(x1, y1, x2, y2);
    rotation: getSlope(x1, y1, x2, y2);

    function getWidth(sx1, sy1, sx2, sy2)
    {
        var w = Math.sqrt(Math.pow((sx2 - sx1), 2) + Math.pow((sy2 - sy1), 2));
        return w;
    }

    function getSlope(sx1, sy1, sx2, sy2)
    {
        var a, m, d;
        if (sx1 === sx2) {
            if (sy1 > sy2) {
                return -90;
            } else {
                return 90;
            }
        }
        var b = sx2 - sx1;
        a = sy2 - sy1;
        m = a / b;
        d = Math.atan(m) * 180 / Math.PI;

        if (a < 0 && b < 0)
            return d + 180;
        else if (a >= 0 && b >= 0)
            return d;
        else if (a < 0 && b >= 0)
            return d;
        else if (a >= 0 && b < 0)
            return d + 180;
        else
            return 0;
    }
}

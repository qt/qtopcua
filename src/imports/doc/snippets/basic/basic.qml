/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
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

import QtQuick 2.3
import QtOpcUa 5.13 as QtOpcUa

Item {
    //! [Basic discovery]
    QtOpcUa.Connection {
        id: connection
        backend: availableBackends[0]
        defaultConnection: true
    }

    QtOpcUa.ServerDiscovery {
        discoveryUrl: "opc.tcp://127.0.0.1:43344"
        onServersChanged: {
            if (status.isGood) {
                if (status.status == QtOpcUa.Status.GoodCompletesAsynchronusly)
                    return; // wait until finished
                if (count > 0) {
                    // choose right server
                    endpointDiscovery.serverUrl = at(0).discoveryUrls[0];
                    console.log("Using server URL:", endpointDiscovery.serverUrl);
                } else {
                    // no servers retrieved
                }
            } else {
                // Fetching servers failed
                console.log("Error fetching server:", servers.status.status);
            }
        }
    }

    QtOpcUa.EndpointDiscovery {
        id: endpointDiscovery
        onEndpointsChanged: {
            if (status.isGood) {
                if (status.status == QtOpcUa.Status.GoodCompletesAsynchronusly)
                    return; // wait until finished
                if (count > 0) {
                    // choose right endpoint
                    console.log("Using endpoint", at(0).endpointUrl, at(0).securityPolicy);
                    connection.connectToEndpoint(at(0));
                } else {
                    // no endpoints retrieved
                }
            } else {
                // Fetching endpoints failed
                console.log("Error fetching endpoints:", status.status);
            }
        }
    }
    //! [Basic discovery]
}

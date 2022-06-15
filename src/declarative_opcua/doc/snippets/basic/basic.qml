// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtOpcUa as QtOpcUa

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

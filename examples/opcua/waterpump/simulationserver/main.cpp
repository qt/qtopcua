// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "simulationserver.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    DemoServer server;
    if (!server.init()) {
        qCritical() << "Could not initialize server.";
        return EXIT_FAILURE;
    }

    server.launch();

    return app.exec();
}

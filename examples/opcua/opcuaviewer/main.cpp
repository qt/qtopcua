// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationVersion(QLatin1String(QT_VERSION_STR));
    QCoreApplication::setApplicationName(QLatin1String("Qt OpcUa Viewer"));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(QLatin1String("url"), QLatin1String("The url to open."));
    parser.process(app);

    const auto positionalArguments = parser.positionalArguments();
    const auto initialUrl = positionalArguments.value(0, QLatin1String("opc.tcp://localhost:48010"));
    MainWindow mainWindow(initialUrl.trimmed());
    mainWindow.setWindowTitle(QLatin1String("Qt OPC UA viewer"));
    mainWindow.show();
    return QCoreApplication::exec();
}

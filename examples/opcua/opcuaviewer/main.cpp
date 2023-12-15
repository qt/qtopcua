// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

using namespace Qt::Literals::StringLiterals;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationVersion(QLatin1StringView(QT_VERSION_STR));
    QCoreApplication::setApplicationName("Qt OpcUa Viewer"_L1);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("url"_L1, "The url to open."_L1);
    parser.process(app);

    const auto positionalArguments = parser.positionalArguments();
    const auto initialUrl = positionalArguments.value(0, "opc.tcp://localhost:48010"_L1);
    MainWindow mainWindow(initialUrl.trimmed());
    mainWindow.setWindowTitle("Qt OPC UA viewer"_L1);
    mainWindow.show();
    return QCoreApplication::exec();
}

// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QDebug>
#include <QFile>
#include <QGuiApplication>
#include <QProcess>
#include <QQmlApplicationEngine>
#include <QString>
#include <QStringLiteral>
#include <QUrl>

using namespace Qt::StringLiterals;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QString serverExePath;
#ifdef Q_OS_WIN
    #ifdef EXAMPLES_CMAKE_SPECIFIC_PATH
        serverExePath = app.applicationDirPath()
                            .append("/../simulationserver/simulationserver.exe");
    #elif QT_DEBUG
        serverExePath = app.applicationDirPath()
                            .append("/../../simulationserver/debug/simulationserver.exe");
    #else
        serverExePath = app.applicationDirPath()
                            .append("/../../simulationserver/release/simulationserver.exe");
    #endif
#elif defined(Q_OS_MACOS)
    serverExePath = app.applicationDirPath()
                        .append("/../../../../simulationserver/simulationserver"
                                    ".app/Contents/MacOS/simulationserver");
#else
    serverExePath = app.applicationDirPath().append("/../simulationserver/simulationserver");
#endif

    if (!QFile::exists(serverExePath)) {
        qWarning() << "Could not find server executable:" << serverExePath;
        return EXIT_FAILURE;
    }

    QProcess serverProcess;

    serverProcess.start(serverExePath);
    if (!serverProcess.waitForStarted()) {
        qWarning() << "Could not start server:" << serverProcess.errorString();
        return EXIT_FAILURE;
    }

    QQmlApplicationEngine engine;
    engine.load(QUrl(u"qrc:/main.qml"_s));
    if (engine.rootObjects().isEmpty())
        return EXIT_FAILURE;

    const int exitCode = app.exec();
    if (serverProcess.state() == QProcess::Running) {
#ifndef Q_OS_WIN
        serverProcess.terminate();
#else
        serverProcess.kill();
#endif
        serverProcess.waitForFinished();
    }
    return exitCode;
}

// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFile>
#include <QDebug>
#include <QProcess>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QString serverExePath;
#ifdef Q_OS_WIN
    #ifdef QT_DEBUG
        serverExePath = app.applicationDirPath().append("/../../../../../../tests/open62541-testserver/debug/open62541-testserver.exe");
    #else
        serverExePath = app.applicationDirPath().append("/../../../../../../tests/open62541-testserver/release/open62541-testserver.exe");
    #endif
#elif defined(Q_OS_MACOS)
    serverExePath = app.applicationDirPath().append("/../../../../../../../../tests/open62541-testserver/open62541-testserver.app/Contents/MacOS/open62541-testserver");
#else
    serverExePath = app.applicationDirPath().append("/../../../../../tests/open62541-testserver/open62541-testserver");
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
    engine.load(QUrl(QStringLiteral("qrc:/basic.qml")));
    if (engine.rootObjects().isEmpty())
        return EXIT_FAILURE;

    return app.exec();
}

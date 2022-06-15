// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "backend_environment.h"
#include <QtQuickTest/quicktest.h>
#include <QObject>
#include <QProcess>
#include <QQmlContext>
#include <QQmlEngine>
#include <QTcpServer>
#include <QTcpSocket>

static QString envOrDefault(const char *env, QString def)
{
    return qEnvironmentVariableIsSet(env) ? qgetenv(env).constData() : def;
}

class SetupClass : public QObject
{
    Q_OBJECT
public:
    SetupClass() {
    };
    ~SetupClass() {
    }

public slots:
    void applicationAvailable() {
        updateEnvironment();
        const quint16 defaultPort = 43344;
        const QHostAddress defaultHost(QHostAddress::LocalHost);

        const QString host = envOrDefault("OPCUA_HOST", defaultHost.toString());
        const QString port = envOrDefault("OPCUA_PORT", QString::number(defaultPort));
        m_opcuaDiscoveryUrl = QString::fromLatin1("opc.tcp://%1:%2").arg(host).arg(port);

        if (qEnvironmentVariableIsEmpty("OPCUA_HOST") && qEnvironmentVariableIsEmpty("OPCUA_PORT")) {
            m_testServerPath = qApp->applicationDirPath()

#if defined(Q_OS_MACOS)
                                         + QLatin1String("/../../open62541-testserver/open62541-testserver.app/Contents/MacOS/open62541-testserver")
#else

#if defined(Q_OS_WIN) && !defined(TESTS_CMAKE_SPECIFIC_PATH)
                                         + QLatin1String("/..")
#endif
                                         + QLatin1String("/../../open62541-testserver/open62541-testserver")
#ifdef Q_OS_WIN
                                         + QLatin1String(".exe")
#endif

#endif
                    ;
            qDebug() << "Server Path:" << m_testServerPath;
            if (!QFile::exists(m_testServerPath)) {
                qFatal("all auto tests rely on an open62541-based test-server");
            }

            // In this case the test is supposed to open its own server.
            // Unfortunately there is no way to check if the server has started up successfully
            // because of missing error handling.
            // This checks will detect other servers blocking the port.

            // Check for running server
            QTcpSocket socket;
            socket.connectToHost(defaultHost, defaultPort);
            QVERIFY2(socket.waitForConnected(1500) == false, "Server is already running");

            // Check for running server which does not respond
            QTcpServer server;
            QVERIFY2(server.listen(defaultHost, defaultPort) == true, "Port is occupied by another process. Check for defunct server.");
            server.close();

            qDebug() << "Starting test server";
            //m_serverProcess.setProcessChannelMode(QProcess::ForwardedChannels);
            m_serverProcess.start(m_testServerPath);
            QVERIFY2(m_serverProcess.waitForStarted(), qPrintable(m_serverProcess.errorString()));
            // Let the server come up
            QTest::qSleep(2000);
        }
    }
    void qmlEngineAvailable(QQmlEngine *engine) {
        bool value = false;
#ifdef SERVER_SUPPORTS_SECURITY
        value = true;
#endif
        engine->rootContext()->setContextProperty("SERVER_SUPPORTS_SECURITY", value);
        engine->rootContext()->setContextProperty("OPCUA_DISCOVERY_URL", m_opcuaDiscoveryUrl);
    }
    void cleanupTestCase() {
        if (m_serverProcess.state() == QProcess::Running) {
            m_serverProcess.kill();
            m_serverProcess.waitForFinished(2000);
        }
    }
private:
    QProcess m_serverProcess;
    QString m_testServerPath;
    QString m_opcuaDiscoveryUrl;
};

QUICK_TEST_MAIN_WITH_SETUP(opcua, SetupClass)

#include "tst_opcua.moc"

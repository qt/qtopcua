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


#include <QtQuick/QQuickView>

#include <QtQml/QQmlContext>

#include <QtGui/QGuiApplication>

#include <QtOpcUa/QOpcUaClient>
#include <QtOpcUa/QOpcUaNode>
#include <QtOpcUa/QOpcUaProvider>

#include <QtCore/QDebug>
#include <QtCore/QCommandLineParser>

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);
    QGuiApplication::setApplicationName("QML OPC UA Browser");
    QGuiApplication::setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("A graphical OPC UA browser based on QOpcClient and QML.");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption endpointOption(QStringList() << "e" << "endpoint",
            QCoreApplication::translate("main", "OPC UA endpoint to connect to."),
                                      "endpoint");
    parser.addOption(endpointOption);

    QCommandLineOption connectOption(QStringList() << "c" << "connect",
            QCoreApplication::translate("main", "Connect on startup, requires endpoint."));
    parser.addOption(connectOption);

    parser.process(a);

    qmlRegisterType<QOpcUaProvider>("org.qtproject.opcua",1,0,"QOpcUaProvider");
    qmlRegisterUncreatableType<QOpcUaClient>("org.qtproject.opcua",1,0,"QOpcUaClient", "");
    qmlRegisterUncreatableType<QOpcUaNode>("org.qtproject.opcua",1,0,"QOpcUaNode","");

    QQuickView view;
    QString endpoint = "opc.tcp://localhost:43344";

    bool autoconnect = parser.isSet(connectOption);
    if (parser.isSet(endpointOption)) {
        endpoint = parser.value(endpointOption);
    }

    view.rootContext()->setContextProperty("endpoint", endpoint);
    view.rootContext()->setContextProperty("autoconnect", QVariant(autoconnect));

    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setWidth(800);
    view.setHeight(600);
    view.setSource(QUrl("qrc:/qml/main.qml"));
    view.show();
    return a.exec();
}

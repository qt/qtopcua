/****************************************************************************
**
** Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QtOpcUa>

class OpcuaConnector
{
public:
    OpcuaConnector(QOpcUaClient *client, const QOpcUaEndpointDescription &endPoint)
        : opcuaClient(client)
    {
        QVERIFY(opcuaClient != nullptr);
        QSignalSpy connectedSpy(opcuaClient, &QOpcUaClient::connected);
        QSignalSpy disconnectedSpy(opcuaClient, &QOpcUaClient::disconnected);
        QSignalSpy stateSpy(opcuaClient, &QOpcUaClient::stateChanged);

        QTest::qWait(500);
        opcuaClient->connectToEndpoint(endPoint);
        QTRY_VERIFY2(opcuaClient->state() == QOpcUaClient::Connected, "Could not connect to server");

        QCOMPARE(connectedSpy.count(), 1); // one connected signal fired
        QCOMPARE(disconnectedSpy.count(), 0); // zero disconnected signals fired
        QCOMPARE(stateSpy.count(), 2);

        QCOMPARE(stateSpy.at(0).at(0).value<QOpcUaClient::ClientState>(),
                 QOpcUaClient::ClientState::Connecting);
        QCOMPARE(stateSpy.at(1).at(0).value<QOpcUaClient::ClientState>(),
                 QOpcUaClient::ClientState::Connected);

        stateSpy.clear();
        connectedSpy.clear();
        disconnectedSpy.clear();

        QVERIFY(opcuaClient->endpoint() == endPoint);
    }

    ~OpcuaConnector()
    {
        QSignalSpy connectedSpy(opcuaClient, &QOpcUaClient::connected);
        QSignalSpy disconnectedSpy(opcuaClient, &QOpcUaClient::disconnected);
        QSignalSpy stateSpy(opcuaClient, &QOpcUaClient::stateChanged);
        QSignalSpy errorSpy(opcuaClient, &QOpcUaClient::errorChanged);

        QVERIFY(opcuaClient != nullptr);
        if (opcuaClient->state() == QOpcUaClient::Connected) {

            opcuaClient->disconnectFromEndpoint();

            QTRY_VERIFY(opcuaClient->state() == QOpcUaClient::Disconnected);

            QCOMPARE(connectedSpy.count(), 0);
            QCOMPARE(disconnectedSpy.count(), 1);
            QCOMPARE(stateSpy.count(), 2);
            QCOMPARE(stateSpy.at(0).at(0).value<QOpcUaClient::ClientState>(),
                     QOpcUaClient::ClientState::Closing);
            QCOMPARE(stateSpy.at(1).at(0).value<QOpcUaClient::ClientState>(),
                     QOpcUaClient::ClientState::Disconnected);
            QCOMPARE(errorSpy.size(), 0);
        }

        opcuaClient = nullptr;
    }

    QOpcUaClient *opcuaClient;
};

class EventsubscriptionTest : public QObject
{
    Q_OBJECT

public:
    EventsubscriptionTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void eventSubscription_data();
    void eventSubscription();

private:
    QVector<QOpcUaClient *> m_clients;
    QString m_serverUrl;
    QOpcUaEndpointDescription m_endpoint;
};

EventsubscriptionTest::EventsubscriptionTest()
{
    m_serverUrl = qEnvironmentVariable("TESTSERVER_URL",
            QStringLiteral("opc.tcp://localhost:4840"));
}

void EventsubscriptionTest::initTestCase()
{
    QOpcUaProvider provider;
    const QStringList backends = provider.availableBackends();

    for (auto it : backends) {
        QOpcUaClient *temp = provider.createClient(it);
        if (temp)
            m_clients.append(temp);
        else
            QFAIL(QStringLiteral("Failed to create client for backend %1").arg(it).toUtf8().constData());
    }

    QOpcUaClient *client = m_clients.first();
    if (client) {
        QSignalSpy endpointSpy(m_clients.first(), &QOpcUaClient::endpointsRequestFinished);

        client->requestEndpoints(m_serverUrl);
        endpointSpy.wait();
        QCOMPARE(endpointSpy.size(), 1);
        QCOMPARE(endpointSpy.at(0).at(2).value<QUrl>(), m_serverUrl);

        const QVector<QOpcUaEndpointDescription> desc = endpointSpy.at(0).at(0).value<QVector<QOpcUaEndpointDescription>>();
        QVERIFY(desc.size() > 0);

        m_endpoint = desc.first();
    }
}

void EventsubscriptionTest::cleanupTestCase()
{
    qDeleteAll(m_clients);
    m_clients.clear();
}

void EventsubscriptionTest::eventSubscription_data()
{
    QTest::addColumn<QOpcUaClient *>("client");
    for (auto it : m_clients)
        QTest::newRow(it->backend().toLatin1().constData()) << it;
}

/*
    This manual test requires the open62541 tutorial_server_events.c example
    for the server side
*/
void EventsubscriptionTest::eventSubscription()
{
    QFETCH(QOpcUaClient *, client);
    OpcuaConnector connector(client, m_endpoint);

    QScopedPointer<QOpcUaNode> serverNode(client->node("ns=0;i=2253")); // Server object
    QVERIFY(serverNode != nullptr);

    QScopedPointer<QOpcUaNode> objectsNode(client->node("ns=0;i=85")); // Objects folder
    QVERIFY(objectsNode != nullptr);

    qDebug() << "Node created";

    QSignalSpy enabledSpy(serverNode.data(), &QOpcUaNode::enableMonitoringFinished);
    QSignalSpy eventSpy(serverNode.data(), &QOpcUaNode::eventOccurred);

    QOpcUaMonitoringParameters::EventFilter filter;
    filter << QOpcUaSimpleAttributeOperand("Severity");
    filter << QOpcUaSimpleAttributeOperand("Message");

//    Where clause is not yet supported in the open62541 server
//    // Only events with severity >= 700
//    QOpcUaContentFilterElement whereElement;
//    whereElement << QOpcUaContentFilterElement::FilterOperator::GreaterThanOrEqual << QOpcUaSimpleAttributeOperand("Severity") <<
//                    QOpcUaLiteralOperand(quint16(700), QOpcUa::Types::UInt16);
//    filter << whereElement;

    QOpcUaMonitoringParameters p(0);
    p.setFilter(filter);

    serverNode->enableMonitoring(QOpcUa::NodeAttribute::EventNotifier, p);
    enabledSpy.wait();
    QCOMPARE(enabledSpy.size(), 1);
    QCOMPARE(enabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::EventNotifier);
    QCOMPARE(enabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);

    QCOMPARE(serverNode->monitoringStatus(QOpcUa::NodeAttribute::EventNotifier).filter().value<QOpcUaMonitoringParameters::EventFilter>(),
             filter);

//    Disabled because the open62541 server does not currently return an EventFilterResult
//    QVERIFY(serverNode->monitoringStatus(QOpcUa::NodeAttribute::EventNotifier).filterResult().isValid());
//    QOpcUaEventFilterResult res = serverNode->monitoringStatus(QOpcUa::NodeAttribute::EventNotifier).filterResult().value<QOpcUaEventFilterResult>();
//    QVERIFY(res.isGood() == true);

    qDebug() << "Monitoring enabled, waiting for event...";

    objectsNode->callMethod(QStringLiteral("ns=1;i=62541")); // Trigger event
    eventSpy.wait();
    QCOMPARE(eventSpy.size(), 1);
    QCOMPARE(eventSpy.at(0).at(0).toList().size(), 2);
    quint16 severity = eventSpy.at(0).at(0).toList().at(0).value<quint16>();
    QOpcUaLocalizedText message = eventSpy.at(0).at(0).toList().at(1).value<QOpcUaLocalizedText>();

    qDebug() << "Event received";
    qDebug() << "Message:" << message.locale() << message.text();
    qDebug() << "Severity:" << severity;

    QCOMPARE(severity, 100);
    QCOMPARE(message, QOpcUaLocalizedText("en-US", "An event has been generated."));

    qDebug() << "Modifying event filter...";

    eventSpy.clear();

    QSignalSpy modifySpy(serverNode.data(), &QOpcUaNode::monitoringStatusChanged);
    filter << QOpcUaSimpleAttributeOperand("SourceNode");
    serverNode->modifyEventFilter(filter);
    modifySpy.wait();
    QCOMPARE(modifySpy.size(), 1);
    QCOMPARE(modifySpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::EventNotifier);
    QVERIFY(modifySpy.at(0).at(1).value<QOpcUaMonitoringParameters::Parameters>().testFlag(QOpcUaMonitoringParameters::Parameter::Filter) == true);
    QCOMPARE(modifySpy.at(0).at(2).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
    QCOMPARE(serverNode->monitoringStatus(QOpcUa::NodeAttribute::EventNotifier).filter().value<QOpcUaMonitoringParameters::EventFilter>(),
             filter);
    qDebug() << "EventFilter modified, waiting for event with additional SourceNode field...";

    objectsNode->callMethod(QStringLiteral("ns=1;i=62541")); // Trigger event
    eventSpy.wait();
    QCOMPARE(eventSpy.size(), 1);
    QCOMPARE(eventSpy.at(0).at(0).toList().size(), 3);
    severity = eventSpy.at(0).at(0).toList().at(0).value<quint16>();
    message = eventSpy.at(0).at(0).toList().at(1).value<QOpcUaLocalizedText>();
    QString sourceNode = eventSpy.at(0).at(0).toList().at(2).toString();

    qDebug() << "Event received";
    qDebug() << "Message:" << message.locale() << message.text();
    qDebug() << "Severity:" << severity;
    qDebug() << "SourceNode:" << sourceNode;

    QCOMPARE(severity, 100);
    QCOMPARE(message, QOpcUaLocalizedText("en-US", "An event has been generated."));
    QCOMPARE(sourceNode, QStringLiteral("ns=0;i=2253"));

    QSignalSpy disabledSpy(serverNode.data(), &QOpcUaNode::disableMonitoringFinished);
    serverNode->disableMonitoring(QOpcUa::NodeAttribute::EventNotifier);
    disabledSpy.wait();
    QCOMPARE(disabledSpy.size(), 1);
    QCOMPARE(disabledSpy.at(0).at(0).value<QOpcUa::NodeAttribute>(), QOpcUa::NodeAttribute::EventNotifier);
    QCOMPARE(disabledSpy.at(0).at(1).value<QOpcUa::UaStatusCode>(), QOpcUa::UaStatusCode::Good);
}

QTEST_MAIN(EventsubscriptionTest)

#include "tst_eventsubscription.moc"

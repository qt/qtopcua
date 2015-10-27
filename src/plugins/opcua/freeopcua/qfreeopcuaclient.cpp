/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
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

#include "qfreeopcuaclient.h"

#include "qfreeopcuanode.h"
#include "qfreeopcuasubscription.h"
#include "qfreeopcuavalueconverter.h"

#include <qopcuasubscription.h>

#include <opc/ua/node.h>
#include <opc/ua/client/client.h>
#include <opc/ua/protocol/variant.h>
#include <opc/ua/services/method.h>

#include <QtCore/qdebug.h>
#include <QtNetwork/qhostinfo.h>

QT_BEGIN_NAMESPACE

QFreeOpcUaClient::QFreeOpcUaClient(QObject *parent)
    : QOpcUaClient(parent)
    , OpcUa::UaClient(false)
{
}

QFreeOpcUaClient::~QFreeOpcUaClient()
{
}

bool QFreeOpcUaClient::connectToEndpoint(const QString &url)
{
    try {
        setUrl(url);

        QString processedUrl;
        QString sNodeName = QHostInfo::localHostName();
        SetApplicationURI(QString("urn:%1:%2:%3").arg(
                              sNodeName).arg("qt-project").arg("QOpcUaClient").toStdString());
        SetProductURI("urn:qt-project:QOpcUaClient");
        SetSessionName(GetApplicationURI());

        processedUrl = url;

        if (!username().isNull()) {
            QString authPart = QString("opc.tcp://%1:%2@").arg(username()).arg(password());
            processedUrl = processedUrl.replace("opc.tcp://", authPart);
            qDebug() << processedUrl;
        }

        Connect(url.toStdString());

        // Check connection status by getting the root node
        GetRootNode();
        qDebug() << "Client connected to" << url.toStdString().c_str();
    } catch (const std::exception &e) {
        Disconnect();
        qWarning() << "Client could not connect to endpoint" << url;
        qWarning() << e.what();
        return setConnected(false);
    }
    return setConnected(true);
}

bool QFreeOpcUaClient::secureConnectToEndpoint(const QString &url)
{
    Q_UNUSED(url);
    return false;
}

bool QFreeOpcUaClient::disconnectFromEndpoint()
{
    try {
        cleanupChildren();

        QList<QFreeOpcUaSubscription *> subs = this->findChildren<QFreeOpcUaSubscription *>();
        qDeleteAll(subs);

        Disconnect();

        setConnected(false);
        return true;
    } catch (const std::exception &ex) {
        qWarning() << "Could not disconnect from endpoint: " << ex.what();
    }

    return false;
}

QVariant QFreeOpcUaClient::read(const QString &xmlNodeId)
{
    try {
        OpcUa::Node node = GetNode(xmlNodeId.toStdString());
        return QVariant(QFreeOpcUaValueConverter::toQVariant(node.GetValue()));
    } catch (const std::exception &ex) {
        qWarning() << "Could not read value of node " << xmlNodeId;
        qWarning() << ex.what();
    }
    return QVariant();
}

QOpcUaMonitoredItem *QFreeOpcUaClient::dataMonitor(double interval,
                                                      const QString &xmlNodeId)
{
    // Use getSubscription() for possible optimization when many items use the same interval
    QOpcUaSubscription *subscription = getSubscription(interval);

    if (subscription && subscription->success()) {
        QOpcUaMonitoredItem *item = subscription->addItem(xmlNodeId);
        if (item)
            return item;
    }

    // FIXME: cleanup subscription?!
    qWarning() << "Adding item" << xmlNodeId << "to subscription failed!";
    return 0;
}

QOpcUaMonitoredItem *QFreeOpcUaClient::eventMonitor(const QString &xmlEventNodeId)
{
    // Note: Callback is not called due to some error in the event implementation in freeopcua

    // Each event monitor gets its own subscription at the moment
    QOpcUaSubscription *temporarySubscription = new QFreeOpcUaSubscription(this);
    if (temporarySubscription->success()) {
        QOpcUaMonitoredItem *item = temporarySubscription->addEventItem(xmlEventNodeId);
        if (item)
            return item;
    }

    delete temporarySubscription;
    return 0;
}


bool QFreeOpcUaClient::write(const QString &xmlNodeId, const QVariant &value, QOpcUa::Types type)
{
    try {
        OpcUa::Node node = GetNode(xmlNodeId.toStdString());
        node.SetValue(QFreeOpcUaValueConverter::toTypedVariant(value, type));
        return true;
    } catch (const std::exception &ex) {
        qDebug() << "Could not write value to node " <<  xmlNodeId;
        qWarning() << ex.what();
    }
    return false;
}

bool QFreeOpcUaClient::call(const QString &xmlObjectNodeId, const QString &xmlMethodNodeId,
                            QVector<QOpcUaTypedVariant> *args, QVector<QVariant> *ret)
{
    OpcUa::NodeId objectId;
    OpcUa::NodeId methodId;

    try {
        objectId = GetNode(xmlObjectNodeId.toStdString()).GetId();
        methodId = GetNode(xmlMethodNodeId.toStdString()).GetId();
    } catch (const std::exception &ex) {
        qWarning() << "Could not get nodes for method call";
        qWarning() << ex.what();
        return false;
    }

    try {
        std::vector<OpcUa::Variant> arguments;
        if (args) {
            arguments.reserve(args->size());
            foreach (const QOpcUaTypedVariant &v, *args)
                arguments.push_back(QFreeOpcUaValueConverter::toTypedVariant(v.first, v.second));
        }
        OpcUa::CallMethodRequest myCallRequest;
        myCallRequest.ObjectId = objectId;
        myCallRequest.MethodId = methodId;
        myCallRequest.InputArguments = arguments;
        std::vector<OpcUa::CallMethodRequest> myCallVector;
        myCallVector.push_back(myCallRequest);


        OpcUa::Node objNode = GetNode(objectId);
        std::vector<OpcUa::Variant> returnedValues = objNode.CallMethod(methodId, arguments);

        // status code of method call is checked via exception inside the node
        ret->reserve(returnedValues.size());
        for (std::vector<OpcUa::Variant>::const_iterator it = returnedValues.cbegin(); it != returnedValues.cend(); ++it)
            ret->push_back(QFreeOpcUaValueConverter::toQVariant(*it));

        return true;

    } catch (const std::exception &ex) {
        qWarning() << "Method call failed: " << ex.what();
        return false;
    }
}

// Support for structures in freeopcua seems to be not implemented yet
QPair<QString, QString> QFreeOpcUaClient::readEui(const QString &xmlNodeId)
{
    OPCUA_UNUSED(xmlNodeId);
    // Return empty QVariant
    return QPair<QString, QString>();
}

// Support for structures in freeopcua seems to be not implemented yet
QPair<double, double> QFreeOpcUaClient::readEuRange(const QString &xmlNodeId)
{
    OPCUA_UNUSED(xmlNodeId);
    // Return empty QVariant
    return QPair<double, double>();
}

QOpcUaNode *QFreeOpcUaClient::node(const QString &xmlNodeId)
{
    qDebug() << "New node requested from freeopcua" << xmlNodeId;
    OpcUa::Node node = GetNode(xmlNodeId.toStdString());
    return new QFreeOpcUaNode(node, this);
}

QList<QPair<QVariant, QDateTime> > QFreeOpcUaClient::readHistorical(const QString &node,
                                                                    int maxCount,
                                                                    const QDateTime &begin, const QDateTime &end)
{
    // not supported with freeopcua
    Q_UNUSED(node);
    Q_UNUSED(begin);
    Q_UNUSED(end);
    Q_UNUSED(maxCount);
    return QList<QPair<QVariant, QDateTime>>();
}

QOpcUaSubscription *QFreeOpcUaClient::createSubscription(double interval)
{
    QFreeOpcUaSubscription *temporarySubscription = new QFreeOpcUaSubscription(this, interval);
    if (temporarySubscription->success())
        return temporarySubscription;

    delete temporarySubscription;
    return Q_NULLPTR;
}

QT_END_NAMESPACE

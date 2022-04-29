/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#ifndef OPCUASERVERDISCOVERY_H
#define OPCUASERVERDISCOVERY_H

#include "opcuastatus.h"
#include "opcuaconnection.h"

#include <QtOpcUa/qopcuatype.h>
#include <QOpcUaApplicationDescription>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE

class OpcUaServerDiscovery : public QStandardItemModel
{
    Q_OBJECT
    Q_PROPERTY(QString discoveryUrl READ discoveryUrl WRITE setDiscoveryUrl NOTIFY discoveryUrlChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(OpcUaStatus status READ status NOTIFY statusChanged)
    Q_PROPERTY(OpcUaConnection* connection READ connection WRITE setConnection NOTIFY connectionChanged)

public:
    OpcUaServerDiscovery(QObject *parent = nullptr);
    ~OpcUaServerDiscovery();

    const QString &discoveryUrl() const;
    void setDiscoveryUrl(const QString &discoverUrl);
    int count() const;
    Q_INVOKABLE QOpcUaApplicationDescription at(int row) const;
    const OpcUaStatus &status() const;
    void setConnection(OpcUaConnection *);
    OpcUaConnection *connection();

signals:
    void discoveryUrlChanged();
    void serversChanged();
    void countChanged();
    void statusChanged();
    void connectionChanged(OpcUaConnection *);

private slots:
    void connectSignals();
    void handleServers(const QList<QOpcUaApplicationDescription> &servers, QOpcUa::UaStatusCode statusCode, const QUrl &requestUrl);
    void startFindServers();

private:
    void clearData();

    QString m_discoveryUrl;
    OpcUaConnection *m_connection = nullptr;
    OpcUaStatus m_status;
};


QT_END_NAMESPACE

#endif // OPCUASERVERDISCOVERY_H

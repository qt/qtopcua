// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef OPCUASERVERDISCOVERY_P_H
#define OPCUASERVERDISCOVERY_P_H

#include <private/opcuastatus_p.h>
#include <private/opcuaconnection_p.h>

#include <QtOpcUa/qopcuatype.h>
#include <QOpcUaApplicationDescription>
#include <QStandardItemModel>
#include <QtQml/qqml.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

QT_BEGIN_NAMESPACE

class OpcUaServerDiscovery : public QStandardItemModel
{
    Q_OBJECT
    Q_PROPERTY(QString discoveryUrl READ discoveryUrl WRITE setDiscoveryUrl NOTIFY discoveryUrlChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(OpcUaStatus status READ status NOTIFY statusChanged)
    Q_PROPERTY(OpcUaConnection* connection READ connection WRITE setConnection NOTIFY connectionChanged)

    QML_NAMED_ELEMENT(ServerDiscovery)
    QML_ADDED_IN_VERSION(5, 13)

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

#endif // OPCUASERVERDISCOVERY_P_H

// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef OPCUAENDPOINTDISCOVERY_P_H
#define OPCUAENDPOINTDISCOVERY_P_H

#include <private/opcuaconnection_p.h>
#include <private/opcuastatus_p.h>

#include <QtOpcUa/qopcuatype.h>
#include <QOpcUaEndpointDescription>

#include <QQmlParserStatus>
#include <QObject>
#include <QList>

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

class OpcUaEndpointDiscovery : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QString serverUrl READ serverUrl WRITE setServerUrl NOTIFY serverUrlChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(OpcUaStatus status READ status NOTIFY statusChanged)
    Q_PROPERTY(OpcUaConnection* connection READ connection WRITE setConnection NOTIFY connectionChanged)
    Q_INTERFACES(QQmlParserStatus)

    QML_NAMED_ELEMENT(EndpointDiscovery)
    QML_ADDED_IN_VERSION(5, 13)

public:
    OpcUaEndpointDiscovery(QObject *parent = nullptr);
    ~OpcUaEndpointDiscovery();

    const QString &serverUrl() const;
    void setServerUrl(const QString &serverUrl);
    int count() const;
    Q_INVOKABLE QOpcUaEndpointDescription at(int row) const;
    const OpcUaStatus &status() const;
    void setConnection(OpcUaConnection *);
    OpcUaConnection *connection();

signals:
    void serverUrlChanged(const QString &serverUrl);
    void endpointsChanged();
    void countChanged();
    void statusChanged();
    void connectionChanged(OpcUaConnection *);

private slots:
    void connectSignals();
    void handleEndpoints(const QList<QOpcUaEndpointDescription> &endpoints, QOpcUa::UaStatusCode statusCode, const QUrl &requestUrl);
    void startRequestEndpoints();

private:
    // Callbacks from QQmlParserStatus
    void classBegin() override;
    void componentComplete() override;

    QString m_serverUrl;
    OpcUaConnection *m_connection = nullptr;
    QList<QOpcUaEndpointDescription> m_endpoints;
    OpcUaStatus m_status;
    bool m_componentCompleted = false;
};


QT_END_NAMESPACE

#endif // OPCUAENDPOINTDISCOVERY_P_H

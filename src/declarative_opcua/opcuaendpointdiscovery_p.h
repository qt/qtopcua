/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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

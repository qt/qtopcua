/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

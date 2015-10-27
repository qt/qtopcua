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

#ifndef QOPCUACLIENT_H
#define QOPCUACLIENT_H

#include <QtCore/qdatetime.h>
#include <QtCore/qobject.h>
#include <QtCore/qpluginloader.h>
#include <QtCore/QUrl>
#include <QtCore/qvariant.h>

#include <QtOpcUa/qopcuaglobal.h>
#include <QtOpcUa/qopcuamonitoreditem.h>
#include <QtOpcUa/qopcuanode.h>
#include <QtOpcUa/qopcuasubscription.h>
#include <QtOpcUa/qopcuatype.h>

QT_BEGIN_NAMESPACE

class Q_OPCUA_EXPORT QOpcUaClient : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

public:
    ~QOpcUaClient();

    // Convenience methods
    Q_INVOKABLE bool connectToEndpoint();
    Q_INVOKABLE bool secureConnectToEndpoint();

    // Overridables
    Q_INVOKABLE virtual bool connectToEndpoint(const QUrl &url) = 0;
    Q_INVOKABLE virtual bool secureConnectToEndpoint(const QUrl &url) = 0;
    Q_INVOKABLE virtual bool disconnectFromEndpoint() = 0;

    Q_INVOKABLE virtual QVariant read(const QString &xmlNodeId) = 0;
    Q_INVOKABLE virtual bool write(const QString &xmlNodeId, const QVariant &value,
            QOpcUa::Types type = QOpcUa::Undefined)  = 0;
    Q_INVOKABLE virtual bool call(const QString &xmlObjectNodeId, const QString &xmlMethodNodeId,
            QVector<QOpcUaTypedVariant> *args = 0, QVector<QVariant>  *ret = 0) = 0;
    Q_INVOKABLE virtual QPair<QString, QString> readEui(const QString &xmlNodeId) = 0;
    Q_INVOKABLE virtual QPair<double, double> readEuRange(const QString &xmlNodeId) = 0;

    Q_INVOKABLE virtual QOpcUaMonitoredItem *dataMonitor(double interval,
            const QString &xmlNodeId) = 0;
    Q_INVOKABLE virtual QOpcUaMonitoredItem *eventMonitor(const QString &xmlEventNodeId) = 0;

    Q_INVOKABLE virtual QOpcUaNode *node(const QString &xmlNodeId) = 0;

    Q_INVOKABLE virtual QList<QPair<QVariant, QDateTime> > readHistorical(
            const QString &node, int maxCount, const QDateTime &begin, const QDateTime &end) = 0;

    QUrl url() const;
    bool isConnected() const;

public Q_SLOTS:
    void setUrl(const QUrl &url);

Q_SIGNALS:
    void connectedChanged(bool connected);

protected:
    explicit QOpcUaClient(QObject *parent = 0);

    QOpcUaSubscription *getSubscription(double interval);
    virtual QOpcUaSubscription *createSubscription(double interval) = 0;
    bool removeSubscription(double interval);

    bool setConnected(bool connected);
    void cleanupChildren();

private:
    QUrl m_url;
    QPluginLoader m_loader;
    bool m_connected;
    QString m_bestPlugin;
    QString m_plugin;

    QHash<double, QOpcUaSubscription*> m_subscriptions;
};

QT_END_NAMESPACE

#endif // QOPCUACLIENT_H

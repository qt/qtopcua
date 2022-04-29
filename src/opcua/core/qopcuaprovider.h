/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
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

#ifndef QOPCUAPROVIDER_H
#define QOPCUAPROVIDER_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qhash.h>
#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QOpcUaPlugin;
class QOpcUaClient;

class Q_OPCUA_EXPORT QOpcUaProvider : public QObject
{
    Q_OBJECT

public:
    static QStringList availableBackends();

    explicit QOpcUaProvider(QObject *parent = nullptr);
    ~QOpcUaProvider() override;

    Q_INVOKABLE QOpcUaClient *createClient(const QString &backend, const QVariantMap &backendProperties = QVariantMap());

private:
    QMultiHash<QString, QOpcUaPlugin *> m_plugins;
};

QT_END_NAMESPACE

#endif // QOPCUAPROVIDER_H

// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAPROVIDER_H
#define QOPCUAPROVIDER_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qhash.h>
#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>
#include <QtCore/qmap.h> // for QVariantMap

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

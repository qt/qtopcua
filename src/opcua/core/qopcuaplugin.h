// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAPLUGIN_H
#define QOPCUAPLUGIN_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

class QOpcUaClient;

#define QOpcUaProviderFactory_iid "org.qt-project.qt.opcua.providerfactory/1.0"

class Q_OPCUA_EXPORT QOpcUaPlugin : public QObject
{
    Q_OBJECT
public:
    explicit QOpcUaPlugin(QObject *parent = nullptr);
    ~QOpcUaPlugin() override;

    virtual QOpcUaClient *createClient(const QVariantMap &backendProperties) = 0;
};
Q_DECLARE_INTERFACE(QOpcUaPlugin, QOpcUaProviderFactory_iid)

QT_END_NAMESPACE

#endif // QOPCUAPLUGIN_H

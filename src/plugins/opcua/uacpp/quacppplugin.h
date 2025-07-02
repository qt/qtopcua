// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QUACPPPLUGIN_H
#define QUACPPPLUGIN_H

#include "qopcuaplugin.h"

#include <uanodeid.h>

QT_BEGIN_NAMESPACE

class QUACppPlugin : public QOpcUaPlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "org.qt-project.qt.opcua.providerfactory/1.0" FILE "uacpp-metadata.json")
    Q_INTERFACES(QOpcUaPlugin)

public:
    explicit QUACppPlugin(QObject *parent = 0);
    ~QUACppPlugin() override;

    QOpcUaClient *createClient(const QVariantMap &backendProperties) override;
private:
};

QT_END_NAMESPACE

#endif // QUACPPPLUGIN_H

// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPEN62541PLUGIN_H
#define QOPEN62541PLUGIN_H

#include "qopen62541.h"
#include <QtOpcUa/qopcuaplugin.h>

QT_BEGIN_NAMESPACE

class QOpen62541Plugin : public QOpcUaPlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "org.qt-project.qt.opcua.providerfactory/1.0" FILE "open62541-metadata.json")
    Q_INTERFACES(QOpcUaPlugin)

public:
    explicit QOpen62541Plugin(QObject *parent = nullptr);
    ~QOpen62541Plugin() override;

    QOpcUaClient *createClient(const QVariantMap &backendProperties) override;
};

QT_END_NAMESPACE

#endif // QOPEN62541PLUGIN_H

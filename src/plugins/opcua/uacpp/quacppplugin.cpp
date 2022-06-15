// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "quacppplugin.h"
#include "quacppclient.h"

#include <QtCore/QLoggingCategory>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_UACPP, "qt.opcua.plugins.uacpp")

QUACppPlugin::QUACppPlugin(QObject *parent)
    : QOpcUaPlugin(parent)
{
    qRegisterMetaType<UaNodeId>();
}

QUACppPlugin::~QUACppPlugin()
{
}

QOpcUaClient *QUACppPlugin::createClient(const QVariantMap &backendProperties)
{
    return new QOpcUaClient(new QUACppClient(backendProperties));
}

QT_END_NAMESPACE

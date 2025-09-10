// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopen62541client.h"
#include "qopen62541node.h"
#include "qopen62541plugin.h"
#include "qopen62541valueconverter.h"
#include <QtOpcUa/qopcuaclient.h>

#include <QtCore/qloggingcategory.h>

QT_BEGIN_NAMESPACE

static void compileTimeEnforceEnumMappings(void)
{
    static_assert(static_cast<QOpcUa::NodeClass>(UA_NODECLASS_UNSPECIFIED) == QOpcUa::NodeClass::Undefined,
                  "open62541 and QOpcUa values for NodeClasses must be the same");
    static_assert(static_cast<QOpcUa::NodeClass>(UA_NODECLASS_VARIABLE) == QOpcUa::NodeClass::Variable,
                  "open62541 and QOpcUa values for NodeClasses must be the same");
}

QOpen62541Plugin::QOpen62541Plugin(QObject *parent)
    : QOpcUaPlugin(parent)
{
    compileTimeEnforceEnumMappings();
    qRegisterMetaType<UA_NodeId>();
}

QOpen62541Plugin::~QOpen62541Plugin()
{
}

QOpcUaClient *QOpen62541Plugin::createClient(const QVariantMap &backendProperties)
{
    Q_UNUSED(backendProperties)
    return new QOpcUaClient(new QOpen62541Client(backendProperties));
}

Q_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_OPEN62541, "qt.opcua.plugins.open62541")

QT_END_NAMESPACE

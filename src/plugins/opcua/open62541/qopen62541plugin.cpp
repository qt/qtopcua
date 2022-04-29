/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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

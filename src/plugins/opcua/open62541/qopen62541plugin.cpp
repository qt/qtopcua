/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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

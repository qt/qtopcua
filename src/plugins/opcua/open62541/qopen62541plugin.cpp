/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
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

#include "qopen62541client.h"
#include "qopen62541node.h"
#include "qopen62541plugin.h"
#include "qopen62541valueconverter.h"
#include <QtOpcUa/qopcuaclient.h>

#include <QtCore/qloggingcategory.h>

QT_BEGIN_NAMESPACE

static void compileTimeEnforceEnumMappings(void)
{
    static_assert(static_cast<QOpcUaNode::NodeClass>(UA_NODECLASS_UNSPECIFIED) == QOpcUaNode::NodeClass::Undefined,
                  "open62541 and QOpcUa values for NodeClasses must be the same");
    static_assert(static_cast<QOpcUaNode::NodeClass>(UA_NODECLASS_VARIABLE) == QOpcUaNode::NodeClass::Variable,
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

QOpcUaClient *QOpen62541Plugin::createClient()
{
    return new QOpcUaClient(new QOpen62541Client);
}

Q_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_OPEN62541, "qt.opcua.plugins.open62541")

QT_END_NAMESPACE

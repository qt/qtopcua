/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#include "opcua_plugin.h"
#include "opcuavaluenode.h"
#include "opcuamethodnode.h"
#include "opcuanodeid.h"
#include "opcuanodeidtype.h"
#include "opcuaconnection.h"
#include "opcuarelativenodepath.h"
#include "opcuarelativenodeid.h"
#include "qopcuatype.h"
#include <QLoggingCategory>

#include <qqml.h>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_QML, "qt.opcua.plugins.qml")

namespace Constants {
    Q_NAMESPACE
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
    Q_ENUM_NS(QOpcUa::ReferenceTypeId)
    // Only one type declaration is needed because all other
    // types of the same meta object will be added automatically.
}

void OpcUaPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QStringLiteral("QtOpcUa"));

    // @uri QtOpcUa
    const int major = 5;
    const int minor = 12;

    // Register the 5.12 types

    qmlRegisterType<OpcUaConnection>(uri, major, minor, "Connection");
    qmlRegisterType<OpcUaValueNode>(uri, major, minor, "ValueNode");
    qmlRegisterType<OpcUaMethodNode>(uri, major, minor, "MethodNode");
    qmlRegisterType<OpcUaNode>(uri, major, minor, "Node");
    qmlRegisterType<OpcUaNodeId>(uri, major, minor, "NodeId");
    qmlRegisterUncreatableType<OpcUaNodeIdType>(uri, major, minor, "NodeIdType",
                  "The type 'NodeIdType' is not creatable. Use 'NodeId' or 'RelativeNodeId' instead.");
    qmlRegisterType<OpcUaRelativeNodePath>(uri, major, minor, "RelativeNodePath");
    qmlRegisterType<OpcUaRelativeNodeId>(uri, major, minor, "RelativeNodeId");
    qmlRegisterUncreatableMetaObject(Constants::staticMetaObject, uri, major, minor, "Constants", "This type can not be created.");

    // insert new versions here

    // Register the latest Qt version as QML type version
    qmlRegisterModule(uri, QT_VERSION_MAJOR, QT_VERSION_MINOR);
}

#include "opcua_plugin.moc"

QT_END_NAMESPACE

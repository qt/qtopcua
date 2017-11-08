/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
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

#include "qfreeopcuaclient.h"
#include "qfreeopcuanode.h"
#include "qfreeopcuasubscription.h"
#include "qfreeopcuavalueconverter.h"
#include "qfreeopcuaworker.h"
#include <QtOpcUa/qopcuamonitoredevent.h>
#include <QtOpcUa/qopcuamonitoredvalue.h>

#include <QtCore/qdatetime.h>
#include <QtCore/qloggingcategory.h>

#include <opc/ua/client/client.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_FREEOPCUA)

QFreeOpcUaNode::QFreeOpcUaNode(OpcUa::Node node, QFreeOpcUaClientImpl *client)
    : m_node(node)
    , m_client(client)
{
    m_client->registerNode(this);
}

QFreeOpcUaNode::~QFreeOpcUaNode()
{
    if (m_client)
        m_client->unregisterNode(this);
}

bool QFreeOpcUaNode::readAttributes(QOpcUaNode::NodeAttributes attr)
{
    return QMetaObject::invokeMethod(m_client->m_opcuaWorker, "readAttributes",
                                     Qt::QueuedConnection,
                                     Q_ARG(uintptr_t, reinterpret_cast<uintptr_t>(this)),
                                     Q_ARG(OpcUa::NodeId, m_node.GetId()),
                                     Q_ARG(QOpcUaNode::NodeAttributes, attr));
}

QStringList QFreeOpcUaNode::childrenIds() const
{
    QStringList result;
    try {
        std::vector<OpcUa::Node> tmp = m_node.GetChildren();
        result.reserve(tmp.size());
        for (std::vector<OpcUa::Node>::const_iterator it = tmp.cbegin(); it != tmp.end(); ++it)
            result.append(QFreeOpcUaValueConverter::nodeIdToString(it->GetId()));
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "Failed to get child ids for node:" << ex.what();
    }

    return result;
}

QString QFreeOpcUaNode::nodeId() const
{
    try {
        return QFreeOpcUaValueConverter::nodeIdToString(m_node.GetId());
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "Failed to get id for node:" << ex.what();
        return QString();
    }
}

bool QFreeOpcUaNode::setValue(const QVariant &value, QOpcUa::Types type)
{
    try {
        OpcUa::Variant toWrite = QFreeOpcUaValueConverter::toTypedVariant(value, type);
        if (toWrite.IsNul())
            return false;

        m_node.SetValue(toWrite);
        return true;
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "Could not write value to node " <<  nodeId();
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << ex.what();
    }
    return false;
}

bool QFreeOpcUaNode::call(const QString &methodNodeId,
                            QVector<QOpcUa::TypedVariant> *args, QVector<QVariant> *ret)
{
    OpcUa::NodeId objectId;
    OpcUa::NodeId methodId;

    if (!m_client)
        return false;

    try {
        objectId = m_node.GetId();

        OpcUa::Node methodNode = m_client->m_opcuaWorker->GetNode(methodNodeId.toStdString());
        methodNode.GetBrowseName();
        methodId = methodNode.GetId();
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "Could not get node for method call";
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << ex.what();
        return false;
    }

    try {
        std::vector<OpcUa::Variant> arguments;
        if (args) {
            arguments.reserve(args->size());
            for (const QOpcUa::TypedVariant &v: qAsConst(*args))
                arguments.push_back(QFreeOpcUaValueConverter::toTypedVariant(v.first, v.second));
        }
        OpcUa::CallMethodRequest myCallRequest;
        myCallRequest.ObjectId = objectId;
        myCallRequest.MethodId = methodId;
        myCallRequest.InputArguments = arguments;
        std::vector<OpcUa::CallMethodRequest> myCallVector;
        myCallVector.push_back(myCallRequest);


        std::vector<OpcUa::Variant> returnedValues = m_node.CallMethod(methodId, arguments);

        // status code of method call is checked via exception inside the node
        ret->reserve(returnedValues.size());
        for (auto it = returnedValues.cbegin(); it != returnedValues.cend(); ++it)
            ret->push_back(QFreeOpcUaValueConverter::toQVariant(*it));

        return true;

    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "Method call failed: " << ex.what();
        return false;
    }
}

// Support for structures in freeopcua seems to be not implemented yet
QPair<QString, QString> QFreeOpcUaNode::readEui() const
{
    // Return empty QVariant
    return QPair<QString, QString>();
}

// Support for structures in freeopcua seems to be not implemented yet
QPair<double, double> QFreeOpcUaNode::readEuRange() const
{
    return QPair<double, double>(qQNaN(), qQNaN());
}

QT_END_NAMESPACE

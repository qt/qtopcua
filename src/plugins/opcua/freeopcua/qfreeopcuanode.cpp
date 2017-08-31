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

#include "qfreeopcuanode.h"

#include "qfreeopcuaclient.h"
#include "qfreeopcuasubscription.h"
#include "qfreeopcuavalueconverter.h"

#include <QtOpcUa/qopcuamonitoredvalue.h>
#include <QtOpcUa/qopcuamonitoredevent.h>

#include <QtCore/qdatetime.h>
#include <QtCore/qdebug.h>

#include <opc/ua/client/client.h>

QFreeOpcUaNode::QFreeOpcUaNode(OpcUa::Node node, OpcUa::UaClient *client)
    : m_node(node)
    , m_client(client)
{
}

QFreeOpcUaNode::~QFreeOpcUaNode()
{
}

QString QFreeOpcUaNode::name() const
{
    try {
        return QString::fromStdString(m_node.GetAttribute(
                                          OpcUa::AttributeId::DisplayName).Value.As<OpcUa::LocalizedText>().Text);
    } catch (const std::exception &ex) {
        qWarning() << "Failed to get BrowseName for node: " << ex.what();
    }
    return QString();
}

QOpcUa::Types QFreeOpcUaNode::type() const
{
    return QOpcUa::Types::Undefined;
//    try {
//        OpcUa::NodeId idNode = m_node.GetDataType().As<OpcUa::NodeId>();
//        return QString::fromStdString(m_client->GetNode(idNode)
//                                      .GetAttribute(OpcUa::AttributeId::DisplayName).Value
//                                      .As<OpcUa::LocalizedText>()
//                                      .Text);
//    } catch (const std::exception &ex) {
//        qWarning() << ex.what();
//        return QString();
//    }
}

QVariant QFreeOpcUaNode::value() const
{
    try {
        OpcUa::Variant val = m_node.GetValue();
        if (val.IsNul())
            return QVariant();
        else
            return QFreeOpcUaValueConverter::toQVariant(val);
    } catch (const std::exception &ex) {
        qWarning() << ex.what();
        return QVariant();
    }
}

QStringList QFreeOpcUaNode::childIds() const
{
    QStringList result;
    try {
        std::vector<OpcUa::Node> tmp = m_node.GetChildren();
        result.reserve(tmp.size());
        for (std::vector<OpcUa::Node>::const_iterator it = tmp.cbegin(); it != tmp.end(); ++it)
            result.append(QFreeOpcUaValueConverter::nodeIdToString(it->GetId()));
    } catch (const std::exception &ex) {
        qWarning() << "Failed to get child ids for node:" << ex.what();
    }

    return result;
}

QString QFreeOpcUaNode::nodeId() const
{
    try {
        return QFreeOpcUaValueConverter::nodeIdToString(m_node.GetId());
    } catch (const std::exception &ex) {
        qWarning() << "Failed to get id for node:" << ex.what();
        return QString();
    }
}

QString QFreeOpcUaNode::nodeClass() const
{
    try {
        int32_t temp = m_node.GetAttribute(OpcUa::AttributeId::NodeClass).Value.As<int32_t>();
        OpcUa::NodeClass nc = static_cast<OpcUa::NodeClass>(temp);

        switch (nc) {
        case OpcUa::NodeClass::Object:
            return QStringLiteral("Object");
        case OpcUa::NodeClass::Variable:
            return QStringLiteral("Variable");
        case OpcUa::NodeClass::Method:
            return QStringLiteral("Method");
        case OpcUa::NodeClass::ObjectType:
            return QStringLiteral("ObjectType");
        case OpcUa::NodeClass::VariableType:
            return QStringLiteral("VariableType");
        case OpcUa::NodeClass::ReferenceType:
            return QStringLiteral("ReferenceType");
        case OpcUa::NodeClass::DataType:
            return QStringLiteral("DataType");
        case OpcUa::NodeClass::View:
            return QStringLiteral("View");
        default:
            return QString();
        }
    } catch (const std::exception &ex) {
        qWarning() << "Failed to get node class for node:" << ex.what();
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
        qWarning() << "Could not write value to node " <<  nodeId();
        qWarning() << ex.what();
    }
    return false;
}

bool QFreeOpcUaNode::call(const QString &methodNodeId,
                            QVector<QOpcUa::TypedVariant> *args, QVector<QVariant> *ret)
{
    OpcUa::NodeId objectId;
    OpcUa::NodeId methodId;

    try {
        objectId = m_node.GetId();

        OpcUa::Node methodNode = m_client->GetNode(methodNodeId.toStdString());
        methodNode.GetBrowseName();
        methodId = methodNode.GetId();
    } catch (const std::exception &ex) {
        qWarning() << "Could not get node for method call";
        qWarning() << ex.what();
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
        qWarning() << "Method call failed: " << ex.what();
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

QVector<QPair<QVariant, QDateTime> > QFreeOpcUaNode::readHistorical(uint maxCount,
        const QDateTime &begin, const QDateTime &end) const
{
    // not supported with freeopcua
    Q_UNUSED(begin);
    Q_UNUSED(end);
    Q_UNUSED(maxCount);
    return QVector<QPair<QVariant, QDateTime>>();
}

bool QFreeOpcUaNode::writeHistorical(QOpcUa::Types type,
            const QVector<QPair<QVariant, QDateTime> > data)
{
    // not supported with freeopcua
    Q_UNUSED(type);
    Q_UNUSED(data);
    return false;
}

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
#include <QtOpcUa/qopcuasubscription.h>

#include <QtNetwork/qhostinfo.h>
#include <QtCore/qloggingcategory.h>

#include <opc/ua/node.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_FREEOPCUA)

QFreeOpcUaWorker::QFreeOpcUaWorker(QFreeOpcUaClientImpl *client)
    : QOpcUaBackend()
    , m_client(client)
{}

void QFreeOpcUaWorker::asyncConnectToEndpoint(const QUrl &url)
{
    try {
        QString sNodeName = QHostInfo::localHostName();
        SetApplicationURI(QString("urn:%1:%2:%3").arg(
                              sNodeName).arg("qt-project").arg("QOpcUaClient").toStdString());
        SetProductURI("urn:qt-project:QOpcUaClient");
        SetSessionName(GetApplicationURI());

        Connect(url.toString().toStdString());

        // Check connection status by getting the root node
        GetRootNode();
    } catch (const std::exception &e) {
        // FreeOPCUA does not expose the error code, the only information is in ex.what()
        const QString errorString = QString::fromUtf8(e.what());
        QOpcUaClient::ClientError error = QOpcUaClient::UnknownError;
        if (errorString.contains(QStringLiteral("0x801f0000")))
            error = QOpcUaClient::AccessDenied;
        try {
            Disconnect();
        } catch (const std::exception &e) {
            qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "Disconnect failed";
            qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << e.what();
        }
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "Client could not connect to endpoint" << url;
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << e.what();
        emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, error);
        return;
    }

    emit stateAndOrErrorChanged(QOpcUaClient::Connected, QOpcUaClient::NoError);
}

void QFreeOpcUaWorker::asyncDisconnectFromEndpoint()
{
    try {
        Disconnect();
        emit m_client->stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::NoError);
        return;
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA) << "Could not disconnect from endpoint: " << ex.what();
    }

    emit m_client->stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::UnknownError);
}

void QFreeOpcUaWorker::readAttributes(uintptr_t handle, OpcUa::NodeId id, QOpcUaNode::NodeAttributes attr)
{
    QVector<QOpcUaReadResult> vec;

    try {
        OpcUa::ReadParameters params;
        OpcUa::ReadValueId attribute;
        attribute.NodeId = id;

        qt_forEachAttribute(attr, [&](QOpcUaNode::NodeAttribute attr) {
            attribute.AttributeId = QFreeOpcUaValueConverter::toUaAttributeId(attr);
            params.AttributesToRead.push_back(attribute);
            QOpcUaReadResult temp;
            temp.attributeId = attr;
            vec.push_back(temp);
        });

        std::vector<OpcUa::DataValue> res = GetRootNode().GetServices()->Attributes()->Read(params);

        for (size_t i = 0; i < res.size(); ++i) {
            vec[i].statusCode = static_cast<QOpcUa::UaStatusCode>(res[i].Status);
            if (res[i].Status == OpcUa::StatusCode::Good) {
                vec[i].value = QFreeOpcUaValueConverter::toQVariant(res[i].Value);
            }
        }

        emit attributesRead(handle, vec, QOpcUa::UaStatusCode::Good);
    } catch(const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Batch read of node attributes failed: %s", ex.what());
        emit attributesRead(handle, vec, QFreeOpcUaValueConverter::exceptionToStatusCode(ex));
    }
}

void QFreeOpcUaWorker::writeAttribute(uintptr_t handle, OpcUa::Node node, QOpcUaNode::NodeAttribute attr, QVariant value, QOpcUa::Types type)
{
    std::vector<OpcUa::StatusCode> res;

    try {
        if (type == QOpcUa::Types::Undefined && attr != QOpcUaNode::NodeAttribute::Value)
            type = attributeIdToTypeId(attr);
        OpcUa::Variant toWrite = QFreeOpcUaValueConverter::toTypedVariant(value, type);

        OpcUa::WriteValue val;
        val.NodeId = node.GetId();
        val.AttributeId = QFreeOpcUaValueConverter::toUaAttributeId(attr);
        val.Value = OpcUa::DataValue(toWrite);
        std::vector<OpcUa::WriteValue> req;
        req.push_back(val);

        res = node.GetServices()->Attributes()->Write(req);

        emit attributeWritten(handle, attr, res[0] == OpcUa::StatusCode::Good ? value : QVariant(), static_cast<QOpcUa::UaStatusCode>(res[0]));
    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Could not write value to node: %s: %s", OpcUa::ToString(node.GetId()).c_str(), ex.what());
        emit attributeWritten(handle, attr, QVariant(), QFreeOpcUaValueConverter::exceptionToStatusCode(ex));
    }
}

void QFreeOpcUaWorker::writeAttributes(uintptr_t handle, OpcUa::Node node, QOpcUaNode::AttributeMap toWrite, QOpcUa::Types valueAttributeType)
{
    if (toWrite.size() == 0) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "No values to be written");
        emit attributeWritten(handle, QOpcUaNode::NodeAttribute::None, QVariant(), QOpcUa::UaStatusCode::BadNothingToDo);
        return;
    }

    std::vector<OpcUa::StatusCode> res;

    try {
        std::vector<OpcUa::WriteValue> req;

        for (auto it = toWrite.constBegin(); it != toWrite.constEnd(); ++it) {
            OpcUa::WriteValue val;
            val.NodeId = node.GetId();
            val.AttributeId = QFreeOpcUaValueConverter::toUaAttributeId(it.key());
            QOpcUa::Types type = it.key() == QOpcUaNode::NodeAttribute::Value ? valueAttributeType : attributeIdToTypeId(it.key());
            val.Value = OpcUa::DataValue(QFreeOpcUaValueConverter::toTypedVariant(it.value(), type));
            req.push_back(val);
        }

        res = node.GetServices()->Attributes()->Write(req);

        size_t index = 0;
        for (auto it = toWrite.constBegin(); it != toWrite.constEnd(); ++it, ++index) {
            emit attributeWritten(handle, it.key(), res[index] == OpcUa::StatusCode::Good ? it.value() : QVariant(), static_cast<QOpcUa::UaStatusCode>(res[index]));
        }

    } catch (const std::exception &ex) {
        qCWarning(QT_OPCUA_PLUGINS_FREEOPCUA, "Could not write attributes: %s", ex.what());
        for (auto it = toWrite.constBegin(); it != toWrite.constEnd(); ++it) {
            emit attributeWritten(handle, it.key(), QVariant(), QFreeOpcUaValueConverter::exceptionToStatusCode(ex));
        }
    }
}

QOpcUaSubscription *QFreeOpcUaWorker::createSubscription(quint32 interval)
{
    QFreeOpcUaSubscription *backendSubscription = new QFreeOpcUaSubscription(this, interval);
    QOpcUaSubscription *subscription = new QOpcUaSubscription(backendSubscription, interval);
    backendSubscription->m_qsubscription = subscription;
    return subscription;
}

QT_END_NAMESPACE

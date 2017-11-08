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

#include "qopen62541backend.h"
#include "qopen62541node.h"
#include "qopen62541valueconverter.h"
#include <private/qopcuaclient_p.h>

#include <QtCore/qloggingcategory.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qurl.h>
#include <QtCore/quuid.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_OPEN62541)

struct UaVariantMemberDeleter
{
    static void cleanup(UA_Variant *p) { UA_Variant_deleteMembers(p); }
};

struct UaLocalizedTextMemberDeleter
{
    static void cleanup(UA_LocalizedText *p) { UA_LocalizedText_deleteMembers(p); }
};

Open62541AsyncBackend::Open62541AsyncBackend(QOpen62541Client *parent)
    : QOpcUaBackend()
    , m_clientImpl(parent)
    , m_uaclient(nullptr)
    , m_subscriptionTimer(nullptr)
{
}

void Open62541AsyncBackend::readAttributes(uintptr_t handle, UA_NodeId id, QOpcUaNode::NodeAttributes attr)
{
    UA_ReadRequest req;
    UA_ReadRequest_init(&req);
    QVector<UA_ReadValueId> valueIds;

    UA_ReadValueId readId;
    UA_ReadValueId_init(&readId);
    readId.nodeId = id;

    QVector<QOpcUaReadResult> vec;

    for (uint i = 0; i < nodeAttributeEnumBits(); ++i) {
        if (attr & (1 << i)) {
            readId.attributeId = QOpen62541ValueConverter::toUaAttributeId(static_cast<QOpcUaNode::NodeAttribute>(1 << i));
            valueIds.push_back(readId);
            QOpcUaReadResult temp;
            temp.attributeId = static_cast<QOpcUaNode::NodeAttribute>(1 << i);
            vec.push_back(temp);
        }
    }

    UA_ReadResponse res;
    UA_ReadResponse_init(&res);
    req.nodesToRead = valueIds.data();
    req.nodesToReadSize = valueIds.size();

    res = UA_Client_Service_read(m_uaclient, req);

    for (size_t i = 0; i < res.resultsSize; ++i) {
        if (res.results[i].hasStatus)
            vec[i].statusCode = static_cast<QOpcUa::UaStatusCode>(res.results[i].status);
        else
            vec[i].statusCode = QOpcUa::UaStatusCode::Good;
        if (res.results[i].hasValue && res.results[i].value.data)
                vec[i].value = QOpen62541ValueConverter::toQVariant(res.results[i].value);
    }
    emit attributesRead(handle, vec, static_cast<QOpcUa::UaStatusCode>(res.responseHeader.serviceResult));
    UA_ReadResponse_deleteMembers(&res);
    UA_NodeId_deleteMembers(&id);
}

bool Open62541AsyncBackend::writeNodeValueAttribute(UA_NodeId id, const QVariant &value, QOpcUa::Types type)
{
    UA_Variant val = QOpen62541ValueConverter::toOpen62541Variant(value, type);
    QScopedPointer<UA_Variant, UaVariantMemberDeleter> vs(&val);
    UA_StatusCode ret = UA_Client_writeValueAttribute(m_uaclient, id, &val);
    if (ret != UA_STATUSCODE_GOOD) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Open62541: Could not write Value Attribute:" << ret;
        return false;
    }

    return true;
}

static UA_StatusCode nodeIter(UA_NodeId childId, UA_Boolean isInverse, UA_NodeId referenceTypeId, void *pass)
{
    Q_UNUSED(referenceTypeId);
    if (isInverse)
        return UA_STATUSCODE_GOOD;

    // ### TODO: Question: Is it actually correct to skip these
    UA_NodeId temp = UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE);
    if (UA_NodeId_equal(&childId, &temp))
        return UA_STATUSCODE_GOOD;

    temp = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE);
    if (UA_NodeId_equal(&childId, &temp))
        return UA_STATUSCODE_GOOD;

    auto back = static_cast<QStringList *>(pass);

    QString childName;
    if (childId.identifierType == UA_NODEIDTYPE_NUMERIC) {
        childName = QStringLiteral("ns=%1;i=%2").arg(childId.namespaceIndex).arg(childId.identifier.numeric);
    } else if (childId.identifierType == UA_NODEIDTYPE_STRING) {
        childName = QStringLiteral("ns=%1;s=%2").arg(childId.namespaceIndex).arg(
                    QString::fromUtf8(reinterpret_cast<char *>(childId.identifier.string.data), childId.identifier.string.length));
    } else if (childId.identifierType == UA_NODEIDTYPE_GUID) {
        UA_Guid tempId = childId.identifier.guid;
        const QUuid uuid(tempId.data1, tempId.data2, tempId.data3, tempId.data4[0], tempId.data4[1], tempId.data4[2],
                tempId.data4[3], tempId.data4[4], tempId.data4[5], tempId.data4[6], tempId.data4[7]);
        childName = QStringLiteral("ns=%1;g=").arg(childId.namespaceIndex).append(uuid.toString().midRef(1, 36)); // Remove enclosing {...}
    } else if (childId.identifierType == UA_NODEIDTYPE_BYTESTRING) {
        const QString base64String = QByteArray(reinterpret_cast<char *>(childId.identifier.byteString.data),
                                          childId.identifier.byteString.length).toBase64();
        childName = QStringLiteral("ns=%1;b=").arg(childId.namespaceIndex).append(base64String);
    }
    else {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Skipping child with unsupported nodeid type";
    }

    if (!childName.isEmpty())
        back->append(childName);

    return UA_STATUSCODE_GOOD;
}

QStringList Open62541AsyncBackend::childrenIds(const UA_NodeId *parentNode)
{
    QStringList result;
    UA_StatusCode sc = UA_Client_forEachChildNodeCall(m_uaclient, *parentNode, nodeIter, &result);
    if (sc != UA_STATUSCODE_GOOD)
        return QStringList();

    return result;
}

UA_UInt32 Open62541AsyncBackend::createSubscription(int interval)
{
    UA_UInt32 result;
    UA_SubscriptionSettings settings = UA_SubscriptionSettings_default;
    settings.requestedPublishingInterval = interval;
    UA_StatusCode ret = UA_Client_Subscriptions_new(m_uaclient, settings, &result);
    if (ret != UA_STATUSCODE_GOOD)
        return 0;
    return result;
}

void Open62541AsyncBackend::deleteSubscription(UA_UInt32 id)
{
    UA_StatusCode ret = UA_Client_Subscriptions_remove(m_uaclient, id);
    if (ret != UA_STATUSCODE_GOOD)
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "QOpcUa::Open62541: Could not remove subscription";
}

void Open62541AsyncBackend::connectToEndpoint(const QUrl &url)
{
    m_uaclient = UA_Client_new(UA_ClientConfig_default);
    UA_StatusCode ret;

    if (url.userName().length()) {
        QUrl temp = url;
        const QString userName = temp.userName();
        const QString password = temp.password();
        temp.setPassword(QString());
        temp.setUserName(QString());
        ret = UA_Client_connect_username(m_uaclient, temp.toString().toUtf8().constData(), userName.toUtf8().constData(), password.toUtf8().constData());
    } else {
        ret = UA_Client_connect(m_uaclient, url.toString().toUtf8().constData());
    }

    if (ret != UA_STATUSCODE_GOOD) {
        UA_Client_delete(m_uaclient);
        m_uaclient = nullptr;
        QOpcUaClient::ClientError error = ret == UA_STATUSCODE_BADUSERACCESSDENIED ? QOpcUaClient::AccessDenied : QOpcUaClient::UnknownError;
        emit m_clientImpl->stateAndOrErrorChanged(QOpcUaClient::Disconnected, error);
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Open62541: Failed to connect.");
        return;
    }

    if (!m_subscriptionTimer) {
        m_subscriptionTimer = new QTimer(this);
        m_subscriptionTimer->setInterval(5000);
        QObject::connect(m_subscriptionTimer, &QTimer::timeout,
                         this, &Open62541AsyncBackend::updatePublishSubscriptionRequests);
    }
    emit stateAndOrErrorChanged(QOpcUaClient::Connected, QOpcUaClient::NoError);
}

void Open62541AsyncBackend::disconnectFromEndpoint()
{
    UA_StatusCode ret = UA_Client_disconnect(m_uaclient);
    if (ret != UA_STATUSCODE_GOOD) {
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541, "Open62541: Failed to disconnect.");
        // Fall through intentionally
    }

    UA_Client_delete(m_uaclient);
    m_uaclient = nullptr;
    m_subscriptionTimer->stop();
    emit stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::NoError);
}

void Open62541AsyncBackend::updatePublishSubscriptionRequests() const
{
    if (m_uaclient)
        UA_Client_Subscriptions_manuallySendPublishRequest(m_uaclient);
}

void Open62541AsyncBackend::activateSubscriptionTimer(int timeout)
{
    // ### TODO: Check all available subscriptions and their timeout value
    // Get minimum value
    if (timeout <= 0)
        return;

    m_subscriptionIntervals.insert(timeout);

    int minInterval = timeout;
    for (auto it : m_subscriptionIntervals) {
        if (it < minInterval)
            minInterval = it;
    }
    // Update subscriptionTimer timeout
    m_subscriptionTimer->setInterval(minInterval);
    // Start / Stop timer
    m_subscriptionTimer->start();
}

void Open62541AsyncBackend::removeSubscriptionTimer(int timeout)
{
    if (!m_subscriptionIntervals.remove(timeout))
        qCWarning(QT_OPCUA_PLUGINS_OPEN62541) << "Trying to remove non-existent interval.";
    if (m_subscriptionIntervals.isEmpty())
        m_subscriptionTimer->stop();
}

QT_END_NAMESPACE

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
#include "qopen62541valueconverter.h"

#include <QtCore/QStringList>
#include <QtCore/QUrl>

#include <QtOpcUa/private/qopcuaclient_p.h>

struct UaVariantMemberDeleter
{
    static void cleanup(UA_Variant *p) { UA_Variant_deleteMembers(p); }
};

struct UaLocalizedTextMemberDeleter
{
    static void cleanup(UA_LocalizedText *p) { UA_LocalizedText_deleteMembers(p); }
};


Open62541AsyncBackend::Open62541AsyncBackend(QOpen62541Client *parent)
    : QObject()
    , m_clientImpl(parent)
    , m_uaclient(nullptr)
    , m_subscriptionTimer(nullptr)
{
}

QString Open62541AsyncBackend::resolveNodeNameById(UA_NodeId id)
{
    UA_LocalizedText type;
    UA_LocalizedText_init(&type);
    QScopedPointer<UA_LocalizedText, UaLocalizedTextMemberDeleter> ts(&type);
    UA_StatusCode ret = UA_Client_readDisplayNameAttribute(m_uaclient, id, &type);
    if (ret != UA_STATUSCODE_GOOD) {
        qWarning() << "Could not read display name attribute:" << ret;
        return QString();
    }

    return QOpen62541ValueConverter::toQString(type.text);
}

QOpcUaNode::NodeClass Open62541AsyncBackend::resolveNodeClassAttribute(UA_NodeId id)
{
    UA_NodeClass nodeClass;

    // ### TODO: We cannot use the high level function as the test server returns an int32
    // Instead of a NodeClass object, hence UA_TYPES[UA_TYPES_NODECLASS] fails for
    // res->value.type == outDataType in __UA_Client_readAttribute.
    // Should this be fixed upstream? Or is this related to test setup infrastructure?

    // UA_StatusCode ret = UA_Client_readNodeClassAttribute(m_uaclient, id, &nodeClass);
    UA_StatusCode ret = __UA_Client_readAttribute(m_uaclient, &id, UA_ATTRIBUTEID_NODECLASS,
                                     &nodeClass, &UA_TYPES[UA_TYPES_INT32]);


    if (ret != UA_STATUSCODE_GOOD)
        return QOpcUaNode::NodeClass::Undefined;

    switch (nodeClass) {
    case UA_NODECLASS_OBJECT:
        return QOpcUaNode::NodeClass::Object;
    case UA_NODECLASS_VARIABLE:
        return QOpcUaNode::NodeClass::Variable;
    case UA_NODECLASS_METHOD:
        return QOpcUaNode::NodeClass::Method;
    case UA_NODECLASS_OBJECTTYPE:
        return QOpcUaNode::NodeClass::ObjectType;
    case UA_NODECLASS_VARIABLETYPE:
        return QOpcUaNode::NodeClass::VariableType;
    case UA_NODECLASS_REFERENCETYPE:
        return QOpcUaNode::NodeClass::ReferenceType;
    case UA_NODECLASS_DATATYPE:
        return QOpcUaNode::NodeClass::DataType;
    case UA_NODECLASS_VIEW:
        return QOpcUaNode::NodeClass::View;
    default:
        return QOpcUaNode::NodeClass::Undefined;
    }
}

QVariant Open62541AsyncBackend::readNodeValueAttribute(UA_NodeId id)
{
    UA_Variant value;
    UA_Variant_init(&value);
    QScopedPointer<UA_Variant, UaVariantMemberDeleter> vs(&value);

    UA_StatusCode ret = UA_Client_readValueAttribute(m_uaclient, id, &value);
    if (ret != UA_STATUSCODE_GOOD)
        return QVariant();

    if (!value.type->builtin) {
        qWarning() << "Open62541: only builtin types are currently supported!";
        return QVariant();
    }

    return QOpen62541ValueConverter::toQVariant(value);
}

QOpcUa::Types Open62541AsyncBackend::readNodeValueType(UA_NodeId id)
{
    UA_Variant value;
    UA_Variant_init(&value);
    QScopedPointer<UA_Variant, UaVariantMemberDeleter> vs(&value);

    UA_StatusCode ret = UA_Client_readValueAttribute(m_uaclient, id, &value);
    if (ret != UA_STATUSCODE_GOOD)
        return QOpcUa::Types::Undefined;

    switch (value.type->typeIndex) {
    case UA_TYPES_BOOLEAN:
        return QOpcUa::Types::Boolean;
    case UA_TYPES_SBYTE:
        return QOpcUa::Types::SByte;
    case UA_TYPES_BYTE:
        return QOpcUa::Types::Byte;
    case UA_TYPES_INT16:
        return QOpcUa::Types::Int16;
    case UA_TYPES_UINT16:
        return QOpcUa::Types::UInt16;
    case UA_TYPES_INT32:
        return QOpcUa::Types::Int32;
    case UA_TYPES_UINT32:
        return QOpcUa::Types::UInt32;
    case UA_TYPES_INT64:
        return QOpcUa::Types::Int64;
    case UA_TYPES_UINT64:
        return QOpcUa::Types::UInt64;
    case UA_TYPES_FLOAT:
        return QOpcUa::Types::Float;
    case UA_TYPES_DOUBLE:
        return QOpcUa::Types::Double;
    case UA_TYPES_BYTESTRING:
        return QOpcUa::Types::ByteString;
    case UA_TYPES_STRING:
        return QOpcUa::Types::String;
    case UA_TYPES_LOCALIZEDTEXT:
        return QOpcUa::Types::LocalizedText;
    case UA_TYPES_NODEID:
        return QOpcUa::Types::NodeId;
    case UA_TYPES_DATETIME:
        return QOpcUa::Types::DateTime;
    default:
        qWarning() << "Type resolution failed for typeIndex:" << value.type->typeIndex;
    }
    return QOpcUa::Types::Undefined;
}

bool Open62541AsyncBackend::writeNodeValueAttribute(UA_NodeId id, const QVariant &value, QOpcUa::Types type)
{
    UA_Variant val = QOpen62541ValueConverter::toOpen62541Variant(value, type);
    QScopedPointer<UA_Variant, UaVariantMemberDeleter> vs(&val);
    UA_StatusCode ret = UA_Client_writeValueAttribute(m_uaclient, id, &val);
    if (ret != UA_STATUSCODE_GOOD) {
        qWarning() << "Open62541: Could not write Value Attribute:" << ret;
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
        childName = QString::fromLatin1("ns=%1;i=%2").arg(childId.namespaceIndex).arg(childId.identifier.numeric);
    } else if (childId.identifierType == UA_NODEIDTYPE_STRING) {
        childName = QString::fromUtf8("ns=%1;s=%2").arg(childId.namespaceIndex).arg(
                    QString::fromUtf8(reinterpret_cast<char *>(childId.identifier.string.data), childId.identifier.string.length));
    }
    else {
        qWarning() << "Skipping child with unsupported nodeid type";
    }

    if (!childName.isEmpty())
        back->append(childName);

    return UA_STATUSCODE_GOOD;
}

QStringList Open62541AsyncBackend::childrenIds(const UA_NodeId *parentNode)
{
    QStringList result;
    UA_StatusCode sc = UA_Client_forEachChildNodeCall(m_uaclient, *parentNode, nodeIter, (void *) &result);
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
        qWarning() << "QOpcUa::Open62541: Could not remove subscription";
}

void Open62541AsyncBackend::connectToEndpoint(const QUrl &url)
{
    m_uaclient = UA_Client_new(UA_ClientConfig_default);

    UA_StatusCode ret = UA_Client_connect(m_uaclient, url.toString().toLatin1().constData());
    if (ret != UA_STATUSCODE_GOOD) {
        UA_Client_delete(m_uaclient);
        m_uaclient = nullptr;
        emit m_clientImpl->stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::UnknownError);
        qWarning("Open62541: Failed to connect.");
        return;
    }

    if (!m_subscriptionTimer) {
        m_subscriptionTimer = new QTimer(this);
        m_subscriptionTimer->setInterval(5000);
        QObject::connect(m_subscriptionTimer, &QTimer::timeout,
                         this, &Open62541AsyncBackend::updatePublishSubscriptionRequests);
    }
    emit m_clientImpl->stateAndOrErrorChanged(QOpcUaClient::Connected, QOpcUaClient::NoError);
}

void Open62541AsyncBackend::disconnectFromEndpoint()
{
    UA_StatusCode ret = UA_Client_disconnect(m_uaclient);
    if (ret != UA_STATUSCODE_GOOD) {
        qWarning("Open62541: Failed to disconnect.");
        // Fall through intentionally
    }

    UA_Client_delete(m_uaclient);
    m_uaclient = nullptr;
    m_subscriptionTimer->stop();
    emit m_clientImpl->stateAndOrErrorChanged(QOpcUaClient::Disconnected, QOpcUaClient::NoError);
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
        qWarning() << "Trying to remove non-existent interval.";
    if (m_subscriptionIntervals.isEmpty())
        m_subscriptionTimer->stop();
}


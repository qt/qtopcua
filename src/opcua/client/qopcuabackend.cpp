// Copyright (C) 2017 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/qopcuabackend_p.h>

QT_BEGIN_NAMESPACE

QOpcUaBackend::QOpcUaBackend()
    : QObject()
{}

QOpcUaBackend::~QOpcUaBackend()
{}

// All attributes except Value have a fixed type.
// A mapping between attribute id and type can be used to simplify the API for writing multiple attributes at once.
QOpcUa::Types QOpcUaBackend::attributeIdToTypeId(QOpcUa::NodeAttribute attr)
{
    switch (attr) {
    case QOpcUa::NodeAttribute::NodeId:
    case QOpcUa::NodeAttribute::DataType:
        return QOpcUa::Types::NodeId;
//    case QOpcUa::NodeAttribute::NodeClass: TODO: Add support for the NodeClass type
//        return QOpcUa::Types::NodeClass;
    case QOpcUa::NodeAttribute::BrowseName:
        return QOpcUa::Types::QualifiedName;
    case QOpcUa::NodeAttribute::DisplayName:
    case QOpcUa::NodeAttribute::Description:
    case QOpcUa::NodeAttribute::InverseName:
        return QOpcUa::Types::LocalizedText;
    case QOpcUa::NodeAttribute::WriteMask:
    case QOpcUa::NodeAttribute::UserWriteMask:
    case QOpcUa::NodeAttribute::ValueRank:
    case QOpcUa::NodeAttribute::ArrayDimensions:
        return QOpcUa::Types::UInt32;
    case QOpcUa::NodeAttribute::IsAbstract:
    case QOpcUa::NodeAttribute::Symmetric:
    case QOpcUa::NodeAttribute::ContainsNoLoops:
    case QOpcUa::NodeAttribute::Historizing:
    case QOpcUa::NodeAttribute::Executable:
    case QOpcUa::NodeAttribute::UserExecutable:
        return QOpcUa::Types::Boolean;
    case QOpcUa::NodeAttribute::EventNotifier:
    case QOpcUa::NodeAttribute::AccessLevel:
    case QOpcUa::NodeAttribute::UserAccessLevel:
        return QOpcUa::Types::Byte;
    case QOpcUa::NodeAttribute::MinimumSamplingInterval:
        return QOpcUa::Types::Double;
     default:
        return QOpcUa::Types::Undefined;
    }
}

double QOpcUaBackend::revisePublishingInterval(double requestedValue, double minimumValue)
{
    return (std::max)(requestedValue, minimumValue);
}

/*!
   This function returns if a given endpoint description is valid.
   If \a message is not \nullptr, an error message will be assigned to it, in case
   the endpoint description is invalid.
 */
bool QOpcUaBackend::verifyEndpointDescription(const QOpcUaEndpointDescription &endpoint, QString *message)
{
    if (endpoint.endpointUrl().isEmpty()) {
        if (message)
            *message = QLatin1String("Endpoint description is invalid because endpoint URL is empty");
        return false;
    }

    if (endpoint.securityPolicy().isEmpty()) {
        if (message)
            *message = QLatin1String("Endpoint description is invalid because security policy is empty");
        return false;
    }

    if (endpoint.securityMode() != QOpcUaEndpointDescription::MessageSecurityMode::None &&
              endpoint.securityMode() != QOpcUaEndpointDescription::MessageSecurityMode::Sign &&
              endpoint.securityMode() != QOpcUaEndpointDescription::MessageSecurityMode::SignAndEncrypt)
    {
        if (message)
            *message = QLatin1String("Endpoint description contains an invalid message security mode");
        return false;
    }

    return true;
}

QT_END_NAMESPACE

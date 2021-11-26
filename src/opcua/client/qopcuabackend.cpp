/****************************************************************************
**
** Copyright (C) 2017 basysKom GmbH, opensource@basyskom.com
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

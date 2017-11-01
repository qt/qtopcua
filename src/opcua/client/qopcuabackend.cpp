/****************************************************************************
**
** Copyright (C) 2017 basysKom GmbH, opensource@basyskom.com
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

#include <private/qopcuabackend_p.h>

QT_BEGIN_NAMESPACE

QOpcUaBackend::QOpcUaBackend()
    : QObject()
{}

QOpcUaBackend::~QOpcUaBackend()
{}

// All attributes except Value have a fixed type.
// A mapping between attribute id and type can be used to simplify the API for writing multiple attributes at once.
QOpcUa::Types QOpcUaBackend::attributeIdToTypeId(QOpcUaNode::NodeAttribute attr)
{
    switch (attr) {
    case QOpcUaNode::NodeAttribute::NodeId:
    case QOpcUaNode::NodeAttribute::DataType:
        return QOpcUa::Types::NodeId;
//    case QOpcUaNode::NodeAttribute::NodeClass: TODO: Add support for the NodeClass type
//        return QOpcUa::Types::NodeClass;
    case QOpcUaNode::NodeAttribute::BrowseName:
        return QOpcUa::Types::QualifiedName;
    case QOpcUaNode::NodeAttribute::DisplayName:
    case QOpcUaNode::NodeAttribute::Description:
    case QOpcUaNode::NodeAttribute::InverseName:
        return QOpcUa::Types::LocalizedText;
    case QOpcUaNode::NodeAttribute::WriteMask:
    case QOpcUaNode::NodeAttribute::UserWriteMask:
    case QOpcUaNode::NodeAttribute::ValueRank:
    case QOpcUaNode::NodeAttribute::ArrayDimensions:
        return QOpcUa::Types::UInt32;
    case QOpcUaNode::NodeAttribute::IsAbstract:
    case QOpcUaNode::NodeAttribute::Symmetric:
    case QOpcUaNode::NodeAttribute::ContainsNoLoops:
    case QOpcUaNode::NodeAttribute::Historizing:
    case QOpcUaNode::NodeAttribute::Executable:
    case QOpcUaNode::NodeAttribute::UserExecutable:
        return QOpcUa::Types::Boolean;
    case QOpcUaNode::NodeAttribute::EventNotifier:
    case QOpcUaNode::NodeAttribute::AccessLevel:
    case QOpcUaNode::NodeAttribute::UserAccessLevel:
        return QOpcUa::Types::Byte;
    case QOpcUaNode::NodeAttribute::MinimumSamplingInterval:
        return QOpcUa::Types::Double;
     default:
        return QOpcUa::Types::Undefined;
    }
}

QT_END_NAMESPACE

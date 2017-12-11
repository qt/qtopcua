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

#ifndef QFREEOPCUAVALUECONVERTER_H
#define QFREEOPCUAVALUECONVERTER_H

#include <QtOpcUa/qopcuanode.h>
#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>

#include <vector>

#include <opc/ua/client/client.h>
#include <opc/ua/protocol/variant.h>

QT_BEGIN_NAMESPACE

namespace QFreeOpcUaValueConverter
{
    OpcUa::Variant toVariant(const QVariant &variant);
    QVariant toQVariant(const OpcUa::Variant &variant);
    OpcUa::Variant toTypedVariant(const QVariant &variant, QOpcUa::Types type);
    QString nodeIdToString(const OpcUa::NodeId &id);

    QOpcUa::UaStatusCode exceptionToStatusCode(const std::exception &ex);

    template <typename UATYPE, typename QTTYPE=UATYPE>
    OpcUa::Variant arrayFromQVariant(const QVariant &var);

    inline OpcUa::AttributeId toUaAttributeId(QOpcUaNode::NodeAttribute attr)
    {
        const int attributeIdUsedBits = 22;
        for (int i = 0; i < attributeIdUsedBits; ++i)
            if (static_cast<int>(attr) == (1 << i))
                return static_cast<OpcUa::AttributeId>(i + 1);

        return OpcUa::AttributeId::Unknown;
    }

    template <typename UATYPE, typename QTTYPE=UATYPE>
    UATYPE scalarFromQVariant(const QVariant &var);

    template<typename QTTYPE, typename UATYPE>
    QTTYPE scalarUaToQt(const UATYPE &data);

    template<typename QTTYPE, typename UATYPE>
    QVariant arrayToQVariant(const OpcUa::Variant &var, QMetaType::Type type = QMetaType::UnknownType);
}

QT_END_NAMESPACE

#endif // QFREEOPCUAVALUECONVERTER_H

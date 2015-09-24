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

#include <QtOpcUa/qopcuatype.h>

#include <opc/ua/protocol/variant.h>

#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>

#include <vector>

QT_BEGIN_NAMESPACE

namespace QFreeOpcUaValueConverter
{
    OpcUa::Variant toVariant(const QVariant &variant);
    QVariant toQVariant(const OpcUa::Variant &variant);
    OpcUa::Variant toTypedVariant(const QVariant &variant, QOpcUa::Types type);
    QString nodeIdToString(const OpcUa::NodeId &id);

    template <typename T> QVariantList getArray(const std::vector<T> &values)
    {
        QVariantList ret;
        ret.reserve(values.size());
        for (typename std::vector<T>::const_iterator it = values.cbegin(); it != values.cend(); ++it)
            ret << *it;
        return ret;
    }

    template <typename T> OpcUa::Variant getArray(const QVariant &variant)
    {
        if (variant.type() == QVariant::Type::List) {
            QVariantList list = variant.toList();
            std::vector<T> vec;
            vec.reserve(list.size());
            for (int i = 0; i < list.size(); ++i)
                vec.push_back(list[i].value<T>());

            return OpcUa::Variant(vec);
        } else {
            return OpcUa::Variant();
        }
    }
}

QT_END_NAMESPACE

#endif // QFREEOPCUAVALUECONVERTER_H

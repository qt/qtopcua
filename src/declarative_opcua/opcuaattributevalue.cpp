/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#include <private/opcuaattributevalue_p.h>

QT_BEGIN_NAMESPACE

/*!
    \class OpcUaAttributeValue
    \inqmlmodule QtOpcUa
    \brief Stores an attribute value and provides a changed signal.
    \internal

    This class is just for internal use in the declarative backend and not exposed to users.

    When setting the value it will emit a changed signal if the value has changed.

    \sa OpcUaAttributeCache
*/

OpcUaAttributeValue::OpcUaAttributeValue(QObject *parent)
    : QObject(parent)
{

}

bool OpcUaAttributeValue::operator ==(const OpcUaAttributeValue &rhs)
{
    return m_value == rhs.m_value;
}

void OpcUaAttributeValue::setValue(const QVariant &value)
{
    if (value.metaType() != m_value.metaType() || value != m_value) {
        m_value = value;
        emit changed(m_value);
    }
}

void OpcUaAttributeValue::invalidate()
{
    setValue(QVariant());
}

const QVariant &OpcUaAttributeValue::value() const
{
    return m_value;
}

OpcUaAttributeValue::operator QVariant() const
{
    return value();
}

QT_END_NAMESPACE

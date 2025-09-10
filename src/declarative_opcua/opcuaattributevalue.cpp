// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

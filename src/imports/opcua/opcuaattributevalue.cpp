/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#include "opcuaattributevalue.h"

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

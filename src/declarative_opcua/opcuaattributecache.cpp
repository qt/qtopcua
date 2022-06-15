// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/opcuaattributecache_p.h>
#include <private/opcuaattributevalue_p.h>

QT_BEGIN_NAMESPACE

/*!
    \class OpcUaAttributeCache
    \inqmlmodule QtOpcUa
    \brief Flexible attribute value cache providing signals.
    \internal

    This class is just for internal use in the declarative backend and not exposed to users.

    It caches node attribute values and provides access. Main purpose is to
    let \l OpcUaAttributeValue provide separate value change signals for each attribute.

    \sa OpcUaAttributeValue
*/

OpcUaAttributeCache::OpcUaAttributeCache(QObject *parent) : QObject(parent)
{
}

void OpcUaAttributeCache::setAttributeValue(QOpcUa::NodeAttribute attr, const QVariant &value)
{
    attribute(attr)->setValue(value);
}

void OpcUaAttributeCache::invalidate()
{
    // Reset all values in the cache to invalid.
    // Do not clear() the cache because there are still objects with
    // connections waiting for notifications
    for (auto i = m_attributeCache.constBegin(); i != m_attributeCache.constEnd(); ++i)
        i.value()->invalidate();
}

OpcUaAttributeValue *OpcUaAttributeCache::attribute(QOpcUa::NodeAttribute attr)
{
    if (!m_attributeCache.contains(attr))
        m_attributeCache.insert(attr, new OpcUaAttributeValue(this));
    return m_attributeCache.value(attr);
}

const QVariant &OpcUaAttributeCache::attributeValue(QOpcUa::NodeAttribute attr)
{
    return attribute(attr)->value();
}

QT_END_NAMESPACE

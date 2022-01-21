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

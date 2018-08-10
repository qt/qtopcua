/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#include "opcuaattributecache.h"
#include "opcuaattributevalue.h"

QT_BEGIN_NAMESPACE

/*!
    \class OpcUaAttributeCache
    \inqmlmodule QtOpcUa
    \brief Flexible attribute value cache providing signals.
    \internal

    This class is just for internal use in the declarative backend and not exposed to users.

    It caches node attribute values and provides accesss. Main purpose is to
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

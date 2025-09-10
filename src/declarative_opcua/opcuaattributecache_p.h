// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUA_OPCUAATTRIBUTECACHE_P_H
#define QOPCUA_OPCUAATTRIBUTECACHE_P_H

#include "qopcuatype.h"
#include <QtCore/qobject.h>
#include <private/qglobal_p.h>
#include <QtCore/qhash.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//


QT_BEGIN_NAMESPACE

class OpcUaAttributeValue;

class OpcUaAttributeCache : public QObject
{
    Q_OBJECT
public:
    explicit OpcUaAttributeCache(QObject *parent = nullptr);
    OpcUaAttributeValue *attribute(QOpcUa::NodeAttribute attribute);
    const QVariant &attributeValue(QOpcUa::NodeAttribute);

public slots:
    void setAttributeValue(QOpcUa::NodeAttribute attribute, const QVariant &value);
    void invalidate();

private:
    QHash<QOpcUa::NodeAttribute, OpcUaAttributeValue *> m_attributeCache;
};

QT_END_NAMESPACE

#endif // QOPCUA_OPCUAATTRIBUTECACHE_P_H

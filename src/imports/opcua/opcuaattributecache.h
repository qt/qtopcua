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

#pragma once

#include "qopcuatype.h"
#include <QObject>

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

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

class OpcUaMethodArgument : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue)
    Q_PROPERTY(QOpcUa::Types type READ type WRITE setType)

public:
    explicit OpcUaMethodArgument(QObject *parent = nullptr);
    QVariant value() const;
    QOpcUa::Types type() const;

public slots:
    void setValue(QVariant value);
    void setType(QOpcUa::Types type);

private:
    QVariant m_value;
    QOpcUa::Types m_type;
};

QT_END_NAMESPACE

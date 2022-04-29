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

#include <QObject>
#include <QVariant>

QT_BEGIN_NAMESPACE

class OpcUaAttributeValue : public QObject
{
    Q_OBJECT
public:
    explicit OpcUaAttributeValue(QObject *parent);
    bool operator ==(const OpcUaAttributeValue &rhs);
    void setValue(const QVariant &value);
    void invalidate();
    const QVariant &value() const;
    operator QVariant() const;

signals:
    void changed(QVariant value);

private:
    QVariant m_value;
};

QT_END_NAMESPACE

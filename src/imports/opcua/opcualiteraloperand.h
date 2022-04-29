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

#ifndef OPCUALITERALOPERAND
#define OPCUALITERALOPERAND

#include "opcuaoperandbase.h"
#include <QOpcUaLiteralOperand>
#include <qopcuatype.h>

QT_BEGIN_NAMESPACE

class OpcUaLiteralOperand : public OpcUaOperandBase {
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue)
    Q_PROPERTY(QOpcUa::Types type READ type WRITE setType)

public:
    explicit OpcUaLiteralOperand(QObject *parent = nullptr);
    ~OpcUaLiteralOperand();
    QVariant toCppVariant(QOpcUaClient *client) const override;

    QVariant value() const;
    void setValue(const QVariant &value);

    QOpcUa::Types type() const;
    void setType(QOpcUa::Types type);

signals:
    void dataChanged();

private:
    QVariant m_value;
    QOpcUa::Types m_type;
};

QT_END_NAMESPACE

#endif // OPCUALITERALOPERAND

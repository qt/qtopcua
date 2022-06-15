// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef OPCUALITERALOPERAND_P_H
#define OPCUALITERALOPERAND_P_H

#include <private/opcuaoperandbase_p.h>

#include <QOpcUaLiteralOperand>
#include <qopcuatype.h>
#include <QtQml/qqml.h>

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

class OpcUaLiteralOperand : public OpcUaOperandBase {
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue)
    Q_PROPERTY(QOpcUa::Types type READ type WRITE setType)
    QML_NAMED_ELEMENT(LiteralOperand)
    QML_ADDED_IN_VERSION(5, 13)

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

#endif // OPCUALITERALOPERAND_P_H

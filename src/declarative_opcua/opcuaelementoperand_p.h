// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef OPCUAELEMENTOPERAND_P_H
#define OPCUAELEMENTOPERAND_P_H

#include <private/opcuaoperandbase_p.h>
#include <QOpcUaElementOperand>
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

class QOpcUaClient;

class OpcUaElementOperand : public OpcUaOperandBase, private QOpcUaElementOperand {
    Q_OBJECT
    Q_PROPERTY(quint32 index READ index WRITE setIndex)
    QML_NAMED_ELEMENT(ElementOperand)
    QML_ADDED_IN_VERSION(5, 13)

public:
    explicit OpcUaElementOperand(QObject *parent = nullptr);
    ~OpcUaElementOperand();
    virtual QVariant toCppVariant(QOpcUaClient *client) const override;

    quint32 index() const;
    void setIndex(quint32 index);

signals:
    void dataChanged();
};

QT_END_NAMESPACE

#endif // OPCUAELEMENTOPERAND_P_H

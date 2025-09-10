// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUA_OPCUAMETHODARGUMENT_P_H
#define QOPCUA_OPCUAMETHODARGUMENT_P_H

#include "qopcuatype.h"
#include <QObject>
#include <QtQml/qqml.h>
#include <QtCore/private/qglobal_p.h>

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

class OpcUaMethodArgument : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue)
    Q_PROPERTY(QOpcUa::Types type READ type WRITE setType)

    QML_NAMED_ELEMENT(MethodArgument)
    QML_ADDED_IN_VERSION(5, 13)

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

#endif // QOPCUA_OPCUAMETHODARGUMENT_P_H

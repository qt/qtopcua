// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma once

#include <QObject>
#include <QVariant>
#include <private/qglobal_p.h>

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

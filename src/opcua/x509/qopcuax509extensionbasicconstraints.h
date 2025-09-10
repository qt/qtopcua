// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAX509EXTENSIONBASICCONSTRAINTS_H
#define QOPCUAX509EXTENSIONBASICCONSTRAINTS_H

#include "QtOpcUa/qopcuax509extension.h"
#include <QtOpcUa/qopcuaglobal.h>

QT_BEGIN_NAMESPACE

class QOpcUaX509ExtensionBasicConstraintsData;

class Q_OPCUA_EXPORT QOpcUaX509ExtensionBasicConstraints : public QOpcUaX509Extension
{
public:
    QOpcUaX509ExtensionBasicConstraints();
    QOpcUaX509ExtensionBasicConstraints(const QOpcUaX509ExtensionBasicConstraints &);
    QOpcUaX509ExtensionBasicConstraints &operator=(const QOpcUaX509ExtensionBasicConstraints &);
    bool operator==(const QOpcUaX509ExtensionBasicConstraints &rhs) const;
    ~QOpcUaX509ExtensionBasicConstraints();

    void setCa(bool value);
    bool ca() const;

    void setPathLength(int length);
    int pathLength() const;
};

QT_END_NAMESPACE

#endif // QOPCUAX509EXTENSIONBASICCONSTRAINTS_H

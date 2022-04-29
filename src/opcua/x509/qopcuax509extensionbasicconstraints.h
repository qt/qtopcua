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

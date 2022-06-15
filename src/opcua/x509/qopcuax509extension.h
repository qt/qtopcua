// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAX509EXTENSION_H
#define QOPCUAX509EXTENSION_H

#include <QtOpcUa/qopcuaglobal.h>
#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaX509ExtensionData;
class Q_OPCUA_EXPORT QOpcUaX509Extension
{
public:
    QOpcUaX509Extension();
    QOpcUaX509Extension(const QOpcUaX509Extension &);
    QOpcUaX509Extension &operator=(const QOpcUaX509Extension &);
    bool operator==(const QOpcUaX509Extension &rhs) const;
    virtual ~QOpcUaX509Extension();
    void setCritical(bool critical);
    bool critical() const;

protected:
    QOpcUaX509Extension(QOpcUaX509ExtensionData*);
    QOpcUaX509Extension(QSharedDataPointer<QOpcUaX509ExtensionData>);
    QSharedDataPointer<QOpcUaX509ExtensionData> data;
};

QT_END_NAMESPACE

#endif // QOPCUAX509EXTENSION_H

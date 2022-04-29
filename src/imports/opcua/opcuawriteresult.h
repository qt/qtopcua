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

#ifndef OPCUAWRITERESULT_H
#define OPCUAWRITERESULT_H

#include <QObject>
#include <QDateTime>
#include "qopcuatype.h"
#include "opcuastatus.h"

QT_BEGIN_NAMESPACE

class QOpcUaWriteResult;
class QOpcUaClient;
class OpcUaWriteResultData;
class OpcUaWriteResult
{
    Q_GADGET
    Q_PROPERTY(QOpcUa::NodeAttribute attribute READ attribute)
    Q_PROPERTY(QString indexRange READ indexRange)
    Q_PROPERTY(QString nodeId READ nodeId)
    Q_PROPERTY(QString namespaceName READ namespaceName)
    Q_PROPERTY(OpcUaStatus status READ status)

public:
    OpcUaWriteResult();
    OpcUaWriteResult(const OpcUaWriteResult &other);
    OpcUaWriteResult(const QOpcUaWriteResult &other, const QOpcUaClient *client);
    OpcUaWriteResult &operator=(const OpcUaWriteResult &rhs);
    ~OpcUaWriteResult();

    const QString &indexRange() const;
    const QString &nodeId() const;
    QOpcUa::NodeAttribute attribute() const;
    const QString &namespaceName() const;

    OpcUaStatus status() const;

private:
    QSharedDataPointer<OpcUaWriteResultData> data;
};


QT_END_NAMESPACE

Q_DECLARE_METATYPE(OpcUaWriteResult)

#endif // OPCUAWRITERESULT_H

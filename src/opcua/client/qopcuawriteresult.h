/****************************************************************************
**
** Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
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

#ifndef QOPCUAWRITERESULT_H
#define QOPCUAWRITERESULT_H

#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaWriteResultData;
class Q_OPCUA_EXPORT QOpcUaWriteResult
{
public:
    QOpcUaWriteResult();
    QOpcUaWriteResult(const QOpcUaWriteResult &other);
    QOpcUaWriteResult &operator=(const QOpcUaWriteResult &rhs);
    ~QOpcUaWriteResult();

    QString nodeId() const;
    void setNodeId(const QString &nodeId);

    QOpcUa::NodeAttribute attribute() const;
    void setAttribute(QOpcUa::NodeAttribute attribute);

    QString indexRange() const;
    void setIndexRange(const QString &indexRange);

    QOpcUa::UaStatusCode statusCode() const;
    void setStatusCode(QOpcUa::UaStatusCode statusCode);

private:
    QSharedDataPointer<QOpcUaWriteResultData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaWriteResult)

#endif // QOPCUAWRITERESULT_H

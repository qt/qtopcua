/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
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

#ifndef QOPCUACONTENTFILTERELEMENTRESULT_H
#define QOPCUACONTENTFILTERELEMENTRESULT_H

#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaContentFilterElementResultData;
class Q_OPCUA_EXPORT QOpcUaContentFilterElementResult
{
public:
    QOpcUaContentFilterElementResult();
    QOpcUaContentFilterElementResult(const QOpcUaContentFilterElementResult &);
    QOpcUaContentFilterElementResult &operator=(const QOpcUaContentFilterElementResult &);
    ~QOpcUaContentFilterElementResult();

    QOpcUa::UaStatusCode statusCode() const;
    void setStatusCode(QOpcUa::UaStatusCode statusCode);

    QList<QOpcUa::UaStatusCode> operandStatusCodes() const;
    QList<QOpcUa::UaStatusCode> &operandStatusCodesRef();
    void setOperandStatusCodes(const QList<QOpcUa::UaStatusCode> &operandStatusCodes);

private:
    QSharedDataPointer<QOpcUaContentFilterElementResultData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaContentFilterElementResult)

#endif // QOPCUACONTENTFILTERELEMENTRESULT_H

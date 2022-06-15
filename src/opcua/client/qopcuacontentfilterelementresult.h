// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

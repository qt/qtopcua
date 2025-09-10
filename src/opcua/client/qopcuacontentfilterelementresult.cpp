// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuacontentfilterelementresult.h"
#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaContentFilterElementResult
    \inmodule QtOpcUa
    \brief The OPC UA ContentFilterElementResult.

    QOpcUaContentFilterElementResult contains the status code for a
    filter element and all its operands.
*/

class QOpcUaContentFilterElementResultData : public QSharedData
{
public:
    QOpcUa::UaStatusCode statusCode {QOpcUa::UaStatusCode::Good};
    QList<QOpcUa::UaStatusCode> operandStatusCodes;
};

/*!
    Default constructs a content filter element result with no parameters set.
*/
QOpcUaContentFilterElementResult::QOpcUaContentFilterElementResult()
    : data(new QOpcUaContentFilterElementResultData)
{
}

/*!
    Constructs a content filter element result from \a rhs.
*/
QOpcUaContentFilterElementResult::QOpcUaContentFilterElementResult(const QOpcUaContentFilterElementResult &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this content filter element result.
*/
QOpcUaContentFilterElementResult &QOpcUaContentFilterElementResult::operator=(const QOpcUaContentFilterElementResult &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUaContentFilterElementResult::~QOpcUaContentFilterElementResult()
{
}

/*!
    Returns the status code for the filter element.
*/
QOpcUa::UaStatusCode QOpcUaContentFilterElementResult::statusCode() const
{
    return data->statusCode;
}

/*!
    Sets the status code for the filter element to \a statusCode.
*/
void QOpcUaContentFilterElementResult::setStatusCode(QOpcUa::UaStatusCode statusCode)
{
    data->statusCode = statusCode;
}

/*!
    Returns the status codes for all filter operands in the order that was used in the filter.
*/
QList<QOpcUa::UaStatusCode> QOpcUaContentFilterElementResult::operandStatusCodes() const
{
    return data->operandStatusCodes;
}

/*!
    Sets the status codes for all filter operands to \a operandStatusCodes.
*/
void QOpcUaContentFilterElementResult::setOperandStatusCodes(const QList<QOpcUa::UaStatusCode> &operandStatusCodes)
{
    data->operandStatusCodes = operandStatusCodes;
}

/*!
    Returns a reference to the operand status codes.

    \sa operandStatusCodes()
*/
QList<QOpcUa::UaStatusCode> &QOpcUaContentFilterElementResult::operandStatusCodesRef()
{
    return data->operandStatusCodes;
}

QT_END_NAMESPACE

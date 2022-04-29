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

#include "qopcuacontentfilterelementresult.h"

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

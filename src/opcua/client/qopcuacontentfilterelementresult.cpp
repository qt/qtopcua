/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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

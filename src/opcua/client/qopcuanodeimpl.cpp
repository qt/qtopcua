/****************************************************************************
**
** Copyright (C) 2016 basysKom GmbH, opensource@basyskom.com
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

#include <private/qopcuanodeimpl_p.h>

QT_BEGIN_NAMESPACE

QOpcUaNodeImpl::QOpcUaNodeImpl()
    : m_handle{0}
    , m_registered{false}
{
}

QOpcUaNodeImpl::~QOpcUaNodeImpl()
{
}

quint64 QOpcUaNodeImpl::handle() const
{
    return m_handle;
}

void QOpcUaNodeImpl::setHandle(quint64 handle)
{
    m_handle = handle;
}

bool QOpcUaNodeImpl::registered() const
{
    return m_registered;
}

void QOpcUaNodeImpl::setRegistered(bool registered)
{
    m_registered = registered;
}

QT_END_NAMESPACE

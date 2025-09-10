// Copyright (C) 2016 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

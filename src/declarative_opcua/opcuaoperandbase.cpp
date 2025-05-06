// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include <private/opcuaoperandbase_p.h>
#include <QLoggingCategory>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_QML)

OpcUaOperandBase::OpcUaOperandBase(QObject *parent)
    : QObject(parent)
{
}

OpcUaOperandBase::~OpcUaOperandBase() = default;

QVariant OpcUaOperandBase::toCppVariant(QOpcUaClient *client) const
{
    Q_UNUSED(client);
    qCWarning(QT_OPCUA_PLUGINS_QML) << "Calling invalid base function of OpcUaOperandBase";
    return QVariant();
}

QT_END_NAMESPACE

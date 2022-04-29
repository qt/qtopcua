/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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

#ifndef QOPEN62541PLUGIN_H
#define QOPEN62541PLUGIN_H

#include "qopen62541.h"
#include <QtOpcUa/qopcuaplugin.h>

QT_BEGIN_NAMESPACE

class QOpen62541Plugin : public QOpcUaPlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "org.qt-project.qt.opcua.providerfactory/1.0" FILE "open62541-metadata.json")
    Q_INTERFACES(QOpcUaPlugin)

public:
    explicit QOpen62541Plugin(QObject *parent = nullptr);
    ~QOpen62541Plugin() override;

    QOpcUaClient *createClient(const QVariantMap &backendProperties) override;
};

QT_END_NAMESPACE

#endif // QOPEN62541PLUGIN_H

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

#ifndef QOPCUAPLUGIN_H
#define QOPCUAPLUGIN_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

class QOpcUaClient;

#define QOpcUaProviderFactory_iid "org.qt-project.qt.opcua.providerfactory/1.0"

class Q_OPCUA_EXPORT QOpcUaPlugin : public QObject
{
    Q_OBJECT
public:
    explicit QOpcUaPlugin(QObject *parent = nullptr);
    ~QOpcUaPlugin() override;

    virtual QOpcUaClient *createClient(const QVariantMap &backendProperties) = 0;
};
Q_DECLARE_INTERFACE(QOpcUaPlugin, QOpcUaProviderFactory_iid)

QT_END_NAMESPACE

#endif // QOPCUAPLUGIN_H

/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qopcuaprovider.h"

#include "qopcuaplugin.h"

#include <QtOpcUa/qopcuatype.h>

#include <private/qfactoryloader_p.h>

#include <QtCore/qdebug.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qpluginloader.h>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaProvider
    \inmodule QtOpcUa

    \brief QOpcUaProvider creates an instance of QOpcUaClient

    QOpcUaProvider allows the user to create an instance of QOpcUaClient by
    loading a QOpcUaPlugin using the Qt plugin system.

    For the available plugins and their capabilities please refer to the
    \l QOpcUaClient documentation.
*/

#ifndef QT_NO_LIBRARY
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
        (QOpcUaProviderFactory_iid, QLatin1String("/opcua")))
#endif

/*!
    Reads the meta data from the plugins known by the loader.
*/
static QHash<QString, QJsonObject> loadPluginMetadata()
{
    QHash<QString, QJsonObject> plugins;
    const QFactoryLoader *l = loader();
    QList<QJsonObject> const meta = l->metaData();
    for (int i = 0; i < meta.size(); ++i) {
        QJsonObject obj = meta.at(i).value(QStringLiteral("MetaData")).toObject();
        obj.insert(QStringLiteral("index"), i);
        plugins.insertMulti(obj.value(QStringLiteral("Provider")).toString(), obj);
    }
    return plugins;
}

/*!
    Returns a QHash mapping names to JSON objects containing the meta data of
    available plugins.
*/
static QHash<QString, QJsonObject> plugins()
{
    static QHash<QString, QJsonObject> plugins;
    static bool alreadyDiscovered = false;

    if (!alreadyDiscovered) {
        plugins = loadPluginMetadata();
        alreadyDiscovered = true;
    }
    return plugins;
}

/*!
    Creates a new OPC UA provider.
 */
QOpcUaProvider::QOpcUaProvider(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<QOpcUa::Types>();
    qRegisterMetaType<QOpcUa::TypedVariant>();
}

QOpcUaProvider::~QOpcUaProvider()
{
    qDeleteAll(m_plugins);
}

static QOpcUaPlugin *loadPlugin(const QString &key)
{
    const int index = loader()->indexOf(key);
    if (index != -1) {
        QObject *factoryObject = loader->instance(index);
        if (QOpcUaPlugin *plugin = qobject_cast<QOpcUaPlugin *>(factoryObject)) {
            return plugin;
        }
    }
    return nullptr;
}

/*!
    Returns a pointer to a QOpcUaClient object by loading the selected backend
    as a plugin and creating a client object.
    If the plugin loading fails, 0 is returned instead.

    The user is responsible for deleting the returned \a QOpcUaClient object
    when it is no longer needed.
*/
QOpcUaClient *QOpcUaProvider::createClient(const QString &backend)
{
    QOpcUaPlugin *plugin;
    auto it = m_plugins.find(backend);
    if (it == m_plugins.end()) {
        plugin = loadPlugin(backend);
        if (!plugin) {
            qWarning() << "Failed to load OPC UA plugin:" << backend;
            qWarning() << "Available plugins:" << availableBackends();
            return nullptr;
        }
        m_plugins.insert(backend, plugin);
    }
    else {
        plugin = it.value();
    }
    return plugin->createClient();
}

/*!
    Returns a QStringList of available plugins.
*/
QStringList QOpcUaProvider::availableBackends()
{
    return plugins().keys();
}

QT_END_NAMESPACE

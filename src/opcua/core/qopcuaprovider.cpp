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
static void loadPluginMetadata(QHash<QString, QJsonObject> &list)
{
    const QFactoryLoader *l = loader();
    QList<QJsonObject> const meta = l->metaData();
    for (int i = 0; i < meta.size(); ++i) {
        QJsonObject obj = meta.at(i).value(QStringLiteral("MetaData")).toObject();
        obj.insert(QStringLiteral("index"), i);
        list.insertMulti(obj.value(QStringLiteral("Provider")).toString(), obj);
    }
}

/*!
    Returns a QHash mapping names to JSON objects containing the meta data of
    available plugins.
*/
static QHash<QString, QJsonObject> plugins(bool reload = false)
{
    static QHash<QString, QJsonObject> plugins;
    static bool alreadyDiscovered = false;

    if (reload == true)
        alreadyDiscovered = false;

    if (!alreadyDiscovered) {
        loadPluginMetadata(plugins);
        alreadyDiscovered = true;
    }
    return plugins;
}

/*!
    Returns the name of the best plugin according to the plugin meta data.
*/
static QString findBestPlugin()
{
    int bestStability = -1;
    QString bestPlugin;

    const QList<QJsonObject> meta = loader()->metaData();

    for (int i = 0; i < meta.size(); ++i) {
        const QJsonObject obj = meta.at(i).value(QStringLiteral("MetaData")).toObject();
        if (obj.value(QStringLiteral("stability")).toInt() > bestStability) {
            // FIXME: is "Keys" correct?
            bestPlugin = obj.value(QStringLiteral("Keys")).toArray().first().toString();
        }
    }

    return bestPlugin;
}

/*!
    Creates a new OPC UA provider using the plugin specified in \a backend.
    The value of \a parent is passed on to the QObject constructor.
    In the standard configuration the only useful value is "freeopcua".
    If the requested plugin is not available a diagnostic message will appear
    and creating objects will not be possible.
 */
QOpcUaProvider::QOpcUaProvider(const QString &backend, QObject* parent)
    : QObject(parent)
    , m_pBackend(0)
{
    setBackend(backend);
}

QOpcUaProvider::~QOpcUaProvider()
{
    delete m_pBackend;
}

/*!
    Loads the OPC UA plugin specified in \a key.
    Returns true if the plugin could be loaded, false if not.
*/
bool QOpcUaProvider::loadPlugin(const QString &key)
{
    const int index = loader()->indexOf(key);
    if (index != -1) {
        QObject *factoryObject = loader->instance(index);
        if (QOpcUaPlugin *factory = qobject_cast<QOpcUaPlugin *>(factoryObject)) {
            m_pBackend = factory;
            return true;
        }
    }
    return false;
}

/*!
    Returns the name of the currently selected backend.
*/
QString QOpcUaProvider::backend() const
{
    return m_backendName;
}

void QOpcUaProvider::setBackend(const QString &name)
{
    m_backendName = name;

    if (m_backendName.isEmpty()) {
        // try to find the most stable plugin
        m_backendName = findBestPlugin();
    }
}

/*!
    Returns a pointer to a QOpcUaClient object by loading the selected backend
    as a plugin and creating a client object.
    If the plugin loading fails, 0 is returned instead.

    The user is responsible for deleting the returned \a QOpcUaClient object
    when it is no longer needed.
*/
QOpcUaClient *QOpcUaProvider::createClient()
{
    if (!m_pBackend) {
        // we need a plugin now
        if (!loadPlugin(m_backendName)) {
            qWarning() << "Failed to load OPC UA plugin:" << m_pBackend;
            qWarning() << "Available plugins:" << availableBackends();
        }
    }

    if (m_pBackend)
        return m_pBackend->createClient();

    return 0;
}


/*!
    Returns a QStringList of available plugins.
*/
QStringList QOpcUaProvider::availableBackends()
{
    return plugins().keys();
}

QT_END_NAMESPACE

// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuaplugin.h"
#include "qopcuaprovider.h"
#include <QtOpcUa/qopcuaclient.h>
#include <QtOpcUa/qopcuanode.h>
#include <QtOpcUa/qopcuatype.h>
#include <QtOpcUa/qopcuaapplicationidentity.h>
#include <QtOpcUa/qopcuapkiconfiguration.h>
#include <private/qopcuanodeimpl_p.h>
#include <QtOpcUa/qopcuaqualifiedname.h>
#include <QtOpcUa/qopcuarange.h>
#include <QtOpcUa/qopcuaeuinformation.h>
#include <QtOpcUa/qopcuacomplexnumber.h>
#include <QtOpcUa/qopcuadoublecomplexnumber.h>
#include <QtOpcUa/qopcuaaxisinformation.h>
#include <QtOpcUa/qopcuaxvalue.h>
#include <QtOpcUa/qopcuaargument.h>
#include <QtOpcUa/qopcuaextensionobject.h>
#include <QtOpcUa/qopcuaendpointdescription.h>
#include <QtOpcUa/qopcuaexpandednodeid.h>
#include <QtOpcUa/qopcuarelativepathelement.h>
#include <QtOpcUa/qopcuabrowsepathtarget.h>
#include <QtOpcUa/qopcuaenumdefinition.h>
#include <QtOpcUa/qopcuaenumfield.h>
#include <QtOpcUa/qopcuastructuredefinition.h>
#include <QtOpcUa/qopcuastructurefield.h>

#include <QtCore/qcborarray.h>
#include <private/qfactoryloader_p.h>
#include <QtCore/qloggingcategory.h>
#include <QtCore/qpluginloader.h>

QT_BEGIN_NAMESPACE

using namespace Qt::Literals::StringLiterals;

Q_LOGGING_CATEGORY(QT_OPCUA, "qt.opcua")
Q_LOGGING_CATEGORY(QT_OPCUA_SECURITY, "qt.opcua.security")

/*!
    \class QOpcUaProvider
    \inmodule QtOpcUa

    \brief QOpcUaProvider creates an instance of QOpcUaClient.

    QOpcUaProvider allows the user to create an instance of QOpcUaClient by
    loading a QOpcUaPlugin using the Qt plugin system.

    \section1 Example
    This code creates a client using the first available backend:
    \code
    QOpcUaProvider provider;
    QStringList available = provider.availableBackends();
    if (!available.isEmpty()) {
        QOpcUaClient *client = provider.createClient(available[0]);
        if (client)
            qDebug() << "Client successfully created";
    }
    \endcode
*/

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, qOpcualoader,
        (QOpcUaProviderFactory_iid, u"/opcua"_s))

/*!
    \fn static QHash<QString, QCborMap> loadPluginMetadata()

    Reads the meta data from the plugins known by the loader.
*/
static QHash<QString, QCborMap> loadPluginMetadata()
{
    QHash<QString, QCborMap> plugins;
    const QFactoryLoader *l = qOpcualoader();
    QList<QPluginParsedMetaData> const meta = l->metaData();
    for (int i = 0; i < meta.size(); ++i) {
        QCborMap map = meta.at(i).value(QtPluginMetaDataKeys::MetaData).toMap();
        map.insert("index"_L1, i);
        plugins.insert(map.value("Provider"_L1).toString(), map);
    }
    return plugins;
}

/*!
    \fn static QHash<QString, QCborMap> plugins()

    Returns a \l QHash mapping names to JSON objects containing the meta data of
    available plugins.
*/
static QHash<QString, QCborMap> plugins()
{
    static QHash<QString, QCborMap> plugins;
    static bool alreadyDiscovered = false;

    if (!alreadyDiscovered) {
        plugins = loadPluginMetadata();
        alreadyDiscovered = true;
    }
    return plugins;
}

/*!
    Creates a new OPC UA provider with a given \a parent.
 */
QOpcUaProvider::QOpcUaProvider(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<QOpcUa::Types>();
    qRegisterMetaType<QOpcUa::TypedVariant>();
    qRegisterMetaType<QList<QOpcUa::TypedVariant>>();
    qRegisterMetaType<QOpcUa::UaStatusCode>();
    qRegisterMetaType<QOpcUa::NodeClass>();
    qRegisterMetaType<QOpcUa::NodeClasses>();
    qRegisterMetaType<QOpcUaQualifiedName>();
    qRegisterMetaType<QOpcUa::NodeAttribute>();
    qRegisterMetaType<QOpcUa::NodeAttributes>();
    qRegisterMetaType<QOpcUaNode::AttributeMap>();
    qRegisterMetaType<QList<QOpcUaReadResult>>();
    qRegisterMetaType<QOpcUaClient::ClientState>();
    qRegisterMetaType<QOpcUaClient::ClientError>();
    qRegisterMetaType<QOpcUa::ReferenceTypeId>();
    qRegisterMetaType<QOpcUaMonitoringParameters::SubscriptionType>();
    qRegisterMetaType<QOpcUaMonitoringParameters::Parameter>();
    qRegisterMetaType<QOpcUaMonitoringParameters::Parameters>();
    qRegisterMetaType<QOpcUaMonitoringParameters>();
    qRegisterMetaType<QOpcUaReferenceDescription>();
    qRegisterMetaType<QList<QOpcUaReferenceDescription>>();
    qRegisterMetaType<QOpcUa::ReferenceTypeId>();
    qRegisterMetaType<QOpcUaRange>();
    qRegisterMetaType<QOpcUaEUInformation>();
    qRegisterMetaType<QOpcUaComplexNumber>();
    qRegisterMetaType<QOpcUaDoubleComplexNumber>();
    qRegisterMetaType<QOpcUaAxisInformation>();
    qRegisterMetaType<QOpcUaXValue>();
    qRegisterMetaType<QOpcUaExpandedNodeId>();
    qRegisterMetaType<QOpcUaRelativePathElement>();
    qRegisterMetaType<QList<QOpcUaRelativePathElement>>();
    qRegisterMetaType<QOpcUaBrowsePathTarget>();
    qRegisterMetaType<QList<QOpcUaBrowsePathTarget>>();
    qRegisterMetaType<QOpcUaEndpointDescription>();
    qRegisterMetaType<QList<QOpcUaEndpointDescription>>();
    qRegisterMetaType<QOpcUaArgument>();
    qRegisterMetaType<QOpcUaExtensionObject>();
    qRegisterMetaType<QOpcUaBrowseRequest>();
    qRegisterMetaType<QOpcUaReadItem>();
    qRegisterMetaType<QOpcUaReadResult>();
    qRegisterMetaType<QList<QOpcUaReadItem>>();
    qRegisterMetaType<QList<QOpcUaReadResult>>();
    qRegisterMetaType<QOpcUaWriteItem>();
    qRegisterMetaType<QOpcUaWriteResult>();
    qRegisterMetaType<QList<QOpcUaWriteItem>>();
    qRegisterMetaType<QList<QOpcUaWriteResult>>();
    qRegisterMetaType<QOpcUaNodeCreationAttributes>();
    qRegisterMetaType<QOpcUaAddNodeItem>();
    qRegisterMetaType<QOpcUaAddReferenceItem>();
    qRegisterMetaType<QOpcUaDeleteReferenceItem>();
    qRegisterMetaType<QList<QOpcUaApplicationDescription>>();
    qRegisterMetaType<QOpcUaApplicationIdentity>();
    qRegisterMetaType<QOpcUaPkiConfiguration>();
    qRegisterMetaType<QOpcUaStructureDefinition>();
    qRegisterMetaType<QOpcUaStructureField>();
    qRegisterMetaType<QOpcUaEnumDefinition>();
    qRegisterMetaType<QOpcUaEnumField>();
}

QOpcUaProvider::~QOpcUaProvider()
{
    qDeleteAll(m_plugins);
}

static QOpcUaPlugin *loadPlugin(const QString &key)
{
    const int index = qOpcualoader()->indexOf(key);
    if (index != -1) {
        QObject *factoryObject = qOpcualoader()->instance(index);
        if (QOpcUaPlugin *plugin = qobject_cast<QOpcUaPlugin *>(factoryObject)) {
            return plugin;
        }
    }
    return nullptr;
}

/*!
    Returns a pointer to a QOpcUaClient object by loading the selected \a backend
    as a plugin and creating a client object.
    If the plugin loading fails, \c nullptr is returned instead.

    The user is responsible for deleting the returned \l QOpcUaClient object
    when it is no longer needed.

    The optional argument \a backendProperties can be used to pass custom backend specific settings as key value pairs.
    Those settings are specific to the backend being instantiated.

    Available settings are
    \table
    \header
        \li Setting string
        \li Backend
        \li Description
    \row
        \li minimumClientIterateIntervalMs
        \li open62541
        \li This parameter is no longer evaluated by the backend and has been replaced by \c clientIterateIntervalMs.
    \row
        \li clientIterateIntervalMs
        \li open62541
        \li Defines the client iterate interval for the backend. If the client is causing too much CPU load,
            setting this value higher than the default will reduce the CPU load at the price of an increased
            response time to service requests and value updates from subscriptions.
            The default value is 50ms.
    \row
        \li asyncRequestTimeoutMs
        \li open62541
        \li Defines the timeout for asynchronous requests to an OPC UA server. If the server doesn't reply to
            a service request before the timeout occurs, the service call fails and the finished signal will
            contain a \c bad status code. The default value is 15000ms.
    \endtable
*/
QOpcUaClient *QOpcUaProvider::createClient(const QString &backend, const QVariantMap &backendProperties)
{
    QOpcUaPlugin *plugin;
    auto it = m_plugins.find(backend);
    if (it == m_plugins.end()) {
        plugin = loadPlugin(backend);
        if (!plugin) {
            qCWarning(QT_OPCUA) << "Failed to load OPC UA plugin:" << backend;
            qCWarning(QT_OPCUA) << "Available plugins:" << availableBackends();
            return nullptr;
        }
        m_plugins.insert(backend, plugin);
    }
    else {
        plugin = it.value();
    }
    return plugin->createClient(backendProperties);
}

/*!
    Returns a QStringList of available plugins.
*/
QStringList QOpcUaProvider::availableBackends()
{
    return plugins().keys();
}

QT_END_NAMESPACE

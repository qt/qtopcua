// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuagdsclient_p.h"
#include <QOpcUaProvider>
#include <QOpcUaExtensionObject>
#include <QOpcUaBinaryDataEncoding>
#include <QOpcUaErrorState>
#include <QOpcUaKeyPair>
#include <QOpcUaX509ExtensionSubjectAlternativeName>
#include <QOpcUaX509ExtensionBasicConstraints>
#include <QOpcUaX509ExtensionKeyUsage>
#include <QFile>
#include <QTimer>
#include <QTemporaryFile>
#include <QSettings>
#include <QSslCertificate>
#include <QtCore/qloggingcategory.h>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(QT_OPCUA_GDSCLIENT, "qt.opcua.gdsclient")

static const QStringList elementsToResolve {
    QLatin1String("GetApplication"),
    QLatin1String("FindApplications"),
    QLatin1String("RegisterApplication"),
    QLatin1String("UnregisterApplication"),
    QLatin1String("GetCertificateGroups"),
    QLatin1String("GetCertificateStatus"),
    QLatin1String("StartSigningRequest"),
    QLatin1String("FinishRequest"),
    QLatin1String("GetTrustList"),
};

/*!
    \class QOpcUaGdsClient
    \inmodule QtOpcUa
    \since 5.14

    \brief Handles communication with the GDS Server.

    This class is currently available as a Technology Preview, and therefore the API
    and functionality provided by the class may be subject to change at any time without
    prior notice.

    This class handles all steps needed for communication with a GDS server. Provided with
    information about the application it does registering with the server and managing
    key/certificates.

    Only few details need to be known in order to take part in a secured network.

    First time registration requires administrative privileges using username and password
    for authentication. All further authentications are application based, using the certificate
    which was received first.

    Expecting the whole process to succeed, you have to wait for the applicationRegistered signal.

    Most of the setup structs have to be the same as for the connection with QOpcUaClient afterwards
    and can be shared.

    Setting up a GDS client:

    \code
    QOpcUaGdsClient c;

    // In case the credentials are needed
    QObject::connect(&c, &QOpcUaGdsClient::authenticationRequired, [&](QOpcUaAuthenticationInformation &authInfo) {
            authInfo.setUsernameAuthentication("root", "secret");
     });

    // Await success
    QObject::connect(&c, &QOpcUaGdsClient::applicationRegistered, [&]() {
        qDebug() << "Application" << c.applicationId() << "registered";
    });

    c.setBackend(...);
    c.setEndpoint(...);
    c.setApplicationIdentity(...);
    c.setPkiConfiguration(...);
    c.setApplicationRecord(...);
    c.setCertificateSigningRequestPresets(...);
    c.start();
    \endcode
*/

/*!
    \enum QOpcUaGdsClient::Error

    This enum is used to specify errors, that could happen during the registration
    process.

    \value NoError
        Everying is fine
    \value InvalidBackend
        The backend could not be instantiated.
        The backend string given, does not match any backend or loading the plugin failed.
    \value InvalidEndpoint
        The given endpoint is not valid.
    \value ConnectionError
        The connection to the server endpoint failed.
    \value DirectoryNodeNotFound
        The directory node on the server could not be resolved
    \value FailedToRegisterApplication
        The registration of the application was not successful.
    \value FailedToUnregisterApplication
        The unregistration of the application was not successful.
    \value FailedToGetCertificateStatus
        The status of the current certificate could not be retrieved.
    \value FailedToGetCertificate
        A certificate could not be retrieved from the server.
*/

/*!
    \enum QOpcUaGdsClient::State

    This enum is used to specify the current state of the registration of the GDS
    client.

    \value Idle
        The client was not started yet.
    \value BackendInstantiated
        The backend was instantiated
    \value Connecting
        A connecting to the server is being made
    \value Connected
        The connection to the server endpoint was successful.
    \value RegisteringApplication
        The application is being registered with the server.
    \value ApplicationRegistered
        Registering the application with the server was successful.
    \value Error
        An error happened. See the return value of error() and the terminal output
        for more details.
*/

/*!
    \fn QOpcUaGdsClient::stateChanged(State state)

    This signal is emitted when the internal state of the client changes.
    The \a state indicates the new state.
*/

/*!
    \fn QOpcUaGdsClient::errorChanged(Error error)

    This signal is emitted when an \a error occurred.
*/

/*!
    \fn QOpcUaGdsClient::applicationRegistered()

    This signal is emitted when an application was registered successfully.
*/

/*!
    \fn QOpcUaGdsClient::certificateGroupsReceived(QStringList certificateGroups)

    This signal is emitted when the GDS client receives a new list of \a certificateGroups
    for this application.
*/

/*!
    \fn QOpcUaGdsClient::certificateUpdateRequired()

    This signal is emitted when the GDS client detects that an update of the currently
    used certificate is necessary.

    This could be caused by the server, requesting the client to update the certificate,
    when the certificate's due date is met or if the certificate is self-signed.

    The certificate update is handled automatically. This signal is only for informational
    purpose that an update is going to happen.
*/

/*!
    \fn QOpcUaGdsClient::certificateUpdated()

    This signal is emitted when the GDS client received a new certificate that was
    stored on disk.
*/

/*!
    \fn QOpcUaGdsClient::unregistered()

    This signal is emitted when the GDS client has unregistered the application.
*/

/*!
    \fn QOpcUaGdsClient::trustListUpdated()

    This signal is emitted when the GDS client has received a new trust list from the
    server and stored to disk.
*/

/*!
    \fn QOpcUaGdsClient::authenticationRequired(QOpcUaAuthenticationInformation &authInfo)

    This signal is emitted when the GDS client tries to do a first time authentication
    with a server, that requires administrative privileges.

    \a authInfo has to be filled with valid authentication information.
    This slot must not be used crossing thread boundaries.
*/

/*
    Step 1
    Create a self-signed certificate for the application.
    Previously created key and certificate are reused if present.

    Step 2
    Connect to the given endpoint and wait for the namespace array to be updated.

    Step 3
    Find the root node of the GDS API

    Step 4
    From the root node find the "Directory" node where the method nodes are located.
        - resolveDirectoryNode()
        - lambda

    Step 5
    Resolve the method nodes that are being used.
        - resolveMethodNodes()
        - _q_handleResolveBrowsePathFinished()

    Step 6a
    In case a previous appliation id is known, check if the application registration is still valid.
    In case it is still valid, reuse it.
        - getApplication()
        - handleGetApplicationFinished()

    Step 6b
    In case there is no valid application id known, try to find a previous registration by the application URI.
        - findRegisteredApplication()
        - handleFindApplicationsFinished()

    Step 6c
    In case an application id was not found by the application URI, register the application.
        - registerApplication()
        - handleRegisterApplicationFinished()

    Step 7
    Get the certificate groups of the application
        - getCertificateGroups()
        - handleGetCertificateGroupsFinished()
        - emit signal certificateGroupsReceived

    Step 8
    Resolve the certificate types from the certificate groups (CertificateGroup->CertificateTypes)
        - resolveCertificateTypes()
        - lambda

    Step 9
    Read certificate type value from the resolved nodes
        - getCertificateTypes()
        - lambda

    Step 10
    The application now is using a application id from 6a, 6b or 6c, resolved the certificates and is ready to interact with the GDS.
        - registrationDone()
        - emit signal applicationRegistered
        -> Start a certificate check
        -> Start a trust list update

    Depending on the certificate state, the certificate needs to be updated by the client.
    This may happen due to
        - the server requires it (GetCertificateStatus)
        - the expiry date is due
        - the certificate is self-signed from the initial connection

    Step 1
    A timer will trigger the certificate check regularly by triggering
        - _q_certificateCheckTimeout()
        - emit signal certificateUpdateRequired

    Step 2a
    Check locally first, if a renew condition is met.
    Otherwise ask the server.

    Step 2b
    Ask the server if the certificate needs renewal
        - getCertificateStatus()
        - handleGetCertificateStatusFinished()
        - emit signal certificateUpdateRequired

    Step 3
    Start a certificate signing request
        - startCertificateRequest()
        - handleStartSigningRequestFinished()

    Step 4
    Regularly ask the server for the certificate signing result
        - finishCertificateRequest()
        - handleFinishRequestFinished()
        - emit signal certificateUpdated
    This will return a new certificate.
*/

/*!
    Constructs a GDS client with \a parent as the parent object.
*/
QOpcUaGdsClient::QOpcUaGdsClient(QObject *parent)
    : QObject(*(new QOpcUaGdsClientPrivate()), parent)
{
    Q_D(QOpcUaGdsClient);
    d->initializePrivateConnections();
}

/*!
    Destructs a GDS client.
*/
QOpcUaGdsClient::~QOpcUaGdsClient()
{

}

/*!
    Sets the backend to be used by the client to communicate with the server to \a backend.

    This function has to be called before starting the GDS client.
    Changing this setting afterwards has no effect.

    \sa QOpcUaProvider::availableBackends() start()
*/
void QOpcUaGdsClient::setBackend(const QString &backend)
{
    Q_D(QOpcUaGdsClient);
    d->setBackend(backend);
}

/*!
    Returns the current backend setting.

    If the backend was changed after starting the client, it will return
    the changed setting, but not the actually used instance.
*/
const QString &QOpcUaGdsClient::backend() const
{
    Q_D(const QOpcUaGdsClient);
    return d->backend();
}

/*!
    Sets the endpoint to be used by the client to communicate with the server to \a endpoint.

    This function has to be called before starting the GDS client.
    Changing this setting afterwards has no effect.

    Communication to a GDS server is only possible through an encrypted endpoint.
    Using an unencrypted endpoint will fail.
*/
void QOpcUaGdsClient::setEndpoint(const QOpcUaEndpointDescription &endpoint)
{
    Q_D(QOpcUaGdsClient);
    d->setEndpoint(endpoint);
}

/*!
    Returns the current endpoint setting.

    If the endpoint was changed after starting the client, it will return
    the changed setting, but not the actually used endpoint.
*/
const QOpcUaEndpointDescription &QOpcUaGdsClient::endpoint() const
{
    Q_D(const QOpcUaGdsClient);
    return d->endpoint();
}

/*!
    Sets the PKI configuration \a pkiConfig to be used by the client.

    All certificates, keys and trust lists will be used from or stored to
    the locations given. In order to use the certificate received from
    the GDS, the same configuration has to be used with QOpcUaClient.

    This function has to be called before starting the GDS client.
    Changing this setting afterwards has no effect.
*/
void QOpcUaGdsClient::setPkiConfiguration(const QOpcUaPkiConfiguration &pkiConfig)
{
    Q_D(QOpcUaGdsClient);
    d->setPkiConfiguration(pkiConfig);
}

/*!
    Returns the current pkiConfiguration.
*/
const QOpcUaPkiConfiguration &QOpcUaGdsClient::pkiConfiguration() const
{
    Q_D(const QOpcUaGdsClient);
    return d->pkiConfiguration();
}

/*!
    Sets the application identity \a appIdentity to be used by the client.

    This identity is used to register with the GDS server.
    This function has to be called before starting the GDS client.
    Changing this setting afterwards has no effect.
*/
void QOpcUaGdsClient::setApplicationIdentity(const QOpcUaApplicationIdentity &appIdentity)
{
    Q_D(QOpcUaGdsClient);
    d->setApplicationIdentity(appIdentity);
}

/*!
    Returns the current applicationIdentity.
*/
const QOpcUaApplicationIdentity &QOpcUaGdsClient::applicationIdentity() const
{
    Q_D(const QOpcUaGdsClient);
    return d->applicationIdentity();
}

/*!
    Sets the application record data \a appRecord to be used by the client.

    This data is used to register with the GDS server.
    This function has to be called before starting the GDS client.

    Most of the data is the same as in the application identity.
    After registration the assigned application id can be retrieved.

    \sa  setApplicationIdentity
*/
void QOpcUaGdsClient::setApplicationRecord(const QOpcUaApplicationRecordDataType &appRecord)
{
    Q_D(QOpcUaGdsClient);
    d->setApplicationRecord(appRecord);
}

/*!
    Returns the application record data that is used by the client.
*/
const QOpcUaApplicationRecordDataType &QOpcUaGdsClient::applicationRecord() const
{
    Q_D(const QOpcUaGdsClient);
    return d->applicationRecord();
}

/*!
    Returns the application id assigned by the server.

    Is is a shortcut to receive the application id from the application record data.

    \sa applicationRecord()
*/
QString QOpcUaGdsClient::applicationId() const
{
    Q_D(const QOpcUaGdsClient);
    return d->applicationId();
}

/*!
    Sets the presets for certificate siging requests; the distinguished name \a dn and
    the DNS string \a dns.

    When creating a certificate signing request some additional information is needed,
    that is not provided by the application identity.

    This function has to be called before starting the GDS client.

    \sa setApplicationIdentity()
*/
void QOpcUaGdsClient::setCertificateSigningRequestPresets(const QOpcUaX509DistinguishedName &dn, const QString &dns)
{
    Q_D(QOpcUaGdsClient);
    d->setCertificateSigningRequestPresets(dn, dns);
}

/*!
   Returns the distinguished name preset for certificate siging requests.
*/
const QOpcUaX509DistinguishedName &QOpcUaGdsClient::distinguishedNameCertificateSigningRequestPreset() const
{
    Q_D(const QOpcUaGdsClient);
    return d->distinguishedNameCertificateSigningRequestPreset();
}

/*!
   Returns the DNS preset for certificate siging requests.
*/
const QString &QOpcUaGdsClient::dnsCertificateSigningRequestPreset() const
{
    Q_D(const QOpcUaGdsClient);
    return d->dnsCertificateSigningRequestPreset();
}

/*!
   Sets the interval in milliseconds for checking the validity of the client certificate
   to \a interval.
*/
void QOpcUaGdsClient::setCertificateCheckInterval(int interval)
{
    Q_D(QOpcUaGdsClient);
    d->setCertificateCheckInterval(interval);
}

/*!
   Returns the interval in milliseconds for checking the validity of the client certificate.
*/
int QOpcUaGdsClient::certificateCheckInterval() const
{
    Q_D(const QOpcUaGdsClient);
    return d->certificateCheckInterval();
}

/*!
   Sets the interval in milliseconds for updating the trust list from the server
   to \a interval.
*/
void QOpcUaGdsClient::setTrustListUpdateInterval(int interval)
{
    Q_D(QOpcUaGdsClient);
    d->setTrustListUpdateInterval(interval);
}

/*!
   Returns the interval in milliseconds for updating the trust list from the server.
*/
int QOpcUaGdsClient::trustListUpdateInterval() const
{
    Q_D(const QOpcUaGdsClient);
    return d->trustListUpdateInterval();
}

/*!
    Returns the current error state.
*/
QOpcUaGdsClient::Error QOpcUaGdsClient::error() const
{
    Q_D(const QOpcUaGdsClient);
    return d->error();
}

/*!
    Returns the current client state.
*/
QOpcUaGdsClient::State QOpcUaGdsClient::state() const
{
    Q_D(const QOpcUaGdsClient);
    return d->state();
}

/*!
    Starts the client process.

    After setting up all information,
    the client can be started.

    \list
    \li setBackend
    \li setEndpoing
    \li setApplicationIdentity
    \li setPkiConfiguration
    \li setApplicationRecord
    \li setCertificateSigingRequestPresets
    \endlist
*/
void QOpcUaGdsClient::start()
{
    Q_D(QOpcUaGdsClient);
    return d->start();
}

/*!
    Unregisters an application from the server.

    This function can be used when an application has to be removed permanently from
    the network. It does not need to be called when rebooting or shutting down.
*/
void QOpcUaGdsClient::unregisterApplication()
{
    Q_D(QOpcUaGdsClient);
    return d->unregisterApplication();
}

QOpcUaGdsClientPrivate::QOpcUaGdsClientPrivate()
    : QObjectPrivate()
    , m_certificateCheckTimer(new QTimer)
    , m_trustListUpdateTimer(new QTimer)
{
    m_certificateCheckTimer->setInterval(60*60*1000);
    m_trustListUpdateTimer->setInterval(60*60*1000);
}

QOpcUaGdsClientPrivate::~QOpcUaGdsClientPrivate()
{
    delete m_client;
    delete m_directoryNode;
    delete m_certificateGroupNode;
    delete m_certificateTypesNode;
    delete m_certificateFinishTimer;
    delete m_certificateCheckTimer;
    delete m_trustListUpdateTimer;
    delete m_trustListNode;
}

void QOpcUaGdsClientPrivate::initializePrivateConnections()
{
    Q_Q(QOpcUaGdsClient);
    QObject::connect(m_certificateCheckTimer, SIGNAL(timeout()), q, SLOT(_q_certificateCheckTimeout()));
    QObject::connect(m_trustListUpdateTimer, SIGNAL(timeout()), q, SLOT(_q_updateTrustList()));
}

void QOpcUaGdsClientPrivate::setEndpoint(const QOpcUaEndpointDescription &endpoint)
{
    m_endpoint = endpoint;
}

const QOpcUaEndpointDescription &QOpcUaGdsClientPrivate::endpoint() const
{
    return m_endpoint;
}

void QOpcUaGdsClientPrivate::setBackend(const QString &backend)
{
    m_backend = backend;
}

const QString &QOpcUaGdsClientPrivate::backend() const
{
    return m_backend;
}

QString QOpcUaGdsClientPrivate::applicationId() const
{
    return m_appRecord.applicationId();
}

void QOpcUaGdsClientPrivate::setError(QOpcUaGdsClient::Error error)
{
    Q_Q(QOpcUaGdsClient);
    m_error = error;
    setState(QOpcUaGdsClient::State::Error);
    emit q->errorChanged(m_error);
}

void QOpcUaGdsClientPrivate::setState(QOpcUaGdsClient::State state)
{
    Q_Q(QOpcUaGdsClient);
    m_state = state;
    emit q->stateChanged(state);
}

void QOpcUaGdsClientPrivate::start()
{
    Q_Q(QOpcUaGdsClient);

    if (m_backend.isEmpty()) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Backend name not set";
        setError(QOpcUaGdsClient::Error::InvalidBackend);
        return;
    }

    if (!m_client) {
        QOpcUaProvider provider;
        setState(QOpcUaGdsClient::State::BackendInstantiated);
        m_client = provider.createClient(m_backend);
        if (!m_client) {
            setError(QOpcUaGdsClient::Error::InvalidBackend);
            qCWarning(QT_OPCUA_GDSCLIENT) << "Invalid backend";
            return;
        }

        QObject::connect(m_client, &QOpcUaClient::namespaceArrayUpdated, [this]() {
            setState(QOpcUaGdsClient::State::Connected);
            this->resolveDirectoryNode();
        });

        QObject::connect(m_client, &QOpcUaClient::errorChanged, q, [this](QOpcUaClient::ClientError error) {
            if (error == QOpcUaClient::InvalidUrl) {
                qCWarning(QT_OPCUA_GDSCLIENT) << "Invalid URL";
                setError(QOpcUaGdsClient::Error::InvalidEndpoint);
            } else {
                qCWarning(QT_OPCUA_GDSCLIENT) << "Connection error";
                setError(QOpcUaGdsClient::Error::ConnectionError);
            }
        });

        QObject::connect(m_client, &QOpcUaClient::disconnected, q, [this]() {
            delete m_directoryNode;
            m_directoryNode = nullptr;
            m_directoryNodes.clear();
            setState(QOpcUaGdsClient::State::Idle);
            if (m_restartRequired) {
                m_restartRequired = false;
                this->start();
            }
        });

        QObject::connect(m_client, &QOpcUaClient::connectError, [](QOpcUaErrorState *errorState) {
            // Ignore all client side errors and continue
            if (errorState->isClientSideError())
                errorState->setIgnoreError();
        });
    }

    setState(QOpcUaGdsClient::State::Connecting);
    if (m_endpoint.endpointUrl().isEmpty()) {
        setError(QOpcUaGdsClient::Error::InvalidEndpoint);
        qCWarning(QT_OPCUA_GDSCLIENT) << "Invalid endpoint";
        return;
    }

    QOpcUaKeyPair keyPair;
    QFile keyFile(m_pkiConfig.privateKeyFile());

    if (!keyFile.exists()) {
        // File does not exist: Create a key
        qCDebug(QT_OPCUA_GDSCLIENT) << "Creating a key";
        keyPair.generateRsaKey(QOpcUaKeyPair::RsaKeyStrength::Bits1024);

        if (!keyFile.open(QFile::WriteOnly | QFile::NewOnly)) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to open private key file" << keyFile.fileName() << "for writing:" << keyFile.errorString();
            setError(QOpcUaGdsClient::Error::ConnectionError);
            return;
        }

        auto data = keyPair.privateKeyToByteArray(QOpcUaKeyPair::Cipher::Unencrypted, QString());

        if (!keyFile.resize(data.size())) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to set file size";
            setError(QOpcUaGdsClient::Error::ConnectionError);
            return;
        }

        if (!keyFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner)) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to set permissions";
            setError(QOpcUaGdsClient::Error::ConnectionError);
            return;
        }

        keyFile.write(data);
        keyFile.close();
    } else {
        qCDebug(QT_OPCUA_GDSCLIENT) << "Using private key" << keyFile.fileName();

        if (!keyFile.open(QFile::ReadOnly)) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to open private key file" << keyFile.fileName() << "for reading:" << keyFile.errorString();
            setError(QOpcUaGdsClient::Error::ConnectionError);
            return;
        }

        auto data = keyFile.readAll();
        keyFile.close();

        if (!keyPair.loadFromPemData(data)) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to load private key";
            setError(QOpcUaGdsClient::Error::ConnectionError);
            return;
        }

        if (!keyPair.hasPrivateKey()) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Private key does not contain a private key";
            setError(QOpcUaGdsClient::Error::ConnectionError);
            return;
        }
    }

    QFile certFile(m_pkiConfig.clientCertificateFile());
    if (!certFile.exists()) {
        qCDebug(QT_OPCUA_GDSCLIENT) << "Creating self-signed certificate in" << certFile.fileName();

        auto csr = createSigningRequest();
        csr.setEncoding(QOpcUaX509CertificateSigningRequest::Encoding::DER);
        auto selfSigned = csr.createSelfSignedCertificate(keyPair);

        if (!certFile.open(QFile::WriteOnly | QFile::NewOnly)) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to open certificate file" << certFile.fileName() << "for writing:" << certFile.errorString();
            setError(QOpcUaGdsClient::Error::ConnectionError);
            return;
        }

        if (!certFile.resize(selfSigned.size())) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to set file size 2";
            setError(QOpcUaGdsClient::Error::ConnectionError);
            return;
        }

        certFile.write(selfSigned);
        certFile.close();
    }

    // Load persistent data
    QSettings settings;
    qCDebug(QT_OPCUA_GDSCLIENT) << "Using settings from" << settings.fileName();
    const auto applicationId = settings.value(QLatin1String("gds/applicationId"), QString()).toString();
    if (applicationId.isEmpty())
        qCInfo(QT_OPCUA_GDSCLIENT) << "No application ID in persistent storage";
    else
        m_appRecord.setApplicationId(applicationId);

    m_client->setApplicationIdentity(m_appIdentitiy);
    m_client->setPkiConfiguration(m_pkiConfig);
    m_client->connectToEndpoint(m_endpoint);
}

void QOpcUaGdsClientPrivate::resolveDirectoryNode()
{
    Q_Q(QOpcUaGdsClient);

    if (!m_client || m_client->state() != QOpcUaClient::Connected) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No connection";
        return;
    }

    delete m_directoryNode;
    m_directoryNode = m_client->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::ObjectsFolder)); // ns=0;i=85
    if (!m_directoryNode) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Root node not found";
        setError(QOpcUaGdsClient::Error::DirectoryNodeNotFound);
        return;
    }

    m_gdsNamespaceIndex = m_client->namespaceArray().indexOf(QLatin1String("http://opcfoundation.org/UA/GDS/"));
    if (m_gdsNamespaceIndex < 0) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Namespace not found";
        setError(QOpcUaGdsClient::Error::DirectoryNodeNotFound);
        return;
    }

    QObject::connect(m_directoryNode, &QOpcUaNode::resolveBrowsePathFinished, [this, q](QList<QOpcUaBrowsePathTarget> targets,
                                                                 QList<QOpcUaRelativePathElement> path,
                                                                 QOpcUa::UaStatusCode statusCode) {
        m_directoryNode->deleteLater();
        m_directoryNode = nullptr;

        if (path.size() != 1) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Invalid path size";
            setError(QOpcUaGdsClient::Error::DirectoryNodeNotFound);
            return;
        }

        if (path[0].targetName().name() != QLatin1String("Directory")) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Invalid resolve name" << path[0].targetName().name();
            setError(QOpcUaGdsClient::Error::DirectoryNodeNotFound);
            return;
        }

        if (statusCode != QOpcUa::Good) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Resolving directory failed" << statusCode;
            setError(QOpcUaGdsClient::Error::DirectoryNodeNotFound);
            return;
        }

        if (targets.size() != 1) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Invalid number of results";
            setError(QOpcUaGdsClient::Error::DirectoryNodeNotFound);
            return;
        }

        if (!targets[0].isFullyResolved()) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Directory not fully resolved";
            setError(QOpcUaGdsClient::Error::DirectoryNodeNotFound);
            return;
        }

        m_directoryNode = m_client->node(targets[0].targetId());
        if (!m_directoryNode) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Invalid directory node";
            setError(QOpcUaGdsClient::Error::DirectoryNodeNotFound);
            return;
        }

        QObject::connect(m_directoryNode, SIGNAL(resolveBrowsePathFinished(QList<QOpcUaBrowsePathTarget>, QList<QOpcUaRelativePathElement>, QOpcUa::UaStatusCode)),
                         q, SLOT(_q_handleResolveBrowsePathFinished(QList<QOpcUaBrowsePathTarget>, QList<QOpcUaRelativePathElement>, QOpcUa::UaStatusCode)));
        QObject::connect(m_directoryNode, SIGNAL(methodCallFinished(QString, QVariant, QOpcUa::UaStatusCode)),
                         q, SLOT(_q_handleDirectoryNodeMethodCallFinished(QString, QVariant, QOpcUa::UaStatusCode)));

        qCDebug(QT_OPCUA_GDSCLIENT) << "Directory node resolved:" << m_directoryNode->nodeId();
        this->resolveMethodNodes();
    });

    QOpcUaRelativePathElement pathElement(QOpcUaQualifiedName(m_gdsNamespaceIndex, QLatin1String("Directory")),
                                           QOpcUa::ReferenceTypeId::Organizes);
    QList<QOpcUaRelativePathElement> browsePath { pathElement };

    if (!m_directoryNode->resolveBrowsePath(browsePath)) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to resolve directory node";
        setError(QOpcUaGdsClient::Error::DirectoryNodeNotFound);
        delete m_directoryNode;
        m_directoryNode = nullptr;
        return;
    }
}

void QOpcUaGdsClientPrivate::resolveMethodNodes()
{
    // See OPC UA Specification 1.04 part 12 6.3.2 "Directory"

    QOpcUaRelativePathElement pathElement(QOpcUaQualifiedName(m_gdsNamespaceIndex, QLatin1String()),
                                           QOpcUa::ReferenceTypeId::HasComponent);
    QList<QOpcUaRelativePathElement> browsePath { pathElement };


    // Resolve all needed nodes from the directory
    for (const auto &key : std::as_const(elementsToResolve)) {
        if (!m_directoryNodes.value(key).isEmpty())
            continue; // Already resolved

        auto target = browsePath[0].targetName();
        target.setName(key);
        browsePath[0].setTargetName(target);

        if (!m_directoryNode->resolveBrowsePath(browsePath)) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Could not resolve Directory node";
            setError(QOpcUaGdsClient::Error::DirectoryNodeNotFound);
            return;
        }
    }
}

void QOpcUaGdsClientPrivate::_q_handleResolveBrowsePathFinished(QList<QOpcUaBrowsePathTarget> targets, QList<QOpcUaRelativePathElement> path, QOpcUa::UaStatusCode statusCode) {
    if (path.size() != 1) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Invalid path size";
        setError(QOpcUaGdsClient::Error::DirectoryNodeNotFound);
        return;
    }

    if (m_directoryNodes.contains(path[0].targetName().name()) || !elementsToResolve.contains(path[0].targetName().name())) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Invalid resolve name" << path[0].targetName().name();
        setError(QOpcUaGdsClient::Error::DirectoryNodeNotFound);
        return;
    }

    if (statusCode != QOpcUa::Good) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Resolving directory failed" << statusCode;
        setError(QOpcUaGdsClient::Error::DirectoryNodeNotFound);
        return;
    }

    if (targets.size() != 1) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Invalid number of results";
        setError(QOpcUaGdsClient::Error::DirectoryNodeNotFound);
        return;
    }

    if (!targets[0].isFullyResolved()) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Directory not fully resolved";
        setError(QOpcUaGdsClient::Error::DirectoryNodeNotFound);
        return;
    }

    m_directoryNodes[path[0].targetName().name()] = targets[0].targetId().nodeId();

    if (elementsToResolve.size() == m_directoryNodes.size()) {
        qCDebug(QT_OPCUA_GDSCLIENT) << "All symbols resolved";

        if (m_appRecord.applicationId().isEmpty() || m_appRecord.applicationId() == QLatin1String("ns=0;i=0"))
            this->findRegisteredApplication();
        else
            this->getApplication();
    }
}

void QOpcUaGdsClientPrivate::getApplication()
{
    // See OPC UA Specification 1.04 part 12 6.3.9 "GetApplication"

    if (!m_client || m_client->state() != QOpcUaClient::Connected) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No connection";
        return;
    }

    if (!m_directoryNode) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No directory node";
        setError(QOpcUaGdsClient::Error::FailedToRegisterApplication);
        return;
    }

    if (!m_directoryNode->callMethod(m_directoryNodes[QLatin1String("GetApplication")],
                                     QList<QPair<QVariant, QOpcUa::Types>> { qMakePair(QVariant(m_appRecord.applicationId()), QOpcUa::Types::NodeId) })) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to call method GetApplication";
        setError(QOpcUaGdsClient::Error::FailedToRegisterApplication);
        return;
    }
}

void QOpcUaGdsClientPrivate::handleGetApplicationFinished(const QVariant &result, QOpcUa::UaStatusCode statusCode)
{
    if (statusCode == QOpcUa::BadNotFound) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No application with ID" << m_appRecord.applicationId() << "registered";
        // Clear application ID and register
        m_appRecord.setApplicationId(QString());

        // Remove invalid id from settings
        QSettings settings;
        settings.remove(QLatin1String("gds/applicationId"));
        settings.sync();

        restartWithCredentials();
        return;
    }

    if (statusCode != QOpcUa::Good) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Get application failed" << statusCode;
        setError(QOpcUaGdsClient::Error::FailedToRegisterApplication);
        return;
    }

    auto extensionObject = result.value<QOpcUaExtensionObject>();
    if (!extensionObject.encodingTypeId().isEmpty()) {
        if (extensionObject.encodingTypeId() != QOpcUa::nodeIdFromInteger(m_gdsNamespaceIndex, ApplicationRecordDataType_Encoding_DefaultBinary)) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Unexpected return type:" << extensionObject.encodingTypeId();
            setError(QOpcUaGdsClient::Error::FailedToRegisterApplication);
            return;
        }

        auto buffer = extensionObject.encodedBody();
        QOpcUaBinaryDataEncoding decoder(&buffer);
        bool ok;
        QOpcUaApplicationRecordDataType appRecord = decoder.decode<QOpcUaApplicationRecordDataType>(ok);

        if (!ok) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to decode data";
            setError(QOpcUaGdsClient::Error::FailedToRegisterApplication);
            return;
        }

        if (m_appRecord.applicationId() != appRecord.applicationId())
            qCWarning(QT_OPCUA_GDSCLIENT) << "Returned application record contains a different application ID" << m_appRecord.applicationId() << appRecord.applicationId();

        // FIXME: To be removed
        appRecord.setApplicationId(m_appRecord.applicationId());

        m_appRecord = appRecord;
        qCInfo(QT_OPCUA_GDSCLIENT) << "Reusing application ID" << m_appRecord.applicationId() << "which is already registered at the server";
    }
    getCertificateGroups();
}

void QOpcUaGdsClientPrivate::findRegisteredApplication()
{
    if (!m_client || m_client->state() != QOpcUaClient::Connected) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No connection";
        return;
    }

    if (!m_directoryNode) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No directory node";
        setError(QOpcUaGdsClient::Error::FailedToRegisterApplication);
        return;
    }

    if (!m_directoryNode->callMethod(m_directoryNodes[QLatin1String("FindApplications")],
                                     QList<QPair<QVariant, QOpcUa::Types>> { qMakePair(QVariant(m_appRecord.applicationUri()), QOpcUa::Types::String) })) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to call method";
        setError(QOpcUaGdsClient::Error::FailedToRegisterApplication);
        return;
    }
}

void QOpcUaGdsClientPrivate::handleFindApplicationsFinished(const QVariant &result, QOpcUa::UaStatusCode statusCode)
{
    if (statusCode != QOpcUa::Good) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Find application failed" << statusCode;
        setError(QOpcUaGdsClient::Error::FailedToRegisterApplication);
        return;
    }

    auto extensionObject = result.value<QOpcUaExtensionObject>();

    if (extensionObject.encodingTypeId().isEmpty()) {
        qCInfo(QT_OPCUA_GDSCLIENT) << "No application with URI" << m_appIdentitiy.applicationUri() << "registered";
        this->registerApplication();
        return;
    }

    if (extensionObject.encodingTypeId() != QOpcUa::nodeIdFromInteger(m_gdsNamespaceIndex, ApplicationRecordDataType_Encoding_DefaultBinary)) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Unexpected return type:" << extensionObject.encodingTypeId();
        setError(QOpcUaGdsClient::Error::FailedToRegisterApplication);
        return;
    }

    auto buffer = extensionObject.encodedBody();
    QOpcUaBinaryDataEncoding decoder(&buffer);
    bool ok;
    QOpcUaApplicationRecordDataType appRecord = decoder.decode<QOpcUaApplicationRecordDataType>(ok);

    if (!ok) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to decode data";
        setError(QOpcUaGdsClient::Error::FailedToRegisterApplication);
        return;
    }

    m_appRecord = appRecord;
    qCInfo(QT_OPCUA_GDSCLIENT) << "Reusing application ID" << appRecord.applicationId() << "registered for URI" << appRecord.applicationUri();
    getCertificateGroups();
}

void QOpcUaGdsClientPrivate::registerApplication()
{
    if (!m_appRecord.applicationId().isEmpty() && m_appRecord.applicationId() != QLatin1String("ns=0;i=0"))
        return;

    if (!m_client || m_client->state() != QOpcUaClient::Connected) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No connection";
        return;
    }

    if (!m_directoryNode) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No directory node";
        setError(QOpcUaGdsClient::Error::FailedToRegisterApplication);
        return;
    }

    QByteArray buffer;
    QOpcUaBinaryDataEncoding encoder(&buffer);
    if (!encoder.encode(m_appRecord)) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to encode body";
        setError(QOpcUaGdsClient::Error::FailedToRegisterApplication);
        return;
    }

    QOpcUaExtensionObject parameter;
    parameter.setBinaryEncodedBody(buffer, QOpcUa::nodeIdFromInteger(m_gdsNamespaceIndex, ApplicationRecordDataType_Encoding_DefaultBinary));

    if (!m_directoryNode->callMethod(m_directoryNodes[QLatin1String("RegisterApplication")],
                                     QList<QOpcUa::TypedVariant> { QOpcUa::TypedVariant(parameter, QOpcUa::ExtensionObject) })) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to call method RegisterApplication";
        setError(QOpcUaGdsClient::Error::FailedToRegisterApplication);
    }
}

void QOpcUaGdsClientPrivate::handleRegisterApplicationFinished(const QVariant &result, QOpcUa::UaStatusCode statusCode)
{
    if (statusCode != QOpcUa::Good) {
        qCDebug(QT_OPCUA_GDSCLIENT) << "Requesting user credentials";
        if (statusCode == QOpcUa::BadUserAccessDenied && m_client->authenticationInformation().authenticationType() == QOpcUaUserTokenPolicy::Anonymous) {
            restartWithCredentials();
            return;
        }
        qCWarning(QT_OPCUA_GDSCLIENT) << "Register application failed with" << statusCode;
        setError(QOpcUaGdsClient::Error::FailedToRegisterApplication);
        return;
    }

    m_appRecord.setApplicationId(result.toString());

    QSettings settings;
    settings.setValue(QLatin1String("gds/applicationId"), m_appRecord.applicationId());
    settings.sync();

    qCInfo(QT_OPCUA_GDSCLIENT) << "Registered application with id" << m_appRecord.applicationId();
    getCertificateGroups();
}

void QOpcUaGdsClientPrivate::getCertificateGroups()
{
    // OPC UA Specification Version 1.04 Part 4 Chapter 7.6.6 GetCertificateGroups

    if (!m_client || m_client->state() != QOpcUaClient::Connected) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No connection";
        return;
    }

    if (!m_directoryNode) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No directory node";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
        return;
    }

    QList<QOpcUa::TypedVariant> arguments;
    arguments.push_back(QOpcUa::TypedVariant(m_appRecord.applicationId(), QOpcUa::NodeId));

    if (!m_directoryNode->callMethod(m_directoryNodes[QLatin1String("GetCertificateGroups")], arguments)) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to call GetCertificateGroups";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
    }
}

void QOpcUaGdsClientPrivate::handleGetCertificateGroupsFinished(const QVariant &result, QOpcUa::UaStatusCode statusCode)
{
    Q_Q(QOpcUaGdsClient);

    if (statusCode != QOpcUa::Good) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Getting certificate groups failed" << statusCode;
        setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
        return;
    }

    m_certificateGroups = result.value<QStringList>();
    qCDebug(QT_OPCUA_GDSCLIENT) << "Certificate groups:" << m_certificateGroups;

    emit q->certificateGroupsReceived(m_certificateGroups);
    resolveCertificateTypes();
}

void QOpcUaGdsClientPrivate::resolveCertificateTypes()
{
    if (!m_client || m_client->state() != QOpcUaClient::Connected) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No connection";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
        return;
    }

    if (m_certificateGroups.isEmpty()) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Certificate groups is empty";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
        return;
    }

    delete m_certificateGroupNode;
    m_certificateGroupNode = m_client->node(m_certificateGroups[0]);

    if (!m_certificateGroupNode) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Certificate node failed";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
        return;
    }

    QObject::connect(m_certificateGroupNode, &QOpcUaNode::resolveBrowsePathFinished, [this](QList<QOpcUaBrowsePathTarget> targets,
                                                                 QList<QOpcUaRelativePathElement> path,
                                                                 QOpcUa::UaStatusCode statusCode) {
        if (path.size() != 1) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Invalid path size";
            setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
            return;
        }

        if (path[0].targetName().name() != QLatin1String("CertificateTypes")) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Invalid resolve name" << path[0].targetName().name();
            setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
            return;
        }

        if (statusCode != QOpcUa::Good) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Resolving certificate types failed" << statusCode;
            setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
            return;
        }

        if (targets.size() != 1) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Invalid number of results";
            setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
            return;
        }

        if (!targets[0].isFullyResolved()) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Certificate types not fully resolved";
            setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
            return;
        }

        m_certificateTypesNodeId = targets[0].targetId().nodeId();
        this->getCertificateTypes();
    });

    QOpcUaRelativePathElement pathElement(QOpcUaQualifiedName(0, QLatin1String("CertificateTypes")),
                                           QOpcUa::ReferenceTypeId::Unspecified);
    QList<QOpcUaRelativePathElement> browsePath { pathElement };


    if (!m_certificateGroupNode->resolveBrowsePath(browsePath)) {
            qCWarning(QT_OPCUA_GDSCLIENT) << "Could not resolve certificate type";
            setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
            return;
    }
}

void QOpcUaGdsClientPrivate::getCertificateTypes()
{
    if (!m_client || m_client->state() != QOpcUaClient::Connected) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No connection";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
        return;
    }

    if (m_certificateTypesNodeId.isEmpty()) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Certificate types node id is empty";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
        return;
    }

    delete m_certificateTypesNode;
    m_certificateTypesNode = m_client->node(m_certificateTypesNodeId);

    if (!m_certificateTypesNode) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Certificate types node failed";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
        return;
    }

    QObject::connect(m_certificateTypesNode, &QOpcUaNode::attributeUpdated, [this](QOpcUa::NodeAttribute attr, QVariant value) {
        if (attr == QOpcUa::NodeAttribute::Value)
            qCWarning(QT_OPCUA_GDSCLIENT) << "possible certificate types" << value;
        registrationDone();
    });

    if (!m_certificateTypesNode->readValueAttribute()) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Could not read certificate types";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
        return;
    }
}

void QOpcUaGdsClientPrivate::registrationDone()
{
    Q_Q(QOpcUaGdsClient);

    setState(QOpcUaGdsClient::State::ApplicationRegistered);
    emit q->applicationRegistered();

    m_certificateCheckTimer->start();
    _q_certificateCheckTimeout(); // Force a check now

    m_trustListUpdateTimer->start();
    _q_updateTrustList(); // Force a check now
}

void QOpcUaGdsClientPrivate::getCertificateStatus()
{
    // OPC UA Specification Version 1.04 Part 4 Chapter 7.6.8 GetCertificateStatus

    if (!m_client || m_client->state() != QOpcUaClient::Connected) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No connection";
        return;
    }

    if (!m_directoryNode) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No directory node";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
        return;
    }

    if (m_certificateGroups.isEmpty()) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No certificate groups received";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
        return;
    }

    QString certificateType = QLatin1String("ns=0;i=0"); // null node

    QList<QOpcUa::TypedVariant> arguments;
    arguments.push_back(QOpcUa::TypedVariant(m_appRecord.applicationId(), QOpcUa::NodeId));
    // Let the server choose the certificate group id
    arguments.push_back(QOpcUa::TypedVariant(m_certificateGroups[0], QOpcUa::NodeId));
    // Let the server choose the certificate type id
    arguments.push_back(QOpcUa::TypedVariant(certificateType, QOpcUa::NodeId));

    if (!m_directoryNode->callMethod(m_directoryNodes[QLatin1String("GetCertificateStatus")], arguments)) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to call GetCertificateStatus";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
    }
}

void QOpcUaGdsClientPrivate::handleGetCertificateStatusFinished(const QVariant &result, QOpcUa::UaStatusCode statusCode)
{
    Q_Q(QOpcUaGdsClient);

    if (statusCode != QOpcUa::Good) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Getting certificate status failed" << statusCode;
        setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
        return;
    }

    if (result.toBool()) {
        qInfo(QT_OPCUA_GDSCLIENT) << "Certificate needs update";
        emit q->certificateUpdateRequired();
        startCertificateRequest();
    }
}

void QOpcUaGdsClientPrivate::startCertificateRequest()
{
    // OPC UA Specification Version 1.04 Part 4 Chapter 7.6.3 StartSigningRequest

    if (!m_client || m_client->state() != QOpcUaClient::Connected) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No connection";
        return;
    }

    if (!m_directoryNode) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No directory node";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
        return;
    }

    QOpcUaKeyPair keyPair;
    QFile keyFile(m_pkiConfig.privateKeyFile());

    qCDebug(QT_OPCUA_GDSCLIENT) << "Using private key" << keyFile.fileName();

    if (!keyFile.open(QFile::ReadOnly)) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to open private key file" << keyFile.fileName() << "for reading:" << keyFile.errorString();
        setError(QOpcUaGdsClient::Error::ConnectionError);
        return;
    }

    auto data = keyFile.readAll();
    keyFile.close();

    if (!keyPair.loadFromPemData(data)) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to load private key";
        setError(QOpcUaGdsClient::Error::ConnectionError);
        return;
    }

    if (!keyPair.hasPrivateKey()) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Private key does not contain a private key";
        setError(QOpcUaGdsClient::Error::ConnectionError);
        return;
    }

    auto csr = createSigningRequest();
    csr.setEncoding(QOpcUaX509CertificateSigningRequest::Encoding::DER);
    const auto csrData = csr.createRequest(keyPair);


    QList<QOpcUa::TypedVariant> arguments;
    arguments.push_back(QOpcUa::TypedVariant(m_appRecord.applicationId(), QOpcUa::NodeId));
    // Let the server choose the certificate group id
    arguments.push_back(QOpcUa::TypedVariant(QLatin1String("ns=0;i=0"), QOpcUa::NodeId));
    // Let the server choose the certificate type id
    arguments.push_back(QOpcUa::TypedVariant(QLatin1String("ns=0;i=0"), QOpcUa::NodeId));

    arguments.push_back(QOpcUa::TypedVariant(csrData, QOpcUa::ByteString));

    if (!m_directoryNode->callMethod(m_directoryNodes[QLatin1String("StartSigningRequest")], arguments)) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to call StartSigningRequest";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificate);
    }
}

void QOpcUaGdsClientPrivate::handleStartSigningRequestFinished(const QVariant &result, QOpcUa::UaStatusCode statusCode)
{
    if (statusCode != QOpcUa::Good) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Getting certificate failed" << statusCode;
        setError(QOpcUaGdsClient::Error::FailedToGetCertificate);
        return;
    }

    m_certificateRequestId = result.toString();

    if (!m_certificateFinishTimer)
        m_certificateFinishTimer = new QTimer;

    m_certificateFinishTimer->setInterval(2000);
    m_certificateFinishTimer->setSingleShot(true);
    QObject::connect(m_certificateFinishTimer, &QTimer::timeout, [this]() {
        this->finishCertificateRequest();
    });

    m_certificateFinishTimer->start();
}

void QOpcUaGdsClientPrivate::finishCertificateRequest()
{
    // OPC UA Specification Version 1.04 Part 4 Chapter 7.6.5 FinishRequest

    if (!m_client || m_client->state() != QOpcUaClient::Connected) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No connection";
        return;
    }

    if (m_certificateRequestId.isEmpty()) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No certificate request id";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
        return;
    }

    QList<QOpcUa::TypedVariant> arguments;
    arguments.push_back(QOpcUa::TypedVariant(m_appRecord.applicationId(), QOpcUa::NodeId));
    arguments.push_back(QOpcUa::TypedVariant(m_certificateRequestId, QOpcUa::NodeId));

    if (!m_directoryNode->callMethod(m_directoryNodes[QLatin1String("FinishRequest")], arguments)) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to call FinishRequest";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificate);
    }
}

void QOpcUaGdsClientPrivate::handleFinishRequestFinished(const QVariant &result, QOpcUa::UaStatusCode statusCode)
{
    Q_Q(QOpcUaGdsClient);

    if (statusCode == QOpcUa::BadNothingToDo) {
        // Server not finished yet: Try again later
        m_certificateFinishTimer->setInterval(m_certificateFinishTimer->interval() * 2);
        m_certificateFinishTimer->start();
        qCWarning(QT_OPCUA_GDSCLIENT) << "Server not finished yet: Trying again in" << m_certificateFinishTimer->interval() / 1000 << "s";
        return;
    }
    if (statusCode != QOpcUa::Good) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Getting certificate failed" << statusCode;
        setError(QOpcUaGdsClient::Error::FailedToGetCertificate);
        return;
    }

    m_certificateRequestId.clear();
    const auto resultList = result.toList();

    if (resultList.size() != 3) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Expected list of 3 results but got" << resultList.size();
        setError(QOpcUaGdsClient::Error::FailedToGetCertificate);
        return;
    }

    const auto certificate = resultList[0].toByteArray();
    //const auto privateKey = resultList[1].toByteArray();
    const auto issuerList = resultList[2].toByteArray();

    if (certificate.isEmpty() || issuerList.isEmpty()) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Certificates are empty";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificate);
        return;
    }

    qCInfo(QT_OPCUA_GDSCLIENT) << "Received new certificate" << certificate;

    QFile certificateFile(m_pkiConfig.clientCertificateFile());
    if (!certificateFile.open(QFile::WriteOnly)) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Could not store certificate";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificate);
        return;
    }

    if (certificateFile.write(certificate) != certificate.size()) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Could not store certificate data";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificate);
        return;
    }

    certificateFile.close();

    // FIMXE: How to store this?
    QTemporaryFile issuerFile(m_pkiConfig.issuerListDirectory() + QLatin1String("/XXXXXX.der"));
    if (!issuerFile.open()) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Could not store issuer certificates to" << m_pkiConfig.issuerListDirectory();
        setError(QOpcUaGdsClient::Error::FailedToGetCertificate);
        return;
    }

    if (issuerFile.write(issuerList) != issuerList.size()) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Could not store certificate data";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificate);
        return;
    }

    qCDebug(QT_OPCUA_GDSCLIENT) << "issuer list stored to" << issuerFile.fileName();
    issuerFile.close();
    issuerFile.setAutoRemove(false);

    emit q->certificateUpdated();
}

void QOpcUaGdsClientPrivate::unregisterApplication()
{
    Q_Q(QOpcUaGdsClient);

    if (m_appRecord.applicationId().isEmpty() || m_appRecord.applicationId() == QLatin1String("ns=0;i=0")) {
        emit q->unregistered();
        return;
    }

    if (!m_client || m_client->state() != QOpcUaClient::Connected) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No connection";
        setError(QOpcUaGdsClient::Error::FailedToUnregisterApplication);
        return;
    }

    if (!m_directoryNode) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No directory node";
        setError(QOpcUaGdsClient::Error::FailedToUnregisterApplication);
        return;
    }

    if (!m_directoryNode->callMethod(m_directoryNodes[QLatin1String("UnregisterApplication")],
                                     QList<QOpcUa::TypedVariant> { QOpcUa::TypedVariant(m_appRecord.applicationId(), QOpcUa::NodeId) })) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to call method UnregisterApplication";
        setError(QOpcUaGdsClient::Error::FailedToUnregisterApplication);
    }
}

void QOpcUaGdsClientPrivate::handleUnregisterApplicationFinished(const QVariant &result, QOpcUa::UaStatusCode statusCode)
{
    Q_Q(QOpcUaGdsClient);
    Q_UNUSED(result);

    if (statusCode != QOpcUa::Good) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Unregister application failed" << statusCode;
        setError(QOpcUaGdsClient::Error::FailedToUnregisterApplication);
        return;
    }

    m_client->disconnectFromEndpoint();
    emit q->unregistered();
}

void QOpcUaGdsClientPrivate::setPkiConfiguration(const QOpcUaPkiConfiguration &pkiConfig)
{
    m_pkiConfig = pkiConfig;
}

const QOpcUaPkiConfiguration &QOpcUaGdsClientPrivate::pkiConfiguration() const
{
    return m_pkiConfig;
}

void QOpcUaGdsClientPrivate::setApplicationIdentity(const QOpcUaApplicationIdentity &appIdentity)
{
    m_appIdentitiy = appIdentity;
}

const QOpcUaApplicationIdentity &QOpcUaGdsClientPrivate::applicationIdentity() const
{
    return m_appIdentitiy;
}

void QOpcUaGdsClientPrivate::setApplicationRecord(const QOpcUaApplicationRecordDataType &appRecord)
{
    m_appRecord = appRecord;
}

const QOpcUaApplicationRecordDataType &QOpcUaGdsClientPrivate::applicationRecord() const
{
    return m_appRecord;
}

void QOpcUaGdsClientPrivate::setCertificateSigningRequestPresets(const QOpcUaX509DistinguishedName &dn, const QString &dns)
{
    csrPresets.dn = dn;
    csrPresets.dns = dns;
}

const QOpcUaX509DistinguishedName &QOpcUaGdsClientPrivate::distinguishedNameCertificateSigningRequestPreset() const
{
    return csrPresets.dn;
}

const QString &QOpcUaGdsClientPrivate::dnsCertificateSigningRequestPreset() const
{
    return csrPresets.dns;
}

void QOpcUaGdsClientPrivate::setCertificateCheckInterval(int interval)
{
    m_certificateCheckTimer->setInterval(interval);
}

int QOpcUaGdsClientPrivate::certificateCheckInterval() const
{
    return m_certificateCheckTimer->interval();
}

void QOpcUaGdsClientPrivate::setTrustListUpdateInterval(int interval)
{
    m_trustListUpdateTimer->setInterval(interval);
}

int QOpcUaGdsClientPrivate::trustListUpdateInterval() const
{
    return m_trustListUpdateTimer->interval();
}

QOpcUaGdsClient::Error QOpcUaGdsClientPrivate::error() const
{
    return m_error;
}

QOpcUaGdsClient::State QOpcUaGdsClientPrivate::state() const
{
    return m_state;
}

QOpcUaX509CertificateSigningRequest QOpcUaGdsClientPrivate::createSigningRequest() const
{
    QOpcUaX509CertificateSigningRequest csr;
    QOpcUaX509DistinguishedName dn = csrPresets.dn;
    // Overwrite the application name because it has to match the identity
    dn.setEntry(QOpcUaX509DistinguishedName::Type::CommonName, m_appIdentitiy.applicationName());
    csr.setSubject(dn);

    QOpcUaX509ExtensionSubjectAlternativeName *san = new QOpcUaX509ExtensionSubjectAlternativeName;
    san->addEntry(QOpcUaX509ExtensionSubjectAlternativeName::Type::URI, m_appIdentitiy.applicationUri());
    if (!csrPresets.dns.isEmpty())
        san->addEntry(QOpcUaX509ExtensionSubjectAlternativeName::Type::DNS, csrPresets.dns);
    san->setCritical(true);
    csr.addExtension(san);

    QOpcUaX509ExtensionBasicConstraints *bc = new QOpcUaX509ExtensionBasicConstraints;
    bc->setCa(false);
    bc->setCritical(true);
    csr.addExtension(bc);

    QOpcUaX509ExtensionKeyUsage *ku = new QOpcUaX509ExtensionKeyUsage;
    ku->setCritical(true);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::DigitalSignature);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::NonRepudiation);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::KeyEncipherment);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::DataEncipherment);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::CertificateSigning);
    csr.addExtension(ku);

    return csr;
}

void QOpcUaGdsClientPrivate::_q_handleDirectoryNodeMethodCallFinished(QString methodNodeId, QVariant result, QOpcUa::UaStatusCode statusCode)
{
    if (methodNodeId == m_directoryNodes[QLatin1String("UnregisterApplication")]) {
        this->handleUnregisterApplicationFinished(result, statusCode);
    } else if ( methodNodeId == m_directoryNodes[QLatin1String("FinishRequest")]) {
        this->handleFinishRequestFinished(result, statusCode);
    } else if (methodNodeId == m_directoryNodes[QLatin1String("StartSigningRequest")]) {
        this->handleStartSigningRequestFinished(result, statusCode);
    } else if (methodNodeId == m_directoryNodes[QLatin1String("GetCertificateStatus")]) {
        this->handleGetCertificateStatusFinished(result, statusCode);
    } else if (methodNodeId == m_directoryNodes[QLatin1String("GetCertificateGroups")]) {
        this->handleGetCertificateGroupsFinished(result, statusCode);
    } else if (methodNodeId == m_directoryNodes[QLatin1String("RegisterApplication")]) {
        this->handleRegisterApplicationFinished(result, statusCode);
    } else if (methodNodeId == m_directoryNodes[QLatin1String("FindApplications")]) {
        this->handleFindApplicationsFinished(result, statusCode);
    } else if (methodNodeId == m_directoryNodes[QLatin1String("GetApplication")]) {
        this->handleGetApplicationFinished(result, statusCode);
    } else if (methodNodeId == m_directoryNodes[QLatin1String("GetTrustList")]) {
        this->handleGetTrustListFinished(result, statusCode);
    } else {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Result unexpeced method call received" << methodNodeId;
    }
}

void QOpcUaGdsClientPrivate::_q_certificateCheckTimeout()
{
    Q_Q(QOpcUaGdsClient);

    QFile file(m_pkiConfig.clientCertificateFile());
    if (!file.open(QFile::ReadOnly)) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to load certificate from file" << m_pkiConfig.clientCertificateFile();
    }

    QSslCertificate cert(&file, QSsl::Der);
    if (cert.isNull()) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to load certificate from file" << m_pkiConfig.clientCertificateFile();
        return;
    }

    bool needsUpdate = false;

    if (cert.isSelfSigned()) {
        needsUpdate = true;
        qCInfo(QT_OPCUA_GDSCLIENT) << "Certificate is self-signed: requesting update";
    }

    if (QDateTime::currentDateTime().addSecs(24*60*60) > cert.expiryDate()) {
        needsUpdate = true;
        qCInfo(QT_OPCUA_GDSCLIENT) << "Certificate expiry date is due: requesting update";
    }

    if (!needsUpdate)
        getCertificateStatus();

    if (needsUpdate) {
        emit q->certificateUpdateRequired();
        startCertificateRequest();
    }
}

void QOpcUaGdsClientPrivate::_q_updateTrustList()
{
    // OPC UA Specification Version 1.04 Part 4 Chapter 7.6.7 GetTrustList

    if (!m_client || m_client->state() != QOpcUaClient::Connected) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No connection";
        return;
    }

    if (!m_certificateGroupNode) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "No certificate group node";
        return;
    }

    QList<QOpcUa::TypedVariant> arguments;
    arguments.push_back(QOpcUa::TypedVariant(m_appRecord.applicationId(), QOpcUa::NodeId));
    arguments.push_back(QOpcUa::TypedVariant(m_certificateGroupNode->nodeId(), QOpcUa::NodeId));

    if (!m_directoryNode->callMethod(m_directoryNodes[QLatin1String("GetTrustList")], arguments)) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Failed to call GetTrustList";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificate);
    }
}

void QOpcUaGdsClientPrivate::handleGetTrustListFinished(const QVariant &result, QOpcUa::UaStatusCode statusCode)
{
    Q_Q(QOpcUaGdsClient);

    if (statusCode != QOpcUa::Good) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Getting trust list node failed" << statusCode;
        setError(QOpcUaGdsClient::Error::FailedToGetCertificate);
        return;
    }

    const auto trustListNodeId = result.toString();

    if (trustListNodeId.isEmpty()) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Trust list node id is empty";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificate);
        return;
    }

    delete m_trustListNode;
    m_trustListNode = m_client->node(trustListNodeId);

    if (!m_trustListNode) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Trust list node failed";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
        return;
    }

    QObject::connect(m_trustListNode, &QOpcUaNode::attributeUpdated, [q](QOpcUa::NodeAttribute attr, QVariant value) {
        Q_UNUSED(value);
        if (attr == QOpcUa::NodeAttribute::Value)
            emit q->trustListUpdated();
    });

    if (!m_trustListNode->readValueAttribute()) {
        qCWarning(QT_OPCUA_GDSCLIENT) << "Could not read trust list";
        setError(QOpcUaGdsClient::Error::FailedToGetCertificateStatus);
        return;
    }
}

// When it is detected that authentication credentials are required this function
// is used to re-connect to the server after requesting credentials
void QOpcUaGdsClientPrivate::restartWithCredentials()
{
    Q_Q(QOpcUaGdsClient);

    QOpcUaAuthenticationInformation authInfo;
    emit q->authenticationRequired(authInfo);
    m_client->setAuthenticationInformation(authInfo);

    qCInfo(QT_OPCUA_GDSCLIENT) << "Restarting connection with credentials";
    m_restartRequired = true;
    m_client->disconnectFromEndpoint();
}

QT_END_NAMESPACE

#include "moc_qopcuagdsclient.cpp"

// Copyright (C) 2018 Unified Automation GmbH
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuapkiconfiguration.h"
#include "qopcuaapplicationidentity.h"

#include <QLoggingCategory>
#include <QSslCertificate>
#include <QSslCertificateExtension>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_SECURITY);

/*!
    \class QOpcUaPkiConfiguration
    \inmodule QtOpcUa
    \brief QOpcUaPkiConfiguration defines the PKI configuration of the application.
    \since QtOpcUa 5.13

    This info must be configured using QOpcUaClient::setPkiConfiguration.
    The used paths and files must be created beforehand.

    \code
    QOpcUaPkiConfiguration pkiConfig;
    const QString pkiDir = QCoreApplication::applicationDirPath() + "/pki";

    pkiConfig.setClientCertificateFile(pkiDir + "/own/certs/application.der");
    pkiConfig.setPrivateKeyFile(pkiDir + "/own/private/application.pem");
    pkiConfig.setTrustListDirectory(pkiDir + "/trusted/certs");
    pkiConfig.setRevocationListDirectory(pkiDir + "/trusted/crl");
    pkiConfig.setIssuerListDirectory(pkiDir + "/issuers/certs");
    pkiConfig.setIssuerRevocationListDirectory(pkiDir + "/issuers/crl");

    client->setPkiConfiguration(pkiConfig);
    \endcode
*/

class QOpcUaPkiConfigurationData : public QSharedData
{
public:
    QOpcUaPkiConfigurationData() {}

    QString m_clientCertificateFile;    /**< Own application certificate filename */
    QString m_privateKeyFile;           /**< Private key filename which belongs to m_certificateFile */
    QString m_trustListDirectory;            /**< Path to trust list directory */
    QString m_revocationListDirectory;       /**< Folder containing  certificate revocation list */
    QString m_issuerListDirectory;           /**< Folder containing issuer intermediate certificates (untrusted) */
    QString m_issuerRevocationListDirectory; /**< Folder containing issuer revocation list */
};

/*!
    Default constructs a PKI configuration with no parameters set.
*/
QOpcUaPkiConfiguration::QOpcUaPkiConfiguration()
    : data(new QOpcUaPkiConfigurationData())
{}

QOpcUaPkiConfiguration::~QOpcUaPkiConfiguration()
{}

/*!
    Constructs a \l QOpcUaPkiConfiguration from \a other.
*/
QOpcUaPkiConfiguration::QOpcUaPkiConfiguration(const QOpcUaPkiConfiguration &other)
    : data(other.data)
{}

/*!
    Sets the values of \a rhs in this PKI configuration.
*/
QOpcUaPkiConfiguration &QOpcUaPkiConfiguration::operator=(const QOpcUaPkiConfiguration &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

/*!
    Returns the file path of the application's client certificate.
 */
QString QOpcUaPkiConfiguration::clientCertificateFile() const
{
    return data->m_clientCertificateFile;
}

/*!
    Sets the file path of the application's client certificate to \a value.

    This file has to be in X509 DER format.
*/
void QOpcUaPkiConfiguration::setClientCertificateFile(const QString &value)
{
    data->m_clientCertificateFile = value;
}

/*!
    Returns the file path of the application's private key.
*/
QString QOpcUaPkiConfiguration::privateKeyFile() const
{
    return data->m_privateKeyFile;
}

/*!
    Sets the file path of the application's private key to \a value.

    This file has to be in X509 PEM format.
*/
void QOpcUaPkiConfiguration::setPrivateKeyFile(const QString &value)
{
    data->m_privateKeyFile = value;
}

/*!
    Returns the folder of the certificate trust list.
*/
QString QOpcUaPkiConfiguration::trustListDirectory() const
{
    return data->m_trustListDirectory;
}

/*!
    Sets the path of the certificate trust list directory to \a value.

    All certificates in this directory will be trusted.
    Certificates have to be in X509 DER format.
*/
void QOpcUaPkiConfiguration::setTrustListDirectory(const QString &value)
{
    data->m_trustListDirectory = value;
}

/*!
    Returns the path of the certificate revocation list directory.
*/
QString QOpcUaPkiConfiguration::revocationListDirectory() const
{
    return data->m_revocationListDirectory;
}

/*!
    Sets the path of the certificate revocation list directory to \a value.
*/
void QOpcUaPkiConfiguration::setRevocationListDirectory(const QString &value)
{
    data->m_revocationListDirectory = value;
}

/*!
    Returns the path of the intermediate issuer list directory.

    These issuers will not be trusted.
*/
QString QOpcUaPkiConfiguration::issuerListDirectory() const
{
    return data->m_issuerListDirectory;
}

/*!
    Sets the path of the intermediate issuer list directory to \a value.
*/
void QOpcUaPkiConfiguration::setIssuerListDirectory(const QString &value)
{
    data->m_issuerListDirectory = value;
}

/*!
    Returns the path of the intermediate issuer revocation list directory.
*/
QString QOpcUaPkiConfiguration::issuerRevocationListDirectory() const
{
    return data->m_issuerRevocationListDirectory;
}

/*!
    Sets the path of the intermediate issuer revocation list directory to \a value.
*/
void QOpcUaPkiConfiguration::setIssuerRevocationListDirectory(const QString &value)
{
    data->m_issuerRevocationListDirectory = value;
}

/*!
    Returns an application identity based on the application's client certificate.

    The application's identity has to match the used certificate. The returned application
    identity is prefilled by using information of the configured client certificate.
*/
QOpcUaApplicationIdentity QOpcUaPkiConfiguration::applicationIdentity() const
{
    QOpcUaApplicationIdentity identity;

    auto certList = QSslCertificate::fromPath(clientCertificateFile(), QSsl::Der);
    if (certList.isEmpty()) {
        qCWarning(QT_OPCUA_SECURITY) << "No client certificate found at" << clientCertificateFile()
                                     << ". Application identity will be invalid.";
        return QOpcUaApplicationIdentity();
    }

    auto extensions = certList[0].extensions();
    for (const auto &extension : std::as_const(extensions)) {
        if (extension.name() == QLatin1String("subjectAltName")) { // OID: 2.5.29.17
            const auto value = extension.value().toMap();
            // const QString dns = value[QLatin1String("DNS")].toString();
            const QString uri = value[QLatin1String("URI")].toString();

            const auto token = uri.split(QChar::fromLatin1(':'), Qt::SkipEmptyParts);

            if (token.size() != 4) {
                qCWarning(QT_OPCUA_SECURITY) << "URI string from certificate has unexpected format:"
                                             << uri << "Application identity will be invalid.";
                return QOpcUaApplicationIdentity();
            }

            identity.setApplicationUri(uri);
            identity.setApplicationName(token.at(3));
            identity.setProductUri(QStringLiteral("%1:%2").arg(token.at(2), token.at(3)));
        }
    }
    return identity;
}

/*!
    Return true if the public key information required to validate the server certificate
    is set.
*/
bool QOpcUaPkiConfiguration::isPkiValid() const
{
    return !issuerListDirectory().isEmpty() &&
           !issuerRevocationListDirectory().isEmpty() &&
           !revocationListDirectory().isEmpty() &&
            !trustListDirectory().isEmpty();
}

/*!
    Returns true if the private key file and client certificate file are set.
*/
bool QOpcUaPkiConfiguration::isKeyAndCertificateFileSet() const
{
    return !clientCertificateFile().isEmpty() &&
           !privateKeyFile().isEmpty();
}

QT_END_NAMESPACE

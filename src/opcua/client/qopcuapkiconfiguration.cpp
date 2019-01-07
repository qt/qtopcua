/****************************************************************************
**
** Copyright (C) 2018 Unified Automation GmbH
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

    This class is currently available as a Technology Preview, and therefore the API
    and functionality provided by the class may be subject to change at any time without
    prior notice.

    This info must be configured using QOpcUaClient::setPkiConfiguration.
    The used paths and files must be created beforehand.

    \code
    QOpcUaPkiConfiguration pkiConfig;
    const QString pkiDir = QCoreApplication::applicationDirPath() + "/pki";

    pkiConfig.setClientCertificateLocation(pkidir + "/own/certs/application.der");
    pkiConfig.setPrivateKeyLocation(pkidir + "/own/private/application.pem");
    pkiConfig.setTrustListLocation(pkidir + "/trusted/certs");
    pkiConfig.setRevocationListLocation(pkidir + "/trusted/crl");
    pkiConfig.setIssuerListLocation(pkidir + "/issuers/certs");
    pkiConfig.setIssuerRevocationListLocation(pkidir + "/issuers/crl");

    client->setPkiConfiguration(pkiConfig);
    \endcode
*/

class QOpcUaPkiConfigurationData : public QSharedData
{
public:
    QOpcUaPkiConfigurationData() {}

    QString m_clientCertificateLocation;    /**< Own application certificate filename */
    QString m_privateKeyLocation;           /**< Private key filename which belongs to m_certificateLocation */
    QString m_trustListLocation;            /**< Path to trust list location */
    QString m_revocationListLocation;       /**< Path to certificate revocation list */
    QString m_issuerListLocation;           /**< Path to issuer intermediate certficates (untrusted) */
    QString m_issuerRevocationListLocation; /**< Path to issure revocation list */
};

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
QString QOpcUaPkiConfiguration::clientCertificateLocation() const
{
    return data->m_clientCertificateLocation;
}

/*!
    Sets the file path of the application's client certificate to \a value.

    This file has to be in X509 DER format.
*/
void QOpcUaPkiConfiguration::setClientCertificateLocation(const QString &value)
{
    data->m_clientCertificateLocation = value;
}

/*!
    Returns the file path of the application's private key.
*/
QString QOpcUaPkiConfiguration::privateKeyLocation() const
{
    return data->m_privateKeyLocation;
}

/*!
    Sets the file path of the application's private key to \a value.

    This file has to be in X509 PEM format.
*/
void QOpcUaPkiConfiguration::setPrivateKeyLocation(const QString &value)
{
    data->m_privateKeyLocation = value;
}

/*!
    Returns the path of the certificate trust list directory.
*/
QString QOpcUaPkiConfiguration::trustListLocation() const
{
    return data->m_trustListLocation;
}

/*!
    Sets the path of the certificate trust list directory to \a value.

    All certificates in this directory will be trusted.
    Certificates have to be in X509 DER format.
*/
void QOpcUaPkiConfiguration::setTrustListLocation(const QString &value)
{
    data->m_trustListLocation = value;
}

/*!
    Returns the path of the certificate revocation list directory.
*/
QString QOpcUaPkiConfiguration::revocationListLocation() const
{
    return data->m_revocationListLocation;
}

/*!
    Sets the path of the certificate revocation list directory to \a value.
*/
void QOpcUaPkiConfiguration::setRevocationListLocation(const QString &value)
{
    data->m_revocationListLocation = value;
}

/*!
    Returns the path of the intermediate issuer list directory.

    These issuers will not be trusted.
*/
QString QOpcUaPkiConfiguration::issuerListLocation() const
{
    return data->m_issuerListLocation;
}

/*!
    Sets the path of the intermediate issuer list directory to \a value.
*/
void QOpcUaPkiConfiguration::setIssuerListLocation(const QString &value)
{
    data->m_issuerListLocation = value;
}

/*!
    Returns the path of the intermediate issuer revocation list directory.
*/
QString QOpcUaPkiConfiguration::issuerRevocationListLocation() const
{
    return data->m_issuerRevocationListLocation;
}

/*!
    Sets the path of the intermediate issuer revocation list directory to \a value.
*/
void QOpcUaPkiConfiguration::setIssuerRevocationListLocation(const QString &value)
{
    data->m_issuerRevocationListLocation = value;
}

/*!
    Returns an application identity based on the application's client certificate.

    The application's identity has to match the used certificate. The returned application
    identity is prefilled by using information of the configured client certificate.
*/
QOpcUaApplicationIdentity QOpcUaPkiConfiguration::applicationIdentity() const
{
    QOpcUaApplicationIdentity identity;

    auto certList = QSslCertificate::fromPath(data->m_clientCertificateLocation, QSsl::Der);
    if (certList.isEmpty()) {
        qCWarning(QT_OPCUA_SECURITY) << "No client certificate found at" << data->m_clientCertificateLocation
                                     << ". Application identity will be invalid.";
        return QOpcUaApplicationIdentity();
    }

    auto extensions = certList[0].extensions();
    for (const auto extension : qAsConst(extensions)) {
        if (extension.name() == QLatin1String("subjectAltName")) { // OID: 2.5.29.17
            const auto value = extension.value().toMap();
            // const QString dns = value[QLatin1String("DNS")].toString();
            const QString uri = value[QLatin1String("URI")].toString();

            const auto token = uri.split(':', QString::SkipEmptyParts);

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
    Return true if the public key infrastructure to validate the server certificate
    is set.
*/
bool QOpcUaPkiConfiguration::isPkiValid() const
{
    return !data->m_issuerListLocation.isEmpty() &&
           !data->m_issuerRevocationListLocation.isEmpty() &&
           !data->m_revocationListLocation.isEmpty() &&
           !data->m_trustListLocation.isEmpty();
}

QT_END_NAMESPACE

// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAGDSCLIENT_H
#define QOPCUAGDSCLIENT_H

#include <QtCore/QObject>
#include <QtOpcUa/qopcuaglobal.h>

QT_BEGIN_NAMESPACE

class QOpcUaApplicationIdentity;
class QOpcUaAuthenticationInformation;
class QOpcUaEndpointDescription;
class QOpcUaPkiConfiguration;
class QOpcUaGdsClientPrivate;
class QOpcUaApplicationRecordDataType;
class QOpcUaX509DistinguishedName;

class Q_OPCUA_EXPORT QOpcUaGdsClient : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QOpcUaGdsClient)

public:
    enum class Error {
        NoError,
        InvalidBackend,
        InvalidEndpoint,
        ConnectionError,
        DirectoryNodeNotFound,
        FailedToRegisterApplication,
        FailedToUnregisterApplication,
        FailedToGetCertificateStatus,
        FailedToGetCertificate,
    };
    Q_ENUM(Error)

    enum class State {
        Idle,
        BackendInstantiated,
        Connecting,
        Connected,
        RegisteringApplication,
        ApplicationRegistered,
        //
        Error,
    };
    Q_ENUM(State)

    QOpcUaGdsClient(QObject *parent = nullptr);
    virtual ~QOpcUaGdsClient();

    void setBackend(const QString &backend);
    const QString &backend() const;

    void setEndpoint(const QOpcUaEndpointDescription &endpoint);
    const QOpcUaEndpointDescription &endpoint() const;

    void setPkiConfiguration(const QOpcUaPkiConfiguration &pkiConfig);
    const QOpcUaPkiConfiguration &pkiConfiguration() const;

    void setApplicationIdentity(const QOpcUaApplicationIdentity &appIdentity);
    const QOpcUaApplicationIdentity &applicationIdentity() const;

    void setApplicationRecord(const QOpcUaApplicationRecordDataType &appRecord);
    const QOpcUaApplicationRecordDataType &applicationRecord() const;

    QString applicationId() const;

    void setCertificateSigningRequestPresets(const QOpcUaX509DistinguishedName &dn, const QString &dns);
    const QOpcUaX509DistinguishedName &distinguishedNameCertificateSigningRequestPreset() const;
    const QString &dnsCertificateSigningRequestPreset() const;

    void setCertificateCheckInterval(int interval);
    int certificateCheckInterval() const;

    void setTrustListUpdateInterval(int interval);
    int trustListUpdateInterval() const;

    Error error() const;
    State state() const;

    void start();
    void unregisterApplication();

Q_SIGNALS:
    void stateChanged(State state);
    void errorChanged(Error error);
    void applicationRegistered();
    void certificateGroupsReceived(QStringList certificateGroups);
    void certificateUpdateRequired();
    void certificateUpdated();
    void unregistered();
    void trustListUpdated();
    void authenticationRequired(QOpcUaAuthenticationInformation &authInfo);

private:
    Q_PRIVATE_SLOT(d_func(), void _q_certificateCheckTimeout())
    Q_PRIVATE_SLOT(d_func(), void _q_updateTrustList())
};

QT_END_NAMESPACE

#endif // QOPCUAGDSCLIENT_H

